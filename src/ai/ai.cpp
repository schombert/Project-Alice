#include "ai.hpp"
#include "system_state.hpp"

namespace ai {

void update_ai_general_status(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(state.world.nation_get_owned_province_count(n) == 0) {
			state.world.nation_set_ai_is_threatened(n, false);
			state.world.nation_set_ai_rival(n, dcon::nation_id{});
			continue;
		}

		auto ll = state.world.nation_get_last_war_loss(n);
		float safety_factor = 1.2f;
		if(ll && state.current_date < ll + 365 * 4) {
			safety_factor = 1.8f;
		}
		auto in_sphere_of = state.world.nation_get_in_sphere_of(n);

		float greatest_neighbor = 0.0f;
		for(auto b : state.world.nation_get_nation_adjacency_as_connected_nations(n)) {
			auto other = b.get_connected_nations(0) != n ? b.get_connected_nations(0) : b.get_connected_nations(1);
			if(!nations::are_allied(state, n, other) && (!in_sphere_of || in_sphere_of != other.get_in_sphere_of())) {
				greatest_neighbor = std::max(greatest_neighbor, float(state.world.nation_get_military_score(other)));
			}
		}

		float self_str = float(state.world.nation_get_military_score(n));
		for(auto subj : n.get_overlord_as_ruler()) {
			self_str += 0.75f * float(subj.get_subject().get_military_score());
		}

		float defensive_str = self_str;
		defensive_str += in_sphere_of ? float(state.world.nation_get_military_score(in_sphere_of)) : 0.0f;
		for(auto d : state.world.nation_get_diplomatic_relation(n)) {
			if(d.get_are_allied()) {
				auto other = d.get_related_nations(0) != n ? d.get_related_nations(0) : d.get_related_nations(1);
				defensive_str += float(state.world.nation_get_military_score(other));
			}
		}

		bool threatened = defensive_str < safety_factor * greatest_neighbor;
		state.world.nation_set_ai_is_threatened(n, threatened);

		if(!n.get_ai_rival()) {
			float min_relation = 200.0f;
			dcon::nation_id potential;
			for(auto adj : n.get_nation_adjacency()) {
				auto other = adj.get_connected_nations(0) != n ? adj.get_connected_nations(0) : adj.get_connected_nations(1);
				auto ol = other.get_overlord_as_subject().get_ruler();
				if(!ol && other.get_in_sphere_of() != n && (!threatened || !nations::are_allied(state, n, other))) {
					auto other_str = float(other.get_military_score());
					if(self_str * 0.5f < other_str && other_str <= self_str * 1.25) {
						auto rel = state.world.diplomatic_relation_get_value(state.world.get_diplomatic_relation_by_diplomatic_pair(n, other));
						if(rel < min_relation) {
							min_relation = rel;
							potential = other;
						}
					}
				}
			}

			if(potential) {
				if(!n.get_is_player_controlled() && nations::are_allied(state, n, potential)) {
					command::cancel_alliance(state, n, potential);
				}
				n.set_ai_rival(potential);
			}
		} else {
			auto rival_str = float(n.get_ai_rival().get_military_score());
			auto ol = n.get_ai_rival().get_overlord_as_subject().get_ruler();
			if(ol || n.get_ai_rival().get_in_sphere_of() == n || rival_str * 2 < self_str || self_str * 2 < rival_str) {
				n.set_ai_rival(dcon::nation_id{});
			}
		}
	}
}

void form_alliances(sys::state& state) {
	static std::vector<dcon::nation_id> alliance_targets;

	for(auto n : state.world.in_nation) {
		if(n.get_is_player_controlled() == false && n.get_ai_is_threatened() && !(n.get_overlord_as_subject().get_ruler())) {
			alliance_targets.clear();

			for(auto nb : n.get_nation_adjacency()) {
				auto other = nb.get_connected_nations(0) != n ? nb.get_connected_nations(0) : nb.get_connected_nations(1);
				if(other.get_is_player_controlled() == false && !(other.get_overlord_as_subject().get_ruler())  && !nations::are_allied(state, n, other) && ai_will_accept_alliance(state, other, n)) {
					alliance_targets.push_back(other.id);
				}
			}

			if(!alliance_targets.empty()) {
				std::sort(alliance_targets.begin(), alliance_targets.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					if(state.world.nation_get_military_score(a) != state.world.nation_get_military_score(b))
						return state.world.nation_get_military_score(a) > state.world.nation_get_military_score(b);
					else
						return a.index() > b.index();
				});

				nations::make_alliance(state, n, alliance_targets[0]);
			}
		}
	}
}

bool ai_will_accept_alliance(sys::state& state, dcon::nation_id target, dcon::nation_id from) {
	if(!state.world.nation_get_ai_is_threatened(target))
		return false;

	if(state.world.nation_get_ai_rival(target) == from || state.world.nation_get_ai_rival(from) == target)
		return false;

	auto target_continent = state.world.province_get_continent(state.world.nation_get_capital(target));
	auto source_continent = state.world.province_get_continent(state.world.nation_get_capital(from));

	bool close_enough = (target_continent == source_continent) || bool(state.world.get_nation_adjacency_by_nation_adjacency_pair(target, from));

	if(!close_enough)
		return false;

	auto target_score = state.world.nation_get_military_score(target);
	auto source_score = state.world.nation_get_military_score(from);

	return source_score * 2 >= target_score;
}

void explain_ai_alliance_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent) {

	text::add_line_with_condition(state, contents, "ai_alliance_1", state.world.nation_get_ai_is_threatened(target), indent);


	auto target_continent = state.world.province_get_continent(state.world.nation_get_capital(target));
	auto source_continent = state.world.province_get_continent(state.world.nation_get_capital(state.local_player_nation));

	bool close_enough = (target_continent == source_continent) || bool(state.world.get_nation_adjacency_by_nation_adjacency_pair(target, state.local_player_nation));

	text::add_line_with_condition(state, contents, "ai_alliance_2", close_enough, indent);

	text::add_line_with_condition(state, contents, "ai_alliance_3", state.world.nation_get_ai_rival(target) != state.local_player_nation && state.world.nation_get_ai_rival(state.local_player_nation) != target, indent);

	auto target_score = state.world.nation_get_military_score(target);
	auto source_score = state.world.nation_get_military_score(state.local_player_nation);

	text::add_line_with_condition(state, contents, "ai_alliance_4", source_score * 2 >= target_score, indent);
}

bool ai_will_grant_access(sys::state& state, dcon::nation_id target, dcon::nation_id from) {
	if(!state.world.nation_get_is_at_war(from))
		return false;
	if(state.world.nation_get_ai_rival(target) == from)
		return false;
	if(military::are_at_war(state, from, state.world.nation_get_ai_rival(target)))
		return true;

	for(auto wa : state.world.nation_get_war_participant(target)) {
		auto is_attacker = wa.get_is_attacker();
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_is_attacker() != is_attacker) {
				if(military::are_at_war(state, o.get_nation(), from))
					return true;
			}
		}
	}
	return false;

}
void explain_ai_access_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent) {
	text::add_line_with_condition(state, contents, "ai_access_1", ai_will_grant_access(state, target, state.local_player_nation), indent);
}

void update_ai_research(sys::state& state) {
	auto ymd_date = state.current_date.to_ymd(state.start_date);
	auto year = uint32_t(ymd_date.year);
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t id) {
		dcon::nation_id n{dcon::nation_id::value_base_t(id)};

		if(state.world.nation_get_is_player_controlled(n)
			|| state.world.nation_get_current_research(n)
			|| !state.world.nation_get_is_civilized(n)
			|| state.world.nation_get_owned_province_count(n) == 0) {

			//skip -- does not need new research
			return;
		}

		struct potential_techs {
			dcon::technology_id id;
			float weight = 0.0f;
		};

		std::vector<potential_techs> potential;

		for(auto tid : state.world.in_technology) {
			if(state.world.nation_get_active_technologies(n, tid))
				continue; // Already researched
		
			if(state.current_date.to_ymd(state.start_date).year >= state.world.technology_get_year(tid)) {
				// Find previous technology before this one
				dcon::technology_id prev_tech = dcon::technology_id(dcon::technology_id::value_base_t(tid.id.index() - 1));
				// Previous technology is from the same folder so we have to check that we have researched it beforehand
				if(tid.id.index() != 0 && state.world.technology_get_folder_index(prev_tech) == state.world.technology_get_folder_index(tid)) {
					// Only allow if all previously researched techs are researched
					if(state.world.nation_get_active_technologies(n, prev_tech))
						potential.push_back(potential_techs{tid, 0.0f});
				} else { // first tech in folder
					potential.push_back(potential_techs{ tid, 0.0f });
				}
			}
		}

		for(auto& pt : potential) { // weight techs
			auto base = state.world.technology_get_ai_weight(pt.id);
			if(state.world.nation_get_ai_is_threatened(n) && state.culture_definitions.tech_folders[state.world.technology_get_folder_index(pt.id)].category == culture::tech_category::army) {
				base *= 2.0f;
			}
			auto cost = std::max(1.0f, culture::effective_technology_cost(state, year, n, pt.id));
			pt.weight = base / cost;
		}
		auto rval = rng::get_random(state, id);
		std::sort(potential.begin(), potential.end(), [&](potential_techs& a, potential_techs& b) {
			if(a.weight != b.weight)
				return a.weight > b.weight;
			else // sort semi randomly
				return (a.id.index() ^ rval) > (b.id.index() ^ rval);
		});

		if(!potential.empty()) {
			state.world.nation_set_current_research(n, potential[0].id);
		}
	});
}

void initialize_ai_tech_weights(sys::state& state) {
	for(auto t : state.world.in_technology) {
		float base = 1000.0f;
		if(state.culture_definitions.tech_folders[t.get_folder_index()].category == culture::tech_category::army)
			base *= 1.5f;

		if(t.get_increase_naval_base())
			base *= 1.1f;
		else if(state.culture_definitions.tech_folders[t.get_folder_index()].category == culture::tech_category::navy)
			base *= 0.9f;

		auto mod = t.get_modifier();
		auto& vals = mod.get_national_values();
		for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
			if(vals.offsets[i] == sys::national_mod_offsets::research_points) {
				base *= 3.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::research_points_modifier) {
				base *= 3.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::education_efficiency) {
				base *= 2.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::education_efficiency_modifier) {
				base *= 2.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::pop_growth) {
				base *= 1.6f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::max_national_focus) {
				base *= 1.7f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::colonial_life_rating) {
				base *= 1.6f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::rgo_output) {
				base *= 1.2f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::factory_output) {
				base *= 1.2f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::factory_throughput) {
				base *= 1.2f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::factory_input) {
				base *= 1.2f;
			}
		}

		t.set_ai_weight(base);
	}
}

void update_influence_priorities(sys::state& state) {
	struct weighted_nation {
		dcon::nation_id id;
		float weight = 0.0f;
	};
	static std::vector<weighted_nation> targets;

	for(auto gprl : state.world.in_gp_relationship) {
		if(gprl.get_great_power().get_is_player_controlled()) {
			// nothing -- player GP
		} else {
			auto& status = gprl.get_status();
			status &= ~nations::influence::priority_mask;
			if((status & nations::influence::level_mask) == nations::influence::level_in_sphere) {
				status |= nations::influence::priority_one;
			}
		}
	}

	for(auto&n : state.great_nations) {
		if(state.world.nation_get_is_player_controlled(n.nation))
			continue;

		targets.clear();
		for(auto t : state.world.in_nation) {
			if(t.get_is_great_power())
				continue;
			if(t.get_owned_province_count() == 0)
				continue;
			if(t.get_in_sphere_of() == n.nation)
				continue;
			if(t.get_demographics(demographics::total) > state.defines.large_population_limit)
				continue;

			float weight = 0.0f;

			for(auto c : state.world.in_commodity) {
				if(auto d = state.world.nation_get_real_demand(n.nation, c); d > 0.001f) {
					auto cweight = std::min(1.0f, t.get_domestic_market_pool(c) * (1.0f - state.world.nation_get_demand_satisfaction(n.nation, c)) / d);
					weight += cweight;
				}
			}

			if(t.get_primary_culture().get_group_from_culture_group_membership() == state.world.nation_get_primary_culture(n.nation).get_group_from_culture_group_membership()) {
				weight += 4.0f;
			}

			if(state.world.get_nation_adjacency_by_nation_adjacency_pair(n.nation, t.id)) {
				weight *= 3.0f;
			}

			targets.push_back(weighted_nation{t.id, weight});
		}

		std::sort(targets.begin(), targets.end(), [](weighted_nation const& a, weighted_nation const& b) {
			if(a.weight != b.weight)
				return a.weight > b.weight;
			else
				return a.id.index() < b.id.index();
		});

		uint32_t i = 0;
		for(; i < 2 && i < targets.size(); ++i) {
			auto rel = state.world.get_gp_relationship_by_gp_influence_pair(targets[i].id, n.nation);
			if(!rel)
				rel = state.world.force_create_gp_relationship(targets[i].id, n.nation);
			state.world.gp_relationship_get_status(rel) |= nations::influence::priority_three;
		}
		for(; i < 4 && i < targets.size(); ++i) {
			auto rel = state.world.get_gp_relationship_by_gp_influence_pair(targets[i].id, n.nation);
			if(!rel)
				rel = state.world.force_create_gp_relationship(targets[i].id, n.nation);
			state.world.gp_relationship_get_status(rel) |= nations::influence::priority_two;
		}
		for(; i < 6 && i < targets.size(); ++i) {
			auto rel = state.world.get_gp_relationship_by_gp_influence_pair(targets[i].id, n.nation);
			if(!rel)
				rel = state.world.force_create_gp_relationship(targets[i].id, n.nation);
			state.world.gp_relationship_get_status(rel) |= nations::influence::priority_one;
		}
	}
}

void perform_influence_actions(sys::state& state) {
	for(auto gprl : state.world.in_gp_relationship) {
		if(gprl.get_great_power().get_is_player_controlled()) {
			// nothing -- player GP
		} else {
			if((gprl.get_status() & nations::influence::is_banned) != 0)
				continue; // can't do anything with a banned nation

			if(military::are_at_war(state, gprl.get_great_power(), gprl.get_influence_target()))
				continue; // can't do anything while at war

			auto clevel = (nations::influence::level_mask & gprl.get_status());
			if(clevel == nations::influence::level_in_sphere)
				continue; // already in sphere

			auto current_sphere = gprl.get_influence_target().get_in_sphere_of();

			if(state.defines.increaseopinion_influence_cost <= gprl.get_influence() && clevel != nations::influence::level_friendly) {

				gprl.get_influence() -= state.defines.increaseopinion_influence_cost;
				auto& l = gprl.get_status();
				l = nations::influence::increase_level(l);

				notification::post(state, notification::message{
					[source = gprl.get_great_power().id, influence_target = gprl.get_influence_target().id](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "msg_op_inc_1", text::variable_type::x, source, text::variable_type::y, influence_target);
					},
					"msg_op_inc_title",
					gprl.get_great_power().id,
					sys::message_setting_type::increase_opinion
				});
			} else if(state.defines.removefromsphere_influence_cost <= gprl.get_influence() && current_sphere /* && current_sphere != gprl.get_great_power()*/ && clevel == nations::influence::level_friendly) { // condition taken care of by check above

				gprl.get_influence() -= state.defines.removefromsphere_influence_cost;

				gprl.get_influence_target().set_in_sphere_of(dcon::nation_id{});

				auto orel = state.world.get_gp_relationship_by_gp_influence_pair(gprl.get_influence_target(), current_sphere);
				auto& l = state.world.gp_relationship_get_status(orel);
				l = nations::influence::decrease_level(l);

				nations::adjust_relationship(state, gprl.get_great_power(), current_sphere, state.defines.removefromsphere_relation_on_accept);
	
				notification::post(state, notification::message{
					[source = gprl.get_great_power().id, influence_target = gprl.get_influence_target().id, affected_gp = current_sphere.id](sys::state& state, text::layout_base& contents) {
						if(source == affected_gp)
							text::add_line(state, contents, "msg_rem_sphere_1", text::variable_type::x, source, text::variable_type::y, influence_target);
						else
							text::add_line(state, contents, "msg_rem_sphere_1", text::variable_type::x, source, text::variable_type::y, influence_target, text::variable_type::val, affected_gp);
					},
					"msg_rem_sphere_title",
					gprl.get_great_power(),
					sys::message_setting_type::rem_sphere_by_nation
				});
				notification::post(state, notification::message{
					[source = gprl.get_great_power().id, influence_target = gprl.get_influence_target().id, affected_gp = current_sphere.id](sys::state& state, text::layout_base& contents) {
						if(source == affected_gp)
							text::add_line(state, contents, "msg_rem_sphere_1", text::variable_type::x, source, text::variable_type::y, influence_target);
						else
							text::add_line(state, contents, "msg_rem_sphere_1", text::variable_type::x, source, text::variable_type::y, influence_target, text::variable_type::val, affected_gp);
					},
					"msg_rem_sphere_title",
					current_sphere,
					sys::message_setting_type::rem_sphere_on_nation
				});
				notification::post(state, notification::message{
					[source = gprl.get_great_power().id, influence_target = gprl.get_influence_target().id, affected_gp = current_sphere.id](sys::state& state, text::layout_base& contents) {
						if(source == affected_gp)
							text::add_line(state, contents, "msg_rem_sphere_1", text::variable_type::x, source, text::variable_type::y, influence_target);
						else
							text::add_line(state, contents, "msg_rem_sphere_1", text::variable_type::x, source, text::variable_type::y, influence_target, text::variable_type::val, affected_gp);
					},
					"msg_rem_sphere_title",
					gprl.get_influence_target(),
					sys::message_setting_type::removed_from_sphere
				});
			} else if(state.defines.addtosphere_influence_cost <= gprl.get_influence() && !current_sphere && clevel == nations::influence::level_friendly) {

				gprl.get_influence() -= state.defines.addtosphere_influence_cost;
				auto& l = gprl.get_status();
				l = nations::influence::increase_level(l);

				gprl.get_influence_target().set_in_sphere_of(gprl.get_great_power());

				notification::post(state, notification::message{
					[source = gprl.get_great_power().id, influence_target = gprl.get_influence_target().id](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "msg_add_sphere_1", text::variable_type::x, source, text::variable_type::y, influence_target);
					},
					"msg_add_sphere_title",
					gprl.get_great_power(),
					sys::message_setting_type::add_sphere
				});
				notification::post(state, notification::message{
					[source = gprl.get_great_power().id, influence_target = gprl.get_influence_target().id](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "msg_add_sphere_1", text::variable_type::x, source, text::variable_type::y, influence_target);
					},
					"msg_add_sphere_title",
					gprl.get_influence_target(),
					sys::message_setting_type::added_to_sphere
				});
			}
		}
	}
}

}
