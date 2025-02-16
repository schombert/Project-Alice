#pragma once

#include "province_tiles.hpp"
#include "container_types.hpp"
#include "dcon_generated.hpp"
#include "commands.hpp"

namespace economy {

std::vector<province_tile> retrieve_province_tiles(sys::state& state, dcon::province_id p) {
	std::vector<province_tile> tiles = std::vector<province_tile>(64);

	int curind = 0;

	// Main province RGOs
	tiles[curind].rgo_commodity = state.world.province_get_rgo(p);
	tiles[curind].empty = false;
	tiles[curind].province = p;
	curind++;

	// Secondary RGOs
	for(auto c : state.world.in_commodity) {
		if(state.world.province_get_rgo_max_size_per_good(p, c) > 1000) {
			tiles[curind].rgo_commodity = c;
			tiles[curind].empty = false;
			tiles[curind].province = p;
			curind++;
		}
	}

	for(auto f : state.world.in_factory) {
		if(f.get_factory_location().get_province() == p) {
			tiles[curind].factory = f;
			tiles[curind].empty = false;
			tiles[curind].province = p;
			curind++;
		}
	}

	for(auto r : state.world.in_regiment_source) {
		if(r.get_pop().get_province_from_pop_location() == p) {
			tiles[curind].regiment = r.get_regiment();
			tiles[curind].empty = false;
			tiles[curind].province = p;
			curind++;
		}
	}

	for(int i = 0; i < state.world.province_get_building_level(p, uint8_t(economy::province_building_type::naval_base)); i++) {
		tiles[curind].province_building = economy::province_building_type::naval_base;
		tiles[curind].empty = false;
		tiles[curind].has_province_building = true;
		tiles[curind].province = p;
		curind++;
	}

	for(int i = 0; i < state.world.province_get_building_level(p, uint8_t(economy::province_building_type::railroad)); i++) {
		tiles[curind].province_building = economy::province_building_type::railroad;
		tiles[curind].empty = false;
		tiles[curind].has_province_building = true;
		tiles[curind].province = p;
		curind++;
	}

	for(int i = 0; i < state.world.province_get_building_level(p, uint8_t(economy::province_building_type::fort)); i++) {
		tiles[curind].province_building = economy::province_building_type::fort;
		tiles[curind].empty = false;
		tiles[curind].has_province_building = true;
		tiles[curind].province = p;
		curind++;
	}

	if(curind < 64) {
		tiles[curind].build_new = true;
		tiles[curind].empty = false;
		tiles[curind].province = p;
		curind++;
	}

	return tiles;
}
province_tile retrieve_nth_province_tile(sys::state& state, dcon::province_id p, int i) {
	auto tiles = retrieve_province_tiles(state, p);

	if(size_t(i) >= tiles.size()) {
		return province_tile{ .empty = true };
	}

	return tiles[i];
}

}
