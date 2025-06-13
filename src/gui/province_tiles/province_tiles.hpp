#pragma once

#include "container_types.hpp"
#include "dcon_generated.hpp"
#include "commands.hpp"

namespace ui {

struct province_tile {
	bool empty = true;
	bool build_new = false;
	dcon::province_id province;
	dcon::administration_id local_administration;
	bool capital_administration = false;
	dcon::market_id market;
	dcon::commodity_id rgo_commodity;
	dcon::commodity_id potential_commodity;
	dcon::factory_id factory;
	dcon::regiment_id regiment;
	dcon::factory_construction_id factory_construction;
	bool has_province_building = false;
	economy::province_building_type province_building;
};

std::vector<province_tile> retrieve_province_tiles(sys::state& state, dcon::province_id p);
province_tile retrieve_nth_province_tile(sys::state& state, dcon::province_id p, int i);


} // namespace economy
