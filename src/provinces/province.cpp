#include "province.hpp"
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include <vector>

namespace province {

void update_connected_regions(sys::state& state) {
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
						if(rel.get_connected_provinces(0).get_nation_from_province_ownership() == rel.get_connected_provinces(1).get_nation_from_province_ownership()) { // both have the same owner
							if(rel.get_connected_provinces(0).get_connected_region_id() == 0)
								to_fill_list.push_back(rel.get_connected_provinces(0));
							if(rel.get_connected_provinces(1).get_connected_region_id() == 0)
								to_fill_list.push_back(rel.get_connected_provinces(1));
						}
					}
				}
			}

			to_fill_list.clear();
		}
	}
}

}
