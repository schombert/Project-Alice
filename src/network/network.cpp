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
#else // NIX
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#endif // ...
#include "system_state.hpp"
#include "commands.hpp"
#include "SPSCQueue.h"
#include "network.hpp"
#include "serialization.hpp"

#define ZSTD_STATIC_LINKING_ONLY
#define XXH_NAMESPACE ZSTD_
#include "zstd.h"

namespace network {

//
// platform specific
//

#ifdef _WIN64
static std::string get_wsa_error_text(int err) {
	LPTSTR err_buf = nullptr;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr, err, 0, (LPTSTR)&err_buf, 0, nullptr);
	native_string err_text = err_buf;
	LocalFree(err_buf);
	return std::to_string(err) + " = " + simple_fs::native_to_utf8(err_text);
}
#endif

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
			return -1;
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
			return -1;
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

static socket_t socket_init_server(struct sockaddr_in& server_address) {
	socket_t socket_fd = static_cast<socket_t>(socket(AF_INET, SOCK_STREAM, 0));

#ifdef _WIN64
	if(socket_fd == static_cast<socket_t>(INVALID_SOCKET))
		std::abort();
#else
	if(socket_fd < 0)
		std::abort();
#endif

	int opt = 1;
#ifdef _WIN64
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt))) {
		MessageBoxA(NULL, ("Network setsockopt error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
		std::abort();
	}
#else
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		std::abort();
	}
#endif
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(default_server_port);
	if(bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
#ifdef _WIN64
		MessageBoxA(NULL, ("Network bind error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
		std::abort();
	}
	if(listen(socket_fd, 3) < 0) {
#ifdef _WIN64
		MessageBoxA(NULL, ("Network listen error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
		std::abort();
	}
#ifdef _WIN64
	u_long mode = 1; // 1 to enable non-blocking socket
	ioctlsocket(socket_fd, FIONBIO, &mode);
#endif
	return socket_fd;
}

static socket_t socket_init_server(struct sockaddr_in6& server_address) {
	socket_t socket_fd = static_cast<socket_t>(socket(AF_INET6, SOCK_STREAM, 0));

#ifdef _WIN64
	if(socket_fd == static_cast<socket_t>(INVALID_SOCKET))
		std::abort();
#else
	if(socket_fd < 0)
		std::abort();
#endif

	int opt = 1;
#ifdef _WIN64
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt))) {
		MessageBoxA(NULL, ("Network setsockpt error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
		std::abort();
	}
#else
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		std::abort();
	}
#endif
	server_address.sin6_addr = IN6ADDR_ANY_INIT;
	server_address.sin6_family = AF_INET6;
	server_address.sin6_port = htons(default_server_port);
	if(bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
#ifdef _WIN64
		MessageBoxA(NULL, ("Network bind error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
		std::abort();
	}
	if(listen(socket_fd, 3) < 0) {
#ifdef _WIN64
		MessageBoxA(NULL, ("Network listen error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
		std::abort();
	}
#ifdef _WIN64
	u_long mode = 1; // 1 to enable non-blocking socket
	ioctlsocket(socket_fd, FIONBIO, &mode);
#endif
	return socket_fd;
}

static socket_t socket_init_client(struct sockaddr_in& client_address, const char *ip_address) {
	socket_t socket_fd = static_cast<socket_t>(socket(AF_INET, SOCK_STREAM, 0));

#ifdef _WIN64
	if(socket_fd == static_cast<socket_t>(INVALID_SOCKET)) {
		MessageBoxA(NULL, ("Network socket error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
	}
#else
	if(socket_fd < 0) {
		std::abort();
	}
#endif

	
	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(default_server_port);

	if(inet_pton(AF_INET, ip_address, &client_address.sin_addr) <= 0) { //ipv4 fallback
#ifdef _WIN64
		MessageBoxA(NULL, ("Network inet_pton error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
		std::abort();
	}
	if(connect(socket_fd, (struct sockaddr*)&client_address, sizeof(client_address)) < 0) {
#ifdef _WIN64
		MessageBoxA(NULL, ("Network connect error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
		std::abort();
	}
	return socket_fd;
}

static socket_t socket_init_client(struct sockaddr_in6& client_address, const char* ip_address) {
	socket_t socket_fd = static_cast<socket_t>(socket(AF_INET6, SOCK_STREAM, 0));

#ifdef _WIN64
	if(socket_fd == static_cast<socket_t>(INVALID_SOCKET)) {
		MessageBoxA(NULL, ("Network socket error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
	}
#else
	if(socket_fd < 0) {
		std::abort();
	}
#endif

	client_address.sin6_addr = IN6ADDR_ANY_INIT;
	client_address.sin6_family = AF_INET6;
	client_address.sin6_port = htons(default_server_port);
	if(inet_pton(AF_INET6, ip_address, &client_address.sin6_addr) <= 0) { //ipv4 fallback
#ifdef _WIN64
		MessageBoxA(NULL, ("Network inet_pton error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
		std::abort();
	}
	if(connect(socket_fd, (struct sockaddr*)&client_address, sizeof(client_address)) < 0) {
#ifdef _WIN64
		MessageBoxA(NULL, ("Network connect error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
		std::abort();
	}
	return socket_fd;
}

//
// non-platform specific
//

static dcon::nation_id get_temp_nation(sys::state& state) {
	// give the client a "joining" nation, basically a temporal nation choosen
	// "randomly" that is tied to the client iself
	for(auto n : state.world.in_nation)
		if(!state.world.nation_get_is_player_controlled(n) && state.world.nation_get_owned_province_count(n) > 0)
			return n;
	return dcon::nation_id{ };
}

void init(sys::state& state) {
	if(state.network_mode == sys::network_mode_type::single_player)
		return; // Do nothing in singleplayer

#ifdef _WIN64
    WSADATA data;
	if(WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		MessageBoxA(NULL, ("WSA startup error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
		std::abort();
	}
#endif
	if(state.network_state.as_v6) {
		if(state.network_mode == sys::network_mode_type::host) {
			state.network_state.socket_fd = socket_init_server(state.network_state.v6_address);
		} else {
			assert(state.network_state.ip_address.size() > 0);
			state.network_state.socket_fd = socket_init_client(state.network_state.v6_address, state.network_state.ip_address.c_str());
		}
	} else {
		if(state.network_mode == sys::network_mode_type::host) {
			state.network_state.socket_fd = socket_init_server(state.network_state.v4_address);
		} else {
			assert(state.network_state.ip_address.size() > 0);
			state.network_state.socket_fd = socket_init_client(state.network_state.v4_address, state.network_state.ip_address.c_str());
		}
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

static void disconnect_client(sys::state& state, client_data& client) {
	command::notify_player_leaves(state, client.playing_as);
	socket_shutdown(client.socket_fd);
	client.socket_fd = 0;
	client.send_buffer.clear();
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
		return std::find_if(state.network_state.v6_banlist.begin(), state.network_state.v6_banlist.end(), [&](auto const& a) {
			return std::memcmp(&v6_address.sin6_addr, &a, sizeof(a)) == 0;
		}) != state.network_state.v6_banlist.end();
	} else {
		return std::find_if(state.network_state.v4_banlist.begin(), state.network_state.v4_banlist.end(), [&](auto const& a) {
			return std::memcmp(&v4_address.sin_addr, &a, sizeof(a)) == 0;
		}) != state.network_state.v4_banlist.end();
	}
}

static void receive_from_clients(sys::state& state) {
	for(auto& client : state.network_state.clients) {
		if(client.is_active()) {
			int r = 0;
			if(client.handshake) {
				r = socket_recv(client.socket_fd, &client.hshake_buffer, sizeof(client.hshake_buffer), &client.recv_count, [&]() {
					{ /* Tell everyone else (ourselves + this client) that this client, in fact, has joined */
						command::payload c;
						memset(&c, 0, sizeof(c));
						c.type = command::command_type::notify_player_joins;
						c.source = client.playing_as;
						c.data.player_name = client.hshake_buffer.nickname;
						broadcast_to_clients(state, c);
						command::execute_command(state, c);
					}
					client.handshake = false; /* Exit from handshake mode */
					state.game_state_updated.store(true, std::memory_order::release);
				});
			} else {
				r = socket_recv(client.socket_fd, &client.recv_buffer, sizeof(client.recv_buffer), &client.recv_count, [&]() {
					switch(client.recv_buffer.type) {
					case command::command_type::invalid:
					case command::command_type::notify_player_ban:
					case command::command_type::notify_player_kick:
					case command::command_type::notify_save_loaded:
					case command::command_type::advance_tick:
					case command::command_type::notify_start_game:
						break; // has to be valid/sendable by client
					default:
						/* Has to be from the nation of the client proper */
						if(client.recv_buffer.source == client.playing_as) {
							state.network_state.outgoing_commands.push(client.recv_buffer);
						}
						break;
					}
				});
			}
			if(r < 0) // error
				disconnect_client(state, client);
		}
	}
}

void broadcast_to_clients(sys::state& state, command::payload& c) {
	if(c.type == command::command_type::save_game)
		return;

	/* Propagate to all the clients */
	if(c.type == command::command_type::notify_save_loaded) {
		/* A save lock will be set when we load a save, naturally
		   loading a save implies that we have done preload/fill_unsaved
		   so we will skip doing that again, to save a bit of sanity on
		   our miserable CPU */
		size_t length = sizeof_save_section(state);
		auto save_buffer = std::unique_ptr<uint8_t[]>(new uint8_t[length]);
		write_save_section(save_buffer.get(), state);
		// this is an upper bound, since compacting the data may require less space
		auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[ZSTD_compressBound(length) + sizeof(uint32_t) * 2]);
		auto buffer_position = write_network_compressed_section(buffer.get(), save_buffer.get(), uint32_t(length));
		auto total_size_used = uint32_t(buffer_position - buffer.get());
		/* We need to regenerate the checksum of the save so it's at this specific point */
		c.data.notify_save_loaded.checksum = state.get_save_checksum();
		for(auto& client : state.network_state.clients) {
			if(client.is_active()) {
				bool send_full = (client.playing_as == c.data.notify_save_loaded.target) || (!c.data.notify_save_loaded.target);
				if(send_full && !state.network_state.is_new_game) {
					/* And then we have to first send the command payload itself */
					socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
					/* And then the bulk payload! */
					client.save_stream_offset = client.total_sent_bytes + client.send_buffer.size();
					client.save_stream_size = size_t(total_size_used);
					socket_add_to_send_queue(client.send_buffer, &total_size_used, sizeof(total_size_used));
					socket_add_to_send_queue(client.send_buffer, buffer.get(), size_t(total_size_used));
				}
			}
		}
	} else {
		for(auto& client : state.network_state.clients) {
			if(client.is_active()) {
				socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
			}
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
		if(!client.is_active()) {
			if(state.network_state.as_v6) {
				socklen_t addr_len = sizeof(client.v6_address);
				client.socket_fd = accept(state.network_state.socket_fd, (struct sockaddr*)&client.v6_address, &addr_len);
			} else {
				socklen_t addr_len = sizeof(client.v4_address);
				client.socket_fd = accept(state.network_state.socket_fd, (struct sockaddr*)&client.v4_address, &addr_len);
			}
			if(client.is_banned(state)) {
				disconnect_client(state, client);
				break;
			}
			/* Do not allow players to join mid-session, they have to go back to the lobby */
			if(state.mode == sys::game_mode_type::in_game || state.mode == sys::game_mode_type::select_states) {
				disconnect_client(state, client);
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
				socket_add_to_send_queue(client.send_buffer, &hshake, sizeof(hshake));
			}
			if(!state.network_state.is_new_game) {
				command::payload c;
				memset(&c, 0, sizeof(c));
				c.type = command::command_type::notify_save_loaded;
				c.source = state.local_player_nation;
				c.data.notify_save_loaded.target = client.playing_as;
				broadcast_to_clients(state, c);
			}
			for(const auto n : state.world.in_nation) {
				if(n.get_is_player_controlled()) {
					command::payload c;
					memset(&c, 0, sizeof(c));
					c.type = command::command_type::notify_player_joins;
					c.source = n;
					c.data.player_name = state.network_state.map_of_player_names[n.id.index()];
					socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
				}
			}
			return;
		}
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
					c->data.advance_tick.checksum = state.get_save_checksum();
				}
				broadcast_to_clients(state, *c);
				command::execute_command(state, *c);
				command_executed = true;
			}
			state.network_state.outgoing_commands.pop();
			c = state.network_state.outgoing_commands.front();
		}

		for(auto& client : state.network_state.clients) {
			if(client.is_active()) {
				size_t old_size = client.send_buffer.size();
				if(socket_send(client.socket_fd, client.send_buffer) < 0) { // error
					disconnect_client(state, client);
				}
				client.total_sent_bytes += old_size - client.send_buffer.size();
			}
		}
	} else if(state.network_mode == sys::network_mode_type::client) {
		if(state.network_state.handshake) {
			/* Send our client's handshake */
			int r = socket_recv(state.network_state.socket_fd, &state.network_state.s_hshake, sizeof(state.network_state.s_hshake), &state.network_state.recv_count, [&]() {
				if(!state.scenario_checksum.is_equal(state.network_state.s_hshake.scenario_checksum)) {
#ifdef _WIN64
					//MessageBoxA(NULL, "Scenario is not the same as the host!", "Network error", MB_OK);
#endif
					//std::abort();
				}
				state.session_host_checksum = state.network_state.s_hshake.save_checksum;
				state.game_seed = state.network_state.s_hshake.seed;
				state.local_player_nation = state.network_state.s_hshake.assigned_nation;
				state.world.nation_set_is_player_controlled(state.local_player_nation, true);
				/* Send our client handshake back */
				client_handshake_data hshake;
				hshake.nickname = state.network_state.nickname;
				socket_add_to_send_queue(state.network_state.send_buffer, &hshake, sizeof(hshake));
				state.network_state.handshake = false;
			});
		} else if(state.network_state.save_stream) {
			int r = 0;
			if(state.network_state.save_size == 0) {
				r = socket_recv(state.network_state.socket_fd, &state.network_state.save_size, sizeof(state.network_state.save_size), &state.network_state.recv_count, [&]() {
					if(state.network_state.save_size == 0) { //no save to send (new game)
						state.network_state.save_data.clear();
						state.network_state.save_stream = false;
					} else {
						if(state.network_state.save_size >= 24 * 1000 * 1000) { // 24 mb
#ifdef _WIN64
							MessageBoxA(NULL, "Network client save stream too big", "Network error", MB_OK);
#endif
							std::abort();
						}
						state.network_state.save_data.resize(static_cast<size_t>(state.network_state.save_size));
					}
				});
			} else {
				r = socket_recv(state.network_state.socket_fd, state.network_state.save_data.data(), state.network_state.save_data.size(), &state.network_state.recv_count, [&]() {
					std::vector<dcon::nation_id> players;
					for(const auto n : state.world.in_nation)
						if(state.world.nation_get_is_player_controlled(n))
							players.push_back(n);
					dcon::nation_id old_local_player_nation = state.local_player_nation;
					state.preload();
					with_network_decompressed_section(state.network_state.save_data.data(), [&](uint8_t const* ptr_in, uint32_t length) { read_save_section(ptr_in, ptr_in + length, state); });
					state.local_player_nation = dcon::nation_id{ };
					state.fill_unsaved_data();
					for(const auto n : players)
						state.world.nation_set_is_player_controlled(n, true);
					state.local_player_nation = old_local_player_nation;
					assert(state.world.nation_get_is_player_controlled(state.local_player_nation));
					//
					state.game_state_updated.store(true, std::memory_order::release);
					state.network_state.save_data.clear();
					state.network_state.save_stream = false; // go back to normal command loop stuff
				});
			}
			if(r < 0) { // error
#ifdef _WIN64
				MessageBoxA(NULL, ("Network client save stream receive error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
				std::abort();
			}
		} else {
			// receive commands from the server and immediately execute them
			int r = socket_recv(state.network_state.socket_fd, &state.network_state.recv_buffer, sizeof(state.network_state.recv_buffer), &state.network_state.recv_count, [&]() {
				command::execute_command(state, state.network_state.recv_buffer);
				command_executed = true;
				// start save stream!
				if(state.network_state.recv_buffer.type == command::command_type::notify_save_loaded) {
					state.network_state.save_size = 0;
					state.network_state.save_stream = true;
				}
			});
			if(r < 0) { // error
#ifdef _WIN64
				MessageBoxA(NULL, ("Network client command receive error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
				std::abort();
			}
			// send the outgoing commands to the server and flush the entire queue
			auto* c = state.network_state.outgoing_commands.front();
			while(c) {
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
			if(socket_send(state.network_state.socket_fd, state.network_state.send_buffer) < 0) { // error
#ifdef _WIN64
				MessageBoxA(NULL, ("Network client command send error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
				std::abort();
			}
		}
	}

	if(command_executed) {
		if(state.network_state.out_of_sync && !state.network_state.reported_oos) {
			command::notify_player_oos(state, state.local_player_nation);
			state.network_state.reported_oos = true;
		}
		state.game_state_updated.store(true, std::memory_order::release);
	}
}

void finish(sys::state& state) {
	if(state.network_mode == sys::network_mode_type::single_player)
		return; // Do nothing in singleplayer
	
	socket_shutdown(state.network_state.socket_fd);
#ifdef _WIN64
	WSACleanup();
#endif
}

void ban_player(sys::state& state, client_data& client) {
	if(client.is_active()) {
		socket_shutdown(client.socket_fd);
		client.socket_fd = 0;
		if(state.network_state.as_v6) {
			state.network_state.v6_banlist.push_back(client.v6_address.sin6_addr);
		} else {
			state.network_state.v4_banlist.push_back(client.v4_address.sin_addr);
		}
	}
}

void kick_player(sys::state& state, client_data& client) {
	if(client.is_active()) {
		socket_shutdown(client.socket_fd);
		client.socket_fd = 0;
	}
}

void switch_player(sys::state& state, dcon::nation_id new_n, dcon::nation_id old_n) {
	state.network_state.map_of_player_names.insert_or_assign(new_n.index(), state.network_state.map_of_player_names[old_n.index()]);
	if(state.network_mode == sys::network_mode_type::host) {
		for(auto& client : state.network_state.clients) {
			if(client.is_active()) {
				if(client.playing_as == old_n) {
					client.playing_as = new_n;
				}
			}
		}
	}
}

}
