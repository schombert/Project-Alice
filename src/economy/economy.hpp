#pragma once

#include "container_types.hpp"
#include "dcon_generated.hpp"

namespace economy {

struct fort_information {
	tagged_vector<float, dcon::commodity_id> cost;
	int32_t max_level = 6;
	int32_t time = 1080;
	dcon::modifier_id province_modifier;
	dcon::text_sequence_id name;
};
struct railroad_information {
	tagged_vector<float, dcon::commodity_id> cost;
	float infrastructure = 0.16f;
	int32_t max_level = 6;
	int32_t time = 1080;
	dcon::modifier_id province_modifier;
	dcon::text_sequence_id name;
};
struct naval_base_information {
	tagged_vector<float, dcon::commodity_id> cost;
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
};

}
