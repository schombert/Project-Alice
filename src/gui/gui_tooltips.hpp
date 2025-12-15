#pragma once

#include "system_state_forward.hpp"
#include "dcon_generated_ids.hpp"

namespace text {
struct columnar_layout;
}

namespace ui {
void factory_stats_tooltip(sys::state& state, text::columnar_layout& contents, dcon::factory_id fid);
void province_building_construction_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id p, economy::province_building_type bt);
void province_owner_rgo_commodity_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov_id, dcon::commodity_id c);
void province_building_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id p, economy::province_building_type bt);
}
