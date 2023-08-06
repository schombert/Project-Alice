#pragma once

#include <atomic>
#include <netinet/in.h>
#include <sys/socket.h>

namespace sys {
struct state;
};
namespace parsers {
struct scenario_building_context;
};

namespace network {
constexpr inline short server_port = 1984;

struct network_state {
	bool as_server = false;

	network_state() {};
	~network_state();
	void init(sys::state& state, bool as_server);
	void server_client_loop(sys::state& state);
	void perform_pending(sys::state& state);
	bool is_present();
private:
	std::atomic<int32_t> ack_clients = 0;
	struct sockaddr_in server_address;
	int socket_fd = 0;
};
}
