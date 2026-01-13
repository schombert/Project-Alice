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
#include "SHA512.hpp"
#include "concurrentqueue.h"

namespace sys {
struct state;
};

namespace network {

struct host_command_wrapper;

enum class disconnect_reason : uint8_t;

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



inline static SHA512 sha512;


bool is_scheduled_shutdown(const sys::state& state, dcon::client_id client);
bool can_add_data(const sys::state& state, dcon::client_id client);
bool can_send_data(const sys::state& state, dcon::client_id client);
bool is_flushing(const sys::state& state, dcon::client_id client);

void reload_save_locally(sys::state& state);
bool should_do_oos_check(const sys::state& state);
bool should_do_clients_to_far_behind_check(const sys::state& state);
std::string get_last_error_msg();
void init(sys::state& state);
void send_and_receive_commands(sys::state& state);
void finish(sys::state& state, bool notify_host);
void add_player_to_ban_list(sys::state& state, dcon::mp_player_id playerid);
void switch_one_player(sys::state& state, dcon::nation_id new_n, dcon::nation_id old_n, dcon::mp_player_id player); // switches only one player from one country, to another. Can only be called in MP.
void write_network_save(sys::state& state);
std::unique_ptr<FT_Byte[]> write_network_entire_mp_state(sys::state& state, uint32_t& size_out);
void broadcast_to_clients(sys::state& state, host_command_wrapper&& c);
void clear_socket(sys::state& state, dcon::client_id client);
void full_reset_after_oos(sys::state& state);


void load_network_save(sys::state& state, const uint8_t* save_buffer);
void decompress_load_entire_mp_state(sys::state& state, const uint8_t* mp_state_data, uint32_t length);

void dump_oos_report(sys::state& state_1, sys::state& state_2);

// gets the host player in the current lobby. Also returns the static player ID of the player in single player.
dcon::mp_player_id get_host_player(sys::state& state);

bool any_player_on_invalid_nation(sys::state& state);
bool check_any_players_loading(sys::state& state); // returns true if any players are loading. If the loading state has not changed, it will not iterate though the players and simply return false
void delete_mp_player(sys::state& state, dcon::mp_player_id player, bool make_ai);
void mp_player_set_fully_loaded(sys::state& state, dcon::mp_player_id player, bool fully_loaded); // wrapper for setting a mp player to being fully loaded or not
dcon::mp_player_id create_mp_player(sys::state& state, const sys::player_name& name, const sys::player_password_raw& password, bool fully_loaded, bool is_oos, dcon::nation_id nation_to_play = dcon::nation_id{} );
dcon::mp_player_id load_mp_player(sys::state& state, sys::player_name& name, sys::player_password_hash& password_hash, sys::player_password_salt& password_salt);
void update_mp_player_password(sys::state& state, dcon::mp_player_id player_id, sys::player_name& password);
dcon::mp_player_id find_mp_player(sys::state& state, const sys::player_name& name);
std::vector<dcon::mp_player_id> find_country_players(sys::state& state, dcon::nation_id nation);
void set_no_ai_nations_after_reload(sys::state& state, std::vector<dcon::nation_id>& no_ai_nations);
bool any_player_oos(sys::state& state);
void log_player_nations(sys::state& state);
void disconnect_player(sys::state& state, dcon::mp_player_id player_id, bool make_ai, disconnect_reason reason);

void server_send_handshake(sys::state& state, dcon::client_id client, dcon::nation_id player_nation, dcon::mp_player_id player_id);
void send_post_handshake_commands(sys::state& state, dcon::client_id client);

// Adds a command directly to the specific player's send buffer, skipping the command queue.
void add_command_to_player_buffer(sys::state& state, dcon::mp_player_id player_target, command::command_data&& command);

bool pause_game(sys::state& state);
bool unpause_game(sys::state& state);

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

