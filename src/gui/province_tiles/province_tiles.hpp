#pragma once

#include "container_types.hpp"
#include "dcon_generated.hpp"
#include "commands.hpp"

namespace ui {

struct province_tile {
	bool empty = true;
	bool build_new = false;
	dcon::province_id province{};
	dcon::administration_id local_administration{};
	bool capital_administration = false;
	bool no_administration_tile = false;
	dcon::market_id market{};
	bool is_rgo = false;
	bool is_resource_potential = false;
	dcon::factory_id factory{};
	dcon::regiment_id regiment{};
	dcon::factory_construction_id factory_construction{};
	bool has_province_building = false;
	economy::province_building_type province_building = economy::province_building_type::railroad;
	dcon::commodity_id commodity{};
	bool is_civilian_port = false;
};

std::vector<province_tile> retrieve_province_tiles(sys::state& state, dcon::province_id p);
province_tile retrieve_nth_province_tile(sys::state& state, dcon::province_id p, int i);


} // namespace economy
