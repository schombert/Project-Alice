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
inline constexpr uint32_t po_liberate = 0x02000000;
inline constexpr uint32_t po_take_from_sphere = 0x04000000;

}

enum class unit_type : uint8_t {
	support, big_ship, cavalry, transport, light_ship, special, infantry
};


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

	unit_definition() {
	}
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
};

void reset_unit_stats(sys::state& state);
void apply_base_unit_stat_modifiers(sys::state& state);
void restore_unsaved_values(sys::state& state);

bool are_at_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b);
bool can_use_cb_against(sys::state const& state, dcon::nation_id from, dcon::nation_id target);

template<typename T>
auto province_is_blockaded(sys::state const& state, T ids);
template<typename T>
auto battle_is_ongoing_in_province(sys::state const& state, T ids);

float recruited_pop_fraction(sys::state const& state, dcon::nation_id n);
bool state_has_naval_base(sys::state const& state, dcon::state_instance_id di);

int32_t supply_limit_in_province(sys::state& state, dcon::nation_id n, dcon::province_id p);
int32_t regiments_created_from_province(sys::state& state, dcon::province_id p);
int32_t regiments_max_possible_from_province(sys::state& state, dcon::province_id p);

void update_recruitable_regiments(sys::state& state, dcon::nation_id n);
void update_all_recruitable_regiments(sys::state& state);
void regenerate_total_regiment_counts(sys::state& state);

void regenerate_land_unit_average(sys::state& state);
void regenerate_ship_scores(sys::state& state);

}
