#pragma once

#include <atomic>
#ifdef _WIN64 // WINDOWS
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#ifndef WINSOCK2_IMPORTED
#define WINSOCK2_IMPORTED
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#else // NIX
#include <netinet/in.h>
#include <sys/socket.h>
#endif
#include "SPSCQueue.h"

namespace sys {
struct state;
};
namespace parsers {
struct scenario_building_context;
};

namespace network {
constexpr inline short server_port = 1984;
constexpr inline uint32_t max_clients = 16;

struct network_state {
	bool as_server = false;
	struct sockaddr_in server_address;
	rigtorp::SPSCQueue<command::payload> client_commands;
	rigtorp::SPSCQueue<command::payload> server_commands;
	struct client_data {
		std::atomic<bool> active = false;
		std::atomic<bool> wait_write = false;
		rigtorp::SPSCQueue<command::payload> worker_commands;
		dcon::nation_id player_nation;
		char player_name[16]; //15 characters

		client_data() : worker_commands(1024) {};
	} clients[max_clients];
#ifdef _WIN64
	SOCKET socket_fd = 0;
#else
	int socket_fd = 0;
#endif

	network_state() : client_commands(1024), server_commands(1024) {};
	~network_state();
	void init(sys::state& state, bool as_server, std::string_view ip_addr = "127.0.0.1");
	void server_client_loop(sys::state& state, int worker_id);
	void perform_pending(sys::state& state);
	bool is_present();
};
}
