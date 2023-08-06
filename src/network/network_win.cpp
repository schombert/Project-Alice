#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#ifndef WINSOCK2_IMPORTED
#define WINSOCK2_IMPORTED
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <windows.h>
#include "network.hpp"

namespace network {

static void copy_queue(rigtorp::SPSCQueue<command::payload>& d, rigtorp::SPSCQueue<command::payload>& s) {
	auto* c = s.front();
	if(c != nullptr)
		for(size_t i = 0; i < s.size(); i++)
			d.push(c[i]);
}

void network_state::init(sys::state& state, bool _as_server) {
    WSADATA data;
    if(WSAStartup(MAKEWORD(2, 2), &data) != 0)
		std::abort();

	as_server = _as_server;
	if(as_server) {
		if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			std::abort();
		
		int opt = 1;
		if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
			std::abort();
		
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
		if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
			std::abort();
		if(connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
			std::abort();
		
		// max speed, no pause (blocked by waiting for server commands however)
		state.actual_game_speed = 5;
		state.ui_pause = false;
	}
}

void network_state::server_client_loop(sys::state& state) {
	if(!is_present() || !as_server)
		return;

	while(state.quit_signaled.load(std::memory_order::acquire) == false) {
		socklen_t addr_len = sizeof(server_address);
		int client_fd;
		if((client_fd = accept(socket_fd, (struct sockaddr *)&server_address, &addr_len)) < 0)
			continue;
		// TODO: Notify other clients  std::fprintf(stderr, "client connected\n");
		num_clients++;
		while(1) {
			ack_sem++; // semaphore
			// read out until no more data
			while(1) {
				command::payload cmd;
				if(recv(client_fd, &cmd, sizeof(cmd), MSG_DONTWAIT) != sizeof(cmd))
					break;
				auto b = state.incoming_commands.try_push(cmd);
			}
			// send commands to client
			rigtorp::SPSCQueue<command::payload> q(1024);
			copy_queue(q, state.incoming_commands);
			auto* c = q.front();
			while(c) {
				if(send(client_fd, c, sizeof(*c), MSG_NOSIGNAL) != sizeof(*c)) {
					// TODO: Notify other clients
					goto close_finish;
				}
				q.pop();
				c = q.front();
			}
			ack_sem--; // semaphore

			if(ack_sem.load(std::memory_order::acquire) != 0) {
				// send "advance tick" notification to client
				command::payload advt;
				advt.type = command::command_type::advance_tick;
				if(send(client_fd, &advt, sizeof(advt), MSG_NOSIGNAL) != sizeof(advt)) {
					// TODO: Notify other clients
					ack_sem--;
					goto close_finish;
				}
				ack_sem--;
				// wait for all other clients to also advance time...
				while(ack_sem.load(std::memory_order::acquire) != 0)
					;
			}
		}
close_finish:
		num_clients--;
		shutdown(client_fd, SD_BOTH);
		close(client_fd);
	}
}

void network_state::client_flush_local_commands(sys::state& state) {
	// clear the (local) incoming commands
	auto* c = state.incoming_commands.front();
	while(c) {
		if(send(socket_fd, c, sizeof(*c), MSG_NOSIGNAL) != sizeof(*c))
			std::abort();
		state.incoming_commands.pop();
		c = state.incoming_commands.front();
	}
}

void network_state::perform_pending(sys::state& state) {
	if(!is_present())
		return;
	
	if(as_server) {
		if(state.actual_game_speed.load(std::memory_order::acquire) <= 0 || state.ui_pause.load(std::memory_order::acquire) || state.internally_paused) {
			ack_sem += 0;
		} else {
			ack_sem += num_clients;
		}
		// wait until ack-clients goes down (all clients finished being sent pending commands)
		while(ack_sem.load(std::memory_order::acquire) != 0)
			;
	} else {
		client_flush_local_commands(state);
		// read from server until we are advised to advance the simulation
		rigtorp::SPSCQueue<command::payload> server_queue(1024);
		while(1) {
			client_flush_local_commands(state);
			command::payload cmd;
			if(recv(socket_fd, &cmd, sizeof(cmd), MSG_DONTWAIT) != sizeof(cmd))
				continue;
			if(cmd.type == command::command_type::advance_tick) {
				client_flush_local_commands(state);
				break;
			}
			auto b = server_queue.try_push(cmd);
		}
		// after collecting all the commands the server sent us, copy it into the incoming commands queue
		client_flush_local_commands(state);
		copy_queue(state.incoming_commands, server_queue);
	}
}

network_state::~network_state() {
	shutdown(socket_fd, SD_BOTH);
	closesocket(socket_fd);
	WSACleanup();
}

bool network_state::is_present() {
	return socket_fd != 0;
}

};
