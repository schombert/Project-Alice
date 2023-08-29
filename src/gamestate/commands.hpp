#pragma once
#include "dcon_generated.hpp"
#include "common_types.hpp"
#include "events.hpp"
#include "diplomatic_messages.hpp"

namespace command {

enum class command_type : uint8_t {
	invalid = 0,
	change_nat_focus = 1,
	start_research = 2,
	make_leader = 3,
	begin_province_building_construction = 4,
	increase_relations = 5,
	decrease_relations = 6,
	begin_factory_building_construction = 7,
	begin_naval_unit_construction = 8,
	cancel_naval_unit_construction = 9,
	change_factory_settings = 10,
	delete_factory = 11,
	make_vassal = 12,
	release_and_play_nation = 13,
	war_subsidies = 14,
	cancel_war_subsidies = 15,
	change_budget = 16,
	start_election = 17,
	change_influence_priority = 18,
	discredit_advisors = 19,
	expel_advisors = 20,
	ban_embassy = 21,
	increase_opinion = 22,
	decrease_opinion = 23,
	add_to_sphere = 24,
	remove_from_sphere = 25,
	upgrade_colony_to_state = 26,
	invest_in_colony = 27,
	abandon_colony = 28,
	finish_colonization = 29,
	intervene_in_war = 30,
	suppress_movement = 31,
	civilize_nation = 32,
	appoint_ruling_party = 33,
	change_issue_option = 34,
	change_reform_option = 35,
	become_interested_in_crisis = 36,
	take_sides_in_crisis = 37,
	begin_land_unit_construction = 38,
	cancel_land_unit_construction = 39,
	change_stockpile_settings = 40,
	take_decision = 41,
	make_n_event_choice = 42,
	make_f_n_event_choice = 43,
	make_p_event_choice = 44,
	make_f_p_event_choice = 45,
	fabricate_cb = 46,
	cancel_cb_fabrication = 47,
	ask_for_military_access = 48,
	ask_for_alliance = 49,
	call_to_arms = 50,
	respond_to_diplomatic_message = 51,
	cancel_military_access = 52,
	cancel_alliance = 53,
	cancel_given_military_access = 54,
	declare_war = 55,
	add_war_goal = 56,
	start_peace_offer = 58,
	add_peace_offer_term = 59,
	send_peace_offer = 60,
	move_army = 61,
	move_navy = 62,
	embark_army = 63,
	merge_armies = 64,
	merge_navies = 65,
	split_army = 66,
	split_navy = 67,
	delete_army = 68,
	delete_navy = 69,
	designate_split_regiments = 70,
	designate_split_ships = 71,
	naval_retreat = 72,
	land_retreat = 73,
	start_crisis_peace_offer = 74,
	invite_to_crisis = 75,
	add_wargoal_to_crisis_offer = 76,
	send_crisis_peace_offer = 77,
	change_admiral = 78,
	change_general = 79,
	toggle_mobilization = 80,
	give_military_access = 81,
	set_rally_point = 82,
	save_game = 83,
	cancel_factory_building_construction = 84,
	
	notify_player_ban = 117,
	notify_player_kick = 118,
	notify_player_picks_nation = 119,
	notify_player_joins = 120,
	notify_player_leaves = 121,
	advance_tick = 122,
	chat_message = 123,

	// console cheats
	switch_nation = 128,
	c_change_diplo_points = 129,
	c_change_money = 130,
	c_westernize = 131,
	c_unwesternize = 132,
	c_change_research_points = 133,
	c_change_cb_progress = 134,
	c_change_infamy = 135,
	c_force_crisis = 136,
	c_change_national_militancy = 137,
	c_end_game = 138,
	c_event = 139,
	c_event_as = 140
};

struct national_focus_data {
	dcon::state_instance_id target_state;
	dcon::national_focus_id focus;
};

struct start_research_data {
	dcon::technology_id tech;
};

struct make_leader_data {
	bool is_general;
};

struct save_game_data {
	bool and_quit;
};

struct province_building_data {
	dcon::province_id location;
	economy::province_building_type type;
};

struct factory_building_data {
	dcon::state_instance_id location;
	dcon::factory_type_id type;
	bool is_upgrade;
};

struct diplo_action_data {
	dcon::nation_id target;
};

struct naval_unit_construction_data {
	dcon::province_id location;
	dcon::unit_type_id type;
};

struct rally_point_data {
	dcon::province_id location;
	bool naval = false;
	bool enable = false;
};

struct land_unit_construction_data {
	dcon::province_id location;
	dcon::culture_id pop_culture;
	dcon::unit_type_id type;
};

struct factory_data {
	dcon::province_id location;
	dcon::factory_type_id type;
	uint8_t priority;
	bool subsidize;
};

struct tag_target_data {
	dcon::national_identity_id ident;
};

struct influence_action_data {
	dcon::nation_id influence_target;
	dcon::nation_id gp_target;
};

struct influence_priority_data {
	dcon::nation_id influence_target;
	uint8_t priority;
};

struct generic_location_data {
	dcon::province_id prov;
};

struct movement_data {
	dcon::issue_option_id iopt;
	dcon::national_identity_id tag;
};

struct political_party_data {
	dcon::political_party_id p;
};

struct reform_selection_data {
	dcon::reform_option_id r;
};

struct issue_selection_data {
	dcon::issue_option_id r;
};

struct budget_settings_data {
	int8_t education_spending;
	int8_t military_spending;
	int8_t administrative_spending;
	int8_t social_spending;
	int8_t land_spending;
	int8_t naval_spending;
	int8_t construction_spending;
	int8_t poor_tax;
	int8_t middle_tax;
	int8_t rich_tax;
	int8_t tariffs;
};

struct war_target_data {
	dcon::war_id war;
	bool for_attacker;
};

struct crisis_join_data {
	bool join_attackers;
};

struct stockpile_settings_data {
	float amount;
	dcon::commodity_id c;
	bool draw_on_stockpiles;
};

struct decision_data {
	dcon::decision_id d;
};

struct message_data {
	dcon::nation_id from;
	diplomatic_message::type type;
	bool accept;
};

struct call_to_arms_data {
	dcon::nation_id target;
	dcon::war_id war;
};

struct pending_human_n_event_data {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	int32_t primary_slot;
	int32_t from_slot;
	dcon::national_event_id e;
	sys::date date;
	uint8_t opt_choice;
	event::slot_type pt;
	event::slot_type ft;
};
struct pending_human_f_n_event_data {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	dcon::free_national_event_id e;
	sys::date date;
	uint8_t opt_choice;
};
struct pending_human_p_event_data {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	int32_t from_slot;
	dcon::provincial_event_id e;
	dcon::province_id p;
	sys::date date;
	uint8_t opt_choice;
	event::slot_type ft;
};
struct pending_human_f_p_event_data {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	dcon::free_provincial_event_id e;
	dcon::province_id p;
	sys::date date;
	uint8_t opt_choice;
};

struct cb_fabrication_data {
	dcon::nation_id target;
	dcon::cb_type_id type;
};

struct new_war_data {
	dcon::nation_id target;
	dcon::state_definition_id cb_state;
	dcon::national_identity_id cb_tag;
	dcon::nation_id cb_secondary_nation;
	dcon::cb_type_id primary_cb;
	bool call_attacker_allies;
};

struct new_war_goal_data {
	dcon::nation_id target;
	dcon::state_definition_id cb_state;
	dcon::national_identity_id cb_tag;
	dcon::nation_id cb_secondary_nation;
	dcon::war_id war;
	dcon::cb_type_id cb_type;
};

struct crisis_invitation_data {
	dcon::nation_id invited;
	dcon::nation_id target;
	dcon::state_definition_id cb_state;
	dcon::national_identity_id cb_tag;
	dcon::nation_id cb_secondary_nation;
	dcon::cb_type_id cb_type;
};

struct new_offer_data {
	dcon::nation_id target;
	dcon::war_id war;
	bool is_concession;
};
struct offer_wargoal_data {
	dcon::wargoal_id wg;
};

struct army_movement_data {
	dcon::army_id a;
	dcon::province_id dest;
};

struct navy_movement_data {
	dcon::navy_id n;
	dcon::province_id dest;
};

struct merge_army_data {
	dcon::army_id a;
	dcon::army_id b;
};

struct merge_navy_data {
	dcon::navy_id a;
	dcon::navy_id b;
};

struct new_general_data {
	dcon::army_id a;
	dcon::leader_id l;
};

struct new_admiral_data {
	dcon::navy_id a;
	dcon::leader_id l;
};

struct naval_battle_data {
	dcon::naval_battle_id b;
};

struct land_battle_data {
	dcon::land_battle_id b;
};

constexpr inline size_t num_packed_units = 10;

struct split_regiments_data {
	dcon::regiment_id regs[num_packed_units];
};
struct split_ships_data {
	dcon::ship_id ships[num_packed_units];
};

struct cheat_data {
	float value;
};
struct cheat_data_int {
	int32_t value;
};
struct cheat_event_data {
	int32_t value;
	dcon::nation_id as;
};

struct chat_message_data {
	char body[ui::max_chat_message_len];
	dcon::nation_id target;
};

struct nation_pick_data {
	dcon::nation_id target;
};

struct advance_tick_data {
	uint32_t checksum;
};

struct payload {
	union dtype {
		national_focus_data nat_focus;
		start_research_data start_research;
		make_leader_data make_leader;
		province_building_data start_province_building;
		diplo_action_data diplo_action;
		factory_building_data start_factory_building;
		naval_unit_construction_data naval_unit_construction;
		land_unit_construction_data land_unit_construction;
		tag_target_data tag_target;
		factory_data factory;
		budget_settings_data budget_data;
		influence_action_data influence_action;
		influence_priority_data influence_priority;
		generic_location_data generic_location;
		war_target_data war_target;
		movement_data movement;
		political_party_data political_party;
		reform_selection_data reform_selection;
		issue_selection_data issue_selection;
		crisis_join_data crisis_join;
		stockpile_settings_data stockpile_settings;
		decision_data decision;
		pending_human_n_event_data pending_human_n_event;
		pending_human_f_n_event_data pending_human_f_n_event;
		pending_human_p_event_data pending_human_p_event;
		pending_human_f_p_event_data pending_human_f_p_event;
		cb_fabrication_data cb_fabrication;
		message_data message;
		call_to_arms_data call_to_arms;
		new_war_data new_war;
		new_war_goal_data new_war_goal;
		new_offer_data new_offer;
		offer_wargoal_data offer_wargoal;
		cheat_data cheat;
		army_movement_data army_movement;
		navy_movement_data navy_movement;
		merge_army_data merge_army;
		merge_navy_data merge_navy;
		split_regiments_data split_regiments;
		split_ships_data split_ships;
		naval_battle_data naval_battle;
		land_battle_data land_battle;
		crisis_invitation_data crisis_invitation;
		new_general_data new_general;
		new_admiral_data new_admiral;
		nation_pick_data nation_pick;
		chat_message_data chat_message;
		rally_point_data rally_point;
		cheat_data_int cheat_int;
		cheat_event_data cheat_event;
		advance_tick_data advance_tick;
		save_game_data save_game;

		dtype() { }
	} data;
	dcon::nation_id source;
	command_type type = command_type::invalid;

	payload() { }
};

void save_game(sys::state& state, dcon::nation_id source, bool and_quit);

void set_rally_point(sys::state& state, dcon::nation_id source, dcon::province_id location, bool naval, bool enable);

bool is_console_command(command_type t);

void set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus);
bool can_set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus);

void start_research(sys::state& state, dcon::nation_id source, dcon::technology_id tech);
bool can_start_research(sys::state& state, dcon::nation_id source, dcon::technology_id tech);

void make_leader(sys::state& state, dcon::nation_id source, bool general);
bool can_make_leader(sys::state& state, dcon::nation_id source, bool general);

void decrease_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target);
bool can_decrease_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target);

void begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id p, economy::province_building_type type);
bool can_begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id p, economy::province_building_type type);

void begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location, dcon::factory_type_id type, bool is_upgrade);
bool can_begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location, dcon::factory_type_id type, bool is_upgrade);

void cancel_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location, dcon::factory_type_id type);
bool can_cancel_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location, dcon::factory_type_id type);

void start_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type);
bool can_start_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type);

void start_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::culture_id soldier_culture, dcon::unit_type_id type);
bool can_start_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::culture_id soldier_culture, dcon::unit_type_id type);

void cancel_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type);
bool can_cancel_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type);

void cancel_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::culture_id soldier_culture, dcon::unit_type_id type);
bool can_cancel_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::culture_id soldier_culture, dcon::unit_type_id type);

void delete_factory(sys::state& state, dcon::nation_id source, dcon::factory_id f);
bool can_delete_factory(sys::state& state, dcon::nation_id source, dcon::factory_id f);

void change_factory_settings(sys::state& state, dcon::nation_id source, dcon::factory_id f, uint8_t priority, bool subsidized);
bool can_change_factory_settings(sys::state& state, dcon::nation_id source, dcon::factory_id f, uint8_t priority, bool subsidized);

void make_vassal(sys::state& state, dcon::nation_id source, dcon::national_identity_id t);
bool can_make_vassal(sys::state& state, dcon::nation_id source, dcon::national_identity_id t);

void release_and_play_as(sys::state& state, dcon::nation_id source, dcon::national_identity_id t);
bool can_release_and_play_as(sys::state& state, dcon::nation_id source, dcon::national_identity_id t);

void give_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target);
bool can_give_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target);

void cancel_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target);
bool can_cancel_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target);

void increase_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target);
bool can_increase_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target);

inline budget_settings_data make_empty_budget_settings() {
	return budget_settings_data{int8_t(-127), int8_t(-127), int8_t(-127), int8_t(-127), int8_t(-127), int8_t(-127), int8_t(-127),
			int8_t(-127), int8_t(-127), int8_t(-127), int8_t(-127)};
}
// when sending new budget settings, leaving any value as int8_t(-127) will cause it to be ignored, leaving the setting the same
// You can use the function above to easily make an instance of the settings struct that will change no values
// Also, in consideration for future networking performance, do not send this command as the slider moves; only send it when the
// player has stopped dragging the slider, in the case of drag, or maybe even only when the window closes / a day passes while the
// window is open, if you think we can get away with it. In any case, we want to try to minimize how many times the command is
// sent per average interaction with the budget.
void change_budget_settings(sys::state& state, dcon::nation_id source, budget_settings_data const& values);
inline bool can_change_budget_settings(sys::state& state, dcon::nation_id source, budget_settings_data const& values);

void start_election(sys::state& state, dcon::nation_id source);
bool can_start_election(sys::state& state, dcon::nation_id source);

void change_influence_priority(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, uint8_t priority);
bool can_change_influence_priority(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, uint8_t priority);

void discredit_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp);
bool can_discredit_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp);

void expel_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp);
bool can_expel_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp);

void ban_embassy(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp);
bool can_ban_embassy(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp);

void increase_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target);
bool can_increase_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target);

void decrease_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp);
bool can_decrease_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp);

void add_to_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target);
bool can_add_to_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target);

void remove_from_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp);
bool can_remove_from_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp);

void upgrade_colony_to_state(sys::state& state, dcon::nation_id source, dcon::state_instance_id si);
bool can_upgrade_colony_to_state(sys::state& state, dcon::nation_id source, dcon::state_instance_id si);

void invest_in_colony(sys::state& state, dcon::nation_id source, dcon::province_id p);
bool can_invest_in_colony(sys::state& state, dcon::nation_id source, dcon::province_id p);

void abandon_colony(sys::state& state, dcon::nation_id source, dcon::province_id p);
bool can_abandon_colony(sys::state& state, dcon::nation_id source, dcon::province_id p);

void finish_colonization(sys::state& state, dcon::nation_id source, dcon::province_id p);
bool can_finish_colonization(sys::state& state, dcon::nation_id source, dcon::province_id p);

void intervene_in_war(sys::state& state, dcon::nation_id source, dcon::war_id w, bool for_attacker);
bool can_intervene_in_war(sys::state& state, dcon::nation_id source, dcon::war_id w, bool for_attacker);

void suppress_movement(sys::state& state, dcon::nation_id source, dcon::movement_id m);
bool can_suppress_movement(sys::state& state, dcon::nation_id source, dcon::movement_id m);

void civilize_nation(sys::state& state, dcon::nation_id source);
bool can_civilize_nation(sys::state& state, dcon::nation_id source);

void appoint_ruling_party(sys::state& state, dcon::nation_id source, dcon::political_party_id p);
bool can_appoint_ruling_party(sys::state& state, dcon::nation_id source, dcon::political_party_id p);

void enact_reform(sys::state& state, dcon::nation_id source, dcon::reform_option_id r);
bool can_enact_reform(sys::state& state, dcon::nation_id source, dcon::reform_option_id r);

void enact_issue(sys::state& state, dcon::nation_id source, dcon::issue_option_id i);
bool can_enact_issue(sys::state& state, dcon::nation_id source, dcon::issue_option_id i);

void become_interested_in_crisis(sys::state& state, dcon::nation_id source);
bool can_become_interested_in_crisis(sys::state& state, dcon::nation_id source);

void take_sides_in_crisis(sys::state& state, dcon::nation_id source, bool join_attacker);
bool can_take_sides_in_crisis(sys::state& state, dcon::nation_id source, bool join_attacker);

void change_stockpile_settings(sys::state& state, dcon::nation_id source, dcon::commodity_id c, float target_amount, bool draw_on_stockpiles);
bool can_change_stockpile_settings(sys::state& state, dcon::nation_id source, dcon::commodity_id c, float target_amount, bool draw_on_stockpiles);

void take_decision(sys::state& state, dcon::nation_id source, dcon::decision_id d);
bool can_take_decision(sys::state& state, dcon::nation_id source, dcon::decision_id d);

void make_event_choice(sys::state& state, event::pending_human_n_event const& e, uint8_t option_id);
void make_event_choice(sys::state& state, event::pending_human_f_n_event const& e, uint8_t option_id);
void make_event_choice(sys::state& state, event::pending_human_p_event const& e, uint8_t option_id);
void make_event_choice(sys::state& state, event::pending_human_f_p_event const& e, uint8_t option_id);

void fabricate_cb(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id type);
bool can_fabricate_cb(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id type);

void cancel_cb_fabrication(sys::state& state, dcon::nation_id source);
bool can_cancel_cb_fabrication(sys::state& state, dcon::nation_id source);

void ask_for_military_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target);
bool can_ask_for_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target, bool ignore_cost = false);

void give_military_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target);
bool can_give_military_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target);

void ask_for_alliance(sys::state& state, dcon::nation_id asker, dcon::nation_id target);
bool can_ask_for_alliance(sys::state& state, dcon::nation_id asker, dcon::nation_id target, bool ignore_cost = false);

void call_to_arms(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::war_id w);
void execute_call_to_arms(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::war_id w);
bool can_call_to_arms(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::war_id w, bool ignore_cost = false);

void respond_to_diplomatic_message(sys::state& state, dcon::nation_id source, dcon::nation_id from, diplomatic_message::type type, bool accept);

void cancel_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target);
bool can_cancel_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target);

void cancel_alliance(sys::state& state, dcon::nation_id source, dcon::nation_id target);
bool can_cancel_alliance(sys::state& state, dcon::nation_id source, dcon::nation_id target);

void cancel_given_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target); // this is for cancelling the access someone has with you
bool can_cancel_given_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target);

void declare_war(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation, bool call_attacker_allies);
bool can_declare_war(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation);

void add_war_goal(sys::state& state, dcon::nation_id source, dcon::war_id w, dcon::nation_id target, dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation);
bool can_add_war_goal(sys::state& state, dcon::nation_id source, dcon::war_id w, dcon::nation_id target, dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation);

// NOTE: sending an invalid province id will stop movement of an army or navy;
//     otherwise path will be appended to its current destination if any
// Thus, if you want to move the unit to a new location from its current location,
//     first stop its current movement and then send the new destination as a second command
// ALSO: can returns an empty vector if no path could be made
void move_army(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::province_id dest);
std::vector<dcon::province_id> can_move_army(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::province_id dest);

void move_navy(sys::state& state, dcon::nation_id source, dcon::navy_id n, dcon::province_id dest);
std::vector<dcon::province_id> can_move_navy(sys::state& state, dcon::nation_id source, dcon::navy_id n, dcon::province_id dest);

// This command is used for getting armies in/out of transports while those transports are docked in port
// If the army is already embarked, it will disembark; if it is not embarked it will embark
// Embarking an army onto/off a ship that is off the coast is done by moving the army into the sea/land tile instead.
void embark_army(sys::state& state, dcon::nation_id source, dcon::army_id a);
bool can_embark_army(sys::state& state, dcon::nation_id source, dcon::army_id a);

void merge_armies(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::army_id b);
bool can_merge_armies(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::army_id b);

void merge_navies(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::navy_id b);
bool can_merge_navies(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::navy_id b);

void split_army(sys::state& state, dcon::nation_id source, dcon::army_id a);
bool can_split_army(sys::state& state, dcon::nation_id source, dcon::army_id a);

void split_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a);
bool can_split_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a);

void delete_army(sys::state& state, dcon::nation_id source, dcon::army_id a);
bool can_delete_army(sys::state& state, dcon::nation_id source, dcon::army_id a);

void delete_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a);
bool can_delete_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a);

// Each ship / regiment carries a "to split" flag. When the split command is sent, any marked units will be split off into
//     a new army / navy and their split flag will be unset
// The commands below *toggle* the split flag (you can also use them to turn the flag off)
// Fill any unused slots with the invalid handle, but remember that each of these requires some network traffic
void mark_regiments_to_split(sys::state& state, dcon::nation_id source,
		std::array<dcon::regiment_id, num_packed_units> const& list);
void mark_ships_to_split(sys::state& state, dcon::nation_id source, std::array<dcon::ship_id, num_packed_units> const& list);

void retreat_from_naval_battle(sys::state& state, dcon::nation_id source, dcon::naval_battle_id b);
bool can_retreat_from_naval_battle(sys::state& state, dcon::nation_id source, dcon::naval_battle_id b);

void retreat_from_land_battle(sys::state& state, dcon::nation_id source, dcon::land_battle_id b);
bool can_retreat_from_land_battle(sys::state& state, dcon::nation_id source, dcon::land_battle_id b);

void change_general(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::leader_id l);
bool can_change_general(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::leader_id l);

void change_admiral(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::leader_id l);
bool can_change_admiral(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::leader_id l);

void invite_to_crisis(sys::state& state, dcon::nation_id source, dcon::nation_id invitation_to, dcon::nation_id target, dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation);
bool can_invite_to_crisis(sys::state& state, dcon::nation_id source, dcon::nation_id invitation_to, dcon::nation_id target, dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation);

void toggle_mobilization(sys::state& state, dcon::nation_id source);

/*
PEACE OFFER COMMANDS:

IMPORTANT:
Even though these are separate commands, they should be sent as a single sequence with no intermediate other commands:
send start_peace_offer then repeat add_to_peace_offer to populate it, and then send_peace_offer to finish the process
DO NOT attempt to issue these commands as the player constructs the offer in the ui
Note that crisis offers are constructed in basically the same way. You cannot have a normal peace offer under construction / in
flight while constructing / offering a crisis peace offer
*/

void start_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::war_id war, bool is_concession);
bool can_start_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::war_id war, bool is_concession);

void add_to_peace_offer(sys::state& state, dcon::nation_id source, dcon::wargoal_id goal);
bool can_add_to_peace_offer(sys::state& state, dcon::nation_id source, dcon::wargoal_id goal);

void send_peace_offer(sys::state& state, dcon::nation_id source);
bool can_send_peace_offer(sys::state& state, dcon::nation_id source);

// CRISIS PEACE OFFER COMMANDS

void start_crisis_peace_offer(sys::state& state, dcon::nation_id source, bool is_concession);
bool can_start_crisis_peace_offer(sys::state& state, dcon::nation_id source, bool is_concession);

void add_to_crisis_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id wargoal_from, dcon::nation_id target, dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation);
bool can_add_to_crisis_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id wargoal_from, dcon::nation_id target, dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation);

void send_crisis_peace_offer(sys::state& state, dcon::nation_id source);
bool can_send_crisis_peace_offer(sys::state& state, dcon::nation_id source);

void chat_message(sys::state& state, dcon::nation_id source, std::string_view body, dcon::nation_id target);
bool can_chat_message(sys::state& state, dcon::nation_id source, std::string_view body, dcon::nation_id target);

void notify_player_ban(sys::state& state, dcon::nation_id source, dcon::nation_id target);
bool can_notify_player_ban(sys::state& state, dcon::nation_id source, dcon::nation_id target);
void notify_player_kick(sys::state& state, dcon::nation_id source, dcon::nation_id target);
bool can_notify_player_kick(sys::state& state, dcon::nation_id source, dcon::nation_id target);
void notify_player_joins(sys::state& state, dcon::nation_id source);
bool can_notify_player_joins(sys::state& state, dcon::nation_id source);
void notify_player_leaves(sys::state& state, dcon::nation_id source);
bool can_notify_player_leaves(sys::state& state, dcon::nation_id source);
void notify_player_picks_nation(sys::state& state, dcon::nation_id source, dcon::nation_id target);
bool can_notify_player_picks_nation(sys::state& state, dcon::nation_id source, dcon::nation_id target);

void switch_nation(sys::state& state, dcon::nation_id source, dcon::national_identity_id t);
bool can_switch_nation(sys::state& state, dcon::nation_id source, dcon::national_identity_id t);
void c_change_diplo_points(sys::state& state, dcon::nation_id source, float value);
void c_change_money(sys::state& state, dcon::nation_id source, float value);
void c_westernize(sys::state& state, dcon::nation_id source);
void c_unwesternize(sys::state& state, dcon::nation_id source);
void c_change_research_points(sys::state& state, dcon::nation_id source, float value);
void c_change_cb_progress(sys::state& state, dcon::nation_id source, float value);
void c_change_infamy(sys::state& state, dcon::nation_id source, float value);
void c_force_crisis(sys::state& state, dcon::nation_id source);
void c_change_national_militancy(sys::state& state, dcon::nation_id source, float value);
void c_end_game(sys::state& state, dcon::nation_id source);
void c_event(sys::state& state, dcon::nation_id source, int32_t id);
void c_event_as(sys::state& state, dcon::nation_id source, dcon::nation_id as, int32_t id);

void execute_command(sys::state& state, payload& c);
void execute_pending_commands(sys::state& state);

} // namespace command
