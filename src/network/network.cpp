#ifdef _WIN64 // WINDOWS
#ifndef _MSC_VER
#include <unistd.h>
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#ifndef WINSOCK2_IMPORTED
#define WINSOCK2_IMPORTED
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <windows.h>
#include <natupnp.h>
#include <iphlpapi.h>
#include <Mstcpip.h>
#include <ip2string.h>
#include "pcp.h"
#else // NIX
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#endif // ...
#include <string_view>
#include "system_state.hpp"
#include "commands.hpp"
#include "SPSCQueue.h"
#include "network.hpp"
#include "serialization.hpp"
#include "gui_error_window.hpp"

#define ZSTD_STATIC_LINKING_ONLY
#define XXH_NAMESPACE ZSTD_
#include "zstd.h"

#ifdef _WIN64
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ntdll.lib")
#endif

namespace network {

//
// platform specific
//

struct local_addresses {
	std::string address;
	bool ipv6 = false;
};

port_forwarder::port_forwarder() { }

void port_forwarder::start_forwarding() {
#ifdef _WIN64
	if(started)
		return;

	internal_wait.lock();
	started = true;
	do_forwarding = std::thread{ [&]() {
		//setup forwarding
		if(!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
			return;

		
		std::vector<local_addresses> found_locals;

		// try to figure out what the computer's local address is
		{
			IP_ADAPTER_ADDRESSES* adapter_addresses(NULL);
			IP_ADAPTER_ADDRESSES* adapter(NULL);

			// Start with a 16 KB buffer and resize if needed -
			// multiple attempts in case interfaces change while
			// we are in the middle of querying them.
			DWORD adapter_addresses_buffer_size = 16 * 1024;
			for(int attempts = 0; attempts != 3; ++attempts) {
				adapter_addresses = (IP_ADAPTER_ADDRESSES*)malloc(adapter_addresses_buffer_size);
				assert(adapter_addresses);

				DWORD error = GetAdaptersAddresses(
				  AF_UNSPEC,
				  GAA_FLAG_SKIP_ANYCAST |
				    GAA_FLAG_SKIP_MULTICAST |
				    GAA_FLAG_SKIP_DNS_SERVER |
				    GAA_FLAG_SKIP_FRIENDLY_NAME,
				  NULL,
				  adapter_addresses,
				  &adapter_addresses_buffer_size);

				if(ERROR_SUCCESS == error) {
					// We're done here, people!
					break;
				} else if(ERROR_BUFFER_OVERFLOW == error) {
					// Try again with the new size
					free(adapter_addresses);
					adapter_addresses = NULL;

					continue;
				} else {
					// Unexpected error code - log and throw
					free(adapter_addresses);
					adapter_addresses = NULL;

					// @todo
				}
			}

			// Iterate through all of the adapters
			for(adapter = adapter_addresses; NULL != adapter; adapter = adapter->Next) {
				// Skip loopback adapters
				if(IF_TYPE_SOFTWARE_LOOPBACK == adapter->IfType) {
					continue;
				}

				// Parse all IPv4 and IPv6 addresses
				for(
				  IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress;
				  NULL != address;
				  address = address->Next) {
					auto family = address->Address.lpSockaddr->sa_family;
					if(address->DadState != NldsPreferred && address->DadState != IpDadStatePreferred)
						continue;

					if(AF_INET == family) {
						// IPv4
						SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(address->Address.lpSockaddr);

						char str_buffer[INET_ADDRSTRLEN] = { 0 };
						inet_ntop(AF_INET, &(ipv4->sin_addr), str_buffer, INET_ADDRSTRLEN);
						found_locals.push_back(local_addresses{ std::string(str_buffer),  false});
					} else if(AF_INET6 == family) {
						// IPv6
						SOCKADDR_IN6* ipv6 = reinterpret_cast<SOCKADDR_IN6*>(address->Address.lpSockaddr);

						char str_buffer[INET6_ADDRSTRLEN] = { 0 };
						inet_ntop(AF_INET6, &(ipv6->sin6_addr), str_buffer, INET6_ADDRSTRLEN);

						std::string ipv6_str(str_buffer);

						// Detect and skip non-external addresses
						bool is_link_local(false);
						bool is_special_use(false);

						if(0 == ipv6_str.find("fe")) {
							char c = ipv6_str[2];
							if(c == '8' || c == '9' || c == 'a' || c == 'b') {
								is_link_local = true;
							}
						} else if(0 == ipv6_str.find("2001:0:")) {
							is_special_use = true;
						}

						if(!(is_link_local || is_special_use)) {
							found_locals.push_back(local_addresses{ std::string(ipv6_str), true });
						}
					} else {
						// Skip all other types of addresses
						continue;
					}
				}
			}

			// Cleanup
			free(adapter_addresses);
			adapter_addresses = NULL;
		}

		// try to add port mapping
		bool mapped_ports_with_upnp = false;
		if(found_locals.size() != 0) {
			IUPnPNAT* nat_interface = nullptr;
			IStaticPortMappingCollection* port_mappings = nullptr;
			IStaticPortMapping* opened_port = nullptr;

			BSTR proto = SysAllocString(L"TCP");
			BSTR desc = SysAllocString(L"Project Alice Host");
			auto tmpwstr = simple_fs::utf8_to_native(found_locals[0].address);
			BSTR local_host = SysAllocString(tmpwstr.c_str());
			VARIANT_BOOL enabled = VARIANT_TRUE;

			if(SUCCEEDED(CoCreateInstance(__uuidof(UPnPNAT), NULL, CLSCTX_ALL, __uuidof(IUPnPNAT), (void**)&nat_interface)) && nat_interface) {
				if(SUCCEEDED(nat_interface->get_StaticPortMappingCollection(&port_mappings)) && port_mappings) {
					if(SUCCEEDED(port_mappings->Add(default_server_port, proto, default_server_port, local_host, enabled, desc, &opened_port)) && opened_port) {
						mapped_ports_with_upnp = true;
					}
				}
			}

			pcp_ctx_t* pcp_obj = nullptr;
			if(!mapped_ports_with_upnp) {
				pcp_obj = pcp_init(ENABLE_AUTODISCOVERY, NULL);

				sockaddr_storage source_ip;
				sockaddr_storage ext_ip;

				WORD wVersionRequested;
				WSADATA wsaData;

				wVersionRequested = MAKEWORD(2, 2);
				auto err = WSAStartup(wVersionRequested, &wsaData);

				memset(&source_ip, 0, sizeof(source_ip));
				memset(&ext_ip, 0, sizeof(ext_ip));

				if(found_locals[0].ipv6 == false) {
					((sockaddr_in*)(&source_ip))->sin_port = 1984;
					((sockaddr_in*)(&source_ip))->sin_addr.s_addr = inet_addr(found_locals[0].address.c_str());
					((sockaddr_in*)(&source_ip))->sin_family = AF_INET;

					((sockaddr_in*)(&ext_ip))->sin_port = 1984;
					((sockaddr_in*)(&ext_ip))->sin_family = AF_INET;
				} else {
					((sockaddr_in6*)(&source_ip))->sin6_port = 1984;
					PCSTR term = nullptr;
					RtlIpv6StringToAddressA(found_locals[0].address.c_str(), &term, &(((sockaddr_in6*)(&source_ip))->sin6_addr));
					((sockaddr_in6*)(&source_ip))->sin6_family = AF_INET6;

					((sockaddr_in6*)(&ext_ip))->sin6_port = 1984;
					((sockaddr_in6*)(&ext_ip))->sin6_family = AF_INET6;
				}

				auto flow = pcp_new_flow(pcp_obj, (sockaddr*)&source_ip, nullptr, (sockaddr*)&ext_ip, IPPROTO_TCP, 3600, nullptr);
				if(flow)
					pcp_wait(flow, 10000, 0);

			}

			// wait for destructor
			internal_wait.lock();

			if(pcp_obj) {
				pcp_terminate(pcp_obj, 0);
			}

			//cleanup forwarding
			if(port_mappings)
				port_mappings->Remove(default_server_port, proto);

			if(opened_port)
				opened_port->Release();
			if(port_mappings)
				port_mappings->Release();
			if(nat_interface)
				nat_interface->Release();

			SysFreeString(proto);
			SysFreeString(local_host);
			SysFreeString(desc);

			internal_wait.unlock();
		}
		CoUninitialize();
	} };
#else
#endif
}

port_forwarder::~port_forwarder() {
#ifdef _WIN64
	if(started) {
		internal_wait.unlock();
		do_forwarding.join();
	}
#else
#endif
}

std::string get_last_error_msg() {
#ifdef _WIN64
	auto err = WSAGetLastError();
	LPTSTR err_buf = nullptr;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, 0, (LPTSTR)&err_buf, 0, nullptr);
	native_string err_text = err_buf;
	LocalFree(err_buf);
	return std::to_string(err) + " = " + simple_fs::native_to_utf8(err_text);
#else
	return std::string("Dummy");
#endif
}

static int internal_socket_recv(socket_t socket_fd, void *data, size_t n) {
#ifdef _WIN64
	u_long has_pending = 0;
	auto r = ioctlsocket(socket_fd, FIONREAD, &has_pending);
	if(has_pending)
		return static_cast<int>(recv(socket_fd, reinterpret_cast<char *>(data), static_cast<int>(n), 0));
	return 0;
#else
	return recv(socket_fd, data, n, MSG_DONTWAIT);
#endif
}

static int internal_socket_send(socket_t socket_fd, const void *data, size_t n) {
#ifdef _WIN64
	return static_cast<int>(send(socket_fd, reinterpret_cast<const char *>(data), static_cast<int>(n), 0));
#else
	return send(socket_fd, data, n, MSG_NOSIGNAL);
#endif
}

template<typename F>
static int socket_recv(socket_t socket_fd, void* data, size_t len, size_t* m, F&& func) {
	while(*m < len) {
		int r = internal_socket_recv(socket_fd, reinterpret_cast<uint8_t*>(data) + *m, len - *m);
		if(r > 0) {
			*m += static_cast<size_t>(r);
		} else if(r < 0) { // error
#ifdef _WIN32
			int err = WSAGetLastError();
			if(err == WSAENOBUFS || err == WSAEWOULDBLOCK) {
				return 0;
			}
			return err;
#else
			return r;
#endif
		} else if(r == 0) {
			break;
		}
	}
	// Did we receive a command?
	if(*m >= len) {
		assert(*m <= len);
		*m = 0; // reset
		func();
	}
	return 0;
}

static int socket_send(socket_t socket_fd, std::vector<char>& buffer) {
	while(!buffer.empty()) {
		int r = internal_socket_send(socket_fd, buffer.data(), buffer.size());
		if(r > 0) {
			buffer.erase(buffer.begin(), buffer.begin() + static_cast<size_t>(r));
		} else if(r < 0) {
#ifdef _WIN32
			int err = WSAGetLastError();
			if(err == WSAENOBUFS || err == WSAEWOULDBLOCK) {
				return 0;
			}
			return err;
#else
			return r;
#endif
		} else if(r == 0) {
			break;
		}
	}
	return 0;
}

static void socket_add_to_send_queue(std::vector<char>& buffer, const void *data, size_t n) {
	buffer.resize(buffer.size() + n);
	std::memcpy(buffer.data() + buffer.size() - n, data, n);
}

static void socket_shutdown(socket_t socket_fd) {
	if(socket_fd > 0) {
#ifdef _WIN64
		shutdown(socket_fd, SD_BOTH);
		closesocket(socket_fd);
#else
		shutdown(socket_fd, SHUT_RDWR);
		close(socket_fd);
#endif
	}
}

static socket_t socket_init_server(bool as_v6, struct sockaddr_storage& server_address) {
	socket_t socket_fd = static_cast<socket_t>(socket(as_v6 ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP));
#ifdef _WIN64
	if(socket_fd == static_cast<socket_t>(INVALID_SOCKET)) {
		window::emit_error_message("Network socket error: " + get_last_error_msg(), true);
	}
#else
	if(socket_fd < 0)
		std::abort();
#endif
	struct timeval timeout;
	timeout.tv_sec = 60;
	timeout.tv_usec = 0;
	int opt = 1;
#ifdef _WIN64
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt))) {
		window::emit_error_message("Network setsockopt [reuseaddr] error: " + get_last_error_msg(), true);
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		window::emit_error_message("Network setsockopt [rcvtimeo] error: " + get_last_error_msg(), true);
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		window::emit_error_message("Network setsockopt [sndtimeo] error: " + get_last_error_msg(), true);
	}
#else
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		std::abort();
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		std::abort();
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
		std::abort();
	}
#endif
	if(as_v6) {
		struct sockaddr_in6 v6_server_address;
		v6_server_address.sin6_addr = IN6ADDR_ANY_INIT;
		v6_server_address.sin6_family = AF_INET6;
		v6_server_address.sin6_port = htons(default_server_port);
		std::memcpy(&server_address, &v6_server_address, sizeof(v6_server_address));
	} else {
		struct sockaddr_in v4_server_address;
		v4_server_address.sin_addr.s_addr = INADDR_ANY;
		v4_server_address.sin_family = AF_INET;
		v4_server_address.sin_port = htons(default_server_port);
		std::memcpy(&server_address, &v4_server_address, sizeof(v4_server_address));
	}
	if(bind(socket_fd, (struct sockaddr*)&server_address, as_v6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in)) < 0) {
		window::emit_error_message("Network bind error: " + get_last_error_msg(), true);
	}
	if(listen(socket_fd, SOMAXCONN) < 0) {
		window::emit_error_message("Network listen error: " + get_last_error_msg(), true);
	}
#ifdef _WIN64
	u_long mode = 1; // 1 to enable non-blocking socket
	ioctlsocket(socket_fd, FIONBIO, &mode);
#endif
	return socket_fd;
}

static socket_t socket_init_client(bool& as_v6, struct sockaddr_storage& client_address, const char *ip_address) {
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	struct addrinfo* result = NULL;
	if(getaddrinfo(ip_address, "1984", &hints, &result) != 0) {
		window::emit_error_message("Network getaddrinfo error: " + get_last_error_msg(), true);
	}
	as_v6 = false;
	bool found = false;
	for(struct addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		if(ptr->ai_socktype == SOCK_STREAM && ptr->ai_protocol == IPPROTO_TCP) {
			if(ptr->ai_family == AF_INET || ptr->ai_family == AF_INET6) {
				as_v6 = ptr->ai_family == AF_INET6;
				std::memcpy(&client_address, ptr->ai_addr, sizeof(sockaddr));
				found = true;
			}
		}
	}
	freeaddrinfo(result);
	if(!found) {
		window::emit_error_message("No suitable host found", true);
	}
	socket_t socket_fd = static_cast<socket_t>(socket(as_v6 ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP));
#ifdef _WIN64
	if(socket_fd == static_cast<socket_t>(INVALID_SOCKET)) {
		window::emit_error_message("Network socket error: " + get_last_error_msg(), true);
	}
#else
	if(socket_fd < 0) {
		window::emit_error_message("Network socket error: " + get_last_error_msg(), true);
	}
#endif
	if(connect(socket_fd, (const struct sockaddr*)&client_address, as_v6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in)) < 0) {
		window::emit_error_message("Network connect error: " + get_last_error_msg(), true);
	}
	return socket_fd;
}

//
// non-platform specific
//

static dcon::nation_id get_temp_nation(sys::state& state) {
	// give the client a "joining" nation, basically a temporal nation choosen
	// "randomly" that is tied to the client iself
	for(auto n : state.nations_by_rank)
		if(!state.world.nation_get_is_player_controlled(n) && state.world.nation_get_owned_province_count(n) > 0) {
			bool is_taken = false;
			for(auto& client : state.network_state.clients) {
				if(client.playing_as == n) {
					is_taken = true;
					break;
				}
			}
			if(!is_taken) {
				return n;
			}
		}
	return dcon::nation_id{ };
}

void init(sys::state& state) {
	if(state.network_mode == sys::network_mode_type::single_player)
		return; // Do nothing in singleplayer

	state.network_state.finished = false;
#ifdef _WIN64
    WSADATA data;
	if(WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		window::emit_error_message("WSA startup error: " + get_last_error_msg(), true);
	}
#endif
	if(state.network_mode == sys::network_mode_type::host) {
		state.network_state.socket_fd = socket_init_server(state.network_state.as_v6, state.network_state.address);
	} else {
		assert(state.network_state.ip_address.size() > 0);
		state.network_state.socket_fd = socket_init_client(state.network_state.as_v6, state.network_state.address, state.network_state.ip_address.c_str());
	}

	// Host must have an already selected nation, to prevent issues...
	if(state.network_mode == sys::network_mode_type::host) {
		state.local_player_nation = get_temp_nation(state);
		assert(bool(state.local_player_nation));
		/* Materialize it into a command we send to new clients who connect and have to replay everything... */
		command::payload c;
		memset(&c, 0, sizeof(c));
		c.type = command::command_type::notify_player_joins;
		c.source = state.local_player_nation;
		c.data.player_name = state.network_state.nickname;
		state.network_state.outgoing_commands.push(c);
	}
}

static void disconnect_client(sys::state& state, client_data& client, bool graceful) {
	if(command::can_notify_player_leaves(state, client.playing_as, graceful)) {
		command::notify_player_leaves(state, client.playing_as, graceful);
	}
	socket_shutdown(client.socket_fd);
	client.socket_fd = 0;
	client.send_buffer.clear();
	client.early_send_buffer.clear();
	client.total_sent_bytes = 0;
	client.save_stream_size = 0;
	client.save_stream_offset = 0;
	client.playing_as = dcon::nation_id{};
	client.recv_count = 0;
	client.handshake = true;
}

static uint8_t* write_network_compressed_section(uint8_t* ptr_out, uint8_t const* ptr_in, uint32_t uncompressed_size) {
	uint32_t decompressed_length = uncompressed_size;
	uint32_t section_length = uint32_t(ZSTD_compress(ptr_out + sizeof(uint32_t) * 2, ZSTD_compressBound(uncompressed_size), ptr_in,
		uncompressed_size, ZSTD_maxCLevel())); // write compressed data
	memcpy(ptr_out, &section_length, sizeof(uint32_t));
	memcpy(ptr_out + sizeof(uint32_t), &decompressed_length, sizeof(uint32_t));
	return ptr_out + sizeof(uint32_t) * 2 + section_length;
}

template<typename T>
static uint8_t const* with_network_decompressed_section(uint8_t const* ptr_in, T const& function) {
	uint32_t section_length = 0;
	uint32_t decompressed_length = 0;
	memcpy(&section_length, ptr_in, sizeof(uint32_t));
	memcpy(&decompressed_length, ptr_in + sizeof(uint32_t), sizeof(uint32_t));
	auto temp_buffer = std::unique_ptr<uint8_t[]>(new uint8_t[decompressed_length]);
	ZSTD_decompress(temp_buffer.get(), decompressed_length, ptr_in + sizeof(uint32_t) * 2, section_length);
	function(temp_buffer.get(), decompressed_length);
	return ptr_in + sizeof(uint32_t) * 2 + section_length;
}

bool client_data::is_banned(sys::state& state) const {
	if(state.network_state.as_v6) {
		auto sa = (struct sockaddr_in6 const*)&address;
		return std::find_if(state.network_state.v6_banlist.begin(), state.network_state.v6_banlist.end(), [&](auto const& a) {
			return std::memcmp(&sa->sin6_addr, &a, sizeof(a)) == 0;
		}) != state.network_state.v6_banlist.end();
	} else {
		auto sa = (struct sockaddr_in const*)&address;
		return std::find_if(state.network_state.v4_banlist.begin(), state.network_state.v4_banlist.end(), [&](auto const& a) {
			return std::memcmp(&sa->sin_addr, &a, sizeof(a)) == 0;
		}) != state.network_state.v4_banlist.end();
	}
}

static void send_post_handshake_commands(sys::state& state, network::client_data& client) {
	std::vector<char> tmp = client.send_buffer;
	client.send_buffer.clear();
	if(state.current_scene.starting_scene) {
		/* Send the savefile to the newly connected client (if not a new game) */
		if(!state.network_state.is_new_game) {
			command::payload c;
			memset(&c, 0, sizeof(command::payload));
			c.type = command::command_type::notify_save_loaded;
			c.source = state.local_player_nation;
			c.data.notify_save_loaded.target = client.playing_as;
			network::broadcast_save_to_clients(state, c, state.network_state.current_save_buffer.get(), state.network_state.current_save_length, state.network_state.current_save_checksum);
#ifndef NDEBUG
			state.console_log("host:send:cmd: (new(2)->save_loaded)");
#endif
		}
		{ /* Tell this client about every other client */
			command::payload c;
			memset(&c, 0, sizeof(c));
			c.type = command::command_type::notify_player_joins;
			for(const auto n : state.world.in_nation) {
				if(n == client.playing_as) {
					c.source = client.playing_as;
					c.data.player_name = client.hshake_buffer.nickname;
					broadcast_to_clients(state, c);
					command::execute_command(state, c);
#ifndef NDEBUG
					state.console_log("host:send:cmd: (new(2)->self_join)");
#endif
				} else if(n.get_is_player_controlled()) {
					c.source = n;
					c.data.player_name = state.network_state.map_of_player_names[n.id.index()];
					socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
#ifndef NDEBUG
					state.console_log("host:send:cmd: (new(2)->others_join) " + std::to_string(n.id.index()));
#endif
				}
			}
		}
	} else if(state.current_scene.game_in_progress) {
		{ /* Tell this client about every other client */
			command::payload c;
			memset(&c, 0, sizeof(c));
			c.type = command::command_type::notify_player_joins;
			for(const auto n : state.world.in_nation) {
				if(n == client.playing_as) {
					c.source = client.playing_as;
					c.data.player_name = client.hshake_buffer.nickname;
					broadcast_to_clients(state, c);
					command::execute_command(state, c);
#ifndef NDEBUG
					state.console_log("host:send:cmd: (new->self_join)");
#endif
				} else if(n.get_is_player_controlled()) {
					c.source = n;
					c.data.player_name = state.network_state.map_of_player_names[n.id.index()];
					socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
#ifndef NDEBUG
					state.console_log("host:send:cmd: (new->others_join) " + std::to_string(n.id.index()));
#endif
				}
			}
		}
		/* Reload clients */
		if(!state.network_state.is_new_game) {
			std::vector<dcon::nation_id> players;
			for(const auto n : state.world.in_nation)
				if(state.world.nation_get_is_player_controlled(n))
					players.push_back(n);
			dcon::nation_id old_local_player_nation = state.local_player_nation;
			state.local_player_nation = dcon::nation_id{ };
			network::write_network_save(state);
			/* Then reload as if we loaded the save data */
			state.preload();
			with_network_decompressed_section(state.network_state.current_save_buffer.get(), [&state](uint8_t const* ptr_in, uint32_t length) {
				read_save_section(ptr_in, ptr_in + length, state);
			});
			state.fill_unsaved_data();
			for(const auto n : players)
				state.world.nation_set_is_player_controlled(n, true);
			state.local_player_nation = old_local_player_nation;
			assert(state.world.nation_get_is_player_controlled(state.local_player_nation));
			{ /* Reload all the other clients except the newly connected one */
				command::payload c;
				memset(&c, 0, sizeof(command::payload));
				c.type = command::command_type::notify_reload;
				c.source = state.local_player_nation;
				c.data.notify_reload.checksum = state.get_save_checksum();
				for(auto& other_client : state.network_state.clients) {
					if(other_client.playing_as != client.playing_as) {
						socket_add_to_send_queue(other_client.send_buffer, &c, sizeof(c));
#ifndef NDEBUG
						state.console_log("host:send:cmd: (new->reload)");
#endif
					}
				}
			}
			{ /* Send the savefile to the newly connected client (if not a new game) */
				command::payload c;
				memset(&c, 0, sizeof(command::payload));
				c.type = command::command_type::notify_save_loaded;
				c.source = state.local_player_nation;
				c.data.notify_save_loaded.target = client.playing_as;
				network::broadcast_save_to_clients(state, c, state.network_state.current_save_buffer.get(), state.network_state.current_save_length, state.network_state.current_save_checksum);
#ifndef NDEBUG
				state.console_log("host:send:cmd: (new->save_loaded)");
#endif
			}
		}
		{
			command::payload c;
			memset(&c, 0, sizeof(c));
			c.type = command::command_type::notify_start_game;
			c.source = state.local_player_nation;
			socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
#ifndef NDEBUG
			state.console_log("host:send:cmd: (new->start_game)");
#endif
		}
	}
	auto old_size = client.send_buffer.size();
	client.send_buffer.resize(old_size + tmp.size());
	std::memcpy(client.send_buffer.data() + old_size, tmp.data(), tmp.size());
}

static void receive_from_clients(sys::state& state) {
	for(auto& client : state.network_state.clients) {
		if(!client.is_active())
			continue;
		int r = 0;
		if(client.handshake) {
			r = socket_recv(client.socket_fd, &client.hshake_buffer, sizeof(client.hshake_buffer), &client.recv_count, [&]() {
				if(std::memcmp(client.hshake_buffer.password, state.network_state.password, sizeof(state.network_state.password)) != 0) {
					disconnect_client(state, client, false);
					return;
				}
				send_post_handshake_commands(state, client);
				/* Exit from handshake mode */
				client.handshake = false;
				state.game_state_updated.store(true, std::memory_order::release);
			});
		} else {
			r = socket_recv(client.socket_fd, &client.recv_buffer, sizeof(client.recv_buffer), &client.recv_count, [&]() {
				switch(client.recv_buffer.type) {
				case command::command_type::invalid:
				case command::command_type::notify_player_ban:
				case command::command_type::notify_player_kick:
				case command::command_type::notify_save_loaded:
				case command::command_type::notify_reload:
				case command::command_type::advance_tick:
				case command::command_type::notify_start_game:
				case command::command_type::notify_stop_game:
				case command::command_type::notify_pause_game:
				case command::command_type::notify_player_joins:
				case command::command_type::save_game:
					break; // has to be valid/sendable by client
				default:
					/* Has to be from the nation of the client proper - and early
					discard invalid commands */
					if(client.recv_buffer.source == client.playing_as
					&& command::can_perform_command(state, client.recv_buffer)) {
						state.network_state.outgoing_commands.push(client.recv_buffer);
					}
					break;
				}
#ifndef NDEBUG
				state.console_log("host:recv:client_cmd: " + std::to_string(uint32_t(client.recv_buffer.type)));
#endif
			});
		}
		if(r != 0) { // error
#if !defined(NDEBUG) && defined(_WIN32)
			state.console_log("host:disconnect: in-receive err=" + std::to_string(int32_t(r)) + "::" + get_last_error_msg());
#endif
			network::disconnect_client(state, client, false);
		}
	}
}

void write_network_save(sys::state& state) {
	/* A save lock will be set when we load a save, naturally loading a save implies
	that we have done preload/fill_unsaved so we will skip doing that again, to save a
	bit of sanity on our miserable CPU */
	size_t length = sizeof_save_section(state);
	auto save_buffer = std::unique_ptr<uint8_t[]>(new uint8_t[length]);
	/* Clear the player nation */
	assert(state.local_player_nation == dcon::nation_id{ });
	write_save_section(save_buffer.get(), state); //writeoff data
	// this is an upper bound, since compacting the data may require less space
	state.network_state.current_save_buffer.reset(new uint8_t[ZSTD_compressBound(length) + sizeof(uint32_t) * 2]);
	auto buffer_position = write_network_compressed_section(state.network_state.current_save_buffer.get(), save_buffer.get(), uint32_t(length));
	state.network_state.current_save_length = uint32_t(buffer_position - state.network_state.current_save_buffer.get());
	state.network_state.current_save_checksum = state.get_save_checksum();
}

void broadcast_save_to_clients(sys::state& state, command::payload& c, uint8_t const* buffer, uint32_t length, sys::checksum_key const& k) {
	assert(length > 0);
	assert(c.type == command::command_type::notify_save_loaded);
	c.data.notify_save_loaded.checksum = k;
	for(auto& client : state.network_state.clients) {
		if(!client.is_active())
			continue;
		bool send_full = (client.playing_as == c.data.notify_save_loaded.target) || (!c.data.notify_save_loaded.target);
		if(send_full && !state.network_state.is_new_game) {
			/* And then we have to first send the command payload itself */
			client.save_stream_size = size_t(length);
			c.data.notify_save_loaded.length = size_t(length);
			socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
			/* And then the bulk payload! */
			client.save_stream_offset = client.total_sent_bytes + client.send_buffer.size();
			socket_add_to_send_queue(client.send_buffer, buffer, size_t(length));
#ifndef NDEBUG
			state.console_log("host:send:save: " + std::to_string(uint32_t(length)));
#endif
		}
	}
}

void broadcast_to_clients(sys::state& state, command::payload& c) {
	if(c.type == command::command_type::save_game)
		return;
	assert(c.type != command::command_type::notify_save_loaded);
	/* Propagate to all the clients */
	for(auto& client : state.network_state.clients) {
		if(client.is_active()) {
			socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
		}
	}
}

static void accept_new_clients(sys::state& state) {
	/* Check if any new clients are to join us */
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(state.network_state.socket_fd, &rfds);
	struct timeval tv{};
	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	if(select(socket_t(int(state.network_state.socket_fd) + 1), &rfds, nullptr, nullptr, &tv) <= 0)
		return;
	
	// Find available slot for client
	for(auto& client : state.network_state.clients) {
		if(client.is_active())
			continue;
		socklen_t addr_len = state.network_state.as_v6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
		client.socket_fd = accept(state.network_state.socket_fd, (struct sockaddr*)&client.address, &addr_len);
		if(client.is_banned(state)) {
			disconnect_client(state, client, false);
			break;
		}
		if(state.current_scene.final_scene) {
			disconnect_client(state, client, false);
			break;
		}
		/* Send it data so she is in sync with everyone else! */
		client.playing_as = get_temp_nation(state);
		assert(client.playing_as);
		{ /* Tell the client their assigned nation */
			server_handshake_data hshake;
			hshake.seed = state.game_seed;
			hshake.assigned_nation = client.playing_as;
			hshake.scenario_checksum = state.scenario_checksum;
			hshake.save_checksum = state.get_save_checksum();
			socket_add_to_send_queue(client.early_send_buffer, &hshake, sizeof(hshake));
		}
#ifndef NDEBUG
		state.console_log("host:send:cmd: handshake -> " + std::to_string(client.playing_as.index()));
#endif
		return;
	}
}

void send_and_receive_commands(sys::state& state) {
	/* An issue that arose in multiplayer is that the UI was loading the savefile
	   directly, while the game state loop was running, this was fine with the
	   assumption that commands weren't executed while the save was being loaded
	   HOWEVER in multiplayer this is often the case, so we have to block all
	   commands until the savefile is finished loading
	   This way, we're able to effectively and safely queue commands until we
	   can receive them AFTER loading the savefile. */
	if(state.network_state.save_slock.load(std::memory_order::acquire) == true)
		return;

	if(state.network_state.finished)
		return;

	bool command_executed = false;
	if(state.network_mode == sys::network_mode_type::host) {
		accept_new_clients(state); // accept new connections
		receive_from_clients(state); // receive new commands
		// send the commands of the server to all the clients
		auto* c = state.network_state.outgoing_commands.front();
		while(c) {
			if(!command::is_console_command(c->type)) {
				// Generate checksum on the spot
				if(c->type == command::command_type::advance_tick) {
					if(state.current_date.to_ymd(state.start_date).day == 1 || state.cheat_data.daily_oos_check) {
						c->data.advance_tick.checksum = state.get_save_checksum();
					}
				}
				broadcast_to_clients(state, *c);
				command::execute_command(state, *c);
				command_executed = true;
#ifndef NDEBUG
				state.console_log("host:receive:cmd: " + std::to_string(uint32_t(c->type)));
#endif
			}
			state.network_state.outgoing_commands.pop();
			c = state.network_state.outgoing_commands.front();
		}

		for(auto& client : state.network_state.clients) {
			if(!client.is_active())
				continue;
			if(client.handshake) {
				if(client.early_send_buffer.size() > 0) {
					size_t old_size = client.early_send_buffer.size();
					int r = socket_send(client.socket_fd, client.early_send_buffer);
					if(r != 0) { // error
#if !defined(NDEBUG) && defined(_WIN32)
						state.console_log("host:disconnect: in-send-EARLY err=" + std::to_string(int32_t(r)) + "::" + get_last_error_msg());
#endif
						disconnect_client(state, client, false);
						continue;
					}
					client.total_sent_bytes += old_size - client.early_send_buffer.size();
#ifndef NDEBUG
					if(old_size != client.early_send_buffer.size())
						state.console_log("host:send:stats: [EARLY] " + std::to_string(uint32_t(client.total_sent_bytes)) + " bytes");
#endif
				}
			} else {
				if(client.send_buffer.size() > 0) {
					size_t old_size = client.send_buffer.size();
					int r = socket_send(client.socket_fd, client.send_buffer);
					if(r != 0) { // error
#if !defined(NDEBUG) && defined(_WIN32)
						state.console_log("host:disconnect: in-send-INGAME err=" + std::to_string(int32_t(r)) + "::" + get_last_error_msg());
#endif
						disconnect_client(state, client, false);
						continue;
					}
					client.total_sent_bytes += old_size - client.send_buffer.size();
#ifndef NDEBUG
					if(old_size != client.send_buffer.size())
						state.console_log("host:send:stats: [SEND] " + std::to_string(uint32_t(client.total_sent_bytes)) + " bytes");
#endif
				}
			}
		}
	} else if(state.network_mode == sys::network_mode_type::client) {
		if(state.network_state.handshake) {
			/* Send our client's handshake */
			int r = socket_recv(state.network_state.socket_fd, &state.network_state.s_hshake, sizeof(state.network_state.s_hshake), &state.network_state.recv_count, [&]() {
#ifndef NDEBUG
				state.console_log("client:recv:handshake: OK");
#endif
				if(!state.scenario_checksum.is_equal(state.network_state.s_hshake.scenario_checksum)) {
					bool found_match = false;
					// Find a scenario with a matching checksum
					auto dir = simple_fs::get_or_create_scenario_directory();
					for(const auto& uf : simple_fs::list_files(dir, NATIVE(".bin"))) {
						auto f = simple_fs::open_file(uf);
						if(f) {
							auto contents = simple_fs::view_contents(*f);
							sys::scenario_header scen_header;
							if(contents.file_size > sizeof(sys::scenario_header)) {
								sys::read_scenario_header(reinterpret_cast<const uint8_t*>(contents.data), scen_header);
								if(!scen_header.checksum.is_equal(state.network_state.s_hshake.scenario_checksum))
									continue; // Same checksum
								if(scen_header.version != sys::scenario_file_version)
									continue; // Same version of scenario
								if(sys::try_read_scenario_and_save_file(state, simple_fs::get_file_name(uf))) {
									state.fill_unsaved_data();
									found_match = true;
									break;
								}
							}
						}
					}
					if(!found_match) {
						std::string msg = "Could not find a scenario with a matching checksum!"
							"This is most likely a false positive, so just ask the host for their"
							"scenario file and it should work. Or you haven't clicked on 'Make scenario'!";
						msg += "\n";
						msg += "Host should give you the scenario from:\n"
							"'My Documents\\Project Alice\\scenarios\\<Most recent scenario>'";
						msg += "And you place it on:\n"
							"'My Documents\\Project Alice\\scenarios\\'\n";

						window::emit_error_message(msg.c_str(), true);
					}
				}
				state.session_host_checksum = state.network_state.s_hshake.save_checksum;
				state.game_seed = state.network_state.s_hshake.seed;
				state.local_player_nation = state.network_state.s_hshake.assigned_nation;
				state.world.nation_set_is_player_controlled(state.local_player_nation, true);
				/* Send our client handshake back */
				client_handshake_data hshake;
				hshake.nickname = state.network_state.nickname;
				std::memcpy(hshake.password, state.network_state.password, sizeof(hshake.password));
				socket_add_to_send_queue(state.network_state.send_buffer, &hshake, sizeof(hshake));
				state.network_state.handshake = false;
			});
			if(r != 0) { // error
				ui::popup_error_window(state, "Network Error", "Network client handshake receive error: " + get_last_error_msg());
				network::finish(state, false);
				return;
			}
		} else if(state.network_state.save_stream) {
			int r = socket_recv(state.network_state.socket_fd, state.network_state.save_data.data(), state.network_state.save_data.size(), &state.network_state.recv_count, [&]() {
#ifndef NDEBUG
				state.console_log("client:recv:save: len=" + std::to_string(uint32_t(state.network_state.save_data.size())));
#endif
				std::vector<dcon::nation_id> players;
				for(const auto n : state.world.in_nation)
					if(state.world.nation_get_is_player_controlled(n))
						players.push_back(n);
				dcon::nation_id old_local_player_nation = state.local_player_nation;
				state.preload();
				with_network_decompressed_section(state.network_state.save_data.data(), [&state](uint8_t const* ptr_in, uint32_t length) {
					read_save_section(ptr_in, ptr_in + length, state);
				});
				state.local_player_nation = dcon::nation_id{ };
				state.fill_unsaved_data();
				for(const auto n : players)
					state.world.nation_set_is_player_controlled(n, true);
				state.local_player_nation = old_local_player_nation;
				assert(state.world.nation_get_is_player_controlled(state.local_player_nation));
#ifndef NDEBUG
				auto save_checksum = state.get_save_checksum();
				assert(save_checksum.is_equal(state.session_host_checksum));
#endif
				state.railroad_built.store(true, std::memory_order::release);
				state.game_state_updated.store(true, std::memory_order::release);
				state.network_state.save_data.clear();
				state.network_state.save_stream = false; // go back to normal command loop stuff
			});
			if(r != 0) { // error
				ui::popup_error_window(state, "Network Error", "Network client save stream receive error: " + get_last_error_msg());
				network::finish(state, false);
				return;
			}
		} else {
			// receive commands from the server and immediately execute them
			int r = socket_recv(state.network_state.socket_fd, &state.network_state.recv_buffer, sizeof(state.network_state.recv_buffer), &state.network_state.recv_count, [&]() {
				command::execute_command(state, state.network_state.recv_buffer);
				command_executed = true;
				// start save stream!
				if(state.network_state.recv_buffer.type == command::command_type::notify_save_loaded) {
					uint32_t save_size = state.network_state.recv_buffer.data.notify_save_loaded.length;
					state.network_state.save_stream = true;
					assert(save_size > 0);
					if(save_size >= 32 * 1000 * 1000) { // 32 MB
						ui::popup_error_window(state, "Network Error", "Network client save stream too big: " + get_last_error_msg());
						network::finish(state, false);
						return;
					}
					state.network_state.save_data.resize(static_cast<size_t>(save_size));
				}
#ifndef NDEBUG
				state.console_log("client:recv:cmd: " + std::to_string(uint32_t(state.network_state.recv_buffer.type)));
#endif
			});
			if(r != 0) { // error
				ui::popup_error_window(state, "Network Error", "Network client command receive error: " + get_last_error_msg());
				network::finish(state, false);
				return;
			}
			// send the outgoing commands to the server and flush the entire queue
			auto* c = state.network_state.outgoing_commands.front();
			while(c) {
#ifndef NDEBUG
				state.console_log("client:send:cmd: " + std::to_string(uint32_t(c->type)));
#endif
				if(c->type == command::command_type::save_game) {
					command::execute_command(state, *c);
					command_executed = true;
				} else {
					socket_add_to_send_queue(state.network_state.send_buffer, c, sizeof(*c));
				}
				state.network_state.outgoing_commands.pop();
				c = state.network_state.outgoing_commands.front();
			}
		}
		/* Do not send commands while we're on save stream mode! */
		if(!state.network_state.save_stream) {
			if(socket_send(state.network_state.socket_fd, state.network_state.send_buffer) != 0) { // error
				ui::popup_error_window(state, "Network Error", "Network client command send error: " + get_last_error_msg());
				network::finish(state, false);
				return;
			}
		}
		assert(state.network_state.early_send_buffer.empty()); //do not use the early send buffer
	}

	if(command_executed) {
		if(state.network_state.out_of_sync && !state.network_state.reported_oos) {
			command::notify_player_oos(state, state.local_player_nation);
			state.network_state.reported_oos = true;
		}
		state.game_state_updated.store(true, std::memory_order::release);
	}
}

void finish(sys::state& state, bool notify_host) {
	if(state.network_mode == sys::network_mode_type::single_player)
		return; // Do nothing in singleplayer

	state.network_state.finished = true;
	if(notify_host && state.network_mode == sys::network_mode_type::client) {
		if(!state.network_state.save_stream) {
			// send the outgoing commands to the server and flush the entire queue
			{
				auto* c = state.network_state.outgoing_commands.front();
				while(c) {
					if(c->type == command::command_type::save_game) {
						command::execute_command(state, *c);
					} else {
						socket_add_to_send_queue(state.network_state.send_buffer, c, sizeof(*c));
					}
					state.network_state.outgoing_commands.pop();
					c = state.network_state.outgoing_commands.front();
				}
			}
			command::payload c;
			memset(&c, 0, sizeof(c));
			c.type = command::command_type::notify_player_leaves;
			c.source = state.local_player_nation;
			c.data.notify_leave.make_ai = true;
			socket_add_to_send_queue(state.network_state.send_buffer, &c, sizeof(c));
			while(state.network_state.send_buffer.size() > 0) {
				if(socket_send(state.network_state.socket_fd, state.network_state.send_buffer) != 0) { // error
					//ui::popup_error_window(state, "Network Error", "Network client command send error: " + get_last_error_msg());
					break;
				}
			}
		}
	}
	
	socket_shutdown(state.network_state.socket_fd);
#ifdef _WIN64
	WSACleanup();
#endif
}

void ban_player(sys::state& state, client_data& client) {
	if(!client.is_active())
		return;
	socket_shutdown(client.socket_fd);
	client.socket_fd = 0;
	if(state.network_state.as_v6) {
		auto sa = (struct sockaddr_in6*)&client.address;
		state.network_state.v6_banlist.push_back(sa->sin6_addr);
	} else {
		auto sa = (struct sockaddr_in*)&client.address;
		state.network_state.v4_banlist.push_back(sa->sin_addr);
	}
}

void kick_player(sys::state& state, client_data& client) {
	if(!client.is_active())
		return;
	socket_shutdown(client.socket_fd);
	client.socket_fd = 0;
}

void switch_player(sys::state& state, dcon::nation_id new_n, dcon::nation_id old_n) {
	state.network_state.map_of_player_names.insert_or_assign(new_n.index(), state.network_state.map_of_player_names[old_n.index()]);
	if(state.network_mode == sys::network_mode_type::host) {
		for(auto& client : state.network_state.clients) {
			if(!client.is_active())
				continue;
			if(client.playing_as == old_n) {
				client.playing_as = new_n;
			}
		}
	}
}

}
