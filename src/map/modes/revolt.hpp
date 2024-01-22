#pragma once


std::vector<uint32_t> revolt_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	std::unordered_map<uint16_t, float> rebels_in_province = {};
	std::unordered_map<int32_t, float> continent_max_rebels = {};
	state.world.for_each_rebel_faction([&](dcon::rebel_faction_id id) {
		auto rebellion = dcon::fatten(state.world, id);
		if((sel_nation && sel_nation == rebellion.get_ruler_from_rebellion_within().id) || !sel_nation) {
			for(auto members : state.world.rebel_faction_get_pop_rebellion_membership(id)) {
				rebels_in_province[province::to_map_id(members.get_pop().get_province_from_pop_location().id)] += members.get_pop().get_size();
			}
		}
	});
	for(auto& [p, value] : rebels_in_province) {
		auto pid = province::from_map_id(p);
		auto cid = dcon::fatten(state.world, pid).get_continent().id.index();
		continent_max_rebels[cid] = std::max(continent_max_rebels[cid], value);
	}
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto i = province::to_map_id(prov_id);
			auto cid = fat_id.get_continent().id.index();

			uint32_t color = 0xDDDDDD; // white
			if(rebels_in_province[i] > 0.0f) {
				float gradient_index = (continent_max_rebels[cid] == 0.f ? 0.f : (rebels_in_province[i] / continent_max_rebels[cid]));
				color = ogl::color_gradient(gradient_index,
						sys::pack_color(247, 15, 15), // red
						sys::pack_color(46, 247, 15) // green
				);
			}
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
