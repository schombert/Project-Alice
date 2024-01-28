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
#include <string_view>
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

static socket_t socket_init_server(struct sockaddr_in& server_address) {
	socket_t socket_fd = static_cast<socket_t>(socket(AF_INET, SOCK_STREAM, 0));

#ifdef _WIN64
	if(socket_fd == static_cast<socket_t>(INVALID_SOCKET))
		std::abort();
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
		MessageBoxA(NULL, ("Network setsockopt error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
		std::abort();
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		MessageBoxA(NULL, ("Network setsockopt [rcvtimeo] error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
		std::abort();
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		MessageBoxA(NULL, ("Network setsockopt [sndtimeo] error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
		std::abort();
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

	struct timeval timeout;
	timeout.tv_sec = 60;
	timeout.tv_usec = 0;
	int opt = 1;
#ifdef _WIN64
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt))) {
		MessageBoxA(NULL, ("Network setsockpt error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
		std::abort();
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		MessageBoxA(NULL, ("Network setsockopt [rcvtimeo] error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
		std::abort();
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		MessageBoxA(NULL, ("Network setsockopt [sndtimeo] error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
		std::abort();
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
	for(auto n : state.nations_by_rank)
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
		return std::find_if(state.network_state.v6_banlist.begin(), state.network_state.v6_banlist.end(), [&](auto const& a) {
			return std::memcmp(&v6_address.sin6_addr, &a, sizeof(a)) == 0;
		}) != state.network_state.v6_banlist.end();
	} else {
		return std::find_if(state.network_state.v4_banlist.begin(), state.network_state.v4_banlist.end(), [&](auto const& a) {
			return std::memcmp(&v4_address.sin_addr, &a, sizeof(a)) == 0;
		}) != state.network_state.v4_banlist.end();
	}
}

static void send_post_handshake_commands(sys::state& state, network::client_data& client) {
	std::vector<char> tmp = client.send_buffer;
	client.send_buffer.clear();
	if(state.mode == sys::game_mode_type::pick_nation) {
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
	} else if(state.mode == sys::game_mode_type::in_game || state.mode == sys::game_mode_type::select_states) {
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
					disconnect_client(state, client);
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
				case command::command_type::notify_player_leaves:
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
			state.console_log("host:disconnect: in-receive err=" + std::to_string(int32_t(r)) + "::" + get_wsa_error_text(WSAGetLastError()));
#endif
			network::disconnect_client(state, client);
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
		if(state.mode == sys::game_mode_type::end_screen) {
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
#ifndef NDEBUG //Debug - daily oos check
					c->data.advance_tick.checksum = state.get_save_checksum();
#else //Release - monthly oos check
					if(state.current_date.to_ymd(state.start_date).day == 1) {
						c->data.advance_tick.checksum = state.get_save_checksum();
					}
#endif
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
						state.console_log("host:disconnect: in-send-EARLY err=" + std::to_string(int32_t(r)) + "::" + get_wsa_error_text(WSAGetLastError()));
#endif
						disconnect_client(state, client);
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
						state.console_log("host:disconnect: in-send-INGAME err=" + std::to_string(int32_t(r)) + "::" + get_wsa_error_text(WSAGetLastError()));
#endif
						disconnect_client(state, client);
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
#ifdef _WIN64
						MessageBoxA(NULL, "Could not find a scenario with a matching checksum! This is most likely a false positive, so just ask the host for their scenario file and it should work. Or you haven't clicked on \"Make scenario\"!", "Network error", MB_OK);
#endif
						std::abort();
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
#ifdef _WIN64
				MessageBoxA(NULL, ("Network client handshake receive error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
				std::abort();
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
					uint32_t save_size = state.network_state.recv_buffer.data.notify_save_loaded.length;
					state.network_state.save_stream = true;
					assert(save_size > 0);
					if(save_size >= 32 * 1000 * 1000) { // 32 MB
#ifdef _WIN64
						MessageBoxA(NULL, "Network client save stream too big", "Network error", MB_OK);
#endif
						std::abort();
					}
					state.network_state.save_data.resize(static_cast<size_t>(save_size));
				}
#ifndef NDEBUG
				state.console_log("client:recv:cmd: " + std::to_string(uint32_t(state.network_state.recv_buffer.type)));
#endif
			});
			if(r != 0) { // error
#ifdef _WIN64
				MessageBoxA(NULL, ("Network client command receive error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
				std::abort();
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
#ifdef _WIN64
				MessageBoxA(NULL, ("Network client command send error: " + get_wsa_error_text(WSAGetLastError())).c_str(), "Network error", MB_OK);
#endif
				std::abort();
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

void finish(sys::state& state) {
	if(state.network_mode == sys::network_mode_type::single_player)
		return; // Do nothing in singleplayer
	
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
		state.network_state.v6_banlist.push_back(client.v6_address.sin6_addr);
	} else {
		state.network_state.v4_banlist.push_back(client.v4_address.sin_addr);
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
