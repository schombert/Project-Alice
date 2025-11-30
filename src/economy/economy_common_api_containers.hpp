#pragma once

#include "dcon_generated_ids.hpp"
#include "container_types_dcon.hpp"

namespace economy {

struct construction_status {
	float progress = 0.0f; // in range [0,1)
	bool is_under_construction = false;
};

struct new_factory {
	float progress = 0.0f;
	dcon::factory_type_id type;
};

struct upgraded_factory {
	float progress = 0.0f;
	dcon::factory_type_id type;
	dcon::factory_type_id target_type;
};

struct building_information {
	economy::commodity_set cost;
	int32_t naval_capacity = 1;
	int32_t colonial_points[8] = { 30, 50, 70, 90, 110, 130, 150, 170 };
	int32_t colonial_range = 50;
	int32_t max_level = 6;
	int32_t time = 1080;
	float infrastructure = 0.16f;
	dcon::text_key name;
	dcon::modifier_id province_modifier;
	uint16_t padding2 = 0;
	bool defined = false;
	uint8_t padding[3] = { 0 };
};

static_assert(sizeof(building_information) == 104);
static_assert(sizeof(building_information::cost) == 40);
static_assert(sizeof(building_information::colonial_points) == 32);
static_assert(sizeof(building_information::province_modifier) == 2);
static_assert(sizeof(building_information::name) == 4);
static_assert(sizeof(building_information::cost)
	+ sizeof(building_information::naval_capacity)
	+ sizeof(building_information::colonial_range)
	+ sizeof(building_information::colonial_points)
	+ sizeof(building_information::max_level)
	+ sizeof(building_information::time)
	+ sizeof(building_information::infrastructure)
	== 92);
static_assert(sizeof(building_information) ==
	sizeof(building_information::cost)
	+ sizeof(building_information::naval_capacity)
	+ sizeof(building_information::colonial_range)
	+ sizeof(building_information::colonial_points)
	+ sizeof(building_information::max_level)
	+ sizeof(building_information::time)
	+ sizeof(building_information::infrastructure)
	+ sizeof(building_information::province_modifier)
	+ sizeof(building_information::name)
	+ sizeof(building_information::defined)
	+ sizeof(building_information::padding)
	+ sizeof(building_information::padding2));
inline std::string_view province_building_type_get_name(economy::province_building_type v) {
	switch(v) {
	case economy::province_building_type::railroad:
		return "railroad";
	case economy::province_building_type::fort:
		return "fort";
	case economy::province_building_type::naval_base:
		return "naval_base";
	case economy::province_building_type::bank:
		return "bank";
	case economy::province_building_type::university:
		return "university";
	default:
		return "???";
	}
}
inline std::string_view province_building_type_get_level_text(economy::province_building_type v) {
	switch(v) {
	case economy::province_building_type::railroad:
		return "railroad_level";
	case economy::province_building_type::fort:
		return "fort_level";
	case economy::province_building_type::naval_base:
		return "naval_base_level";
	case economy::province_building_type::bank:
		return "bank_level";
	case economy::province_building_type::university:
		return "university_level";
	default:
		return "???";
	}
}
struct global_economy_state {
	building_information building_definitions[max_building_types];
	float craftsmen_fraction = 0.8f;
	dcon::modifier_id selector_modifier{};
	dcon::modifier_id immigrator_modifier{};
};
static_assert(sizeof(global_economy_state) ==
	sizeof(global_economy_state::building_definitions)
	+ sizeof(global_economy_state::selector_modifier)
	+ sizeof(global_economy_state::immigrator_modifier)
	+ sizeof(global_economy_state::craftsmen_fraction));

namespace gdp {
struct breakdown {
	float primary;
	float secondary_factory;
	float secondary_artisan;
	float total;
	float total_non_negative;
};
}
}
