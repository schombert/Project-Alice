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

#ifdef _WIN64
static int internal_recv(SOCKET client_fd, void *data, size_t n) {
    u_long has_pending = 0;
    auto r = ioctlsocket(client_fd, FIONREAD, &has_pending);
	if(has_pending)
		return (int)recv(client_fd, (char *)data, (int)n, 0);
	return 0;
}
static int internal_send(SOCKET client_fd, const void *data, size_t n) {
	return (int)send(client_fd, (const char *)data, (int)n, 0);
}
#else
static int internal_recv(int client_fd, void *data, size_t n) {
	return recv(client_fd, data, n, MSG_DONTWAIT);
}
static int internal_send(int client_fd, const void *data, size_t n) {
	return send(client_fd, data, n, MSG_NOSIGNAL);
}
#endif

void network_state::init(sys::state& state, bool _as_server, std::string_view ip_addr) {
	as_server = _as_server;

#ifdef _WIN64
    WSADATA data;
    if(WSAStartup(MAKEWORD(2, 2), &data) != 0)
		std::abort();
#endif

	if(as_server) {
		if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			std::abort();
		
#ifdef _WIN64
		int opt = 1;
		if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)))
			std::abort();
#else
		int opt = 1;
		if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
			std::abort();
#endif

		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = INADDR_ANY;
		server_address.sin_port = htons(server_port);
		if(bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
			std::abort();
		if(listen(socket_fd, 3) < 0)
			std::abort();
		
		// TODO: Only one client for now, maybe allow for more???
	} else {
		if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			std::abort();
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(server_port);
		if(inet_pton(AF_INET, std::string(ip_addr).c_str(), &server_address.sin_addr) <= 0)
			std::abort();
		if(connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
			std::abort();
		
		// max speed, no pause (blocked by waiting for server commands however)
		state.actual_game_speed = 5;
		state.ui_pause = false;
	}
}

void network_state::server_client_loop(sys::state& state, int worker_id) {
	if(!is_present() || !as_server)
		return;

	while(state.quit_signaled.load(std::memory_order::acquire) == false) {
		socklen_t addr_len = sizeof(server_address);
#ifdef _WIN64
		SOCKET client_fd;
#else
		int client_fd;
#endif
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(socket_fd, &rfds);
		struct timeval  tv = {};
		tv.tv_sec = 1;
		tv.tv_usec = 0;
#ifdef _WIN64
		if(select(static_cast<SOCKET>(static_cast<int>(socket_fd) + 1), &rfds, NULL, NULL, &tv) <= 0)
			continue;
#else
		if(select(socket_fd + 1, &rfds, NULL, NULL, &tv) <= 0)
			continue;
#endif

		if((client_fd = accept(socket_fd, (struct sockaddr *)&server_address, &addr_len)) < 0)
			continue;
		// TODO: Notify other clients  std::fprintf(stderr, "client connected\n");
		clients[worker_id].active.store(true, std::memory_order_release);
		while(1) {
			// read out until no more data
			while(1) {
				command::payload cmd;
				if(internal_recv(client_fd, &cmd, sizeof(cmd)) != sizeof(cmd))
					break;
				server_commands.push(cmd);
			}
			// send commands to client
			auto* c = clients[worker_id].worker_commands.front();
			while(c) {
				if(internal_send(client_fd, c, sizeof(*c)) != sizeof(*c)) {
					// TODO: Notify other clients
					goto close_finish;
				}
				clients[worker_id].worker_commands.pop();
				c = clients[worker_id].worker_commands.front();
			}
		}
close_finish:
		clients[worker_id].active.store(false, std::memory_order_release);
#ifdef _WIN64
		shutdown(client_fd, SD_BOTH);
		closesocket(client_fd);
#else
		shutdown(client_fd, SHUT_RDWR);
		close(client_fd);
#endif
	}
}

void network_state::perform_pending(sys::state& state) {
	if(!is_present())
		return;
	
	if(as_server) {
		if(!(state.actual_game_speed.load(std::memory_order::acquire) <= 0 || state.ui_pause.load(std::memory_order::acquire) || state.internally_paused)) {
			// place "advance time" command
			command::payload advt;
			advt.type = command::command_type::advance_tick;
			server_commands.push(advt);
		}
		// put server commands onto the queues of the worker commands
		auto* c = server_commands.front();
		while(c) {
			for(uint32_t i = 0; i < max_clients; i++)
				if(clients[i].active)
					clients[i].worker_commands.push(*c);
			state.incoming_commands.push(*c);
			//
			server_commands.pop();
			c = server_commands.front();
		}
	} else {
		// read from server until we are advised to advance the simulation
		while(1) {
			// clear the (local) incoming commands and send them to the server
			auto* c = state.network_state.client_commands.front();
			while(c) {
				if(internal_send(socket_fd, c, sizeof(*c)) != sizeof(*c))
					std::abort();
				state.network_state.client_commands.pop();
				c = state.network_state.client_commands.front();
			}
			// obtain messages from the server, push them to the incoming commands
			command::payload cmd;
			if(internal_recv(socket_fd, &cmd, sizeof(cmd)) != sizeof(cmd))
				continue;
			if(cmd.type == command::command_type::advance_tick)
				break;
			state.incoming_commands.push(cmd);
		}
	}
}

network_state::~network_state() {
#ifdef _WIN64
	shutdown(socket_fd, SD_BOTH);
	closesocket(socket_fd);
	WSACleanup();
#else
	shutdown(socket_fd, SHUT_RDWR);
	close(socket_fd);
#endif
}

bool network_state::is_present() {
	return socket_fd != 0;
}

}
