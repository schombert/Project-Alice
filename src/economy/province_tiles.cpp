#include "province_tiles.hpp"
#include "container_types.hpp"
#include "dcon_generated.hpp"
#include "commands.hpp"

namespace economy {

std::vector<province_tile> retrieve_province_tiles(sys::state& state, dcon::province_id p) {
	std::vector<province_tile> tiles = std::vector<province_tile>(64);

	auto owner = state.world.province_get_nation_from_province_ownership(p);

	int curind = 0;

	// Main province RGOs
	tiles[curind].rgo_commodity = state.world.province_get_rgo(p);
	tiles[curind].empty = false;
	tiles[curind].province = p;
	curind++;

	// Secondary RGOs
	for(auto c : state.world.in_commodity) {
		if(c == state.world.province_get_rgo(p)) {
			continue;
		}
		if(economy::rgo_max_employment(state, owner, p, c) > 500.f) {
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

	for(auto c : state.world.in_commodity) {
		if(state.world.province_get_factory_max_level_per_good(p, c) > 0) {
			bool foundcorrespondingfactory = false;
			// If there is already a factory developing this resource - don't show it with a separate tile.
			for(int i = 0; i < curind; i++) {
				// Not a factory tile
				if(!tiles[i].factory) {
					continue;
				}
				auto type = state.world.factory_get_building_type(tiles[i].factory);
				if(state.world.factory_type_get_output(type) == c) {
					foundcorrespondingfactory = true;
					break;
				}
			}
			if(foundcorrespondingfactory) {
				continue;
			}
			tiles[curind].potential_commodity = c;
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
