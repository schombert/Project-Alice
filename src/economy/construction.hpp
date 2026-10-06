#pragma once
#include "dcon_generated_ids.hpp"
#include "container_types_dcon.hpp"
#include "concept_declarations.hpp"

namespace sys {
struct state;
}

namespace economy {

enum class construction_completed : uint8_t {
	no,
	yes
};

// Deletes a military unit construction, and performs any auxillary actions required
template<construction_completed completed, concepts::military_construction_type mil_construction_type>
void delete_unit_construction(sys::state& state, mil_construction_type c);

economy::commodity_set calculate_factory_upgrade_goods_cost(
	sys::state& state,
	dcon::nation_id n,
	dcon::province_id pid,
	dcon::factory_type_id upgrade_target,
	bool is_pop_project
);

float estimate_private_construction_spendings(sys::state& state, dcon::nation_id nid);
void populate_government_construction_consumption(sys::state& state);
float estimate_construction_stockpile_spending(const sys::state& state, dcon::nation_id nation, float budget);
tagged_vector<float, dcon::commodity_id> estimate_construction_stockpile_spending_by_commodity(const sys::state& state, dcon::nation_id nation, float budget);



struct unit_construction_data {
	bool can_be_advanced;
	uint32_t construction_time;
	float cost_multiplier;
	dcon::nation_id owner;
	dcon::market_id market;
	dcon::province_id province;
	dcon::unit_type_id unit_type;
};
unit_construction_data explain_land_unit_construction(
	sys::state& state,
	dcon::province_land_construction_id construction
);
unit_construction_data explain_naval_unit_construction(
	sys::state& state,
	dcon::province_naval_construction_id construction
);
struct province_construction_spending_entry {
	dcon::province_building_construction_id construction;
	float spending;
};
struct state_construction_spending_entry {
	dcon::factory_construction_id construction;
	float spending;
};
struct province_land_construction_spending_entry {
	dcon::province_land_construction_id construction;
	float spending;
};
struct province_naval_construction_spending_entry {
	dcon::province_naval_construction_id construction;
	float spending;
};
struct construction_spending_explanation {
	int32_t ongoing_projects;
	float budget_limit_per_project;
	float estimated_spendings;
	std::vector<province_construction_spending_entry> province_buildings;
	std::vector<state_construction_spending_entry> factories;
};
struct construction_spending_explanation_light {
	int32_t ongoing_projects;
	float budget_limit_per_project;
	float estimated_spendings;
	float province_buildings;
	float factories;
};

bool can_advance_construction(const sys::state& state, dcon::province_naval_construction_id con);
bool can_advance_construction(const sys::state& state, dcon::province_land_construction_id con);
bool can_advance_construction(const sys::state& state, dcon::factory_construction_id con);
bool can_advance_construction(const sys::state& state, dcon::province_building_construction_id con);

float construction_build_cost_multiplier(const sys::state& state, dcon::province_land_construction_id con);
float construction_build_cost_multiplier(const sys::state& state, dcon::province_naval_construction_id con);
float construction_build_cost_multiplier(const sys::state& state, dcon::province_building_construction_id con);
float construction_build_cost_multiplier(const sys::state& state, dcon::factory_construction_id con);

template<concepts::construction_type con_type>
economy::commodity_set construction_get_actual_build_cost(const sys::state& state, con_type construction);

template<concepts::construction_type con_type>
float construction_progress(const sys::state& state, con_type c);

template<concepts::construction_type con_type>
float nation_average_construction_satisfaction_by_type(const sys::state& state, dcon::nation_id nation);

float location_build_cost_multiplier(const sys::state& state, dcon::province_id location, bool is_pop_project);
float global_factory_construction_time_modifier(const sys::state& state);
uint32_t factory_building_construction_time(const sys::state& state, dcon::factory_type_id ftid, bool is_upgrade);

template<concepts::construction_type con_type>
uint32_t construction_get_actual_construction_time(const sys::state& state, con_type con);

float factory_build_cost_multiplier(const sys::state& state, dcon::nation_id n, dcon::province_id location, bool privately_owned);
template<concepts::construction_type con_type>
const economy::commodity_set& construction_get_base_build_cost(const sys::state& state, con_type construction);
template<concepts::construction_type con_type>
economy::commodity_set& construction_get_base_build_cost(sys::state& state, con_type construction);
template<concepts::construction_type con_type>
const economy::commodity_set& construction_get_purchased_goods(const sys::state& state, con_type construction);
template<concepts::construction_type con_type>
economy::commodity_set& construction_get_purchased_goods(sys::state& state, con_type construction);

template<concepts::construction_type con_type>
bool construction_is_privately_owned(const sys::state& state, con_type construction);


dcon::internal::const_iterator_province_land_construction_foreach_land_construction_supply_route_as_construction_generator construction_get_supply_routes(const sys::state& state, dcon::province_land_construction_id con);
dcon::internal::iterator_province_land_construction_foreach_land_construction_supply_route_as_construction_generator construction_get_supply_routes(sys::state& state, dcon::province_land_construction_id con);
dcon::internal::const_iterator_province_naval_construction_foreach_naval_construction_supply_route_as_construction_generator construction_get_supply_routes(const sys::state& state, dcon::province_naval_construction_id con);
dcon::internal::iterator_province_naval_construction_foreach_naval_construction_supply_route_as_construction_generator construction_get_supply_routes(sys::state& state, dcon::province_naval_construction_id con);
dcon::internal::const_iterator_factory_construction_foreach_factory_construction_supply_route_as_construction_generator construction_get_supply_routes(const sys::state& state, dcon::factory_construction_id con);
dcon::internal::iterator_factory_construction_foreach_factory_construction_supply_route_as_construction_generator construction_get_supply_routes(sys::state& state, dcon::factory_construction_id con);
dcon::internal::const_iterator_province_building_construction_foreach_building_construction_supply_route_as_construction_generator construction_get_supply_routes(const sys::state& state, dcon::province_building_construction_id con);
dcon::internal::iterator_province_building_construction_foreach_building_construction_supply_route_as_construction_generator construction_get_supply_routes(sys::state& state, dcon::province_building_construction_id con);

template<concepts::construction_type con_type>
void construction_set_last_required_percent_base_cost(sys::state& state, con_type con, float val);

template<concepts::construction_type con_type>
float construction_get_last_required_percent_base_cost(const sys::state& state, con_type con);

// Gets last days' required goods need for a specific construction.
template<concepts::construction_type con_type>
tagged_vector<float, dcon::commodity_id> get_last_required_construction_need(const sys::state& state, con_type construction);
// Gets last days' required construction goods need for an entire nation by a specific construction type
template<concepts::construction_type con_type>
tagged_vector<float, dcon::commodity_id> nation_get_last_required_construction_need(const sys::state& state, dcon::nation_id nation);
// Gets last days' fufilled goods need for a specific construction.
template<concepts::construction_type con_type>
tagged_vector<float, dcon::commodity_id> get_last_fufilled_construction_need(const sys::state& state, con_type construction);
// Gets last days' fufilled construction goods need for an entire nation by a specific construction type
template<concepts::construction_type con_type>
tagged_vector<float, dcon::commodity_id> nation_get_last_fufilled_construction_need(const sys::state& state, dcon::nation_id nation);

uint32_t land_unit_construction_time(const sys::state& state, dcon::unit_type_id utid, dcon::nation_id builder);
uint32_t naval_unit_construction_time(const sys::state& state, dcon::unit_type_id utid, dcon::nation_id builder);
void populate_private_construction_consumption(sys::state& state);
void advance_nation_private_constructions(sys::state& state, dcon::nation_id n, float total_spent_on_construction);
void emulate_construction_demand(sys::state& state, dcon::nation_id n);

// Resolves all constructions which are advancable, has been constructing for enough time, and has all goods required
void resolve_constructions(sys::state& state);

// Advances all valid and advancable constructions by 1 day
void advance_constructions_progress(sys::state& state);


economy::commodity_set calculate_factory_refit_goods_cost(const sys::state& state, dcon::nation_id n, dcon::province_id pid, dcon::factory_type_id from, dcon::factory_type_id to);
float calculate_factory_refit_money_cost(sys::state& state, dcon::nation_id n, dcon::province_id pid, dcon::factory_type_id from, dcon::factory_type_id to);


// Check rules for factories in colonies
bool is_colony(sys::state& state, dcon::province_id p);
bool is_colony(sys::state& state, dcon::state_instance_id s);
bool can_build_factory_in_colony(sys::state& state, dcon::province_id p);
bool can_build_factory_in_colony(sys::state& state, dcon::state_instance_id s);
bool can_build_factory_type_in_colony(sys::state& state, dcon::province_id p, dcon::factory_type_id ft);
bool can_build_factory_type_in_colony(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id ft);

}
