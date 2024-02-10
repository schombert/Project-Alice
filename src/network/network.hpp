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
#include "commands.hpp"

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

struct client_handshake_data {
	sys::player_name nickname;
	uint8_t password[16] = {0};
	uint8_t reserved[48] = {0};
};

struct server_handshake_data {
	sys::checksum_key scenario_checksum;
	sys::checksum_key save_checksum;
	uint32_t seed;
	dcon::nation_id assigned_nation;
	uint8_t reserved[64] = {0};
};

struct client_data {
	dcon::nation_id playing_as{};
	socket_t socket_fd = 0;
	struct sockaddr_in6 v6_address;
	struct sockaddr_in v4_address;

	client_handshake_data hshake_buffer;
	command::payload recv_buffer;
	size_t recv_count = 0;
	std::vector<char> send_buffer;
	std::vector<char> early_send_buffer;

	// accounting for save progress
	size_t total_sent_bytes = 0;
	size_t save_stream_offset = 0;
	size_t save_stream_size = 0;
	bool handshake = true;

	bool is_banned(sys::state& state) const;
	inline bool is_active() const {
		return socket_fd > 0;
	}
};

struct network_state {
	server_handshake_data s_hshake;
	sys::player_name nickname;
	sys::checksum_key current_save_checksum;
	struct sockaddr_in6 v6_address;
	struct sockaddr_in v4_address;
	rigtorp::SPSCQueue<command::payload> outgoing_commands;
	std::array<client_data, 128> clients;
	std::vector<struct in6_addr> v6_banlist;
	std::vector<struct in_addr> v4_banlist;
	std::string ip_address = "127.0.0.1";
	std::vector<char> send_buffer;
	std::vector<char> early_send_buffer;
	command::payload recv_buffer;
	std::vector<uint8_t> save_data; //client
	ankerl::unordered_dense::map<int32_t, sys::player_name> map_of_player_names;
	std::unique_ptr<uint8_t[]> current_save_buffer;
	size_t recv_count = 0;
	uint32_t current_save_length = 0;
	socket_t socket_fd = 0;
	uint8_t password[16] = { 0 };
	std::atomic<bool> save_slock = false;
	bool as_v6 = false;
	bool as_server = false;
	bool save_stream = false; //client
	bool is_new_game = true; // has save been loaded?
	bool out_of_sync = false; // network -> game state signal
	bool reported_oos = false; // has oos been reported to host yet?
	bool handshake = true; // if in handshake mode -> send handshake data

	network_state() : outgoing_commands(1024) {}
	~network_state() {}
};

void init(sys::state& state);
void send_and_receive_commands(sys::state& state);
void finish(sys::state& state);
void ban_player(sys::state& state, client_data& client);
void kick_player(sys::state& state, client_data& client);
void switch_player(sys::state& state, dcon::nation_id new_n, dcon::nation_id old_n);
void write_network_save(sys::state& state);
void broadcast_save_to_clients(sys::state& state, command::payload& c, uint8_t const* buffer, uint32_t length, sys::checksum_key const& k);
void broadcast_to_clients(sys::state& state, command::payload& c);

}
