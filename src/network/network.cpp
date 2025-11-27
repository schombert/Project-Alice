#ifdef _WIN64 // WINDOWS
#define UNICODE
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
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
using std::format;
#else // NIX
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <fmt/chrono.h>
using fmt::format;
#endif // ...
#include <string_view>
#include "system_state.hpp"
#include "commands.hpp"
#include "SPSCQueue.h"
#include "network.hpp"
#include "serialization.hpp"
#include "gui_error_window.hpp"
#include "persistent_server_extensions.hpp"

#define ZSTD_STATIC_LINKING_ONLY
#define XXH_NAMESPACE ZSTD_
#include "zstd.h"

#ifdef _WIN64
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ntdll.lib")
#endif

#include <webapi/json.hpp>
#include "dcon_oos_reporter_generated.hpp"
using json = nlohmann::json;

namespace network {

//
// platform specific
//

struct local_addresses {
	std::string address;
	bool ipv6 = false;
};

port_forwarder::port_forwarder() { }


void port_forwarder::start_forwarding(uint16_t port) {
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
					if(SUCCEEDED(port_mappings->Add(port, proto, port, local_host, enabled, desc, &opened_port)) && opened_port) {
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
					((sockaddr_in*)(&source_ip))->sin_port = port;
					((sockaddr_in*)(&source_ip))->sin_addr.s_addr = inet_addr(found_locals[0].address.c_str());
					((sockaddr_in*)(&source_ip))->sin_family = AF_INET;

					((sockaddr_in*)(&ext_ip))->sin_port = port;
					((sockaddr_in*)(&ext_ip))->sin_family = AF_INET;
				} else {
					((sockaddr_in6*)(&source_ip))->sin6_port = port;
					PCSTR term = nullptr;
					RtlIpv6StringToAddressA(found_locals[0].address.c_str(), &term, &(((sockaddr_in6*)(&source_ip))->sin6_addr));
					((sockaddr_in6*)(&source_ip))->sin6_family = AF_INET6;

					((sockaddr_in6*)(&ext_ip))->sin6_port = port;
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
				port_mappings->Remove(port, proto);

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

	std::string err_msg;
	if(err == WSAECONNRESET) {
		err_msg += "Host was lost ";
	} else if(err == WSAEHOSTDOWN) {
		err_msg += "Host is down ";
	} else if(err == WSAEHOSTUNREACH) {
		err_msg += "Host is unreachable ";
	} else {
		err_msg += "Network issue ocurred ";
	}
	err_msg += "Technical details: ";
	err_msg += std::to_string(err);
	err_msg += " => ";
	err_msg += simple_fs::native_to_utf8(err_text);
	return err_msg;
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
	// Did we receive the amount of data?
	if(*m >= len) {
		assert(*m <= len);
		*m = 0; // reset
		func();
		return 0;
	}
	// No data received
	return -1;
}



template<sys::network_mode_type NetworkType, typename F>
static int socket_recv_command(socket_t socket_fd, command::command_data* data, size_t* recv_count, bool* receiving_payload , F&& func) {
	// check flag to see if the receiving payload flag is off, an thus should read the header
	if(!(*receiving_payload)) {
		return socket_recv(socket_fd, data, sizeof(command::cmd_header), recv_count, [&]() {
			// if this is being run as host, early discard commands which the host aren't meant to receive
			if constexpr(NetworkType == sys::network_mode_type::host) {
				if(command::valid_host_receive_commands(data->header.type)) {
					*receiving_payload = true;
				}
				else {
					*receiving_payload = false;
				}
			}
			else {
				*receiving_payload = true;
			}
		});
	}
	// otherwise, start receiving the payload
	else {
		auto payload_sz = data->header.payload_size;
		auto cmd_mapping = command::command_type_handlers.find(data->header.type);
		// command must have a defined max and min size, and the specified size in the header must be equal to or less than the max size, and equal to or greater than the min size
		if(cmd_mapping != command::command_type_handlers.end() && cmd_mapping->second.min_payload_size <= payload_sz && cmd_mapping->second.max_payload_size >= payload_sz) {
			data->payload.resize(payload_sz);
			return socket_recv(socket_fd, reinterpret_cast<uint8_t*>(data->payload.data()), payload_sz, recv_count, [&]() { func(); *receiving_payload = false; });
		}
		// if the command does not fit these requirements, discard it and reset recv count
		else {
			*recv_count = 0;
			*receiving_payload = false;
			return -1;
		}
	}
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

static void socket_add_command_to_send_queue(std::vector<char>& buffer, const command::command_data* data) {
	auto payload_sz = data->header.payload_size;
	assert(payload_sz == data->payload.size());
	auto cmd_mapping = command::command_type_handlers.find(data->header.type);
	if(cmd_mapping != command::command_type_handlers.end() && cmd_mapping->second.min_payload_size <= payload_sz && cmd_mapping->second.max_payload_size >= payload_sz) {
		// Send the header
		socket_add_to_send_queue(buffer, data, sizeof(command::cmd_header));
		// Then the payload
		socket_add_to_send_queue(buffer, data->payload.data(), payload_sz);
	}

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

static socket_t socket_init_server(bool as_v6, struct sockaddr_storage& server_address, uint16_t port) {
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
		v6_server_address.sin6_port = htons(port);
		std::memcpy(&server_address, &v6_server_address, sizeof(v6_server_address));
	} else {
		struct sockaddr_in v4_server_address;
		v4_server_address.sin_addr.s_addr = INADDR_ANY;
		v4_server_address.sin_family = AF_INET;
		v4_server_address.sin_port = htons(port);
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

static socket_t socket_init_client(bool& as_v6, struct sockaddr_storage& client_address, const char *ip_address, const char* port) {
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	struct addrinfo* result = NULL;
	if(getaddrinfo(ip_address, port, &hints, &result) != 0) {
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

void clear_socket(sys::state& state, client_data& client) {
	socket_shutdown(client.socket_fd);
	client.socket_fd = 0;
	client.send_buffer.clear();
	client.early_send_buffer.clear();
	client.total_sent_bytes = 0;
	client.save_stream_size = 0;
	client.save_stream_offset = 0;
	client.recv_count = 0;
	client.handshake = true;
	client.last_seen = sys::date{};
	client.receiving_payload_flag = false;
}

static void disconnect_client(sys::state& state, client_data& client, bool graceful) {
	auto leaving_player_nation = state.world.mp_player_get_nation_from_player_nation(client.player_id);
	if(command::can_notify_player_leaves(state, leaving_player_nation, graceful, client.player_id)) {
		command::notify_player_leaves(state, leaving_player_nation, graceful, client.player_id);
	}
#ifndef NDEBUG
	state.console_log("server:disconnectclient | country:" + std::to_string(leaving_player_nation.index()));
	log_player_nations(state);
#endif
	clear_socket(state, client);
}


// Host-only. Sends commands directly to client socket while skipping the command queue. Should be used for utility network commands ONLY. For regular commands use their respective functions
template<typename F>
void send_network_command(sys::state& state, F&& client_selector, const command::command_data& command) {

	/* Send the command to the clients matching the selector */

	for(auto& client : state.network_state.clients) {
		if(client.is_active() && client_selector(client)) {
#ifndef NDEBUG
			state.console_log("host:sent network command to player  " + client.hshake_buffer.nickname.to_string() + ":cmd = " + readableCommandTypes[uint8_t(command.header.type)]);
#endif
			socket_add_command_to_send_queue(client.send_buffer, &command);
		}
	}


}

template<typename F>
void send_mp_data(sys::state& state, F&& client_selector) {

	/* Send the MP data to the clients matching the selector */
	{
		// write MP data in buffer
		auto mp_data_sz = uint32_t(sys::sizeof_mp_data(state));
		auto mp_data_buffer = std::unique_ptr<uint8_t[]>(new uint8_t[mp_data_sz]);
		sys::write_mp_data(mp_data_buffer.get(), state);

		command::command_data mp_data_cmd{ command::command_type::notify_mp_data, state.local_player_id };
		command::notify_mp_data_data mp_data_payload{ };

		mp_data_payload.data_len = mp_data_sz;

		mp_data_cmd << mp_data_payload;
		mp_data_cmd.push_ptr(mp_data_buffer.get(), mp_data_sz);

		for(auto& client : state.network_state.clients) {
			if(client.is_active() && client_selector(client)) {
				socket_add_command_to_send_queue(client.send_buffer, &mp_data_cmd);
			}
		}

	}

}


template<typename F>
void send_savegame(sys::state& state, F&& client_selector) {

	/* Send the savefile only to the clients matching the selector */
	{
		command::command_data c{ command::command_type::notify_save_loaded, state.local_player_id };
		command::notify_save_loaded_data payload{ };

		payload.checksum = state.network_state.current_mp_state_checksum;
		payload.length = size_t(state.network_state.current_save_length);

		c << payload;

		assert(state.network_state.current_save_length);

		for(auto& client : state.network_state.clients) {
			// if selector matches, send the save data
			if(client.is_active() && client_selector(client)) {
#ifndef NDEBUG
				const auto now = std::chrono::system_clock::now();
				state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " host:broadcast:cmd | (new->save_loaded) | checksum: " + sha512.hash(state.network_state.current_mp_state_checksum.to_char())
				+ " | target playerid: " + std::to_string(client.player_id.index()));
				log_player_nations(state);
#endif
				network::broadcast_save_to_single_client(state, c, client, state.network_state.current_save_buffer.get());
			}
		}

	}

	//	if(hotjoin) {
	//		// we expect the save has been reloaded by now
	//		{ /* Reload all the other clients except the newly connected one */
	//
	//			command::command_data c{ command::command_type::notify_reload, get_host_player(state) };
	//			command::notify_reload_data payload{ host_state_checksum };
	//
	//			c << payload;
	//
	//			for(auto& other_client : state.network_state.clients) {
	//				if(other_client.is_active() && !client_selector(other_client)) {
	//					// for each client that must reload, notify every other client that they are loading
	//					network::notify_player_is_loading(state, other_client.player_id, true);
	//
	//					// then send the actual reload notification
	//					socket_add_command_to_send_queue(other_client.send_buffer, &c);
	//#ifndef NDEBUG
	//					state.console_log("host:send:cmd: (new->reload) | to:" + std::to_string(other_client.playing_as.index()));
	//#endif
	//				}
	//			}
	//		}
	//	}
}


void log_player_nations(sys::state& state) {
	auto msg = std::string{};
	for(const auto n : state.world.in_nation)
		if(state.world.nation_get_is_player_controlled(n))
			state.console_log("player controlled: " + std::to_string(n.id.index()));

	for(const auto p : state.world.in_mp_player) {
		auto data = p.get_nickname();
		auto nickname = sys::player_name{ data };
		auto nation = p.get_player_nation().get_nation();

		state.console_log("player id: " + std::to_string(p.id.index()) + " nickname: " + nickname.to_string() + " nation:" + std::to_string(nation.id.index()));
	}
}


void mp_player_set_fully_loaded(sys::state& state, dcon::mp_player_id player, bool fully_loaded) {
		state.world.mp_player_set_fully_loaded(player, fully_loaded);
		state.network_state.clients_loading_state_changed.store(true);

}

// the way mp_player is used right now, we can safely assume the host is always the first ID, as the host should never be able to leave (and free up the slot) without the game ending.
dcon::mp_player_id get_host_player(sys::state& state) {
	return dcon::mp_player_id{ 0 };
}

bool any_player_on_invalid_nation(sys::state& state) {
	assert(state.network_mode == sys::network_mode_type::host);
	for(auto player : state.world.in_mp_player) {
		if(!bool(player)) {
			continue;
		}
		auto nation = state.world.mp_player_get_nation_from_player_nation(player);
		// the nation must not be invalid, and the nation must exist, but if its the rebel tag, it is allowed to have 0 provinces.
		if(!nation || (state.world.nation_get_owned_province_count(nation) == 0 && state.world.national_identity_get_nation_from_identity_holder(state.national_definitions.rebel_id) != nation)) {
			return true;
		}
	}
	return false;
}

bool nation_has_any_players_on_it(sys::state& state, dcon::nation_id nation) {
	auto iterator = state.world.nation_get_player_nation(nation);
	if(iterator.begin() == iterator.end()) {
		return false;
	}
	else {
		return true;
	}

}

bool check_any_players_loading(sys::state& state) {
	if(state.network_state.clients_loading_state_changed.exchange(false)) {
		for(auto player : state.world.in_mp_player) {
			if(player && !player.get_fully_loaded()) {
				state.network_state.any_client_loading_flag.store(true);
				return true;
			}
		}
		state.network_state.any_client_loading_flag.store(false);
		return false;
	}
	else {
		return state.network_state.any_client_loading_flag.load();
	}
}

void delete_mp_player(sys::state& state, dcon::mp_player_id player, bool make_ai) {
	auto current_nation = state.world.mp_player_get_nation_from_player_nation(player);
	if(!state.world.mp_player_get_fully_loaded(player)) {
		state.network_state.clients_loading_state_changed.store(true);
	}
	auto rel = state.world.mp_player_get_player_nation(player);
	if(rel) {
		state.world.delete_player_nation(rel);
		// check if there are any other players on the nation, if so don't set it to ai
		if(make_ai && !nation_has_any_players_on_it(state, current_nation)) {
			state.world.nation_set_is_player_controlled(current_nation, false);
		}
	}
	state.world.delete_mp_player(player);
}


dcon::mp_player_id create_mp_player(sys::state& state, const sys::player_name& name, const sys::player_password_raw& password, bool fully_loaded, bool is_oos, dcon::nation_id nation_to_play) {
	auto p = state.world.create_mp_player();
	state.world.mp_player_set_nickname(p, name.data);

	auto salt = sys::player_password_salt{}.from_string_view(std::to_string(int32_t(rand())));
	auto hash = sys::player_password_hash{}.from_string_view(sha512.hash(password.to_string() + salt.to_string()));
	state.world.mp_player_set_password_hash(p, hash.data);
	state.world.mp_player_set_password_salt(p, salt.data);
	mp_player_set_fully_loaded(state, p, fully_loaded);
	state.world.mp_player_set_is_oos(p, is_oos);
	if(nation_to_play) {
		state.world.force_create_player_nation(nation_to_play, p);
		state.world.nation_set_is_player_controlled(nation_to_play, true);
	}

	return p;
}

dcon::mp_player_id load_mp_player(sys::state& state, sys::player_name& name, sys::player_password_hash& password_hash, sys::player_password_salt& password_salt) {
	auto p = state.world.create_mp_player();
	state.world.mp_player_set_nickname(p, name.data);
	state.world.mp_player_set_password_hash(p, password_hash.data);
	state.world.mp_player_set_password_salt(p, password_salt.data);

	return p;
}

void update_mp_player_password(sys::state& state, dcon::mp_player_id player_id, sys::player_password_raw& password) {
	auto salt = sys::player_password_salt{}.from_string_view(std::to_string(int32_t(rand())));
	auto hash = sys::player_password_hash{}.from_string_view(sha512.hash(password.to_string() + salt.to_string()));
	state.world.mp_player_set_password_hash(player_id, hash.data);
	state.world.mp_player_set_password_salt(player_id, salt.data);
}

dcon::mp_player_id find_mp_player(sys::state& state, sys::player_name name) {
	for(const auto p : state.world.in_mp_player) {
		auto nickname = p.get_nickname();

		if(std::equal(std::begin(nickname), std::end(nickname), std::begin(name.data))) {
			return p;
		}
	}

	return dcon::mp_player_id{};
}

std::vector<dcon::mp_player_id> find_country_players(sys::state& state, dcon::nation_id nation) {
	std::vector<dcon::mp_player_id> players;
	for(auto p : state.world.nation_get_player_nation(nation)) {
		players.push_back(p.get_mp_player());
	}
	return players;
}

// Reassign player to his previous nation if any
static dcon::nation_id get_player_nation(sys::state& state, sys::player_name name) {

	auto p = find_mp_player(state, name);
	if(p) {
		return state.world.mp_player_get_nation_from_player_nation(p);
	}

	return dcon::nation_id{ };
}

static dcon::nation_id choose_nation_for_player(sys::state& state) {
	// On join or hotjoin give the client a "joining" nation, basically a temporal nation choosen
	// "randomly" that is tied to the client iself

	for(auto n : state.nations_by_rank)
		if(!state.world.nation_get_is_player_controlled(n) && state.world.nation_get_owned_province_count(n) > 0) {
			bool is_taken = false;
			for(auto& client : state.network_state.clients) {
				if(state.world.mp_player_get_nation_from_player_nation( client.player_id) == n) {
					is_taken = true;
					break;
				}
			}
			if(!is_taken) {
				return n;
			}
		}
	// if no nation available, set to rebels (observer basically)
	return state.world.national_identity_get_nation_from_identity_holder(state.national_definitions.rebel_id);
}

void set_no_ai_nations_after_reload(sys::state& state, std::vector<dcon::nation_id>& no_ai_nations, dcon::nation_id old_local_player_nation) {
	for(auto no_ai_nation : no_ai_nations) {
		state.world.nation_set_is_player_controlled(no_ai_nation, true);
	}
	state.local_player_nation = old_local_player_nation;
}

bool any_player_oos(sys::state& state) {
	for(auto player : state.world.in_mp_player) {
		if(player && player.get_is_oos()) {
			return true;
		}
	}
	return false;
}

void client_send_handshake(sys::state& state) {
	/* Send our client handshake back */
	client_handshake_data hshake;
	hshake.nickname = state.network_state.nickname;
	hshake.player_password = state.network_state.player_password;
	std::memcpy(hshake.lobby_password, state.network_state.lobby_password, sizeof(hshake.lobby_password));
	socket_add_to_send_queue(state.network_state.send_buffer, &hshake, sizeof(hshake));

#ifndef NDEBUG
	const auto now = std::chrono::system_clock::now();
	state.console_log(std::string_view(format("{:%d-%m-%Y %H:%M:%OS}", now) + "client:send:handshake"));
#endif
}

int client_process_handshake(sys::state& state) {
	int r = socket_recv(state.network_state.socket_fd, &state.network_state.s_hshake, sizeof(state.network_state.s_hshake), &state.network_state.recv_count, [&]() {
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
		state.local_player_id = state.network_state.s_hshake.assigned_player_id;
		state.world.nation_set_is_player_controlled(state.local_player_nation, true);

#ifndef NDEBUG
		const auto now = std::chrono::system_clock::now();
		state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " client:recv:handshake");
#endif

		state.network_state.handshake = false;

		//update map
		state.set_selected_province(dcon::province_id{});
			});

	return r;
}

void server_send_handshake(sys::state& state, network::client_data& client) {
	/* Tell the client their assigned nation */
	auto plnation = get_player_nation(state, client.hshake_buffer.nickname);
	if(!plnation) {
		plnation = choose_nation_for_player(state);

	}
	player_joins(state, client, plnation);

	/* Send it data so the client is in sync with everyone else! */
	server_handshake_data hshake;
	hshake.seed = state.game_seed;
	hshake.assigned_nation = plnation;
	hshake.scenario_checksum = state.scenario_checksum;
	hshake.save_checksum = state.get_save_checksum();
	hshake.assigned_player_id = client.player_id;
	socket_add_to_send_queue(client.early_send_buffer, &hshake, sizeof(hshake));


#ifndef NDEBUG
	const auto now = std::chrono::system_clock::now();
	state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now)+ " host:send:handshake | assignednation:" + std::to_string(state.world.mp_player_get_nation_from_player_nation(client.player_id).index()));
#endif

	/* Exit from handshake mode */
	client.handshake = false;
}

void init(sys::state& state) {
	if(state.network_mode == sys::network_mode_type::single_player) {
		// create the single player ID needed for a singleplayer match.
		state.local_player_nation = choose_nation_for_player(state);
		auto player_name = sys::player_name{ 'P', 'l', 'a', 'y', 'e', 'r', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		auto discard = sys::player_password_raw{ };
		state.local_player_id = create_mp_player(state, player_name, discard, true, false, state.local_player_nation);
		return;
	}


	state.network_state.finished = false;
#ifdef _WIN64
	WSADATA data;
	if(WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		window::emit_error_message("WSA startup error: " + get_last_error_msg(), true);
	}
#endif
	if(state.network_mode == sys::network_mode_type::host) {
		state.network_state.socket_fd = socket_init_server(state.network_state.as_v6, state.network_state.address, state.host_settings.alice_host_port);
	} else {
		assert(state.network_state.ip_address.size() > 0);
		state.network_state.socket_fd = socket_init_client(state.network_state.as_v6, state.network_state.address, state.network_state.ip_address.c_str(), state.network_state.port.c_str());
	}

	// Host must have an already selected nation, to prevent issues...
	if(state.network_mode == sys::network_mode_type::host) {
		load_player_nations(state);

		auto nid = get_player_nation(state, state.network_state.nickname);
		state.local_player_nation = nid ? nid : choose_nation_for_player(state);

		assert(bool(state.local_player_nation));

		// execute player joins command to create the host player in in the lobby. There should not be any other players yet connected.
		state.local_player_id = command::execute_notify_player_joins(state, state.local_player_nation, state.network_state.nickname, state.network_state.player_password, false, state.local_player_nation);

	}
	else if(state.network_mode == sys::network_mode_type::client) {
		/* Send our client's handshake */
		client_send_handshake(state);
	}
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

std::string add_line_to_oos_report(const std::string& member_name, const std::string& value_1, const std::string& value_2) {
	return "\tObject " + member_name + ": " + value_1 + ", " + value_2 + "\n";

}
template<typename T>
std::string add_compare_to_oos_report_indexed(const T& item_1, const T& item_2, const std::string& member_name, uint32_t index) {
	bool equal = false;
	// have to have this special case as bitfield type does not have a comparison operator and is an external dependency
	if constexpr(std::is_same<T, dcon::bitfield_type>::value) {
		equal = (item_1.v == item_2.v);
	} else {
		equal = (item_1 == item_2);
	}
	if(!equal) {
		if constexpr(std::is_arithmetic<T>::value) {
			return "\tobject " + member_name + ": at index " + std::to_string(index) + ": " + std::to_string(item_1) + ", " + std::to_string(item_2) + "\n";
		}
		else if constexpr(std::is_same<T, dcon::bitfield_type>::value) {
			return "\tobject " + member_name + ": at index " + std::to_string(index) + ": " + std::to_string(item_1.v) + ", " + std::to_string(item_2.v) + "\n";
		}
		else if constexpr(std::is_same<T, char>::value) {
			return "\tobject " + member_name + ": at index " + std::to_string(index) + ": " + item_1 + ", " + item_2 + "\n";
		}
		else if constexpr(std::is_same<T, sys::date>::value || std::is_same<T, dcon::nation_id>::value || std::is_same<T, dcon::state_instance_id>::value || std::is_same<T, dcon::war_id>::value) {
			return "\tobject " + member_name + ": at index " + std::to_string(index) + ": " + std::to_string(item_1.value) + ", " + std::to_string(item_2.value) + "\n";
		}
		else if constexpr(std::is_enum<T>::value) {
			return "\tobject " + member_name + ": at index " + std::to_string(index) + ": " + std::to_string(std::underlying_type_t<T>(item_1)) + ", " + std::to_string(std::underlying_type_t<T>(item_2)) + "\n";
		}
		else if constexpr(std::is_same<T, bool>::value) {
			std::string item_1_str = (item_1) ? "true" : "false";
			std::string item_2_str = (item_2) ? "true" : "false";
			return "\tobject " + member_name + ": at index " + std::to_string(index) + ": " + item_1_str + ", " + item_2_str + "\n";
		}
		else {
			return "\tobject " + member_name + ": at index " + std::to_string(index) + ":\n";
		}
	}
	else {
		return "";
	}
}

template<typename T>
std::string add_compare_to_oos_report(const T& item_1, const T& item_2, const std::string& member_name) {
	bool equal = false;
	// have to have this special case as bitfield type does not have a comparison operator and is an external dependency
	if constexpr(std::is_same<T, dcon::bitfield_type>::value) {
		equal = (item_1.v == item_2.v);
	}
	else {
		equal = (item_1 == item_2);
	}
	if(!equal) {
		if constexpr(std::is_arithmetic<T>::value) {
			return "\tobject " + member_name + ": " + std::to_string(item_1) + ", " + std::to_string(item_2) + "\n";
		} else if constexpr(std::is_same<T, char>::value) {
			return  "\tobject " + member_name + ": " + item_1 + ", " + item_2 + "\n";
		} else if constexpr(std::is_same<T, sys::date>::value || std::is_same<T, dcon::nation_id>::value || std::is_same<T, dcon::state_instance_id>::value || std::is_same<T, dcon::war_id>::value) {
			return "\tobject " + member_name + ": " + std::to_string(item_1.value) + ", " + std::to_string(item_2.value) + "\n";
		}
		else if constexpr(std::is_enum<T>::value) {
			return "\tobject " + member_name + ": " + std::to_string(std::underlying_type_t<T>(item_1)) + ", " + std::to_string(std::underlying_type_t<T>(item_2)) + "\n";
		}
		else if constexpr(std::is_same<T, bool>::value) {
			std::string item_1_str = (item_1) ? "true" : "false";
			std::string item_2_str = (item_2) ? "true" : "false";
			return "\tobject " + member_name + ": " + item_1_str + ", " + item_2_str + "\n";
		}
		else {
			return "\tobject " + member_name + ":\n";
		}
	}
	else {
		return "";
	}
}

template<typename T>
std::string add_collection_compare_to_oos_report(const std::span<const T> collection_1, const std::span<const T> collection_2, const std::string& member_name) {
	std::string result{ };
	if(collection_1.size() != collection_2.size()) {
		result += "\tSize mismatch in " + member_name + ", 1st size: " + std::to_string(collection_1.size()) + ", 2nd size: " + std::to_string(collection_2.size()) + "\n";
	}
	uint32_t smallest_size = std::min(uint32_t(collection_1.size()), uint32_t(collection_2.size()));
	for(uint32_t i = 0; i < smallest_size; i++) {
		result += add_compare_to_oos_report_indexed<T>(collection_1[i], collection_2[i], member_name, i);
	}
	return result;
}

std::string generate_full_oos_report(const sys::state& state_1, const sys::state& state_2) {
	dcon::load_record record = state_1.world.make_serialize_record_store_mp_checksum_excluded();
	std::string report = generate_oos_report(state_1.world, state_2.world, record);
	report += "HANDWRITTEN CONTRIBUTION\n";
	report += add_collection_compare_to_oos_report<char>(state_1.unit_names, state_1.unit_names, "unit_names") +
		add_collection_compare_to_oos_report<int32_t>(state_1.unit_names_indices, state_2.unit_names_indices, "unit_names_indices") +
		add_compare_to_oos_report(state_1.local_player_nation, state_2.local_player_nation, "local_player_nation") +
		add_compare_to_oos_report(state_1.current_date, state_2.current_date, "current_date") +
		add_compare_to_oos_report(state_1.game_seed, state_2.game_seed, "game_seed") +
		add_compare_to_oos_report(state_1.current_crisis_state, state_2.current_crisis_state, "current_crisis_state") +
		add_collection_compare_to_oos_report<sys::crisis_member_def>(state_1.crisis_participants, state_2.crisis_participants, "crisis_participants") +
		add_compare_to_oos_report(state_1.crisis_temperature, state_2.crisis_temperature, "crisis_temperature") +
		add_compare_to_oos_report(state_1.crisis_attacker, state_2.crisis_attacker, "crisis_attacker") +
		add_compare_to_oos_report(state_1.crisis_defender, state_2.crisis_defender, "crisis_defender") +
		add_compare_to_oos_report(state_1.primary_crisis_attacker, state_2.primary_crisis_attacker, "primary_crisis_attacker") +
		add_compare_to_oos_report(state_1.primary_crisis_defender, state_2.primary_crisis_defender, "primary_crisis_defender") +
		add_compare_to_oos_report(state_1.crisis_state_instance, state_2.crisis_state_instance, "crisis_state_instance") +
		add_compare_to_oos_report(state_1.crisis_last_checked_gp, state_2.crisis_last_checked_gp, "crisis_last_checked_gp") +
		add_compare_to_oos_report(state_1.crisis_war, state_2.crisis_war, "crisis_war") +
		add_compare_to_oos_report(state_1.last_crisis_end_date, state_2.last_crisis_end_date, "last_crisis_end_date") +
		add_collection_compare_to_oos_report<sys::full_wg>(state_1.crisis_defender_wargoals, state_2.crisis_defender_wargoals, "crisis_defender_wargoals") +
		add_collection_compare_to_oos_report<sys::full_wg>(state_1.crisis_attacker_wargoals, state_2.crisis_attacker_wargoals, "crisis_attacker_wargoals") +
		add_compare_to_oos_report(state_1.inflation, state_2.inflation, "inflation") +
		add_collection_compare_to_oos_report<sys::great_nation>(state_1.great_nations, state_2.great_nations, "great_nations") +
		add_collection_compare_to_oos_report<event::pending_human_n_event>(state_1.pending_n_event, state_2.pending_n_event, "pending_n_event") +
		add_collection_compare_to_oos_report<event::pending_human_f_n_event>(state_1.pending_f_n_event, state_2.pending_f_n_event, "pending_f_n_event") +
		add_collection_compare_to_oos_report<event::pending_human_p_event>(state_1.pending_p_event, state_2.pending_p_event, "pending_p_event") +
		add_collection_compare_to_oos_report<event::pending_human_f_p_event>(state_1.pending_f_p_event, state_2.pending_f_p_event, "pending_f_p_event") +
		add_collection_compare_to_oos_report<diplomatic_message::message>(state_1.pending_messages, state_2.pending_messages, "pending_messages") +
		add_collection_compare_to_oos_report<event::pending_human_n_event>(state_1.future_n_event, state_2.future_n_event, "future_n_event") +
		add_collection_compare_to_oos_report<event::pending_human_p_event>(state_1.future_p_event, state_2.future_p_event, "future_p_event") +
		add_collection_compare_to_oos_report<dcon::bitfield_type>(state_1.national_definitions.global_flag_variables, state_2.national_definitions.global_flag_variables, "global_flag_variables") +
		add_compare_to_oos_report(state_1.military_definitions.great_wars_enabled, state_2.military_definitions.great_wars_enabled, "great_wars_enabled") +
		add_compare_to_oos_report(state_1.military_definitions.world_wars_enabled, state_2.military_definitions.world_wars_enabled, "world_wars_enabled");
	return report;
}



void notify_player_is_loading(sys::state& state, dcon::mp_player_id loading_player, bool execute_self) {


	command::command_data c{ command::command_type::notify_player_is_loading , loading_player };

	for(auto& cl : state.network_state.clients) {
		if(!cl.is_active()) {
			continue;
		}
		socket_add_command_to_send_queue(cl.send_buffer, &c);
	}
	if(execute_self) {
		command::execute_command(state, c);
	}
#ifndef NDEBUG
	const auto now = std::chrono::system_clock::now();
	state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + "host:broadcast:cmd | type:notify_player_is_loading playerid:" + std::to_string(loading_player.index()));
#endif
	write_player_nations(state);
}

void player_joins(sys::state& state, client_data& joining_client, dcon::nation_id player_nation) {
	// Tell all clients about this client. Don't include passwords. Execute the command for the host first to create the new player id

	joining_client.player_id = command::execute_notify_player_joins(state, state.local_player_nation, joining_client.hshake_buffer.nickname, joining_client.hshake_buffer.player_password, !state.network_state.is_new_game, player_nation);

	command::command_data c{ command::command_type::notify_player_joins, state.local_player_id };
	command::notify_joins_data payload{ };
	payload.player_name = joining_client.hshake_buffer.nickname;
	payload.needs_loading = !state.network_state.is_new_game;
	payload.player_nation = player_nation;

	c << payload;

	// Do not send it to the client which just joined. That client will receive it via the MP data soon.
	send_network_command(state, [&](const client_data& other_client) { return joining_client.player_id != other_client.player_id; }, c);
#ifndef NDEBUG
	const auto now = std::chrono::system_clock::now();
	state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " host:broadcast:cmd | type:notify_player_joins playerid:" + std::to_string(joining_client.player_id.index()));
#endif
	write_player_nations(state);
}

// loads the save from network which is currently in the save buffer
void load_network_save(sys::state& state, const uint8_t* save_buffer) {
	window::change_cursor(state, window::cursor_type::busy);
	{
		state.yield_ui_lock = true;
		std::unique_lock lock(state.ui_lock);

		std::vector<dcon::nation_id> no_ai_nations;
		for(const auto n : state.world.in_nation)
			if(state.world.nation_get_is_player_controlled(n))
				no_ai_nations.push_back(n);
		dcon::nation_id old_local_player_nation = state.local_player_nation;
		state.local_player_nation = dcon::nation_id{ };
		// Then reload from network
		state.reset_state();
		with_network_decompressed_section(save_buffer, [&state](uint8_t const* ptr_in, uint32_t length) {
			read_save_section(ptr_in, ptr_in + length, state);
		});
		network::set_no_ai_nations_after_reload(state, no_ai_nations, old_local_player_nation);
		state.fill_unsaved_data();
		assert(state.world.nation_get_is_player_controlled(state.local_player_nation));

		state.yield_ui_lock = false;
		lock.unlock();
		state.ui_lock_cv.notify_one();
	}
	window::change_cursor(state, window::cursor_type::normal);
}


void notify_start_game(sys::state& state, network::client_data& client) {
	// notify_start_game

	command::command_data c{ command::command_type::notify_start_game, get_host_player(state) };

	socket_add_command_to_send_queue(client.send_buffer, &c);
#ifndef NDEBUG
	state.console_log("host:send:cmd | (new->start_game) to playerid:" + std::to_string(client.player_id.index()));
#endif
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

	bool paused = false;

	send_mp_data(state, [&](const client_data& other_client) { return client.player_id == other_client.player_id; });

	if(state.current_scene.starting_scene) {
		/* Lobby - existing savegame */
		if(!state.network_state.is_new_game) {
			// compare the last save checksum with the current one, if it dosent match, write new save into the buffer
			if(!state.network_state.last_save_checksum.is_equal(state.get_save_checksum())) {
				network::write_network_save(state);
			}
			// load the save which was just written
			load_network_save(state, state.network_state.current_save_buffer.get());
			// generate checksum for the entire mp state
			state.network_state.current_mp_state_checksum = state.get_mp_state_checksum();
			send_savegame(state, [&](const client_data& other_client) { return client.player_id == other_client.player_id; });
			command::command_data reload_cmd{ command::command_type::notify_reload, state.local_player_id };
			command::notify_reload_data reload_payload{ state.network_state.current_mp_state_checksum };
			reload_cmd << reload_payload;
			send_network_command(state, [&](const client_data& other_client) {return client.player_id != other_client.player_id; }, reload_cmd);
		}

	} else if(state.current_scene.game_in_progress) {
		if(!state.network_state.is_new_game) {
			paused = pause_game(state);
			// compare the last save checksum with the current one, if it dosent match, write new save into the buffer
			if(!state.network_state.last_save_checksum.is_equal( state.get_save_checksum())) {
				network::write_network_save(state);
			}
			// load the save which was just written
			load_network_save(state, state.network_state.current_save_buffer.get());
			// generate checksum for the entire mp state
			state.network_state.current_mp_state_checksum = state.get_mp_state_checksum();
			send_savegame(state, [&](const client_data& other_client) { return client.player_id == other_client.player_id; });
			command::command_data reload_cmd{ command::command_type::notify_reload, state.local_player_id };
			command::notify_reload_data reload_payload{ state.network_state.current_mp_state_checksum };
			reload_cmd << reload_payload;
			send_network_command(state, [&](const client_data& other_client) {return client.player_id != other_client.player_id; }, reload_cmd );
		}

		notify_start_game(state, client);
	}

	auto old_size = client.send_buffer.size();
	client.send_buffer.resize(old_size + tmp.size());
	std::memcpy(client.send_buffer.data() + old_size, tmp.data(), tmp.size());

	if(paused) {
		unpause_game(state);
	}
}

void full_reset_after_oos(sys::state& state) {
	pause_game(state);
	window::change_cursor(state, window::cursor_type::busy);
#ifndef NDEBUG
	state.console_log("host:full_reset_after_oos");
	network::log_player_nations(state);
#endif
	// notfy every client that every client is now loading (reloading or loading the save)
	for(auto& loading_client : state.network_state.clients) {
		if(loading_client.is_active()) {
			network::notify_player_is_loading(state, loading_client.player_id, true);
		}
	}
	{
		// update UI signal so it displays everyone as loading
		state.game_state_updated.store(true, std::memory_order_release);

		// if the save state has changed, write a new network save
		if(!state.network_state.last_save_checksum.is_equal(state.get_save_checksum())) {
			network::write_network_save(state);
		}

		load_network_save(state, state.network_state.current_save_buffer.get());

		// generate checksum for the entire mp state
		state.network_state.current_mp_state_checksum = state.get_mp_state_checksum();

	}
	{

		// send MP data and savegame to oos'd clients. Reload will happen automatically together with saveloading
		send_mp_data(state, [&](const client_data& other_client) { return state.world.mp_player_get_is_oos(other_client.player_id) == true; });
		send_savegame(state, [&](const client_data& other_client) { return state.world.mp_player_get_is_oos(other_client.player_id) == true; });

		command::command_data reload_cmd{ command::command_type::notify_reload, state.local_player_id };
		command::notify_reload_data reload_payload{ state.network_state.current_mp_state_checksum };

		reload_cmd << reload_payload;

		send_network_command(state, [&](const client_data& other_client) {return state.world.mp_player_get_is_oos(other_client.player_id) == false; }, reload_cmd);
	}
	{

		// send message to everyone letting them know that the lobby has been resync'd
		command::chat_message(state, state.local_player_nation, text::produce_simple_string(state, "alice_host_has_resync"), dcon::nation_id{ });
	}
	window::change_cursor(state, window::cursor_type::normal);

}

int server_process_handshake(sys::state& state, network::client_data& client) {
	auto r = socket_recv(client.socket_fd, &client.hshake_buffer, sizeof(client.hshake_buffer), &client.recv_count, [&]() {
#ifndef NDEBUG
		const auto now = std::chrono::system_clock::now();
		state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " host:recv:handshake | nickname: " + client.hshake_buffer.nickname.to_string());
#endif
		// Check lobby password
		if(std::memcmp(client.hshake_buffer.lobby_password, state.network_state.lobby_password, sizeof(state.network_state.lobby_password)) != 0) {
			disconnect_client(state, client, false);
			return;
		}

		// Don't allow two players with the same nickname
		for(auto& c : state.network_state.clients) {
			if(!c.is_active()) {
				continue;
			}

			if(c.hshake_buffer.nickname.to_string_view() == client.hshake_buffer.nickname.to_string_view() && c.socket_fd != client.socket_fd) {
				disconnect_client(state, client, false);
				return;
			}
		}

		// Check player password
		for(auto pl : state.world.in_mp_player) {
			auto nickname_1 = sys::player_name{ pl.get_nickname() }.to_string();
			auto nickname_2 = client.hshake_buffer.nickname.to_string();
			auto hash_1 = sys::player_password_hash{ pl.get_password_hash() };
			auto password_2 = client.hshake_buffer.player_password.to_string();
			auto salt = sys::player_password_salt{ pl.get_password_salt() }.to_string();
			auto hash_2 = sys::player_password_hash{}.from_string_view(sha512.hash(password_2 + salt));

			// If no password is set we allow player to set new password with this connection
			if(nickname_1 == nickname_2 && !hash_1.empty() && hash_1.to_string() != hash_2.to_string()) {
				disconnect_client(state, client, false);
				return;
			}
			else if(nickname_1 == nickname_2) {
				break;
			}
		}

		server_send_handshake(state, client);
		send_post_handshake_commands(state, client);
		state.game_state_updated.store(true, std::memory_order::release);
			});

	return r;
}

int server_process_client_commands(sys::state& state, network::client_data& client) {
	int r = socket_recv_command<sys::network_mode_type::host>(client.socket_fd, &client.recv_buffer, &client.recv_count, &client.receiving_payload_flag , [&]() {
		switch(client.recv_buffer.header.type) {
			// client can notify the host that they are loaded without needing to check the num of clients loading
		case command::command_type::notify_player_fully_loaded:
			if(client.recv_buffer.header.player_id == client.player_id) {
				state.network_state.outgoing_commands.push(client.recv_buffer);
			}
			break;
		default:
			/* Has to be from the client proper and no clients must be currently loading */
			if(client.recv_buffer.header.player_id == client.player_id
			&& !network::check_any_players_loading(state)) {
				state.network_state.outgoing_commands.push(client.recv_buffer);
			}
			break;
		}
#ifndef NDEBUG
		const auto now = std::chrono::system_clock::now();
		state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " host:recv:client_cmd | from playerid:" + std::to_string(client.player_id.index()) + " type:" + readableCommandTypes[uint32_t(client.recv_buffer.header.type)]);
#endif
			});

	return r;
}

static void receive_from_clients(sys::state& state) {

	for(auto& client : state.network_state.clients) {
		if(!client.is_active())
			continue;
		int r = 0;
		if(client.handshake) {
			r = server_process_handshake(state, client);
			if(r > 0) { // error
#if !defined(NDEBUG) && defined(_WIN32)
				state.console_log("host:disconnect | in-receive err=" + std::to_string(int32_t(r)) + "::" + get_last_error_msg() + " from playerid:" + std::to_string(client.player_id.index()));
#endif
				network::disconnect_client(state, client, false);
			}

			return;
		}

		int commandspertick = 0;
		while(r == 0 && commandspertick < 10) {
			r = server_process_client_commands(state, client);
			commandspertick++;
		}

		if(r > 0) { // error
#if !defined(NDEBUG) && defined(_WIN32)
			state.console_log("host:disconnect | in-receive err=" + std::to_string(int32_t(r)) + "::" + get_last_error_msg() + " from playerid:" + std::to_string(client.player_id.index()));
#endif
			network::disconnect_client(state, client, false);
		}
	}
}

bool pause_game(sys::state& state) {
	state.console_log("Pausing the game");

	if(state.actual_game_speed == 0) {
		return false;
	}

	state.ui_state.held_game_speed = state.actual_game_speed.load();
	state.actual_game_speed = 0;
	if(state.network_mode == sys::network_mode_type::host) {
		command::notify_pause_game(state, state.local_player_nation);
	}
	return true;
}

bool unpause_game(sys::state& state) {
	state.console_log("Unpausing the game");

	state.actual_game_speed = state.ui_state.held_game_speed;
	return true;
}

void write_network_save(sys::state& state) {
	/* A save lock will be set when we load a save, naturally loading a save implies
	that we have done preload/fill_unsaved so we will skip doing that again, to save a
	bit of sanity on our miserable CPU */
	size_t length = sizeof_save_section(state);
	auto save_buffer = std::unique_ptr<uint8_t[]>(new uint8_t[length]);
	/* Clear the player nation since it is part of the savegame */
	write_save_section(save_buffer.get(), state); //writeoff data
	// this is an upper bound, since compacting the data may require less space
	state.network_state.current_save_buffer.reset(new uint8_t[ZSTD_compressBound(length) + sizeof(uint32_t) * 2]);
	auto buffer_position = write_network_compressed_section(state.network_state.current_save_buffer.get(), save_buffer.get(), uint32_t(length));
	state.network_state.current_save_length = uint32_t(buffer_position - state.network_state.current_save_buffer.get());
	state.network_state.last_save_checksum = state.get_save_checksum();
	//state.network_state.current_mp_state_checksum = state.get_mp_state_checksum();

}

void broadcast_save_to_clients(sys::state& state) {
	send_savegame(state, [](const client_data& d) {return true; });
}
void broadcast_save_to_single_client(sys::state& state, command::command_data& c, client_data& client, uint8_t const* buffer) {
	assert(c.header.type == command::command_type::notify_save_loaded);
	auto& payload = c.get_payload<command::notify_save_loaded_data>();
	/* And then we have to first send the command payload itself */
	client.save_stream_size = size_t(payload.length);
	socket_add_command_to_send_queue(client.send_buffer, &c);
	/* And then the bulk payload! */
	client.save_stream_offset = client.total_sent_bytes + client.send_buffer.size();
	socket_add_to_send_queue(client.send_buffer, buffer, size_t(payload.length));
#ifndef NDEBUG
	const auto now = std::chrono::system_clock::now();
	state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now)  + " host:send:save | to playerid" + std::to_string(client.player_id.index()) + " len: " + std::to_string(uint32_t(payload.length)));
#endif
}

void broadcast_to_clients(sys::state& state, command::command_data& c) {
	if(c.header.type == command::command_type::save_game)
		return;
	assert(c.header.type != command::command_type::notify_save_loaded);

	if(c.header.type == command::command_type::notify_player_joins) {
		auto& payload = c.get_payload<command::notify_joins_data>();
		payload.player_password = sys::player_password_raw{}; // Never send password to clients
	}
	/* Propagate to all the clients */
	for(auto& client : state.network_state.clients) {
		if(client.is_active()) {
			socket_add_command_to_send_queue(client.send_buffer, &c);
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
		client.last_seen = state.current_date;
		if(client.is_banned(state)) {
			disconnect_client(state, client, false);
			break;
		}
		if(state.current_scene.final_scene) {
			disconnect_client(state, client, false);
			break;
		}
		return;
	}
}

void send_and_receive_commands(sys::state& state) {



	if(state.network_state.finished)
		return;

	bool command_executed = false;
	if(state.network_mode == sys::network_mode_type::host) {

		/* An issue that arose in multiplayer is that the UI was loading the savefile
	   directly, while the game state loop was running, this was fine with the
	   assumption that commands weren't executed while the save was being loaded
	   HOWEVER in multiplayer this is often the case, so we have to block all
	   commands until the savefile is finished loading
	   This way, we're able to effectively and safely queue commands until we
	   can receive them AFTER loading the savefile.
	   The slock is only locked here in the host section as it is redundant to lock it in singleplayer, and clients in a mp game cannot load the save via the ui anyway */



			accept_new_clients(state); // accept new connections
			receive_from_clients(state); // receive new commands

			// send the commands of the server to all the clients
			auto* c = state.network_state.outgoing_commands.front();
			while(c) {
				if(!command::is_console_command(c->header.type)) {
					// Generate checksum on the spot
					if(c->header.type == command::command_type::advance_tick) {
						if(state.current_date.to_ymd(state.start_date).day == 1 || state.cheat_data.daily_oos_check) {
							auto& payload = c->get_payload<command::advance_tick_data>();
							payload.checksum = state.get_mp_state_checksum();
						}
					}
#ifndef NDEBUG
					const auto now = std::chrono::system_clock::now();
					state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " host:send:cmd | from " + std::to_string(c->header.player_id.index()) + " type:" + readableCommandTypes[(uint32_t(c->header.type))]);
#endif
					// if the command could not be performed on the host, don't bother sending it to the clients. Also check if command is supposed to be broadcast
					if(command::execute_command(state, *c) && command::should_broadcast_command(state, *c)) {
						broadcast_to_clients(state, *c);
					}
					command_executed = true;
				}
				state.network_state.outgoing_commands.pop();
				c = state.network_state.outgoing_commands.front();
			}

			// Clear lost sockets
			if(state.current_date.to_ymd(state.start_date).day == 1 || state.cheat_data.daily_oos_check) {
				for(auto& client : state.network_state.clients) {
					if(!client.is_active())
						continue;

					// Drop lost clients
					if(state.current_scene.game_in_progress && state.current_date.value > state.host_settings.alice_lagging_behind_days_to_drop && state.current_date.value - client.last_seen.value > state.host_settings.alice_lagging_behind_days_to_drop) {
						disconnect_client(state, client, true);
					}
					// Slow down for the lagging ones
					else if(state.current_scene.game_in_progress && state.current_date.value > state.host_settings.alice_lagging_behind_days_to_slow_down && state.current_date.value - client.last_seen.value > state.host_settings.alice_lagging_behind_days_to_slow_down) {
						state.actual_game_speed = std::clamp(state.actual_game_speed - 1, 1, 4);
					}
				}
			}

			for(auto& client : state.network_state.clients) {
				if(!client.is_active())
					continue;
				if(client.early_send_buffer.size() > 0) {
					size_t old_size = client.early_send_buffer.size();
					int r = socket_send(client.socket_fd, client.early_send_buffer);
					if(r > 0) { // error
#if !defined(NDEBUG) && defined(_WIN32)
						const auto now = std::chrono::system_clock::now();
						state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " host:disconnect | in-send-EARLY err=" + std::to_string(int32_t(r)) + "::" + get_last_error_msg());
#endif
						disconnect_client(state, client, false);
						continue;
					}
					client.total_sent_bytes += old_size - client.early_send_buffer.size();
#ifndef NDEBUG
					if(old_size != client.early_send_buffer.size()) {
						const auto now = std::chrono::system_clock::now();
						state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " host:send:stats | [EARLY] to playerid:" + std::to_string(client.player_id.index()) + "total:" + std::to_string(uint32_t(client.total_sent_bytes)) + " bytes");
					}

#endif
				} else if(client.send_buffer.size() > 0) {
					size_t old_size = client.send_buffer.size();
					int r = socket_send(client.socket_fd, client.send_buffer);
					if(r > 0) { // error
#if !defined(NDEBUG) && defined(_WIN32)
						const auto now = std::chrono::system_clock::now();
						state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " host:disconnect | in-send-INGAME err=" + std::to_string(int32_t(r)) + "::" + get_last_error_msg());
#endif
						disconnect_client(state, client, false);
						continue;
					}
					client.total_sent_bytes += old_size - client.send_buffer.size();
#ifndef NDEBUG
					if(old_size != client.send_buffer.size()) {
						const auto now = std::chrono::system_clock::now();
						state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " host:send:stats | [SEND] to playerid:" + std::to_string(client.player_id.index()) + "total:" + std::to_string(uint32_t(client.total_sent_bytes)) + " bytes");
					}

#endif
				}
			}

	} else if(state.network_mode == sys::network_mode_type::client) {
		if(state.network_state.handshake) {
			int r = client_process_handshake(state);
			if(r > 0) { // error
				ui::popup_error_window(state, "Network Error", "Network client handshake receive error: " + get_last_error_msg());
				network::finish(state, false);
				return;
			}
		} else if(state.network_state.save_stream) {
			int r = socket_recv(state.network_state.socket_fd, state.network_state.save_data.data(), state.network_state.save_data.size(),
				&state.network_state.recv_count, [&]() {
#ifndef NDEBUG
				const auto now = std::chrono::system_clock::now();
				state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " client:recv:save | len=" + std::to_string(uint32_t(state.network_state.save_data.size())));
#endif
				load_network_save(state, state.network_state.save_data.data());
				auto mp_state_checksum = state.get_mp_state_checksum();

#ifndef NDEBUG
				assert(mp_state_checksum.is_equal(state.session_host_checksum));
				const auto noww = std::chrono::system_clock::now();
				state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", noww) + " client:loadsave | checksum:" + sha512.hash(state.session_host_checksum.to_char()) + "| localchecksum: " + sha512.hash(mp_state_checksum.to_char()));
				log_player_nations(state);
#endif

				state.railroad_built.store(true, std::memory_order::release);
				state.game_state_updated.store(true, std::memory_order::release);
				state.map_state.unhandled_province_selection = true;
				state.sprawl_update_requested.store(true, std::memory_order::release);
				state.network_state.save_data.clear();
				state.network_state.save_stream = false; // go back to normal command loop stuff
				// check that the client gamestate is equal to the gamestate of the host, otherwise oos
				if(!mp_state_checksum.is_equal(state.session_host_checksum)) {
					state.network_state.out_of_sync = true;
				}
				command::notify_player_fully_loaded(state, state.local_player_nation); // notify that we are loaded and ready to start

			});
			if(r > 0) { // error
				ui::popup_error_window(state, "Network Error", "Network client save stream receive error: " + get_last_error_msg());
				network::finish(state, false);
				return;
			}
		} else {
			// receive commands from the server and immediately execute them
			int r = socket_recv_command<sys::network_mode_type::client>(state.network_state.socket_fd, &state.network_state.recv_buffer, &state.network_state.recv_count, &state.network_state.receiving_payload_flag, [&]() {

#ifndef NDEBUG
				const auto now = std::chrono::system_clock::now();
				state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " client:recv:cmd | from:" + std::to_string(state.network_state.recv_buffer.header.player_id.index()) + "type:" + readableCommandTypes[uint32_t(state.network_state.recv_buffer.header.type)]);
#endif

				command::execute_command(state, state.network_state.recv_buffer);
				command_executed = true;

			});
			if(r > 0) { // error
				ui::popup_error_window(state, "Network Error", "Network client command receive error: " + get_last_error_msg());
				network::finish(state, false);
				return;
			}
			// send the outgoing commands to the server and flush the entire queue
			auto* c = state.network_state.outgoing_commands.front();
			while(c) {
#ifndef NDEBUG
				const auto now = std::chrono::system_clock::now();
				state.console_log(format("{:%d-%m-%Y %H:%M:%OS}", now) + " client:send:cmd | type:" + readableCommandTypes[uint32_t(c->header.type)]);
#endif
				if(c->header.type == command::command_type::save_game) {
					command::execute_command(state, *c);
					command_executed = true;
				} else {
					socket_add_command_to_send_queue(state.network_state.send_buffer, c);
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
					if(c->header.type == command::command_type::save_game) {
						command::execute_command(state, *c);
					} else {
						socket_add_command_to_send_queue(state.network_state.send_buffer, c);
					}
					state.network_state.outgoing_commands.pop();
					c = state.network_state.outgoing_commands.front();
				}
			}

			command::command_data c{ command::command_type::notify_player_leaves, state.local_player_id };

			command::notify_leaves_data payload{  };
			payload.make_ai = (state.host_settings.alice_place_ai_upon_disconnection == 1);

			c << payload;

			socket_add_command_to_send_queue(state.network_state.send_buffer, &c);
#ifndef NDEBUG
			state.console_log("client:send:cmd | type:notify_player_leaves");
#endif
			while(state.network_state.send_buffer.size() > 0) {
				if(socket_send(state.network_state.socket_fd, state.network_state.send_buffer) != 0) { // error
					state.console_log("Network client command send error: " + get_last_error_msg());
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


void kick_player(sys::state& state, client_data& client) {
	socket_shutdown(client.socket_fd);


	clear_socket(state, client);
}

void ban_player(sys::state& state, client_data& client) {
	socket_shutdown(client.socket_fd);


	clear_socket(state, client);

	if(state.network_state.as_v6) {
		auto sa = (struct sockaddr_in6*)&client.address;
		state.network_state.v6_banlist.push_back(sa->sin6_addr);
	} else {
		auto sa = (struct sockaddr_in*)&client.address;
		state.network_state.v4_banlist.push_back(sa->sin_addr);
	}
}

void switch_one_player(sys::state& state, dcon::nation_id new_n, dcon::nation_id old_n, dcon::mp_player_id player) {
	assert(old_n == state.world.mp_player_get_nation_from_player_nation(player));
	assert(player);
	state.world.force_create_player_nation(new_n, player);
	state.world.nation_set_is_player_controlled(new_n, true);
	if(!nation_has_any_players_on_it(state, old_n)) {
		state.world.nation_set_is_player_controlled(old_n, false);
	}

	if(state.network_mode == sys::network_mode_type::host) {
		write_player_nations(state);
	}

	if(state.local_player_id == player) {
		state.set_local_player_nation(new_n);
	}

	// We will also re-assign all chat messages from this nation to the new one
	for(auto& msg : state.ui_state.chat_messages)
		if(bool(msg.source) && msg.source == old_n && state.world.mp_player_get_nickname(player) == msg.get_sender_name())
			msg.source = new_n;

	if(state.current_scene.game_in_progress) {
		// give back units if puppet becomes player controlled. This is also done when the game starts and goes from lobby to game in progress
		if(bool(state.world.nation_get_overlord_as_subject(new_n)) && state.world.nation_get_overlord_commanding_units(new_n)) {
			military::give_back_units(state, new_n);
		}
	}

}


void load_host_settings(sys::state& state) {
	auto settings_location = simple_fs::get_or_create_settings_directory();
	auto settings_file = open_file(settings_location, NATIVE("host_settings.json"));
	if(settings_file) {
		auto content = view_contents(*settings_file);
		json data = json::parse(content.data);

		if(!data["alice_expose_webui"].empty()) \
			state.host_settings.alice_expose_webui = data["alice_expose_webui"];

#define HS_LOAD(x, y) \
if(!data[x].empty()) \
state.host_settings.y = data[x]

		HS_LOAD("alice_expose_webui", alice_expose_webui);
		HS_LOAD("alice_lagging_behind_days_to_drop", alice_lagging_behind_days_to_drop);
		HS_LOAD("alice_persistent_server_mode", alice_persistent_server_mode);
		HS_LOAD("alice_place_ai_upon_disconnection", alice_place_ai_upon_disconnection);
		HS_LOAD("alice_persistent_server_pause", alice_persistent_server_pause);
		HS_LOAD("alice_persistent_server_unpause", alice_persistent_server_unpause);
		HS_LOAD("alice_host_port", alice_host_port);
	}
}

// Used primarily to create JSON file with default values
void save_host_settings(sys::state& state) {
	auto settings_location = simple_fs::get_or_create_settings_directory();
	auto settings_file = open_file(settings_location, NATIVE("host_settings.json"));
	if(!settings_file) {

#define HS_SAVE(x, y) \
data[x] = state.host_settings.y

		json data = json::object();

		HS_SAVE("alice_expose_webui", alice_expose_webui);
		HS_SAVE("alice_lagging_behind_days_to_drop", alice_lagging_behind_days_to_drop);
		HS_SAVE("alice_persistent_server_mode", alice_persistent_server_mode);
		HS_SAVE("alice_place_ai_upon_disconnection", alice_place_ai_upon_disconnection);
		HS_SAVE("alice_persistent_server_pause", alice_persistent_server_pause);
		HS_SAVE("alice_persistent_server_unpause", alice_persistent_server_unpause);
		HS_SAVE("alice_host_port", alice_host_port);

		std::string res = data.dump();

		simple_fs::write_file(settings_location, NATIVE("host_settings.json"), res.data(), uint32_t(res.length()));
	}
}

}
