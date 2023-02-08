#include "map_modes.hpp"

#include "system_state.hpp"
#include "dcon_generated.hpp"
#include "province.hpp"

namespace map_mode {

void set_political(sys::state& state) {
	std::vector<uint32_t> prov_color(state.world.province_size() + 1);

	state.world.for_each_province([&](dcon::province_id prov_id){
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto id = fat_id.get_nation_from_province_ownership();
		uint32_t color;
		if(bool(id))
			color = id.get_identity_from_identity_holder().get_color();
		else // If no owner use default color
			color = 255 << 16 | 255 << 8 | 255;
		auto i = province::to_map_id(prov_id);
		prov_color[i] = color;
	});

	state.map_display.set_province_color(prov_color, mode::political);
}

void set_map_mode(sys::state& state, mode mode) {
	switch (mode)
	{
	case mode::terrain:
		state.map_display.set_terrain_map_mode();
		break;
	case mode::political:
		set_political(state);
		break;
	default:
		break;
	}
}
}
