#pragma once

// Advanced province buildings are a future replacement for province buildings
// They can hire people, expand and produce services (or, in future, local modifiers)
// They could be owned by government at expense of national budget
// They could produce "free services"
// They hase current size which depends on national funding and profitability
// National funding sets size to funding / cost of worker * province weight
// Private funding increases size when operation is profitable and shrinks when it is not
// Currently they are hardcoded as "enums with properties"
// Future work should be directed toward moving their definitions into content files and integration with dcon

namespace services {

namespace list {
inline constexpr int32_t education = 0;
inline constexpr int32_t total = 1;
}

void initialize_size_of_dcon_arrays(sys::state& state);
void update_price(sys::state& state);
void reset_supply(sys::state& state);
void reset_demand(sys::state& state);
void reset_price(sys::state& state);
void match_supply_and_demand(sys::state& state);
}

namespace advanced_province_buildings {

namespace list {
inline constexpr int32_t schools_and_universities = 0;
inline constexpr int32_t total = 1;
}

struct advanced_building_definition {
	int32_t throughput_labour_type;
	int32_t output;
	float output_amount;
};

const extern advanced_building_definition definitions[services::list::total];
void initialize_size_of_dcon_arrays(sys::state& state);
void update_consumption(sys::state& state);
void update_size(sys::state& state);
void update_production(sys::state& state);

}
