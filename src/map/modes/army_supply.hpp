#pragma once

std::vector<uint32_t> army_supply_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2, sys::pack_color(55, 55, 55));
	std::vector<float> worst_supply(province_size, 1.0f);
	std::vector<bool> has_army(province_size, false);

	if(state.local_player_nation) {
		military::update_army_supply_cache(state);
		for(auto army_control : state.world.nation_get_army_control(state.local_player_nation)) {
			auto army = army_control.get_army();
			auto province = army.get_location_from_army_location();
			if(!province || province.id.index() >= int32_t(province_size)) {
				continue;
			}
			auto access = military::calculate_army_supply_access_cached(state, army.id);
			has_army[province.id.index()] = true;
			worst_supply[province.id.index()] = std::min(worst_supply[province.id.index()], access.effective_supply);
		}
	}

	state.world.for_each_province([&](dcon::province_id province) {
		if(military::is_supply_depot(state, province)) {
			auto ratio = military::supply_depot_capacity(state, province) > 0.0f
				? state.world.province_get_supply_depot_stockpile(province) / military::supply_depot_capacity(state, province) : 0.0f;
			auto color = ogl::color_gradient(std::clamp(ratio, 0.0f, 1.0f), sys::pack_color(36, 186, 242),
				sys::pack_color(20, 45, 95));
			auto map_id = province::to_map_id(province);
			prov_color[map_id] = color;
			prov_color[map_id + texture_size] = color;
		}
		if(!has_army[province.index()]) {
			return;
		}
		auto color = ogl::color_gradient(worst_supply[province.index()], sys::pack_color(46, 247, 15),
			sys::pack_color(247, 15, 15));
		auto map_id = province::to_map_id(province);
		prov_color[map_id] = color;
		prov_color[map_id + texture_size] = color;
	});
	return prov_color;
}
