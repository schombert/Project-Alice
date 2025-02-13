#pragma once

#include "container_types.hpp"
#include "dcon_generated.hpp"
#include "commands.hpp"

namespace economy {

struct province_tile {
	bool empty = true;
	dcon::commodity_id rgo_commodity;
	dcon::factory_id factory;
	dcon::regiment_id regiment;
	bool railroad = false;
	bool naval_base = false;
	bool fort = false;
};

std::vector<province_tile> retrieve_province_tiles(sys::state& state, dcon::nation_id n, dcon::province_id p);
province_tile retrieve_nth_province_tile(sys::state& state, dcon::nation_id n, dcon::province_id p, int i);


} // namespace economy
