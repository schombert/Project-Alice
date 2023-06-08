#pragma once
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

} // namespace cb_flag

enum class unit_type : uint8_t { support, big_ship, cavalry, transport, light_ship, special, infantry };

struct unit_definition : public sys::unit_variable_stats {
	economy::commodity_set build_cost;
	economy::commodity_set supply_cost;

	float discipline = 0.0f;
	float maneuver_or_evasion = 0.0f;
	int32_t colonial_points = 0;
	int32_t min_port_level = 0;
	int32_t supply_consumption_score = 0;

	int32_t icon = 0;
	int32_t naval_icon = 0;

	dcon::text_sequence_id name;

	bool is_land = true;
	bool capital = false;
	bool can_build_overseas = true;
	bool primary_culture = false;
	bool active = true;

	unit_type type = unit_type::infantry;

	unit_definition() { }
};

struct global_military_state {
	dcon::leader_trait_id first_background_trait;
	tagged_vector<unit_definition, dcon::unit_type_id> unit_base_definitions;
	bool great_wars_enabled = false;
	bool world_wars_enabled = false;

	dcon::unit_type_id base_army_unit;
	dcon::unit_type_id base_naval_unit;

	dcon::cb_type_id standard_civil_war;
	dcon::cb_type_id standard_great_war;

	dcon::cb_type_id liberate;
	dcon::cb_type_id uninstall_communist_gov;
};

struct available_cb {
	dcon::nation_id target;
	sys::date expiration;
	dcon::cb_type_id cb_type;
};

void reset_unit_stats(sys::state& state);
void apply_base_unit_stat_modifiers(sys::state& state);
void restore_unsaved_values(sys::state& state); // must run after determining connectivity

bool are_at_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b);
bool are_allied_in_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b);
bool are_in_common_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b);
dcon::war_id find_war_between(sys::state const& state, dcon::nation_id a, dcon::nation_id b);
bool has_truce_with(sys::state& state, dcon::nation_id attacker, dcon::nation_id target);
bool can_use_cb_against(sys::state& state, dcon::nation_id from, dcon::nation_id target);
bool leader_is_in_combat(sys::state& state, dcon::leader_id l);
bool joining_war_does_not_violate_constraints(sys::state const& state, dcon::nation_id a, dcon::war_id w,
		bool as_attacker); // tests whether joining the war would violate the constraint that you can't both be in a war with and
											 // fighting against the same nation or fighting against them twice
bool is_civil_war(sys::state const& state, dcon::war_id w);
bool joining_as_attacker_would_break_truce(sys::state& state, dcon::nation_id a, dcon::war_id w);
bool defenders_have_non_status_quo_wargoal(sys::state const& state, dcon::war_id w);

// war score from the perspective of the primary attacker offering a peace deal to the primary defender; -100 to 100
float primary_warscore(sys::state& state, dcon::war_id w);

// war score from the perspective of the primary nation offering peace to the secondary nation; 0 to 100
// DO NOT use this when calculating the overall score of the war or when looking at a peace deal between primary attacker and
// defender
float directed_warscore(sys::state& state, dcon::war_id w, dcon::nation_id primary, dcon::nation_id secondary);

bool is_defender_wargoal(sys::state const& state, dcon::war_id w, dcon::wargoal_id wg);

enum class war_role { none, attacker, defender };
war_role get_role(sys::state const& state, dcon::war_id w, dcon::nation_id n);

template<typename T> auto province_is_blockaded(sys::state const& state, T ids);
template<typename T> auto province_is_under_siege(sys::state const& state, T ids);
template<typename T> auto battle_is_ongoing_in_province(sys::state const& state, T ids);

float recruited_pop_fraction(sys::state const& state, dcon::nation_id n);
bool state_has_naval_base(sys::state const& state, dcon::state_instance_id di);

int32_t supply_limit_in_province(sys::state& state, dcon::nation_id n, dcon::province_id p);
int32_t regiments_created_from_province(sys::state& state, dcon::province_id p); // does not include mobilized regiments
int32_t regiments_max_possible_from_province(sys::state& state, dcon::province_id p);
int32_t main_culture_regiments_created_from_province(sys::state& state, dcon::province_id p);
int32_t main_culture_regiments_max_possible_from_province(sys::state& state, dcon::province_id p);
int32_t regiments_under_construction_in_province(sys::state& state, dcon::province_id p);
int32_t main_culture_regiments_under_construction_in_province(sys::state& state, dcon::province_id p);
int32_t mobilized_regiments_created_from_province(sys::state& state, dcon::province_id p);
int32_t mobilized_regiments_possible_from_province(sys::state& state, dcon::province_id p);
dcon::pop_id find_available_soldier(sys::state& state, dcon::province_id p, bool require_accepted);
dcon::pop_id find_available_soldier(sys::state& state, dcon::province_id p, dcon::culture_id pop_culture);

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

void update_naval_supply_points(sys::state& state); // must run after determining connectivity
void update_cbs(sys::state& state);
void monthly_leaders_update(sys::state& state);
void daily_leaders_update(sys::state& state);

bool cb_conditions_satisfied(sys::state& state, dcon::nation_id actor, dcon::nation_id target, dcon::cb_type_id cb);
bool cb_instance_conditions_satisfied(sys::state& state, dcon::nation_id actor, dcon::nation_id target, dcon::cb_type_id cb,
		dcon::state_definition_id st, dcon::national_identity_id tag, dcon::nation_id secondary);
void add_cb(sys::state& state, dcon::nation_id n, dcon::cb_type_id cb,
		dcon::nation_id target); // do not call this function directly unless you know what you are doing
void execute_cb_discovery(sys::state& state, dcon::nation_id n);

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
void join_war(sys::state& state, dcon::war_id w, dcon::nation_id n, bool is_attacker);

float truce_break_cb_prestige_cost(sys::state& state, dcon::cb_type_id type);
float truce_break_cb_militancy(sys::state& state, dcon::cb_type_id type);
float truce_break_cb_infamy(sys::state& state, dcon::cb_type_id type);

int32_t peace_cost(sys::state& state, dcon::war_id war, dcon::cb_type_id wargoal, dcon::nation_id from, dcon::nation_id target,
		dcon::nation_id secondary_nation, dcon::state_definition_id wargoal_state, dcon::national_identity_id wargoal_t);
int32_t cost_of_peace_offer(sys::state& state, dcon::peace_offer_id offer);

float successful_cb_prestige(sys::state& state, dcon::cb_type_id type, dcon::nation_id actor);
float cb_infamy(sys::state const& state, dcon::cb_type_id t); // the fabrication cost in infamy
float cb_addition_infamy_cost(sys::state& state, dcon::war_id war, dcon::cb_type_id type, dcon::nation_id from,
		dcon::nation_id target); // the cost of adding a particular cb to the war -- does NOT check if the CB is valid to add

bool cb_requires_selection_of_a_valid_nation(sys::state const& state, dcon::cb_type_id t);
bool cb_requires_selection_of_a_liberatable_tag(sys::state const& state, dcon::cb_type_id t);
bool cb_requires_selection_of_a_state(sys::state const& state, dcon::cb_type_id t);

void remove_from_war(sys::state& state, dcon::war_id w, dcon::nation_id n, bool as_loss);
enum class war_result { draw, attacker_won, defender_won };
void cleanup_war(sys::state& state, dcon::war_id w, war_result result);

void implement_war_goal(sys::state& state, dcon::war_id war, dcon::cb_type_id wargoal, dcon::nation_id from,
		dcon::nation_id target, dcon::nation_id secondary_nation, dcon::state_definition_id wargoal_state,
		dcon::national_identity_id wargoal_t);
void implement_peace_offer(sys::state& state, dcon::peace_offer_id offer);
void reject_peace_offer(sys::state& state, dcon::peace_offer_id offer);

void update_ticking_war_score(sys::state& state);

bool can_embark_onto_sea_tile(sys::state& state, dcon::nation_id n, dcon::province_id p, dcon::army_id a);
float effective_army_speed(sys::state& state, dcon::army_id a);
float effective_navy_speed(sys::state& state, dcon::navy_id n);

sys::date arrival_time_to(sys::state& state, dcon::army_id a, dcon::province_id p);
sys::date arrival_time_to(sys::state& state, dcon::navy_id n, dcon::province_id p);

void update_movement(sys::state& state);

} // namespace military
