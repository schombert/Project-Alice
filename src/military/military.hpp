#pragma once
#include <span>
#include "dcon_generated.hpp"
#include "container_types.hpp"
#include "modifiers.hpp"

namespace military {
namespace cb_flag {

inline constexpr uint32_t is_civil_war = 0x00000001;
inline constexpr uint32_t always = 0x00000002;
inline constexpr uint32_t is_triggered_only = 0x00000004;
inline constexpr uint32_t is_not_constructing_cb = 0x00000008;
inline constexpr uint32_t great_war_obligatory = 0x00000010;
inline constexpr uint32_t all_allowed_states = 0x00000020;
inline constexpr uint32_t not_in_crisis = 0x00000040;
inline constexpr uint32_t po_clear_union_sphere = 0x00000080;
inline constexpr uint32_t po_gunboat = 0x00000100;
inline constexpr uint32_t po_annex = 0x00000200;
inline constexpr uint32_t po_demand_state = 0x00000400;
inline constexpr uint32_t po_add_to_sphere = 0x00000800;
inline constexpr uint32_t po_disarmament = 0x00001000;
inline constexpr uint32_t po_reparations = 0x00002000;
inline constexpr uint32_t po_transfer_provinces = 0x00004000;
inline constexpr uint32_t po_remove_prestige = 0x00008000;
inline constexpr uint32_t po_make_puppet = 0x00010000;
inline constexpr uint32_t po_release_puppet = 0x00020000;
inline constexpr uint32_t po_status_quo = 0x00040000;
inline constexpr uint32_t po_install_communist_gov_type = 0x00080000;
inline constexpr uint32_t po_uninstall_communist_gov_type = 0x00100000;
inline constexpr uint32_t po_remove_cores = 0x00200000;
inline constexpr uint32_t po_colony = 0x00400000;
inline constexpr uint32_t po_destroy_forts = 0x00800000;
inline constexpr uint32_t po_destroy_naval_bases = 0x01000000;
inline constexpr uint32_t po_make_substate = 0x02000000;
inline constexpr uint32_t po_save_subjects = 0x04000000;
inline constexpr uint32_t po_unequal_treaty = 0x08000000;


} // namespace cb_flag

// The distance from one side of of the naval battle to the middle. Unit speed is cast to this distance with define:NAVAL_COMBAT_SPEED_TO_DISTANCE_FACTOR and naval_battle_speed_mult.
// The "total" distance for both sides is double this number, as each ship will start at 100 distance from the middle (which equals to 200 distance between them)
// the actual integer is 1000 units, which here means 100.0 with one fixed-point decimal.
constexpr uint16_t naval_battle_distance_to_center = 1000;

constexpr uint16_t naval_battle_total_distance = naval_battle_distance_to_center * 2; // total distance from one end of the battle to another

constexpr uint16_t naval_battle_center_line = 0; // The "center line" of a naval battle. Ships on one side cannot go past this.

constexpr uint16_t naval_battle_speed_mult = 1000; // mult for casting unit speed to battle speed


struct ship_in_battle {
	static constexpr uint16_t distance_mask = 0x03FF;

	static constexpr uint16_t mode_mask = 0x1C00;
	static constexpr uint16_t mode_seeking = 0x0400;
	static constexpr uint16_t mode_approaching = 0x0800;
	static constexpr uint16_t mode_engaged = 0x0C00;
	static constexpr uint16_t mode_retreating = 0x1000;
	static constexpr uint16_t mode_retreated = 0x1400;
	static constexpr uint16_t mode_sunk = 0x0000;

	static constexpr uint16_t is_attacking = 0x2000;

	static constexpr uint16_t type_mask = 0xC000;
	static constexpr uint16_t type_big = 0x4000;
	static constexpr uint16_t type_small = 0x8000;
	static constexpr uint16_t type_transport = 0x0000;

	dcon::ship_id ship;
	int16_t target_slot = -1;
	uint16_t flags = 0;
	uint16_t ships_targeting_this = 0;
	bool operator == (const ship_in_battle&) const = default;
	bool operator != (const ship_in_battle&) const = default;

	uint16_t get_distance() {
		return flags & distance_mask;
	}
	void set_distance(uint16_t distance) {
		flags &= ~distance_mask;
		flags |= distance_mask & (distance);

	}
};
static_assert(sizeof(ship_in_battle) ==
	sizeof(ship_in_battle::ship)
	+ sizeof(ship_in_battle::ships_targeting_this)
	+ sizeof(ship_in_battle::target_slot)
	+ sizeof(ship_in_battle::flags));

struct mobilization_order {
	sys::date when; //2
	dcon::province_id where; //2
};
static_assert(sizeof(mobilization_order) ==
	sizeof(mobilization_order::where)
	+ sizeof(mobilization_order::when));

struct reserve_regiment {
	static constexpr uint16_t is_attacking = 0x0001;

	static constexpr uint16_t type_mask = 0x0006;
	static constexpr uint16_t type_infantry = 0x0000;
	static constexpr uint16_t type_cavalry = 0x0002;
	static constexpr uint16_t type_support = 0x0004;

	dcon::regiment_id regiment;
	uint16_t flags = 0;
};
static_assert(sizeof(reserve_regiment) ==
	sizeof(reserve_regiment::regiment)
	+ sizeof(reserve_regiment::flags));

constexpr inline uint8_t defender_bonus_crossing_mask = 0xC0;
constexpr inline uint8_t defender_bonus_crossing_none = 0x00;
constexpr inline uint8_t defender_bonus_crossing_river = 0x40;
constexpr inline uint8_t defender_bonus_crossing_sea = 0x80;
constexpr inline uint8_t defender_bonus_dig_in_mask = 0x3F;

enum class unit_type : uint8_t {
	support, big_ship, cavalry, transport, light_ship, special, infantry
};

struct unit_definition : public sys::unit_variable_stats {
	economy::commodity_set build_cost;
	economy::commodity_set supply_cost;

	int32_t colonial_points = 0;
	int32_t min_port_level = 0;
	int32_t supply_consumption_score = 0;

	int32_t icon = 0;
	int32_t naval_icon = 0;

	dcon::text_key name;

	bool is_land = true;
	bool capital = false;
	bool can_build_overseas = true;
	bool primary_culture = false;
	bool active = true;

	unit_type type = unit_type::infantry;
	uint16_t padding = 0;

	unit_definition() { }
};
static_assert(sizeof(unit_definition) ==
	sizeof(sys::unit_variable_stats)
	+ sizeof(unit_definition::build_cost)
	+ sizeof(unit_definition::supply_cost)
	+ sizeof(unit_definition::colonial_points)
	+ sizeof(unit_definition::min_port_level)
	+ sizeof(unit_definition::supply_consumption_score)
	+ sizeof(unit_definition::icon)
	+ sizeof(unit_definition::naval_icon)
	+ sizeof(unit_definition::name)
	+ sizeof(unit_definition::is_land)
	+ sizeof(unit_definition::capital)
	+ sizeof(unit_definition::can_build_overseas)
	+ sizeof(unit_definition::primary_culture)
	+ sizeof(unit_definition::active)
	+ sizeof(unit_definition::type)
	+ sizeof(unit_definition::padding));

struct global_military_state {
	tagged_vector<unit_definition, dcon::unit_type_id> unit_base_definitions;

	dcon::leader_trait_id first_background_trait;

	bool great_wars_enabled = false;
	bool world_wars_enabled = false;

	dcon::unit_type_id base_army_unit;
	dcon::unit_type_id base_naval_unit;

	dcon::cb_type_id standard_civil_war;
	dcon::cb_type_id standard_great_war;

	dcon::cb_type_id standard_status_quo;

	dcon::cb_type_id liberate;
	dcon::cb_type_id uninstall_communist_gov;

	// CB type used to resolve crisis over colonizing the same state. Both parties have this WG.
	dcon::cb_type_id crisis_colony;
	/*
	CB type used to liberate a tag from the target in the liberation crisis.
	In vanilla - free_peoples. 
	po_transfer_provinces = yes
	*/
	dcon::cb_type_id crisis_liberate;
	/* This type of a wargoal will be used for annex nation crises (restore order cb for example) */
	dcon::cb_type_id crisis_annex;

	dcon::unit_type_id irregular;
	dcon::unit_type_id infantry;
	dcon::unit_type_id artillery;

	bool pending_blackflag_update = false;
};

struct available_cb {
	sys::date expiration; //2
	dcon::nation_id target; //2
	dcon::cb_type_id cb_type; //2
	dcon::state_definition_id target_state;
};
static_assert(sizeof(available_cb) ==
	+sizeof(available_cb::target)
	+ sizeof(available_cb::expiration)
	+ sizeof(available_cb::cb_type) +
	sizeof(available_cb::target_state));

struct wg_summary {
	dcon::nation_id secondary_nation;
	dcon::national_identity_id wg_tag;
	dcon::state_definition_id state;
	dcon::cb_type_id cb;
};

struct naval_battle_report {
	float warscore_effect = 0.0f;
	float prestige_effect = 0.0f;

	uint16_t attacker_big_ships;
	uint16_t attacker_small_ships;
	uint16_t attacker_transport_ships;

	uint16_t attacker_big_losses;
	uint16_t attacker_small_losses;
	uint16_t attacker_transport_losses;

	uint16_t defender_big_ships;
	uint16_t defender_small_ships;
	uint16_t defender_transport_ships;

	uint16_t defender_big_losses;
	uint16_t defender_small_losses;
	uint16_t defender_transport_losses;

	dcon::leader_id attacking_admiral;
	dcon::leader_id defending_admiral;

	dcon::nation_id attacking_nation;
	dcon::nation_id defending_nation;

	dcon::province_id location;

	bool attacker_won;
	bool player_on_winning_side;
};

struct land_battle_report {
	float warscore_effect = 0.0f;
	float prestige_effect = 0.0f;

	float attacker_infantry;
	float attacker_cavalry;
	float attacker_support;

	float attacker_infantry_losses;
	float attacker_cavalry_losses;
	float attacker_support_losses;

	float defender_infantry;
	float defender_cavalry;
	float defender_support;

	float defender_infantry_losses;
	float defender_cavalry_losses;
	float defender_support_losses;

	dcon::leader_id attacking_general;
	dcon::leader_id defending_general;

	dcon::nation_id attacking_nation;
	dcon::nation_id defending_nation;

	dcon::province_id location;

	bool attacker_won;
	bool player_on_winning_side;
};

struct arrival_time_info {
	sys::date arrival_time;
	float unused_travel_days;
};
struct arrival_time_info_raw {
	int32_t travel_days;
	float unused_travel_days;
};


struct naval_range_display_data {
	dcon::province_id closest_port;
	float distance;
	sys::date timestamp;
};

constexpr inline int32_t days_before_retreat = 11;

enum class battle_result {
	indecisive, attacker_won, defender_won
};
enum class regiment_dmg_source {
	combat, attrition
};

struct ai_path_length {
	uint32_t length = 0;
	bool operator==(const ai_path_length& other) const = default;
	bool operator!=(const ai_path_length& other) const = default;

};

void reset_unit_stats(sys::state& state);
void apply_base_unit_stat_modifiers(sys::state& state);
void restore_unsaved_values(sys::state& state); // must run after determining connectivity

bool is_infantry_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given);
bool is_artillery_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given);
bool is_cavalry_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given);
bool is_transport_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given);
bool is_light_ship_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given);
bool is_big_ship_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given);

dcon::unit_type_id get_best_infantry(sys::state& state, dcon::nation_id n, bool primary_culture = false, bool evaluate_shortages = true);
dcon::unit_type_id get_best_artillery(sys::state& state, dcon::nation_id n, bool primary_culture = false, bool evaluate_shortages = true);
dcon::unit_type_id get_best_cavalry(sys::state& state, dcon::nation_id n, bool primary_culture = false, bool evaluate_shortages = true);
dcon::unit_type_id get_best_transport(sys::state& state, dcon::nation_id n, bool primary_culture = false, bool evaluate_shortages = true);
dcon::unit_type_id get_best_light_ship(sys::state& state, dcon::nation_id n, bool primary_culture = false, bool evaluate_shortages = true);
dcon::unit_type_id get_best_big_ship(sys::state& state, dcon::nation_id n, bool primary_culture = false, bool evaluate_shortages = true);

bool are_enemies(sys::state const& state, dcon::nation_id a, dcon::nation_id b);
bool are_at_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b);
bool are_allied_in_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b);
bool are_in_common_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b);
void remove_from_common_allied_wars(sys::state& state, dcon::nation_id a, dcon::nation_id b);
dcon::war_id find_war_between(sys::state const& state, dcon::nation_id a, dcon::nation_id b);
bool has_truce_with(sys::state& state, dcon::nation_id attacker, dcon::nation_id target);
sys::date truce_end_date(sys::state& state, dcon::nation_id attacker, dcon::nation_id target);
bool can_use_cb_against(sys::state& state, dcon::nation_id from, dcon::nation_id target);
bool leader_is_in_combat(sys::state& state, dcon::leader_id l);
dcon::leader_id make_new_leader(sys::state& state, dcon::nation_id n, bool is_general);
void kill_leader(sys::state& state, dcon::leader_id l);

void give_back_units(sys::state& state, dcon::nation_id target);

// tests whether joining the war would violate the constraint that you can't both be in a war with and
// fighting against the same nation or fighting against them twice
bool joining_war_does_not_violate_constraints(sys::state const& state, dcon::nation_id a, dcon::war_id w, bool as_attacker);

bool is_civil_war(sys::state const& state, dcon::war_id w);
bool standard_war_joining_is_possible(sys::state& state, dcon::war_id wfor, dcon::nation_id n, bool as_attacker);
bool joining_as_attacker_would_break_truce(sys::state& state, dcon::nation_id a, dcon::war_id w);
bool defenders_have_non_status_quo_wargoal(sys::state const& state, dcon::war_id w);
bool defenders_have_status_quo_wargoal(sys::state const& state, dcon::war_id w);
bool attackers_have_status_quo_wargoal(sys::state const& state, dcon::war_id w);
bool can_add_always_cb_to_war(sys::state& state, dcon::nation_id actor, dcon::nation_id target, dcon::cb_type_id cb, dcon::war_id w);
bool is_attacker(sys::state& state, dcon::war_id w, dcon::nation_id n);
bool war_goal_would_be_duplicate(sys::state& state, dcon::nation_id source, dcon::war_id w, dcon::nation_id target, dcon::cb_type_id cb_type, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation);
bool state_claimed_in_war(sys::state& state, dcon::war_id w, dcon::nation_id from, dcon::nation_id target, dcon::state_definition_id cb_state);
void set_initial_leaders(sys::state& state);
std::string get_war_name(sys::state& state, dcon::war_id war);

// war score from the perspective of the primary attacker offering a peace deal to the primary defender; -100 to 100
float primary_warscore(sys::state& state, dcon::war_id w);
float primary_warscore_from_occupation(sys::state& state, dcon::war_id w);
float primary_warscore_from_battles(sys::state& state, dcon::war_id w);
float primary_warscore_from_war_goals(sys::state& state, dcon::war_id w);
float primary_warscore_from_blockades(sys::state& state, dcon::war_id w);

// war score from the perspective of the primary nation offering peace to the secondary nation; 0 to 100
// DO NOT use this when calculating the overall score of the war or when looking at a peace deal between primary attacker and
// defender
float directed_warscore(sys::state& state, dcon::war_id w, dcon::nation_id primary, dcon::nation_id secondary);

bool is_defender_wargoal(sys::state const& state, dcon::war_id w, dcon::wargoal_id wg);

enum class war_role {
	none, attacker, defender
};
war_role get_role(sys::state const& state, dcon::war_id w, dcon::nation_id n);

bool province_is_blockaded(sys::state const& state, dcon::province_id ids);
bool province_is_under_siege(sys::state const& state, dcon::province_id ids);
void update_blockade_status(sys::state& state);

float recruited_pop_fraction(sys::state const& state, dcon::nation_id n);
bool state_has_naval_base(sys::state const& state, dcon::state_instance_id si);
uint32_t state_naval_base_level(sys::state const& state, dcon::state_instance_id si);
uint32_t state_railroad_level(sys::state const& state, dcon::state_instance_id si);

int32_t supply_limit_in_province(sys::state& state, dcon::nation_id n, dcon::province_id p);
int32_t regiments_possible_from_pop(sys::state& state, dcon::pop_id p);
int32_t regiments_created_from_province(sys::state& state, dcon::province_id p); // does not include mobilized regiments
int32_t regiments_max_possible_from_province(sys::state& state, dcon::province_id p);
int32_t main_culture_regiments_created_from_province(sys::state& state, dcon::province_id p);
int32_t main_culture_regiments_max_possible_from_province(sys::state& state, dcon::province_id p);
int32_t regiments_under_construction_in_province(sys::state& state, dcon::province_id p);
int32_t main_culture_regiments_under_construction_in_province(sys::state& state, dcon::province_id p);
int32_t mobilized_regiments_created_from_province(sys::state& state, dcon::province_id p);
int32_t mobilized_regiments_possible_from_province(sys::state& state, dcon::province_id p);
dcon::pop_id find_available_soldier(sys::state& state, dcon::province_id p, dcon::culture_id pop_culture);
int32_t mobilized_regiments_pop_limit(sys::state& state, dcon::nation_id n);
uint8_t make_dice_rolls(sys::state& state, uint32_t seed);

//implementation function for deciding if a specific pop with the given divisor can form another regiment. Should not be called directly except in the find_available_soldier calls
bool can_pop_form_regiment(sys::state& state, dcon::pop_id pop, float divisor);

int32_t total_regiments(sys::state& state, dcon::nation_id n);
int32_t total_ships(sys::state& state, dcon::nation_id n);

dcon::regiment_id create_new_regiment(sys::state& state, dcon::nation_id n, dcon::unit_type_id t);
dcon::ship_id create_new_ship(sys::state& state, dcon::nation_id n, dcon::unit_type_id t);

void update_recruitable_regiments(sys::state& state, dcon::nation_id n);
void update_all_recruitable_regiments(sys::state& state);
void regenerate_total_regiment_counts(sys::state& state);

void regenerate_land_unit_average(sys::state& state);
void regenerate_ship_scores(sys::state& state);

int32_t naval_supply_points(sys::state& state, dcon::nation_id n);
int32_t naval_supply_points_used(sys::state& state, dcon::nation_id n);

float mobilization_size(sys::state const& state, dcon::nation_id n);
float mobilization_impact(sys::state const& state, dcon::nation_id n);
ve::fp_vector ve_mobilization_impact(sys::state const& state, ve::tagged_vector<dcon::nation_id> nations);

float get_ship_combat_score(sys::state& state, dcon::ship_id ship);

naval_range_display_data closest_naval_range_port_with_distance(sys::state& state, dcon::province_id prov, dcon::nation_id nation);

uint32_t naval_supply_from_naval_base(sys::state& state, dcon::province_id prov, dcon::nation_id nation);
void update_naval_supply_points(sys::state& state); // must run after determining connectivity
void update_cbs(sys::state& state);
float calculate_monthly_leadership_points(sys::state& state, dcon::nation_id nation);
void monthly_leaders_update(sys::state& state);
void daily_leaders_update(sys::state& state);

bool cb_conditions_satisfied(sys::state& state, dcon::nation_id actor, dcon::nation_id target, dcon::cb_type_id cb);
bool cb_instance_conditions_satisfied(sys::state& state, dcon::nation_id actor, dcon::nation_id target, dcon::cb_type_id cb, dcon::state_definition_id st, dcon::national_identity_id tag, dcon::nation_id secondary);
void add_cb(sys::state& state, dcon::nation_id n, dcon::cb_type_id cb, dcon::nation_id target, dcon::state_definition_id target_state); // do not call this function directly unless you know what you are doing
void execute_cb_discovery(sys::state& state, dcon::nation_id n);

dcon::nation_id get_effective_unit_commander(sys::state& state, dcon::army_id unit);
dcon::nation_id get_effective_unit_commander(sys::state& state, dcon::navy_id unit);

void give_military_access(sys::state& state, dcon::nation_id accessing_nation, dcon::nation_id target);
void remove_military_access(sys::state& state, dcon::nation_id accessing_nation, dcon::nation_id target);
void end_wars_between(sys::state& state, dcon::nation_id a, dcon::nation_id b);

dcon::war_id create_war(sys::state& state, dcon::nation_id primary_attacker, dcon::nation_id primary_defender,
		dcon::cb_type_id primary_wargoal, dcon::state_definition_id primary_wargoal_state,
		dcon::national_identity_id primary_wargoal_tag, dcon::nation_id primary_wargoal_secondary);
void call_defender_allies(sys::state& state, dcon::war_id wfor);
void call_attacker_allies(sys::state& state, dcon::war_id wfor);
void add_wargoal(sys::state& state, dcon::war_id wfor, dcon::nation_id added_by, dcon::nation_id target, dcon::cb_type_id type,
		dcon::state_definition_id sd, dcon::national_identity_id tag, dcon::nation_id secondary_nation);
void add_to_war(sys::state& state, dcon::war_id w, dcon::nation_id n, bool as_attacker, bool on_war_creation = false);

float truce_break_cb_prestige_cost(sys::state& state, dcon::cb_type_id type);
float truce_break_cb_militancy(sys::state& state, dcon::cb_type_id type);
float truce_break_cb_infamy(sys::state& state, dcon::cb_type_id type, dcon::nation_id target, dcon::state_definition_id cb_state = dcon::state_definition_id{});


int32_t province_point_cost(sys::state& state, dcon::province_id p, dcon::nation_id n);
int32_t peace_cost(sys::state& state, dcon::war_id war, dcon::cb_type_id wargoal, dcon::nation_id from, dcon::nation_id target,
		dcon::nation_id secondary_nation, dcon::state_definition_id wargoal_state, dcon::national_identity_id wargoal_t);
int32_t cost_of_peace_offer(sys::state& state, dcon::peace_offer_id offer);
int32_t peace_offer_truce_months(sys::state& state, dcon::peace_offer_id offer);
int32_t attacker_peace_cost(sys::state& state, dcon::war_id war);
int32_t defender_peace_cost(sys::state& state, dcon::war_id war);

float successful_cb_prestige(sys::state& state, dcon::cb_type_id type, dcon::nation_id actor);
float cb_infamy(sys::state& state, dcon::cb_type_id t, dcon::nation_id target, dcon::state_definition_id cb_state = dcon::state_definition_id{}); // the fabrication cost in infamy

float war_declaration_infamy_cost(sys::state& state, dcon::cb_type_id type, dcon::nation_id from,
		dcon::nation_id target, dcon::state_definition_id cb_state = dcon::state_definition_id{ }); // the cost of starting a war with a CB -- does NOT check if the CB is valid to add
float cb_addition_infamy_cost(sys::state& state, dcon::war_id war, dcon::cb_type_id type, dcon::nation_id from,
		dcon::nation_id target, dcon::state_definition_id cb_state = dcon::state_definition_id{ }); // the cost of adding a particular cb to the war -- does NOT check if the CB is valid to add
float crisis_cb_addition_infamy_cost(sys::state& state, dcon::cb_type_id type, dcon::nation_id from, dcon::nation_id target, dcon::state_definition_id cb_state);

bool cb_requires_selection_of_a_valid_nation(sys::state const& state, dcon::cb_type_id t);
bool cb_requires_selection_of_a_liberatable_tag(sys::state const& state, dcon::cb_type_id t);
bool cb_requires_selection_of_a_state(sys::state const& state, dcon::cb_type_id t);

void remove_from_war(sys::state& state, dcon::war_id w, dcon::nation_id n, bool as_loss);
enum class war_result {
	draw, attacker_won, defender_won
};
void cleanup_war(sys::state& state, dcon::war_id w, war_result result);

void cleanup_army(sys::state& state, dcon::army_id n);
void cleanup_navy(sys::state& state, dcon::navy_id n);

void merge_navies_impl(sys::state& state, dcon::navy_id a, dcon::navy_id b);

void implement_war_goal(sys::state& state, dcon::war_id war, dcon::cb_type_id wargoal, dcon::nation_id from,
		dcon::nation_id target, dcon::nation_id secondary_nation, dcon::state_definition_id wargoal_state,
		dcon::national_identity_id wargoal_t);
void implement_peace_offer(sys::state& state, dcon::peace_offer_id offer);
void reject_peace_offer(sys::state& state, dcon::peace_offer_id offer);
void add_truce(sys::state& state, dcon::nation_id a, dcon::nation_id b, int32_t days);

void update_ticking_war_score(sys::state& state);

void start_mobilization(sys::state& state, dcon::nation_id n);
void end_mobilization(sys::state& state, dcon::nation_id n);
void advance_mobilizations(sys::state& state);

int32_t transport_capacity(sys::state& state, dcon::navy_id n);
int32_t free_transport_capacity(sys::state& state, dcon::navy_id n);

void upgrade_ship(sys::state& state, dcon::ship_id ship, dcon::unit_type_id new_type);
void upgrade_regiment(sys::state& state, dcon::regiment_id ship, dcon::unit_type_id new_type);

bool can_embark_onto_sea_tile(sys::state& state, dcon::nation_id n, dcon::province_id p, dcon::army_id a);
dcon::navy_id find_embark_target(sys::state& state, dcon::nation_id from, dcon::province_id p, dcon::army_id a);
float effective_army_speed(sys::state& state, dcon::army_id a);
float effective_navy_speed(sys::state& state, dcon::navy_id n);
bool will_recieve_attrition(sys::state& state, dcon::navy_id a);
bool will_recieve_attrition(sys::state& state, dcon::army_id a);
float attrition_amount(sys::state& state, dcon::navy_id a);
float attrition_amount(sys::state& state, dcon::army_id a);
float relative_attrition_amount(sys::state& state, dcon::navy_id a, dcon::province_id prov);
float relative_attrition_amount(sys::state& state, dcon::army_id a, dcon::province_id prov, float additional_army_weight = 0.0f);
float relative_attrition_amount(sys::state& state, dcon::army_id a, dcon::province_id prov, const std::vector<dcon::army_id>& extra_armies);
float local_army_weight(sys::state& state, dcon::province_id prov);
float local_army_weight_max(sys::state& state, dcon::province_id prov);
float local_enemy_army_weight_max(sys::state& state, dcon::province_id prov, dcon::nation_id nation);
float attrition_amount(sys::state& state, dcon::navy_id a);
float attrition_amount(sys::state& state, dcon::army_id a);
float peacetime_attrition_limit(sys::state& state, dcon::nation_id n, dcon::province_id prov);

enum class reinforcement_estimation_type {
	today, monthly, full_supplies
};


template<reinforcement_estimation_type reinf_est_type>
float calculate_army_combined_reinforce(sys::state& state, dcon::army_id a);

void reduce_regiment_strength_safe(sys::state& state, dcon::regiment_id reg, float value);
void reduce_ship_strength_safe(sys::state& state, dcon::ship_id reg, float value);

template<regiment_dmg_source damage_source>
void regiment_take_damage(sys::state& state, dcon::regiment_id reg, float value);

float movement_time_from_to(sys::state& state, dcon::army_id a, dcon::province_id from, dcon::province_id to);
float movement_time_from_to(sys::state& state, dcon::navy_id n, dcon::province_id from, dcon::province_id to);
arrival_time_info arrival_time_to(sys::state& state, dcon::army_id a, dcon::province_id p);
arrival_time_info arrival_time_to(sys::state& state, dcon::navy_id n, dcon::province_id p);
arrival_time_info_raw arrival_time_to_in_days(sys::state& state, dcon::army_id a, dcon::province_id to, dcon::province_id from);
arrival_time_info_raw arrival_time_to_in_days(sys::state& state, dcon::navy_id n, dcon::province_id to, dcon::province_id from);
float fractional_distance_covered(sys::state& state, dcon::army_id a);
float fractional_distance_covered(sys::state& state, dcon::navy_id a);

template<typename T>
void update_movement_arrival_days(sys::state& state, dcon::province_id to, dcon::province_id from, T army, float& unused_travel_days, sys::date& arrival_time);

template<typename T>
void update_movement_arrival_days_on_unit(sys::state& state, dcon::province_id to, dcon::province_id from, T army);

enum class crossing_type {
	none, river, sea
};

enum class apply_attrition_on_arrival {
	no, yes

};

enum class battle_is_ending {
	no, yes
};

template <apply_attrition_on_arrival attrition_tick = apply_attrition_on_arrival::no>
void army_arrives_in_province(sys::state& state, dcon::army_id a, dcon::province_id p, crossing_type crossing, dcon::land_battle_id from = dcon::land_battle_id{}); // only for land provinces
void navy_arrives_in_province(sys::state& state, dcon::navy_id n, dcon::province_id p, dcon::naval_battle_id from = dcon::naval_battle_id{}); // only for sea provinces

std::vector<dcon::nation_id> get_one_side_war_participants(sys::state& state, dcon::war_id war, bool attackers);

template<battle_is_ending battle_state>
bool retreat(sys::state& state, dcon::navy_id n);

void end_battle(sys::state& state, dcon::naval_battle_id b, battle_result result);
void end_battle(sys::state& state, dcon::land_battle_id b, battle_result result);

void invalidate_unowned_wargoals(sys::state& state);
void update_blackflag_status(sys::state& state, dcon::province_id p);
void eject_ships(sys::state& state, dcon::province_id p);
void update_movement(sys::state& state);
bool siege_potential(sys::state& state, dcon::nation_id army_controller, dcon::nation_id province_controller);
void update_siege_progress(sys::state& state);
void single_ship_start_retreat(sys::state& state, ship_in_battle& ship, dcon::naval_battle_id battle);
float required_avg_dist_to_center_for_retreat(sys::state& state);
void update_naval_battles(sys::state& state);
void update_land_battles(sys::state& state);
void apply_regiment_damage(sys::state& state);
uint16_t unit_type_to_reserve_regiment_type(unit_type utype);
float naval_battle_get_coordination_penalty(sys::state& state, uint32_t friendly_ships, uint32_t enemy_ships);
float naval_battle_get_coordination_bonus(sys::state& state, uint32_t friendly_ships, uint32_t enemy_ships);
uint32_t get_reserves_count_by_side(sys::state& state, dcon::land_battle_id b, bool attacker);
float get_damage_reduction_stacking_penalty(sys::state& state, uint32_t friendly_ships, uint32_t enemy_ships);
void add_regiment_to_reserves(sys::state& state, dcon::land_battle_id bat, dcon::regiment_id reg, bool is_attacking);
bool is_regiment_in_reserve(sys::state& state, dcon::regiment_id reg);
void sort_reserves_by_deployment_order(sys::state& state, dcon::dcon_vv_fat_id<reserve_regiment> reserves);
uint8_t get_effective_battle_dig_in(sys::state& state, dcon::land_battle_id battle);
float get_army_recon_eff(sys::state& state, dcon::army_id army);
float get_army_siege_eff(sys::state& state, dcon::army_id army);
dcon::nation_id tech_nation_for_army(sys::state& state, dcon::army_id army);
dcon::regiment_id get_land_combat_target(sys::state& state, dcon::regiment_id damage_dealer, int32_t position, const std::array<dcon::regiment_id, 30>& opposing_line);
void apply_attrition_to_army(sys::state& state, dcon::army_id army);
void apply_attrition(sys::state& state);
void increase_dig_in(sys::state& state);
economy::commodity_set get_required_supply(sys::state& state, dcon::nation_id owner, dcon::army_id army);
economy::commodity_set get_required_supply(sys::state& state, dcon::nation_id owner, dcon::navy_id navy);
void recover_org(sys::state& state);
float calculate_location_reinforce_modifier_battle(sys::state& state, dcon::province_id location, dcon::nation_id in_nation);
float unit_get_strength(sys::state& state, dcon::regiment_id regiment_id);
float unit_get_strength(sys::state& state, dcon::ship_id ship_id);
// stops the unit movement completly and clears all other auxillary movement effects (arrival date, path etc)
void stop_navy_movement(sys::state& state, dcon::navy_id navy);
// stops the unit movement completly and clears all other auxillary movement effects (arrival date, path etc)
void stop_army_movement(sys::state& state, dcon::army_id army);

bool province_has_enemy_fleet(sys::state& state, dcon::province_id location, dcon::nation_id our_nation);
float calculate_battle_reinforcement(sys::state& state, dcon::land_battle_id b, bool attacker);
float calculate_average_battle_supply_spending(sys::state& state, dcon::land_battle_id b, bool attacker);
float calculate_average_battle_location_modifier(sys::state& state, dcon::land_battle_id b, bool attacker);
float calculate_average_battle_national_modifiers(sys::state& state, dcon::land_battle_id b, bool attacker);

template<reinforcement_estimation_type reinf_estimation>
float unit_calculate_reinforcement(sys::state& state, dcon::regiment_id reg, bool potential_reinf = false);

template<reinforcement_estimation_type reinf_estimation>
float unit_calculate_reinforcement(sys::state& state, dcon::ship_id reg);
void reinforce_regiments(sys::state& state);
void repair_ships(sys::state& state);
void run_gc(sys::state& state);
void update_blackflag_status(sys::state& state);
void send_rebel_hunter_to_next_province(sys::state& state, dcon::army_id ar, dcon::province_id prov);

bool can_retreat_from_battle(sys::state& state, dcon::naval_battle_id battle);
bool can_retreat_from_battle(sys::state& state, dcon::land_battle_id battle);

dcon::nation_id get_land_battle_lead_attacker(sys::state& state, dcon::land_battle_id b);
dcon::nation_id get_land_battle_lead_defender(sys::state& state, dcon::land_battle_id b);
dcon::nation_id get_naval_battle_lead_defender(sys::state& state, dcon::naval_battle_id b);
dcon::nation_id get_naval_battle_lead_attacker(sys::state& state, dcon::naval_battle_id b);

float get_leader_select_score(sys::state& state, dcon::leader_id l, bool is_attacking);
bool is_attacker_in_battle(sys::state& state, dcon::army_id a);
bool is_attacker_in_battle(sys::state& state, dcon::navy_id a);
dcon::leader_trait_id get_leader_background_wrapper(sys::state& state, dcon::leader_id id);
dcon::leader_trait_id get_leader_personality_wrapper(sys::state& state, dcon::leader_id id);
void update_battle_leaders(sys::state& state, dcon::land_battle_id b);
void update_battle_leaders(sys::state& state, dcon::naval_battle_id b);

void delete_regiment_safe_wrapper(sys::state& state, dcon::regiment_id reg); // safely deletes the regiment even if the army is currently in a battle
bool rebel_army_in_province(sys::state& state, dcon::province_id p);
dcon::province_id find_land_rally_pt(sys::state& state, dcon::nation_id by, dcon::province_id start);
dcon::province_id find_naval_rally_pt(sys::state& state, dcon::nation_id by, dcon::province_id start);
void move_land_to_merge(sys::state& state, dcon::nation_id by, dcon::army_id a, dcon::province_id start, dcon::province_id dest);
void move_navy_to_merge(sys::state& state, dcon::nation_id by, dcon::navy_id a, dcon::province_id start, dcon::province_id dest);


// shortcut function for moving navies. skips most player-movement checks and assumes the move command is legitimate. Will return false if there is no valid path and no movement has happend.. 
// takes a path directly instead of calculating it
bool move_navy_fast(sys::state& state, dcon::navy_id navy, const std::span<dcon::province_id, std::dynamic_extent> naval_path, bool reset = true);

// shortcut function for moving navies. skips most player-movement checks and assumes the move command is legitimate. Will return false if there is no valid path and no movement has happend..
// if path_length_to_use is 0, use the entire path. Otherwise, it will only use said length of the path
template<ai_path_length path_length_to_use = ai_path_length{ 0 } >
bool move_navy_fast(sys::state& state, dcon::navy_id navy, dcon::province_id destination, bool reset = true);

// shortcut function for moving armies. skips most player-movement checks and assumes the move command is legitimate. Will return false if there is no valid path and no movement has happend.
// takes a path directly instead of calculating it
bool move_army_fast(sys::state& state, dcon::army_id army, const std::span<dcon::province_id, std::dynamic_extent>, dcon::nation_id nation_as, bool reset = true);

// shortcut function for moving armies. skips most player-movement checks and assumes the move command is legitimate. Will return false if there is no valid path and no movement has happend..
// if path_length_to_use is 0, use the entire path. Otherwise, it will only use said length of the path
template<ai_path_length path_length_to_use = ai_path_length{ 0 } >
bool move_army_fast(sys::state& state, dcon::army_id army, dcon::province_id destination, dcon::nation_id nation_as, bool reset = true);

bool pop_eligible_for_mobilization(sys::state& state, dcon::pop_id p);

template<regiment_dmg_source damage_source>
void disband_regiment_w_pop_death(sys::state& state, dcon::regiment_id reg_id);

enum special_army_order {
	none,
	move_to_siege,
	strategic_redeployment,
	pursue_to_engage
};

} // namespace military
