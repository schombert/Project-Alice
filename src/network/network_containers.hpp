#pragma once
#include "concurrentqueue.h"

namespace network {


enum class oos_check_interval : uint8_t {
	never = 0,
	daily = 1,
	monthly = 2,
	yearly = 3,
};

enum class disconnect_reason : uint8_t {
	left_game = 0,
	kicked = 1,
	banned = 2,
	on_banlist = 3,
	name_taken = 4,
	incorrect_password = 5,
	game_has_ended = 6,
	timed_out = 7,
	network_error = 8,
	game_full = 9
};

enum class handshake_result : uint8_t {
	sucess = 0,
	fail_name_taken = 1,
	fail_wrong_password = 2,
	fail_on_banlist = 3,
	fail_game_ended = 4,
	fail_game_full = 5,
};

union selector_arg {
	dcon::client_id client;
	dcon::mp_player_id player;

	selector_arg() {
	}
	selector_arg(dcon::client_id _client) : client(_client) {
	}
	selector_arg(dcon::mp_player_id _player) : player(_player) {
	}
};

typedef bool (*selector_function)(dcon::client_id, const sys::state&, const selector_arg);



struct host_settings_s {
	float alice_persistent_server_mode = 0.0f;
	float alice_persistent_server_unpause = 12.f;
	float alice_persistent_server_pause = 20.f;
	float alice_expose_webui = 0.0f;
	float alice_place_ai_upon_disconnection = 1.0f;
	float alice_lagging_behind_days_to_slow_down = 30.f;
	float alice_lagging_behind_days_to_drop = 90.f;
	uint16_t alice_host_port = 1984;
	oos_check_interval oos_interval = oos_check_interval::monthly;
	bool oos_debug_mode = false; // enables sending of gamestate from client to host when an OOS happens, so the host can generate a OOS report. Is NOT safe to enable unless you trust clients
	uint8_t max_players = MAX_PLAYER_COUNT;
};


struct server_handshake_data {
	sys::checksum_key scenario_checksum;
	uint32_t seed;
	dcon::nation_id assigned_nation;
	dcon::mp_player_id assigned_player_id;
	network::host_settings_s host_settings;
	handshake_result result;
};


// Wrapper for commands in the host queue, with some extra members deciding which clients should get the command, and if the host should execute it
struct host_command_wrapper {
	command::command_data cmd_data; // the command data itself
	selector_arg arg; // optional argument to pass to the selector function
	selector_function selector; // Selector function for which clients should get the command broadcasted. nullptr = all clients
	bool host_execute = true; // Should the host execute this command?

	host_command_wrapper() {
	};

	host_command_wrapper(const command::command_data& _cmd_data, selector_arg _arg, selector_function _selector) : cmd_data(_cmd_data), arg(_arg), selector(_selector) {
	}
	host_command_wrapper(command::command_data&& _cmd_data, selector_arg _arg, selector_function _selector) : cmd_data(std::move(_cmd_data)), arg(_arg), selector(_selector) {
	}

	host_command_wrapper(const command::command_data& _cmd_data, selector_arg _arg, selector_function _selector, bool _host_execute) : cmd_data(_cmd_data), arg(_arg), selector(_selector), host_execute(_host_execute) {
	}
	host_command_wrapper(command::command_data&& _cmd_data, selector_arg _arg, selector_function _selector, bool _host_execute) : cmd_data(std::move(_cmd_data)), arg(_arg), selector(_selector), host_execute(_host_execute) {
	}
	host_command_wrapper(host_command_wrapper&& other) = default;
	host_command_wrapper(const host_command_wrapper& other) = default;

	host_command_wrapper& operator=(host_command_wrapper&& other) = default;
	host_command_wrapper& operator=(const host_command_wrapper& other) = default;

	~host_command_wrapper() = default;

};


struct server_send_recv_buffers {
	std::queue<std::shared_ptr<command::command_data>> send_buffer;
	std::vector<char> early_send_buffer;
	command::command_data command_recv_buffer;

	void reset() {
		command_recv_buffer.payload.clear();
		early_send_buffer.clear();
		while(!send_buffer.empty()) {
			send_buffer.pop();
		}
	}
};



struct network_state {
	server_handshake_data s_hshake;
	sys::player_name nickname;
	sys::player_password_raw player_password;
	sys::checksum_key current_mp_state_checksum;
	struct sockaddr_storage address;
	moodycamel::ConcurrentQueue<host_command_wrapper> server_outgoing_commands;
	moodycamel::ConcurrentQueue<command::command_data> client_outgoing_commands;
	std::vector<struct in6_addr> v6_banlist;
	std::vector<struct in_addr> v4_banlist;
	std::string ip_address = "127.0.0.1";
	std::string port = "1984";
	tagged_vector<server_send_recv_buffers, dcon::client_id> server_send_recv_buffers; // These buffers are kept outside of dcon because they allocate dynamic memory and are not trivially copyable.

	inline std::vector<char>& server_get_early_send_buffer(dcon::client_id client) {
		return server_send_recv_buffers[client].early_send_buffer;
	}
	inline std::queue<std::shared_ptr<command::command_data>>& server_get_send_buffer(dcon::client_id client) {
		return server_send_recv_buffers[client].send_buffer;
	}
	inline command::command_data& server_get_recv_buffer(dcon::client_id client) {
		return server_send_recv_buffers[client].command_recv_buffer;
	}

	std::vector<char> send_buffer;
	std::vector<char> early_send_buffer;
	command::command_data command_recv_buffer;
	std::array<uint8_t, RECV_BUFFER_SIZE> receive_buffer;
	fixed_bool_t receiving_payload = false; // flag indicating whether we are currently awaiting a payload for a command, or if its awaiting a header for a command from the server
	uint32_t command_send_count = 0;
	size_t total_send_count = 0;
	size_t recv_count = 0;
	int recv_buffer_size_used = 0;
	int recv_buffer_bytes_read = 0;

	socket_t socket_fd = 0;
	std::unique_ptr<uint8_t[]> current_save_buffer;
	uint32_t current_save_length = 0;
	uint8_t lobby_password[16] = { 0 };
	bool as_v6 = false;
	bool as_server = false;
	bool is_new_game = true; // has save been loaded?
	bool out_of_sync = false; // network -> game state signal
	bool reported_oos = false; // has oos been reported to host yet?
	bool handshake = true; // if in handshake mode -> expect handshake data
	bool finished = false; //game can run after disconnection but only to show error messages
	sys::checksum_key last_save_checksum; // the last save checksum which was written to the network
	std::atomic<bool> clients_loading_state_changed; // flag to indicate if any client loading state has changed (client has started loading, finished loading, or left the game)
	std::atomic<bool> any_client_loading_flag; // flag to signal if any clients are currently loading. If "clients_loading_state_changed" is false, it will use this instead, otherwise compute it manually by iterating over the players.

	network_state() : server_outgoing_commands(4096), client_outgoing_commands(4096) {
	}
	~network_state() {
	}
};

}
