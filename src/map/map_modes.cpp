#include "map_modes.hpp"

#include "system_state.hpp"
#include "dcon_generated.hpp"
#include "province.hpp"

namespace map_mode {

void set_political(sys::state& state) {
	std::vector<uint32_t> prov_color(state.map_display.nr_of_provinces);

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

// borrowed from http://www.burtleburtle.net/bob/hash/doobs.html
inline constexpr uint32_t scramble(uint32_t color) {
	uint32_t m1 = 0x1337CAFE;
	uint32_t m2 = 0xDEADBEEF;
	m1 -= m2; m1 -= color; m1 ^= (color>>13); 
	m2 -= color; m2 -= m1; m2 ^= (m1<<8); 
	color -= m1; color -= m2; color ^= (m2>>13); 
	m1 -= m2; m1 -= color; m1 ^= (color>>12);  
	m2 -= color; m2 -= m1; m2 ^= (m1<<16); 
	color -= m1; color -= m2; color ^= (m2>>5); 
	m1 -= m2; m1 -= color; m1 ^= (color>>3);  
	m2 -= color; m2 -= m1; m2 ^= (m1<<10); 
	color -= m1; color -= m2; color ^= (m2>>15);
	return color;
}

void set_region(sys::state& state) {
	std::vector<uint32_t> prov_color(state.map_display.nr_of_provinces);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto id = fat_id.get_abstract_state_membership();
		uint32_t color = scramble(id.get_state().id.index());
		auto i = province::to_map_id(prov_id);
		prov_color[i] = color;
	});

	state.map_display.set_province_color(prov_color, mode::region);
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
	case mode::region:
		set_region(state);
		break;
	default:
		break;
	}
}
}
