#pragma once

std::vector<uint32_t> region_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto id = fat_id.get_abstract_state_membership();
		uint32_t color = ogl::color_from_hash(id.get_state().id.index());
		auto i = province::to_map_id(prov_id);
		prov_color[i] = color;
		prov_color[i + texture_size] = color;
	});

	return prov_color;
}
