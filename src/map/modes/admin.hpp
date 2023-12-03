std::vector<uint32_t> admin_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto nation = fat_id.get_nation_from_province_ownership();

		if(nation == state.local_player_nation) {
			auto admin_efficiency = province::state_admin_efficiency(state, fat_id.get_state_membership());

			uint32_t color = ogl::color_gradient(admin_efficiency, sys::pack_color(46, 247, 15), // red
					sys::pack_color(247, 15, 15)																										 // green
			);

			auto i = province::to_map_id(prov_id);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}

std::vector<uint32_t> civilization_level_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		auto status = nations::get_status(state, nation);

		uint32_t color;

		// if it is uncolonized
		if(!nation) {

			color = sys::pack_color(250, 5, 5); // red

		} else if(state.world.nation_get_is_civilized(nation)) {

			color = sys::pack_color(53, 196, 53); // green

		} else {

			float civ_level = state.world.nation_get_modifier_values(nation, sys::national_mod_offsets::civilization_progress_modifier);
			// gray <-> yellow
			color = ogl::color_gradient(civ_level * (1 + (1 - civ_level)), sys::pack_color(250, 250, 5), sys::pack_color(64, 64, 64));
		}
		auto i = province::to_map_id(prov_id);

		prov_color[i] = color;
		if(!state.world.province_get_is_colonial(prov_id)) {
			prov_color[i + texture_size] = color;
		} else {
			prov_color[i + texture_size] = sys::pack_color(53, 53, 250);
		}
	});
	return prov_color;
}

std::vector<uint32_t> colonial_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto i = province::to_map_id(prov_id);

		if(!(fat_id.get_nation_from_province_ownership())) {
			if(province::is_colonizing(state, state.local_player_nation, fat_id.get_state_from_abstract_state_membership())) {
				if(province::can_invest_in_colony(state, state.local_player_nation, fat_id.get_state_from_abstract_state_membership())) {
					prov_color[i] = sys::pack_color(46, 247, 15);
					prov_color[i + texture_size] = sys::pack_color(46, 247, 15);
				} else {
					prov_color[i] = sys::pack_color(250, 250, 5);
					prov_color[i + texture_size] = sys::pack_color(250, 250, 5);
				}
			} else if(province::can_start_colony(state, state.local_player_nation, fat_id.get_state_from_abstract_state_membership())) {
				prov_color[i] = sys::pack_color(46, 247, 15);
				prov_color[i + texture_size] = sys::pack_color(46, 247, 15);
			} else {
				prov_color[i] = sys::pack_color(247, 15, 15);
				prov_color[i + texture_size] = sys::pack_color(247, 15, 15);
			}
		}
	});
	return prov_color;
}

#pragma once
std::vector<uint32_t> crisis_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto nation = fat_id.get_nation_from_province_ownership();
		auto i = province::to_map_id(prov_id);

		if(nation) {
			auto color = ogl::color_gradient(fat_id.get_state_membership().get_flashpoint_tension() / 100.0f, sys::pack_color(247, 15, 15), sys::pack_color(46, 247, 15));
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}

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

	auto fat_selected_id = dcon::fatten(state.world, state.map_state.get_selected_province());
	auto selected_nation = fat_selected_id.get_nation_from_province_ownership();

	if(!bool(selected_nation)) {
		selected_nation = state.local_player_nation;
	}

	std::vector<dcon::nation_id> enemies, allies, sphere;

	// Get all enemies
	for(auto wa : state.world.nation_get_war_participant(selected_nation)) {
		bool is_attacker = wa.get_is_attacker();
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_is_attacker() != is_attacker) {
				enemies.push_back(o.get_nation().id);
			}
		}
	}

	// Get all allies
	selected_nation.for_each_diplomatic_relation([&](dcon::diplomatic_relation_fat_id relation_id) {
		if(relation_id.get_are_allied()) {
			dcon::nation_id ally_id = relation_id.get_related_nations(0).id != selected_nation.id
				? relation_id.get_related_nations(0).id
				: relation_id.get_related_nations(1).id;
			allies.push_back(ally_id);
		}
	});

	auto selected_primary_culture = selected_nation.get_primary_culture();

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto i = province::to_map_id(prov_id);
		uint32_t color = 0x222222;
		uint32_t stripe_color = 0x222222;

		auto fat_owner = fat_id.get_province_control().get_nation(); // hehe

		if(bool(fat_owner)) {

			// Selected nation
			if(fat_owner.id == selected_nation.id) {
				color = selected_color;
				// By default asume its not a core
				stripe_color = non_cores_color;
			} else {
				// Cultural Union
				auto cultural_union_identity =
					selected_primary_culture.get_culture_group_membership().get_group().get_identity_from_cultural_union_of();
				fat_id.for_each_core([&](dcon::core_fat_id core_id) {
					if(core_id.get_identity().id == cultural_union_identity.id) {
						stripe_color = cultural_union_color;
					}
				});

				// Sphere
				auto master_rel_id = state.world.get_gp_relationship_by_gp_influence_pair(fat_owner, selected_nation);
				if(bool(master_rel_id) && state.world.gp_relationship_get_status(master_rel_id) == nations::influence::level_in_sphere) {
					color = sphere_color;
				}

				// Puppets
				auto province_overlord_id = fat_id.get_province_ownership().get_nation().get_overlord_as_subject();
				if(bool(province_overlord_id) && province_overlord_id.get_ruler().id == selected_nation.id) {
					color = puppet_color;
				} else {
					auto selected_overlord_id = selected_nation.get_overlord_as_subject();
					if(bool(selected_overlord_id) && selected_overlord_id.get_ruler().id == fat_owner.id) {
						color = puppet_color;
					}
				}

				// War
				if(std::find(enemies.begin(), enemies.end(), fat_owner.id) != enemies.end()) {
					color = war_color;
				}

				// Allies
				if(std::find(allies.begin(), allies.end(), fat_owner.id) != allies.end()) {
					color = ally_color;
				}
			}

			// Core
			fat_id.for_each_core([&](dcon::core_fat_id core_id) {
				if(core_id.get_identity().get_nation_from_identity_holder().id == selected_nation.id) {
					stripe_color = selected_color;
				}
			});
		}
		// If no stripe has been set, use the prov color
		if(stripe_color == 0x222222) {
			stripe_color = color;
		}
		prov_color[i] = color;
		prov_color[i + texture_size] = stripe_color;
	});

	return prov_color;
}

std::vector<uint32_t> diplomatic_map_from(sys::state& state) {
	std::vector<uint32_t> prov_color;
	if(state.map_state.get_selected_province()) {
		prov_color = get_selected_diplomatic_color(state);
	} else {
		prov_color = get_selected_diplomatic_color(state);
	}
	return prov_color;
}


std::vector<uint32_t> infrastructure_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	int32_t max_rails_lvl = state.economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].max_level;
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		int32_t current_rails_lvl = state.world.province_get_building_level(prov_id, economy::province_building_type::railroad);
		int32_t max_local_rails_lvl = state.world.nation_get_max_building_level(state.local_player_nation, economy::province_building_type::railroad);
		bool party_allows_building_railroads =
			(nation == state.local_player_nation &&
					(state.world.nation_get_combined_issue_rules(nation) & issue_rule::build_railway) != 0) ||
			(nation != state.local_player_nation &&
					(state.world.nation_get_combined_issue_rules(nation) & issue_rule::allow_foreign_investment) != 0);
		uint32_t color = 0;
		if(party_allows_building_railroads) {
			if(province::can_build_railroads(state, prov_id, state.local_player_nation)) {
				color = ogl::color_gradient(float(current_rails_lvl) / float(max_rails_lvl), sys::pack_color(14, 240, 44), // green
						sys::pack_color(41, 5, 245)																																						 // blue);
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


std::vector<uint32_t> migration_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	auto selected = state.map_state.selected_province;
	auto for_nation = state.world.province_get_nation_from_province_ownership(selected);
	if(for_nation) {
		float mx = 0.0f;
		float mn = 0.0f;
		for(auto p : state.world.nation_get_province_ownership(for_nation)) {
			auto v = p.get_province().get_daily_net_migration();
			mn = std::min(mn, v);
			mx = std::max(mx, v);
		}
		if(mx > mn) {
			for(auto p : state.world.nation_get_province_ownership(for_nation)) {
				auto v = p.get_province().get_daily_net_migration();

				uint32_t color = ogl::color_gradient((v - mn) / (mx - mn),
					sys::pack_color(46, 247, 15),	// to green
					sys::pack_color(247, 15, 15)	// from red
				);
				auto i = province::to_map_id(p.get_province());
				prov_color[i] = color;
				prov_color[i + texture_size] = color;
			}
		}
	} else {
		static auto last_checked_date = sys::date{};
		static std::vector<float> nation_totals;

		if(state.ui_date != last_checked_date) {
			last_checked_date = state.ui_date;

			auto sz = state.world.nation_size();
			if(uint32_t(nation_totals.size()) < sz) {
				nation_totals.resize(sz);
			}

			for(uint32_t i = 0; i < sz; ++i) {
				nation_totals[i] = 0.0f;
			}
			float least_neg = 0.0f;
			float greatest_pos = 0.0f;
			for(auto p : state.world.in_province) {
				auto owner = p.get_nation_from_province_ownership();
				if(owner && uint32_t(owner.id.index()) < sz) {
					auto v = p.get_daily_net_immigration();
					nation_totals[owner.id.index()] += v;
				}
			}
			for(uint32_t i = 0; i < sz; ++i) {
				if(nation_totals[i] < 0.0f)
					least_neg = std::min(nation_totals[i], least_neg);
				else
					greatest_pos = std::max(nation_totals[i], greatest_pos);
			}
			for(uint32_t i = 0; i < sz; ++i) {
				if(nation_totals[i] < 0.0f) {
					nation_totals[i] = 0.5f - 0.5f * nation_totals[i] / least_neg;
				} else if(nation_totals[i] > 0.0f) {
					nation_totals[i] = 0.5f + 0.5f * nation_totals[i] / greatest_pos;
				}
			}
		}
		for(auto p : state.world.in_province) {
			auto owner = p.get_nation_from_province_ownership();
			if(owner && uint32_t(owner.id.index()) < nation_totals.size()) {
				uint32_t color = ogl::color_gradient(nation_totals[owner.id.index()],
					sys::pack_color(46, 247, 15),	// to green
					sys::pack_color(247, 15, 15)	// from red
				);
				auto i = province::to_map_id(p);
				prov_color[i] = color;
				prov_color[i + texture_size] = color;
			}
		}
	}
	return prov_color;
}


std::vector<uint32_t> get_nationality_global_color(sys::state& state) {
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto id = province::to_map_id(prov_id);
		float total_pops = state.world.province_get_demographics(prov_id, demographics::total);

		dcon::culture_id primary_culture, secondary_culture;
		float primary_culture_percent = 0.f, secondary_culture_percent = 0.f;

		state.world.for_each_culture([&](dcon::culture_id culture_id) {
			auto demo_key = demographics::to_key(state, culture_id);
			auto volume = state.world.province_get_demographics(prov_id, demo_key);
			float percent = volume / total_pops;

			if(percent > primary_culture_percent) {
				secondary_culture = primary_culture;
				secondary_culture_percent = primary_culture_percent;
				primary_culture = culture_id;
				primary_culture_percent = percent;
			} else if(percent > secondary_culture_percent) {
				secondary_culture = culture_id;
				secondary_culture_percent = percent;
			}
		});

		dcon::culture_fat_id fat_primary_culture = dcon::fatten(state.world, primary_culture);

		uint32_t primary_culture_color = fat_primary_culture.get_color();
		uint32_t secondary_culture_color = 0xFFAAAAAA; // This color won't be reached

		if(bool(secondary_culture)) {
			dcon::culture_fat_id fat_secondary_culture = dcon::fatten(state.world, secondary_culture);
			secondary_culture_color = fat_secondary_culture.get_color();
		}

		if(secondary_culture_percent >= .35) {
			prov_color[id] = primary_culture_color;
			prov_color[id + texture_size] = secondary_culture_color;
		} else {
			prov_color[id] = primary_culture_color;
			prov_color[id + texture_size] = primary_culture_color;
		}
	});

	return prov_color;
}

std::vector<uint32_t> get_nationality_diaspora_color(sys::state& state) {
	auto fat_selected_id = dcon::fatten(state.world, state.map_state.get_selected_province());
	auto culture_id = fat_selected_id.get_dominant_culture();
	auto culture_key = demographics::to_key(state, culture_id.id);

	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	if(bool(culture_id)) {
		uint32_t full_color = culture_id.get_color();
		uint32_t empty_color = 0xDDDDDD;
		// Make the other end of the gradient dark if the color is bright and vice versa.
		// This should make it easier to see cultures that would otherwise be problematic.
		if((full_color & 0xFF) + (full_color >> 8 & 0xFF) + (full_color >> 16 & 0xFF) > 140 * 3) {
			empty_color = 0x222222;
		}

		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto i = province::to_map_id(prov_id);
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto total_pop = state.world.province_get_demographics(prov_id, demographics::total);
			auto culture_pop = state.world.province_get_demographics(prov_id, culture_key);
			auto ratio = culture_pop / total_pop;

			auto color = ogl::color_gradient(ratio, full_color, empty_color);

			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		});
	}
	return prov_color;
}

std::vector<uint32_t> nationality_map_from(sys::state& state) {
	std::vector<uint32_t> prov_color;
	if(state.map_state.get_selected_province()) {
		prov_color = get_nationality_diaspora_color(state);
	} else {
		prov_color = get_nationality_global_color(state);
	}

	return prov_color;
}

std::vector<uint32_t> national_focus_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto nation = fat_id.get_nation_from_province_ownership();
		auto i = province::to_map_id(prov_id);

		if(nation == state.local_player_nation && fat_id.get_state_membership().get_owner_focus()) {
			prov_color[i] = sys::pack_color(46, 247, 15);
			prov_color[i + texture_size] = sys::pack_color(46, 247, 15);
		}
	});
	return prov_color;
}

std::vector<uint32_t> naval_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto nation = fat_id.get_nation_from_province_ownership();

		if(nation == state.local_player_nation) {
			uint32_t color = 0x222222;
			uint32_t stripe_color = 0x222222;

			if(fat_id.get_is_coast()) {
				auto state_id = fat_id.get_abstract_state_membership();
				color = ogl::color_from_hash(state_id.get_state().id.index());
				stripe_color = 0x00FF00; // light green

				auto state_has_naval_base = military::state_has_naval_base(state, fat_id.get_state_membership());
				auto naval_base_lvl = fat_id.get_building_level(economy::province_building_type::naval_base);
				auto max_naval_base_lvl = state.world.nation_get_max_building_level(nation, economy::province_building_type::naval_base);

				if(state_has_naval_base && naval_base_lvl == 0) {
					stripe_color = ogl::color_from_hash(state_id.get_state().id.index());
				} else if(naval_base_lvl == max_naval_base_lvl) {
					stripe_color = 0x005500; // dark green
				}
			}

			auto i = province::to_map_id(prov_id);
			prov_color[i] = color;
			prov_color[i + texture_size] = stripe_color;
		}
	});

	return prov_color;
}

struct party_info {
	dcon::ideology_id ideology;
	float loyalty;
	uint32_t color;
};

std::vector<party_info> get_sorted_parties_info(sys::state& state, dcon::province_id prov_id) {
	std::vector<party_info> result;

	state.world.for_each_ideology([&](dcon::ideology_id ideology) {
		auto loyalty = state.world.province_get_party_loyalty(prov_id, ideology);
		if(loyalty > 0) {
			result.push_back({ ideology, loyalty, state.world.ideology_get_color(ideology) });
		}
	});

	std::sort(result.begin(), result.end(), [&](party_info a, party_info b) { return a.loyalty > b.loyalty; });

	return result;
}

std::vector<uint32_t> party_loyalty_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto parties_info = get_sorted_parties_info(state, prov_id);

		auto i = province::to_map_id(prov_id);

		if(parties_info.size() == 0) {

			prov_color[i] = 0xFFFFFF;
			prov_color[i + texture_size] = 0xFFFFFF;

		} else if(parties_info.size() == 1) {

			uint32_t color = parties_info[0].color;
			prov_color[i] = color;
			prov_color[i + texture_size] = color;

		} else {
			party_info a = parties_info[0];
			party_info b = parties_info[1];

			prov_color[i] = a.color;

			if(b.loyalty >= a.loyalty * 0.75) {
				prov_color[i + texture_size] = b.color;
			} else {
				prov_color[i + texture_size] = a.color;
			}
		}
	});

	return prov_color;
}

std::vector<uint32_t> political_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto id = fat_id.get_nation_from_province_ownership();
		uint32_t color = 0;
		if(bool(id))
			color = id.get_color();
		else // If no owner use default color
			color = 255 << 16 | 255 << 8 | 255;
		auto i = province::to_map_id(prov_id);

		auto occupier = fat_id.get_nation_from_province_control();
		uint32_t color_b = occupier ? occupier.get_color() : (id ? sys::pack_color(127, 127, 127) : sys::pack_color(255, 255, 255));

		prov_color[i] = color;
		prov_color[i + texture_size] = color_b;
	});

	return prov_color;
}

