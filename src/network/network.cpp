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

namespace network {

//
// platform specific
//

static int socket_recv(socket_t client_fd, void *data, size_t n) {
#ifdef _WIN64
	u_long has_pending = 0;
	auto r = ioctlsocket(client_fd, FIONREAD, &has_pending);
	if(has_pending)
		return (int)recv(client_fd, (char *)data, (int)n, 0);
	return 0;
#else
	return recv(client_fd, data, n, MSG_DONTWAIT);
#endif
}

static int socket_send(socket_t client_fd, const void *data, size_t n) {
#ifdef _WIN64
	return (int)send(client_fd, (const char *)data, (int)n, 0);
#else
	return send(client_fd, data, n, MSG_NOSIGNAL);
#endif
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
	socket_t socket_fd = (socket_t)socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0)
		std::abort();
	int opt = 1;
#ifdef _WIN64
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)))
		std::abort();
#else
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
		std::abort();
#endif
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(default_server_port);
	if(bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
		std::abort();
	if(listen(socket_fd, 3) < 0)
		std::abort();
#ifdef _WIN64
	u_long mode = 1; // 1 to enable non-blocking socket
	ioctlsocket(socket_fd, FIONBIO, &mode);
#endif
	return socket_fd;
}

static socket_t socket_init_server(struct sockaddr_in6& server_address) {
	socket_t socket_fd = (socket_t)socket(AF_INET6, SOCK_STREAM, 0);
	if(socket_fd < 0)
		std::abort();
	int opt = 1;
#ifdef _WIN64
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)))
		std::abort();
#else
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
		std::abort();
#endif
	server_address.sin6_addr = IN6ADDR_ANY_INIT;
	server_address.sin6_family = AF_INET6;
	server_address.sin6_port = htons(default_server_port);
	if(bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
		std::abort();
	if(listen(socket_fd, 3) < 0)
		std::abort();
#ifdef _WIN64
	u_long mode = 1; // 1 to enable non-blocking socket
	ioctlsocket(socket_fd, FIONBIO, &mode);
#endif
	return socket_fd;
}

static socket_t socket_init_client(struct sockaddr_in& client_address, const char *ip_address) {
	socket_t socket_fd = (socket_t)socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0)
		std::abort();
	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(default_server_port);
	if(inet_pton(AF_INET, ip_address, &client_address.sin_addr) <= 0) //ipv4 fallback
		std::abort();
	if(connect(socket_fd, (struct sockaddr*)&client_address, sizeof(client_address)) < 0)
		std::abort();
	return socket_fd;
}

static socket_t socket_init_client(struct sockaddr_in6& client_address, const char *ip_address) {
	socket_t socket_fd = (socket_t)socket(AF_INET6, SOCK_STREAM, 0);
	if(socket_fd < 0)
		std::abort();
	client_address.sin6_addr = IN6ADDR_ANY_INIT;
	client_address.sin6_family = AF_INET6;
	client_address.sin6_port = htons(default_server_port);
	if(inet_pton(AF_INET6, ip_address, &client_address.sin6_addr) <= 0) //ipv4 fallback
		std::abort();
	if(connect(socket_fd, (struct sockaddr*)&client_address, sizeof(client_address)) < 0)
		std::abort();
	return socket_fd;
}

//
// non-platform specific
//

void init(sys::state& state) {
	if(state.network_mode == sys::network_mode_type::single_player)
		return; // Do nothing in singleplayer

#ifdef _WIN64
    WSADATA data;
    if(WSAStartup(MAKEWORD(2, 2), &data) != 0)
		std::abort();
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
}

static void disconnect_client(sys::state& state, client_data& client) {
	command::notify_player_leaves(state, client.playing_as);
	socket_shutdown(client.socket_fd);
	client.socket_fd = 0;
}

static void accept_new_clients(sys::state& state) {
	// Check if any new clients are to join us
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
				// enforce bans
				if(std::find_if(state.network_state.v6_banlist.begin(), state.network_state.v6_banlist.end(), [&](auto const a) {
					return memcmp(&client.v6_address.sin6_addr, &a, sizeof(a)) == 0;
					}) != state.network_state.v6_banlist.end()) {
					disconnect_client(state, client);
					break;
				}
			} else {
				socklen_t addr_len = sizeof(client.v4_address);
				client.socket_fd = accept(state.network_state.socket_fd, (struct sockaddr*)&client.v4_address, &addr_len);
				// enforce bans
				if(std::find_if(state.network_state.v4_banlist.begin(), state.network_state.v4_banlist.end(), [&](auto const a) {
					return memcmp(&client.v4_address.sin_addr, &a, sizeof(a)) == 0;
					}) != state.network_state.v4_banlist.end()) {
					disconnect_client(state, client);
					break;
				}
			}

			dcon::nation_id assigned_nation{};
			{
				// give the client a "joining" nation, basically a temporal nation choosen
				// "randomly" that is tied to the client iself
				state.world.for_each_nation([&](dcon::nation_id n) {
					if(!state.world.nation_get_is_player_controlled(n) && state.world.nation_get_owned_province_count(n) > 0) {
						assigned_nation = n;
					}
				});
				assert(bool(assigned_nation));
				client.playing_as = assigned_nation;
				state.world.nation_set_is_player_controlled(assigned_nation, false);

				command::payload c;
				c.type = command::command_type::notify_player_picks_nation;
				c.source = dcon::nation_id{};
				c.data.nation_pick.target = assigned_nation;
				auto r = socket_send(client.socket_fd, &c, sizeof(c));
				if(r < 0) { // error
					disconnect_client(state, client);
				}
			}
			{ //tell the client general information about the game
				command::payload c;
				c.type = command::command_type::update_session_info;
				c.source = dcon::nation_id{};
				c.data.update_session_info.seed = state.game_seed;
				c.data.update_session_info.checksum = state.get_save_checksum();
				auto r = socket_send(client.socket_fd, &c, sizeof(c));
				if(r < 0) { // error
					disconnect_client(state, client);
				}
			}
			{
				// Tell all the other clients that we have joined
				command::payload c;
				c.type = command::command_type::notify_player_joins;
				c.source = assigned_nation;
				state.network_state.outgoing_commands.push(c);
			}

			// notify the client of all current players
			state.world.for_each_nation([&](dcon::nation_id n) {
				if(state.world.nation_get_is_player_controlled(n)) {
					command::payload c;
					c.type = command::command_type::notify_player_joins;
					c.source = n;
					auto r = socket_send(client.socket_fd, &c, sizeof(c));
					if(r < 0) { // error
						disconnect_client(state, client);
					}
				}
			});
			return;
		}
	}
}

static void receive_from_clients(sys::state& state) {
	for(auto& client : state.network_state.clients) {
		if(client.is_active()) {
			command::payload cmd{};
			auto r = socket_recv(client.socket_fd, &cmd, sizeof(cmd));
			if(r == sizeof(cmd)) { // got command
				switch(cmd.type) {
				case command::command_type::invalid:
				case command::command_type::notify_player_joins:
					break; // has to be valid/sendable by client
				case command::command_type::notify_player_picks_nation:
					if(command::can_notify_player_picks_nation(state, cmd.source, cmd.data.nation_pick.target)) {
						client.playing_as = cmd.data.nation_pick.target;
					}
					state.network_state.outgoing_commands.push(cmd);
					break;
				default:
					state.network_state.outgoing_commands.push(cmd);
					break;
				}
			} else if(r < 0) { // error
				disconnect_client(state, client);
			}
		}
	}
}

static void broadcast_to_clients(sys::state& state, command::payload& c) {
	// propagate to all the clients
	for(auto& client : state.network_state.clients) {
		if(client.is_active()) {
			auto r = socket_send(client.socket_fd, &c, sizeof(c));
			if(r < 0) { // error
				disconnect_client(state, client);
			}
		}
	}
}

void send_and_receive_commands(sys::state& state) {
	bool command_executed = false;
	if(state.network_mode == sys::network_mode_type::host) {
		accept_new_clients(state); // accept new connections
		receive_from_clients(state); // receive new commands
		// send the commands of the server to all the clients
		auto* c = state.network_state.outgoing_commands.front();
		while(c) {
			bool valid = true;
			if((c->type == command::command_type::notify_player_ban || c->type == command::command_type::notify_player_kick)) {
				valid = (c->source == state.local_player_nation);
			}
			if(command::is_console_command(c->type) == false && valid) {
				broadcast_to_clients(state, *c);
				command::execute_command(state, *c);
				command_executed = true;
			}
			state.network_state.outgoing_commands.pop();
			c = state.network_state.outgoing_commands.front();
		}
	} else if(state.network_mode == sys::network_mode_type::client) {
		// receive commands from the server and immediately execute them
		while(1) {
			command::payload cmd{};
			int r = socket_recv(state.network_state.socket_fd, &cmd, sizeof(cmd));
			if(r == sizeof(cmd)) { // got command
				command::execute_command(state, cmd);
				command_executed = true;
			} else if(r == 0) { // nothing
				break;
			} else if(r < 0) { // error
#ifdef _WIN64
				MessageBoxA(NULL, "Network client receive error", "Network error", MB_OK);
#endif
				std::abort();
			}
		}
		// send the outgoing commands to the server and flush the entire queue
		auto* c = state.network_state.outgoing_commands.front();
		while(c) {
			int r = socket_send(state.network_state.socket_fd, c, sizeof(*c));
			if(r == sizeof(*c)) { // sent command
				// ...
			} else if(r < 0) { // error
#ifdef _WIN64
				MessageBoxA(NULL, "Network client send error", "Network error", MB_OK);
#endif
				std::abort();
			}
			state.network_state.outgoing_commands.pop();
			c = state.network_state.outgoing_commands.front();
		}
	}

	if(command_executed) {
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

}
