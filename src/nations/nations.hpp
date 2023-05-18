#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include "culture.hpp"
#include "dcon_generated.hpp"

namespace nations {
inline uint32_t tag_to_int(char first, char second, char third) {
	return (uint32_t(first) << 16) | (uint32_t(second) << 8) | (uint32_t(third) << 0);
}
inline std::string int_to_tag(uint32_t v) {
	char values[] = { char((v >> 16) & 0xFF), char((v >> 8) & 0xFF), char((v >> 0) & 0xFF) };
	return std::string(values, values + 3);
}

struct triggered_modifier {
	dcon::modifier_id linked_modifier;
	dcon::trigger_key trigger_condition;
};

struct fixed_event {
	int16_t chance;
	dcon::national_event_id id;
};
struct fixed_province_event {
	int16_t chance;
	dcon::provincial_event_id id;
};

enum class focus_type : uint8_t {
	unknown             = 0,
	rail_focus          = 1,
	immigration_focus   = 2,
	diplomatic_focus    = 3,
	promotion_focus     = 4,
	production_focus    = 5,
	party_loyalty_focus = 6
};

struct global_national_state {
	std::vector<triggered_modifier> triggered_modifiers;
	std::vector<dcon::bitfield_type> global_flag_variables;
	std::vector<dcon::nation_id> nations_by_rank;

	tagged_vector<dcon::text_sequence_id, dcon::national_flag_id> flag_variable_names;
	tagged_vector<dcon::text_sequence_id, dcon::global_flag_id> global_flag_variable_names;
	tagged_vector<dcon::text_sequence_id, dcon::national_variable_id> variable_names;

	dcon::modifier_id very_easy_player;
	dcon::modifier_id easy_player;
	dcon::modifier_id hard_player;
	dcon::modifier_id very_hard_player;
	dcon::modifier_id very_easy_ai;
	dcon::modifier_id easy_ai;
	dcon::modifier_id hard_ai;
	dcon::modifier_id very_hard_ai;

	//provincial
	dcon::modifier_id overseas;
	dcon::modifier_id coastal;
	dcon::modifier_id non_coastal;
	dcon::modifier_id coastal_sea;
	dcon::modifier_id sea_zone;
	dcon::modifier_id land_province;
	dcon::modifier_id blockaded;
	dcon::modifier_id no_adjacent_controlled;
	dcon::modifier_id core;
	dcon::modifier_id has_siege;
	dcon::modifier_id occupied;
	dcon::modifier_id nationalism;
	dcon::modifier_id infrastructure;

	// national
	dcon::modifier_id base_values;
	dcon::modifier_id war;
	dcon::modifier_id peace;
	dcon::modifier_id disarming;
	dcon::modifier_id war_exhaustion;
	dcon::modifier_id badboy;
	dcon::modifier_id debt_default_to;
	dcon::modifier_id bad_debter;
	dcon::modifier_id great_power;
	dcon::modifier_id second_power;
	dcon::modifier_id civ_nation;
	dcon::modifier_id unciv_nation;
	dcon::modifier_id average_literacy;
	dcon::modifier_id plurality;
	dcon::modifier_id generalised_debt_default;
	dcon::modifier_id total_occupation;
	dcon::modifier_id total_blockaded;
	dcon::modifier_id in_bankrupcy;

	int32_t num_allocated_national_variables = 0;
	int32_t num_allocated_national_flags = 0;
	int32_t num_allocated_global_flags = 0;

	dcon::national_focus_id flashpoint_focus;
	float flashpoint_amount = 0.15f;

	std::vector<fixed_event> on_yearly_pulse;
	std::vector<fixed_event> on_quarterly_pulse;
	std::vector<fixed_province_event> on_battle_won;
	std::vector<fixed_province_event> on_battle_lost;
	std::vector<fixed_event> on_surrender;
	std::vector<fixed_event> on_new_great_nation;
	std::vector<fixed_event> on_lost_great_nation;
	std::vector<fixed_event> on_election_tick;
	std::vector<fixed_event> on_colony_to_state;
	std::vector<fixed_event> on_state_conquest;
	std::vector<fixed_event> on_colony_to_state_free_slaves;
	std::vector<fixed_event> on_debtor_default;
	std::vector<fixed_event> on_debtor_default_small;
	std::vector<fixed_event> on_debtor_default_second;
	std::vector<fixed_event> on_civilize;
	std::vector<fixed_event> on_my_factories_nationalized;
	std::vector<fixed_event> on_crisis_declare_interest;

	bool is_global_flag_variable_set(dcon::global_flag_id id) const;
	void set_global_flag_variable(dcon::global_flag_id id, bool state);
};

namespace influence {

constexpr inline uint8_t level_mask = uint8_t(0x07);
constexpr inline uint8_t level_neutral = uint8_t(0x00);
constexpr inline uint8_t level_opposed = uint8_t(0x01);
constexpr inline uint8_t level_hostile = uint8_t(0x02);
constexpr inline uint8_t level_cordial = uint8_t(0x03);
constexpr inline uint8_t level_friendly = uint8_t(0x04);
constexpr inline uint8_t level_in_sphere = uint8_t(0x05);

constexpr inline uint8_t priority_mask = uint8_t(0x18);
constexpr inline uint8_t priority_zero = uint8_t(0x00);
constexpr inline uint8_t priority_one = uint8_t(0x08);
constexpr inline uint8_t priority_two = uint8_t(0x10);
constexpr inline uint8_t priority_three = uint8_t(0x18);

// constexpr inline uint8_t is_expelled = uint8_t(0x20); // <-- didn't need this; free bit
constexpr inline uint8_t is_discredited = uint8_t(0x40);
constexpr inline uint8_t is_banned = uint8_t(0x80);

inline uint8_t increase_level(uint8_t v) {
	switch(v & level_mask) {
		case level_neutral:
			return uint8_t((v & ~level_mask) | level_cordial);
		case level_opposed:
			return uint8_t((v & ~level_mask) | level_neutral);
		case level_hostile:
			return uint8_t((v & ~level_mask) | level_opposed);
		case level_cordial:
			return uint8_t((v & ~level_mask) | level_friendly);
		case level_friendly:
			return uint8_t((v & ~level_mask) | level_in_sphere);
		case level_in_sphere:
			return uint8_t((v & ~level_mask) | level_in_sphere);
		default:
			return v;
	}
}
inline uint8_t decrease_level(uint8_t v) {
	switch(v & level_mask) {
		case level_neutral:
			return uint8_t((v & ~level_mask) | level_opposed);
		case level_opposed:
			return uint8_t((v & ~level_mask) | level_hostile);
		case level_hostile:
			return uint8_t((v & ~level_mask) | level_hostile);
		case level_cordial:
			return uint8_t((v & ~level_mask) | level_neutral);
		case level_friendly:
			return uint8_t((v & ~level_mask) | level_cordial);
		case level_in_sphere:
			return uint8_t((v & ~level_mask) | level_friendly);
		default:
			return v;
	}
}
inline uint8_t increase_priority(uint8_t v) {
	if((v & priority_mask) != priority_three) {
		return uint8_t(v + priority_one);
	} else {
		return v;
	}
}
inline uint8_t decrease_priority(uint8_t v) {
	if((v & priority_mask) != priority_zero) {
		return uint8_t(v - priority_one);
	} else {
		return v;
	}
}

}

dcon::nation_id get_nth_great_power(sys::state const& state, uint16_t n);

// returns whether a culture is on the accepted list OR is the primary culture
template<typename T, typename U>
auto nation_accepts_culture(sys::state const& state, T ids, U c);

template<typename T>
auto primary_culture_group(sys::state const& state, T ids);
template<typename T>
auto owner_of_pop(sys::state const& state, T pop_ids);
template<typename T>
auto central_reb_controlled_fraction(sys::state const& state, T ids);
template<typename T>
auto central_blockaded_fraction(sys::state const& state, T ids);
template<typename T>
auto central_has_crime_fraction(sys::state const& state, T ids);
template<typename T>
auto occupied_provinces_fraction(sys::state const& state, T ids);

bool can_release_as_vassal(sys::state const& state, dcon::nation_id n, dcon::national_identity_id releasable);
bool identity_has_holder(sys::state const& state, dcon::national_identity_id ident);
dcon::nation_id get_relationship_partner(sys::state const& state, dcon::diplomatic_relation_id rel_id, dcon::nation_id query);

void update_cached_values(sys::state& state);
void restore_unsaved_values(sys::state& state);
void restore_state_instances(sys::state& state);
void generate_initial_state_instances(sys::state& state);

dcon::text_sequence_id name_from_tag(sys::state const& state, dcon::national_identity_id tag);

void update_administrative_efficiency(sys::state& state);

float daily_research_points(sys::state& state, dcon::nation_id n);
void update_research_points(sys::state& state);
void update_colonial_points(sys::state& state); // NOTE: relies on naval supply being set

void update_industrial_scores(sys::state& state);
void update_military_scores(sys::state& state);
void update_rankings(sys::state& state);
void update_ui_rankings(sys::state& state);

bool is_great_power(sys::state const& state, dcon::nation_id n);
float prestige_score(sys::state const& state, dcon::nation_id n);

enum class status : uint8_t {
	great_power, secondary_power, civilized, westernizing, uncivilized, primitive
};
status get_status(sys::state& state, dcon::nation_id n);

dcon::technology_id current_research(sys::state const& state, dcon::nation_id n);
float suppression_points(sys::state const& state, dcon::nation_id n);
float diplomatic_points(sys::state const& state, dcon::nation_id n);
float leadership_points(sys::state const& state, dcon::nation_id n);
float get_treasury(sys::state& state, dcon::nation_id n);
float get_bank_funds(sys::state& state, dcon::nation_id n);
float get_debt(sys::state& state, dcon::nation_id n);
float tariff_efficiency(sys::state& state, dcon::nation_id n);
float tax_efficiency(sys::state& state, dcon::nation_id n);
int32_t free_colonial_points(sys::state const& state, dcon::nation_id n);
int32_t max_colonial_points(sys::state const& state, dcon::nation_id n);

bool has_political_reform_available(sys::state& state, dcon::nation_id n);
bool has_social_reform_available(sys::state& state, dcon::nation_id n);
bool has_reform_available(sys::state& state, dcon::nation_id n);
bool has_decision_available(sys::state& state, dcon::nation_id n);
int32_t max_national_focuses(sys::state& state, dcon::nation_id n);
int32_t national_focuses_in_use(sys::state& state, dcon::nation_id n);
bool can_expand_colony(sys::state& state, dcon::nation_id n);
bool is_losing_colonial_race(sys::state& state, dcon::nation_id n);
bool sphereing_progress_is_possible(sys::state& state, dcon::nation_id n); // can increase opinion or add to sphere
bool is_involved_in_crisis(sys::state const& state, dcon::nation_id n);
bool can_put_flashpoint_focus_in_state(sys::state& state, dcon::state_instance_id s, dcon::nation_id fp_nation);
int64_t get_monthly_pop_increase_of_nation(sys::state& state, dcon::nation_id n);

std::vector<dcon::political_party_id> get_active_political_parties(sys::state& state, dcon::nation_id n);

void update_monthly_points(sys::state& state);

// may create a relationship DO NOT call in a context where two or more such functions may run in parallel
void adjust_relationship(sys::state& state, dcon::nation_id a, dcon::nation_id b, float delta);
// used for creating a "new" nation when it is released
void create_nation_based_on_template(sys::state& state, dcon::nation_id n, dcon::nation_id base);
// call after a nation loses its last province
void cleanup_nation(sys::state& state, dcon::nation_id n);

void adjust_prestige(sys::state& state, dcon::nation_id n, float delta);
void destroy_diplomatic_relationships(sys::state& state, dcon::nation_id n);
void release_vassal(sys::state& state, dcon::overlord_id rel);
void break_alliance(sys::state& state, dcon::diplomatic_relation_id rel);
void break_alliance(sys::state& state, dcon::nation_id a, dcon::nation_id b);
void make_alliance(sys::state& state, dcon::nation_id a, dcon::nation_id b);
void adjust_influence(sys::state& state, dcon::nation_id great_power, dcon::nation_id target, float delta);

void update_great_powers(sys::state& state);
void update_influence(sys::state& state);

void monthly_flashpoint_update(sys::state& state);
void daily_update_flashpoint_tension(sys::state& state);
void update_crisis(sys::state& state);

void add_as_primary_crisis_defender(sys::state& state, dcon::nation_id n);
void add_as_primary_crisis_attacker(sys::state& state, dcon::nation_id n);
void reject_crisis_participation(sys::state& state);
void cleanup_crisis(sys::state& state);
void update_crisis(sys::state& state);

void update_pop_acceptance(sys::state& state, dcon::nation_id n);
void liberate_nation_from(sys::state& state, dcon::national_identity_id liberated, dcon::nation_id from);
void release_nation_from(sys::state& state, dcon::national_identity_id liberated, dcon::nation_id from); // difference from liberate: only non-cores can be lost with release
void perform_nationalization(sys::state& state, dcon::nation_id n);

}

