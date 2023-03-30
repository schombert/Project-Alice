#pragma once

#include "container_types.hpp"
#include "dcon_generated.hpp"

namespace economy {

struct fort_information {
	economy::commodity_set cost;
	int32_t max_level = 6;
	int32_t time = 1080;
	dcon::modifier_id province_modifier;
	dcon::text_sequence_id name;
};
struct railroad_information {
	economy::commodity_set cost;
	float infrastructure = 0.16f;
	int32_t max_level = 6;
	int32_t time = 1080;
	dcon::modifier_id province_modifier;
	dcon::text_sequence_id name;
};
struct naval_base_information {
	economy::commodity_set cost;
	int32_t naval_capacity = 1;
	int32_t colonial_range = 50;
	int32_t colonial_points[8] = { 30,50,70,90,110,130,150,170 };
	int32_t max_level = 6;
	int32_t time = 1080;
	dcon::modifier_id province_modifier;
	dcon::text_sequence_id name;
};

struct global_economy_state {
	fort_information fort_definition;
	railroad_information railroad_definition;
	naval_base_information naval_base_definition;

	float craftsmen_fraction = 0.8f;
};

enum class worker_effect : uint8_t {
	none = 0, input, output, throughput
};

template<typename T>
auto desired_needs_spending(sys::state const& state, T pop_indices) {
	// TODO: gather pop types, extract cached needs sum, etc etc
	return 0.0f;
}

constexpr inline dcon::commodity_id money(0);

bool has_factory(sys::state const& state, dcon::state_instance_id si);
bool has_building(sys::state const& state, dcon::state_instance_id si, dcon::factory_type_id fac);
bool is_bankrupt_debtor_to(sys::state& state, dcon::nation_id debt_holder, dcon::nation_id debtor);

int32_t factory_priority(sys::state const& state, dcon::factory_id f);
void set_factory_priority(sys::state& state, dcon::factory_id f, int32_t priority);
bool factory_is_profitable(sys::state const& state, dcon::factory_id f);

bool nation_is_constructing_factories(sys::state& state, dcon::nation_id n);
bool nation_has_closed_factories(sys::state& state, dcon::nation_id n);

void initialize(sys::state& state);
void regenerate_unsaved_values(sys::state& state);

void update_rgo_employement(sys::state& state);
void update_factory_employement(sys::state& state);

}
