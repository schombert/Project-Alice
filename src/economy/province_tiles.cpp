#pragma once

#include "province_tiles.hpp"
#include "container_types.hpp"
#include "dcon_generated.hpp"
#include "commands.hpp"

namespace economy {

std::vector<province_tile> retrieve_province_tiles(sys::state& state, dcon::nation_id n, dcon::province_id p) {
	std::vector<province_tile> tiles = std::vector<province_tile>(64);

	int curind = 0;

	for(auto c : state.world.in_commodity) {
		if(state.world.province_get_rgo_max_size_per_good(p, c) > 1000) {
			tiles[curind].rgo_commodity = c;
			tiles[curind].empty = false;
			curind++;
		}
	}

	for(auto f : state.world.in_factory) {
		if(f.get_factory_location().get_province() == p) {
			tiles[curind].factory = f;
			tiles[curind].empty = false;
			curind++;
		}
	}

	for(auto r : state.world.in_regiment_source) {
		if(r.get_pop().get_province_from_pop_location() == p) {
			tiles[curind].regiment = r.get_regiment();
			tiles[curind].empty = false;
			curind++;
		}
	}

	for(int i = 0; i < state.world.province_get_building_level(p, uint8_t(economy::province_building_type::naval_base)); i++) {
		tiles[curind].naval_base = true;
		tiles[curind].empty = false;
		curind++;
	}

	for(int i = 0; i < state.world.province_get_building_level(p, uint8_t(economy::province_building_type::railroad)); i++) {
		tiles[curind].railroad = true;
		tiles[curind].empty = false;
		curind++;
	}

	for(int i = 0; i < state.world.province_get_building_level(p, uint8_t(economy::province_building_type::fort)); i++) {
		tiles[curind].fort = true;
		tiles[curind].empty = false;
		curind++;
	}

	return tiles;
}
province_tile retrieve_nth_province_tile(sys::state& state, dcon::nation_id n, dcon::province_id p, int i) {
	auto tiles = retrieve_province_tiles(state, n, p);

	if(size_t(i) >= tiles.size()) {
		return province_tile{ .empty = true };
	}

	return tiles[i];
}

}
