#pragma once

std::vector<uint32_t> get_selected_diplomatic_color(sys::state& state) {
	/**
	 * Color:
	 *	- Yellorange -> Casus belli TODO: How do I get the casus belli?
	 *  - Skyblue -> Ally -> X
	 *  - Green stripes -> Cores X
	 *  - White stripes -> Not cores X
	 *  - Yellow stripes -> Nation cultural union X
	 *  - Red -> War X
	 *  - Green -> Selected X
	 *  - Light Green -> Sphereling X
	 *  - Dark green -> Puppet or puppet master X
	 */

	 // This could be stored in an other place
	uint32_t causus_belli_color = 0x00AAFF;
	uint32_t ally_color = 0xFFAA00;
	uint32_t selected_color = 0x00FF00; // Also unclaimed cores stripes color
	uint32_t sphere_color = 0x55AA55;
	uint32_t puppet_color = 0x009900;
	uint32_t non_cores_color = 0xFFFFFF;
	uint32_t war_color = 0x0000FF;
	uint32_t cultural_union_color = 0x00FFFF;

	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);

	auto fat_selected_id = dcon::fatten(state.world, state.map_display.get_selected_province());
	auto selected_nation = fat_selected_id.get_nation_from_province_ownership();

	if(!bool(selected_nation)) {
		selected_nation = state.local_player_nation;
	}

	std::vector<dcon::nation_id> enemies, allies, sphere;

	// Get all enemies
	for (auto wa : state.world.nation_get_war_participant(selected_nation)) {
		bool is_attacker = wa.get_is_attacker();
		for (auto o : wa.get_war().get_war_participant()) {
			if (o.get_is_attacker() != is_attacker) {
				enemies.push_back(o.get_nation().id);
			}
		}
	}

	// Get all allies
	selected_nation.for_each_diplomatic_relation([&](dcon::diplomatic_relation_fat_id relation_id) {
		if (relation_id.get_are_allied()) {
			dcon::nation_id ally_id = relation_id.get_related_nations(0).id != selected_nation.id ? relation_id.get_related_nations(0).id : relation_id.get_related_nations(1).id;
			allies.push_back(ally_id);
		}
		});

	auto selected_primary_culture = selected_nation.get_primary_culture();

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto i = province::to_map_id(prov_id);
		uint32_t color = 0x222222;
		uint32_t stripe_color = 0x222222;

		auto fat_owner = fat_id.get_province_control().get_nation();  // hehe

		if (bool(fat_owner)) {

			// Selected nation
			if (fat_owner.id == selected_nation.id) {
				color = selected_color;
				// By default asume its not a core
				stripe_color = non_cores_color;
			}
			else {
				// Cultural Union
				auto cultural_union_identity = selected_primary_culture.get_culture_group_membership().get_group().get_identity_from_cultural_union_of();
				fat_id.for_each_core([&](dcon::core_fat_id core_id) {
					if (core_id.get_identity().id == cultural_union_identity.id) {
						stripe_color = cultural_union_color;
					}
					});

				// Sphere
				auto master_rel_id = state.world.get_gp_relationship_by_gp_influence_pair(fat_owner, selected_nation);
				if (bool(master_rel_id) && state.world.gp_relationship_get_status(master_rel_id) == nations::influence::level_in_sphere) {
					color = sphere_color;
				}

				// Puppets
				auto province_overlord_id = fat_id.get_province_ownership().get_nation().get_overlord_as_subject();
				if (bool(province_overlord_id) && province_overlord_id.get_ruler().id == selected_nation.id) {
					color = puppet_color;
				}
				else {
					auto selected_overlord_id = selected_nation.get_overlord_as_subject();
					if (bool(selected_overlord_id) && selected_overlord_id.get_ruler().id == fat_owner.id) {
						color = puppet_color;
					}
				}

				// War
				if (std::find(enemies.begin(), enemies.end(), fat_owner.id) != enemies.end()) {
					color = war_color;
				}

				// Allies
				if (std::find(allies.begin(), allies.end(), fat_owner.id) != allies.end()) {
					color = ally_color;
				}
			}

			// Core
			fat_id.for_each_core([&](dcon::core_fat_id core_id) {
				if (core_id.get_identity().get_nation_from_identity_holder().id == selected_nation.id) {
					stripe_color = selected_color;
				}
				});
		}

		// If no stripe has been set, use the prov color
		if (stripe_color == 0x222222) {
			stripe_color = color;
		}

		prov_color[i] = color;
		prov_color[i + texture_size] = stripe_color;
	});
	
	return prov_color;
}

std::vector<uint32_t> diplomatic_map_from(sys::state& state) {
	std::vector<uint32_t> prov_color;

	if (state.map_display.get_selected_province()) {
		prov_color = get_selected_diplomatic_color(state);
	}
	else {
		prov_color = get_selected_diplomatic_color(state);
	}

	return prov_color;
}
