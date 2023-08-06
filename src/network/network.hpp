#pragma once

#include <atomic>
#include <netinet/in.h>
#include <sys/socket.h>
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
	int socket_fd = 0;
	rigtorp::SPSCQueue<command::payload> client_commands;
	rigtorp::SPSCQueue<command::payload> server_commands;
	struct client_data {
		std::atomic<bool> active = false;
		std::atomic<bool> wait_write = false;
		rigtorp::SPSCQueue<command::payload> worker_commands;
		client_data() : worker_commands(1024) {};
	} clients[max_clients];

	network_state() : client_commands(1024), server_commands(1024) {};
	~network_state();
	void init(sys::state& state, bool as_server);
	void server_client_loop(sys::state& state, int worker_id);
	void perform_pending(sys::state& state);
	bool is_present();
};
}
