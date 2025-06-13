#include "province_tiles.hpp"
#include "container_types.hpp"
#include "dcon_generated.hpp"
#include "commands.hpp"
#include "economy_production.hpp"

namespace ui {

std::vector<province_tile> retrieve_province_tiles(sys::state& state, dcon::province_id p) {
	std::vector<province_tile> tiles = std::vector<province_tile>(64);

	auto owner = state.world.province_get_nation_from_province_ownership(p);
	auto si = state.world.province_get_state_membership(p);

	int curind = 0;

	bool administration_found = false;

	// Capital administration is located in the nation capital
	if(state.world.nation_get_capital(owner) == p) {
		tiles[curind].capital_administration = true;
		tiles[curind].empty = false;
		tiles[curind].province = p;
		curind++;

		administration_found = true;
	}

	for(auto admin : state.world.nation_get_nation_administration(owner)) {
		if(admin.get_administration().get_capital() == p) {
			// There can be several administrations per state located in any province of the state
			tiles[curind].local_administration = admin.get_administration();
			tiles[curind].empty = false;
			tiles[curind].province = p;
			curind++;

			administration_found = true;
			break;
		}
	}

	// If there is no administration in the province, we display a tile with a small tax office.
	if(!administration_found) {
		tiles[curind].no_administration_tile = true;
		tiles[curind].empty = false;
		tiles[curind].province = p;
		curind++;
	}

	// Market is located in the capital of the state instabce
	if(state.world.state_instance_get_capital(si) == p) {
		auto market = state.world.state_instance_get_market_from_local_market(si);
		tiles[curind].market = market;
		tiles[curind].empty = false;
		tiles[curind].province = p;
		curind++;
	}

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
		if(economy::rgo_max_employment(state, c, p) > 100.f) {
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
		if(state.world.province_get_factory_max_size(p, c) > 0) {
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

	for(auto fc : state.world.in_factory_construction) {
		if(fc.get_province() == p) {
			tiles[curind].factory_construction = fc;
			tiles[curind].empty = false;
			tiles[curind].province = p;
			curind++;
		}
	}

	/*
	* Since from province UI we can build factories, buildings, regiments, and ships. I don't think it'd be useful to duplicate them with "new" submenu.
	if(curind < 64) {
		tiles[curind].build_new = true;
		tiles[curind].empty = false;
		tiles[curind].province = p;
		curind++;
	}*/

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
