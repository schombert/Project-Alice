#include "province.hpp"
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include <vector>

namespace province {

template<typename F>
void for_each_land_province(sys::state& state, F const& func) {
	int32_t last = state.province_definitions.first_sea_province.index();
	for(int32_t i = 0; i < last; ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		func(pid);
	}
}

template<typename F>
void for_each_sea_province(sys::state& state, F const& func) {
	int32_t first = state.province_definitions.first_sea_province.index();
	for(int32_t i = first; i < int32_t(state.world.province_size()); ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		func(pid);
	}
}

bool nations_are_adjacent(sys::state& state, dcon::nation_id a, dcon::nation_id b) {
	auto it = state.world.get_nation_adjacency_by_nation_adjacency_pair(a, b);
	return bool(it);
}

void update_connected_regions(sys::state& state) {
	if(!state.adjacency_data_out_of_date)
		return;

	state.adjacency_data_out_of_date = false;

	state.world.nation_adjacency_resize(0);

	state.world.for_each_province([&](dcon::province_id id) {
		state.world.province_set_connected_region_id(id, 0);
	});
	// TODO get a better allocator
	static std::vector<dcon::province_id> to_fill_list;
	uint16_t current_fill_id = 0;

	to_fill_list.reserve(state.world.province_size());

	for(int32_t i = state.province_definitions.first_sea_province.index(); i-- > 0; ) {
		dcon::province_id id{ dcon::province_id ::value_base_t(i) };
		if(state.world.province_get_connected_region_id(id) == 0) {
			++current_fill_id;
			
			to_fill_list.push_back(id);

			while(!to_fill_list.empty()) {
				auto current_id = to_fill_list.back();
				to_fill_list.pop_back();

				state.world.province_set_connected_region_id(current_id, current_fill_id);
				for(auto rel : state.world.province_get_province_adjacency(current_id)) {
					if((rel.get_type() & (province::border::coastal_bit | province::border::impassible_bit)) == 0) { // not entering sea, not impassible
						auto owner_a = rel.get_connected_provinces(0).get_nation_from_province_ownership();
						auto owner_b = rel.get_connected_provinces(1).get_nation_from_province_ownership();
						if(owner_a == owner_b) { // both have the same owner
							if(rel.get_connected_provinces(0).get_connected_region_id() == 0)
								to_fill_list.push_back(rel.get_connected_provinces(0));
							if(rel.get_connected_provinces(1).get_connected_region_id() == 0)
								to_fill_list.push_back(rel.get_connected_provinces(1));
						} else {
							state.world.try_create_nation_adjacency(owner_a, owner_b);
						}
					}
				}
			}

			to_fill_list.clear();
		}
	}
}


void restore_unsaved_values(sys::state& state) {
	for(int32_t i = 0; i < state.province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		[&]() {
			for(auto adj : state.world.province_get_province_adjacency(pid)) {
				if((state.world.province_adjacency_get_type(adj) & province::border::coastal_bit) != 0) {
					state.world.province_set_is_coast(pid, true);
					return;
				}
			}
		}();
	}
}

}
