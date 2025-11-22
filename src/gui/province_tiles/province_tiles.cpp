#include "province_tiles.hpp"
#include "container_types.hpp"
#include "dcon_generated.hpp"
#include "commands.hpp"
#include "economy_production.hpp"

namespace ui {

// Safely assign tile within vector length limits
void push_tile(std::vector<province_tile>& tiles, province_tile& tile, int& index) {
	if(index >= 64) {
		return;
	}

	tiles[index] = tile;
	index++;
}

std::vector<province_tile> retrieve_province_tiles(sys::state& state, dcon::province_id p) {
	std::vector<province_tile> tiles = std::vector<province_tile>(64);

	auto owner = state.world.province_get_nation_from_province_ownership(p);
	auto si = state.world.province_get_state_membership(p);

	int curind = 0;

	if(owner) {
		bool administration_found = false;

		// Capital administration is located in the nation capital
		if(state.world.nation_get_capital(owner) == p) {
			auto tile = province_tile{};
			tile.capital_administration = true;
			tile.empty = false;
			tile.province = p;
			push_tile(tiles, tile, curind);

			administration_found = true;
		}

		for(auto admin : state.world.nation_get_nation_administration(owner)) {
			if(admin.get_administration().get_capital() == p) {
				// There can be several administrations per state located in any province of the state
				auto tile = province_tile{};
				tile.local_administration = admin.get_administration();
				tile.empty = false;
				tile.province = p;
				push_tile(tiles, tile, curind);

				administration_found = true;
				break;
			}
		}

		// If there is no administration in the province, we display a tile with a small tax office.
		if(!administration_found) {
			auto tile = province_tile{};
			tile.no_administration_tile = true;
			tile.empty = false;
			tile.province = p;
			push_tile(tiles, tile, curind);
		}

		// Skip to the next row
		while(curind % 8 != 0) {
			curind++;
		}

		// Market is located in the capital of the state instabce
		if(state.world.state_instance_get_capital(si) == p) {
			auto market = state.world.state_instance_get_market_from_local_market(si);
			auto tile = province_tile{};
			tile.market = market;
			tile.empty = false;
			tile.province = p;
			push_tile(tiles, tile, curind);
		}
	}

	// Display factories and factories under construction for provinces with owner (colonized)
	if(owner) {
		// Display dirt roads when there are no railroads
		// if(state.world.province_get_building_level(p, uint8_t(economy::province_building_type::railroad) > 0) {
		{
			auto tile = province_tile{};
			tile.province_building = economy::province_building_type::railroad;
			tile.empty = false;
			tile.has_province_building = true;
			tile.province = p;
			push_tile(tiles, tile, curind);
		}

		for(auto f : state.world.in_factory) {
			if(f.get_factory_location().get_province() == p) {
				auto tile = province_tile{};
				tile.factory = f;
				tile.empty = false;
				tile.province = p;
				push_tile(tiles, tile, curind);
			}
		}

		for(auto fc : state.world.in_factory_construction) {
			if(fc.get_province() == p) {
				auto tile = province_tile{};
				tile.factory_construction = fc;
				tile.empty = false;
				tile.province = p;
				push_tile(tiles, tile, curind);
			}
		}

		// Plus sign to build
		auto tile = province_tile{};
		tile.build_new = true;
		tile.empty = false;
		tile.province = p;
		push_tile(tiles, tile, curind);
	}

	// Skip to the next row
	while(curind % 8 != 0) {
		curind++;
	}

	// Main province RGOs
	{
		auto tile = province_tile{};
		tile.commodity = state.world.province_get_rgo(p);
		tile.is_rgo = true;
		tile.empty = false;
		tile.province = p;
		push_tile(tiles, tile, curind);
	}

	// Secondary RGOs
	for(auto c : state.world.in_commodity) {
		if(c == state.world.province_get_rgo(p)) {
			continue;
		}
		if(economy::rgo_max_employment(state, c, p) > 100.f) {
			auto tile = province_tile{};
			tile.commodity = c;
			tile.is_rgo = true;
			tile.empty = false;
			tile.province = p;
			push_tile(tiles, tile, curind);
		}
	}

	// Unexploited resource potentials
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
			auto tile = province_tile{};
			tile.commodity = c;
			tile.is_resource_potential = true;
			tile.empty = false;
			tile.province = p;
			push_tile(tiles, tile, curind);
		}
	}

	// Skip to the next row
	while(curind % 8 != 0) {
		curind++;
	}

	if(state.world.province_get_building_level(p, uint8_t(economy::province_building_type::fort)) > 0) {
		auto tile = province_tile{};
		tile.province_building = economy::province_building_type::fort;
		tile.empty = false;
		tile.has_province_building = true;
		tile.province = p;
		push_tile(tiles, tile, curind);
	}

	if(state.world.province_get_building_level(p, uint8_t(economy::province_building_type::naval_base)) > 0) {
		auto tile = province_tile{};
		tile.province_building = economy::province_building_type::naval_base;
		tile.empty = false;
		tile.has_province_building = true;
		tile.province = p;
		push_tile(tiles, tile, curind);
	}

	// Display regiments originating from the province
	if(owner) {
		for(auto r : state.world.in_regiment_source) {
			if(r.get_pop().get_province_from_pop_location() == p) {
				auto tile = province_tile{};
				tile.regiment = r.get_regiment();
				tile.empty = false;
				tile.province = p;
				push_tile(tiles, tile, curind);
			}
		}
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
