#pragma once

#include "dcon_generated_ids.hpp"

namespace nations {

struct fixed_event {
	int16_t chance; //0,2
	dcon::national_event_id id; //2,2
	dcon::trigger_key condition; //4,2
	uint16_t padding = 0; //6,2
};
struct fixed_election_event {
	int16_t chance; //0,2
	dcon::national_event_id id; //2,2
	dcon::trigger_key condition; //4,2
	dcon::issue_id issue_group; //6,1
	uint8_t padding = 0; //7,1
};
struct fixed_province_event {
	int16_t chance; //0,2
	dcon::provincial_event_id id; //2,2
	dcon::trigger_key condition; //4,2
	uint16_t padding = 0; //6,2
};

struct global_national_state {
	std::vector<triggered_modifier> triggered_modifiers;
	std::vector<dcon::bitfield_type> global_flag_variables;
	std::vector<dcon::nation_id> nations_by_rank;

	tagged_vector<dcon::text_key, dcon::national_flag_id> flag_variable_names;
	tagged_vector<dcon::text_key, dcon::global_flag_id> global_flag_variable_names;
	tagged_vector<dcon::text_key, dcon::national_variable_id> variable_names;

	dcon::national_identity_id rebel_id;

	dcon::modifier_id very_easy_player;
	dcon::modifier_id easy_player;
	dcon::modifier_id hard_player;
	dcon::modifier_id very_hard_player;
	dcon::modifier_id very_easy_ai;
	dcon::modifier_id easy_ai;
	dcon::modifier_id hard_ai;
	dcon::modifier_id very_hard_ai;

	// provincial
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
	dcon::national_focus_id clergy_focus;
	dcon::national_focus_id soldier_focus;
	dcon::national_focus_id aristocrat_focus;
	dcon::national_focus_id capitalist_focus;
	dcon::national_focus_id primary_factory_worker_focus;
	dcon::national_focus_id secondary_factory_worker_focus;

	float flashpoint_amount = 0.15f;

	std::vector<fixed_event> on_yearly_pulse;
	std::vector<fixed_event> on_quarterly_pulse;
	std::vector<fixed_province_event> on_battle_won;
	std::vector<fixed_province_event> on_battle_lost;
	std::vector<fixed_event> on_surrender;
	std::vector<fixed_event> on_new_great_nation;
	std::vector<fixed_event> on_lost_great_nation;
	std::vector<fixed_election_event> on_election_tick;
	std::vector<fixed_event> on_colony_to_state;
	std::vector<fixed_event> on_state_conquest;
	std::vector<fixed_event> on_colony_to_state_free_slaves;
	std::vector<fixed_event> on_debtor_default;
	std::vector<fixed_event> on_debtor_default_small;
	std::vector<fixed_event> on_debtor_default_second;
	std::vector<fixed_event> on_civilize;
	std::vector<fixed_event> on_my_factories_nationalized;
	std::vector<fixed_event> on_crisis_declare_interest;
	std::vector<fixed_event> on_election_started;
	std::vector<fixed_event> on_election_finished;

	bool gc_pending = false;

	bool is_global_flag_variable_set(dcon::global_flag_id id) const;
	void set_global_flag_variable(dcon::global_flag_id id, bool state);
};
}
