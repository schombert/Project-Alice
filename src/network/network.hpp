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
#include "container_types.hpp"

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
	socket_t socket_fd = 0;
	struct sockaddr_in6 v6_address;
	struct sockaddr_in v4_address;

	command::payload recv_buffer;
	size_t recv_count = 0;
	std::vector<char> send_buffer;

	// accounting for save progress
	size_t total_sent_bytes = 0;
	size_t save_stream_offset = 0;
	size_t save_stream_size = 0;

	bool is_banned(sys::state& state) const;
	inline bool is_active() const {
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

	command::payload recv_buffer;
	size_t recv_count = 0;
	std::vector<char> send_buffer;
	/* Data to send new clients who join the lobby, replaying the commands of the host as they occurred */
	std::vector<char> new_client_send_buffer;

	bool save_stream = false; //client
	uint32_t save_size = 0; //client
	std::vector<uint8_t> save_data; //client

	bool is_new_game = true; // has save been loaded?
	bool out_of_sync = false; // network -> game state signal
	bool reported_oos = false; // has oos been reported to host yet?
	bool sent_nickname = false; // one-time send the nation_joins command to the server to update our ekename

	sys::player_name nickname;
	ankerl::unordered_dense::map<int32_t, sys::player_name> map_of_player_names;

	std::atomic<bool> save_slock = false;

	network_state() : outgoing_commands(1024) {}
	~network_state() {}
};

void init(sys::state& state);
void send_and_receive_commands(sys::state& state);
void finish(sys::state& state);
void ban_player(sys::state& state, client_data& client);
void kick_player(sys::state& state, client_data& client);
void switch_player(sys::state& state, dcon::nation_id new_n, dcon::nation_id old_n);
void broadcast_to_clients(sys::state& state, command::payload& c);

}
