#pragma once

#include <array>
#include <string>
#ifdef _WIN64 // WINDOWS
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#ifndef WINSOCK2_IMPORTED
#define WINSOCK2_IMPORTED
#include <winsock2.h>
#include <ws2tcpip.h>
#include <condition_variable>
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


static std::map<int, std::string> readableCommandTypes = {
	{0,"invalid"},
{1,"change_nat_focus"},
{2,"start_research"},
{3,"make_leader"},
{4,"begin_province_building_construction"},
{5,"increase_relations"},
{6,"decrease_relations"},
{7,"begin_factory_building_construction"},
{8,"begin_naval_unit_construction"},
{9,"cancel_naval_unit_construction"},
{10,"change_factory_settings"},
{11,"delete_factory"},
{12,"make_vassal"},
{13,"release_and_play_nation"},
{14,"war_subsidies"},
{15,"cancel_war_subsidies"},
{16,"change_budget"},
{17,"start_election"},
{18,"change_influence_priority"},
{19,"discredit_advisors"},
{20,"expel_advisors"},
{21,"ban_embassy"},
{22,"increase_opinion"},
{23,"decrease_opinion"},
{24,"add_to_sphere"},
{25,"remove_from_sphere"},
{26,"upgrade_colony_to_state"},
{27,"invest_in_colony"},
{28,"abandon_colony"},
{29,"finish_colonization"},
{30,"intervene_in_war"},
{31,"suppress_movement"},
{32,"civilize_nation"},
{33,"appoint_ruling_party"},
{34,"change_issue_option"},
{35,"change_reform_option"},
{36,"become_interested_in_crisis"},
{37,"take_sides_in_crisis"},
{38,"begin_land_unit_construction"},
{39,"cancel_land_unit_construction"},
{40,"change_stockpile_settings"},
{41,"take_decision"},
{42,"make_n_event_choice"},
{43,"make_f_n_event_choice"},
{44,"make_p_event_choice"},
{45,"make_f_p_event_choice"},
{46,"fabricate_cb"},
{47,"cancel_cb_fabrication"},
{48,"ask_for_military_access"},
{49,"ask_for_alliance"},
{50,"call_to_arms"},
{51,"respond_to_diplomatic_message"},
{52,"cancel_military_access"},
{53,"cancel_alliance"},
{54,"cancel_given_military_access"},
{55,"declare_war"},
{56,"add_war_goal"},
{58,"start_peace_offer"},
{59,"add_peace_offer_term"},
{60,"send_peace_offer"},
{61,"move_army"},
{62,"move_navy"},
{63,"embark_army"},
{64,"merge_armies"},
{65,"merge_navies"},
{66,"split_army"},
{67,"split_navy"},
{68,"delete_army"},
{69,"delete_navy"},
{70,"designate_split_regiments"},
{71,"designate_split_ships"},
{72,"naval_retreat"},
{73,"land_retreat"},
{74,"start_crisis_peace_offer"},
{75,"invite_to_crisis"},
{76,"add_wargoal_to_crisis_offer"},
{77,"send_crisis_peace_offer"},
{78,"change_admiral"},
{79,"change_general"},
{80,"toggle_mobilization"},
{81,"give_military_access"},
{82,"set_rally_point"},
{83,"save_game"},
{84,"cancel_factory_building_construction"},
{85,"disband_undermanned"},
{86,"even_split_army"},
{87,"even_split_navy"},
{88,"toggle_hunt_rebels"},
{89,"toggle_select_province"},
{90,"toggle_immigrator_province"},
{91,"state_transfer"},
{92,"release_subject"},
{93,"enable_debt"},
{94,"move_capital"},
{95,"toggle_unit_ai_control"},
{96,"toggle_mobilized_is_ai_controlled"},
{97,"toggle_interested_in_alliance"},
{98,"pbutton_script"},
{99,"nbutton_script"},
{100,"set_factory_type_priority"},
{101,"crisis_add_wargoal" },
{102,"change_unit_type" },
{ 102,"change_unit_type" },
{ 103,"take_province" },
{ 104,"grant_province" },
{ 105,"ask_for_free_trade_agreement" },
{ 106,"switch_embargo_status" },
{ 107,"revoke_trade_rights" },
{ 108,"toggle_local_administration" },
{109, "stop_army_movement" },
{110, "stop_navy_movement" },
{ 235, "notify_mp_data" },
{ 236, "resync_lobby" },
{ 237,"notify_player_ban" },
{ 238,"notify_player_kick" },
{ 239,"notify_player_picks_nation" },
{ 240,"notify_player_joins" },
{ 241,"notify_player_leaves" },
{ 242,"notify_player_oos" },
{ 243,"notify_save_loaded" },
{ 244,"notify_start_game" },
{ 245,"notify_stop_game" },
{ 246,"notify_pause_game" },
{ 247,"notify_reload" },
{ 248,"advance_tick" },
{ 249,"chat_message" },
{ 250,"network_inactivity_ping" },
{ 251, "notify_player_fully_loaded" },
{ 252, "notify_player_is_loading" },
{ 253, "change_ai_nation_state" },
{ 254, "network_populate" },
{ 255,"console_command" },
};



inline constexpr short default_server_port = 1984;

inline static SHA512 sha512;

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
	dcon::mp_player_id assigned_player_id;
	uint8_t reserved[64] = {0};
};

struct client_data {
	dcon::mp_player_id player_id{ };
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
	bool receiving_payload_flag = false;

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
	std::string port = "1984";
	std::vector<char> send_buffer;
	std::vector<char> early_send_buffer;
	command::command_data recv_buffer;
	bool receiving_payload_flag = false;
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

std::string get_last_error_msg();
inline void write_player_nations(sys::state& state) noexcept;
void init(sys::state& state);
void send_and_receive_commands(sys::state& state);
void finish(sys::state& state, bool notify_host);
void ban_player(sys::state& state, client_data& client);
void kick_player(sys::state& state, client_data& client);
void switch_one_player(sys::state& state, dcon::nation_id new_n, dcon::nation_id old_n, dcon::mp_player_id player); // switches only one player from one country, to another. Can only be called in MP.
void write_network_save(sys::state& state);
void broadcast_save_to_clients(sys::state& state);
void broadcast_save_to_single_client(sys::state& state, command::command_data& c, client_data& client, uint8_t const* buffer);
void broadcast_to_clients(sys::state& state, command::command_data& c);
void clear_socket(sys::state& state, client_data& client);
void full_reset_after_oos(sys::state& state);


void load_network_save(sys::state& state, const uint8_t* save_buffer);

// gets the host player in the current lobby. Shouldn't be used in single-player mode
dcon::mp_player_id get_host_player(sys::state& state);

bool any_player_on_invalid_nation(sys::state& state);
bool check_any_players_loading(sys::state& state); // returns true if any players are loading. If the loading state has not changed, it will not iterate though the players and simply return false
void delete_mp_player(sys::state& state, dcon::mp_player_id player, bool make_ai);
void mp_player_set_fully_loaded(sys::state& state, dcon::mp_player_id player, bool fully_loaded); // wrapper for setting a mp player to being fully loaded or not
dcon::mp_player_id create_mp_player(sys::state& state, const sys::player_name& name, const sys::player_password_raw& password, bool fully_loaded, bool is_oos, dcon::nation_id nation_to_play = dcon::nation_id{} );
void notify_player_is_loading(sys::state& state, dcon::mp_player_id loading_player, bool execute_self); // wrapper for notiying clients are loading
dcon::mp_player_id load_mp_player(sys::state& state, sys::player_name& name, sys::player_password_hash& password_hash, sys::player_password_salt& password_salt);
void update_mp_player_password(sys::state& state, dcon::mp_player_id player_id, sys::player_name& password);
dcon::mp_player_id find_mp_player(sys::state& state, sys::player_name name);
std::vector<dcon::mp_player_id> find_country_players(sys::state& state, dcon::nation_id nation);
void set_no_ai_nations_after_reload(sys::state& state, std::vector<dcon::nation_id>& no_ai_nations, dcon::nation_id old_local_player_nation); // places the players back on their nations, or new ones if the old ones are no longer valid
bool any_player_oos(sys::state& state);
void log_player_nations(sys::state& state);

bool pause_game(sys::state& state);
bool unpause_game(sys::state& state);
void player_joins(sys::state& state, client_data& joining_client, dcon::nation_id player_nation);

void load_host_settings(sys::state& state);
void save_host_settings(sys::state& state);

class port_forwarder {
private:
	std::mutex internal_wait;
	std::thread do_forwarding;
	bool started = false;
public:
	port_forwarder();
	void start_forwarding(uint16_t port);
	~port_forwarder();
};

}

