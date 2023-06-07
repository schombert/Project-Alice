#pragma once

std::vector<uint32_t> infrastructure_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	int32_t max_rails_lvl = state.economy_definitions.railroad_definition.max_level;
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);

		int32_t current_rails_lvl = state.world.province_get_railroad_level(prov_id);
		int32_t max_local_rails_lvl = state.world.nation_get_max_railroad_level(state.local_player_nation);
		bool party_allows_building_railroads =
		    (nation == state.local_player_nation &&
		     (state.world.nation_get_combined_issue_rules(nation) & issue_rule::build_railway) != 0) ||
		    (nation != state.local_player_nation &&
		     (state.world.nation_get_combined_issue_rules(nation) & issue_rule::allow_foreign_investment) != 0);
		uint32_t color;

		if(party_allows_building_railroads) {

			if(province::can_build_railroads(state, prov_id, state.local_player_nation)) {

				color = ogl::color_gradient(float(current_rails_lvl) / float(max_rails_lvl), sys::pack_color(14, 240, 44), // green
				                            sys::pack_color(41, 5, 245)                                                    // blue

				);

			} else if(current_rails_lvl == max_local_rails_lvl) {
				color = sys::pack_color(232, 228, 111); // yellow
			} else {
				color = sys::pack_color(222, 7, 46); // red
			}

		} else {
			color = sys::pack_color(232, 228, 111); // yellow
		}
		auto i = province::to_map_id(prov_id);

		prov_color[i] = color;

		if(province::has_railroads_being_built(state, prov_id)) {
			prov_color[i + texture_size] = sys::pack_color(232, 228, 111); // yellow
		} else {
			prov_color[i + texture_size] = color;
		}
	});

	return prov_color;
}
