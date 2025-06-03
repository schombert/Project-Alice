#pragma once
#include "dcon_generated.hpp"

namespace sys {
struct state;
}

namespace ai {
void update_budget(sys::state& state);
void get_craved_factory_types(sys::state& state, dcon::nation_id nid, dcon::market_id mid, dcon::province_id, std::vector<dcon::factory_type_id>& desired_types, bool pop_project);
void get_desired_factory_types(sys::state& state, dcon::nation_id nid, dcon::market_id mid, dcon::province_id, std::vector<dcon::factory_type_id>& desired_types, bool pop_project);
void update_ai_econ_construction(sys::state& state);
void update_factory_types_priority(sys::state& state);
}
