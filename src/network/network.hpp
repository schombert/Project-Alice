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
#include "SHA512.hpp"

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
	sys::player_password_raw player_password;
	uint8_t lobby_password[16] = {0};
	uint8_t reserved[24] = {0};
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
	struct sockaddr_storage address;

	client_handshake_data hshake_buffer;
	command::command_data recv_buffer;
	size_t recv_count = 0;
	std::vector<char> send_buffer;
	std::vector<char> early_send_buffer;

	// accounting for save progress
	size_t total_sent_bytes = 0;
	size_t save_stream_offset = 0;
	size_t save_stream_size = 0;
	bool handshake = true;

	sys::date last_seen;

	bool is_banned(sys::state& state) const;
	inline bool is_active() const {
		return socket_fd > 0;
	}
};

struct network_state {
	server_handshake_data s_hshake;
	sys::player_name nickname;
	sys::player_password_raw player_password;
	sys::checksum_key current_mp_state_checksum;
	struct sockaddr_storage address;
	rigtorp::SPSCQueue<command::command_data> outgoing_commands;
	std::array<client_data, 128> clients;
	std::vector<struct in6_addr> v6_banlist;
	std::vector<struct in_addr> v4_banlist;
	std::string ip_address = "127.0.0.1";
	std::vector<char> send_buffer;
	std::vector<char> early_send_buffer;
	command::command_data recv_buffer;
	std::vector<uint8_t> save_data; //client

	std::unique_ptr<uint8_t[]> current_save_buffer;
	size_t recv_count = 0;
	uint32_t current_save_length = 0;
	socket_t socket_fd = 0;
	uint8_t lobby_password[16] = { 0 };
	std::mutex command_lock; // when this lock is held, the command thread will be blocked. Used on the UI thread to ensure no commands are executed in the meantime
	std::condition_variable command_lock_cv; // condition variable for command lock
	bool yield_command_lock = false;
	bool as_v6 = false;
	bool as_server = false;
	bool save_stream = false; //client
	bool is_new_game = true; // has save been loaded?
	bool out_of_sync = false; // network -> game state signal
	bool reported_oos = false; // has oos been reported to host yet?
	bool handshake = true; // if in handshake mode -> expect handshake data
	bool finished = false; //game can run after disconnection but only to show error messages
	sys::checksum_key last_save_checksum; // the last save checksum which was written to the network
	std::atomic<bool> clients_loading_state_changed; // flag to indicate if any client loading state has changed (client has started loading, finished loading, or left the game)
	std::atomic<bool> any_client_loading_flag; // flag to signal if any clients are currently loading. If "clients_loading_state_changed" is false, it will use this instead, otherwise compute it manually by iterating over the players.

	network_state() : outgoing_commands(4096) {}
	~network_state() {}
};
inline void write_player_nations(sys::state& state) noexcept;
void init(sys::state& state);
void send_and_receive_commands(sys::state& state);
void finish(sys::state& state, bool notify_host);
void ban_player(sys::state& state, client_data& client);
void kick_player(sys::state& state, client_data& client);
void switch_one_player(sys::state& state, dcon::nation_id new_n, dcon::nation_id old_n, dcon::mp_player_id player); // switches only one player from one country, to another. Can only be called in MP.
void write_network_save(sys::state& state);
void broadcast_save_to_clients(sys::state& state, command::command_data& c, uint8_t const* buffer);
void broadcast_save_to_single_client(sys::state& state, command::command_data& c, client_data& client, uint8_t const* buffer);
void broadcast_to_clients(sys::state& state, command::command_data& c);
void clear_socket(sys::state& state, client_data& client);
void full_reset_after_oos(sys::state& state);

// gets the host player in the current lobby. Shouldn't be used in single-player mode
dcon::mp_player_id get_host_player(sys::state& state);

bool any_player_on_invalid_nation(sys::state& state);
bool check_any_players_loading(sys::state& state); // returns true if any players are loading. If the loading state has not changed, it will not iterate though the players and simply return false
void delete_mp_player(sys::state& state, dcon::mp_player_id player, bool make_ai);
void mp_player_set_fully_loaded(sys::state& state, dcon::mp_player_id player, bool fully_loaded); // wrapper for setting a mp player to being fully loaded or not
dcon::mp_player_id create_mp_player(sys::state& state, sys::player_name& name, sys::player_password_raw& password, bool fully_loaded, bool is_oos, dcon::nation_id nation_to_play = dcon::nation_id{} );
void notify_player_is_loading(sys::state& state, sys::player_name& name, dcon::nation_id nation, bool execute_self); // wrapper for notiying clients are loading
dcon::mp_player_id load_mp_player(sys::state& state, sys::player_name& name, sys::player_password_hash& password_hash, sys::player_password_salt& password_salt);
void update_mp_player_password(sys::state& state, dcon::mp_player_id player_id, sys::player_name& password);
dcon::mp_player_id find_mp_player(sys::state& state, sys::player_name name);
std::vector<dcon::mp_player_id> find_country_players(sys::state& state, dcon::nation_id nation);
void set_no_ai_nations_after_reload(sys::state& state, std::vector<dcon::nation_id>& no_ai_nations, dcon::nation_id old_local_player_nation); // places the players back on their nations, or new ones if the old ones are no longer valid
bool any_player_oos(sys::state& state);
void log_player_nations(sys::state& state);

void place_host_player_after_saveload(sys::state& state);
bool pause_game(sys::state& state);
bool unpause_game(sys::state& state);
void notify_player_joins(sys::state& state, network::client_data& client, bool needs_loading);
void notify_player_joins(sys::state& state, sys::player_name& name, dcon::nation_id nation, bool needs_loading);

void load_host_settings(sys::state& state);
void save_host_settings(sys::state& state);

class port_forwarder {
private:
	std::mutex internal_wait;
	std::thread do_forwarding;
	bool started = false;
public:
	port_forwarder();
	void start_forwarding();
	~port_forwarder();
};

}

