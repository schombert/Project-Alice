#pragma once


std::vector<uint32_t> get_global_sphere_color(sys::state& state) {

	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto i = province::to_map_id(prov_id);

		auto owner = fat_id.get_nation_from_province_ownership();

		uint32_t color = 0x222222;

		if(bool(owner)) {
			if(nations::is_great_power(state, owner.id)) {
				color = owner.get_color();
			} else {
				auto master = owner.get_in_sphere_of();
				if(bool(master)) {
					color = master.get_color();
				}
			}
		}
		prov_color[i] = color;
		prov_color[i + texture_size] = color;
	});

	return prov_color;
}

std::vector<uint32_t> get_selected_sphere_color(sys::state& state) {
	/**
	 * Color logic
	 *	- GP -> Green
	 *  - In Sphere -> Yellow
	 *	- Is influenced -> Blue
	 *  - Is influenced by more than 1 -> Add Red Stripes
	 *
	 *  The concept of influenced is weird, if a tag has cordial relations with a gp but the influence is 0, then its not influenced
	 *  Only the influence value should be taken into account
	 *
	 * If country is in sphere or is sphere master
	 *	Paint only the tags influenced by the master, including spherearlings
	 * Else
	 *	Paint the tags that influence the country + the country
	**/

	// Todo: Get better colors?
	uint32_t gp_color = 0x00FF00;        // Green
	uint32_t inf_color = 0xFF0000;       // Blue
	uint32_t sphere_color = 0x00FFFF;    // Yellow
	uint32_t other_inf_color = 0x0000FF; // Red

	// Province color vector init
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);

	auto fat_selected_id = dcon::fatten(state.world, state.map_display.get_selected_province());
	auto selected_nation = fat_selected_id.get_nation_from_province_ownership();

	// Get sphere master if exists
	auto master = selected_nation.get_in_sphere_of();
	if(!bool(master) && nations::is_great_power(state, selected_nation)) {
		master = selected_nation;
	}

	// Paint only sphere countries
	if(bool(master)) {
		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto i = province::to_map_id(prov_id);

			uint32_t color = 0x222222;
			uint32_t stripe_color = 0x222222;

			auto owner = fat_id.get_nation_from_province_ownership();

			if(bool(owner)) {
				if(owner.id == master.id) {
					color = gp_color;
				} else {
					auto master_rel_id = state.world.get_gp_relationship_by_gp_influence_pair(owner, master);
					if(bool(master_rel_id)) {
						auto master_rel_status = state.world.gp_relationship_get_status(master_rel_id);
						auto master_rel_inf = state.world.gp_relationship_get_influence(master_rel_id);

						if(master_rel_status == nations::influence::level_in_sphere || master_rel_inf != 0) {
							owner.for_each_gp_relationship_as_influence_target([&](dcon::gp_relationship_id rel_id) {
								// Has more than one influencer
								if(rel_id != master_rel_id && state.world.gp_relationship_get_influence(rel_id) != 0 && state.world.gp_relationship_get_influence(rel_id) != 0) {
									stripe_color = other_inf_color;
								}
							});

							if(master_rel_status == nations::influence::level_in_sphere) {
								color = sphere_color;
							} else if(master_rel_inf != 0) {
								color = inf_color;
							}
						}
					}
				}
			}

			// If no stripe has been set, use the prov color
			if(stripe_color == 0x222222) {
				stripe_color = color;
			}

			prov_color[i] = color;
			prov_color[i + texture_size] = stripe_color;
		});
	} else { // Paint selected country and influencers
		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto i = province::to_map_id(prov_id);

			uint32_t color = 0x222222;
			uint32_t stripe_color = 0x222222;

			auto owner = fat_id.get_nation_from_province_ownership();
			if(bool(owner)) {
				bool is_gp = false;
				uint8_t rel_count = 0;

				selected_nation.for_each_gp_relationship_as_influence_target([&](dcon::gp_relationship_fat_id relation_id) {
					if(!is_gp && relation_id.get_influence() > 0) {
						if(owner.id == selected_nation.id) {
							rel_count++;
						}
						if(relation_id.get_great_power() == owner.id) {
							is_gp = true;
						}
					}
				});

				if(is_gp) {
					color = gp_color;
				} else if(rel_count >= 1) {
					color = inf_color;
					if(rel_count > 1) {
						stripe_color = other_inf_color;
					}
				}
			}

			// If no stripe has been set, use the prov color
			if(stripe_color == 0x222222) {
				stripe_color = color;
			}

			prov_color[i] = color;
			prov_color[i + texture_size] = stripe_color;
		});
	}

	return prov_color;
}

std::vector<uint32_t> sphere_map_from(sys::state& state) {
	std::vector<uint32_t> prov_color;

	if(state.map_display.get_selected_province()) {
		prov_color = get_selected_sphere_color(state);
	} else {
		prov_color = get_global_sphere_color(state);
	}

	return prov_color;
}
