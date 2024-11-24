#pragma once

std::vector<uint32_t> admin_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);
	dcon::province_id selected_province = state.map_state.get_selected_province();
	dcon::nation_id selected_nation = selected_province
		? state.world.province_get_nation_from_province_ownership(selected_province)
		: state.local_player_nation;
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto i = province::to_map_id(prov_id);
		auto fat_id = dcon::fatten(state.world, prov_id);
		if(!selected_nation || fat_id.get_nation_from_province_ownership() == selected_nation) {
			auto admin_efficiency = province::state_admin_efficiency(state, fat_id.get_state_membership());
			uint32_t color = ogl::color_gradient_viridis(admin_efficiency);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		} else {
			prov_color[i] = 0;
			prov_color[i + texture_size] = 0;
		}
	});
	return prov_color;
}
