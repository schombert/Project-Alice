#pragma once
std::vector<uint32_t> crisis_map_from(sys::state &state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	return prov_color;
}
