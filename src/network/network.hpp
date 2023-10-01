#pragma once

#include <array>
#include <string>
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

namespace network {

inline constexpr short default_server_port = 1984;

#ifdef _WIN64
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

struct client_data {
	dcon::nation_id playing_as{};
	std::string player_name;
	socket_t socket_fd = 0;
	struct sockaddr_in6 v6_address;
	struct sockaddr_in v4_address;

	inline bool is_active() {
		return socket_fd > 0;
	}
};

struct network_state {
	bool as_v6 = false;
	bool as_server = false;
	struct sockaddr_in6 v6_address;
	struct sockaddr_in v4_address;
	rigtorp::SPSCQueue<command::payload> outgoing_commands;
	std::array<client_data, 16> clients;
	std::vector<struct in6_addr> v6_banlist;
	std::vector<struct in_addr> v4_banlist;
	socket_t socket_fd = 0;
	std::string ip_address = "127.0.0.1";

	network_state() : outgoing_commands(1024) {}
	~network_state() {}
};

void init(sys::state& state);
void send_and_receive_commands(sys::state& state);
void finish(sys::state& state);
void ban_player(sys::state& state, client_data& client);
void kick_player(sys::state& state, client_data& client);

}
