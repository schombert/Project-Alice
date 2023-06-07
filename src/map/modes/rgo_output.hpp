#pragma once
std::vector<uint32_t> rgo_output_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	auto selected_province = state.map_state.get_selected_province();

	if(selected_province) {
		auto searched_rgo = state.world.province_get_rgo(selected_province);
		float max_rgo_size = 0;

		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto prov_rgo = state.world.province_get_rgo(prov_id);

			if(searched_rgo == prov_rgo) {

				max_rgo_size = std::max(max_rgo_size, province::rgo_size(state, prov_id));
			}
		});

		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto prov_rgo = state.world.province_get_rgo(prov_id);

			if(searched_rgo == prov_rgo) {
				uint32_t color =
				    ogl::color_gradient(province::rgo_size(state, prov_id) / max_rgo_size, sys::pack_color(46, 247, 15), // red
				                        sys::pack_color(247, 15, 15)                                                     // green
				    );

				auto i = province::to_map_id(prov_id);
				prov_color[i] = color;
				prov_color[i + texture_size] = color;
			}
		});
	}

	return prov_color;
}
