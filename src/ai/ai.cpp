#include "ai.hpp"
#include "system_state.hpp"
#include "math_fns.hpp"

namespace ai {

float estimate_strength(sys::state& state, dcon::nation_id n) {
	float value = state.world.nation_get_military_score(n);
	for(auto subj : state.world.nation_get_overlord_as_ruler(n))
		value += subj.get_subject().get_military_score();
	return value;
}

float estimate_defensive_strength(sys::state& state, dcon::nation_id n) {
	float value = estimate_strength(state, n);
	for(auto dr : state.world.nation_get_diplomatic_relation(n)) {
		if(!dr.get_are_allied())
			continue;

		auto other = dr.get_related_nations(0) != n ? dr.get_related_nations(0) : dr.get_related_nations(1);
		if(other.get_overlord_as_subject().get_ruler() != n)
			value += estimate_strength(state, other);
	}
	if(auto sl = state.world.nation_get_in_sphere_of(n); sl)
		value += estimate_strength(state, sl);
	return value;
}

float estimate_additional_offensive_strength(sys::state& state, dcon::nation_id n, dcon::nation_id target) {
	float value = 0.f;
	for(auto dr : state.world.nation_get_diplomatic_relation(n)) {
		if(!dr.get_are_allied())
			continue;

		auto other = dr.get_related_nations(0) != n ? dr.get_related_nations(0) : dr.get_related_nations(1);
		if(other.get_overlord_as_subject().get_ruler() != n && military::can_use_cb_against(state, other, target) && !military::has_truce_with(state, other, target))
			value += estimate_strength(state, other);
	}
	return value;
}

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
				greatest_neighbor = std::max(greatest_neighbor, estimate_strength(state, other));
			}
		}

		float self_str = float(state.world.nation_get_military_score(n));
		for(auto subj : n.get_overlord_as_ruler())
			self_str += 0.75f * float(subj.get_subject().get_military_score());
		float defensive_str = estimate_defensive_strength(state, n);

		bool threatened = defensive_str < safety_factor * greatest_neighbor;
		state.world.nation_set_ai_is_threatened(n, threatened);

		if(!n.get_ai_rival()) {
			float min_relation = 200.0f;
			dcon::nation_id potential;
			for(auto adj : n.get_nation_adjacency()) {
				auto other = adj.get_connected_nations(0) != n ? adj.get_connected_nations(0) : adj.get_connected_nations(1);
				auto ol = other.get_overlord_as_subject().get_ruler();
				if(!ol && other.get_in_sphere_of() != n && (!threatened || !nations::are_allied(state, n, other))) {
					auto other_str = estimate_strength(state, other);
					if(self_str * 0.5f < other_str && other_str <= self_str * 1.5f) {
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
					command::execute_cancel_alliance(state, n, potential);
				}
				n.set_ai_rival(potential);
			}
		} else {
			auto rival_str = estimate_strength(state, n.get_ai_rival());
			auto ol = n.get_ai_rival().get_overlord_as_subject().get_ruler();
			if(ol || n.get_ai_rival().get_in_sphere_of() == n || rival_str * 2 < self_str || self_str * 2 < rival_str) {
				n.set_ai_rival(dcon::nation_id{});
			}
		}
	}
}

static void internal_get_alliance_targets_by_adjacency(sys::state& state, dcon::nation_id n, dcon::nation_id adj, std::vector<dcon::nation_id>& alliance_targets) {
	for(auto nb : state.world.nation_get_nation_adjacency(adj)) {
		auto other = nb.get_connected_nations(0) != adj ? nb.get_connected_nations(0) : nb.get_connected_nations(1);
		if(other != n && other.get_is_player_controlled() == false && !(other.get_overlord_as_subject().get_ruler()) && !nations::are_allied(state, n, other) && !military::are_at_war(state, other, n) && ai_will_accept_alliance(state, other, n))
			alliance_targets.push_back(other.id);
	}
}
static void internal_get_alliance_targets(sys::state& state, dcon::nation_id n, std::vector<dcon::nation_id>& alliance_targets) {
	// Adjacency with us
	internal_get_alliance_targets_by_adjacency(state, n, n, alliance_targets);
	if(!alliance_targets.empty())
		return;

	// Adjacency with rival (useful for e.x, Chile allying Paraguay to fight bolivia)
	if(auto rival = state.world.nation_get_ai_rival(n); bool(rival)) {
		internal_get_alliance_targets_by_adjacency(state, n, rival, alliance_targets);
	if(!alliance_targets.empty())
		return;
	}
	
	// Adjacency with people who are at war with us
	for(auto wp : state.world.nation_get_war_participant(n)) {
		for(auto p : state.world.war_get_war_participant(wp.get_war())) {
			if(p.get_is_attacker() == !wp.get_is_attacker()) {
				internal_get_alliance_targets_by_adjacency(state, n, p.get_nation(), alliance_targets);
				if(!alliance_targets.empty())
					return;
			}
		}
	}
}

void form_alliances(sys::state& state) {
	static std::vector<dcon::nation_id> alliance_targets;
	for(auto n : state.world.in_nation) {
		if(!n.get_is_player_controlled() && n.get_ai_is_threatened() && !(n.get_overlord_as_subject().get_ruler())) {
			alliance_targets.clear();
			internal_get_alliance_targets(state, n, alliance_targets);
			if(!alliance_targets.empty()) {
				std::sort(alliance_targets.begin(), alliance_targets.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					if(estimate_strength(state, a) != estimate_strength(state, b))
						return estimate_strength(state, a) > estimate_strength(state, b);
					else
						return a.index() > b.index();
				});
				nations::make_alliance(state, n, alliance_targets[0]);

				// Call our new allies into wars.... they may not accept but they may just may join!
				//for(auto wp : state.world.nation_get_war_participant(n))
				//	if(!military::are_allied_in_war(state, n, alliance_targets[0]) && will_join_war(state, alliance_targets[0], wp.get_war(), wp.get_is_attacker()))
				//		command::execute_call_to_arms(state, n, alliance_targets[0], wp.get_war());
			}
		}
	}
}

bool ai_is_close_enough(sys::state& state, dcon::nation_id target, dcon::nation_id from) {
	auto target_continent = state.world.province_get_continent(state.world.nation_get_capital(target));
	auto source_continent = state.world.province_get_continent(state.world.nation_get_capital(from));
	return (target_continent == source_continent) || bool(state.world.get_nation_adjacency_by_nation_adjacency_pair(target, from));
}

static bool ai_has_mutual_enemy(sys::state& state, dcon::nation_id from, dcon::nation_id target) {
	auto rival_a = state.world.nation_get_ai_rival(target);
	auto rival_b = state.world.nation_get_ai_rival(from);
	// Same rival equates to instantaneous alliance (we benefit from more allies against a common enemy)
	if(rival_a && rival_a == rival_b)
		return true;
	// // Our rivals are allied?
	// if(rival_a && rival_b && rival_a != rival_b && nations::are_allied(state, rival_a, rival_b))
	// 	return true;

	// // One of the allies of our rivals can be declared on?
	// for(auto n : state.world.in_nation) {
	// 	if(n.id != target && n.id != from && n.id != rival_a && n.id != rival_b) {
	// 		if(nations::are_allied(state, rival_a, n.id) || nations::are_allied(state, rival_b, n.id)) {
	// 			bool enemy_a = military::can_use_cb_against(state, from, n.id);
	// 			bool enemy_b = military::can_use_cb_against(state, target, n.id);
	// 			if(enemy_a || enemy_b)
	// 				return true;
	// 		}
	// 	}
	// }
	return false;
}

constexpr inline float ally_overestimate = 2.f;

bool ai_will_accept_alliance(sys::state& state, dcon::nation_id target, dcon::nation_id from) {
	if(!state.world.nation_get_ai_is_threatened(target))
		return false;
	
	// Won't ally our rivals
	if(state.world.nation_get_ai_rival(target) == from || state.world.nation_get_ai_rival(from) == target)
		return false;

	if(ai_has_mutual_enemy(state, from, target))
		return true;
	
	// Otherwise we may consider alliances only iff they are close to our continent or we are adjacent
	if(!ai_is_close_enough(state, target, from))
		return false;
	
	// And also if they're powerful enough to be considered for an alliance
	auto target_score = estimate_strength(state, target);
	auto source_score = estimate_strength(state, from);
	return std::max<float>(source_score, 1.f) * ally_overestimate >= target_score;
}

void explain_ai_alliance_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent) {

	text::add_line_with_condition(state, contents, "ai_alliance_1", state.world.nation_get_ai_is_threatened(target), indent);

	text::add_line(state, contents, "kierkegaard_1", indent);

	text::add_line_with_condition(state, contents, "ai_alliance_5", ai_has_mutual_enemy(state, state.local_player_nation, target), indent + 15);

	text::add_line(state, contents, "kierkegaard_2", indent);

	text::add_line_with_condition(state, contents, "ai_alliance_2", ai_is_close_enough(state, target, state.local_player_nation), indent + 15);

	text::add_line_with_condition(state, contents, "ai_alliance_3", state.world.nation_get_ai_rival(target) != state.local_player_nation && state.world.nation_get_ai_rival(state.local_player_nation) != target, indent + 15);

	auto target_score = estimate_strength(state, target);
	auto source_score = estimate_strength(state, state.local_player_nation);
	text::add_line_with_condition(state, contents, "ai_alliance_4", std::max<float>(source_score, 1.f) * ally_overestimate >= target_score, indent + 15);
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

		if(t.get_increase_building(economy::province_building_type::naval_base))
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
					auto cweight = std::min(1.0f, t.get_domestic_market_pool(c) / d) * (1.0f - state.world.nation_get_demand_satisfaction(n.nation, c));
					weight += cweight;
				}
			}

			if(t.get_primary_culture().get_group_from_culture_group_membership() == state.world.nation_get_primary_culture(n.nation).get_group_from_culture_group_membership()) {
				weight += 4.0f;
			} else if(t.get_in_sphere_of()) {
				weight /= 3.0f;
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
							text::add_line(state, contents, "msg_rem_sphere_2", text::variable_type::x, source, text::variable_type::y, influence_target, text::variable_type::val, affected_gp);
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
							text::add_line(state, contents, "msg_rem_sphere_2", text::variable_type::x, source, text::variable_type::y, influence_target, text::variable_type::val, affected_gp);
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
							text::add_line(state, contents, "msg_rem_sphere_2", text::variable_type::x, source, text::variable_type::y, influence_target, text::variable_type::val, affected_gp);
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

void identify_focuses(sys::state& state) {
	for(auto f : state.world.in_national_focus) {
		if(f.get_promotion_amount() > 0) {
			if(f.get_promotion_type() == state.culture_definitions.clergy)
				state.national_definitions.clergy_focus = f;
			if(f.get_promotion_type() == state.culture_definitions.soldiers)
				state.national_definitions.soldier_focus = f;
		}
	}
}

void update_focuses(sys::state& state) {
	for(auto si : state.world.in_state_instance) {
		if(!si.get_nation_from_state_ownership().get_is_player_controlled())
			si.set_owner_focus(dcon::national_focus_id{});
	}

	for(auto n : state.world.in_nation) {
		if(n.get_is_player_controlled())
			continue;
		if(n.get_owned_province_count() == 0)
			continue;

		n.set_state_from_flashpoint_focus(dcon::state_instance_id{});

		auto num_focuses_total = nations::max_national_focuses(state, n);
		if(num_focuses_total <= 0)
			return;

		auto base_opt = state.world.pop_type_get_research_optimum(state.culture_definitions.clergy);
		auto clergy_frac = n.get_demographics(demographics::to_key(state, state.culture_definitions.clergy)) / n.get_demographics(demographics::total);
		bool max_clergy = clergy_frac >= base_opt;

		static std::vector<dcon::state_instance_id> ordered_states;
		ordered_states.clear();
		for(auto si : n.get_state_ownership()) {
			ordered_states.push_back(si.get_state().id);
		}
		std::sort(ordered_states.begin(), ordered_states.end(), [&](auto a, auto b) {
			auto apop = state.world.state_instance_get_demographics(a, demographics::total);
			auto bpop = state.world.state_instance_get_demographics(b, demographics::total);
			if(apop != bpop)
				return apop > bpop;
			else
				return a.index() < b.index();
		});
		bool threatened = n.get_ai_is_threatened() || n.get_is_at_war();
		for(uint32_t i = 0; num_focuses_total > 0 && i < ordered_states.size(); ++i) {
			if(max_clergy) {
				if(threatened) {
					state.world.state_instance_set_owner_focus(ordered_states[i], state.national_definitions.soldier_focus);
					--num_focuses_total;
				} else {
					auto cfrac = state.world.state_instance_get_demographics(ordered_states[i], demographics::to_key(state, state.culture_definitions.clergy)) / state.world.state_instance_get_demographics(ordered_states[i], demographics::total);
					if(cfrac < state.defines.max_clergy_for_literacy * 0.8f) {
						state.world.state_instance_set_owner_focus(ordered_states[i], state.national_definitions.clergy_focus);
						--num_focuses_total;
					}
				}
			} else {
				auto cfrac = state.world.state_instance_get_demographics(ordered_states[i], demographics::to_key(state, state.culture_definitions.clergy)) / state.world.state_instance_get_demographics(ordered_states[i], demographics::total);
				if(cfrac < base_opt * 1.2f) {
					state.world.state_instance_set_owner_focus(ordered_states[i], state.national_definitions.clergy_focus);
					--num_focuses_total;
				}
			}
		}

	}
}

void take_ai_decisions(sys::state& state) {
	for(auto d : state.world.in_decision) {
		auto e = d.get_effect();
		if(!e)
			continue;

		auto potential = d.get_potential();
		auto allow = d.get_allow();
		auto ai_will_do = d.get_ai_will_do();

		ve::execute_serial_fast<dcon::nation_id>(state.world.nation_size(), [&](auto ids) {
			ve::vbitfield_type filter_a = potential
				? (ve::compress_mask(trigger::evaluate(state, potential, trigger::to_generic(ids), trigger::to_generic(ids), 0)) & !state.world.nation_get_is_player_controlled(ids)) 
				: !state.world.nation_get_is_player_controlled(ids);

			if(filter_a.v != 0) {
				ve::mask_vector filter_c = allow
					? (trigger::evaluate(state, allow, trigger::to_generic(ids), trigger::to_generic(ids), 0) && (state.world.nation_get_owned_province_count(ids) != 0) )&& filter_a
					: ve::mask_vector{ filter_a } && (state.world.nation_get_owned_province_count(ids) != 0);
				ve::mask_vector filter_b = ai_will_do
					? filter_c && (trigger::evaluate_multiplicative_modifier(state, ai_will_do, trigger::to_generic(ids), trigger::to_generic(ids), 0) > 0.0f)
					: filter_c;

				ve::apply([&](dcon::nation_id n, bool passed_filter) {
					if(passed_filter) {
						effect::execute(state, e, trigger::to_generic(n), trigger::to_generic(n), 0, uint32_t(state.current_date.value),
									uint32_t(n.index() << 4 ^ d.id.index()));

						notification::post(state, notification::message{
							[e, n, did = d.id, when = state.current_date](sys::state& state, text::layout_base& contents) {
								text::add_line(state, contents, "msg_decision_1", text::variable_type::x, n, text::variable_type::y, state.world.decision_get_name(did));
								text::add_line(state, contents, "msg_decision_2");
								ui::effect_description(state, contents, e, trigger::to_generic(n), trigger::to_generic(n), 0, uint32_t(when.value), uint32_t(n.index() << 4 ^ did.index()));
							},
							"msg_decision_title",
							n,
							sys::message_setting_type::decision
						});
					}
				}, ids, filter_b);
			}
		});
	}
}

float estimate_pop_party_support(sys::state& state, dcon::nation_id n, dcon::political_party_id pid) {
	auto iid = state.world.political_party_get_ideology(pid);
	/*float v = 0.f;
	for(const auto poid : state.world.nation_get_province_ownership_as_nation(n)) {
		for(auto plid : state.world.province_get_pop_location_as_province(poid.get_province())) {
			float weigth = plid.get_pop().get_size() * 0.001f;
			v += state.world.pop_get_demographics(plid.get_pop(), pop_demographics::to_key(state, iid)) * weigth;
		}
	}*/
	return state.world.nation_get_demographics(n, demographics::to_key(state, iid));
}

bool ai_can_appoint_political_party(sys::state& state, dcon::nation_id n) {
	if(!politics::can_appoint_ruling_party(state, n))
		return false;
	auto last_change = state.world.nation_get_ruling_party_last_appointed(n);
	if(last_change && state.current_date < last_change + 365)
		return false;
	if(politics::is_election_ongoing(state, n))
		return false;
	// Do not appoint if we are a democracy!
	if(politics::has_elections(state, n))
		return false;
	return true;
}

void update_ai_ruling_party(sys::state& state) {
	for(auto n : state.world.in_nation) {
		// skip over: non ais, dead nations
		if(n.get_is_player_controlled() || n.get_owned_province_count() == 0)
			continue;

		if(ai_can_appoint_political_party(state, n)) {
			auto gov = n.get_government_type();
			auto identity = n.get_identity_from_identity_holder();
			auto start = state.world.national_identity_get_political_party_first(identity).id.index();
			auto end = start + state.world.national_identity_get_political_party_count(identity);

			dcon::political_party_id target;
			float max_support = estimate_pop_party_support(state, n, state.world.nation_get_ruling_party(n));
			for(int32_t i = start; i < end; i++) {
				auto pid = dcon::political_party_id(uint16_t(i));
				if(pid != state.world.nation_get_ruling_party(n) && politics::political_party_is_active(state, pid) && (state.culture_definitions.governments[gov].ideologies_allowed & ::culture::to_bits(state.world.political_party_get_ideology(pid))) != 0) {
					auto support = estimate_pop_party_support(state, n, pid);
					if(support > max_support) {
						target = pid;
						max_support = support;
					}
				}
			}

			assert(target != state.world.nation_get_ruling_party(n));
			if(target) {
				politics::appoint_ruling_party(state, n, target);
			}
		}
	}
}

void update_ai_econ_construction(sys::state& state) {
	for(auto n : state.world.in_nation) {
		// skip over: non ais, dead nations, and nations that aren't making money
		if(n.get_is_player_controlled() || n.get_owned_province_count() == 0)
			continue;
		if(n.get_spending_level() < 1.0f || n.get_last_treasury() >= n.get_stockpiles(economy::money))
			continue;

		// buy stuff from the global market if we need it
		//state.world.for_each_commodity([&](dcon::commodity_id c) {
		//	n.set_stockpile_targets(c, 10000.f);
		//	if(n.get_demand_satisfaction(c) < 1.0f) {
		//		n.set_drawing_on_stockpiles(c, true);
		//	} else {
		//		n.set_drawing_on_stockpiles(c, false);
		//	}
		//});

		auto treasury = n.get_stockpiles(economy::money);
		int32_t max_projects = std::max(8, int32_t(treasury / 8000.0f));
		auto rules = n.get_combined_issue_rules();

		if((rules & issue_rule::expand_factory) != 0 || (rules & issue_rule::build_factory) != 0) {
			static::std::vector<dcon::factory_type_id> desired_types;
			desired_types.clear();

			// first pass: try to fill shortages
			for(auto type : state.world.in_factory_type) {
				if(n.get_active_building(type) || type.get_is_available_from_start()) {
					bool lacking_output = n.get_demand_satisfaction(type.get_output()) < 1.0f;

					if(lacking_output) {
						auto& inputs = type.get_inputs();
						bool lacking_input = false;

						for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
							if(inputs.commodity_type[i]) {
								if(n.get_demand_satisfaction(inputs.commodity_type[i]) < 1.0f)
									lacking_input = true;
							} else {
								break;
							}
						}

						if(!lacking_input)
							desired_types.push_back(type.id);
					}
				} // END if building unlocked
			}

			if(desired_types.empty()) { // second pass: try to make money
				for(auto type : state.world.in_factory_type) {
					if(n.get_active_building(type) || type.get_is_available_from_start()) {
						auto& inputs = type.get_inputs();
						bool lacking_input = false;

						for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
							if(inputs.commodity_type[i]) {
								if(n.get_demand_satisfaction(inputs.commodity_type[i]) < 1.0f)
									lacking_input = true;
							} else {
								break;
							}
						}

						if(!lacking_input)
							desired_types.push_back(type.id);
					} // END if building unlocked
				}
			}

			// desired types filled: try to construct or upgrade
			if(!desired_types.empty()) {
				static std::vector<dcon::state_instance_id> ordered_states;
				ordered_states.clear();
				for(auto si : n.get_state_ownership()) {
					if(si.get_state().get_capital().get_is_colonial() == false)
						ordered_states.push_back(si.get_state().id);
				}
				std::sort(ordered_states.begin(), ordered_states.end(), [&](auto a, auto b) {
					auto apop = state.world.state_instance_get_demographics(a, demographics::total);
					auto bpop = state.world.state_instance_get_demographics(b, demographics::total);
					if(apop != bpop)
						return apop > bpop;
					else
						return a.index() < b.index();
				});

				if((rules & issue_rule::build_factory) == 0) { // can't build -- by elimination, can upgrade
					for(auto si : ordered_states) {
						if(max_projects <= 0)
							break;

						auto pw_num = state.world.state_instance_get_demographics(si,
								demographics::to_key(state, state.culture_definitions.primary_factory_worker));
						auto pw_employed = state.world.state_instance_get_demographics(si,
								demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));

						if(pw_employed >= pw_num && pw_num > 0.0f)
							continue; // no spare workers

						province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
							for(auto fac : state.world.province_get_factory_location(p)) {
								auto type = fac.get_factory().get_building_type();
								if(fac.get_factory().get_unprofitable() == false
									&& fac.get_factory().get_level() < uint8_t(255)
									&& std::find(desired_types.begin(), desired_types.end(), type) != desired_types.end()) {

									auto ug_in_progress = false;
									for(auto c : state.world.state_instance_get_state_building_construction(si)) {
										if(c.get_type() == type) {
											ug_in_progress = true;
											break;
										}
									}
									if(!ug_in_progress) {
										auto new_up = fatten(state.world, state.world.force_create_state_building_construction(si, n));
										new_up.set_is_pop_project(false);
										new_up.set_is_upgrade(true);
										new_up.set_type(type);

										--max_projects;
										return;
									}
								}
							}
						});
					} // END for(auto si : ordered_states) {
				} else { // if if((rules & issue_rule::build_factory) == 0) -- i.e. if building is possible
					for(auto si : ordered_states) {
						if(max_projects <= 0)
							break;

						// check -- either unemployed factory workers or no factory workers
						auto pw_num = state.world.state_instance_get_demographics(si,
								demographics::to_key(state, state.culture_definitions.primary_factory_worker));
						pw_num += state.world.state_instance_get_demographics(si,
								demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
						auto pw_employed = state.world.state_instance_get_demographics(si,
								demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
						pw_employed += state.world.state_instance_get_demographics(si,
								demographics::to_employment_key(state, state.culture_definitions.secondary_factory_worker));

						if(pw_employed >= float(pw_num) * 2.5f && pw_num > 0.0f)
							continue; // no spare workers

						auto type_selection = desired_types[rng::get_random(state, uint32_t(n.id.index() + max_projects)) % desired_types.size()];
						assert(type_selection);

						if(state.world.factory_type_get_is_coastal(type_selection) && !province::state_is_coastal(state, si))
							continue;

						bool already_in_progress = [&]() {
							for(auto p : state.world.state_instance_get_state_building_construction(si)) {
								if(p.get_type() == type_selection)
									return true;
							}
							return false;
						}();
						if(already_in_progress)
							continue;

						if((rules & issue_rule::expand_factory) != 0) { // check: if present, try to upgrade
							bool present_in_location = false;
							province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
								for(auto fac : state.world.province_get_factory_location(p)) {
									auto type = fac.get_factory().get_building_type();
									if(type_selection == type) {
										present_in_location = true;
										return;
									}
								}
							});
							if(present_in_location) {
								auto new_up = fatten(state.world, state.world.force_create_state_building_construction(si, n));
								new_up.set_is_pop_project(false);
								new_up.set_is_upgrade(true);
								new_up.set_type(type_selection);

								--max_projects;
								continue;
							}
						}

						// else -- try to build -- must have room
						int32_t num_factories = 0;

						auto d = state.world.state_instance_get_definition(si);
						for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
							if(p.get_province().get_nation_from_province_ownership() == n) {
								for(auto f : p.get_province().get_factory_location()) {
									++num_factories;
								}
							}
						}
						for(auto p : state.world.state_instance_get_state_building_construction(si)) {
							if(p.get_is_upgrade() == false)
								++num_factories;
						}
						if(num_factories < int32_t(state.defines.factories_per_state)) {
							auto new_up = fatten(state.world, state.world.force_create_state_building_construction(si, n));
							new_up.set_is_pop_project(false);
							new_up.set_is_upgrade(false);
							new_up.set_type(type_selection);

							--max_projects;
							continue;
						} else {
							// TODO: try to delete a factory here
						}

					} // END for(auto si : ordered_states) {
				} // END if((rules & issue_rule::build_factory) == 0) 
			} // END if(!desired_types.empty()) {
		} // END  if((rules & issue_rule::expand_factory) != 0 || (rules & issue_rule::build_factory) != 0)

		static std::vector<dcon::province_id> project_provs;

		// try naval bases
		if(max_projects > 0) {
			project_provs.clear();
			for(auto o : n.get_province_ownership()) {
				if(!o.get_province().get_is_coast())
					continue;
				if(n != o.get_province().get_nation_from_province_control())
					continue;
				if(military::province_is_under_siege(state, o.get_province()))
					continue;
				if(o.get_province().get_building_level(economy::province_building_type::naval_base) == 0 && o.get_province().get_state_membership().get_naval_base_is_taken())
					continue;

				int32_t current_lvl = o.get_province().get_building_level(economy::province_building_type::naval_base);
				int32_t max_local_lvl = n.get_max_building_level(economy::province_building_type::naval_base);
				int32_t min_build = int32_t(o.get_province().get_modifier_values(sys::provincial_mod_offsets::min_build_naval_base));

				if(max_local_lvl - current_lvl - min_build <= 0)
					continue;

				if(!province::has_naval_base_being_built(state, o.get_province()))
					project_provs.push_back(o.get_province().id);
			}

			auto cap = n.get_capital();
			std::sort(project_provs.begin(), project_provs.end(), [&](dcon::province_id a, dcon::province_id b) {
				auto a_dist = province::sorting_distance(state, a, cap);
				auto b_dist = province::sorting_distance(state, b, cap);
				if(a_dist != b_dist)
					return a_dist < b_dist;
				else
					return a.index() < b.index();
			});
			if(!project_provs.empty()) {
				auto si = state.world.province_get_state_membership(project_provs[0]);
				if(si)
					si.set_naval_base_is_taken(true);
				auto new_rr = fatten(state.world, state.world.force_create_province_building_construction(project_provs[0], n));
				new_rr.set_is_pop_project(false);
				new_rr.set_type(uint8_t(economy::province_building_type::naval_base));
				--max_projects;
			}
		}

		// try railroads
		if((rules & issue_rule::build_railway) != 0 && max_projects > 0) {
			project_provs.clear();
			for(auto o : n.get_province_ownership()) {
				if(n != o.get_province().get_nation_from_province_control())
					continue;
				if(military::province_is_under_siege(state, o.get_province()))
					continue;

				int32_t current_rails_lvl = state.world.province_get_building_level(o.get_province(), economy::province_building_type::railroad);
				int32_t max_local_rails_lvl = state.world.nation_get_max_building_level(n, economy::province_building_type::railroad);
				int32_t min_build_railroad =
					int32_t(state.world.province_get_modifier_values(o.get_province(), sys::provincial_mod_offsets::min_build_railroad));

				if(max_local_rails_lvl - current_rails_lvl - min_build_railroad <= 0)
					continue;

				if(!province::has_railroads_being_built(state, o.get_province())) {
					project_provs.push_back(o.get_province().id);
				}
			}

			auto cap = n.get_capital();
			std::sort(project_provs.begin(), project_provs.end(), [&](dcon::province_id a, dcon::province_id b) {
				auto a_dist = province::sorting_distance(state, a, cap);
				auto b_dist = province::sorting_distance(state, b, cap);
				if(a_dist != b_dist)
					return a_dist < b_dist;
				else
					return a.index() < b.index();
			});

			for(uint32_t i = 0; i < project_provs.size() && max_projects > 0; ++i) {
				auto new_rr = fatten(state.world, state.world.force_create_province_building_construction(project_provs[i], n));
				new_rr.set_is_pop_project(false);
				new_rr.set_type(uint8_t(economy::province_building_type::railroad));
				--max_projects;
			}
		}

		// try forts
		if(max_projects > 0) {
			project_provs.clear();

			for(auto o : n.get_province_ownership()) {
				if(n != o.get_province().get_nation_from_province_control())
					continue;
				if(military::province_is_under_siege(state, o.get_province()))
					continue;

				int32_t current_lvl = state.world.province_get_building_level(o.get_province(), economy::province_building_type::railroad);
				int32_t max_local_lvl = state.world.nation_get_max_building_level(n, economy::province_building_type::railroad);
				int32_t min_build = int32_t(state.world.province_get_modifier_values(o.get_province(), sys::provincial_mod_offsets::min_build_fort));

				if(max_local_lvl - current_lvl - min_build <= 0)
					continue;

				if(!province::has_fort_being_built(state, o.get_province())) {
					project_provs.push_back(o.get_province().id);
				}
			}

			auto cap = n.get_capital();
			std::sort(project_provs.begin(), project_provs.end(), [&](dcon::province_id a, dcon::province_id b) {
				auto a_dist = province::sorting_distance(state, a, cap);
				auto b_dist = province::sorting_distance(state, b, cap);
				if(a_dist != b_dist)
					return a_dist < b_dist;
				else
					return a.index() < b.index();
			});

			for(uint32_t i = 0; i < project_provs.size() && max_projects > 0; ++i) {
				auto new_rr = fatten(state.world, state.world.force_create_province_building_construction(project_provs[i], n));
				new_rr.set_is_pop_project(false);
				new_rr.set_type(uint8_t(economy::province_building_type::fort));
				--max_projects;
			}
		}
	}
}

void update_ai_colonial_investment(sys::state& state) {
	static std::vector<dcon::state_definition_id> investments;
	static std::vector<int32_t> free_points;

	investments.clear();
	investments.resize(uint32_t(state.defines.colonial_rank));

	free_points.clear();
	free_points.resize(uint32_t(state.defines.colonial_rank), -1);

	for(auto col : state.world.in_colonization) {
		auto n = col.get_colonizer();
		if(n.get_is_player_controlled() == false
			&& n.get_rank() <= uint16_t(state.defines.colonial_rank)
			&& !investments[n.get_rank() - 1]
			&& col.get_state().get_colonization_stage() <= uint8_t(2)
			&& state.crisis_colony != col.get_state()
			&& (!state.crisis_war || n.get_is_at_war() == false)
			 ) {

			auto crange = col.get_state().get_colonization();
			if(crange.end() - crange.begin() > 1) {
				if(col.get_last_investment() + int32_t(state.defines.colonization_days_between_investment) <= state.current_date) {

					if(free_points[n.get_rank() - 1] < 0) {
						free_points[n.get_rank() - 1] = nations::free_colonial_points(state, n);
					}

					int32_t cost = 0;;
					if(col.get_state().get_colonization_stage() == 1) {
						cost = int32_t(state.defines.colonization_interest_cost);
					} else if(col.get_level() <= 4) {
						cost = int32_t(state.defines.colonization_influence_cost);
					} else {
						cost =
							int32_t(state.defines.colonization_extra_guard_cost * (col.get_level() - 4) + state.defines.colonization_influence_cost);
					}
					if(free_points[n.get_rank() - 1] >= cost) {
						investments[n.get_rank() - 1] = col.get_state().id;
					}
				}
			}
		}
	}
	for(uint32_t i = 0; i < investments.size(); ++i) {
		if(investments[i])
			province::increase_colonial_investment(state, state.nations_by_rank[i], investments[i]);
	}
}
void update_ai_colony_starting(sys::state& state) {
	static std::vector<int32_t> free_points;
	free_points.clear();
	free_points.resize(uint32_t(state.defines.colonial_rank), -1);
	for(int32_t i = 0; i < int32_t(state.defines.colonial_rank); ++i) {
		if(state.world.nation_get_is_player_controlled(state.nations_by_rank[i])) {
			free_points[i] = 0;
		} else {
			if(military::get_role(state, state.crisis_war, state.nations_by_rank[i]) != military::war_role::none) {
				free_points[i] = 0;
			} else {
				free_points[i] = nations::free_colonial_points(state, state.nations_by_rank[i]);
			}
		}
	}
	for(auto sd : state.world.in_state_definition) {
		if(sd.get_colonization_stage() <= 1) {
			bool has_unowned_land = false;

			dcon::province_id coastal_target;
			for(auto p : state.world.state_definition_get_abstract_state_membership(sd)) {
				if(!p.get_province().get_nation_from_province_ownership()) {
					if(p.get_province().get_is_coast() && !coastal_target) {
						coastal_target = p.get_province();
					}
					if(p.get_province().id.index() < state.province_definitions.first_sea_province.index())
						has_unowned_land = true;
				}
			}
			if(has_unowned_land) {
				for(int32_t i = 0; i < int32_t(state.defines.colonial_rank); ++i) {
					if(free_points[i] > 0) {
						bool adjacent = false;
						if(province::fast_can_start_colony(state, state.nations_by_rank[i], sd, free_points[i], coastal_target, adjacent)) {
							free_points[i] -= int32_t(state.defines.colonization_interest_cost_initial + (adjacent ? state.defines.colonization_interest_cost_neighbor_modifier : 0.0f));

							auto new_rel = fatten(state.world, state.world.force_create_colonization(sd, state.nations_by_rank[i]));
							new_rel.set_level(uint8_t(1));
							new_rel.set_last_investment(state.current_date);
							new_rel.set_points_invested(uint16_t(state.defines.colonization_interest_cost_initial + (adjacent ? state.defines.colonization_interest_cost_neighbor_modifier : 0.0f)));

							state.world.state_definition_set_colonization_stage(sd, uint8_t(1));
						}
					}
				}
			}
		}
	}
}

void upgrade_colonies(sys::state& state) {
	for(auto si : state.world.in_state_instance) {
		if(si.get_capital().get_is_colonial() && si.get_nation_from_state_ownership().get_is_player_controlled() == false) {
			if(province::can_integrate_colony(state, si)) {
				province::upgrade_colonial_state(state, si.get_nation_from_state_ownership(), si);
			}
		}
	}
}

void civilize(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(!n.get_is_player_controlled() && !n.get_is_civilized() && n.get_modifier_values(sys::national_mod_offsets::civilization_progress_modifier) >= 1.0f) {
			nations::make_civilized(state, n);
		}
	}
}

void take_reforms(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(n.get_is_player_controlled() || n.get_owned_province_count() == 0)
			continue;

		if(n.get_is_civilized()) { // political & social
			// Enact social policies to deter Jacobin rebels from overruning the country
			// Reactionaries will popup in effect but they are MORE weak that Jacobins
			dcon::issue_option_id iss;
			float max_support = 0.0f;

			for(auto m : state.world.nation_get_movement_within(n)) {
				if(m.get_movement().get_associated_issue_option() && m.get_movement().get_pop_support() > max_support) {
					iss = m.get_movement().get_associated_issue_option();
					max_support = m.get_movement().get_pop_support();
				}
			}
			if(!iss || !command::can_enact_issue(state, n, iss)) {
				max_support = 0.0f;
				iss = dcon::issue_option_id{};
				state.world.for_each_issue_option([&](dcon::issue_option_id io) {
					if(command::can_enact_issue(state, n, io)) {
						float support = 0.f;
						for(const auto poid : state.world.nation_get_province_ownership_as_nation(n)) {
							for(auto plid : state.world.province_get_pop_location_as_province(poid.get_province())) {
								float weigth = plid.get_pop().get_size() * 0.001f;
								support += state.world.pop_get_demographics(plid.get_pop(), pop_demographics::to_key(state, io)) * weigth;
							}
						}
						if(support > max_support) {
							iss = io;
							max_support = support;
						}
					}
				});
			}
			if(iss) {
				nations::enact_issue(state, n, iss);
			}
		} else { // military and economic
			dcon::reform_option_id cheap_r;
			float cheap_cost = 0.0f;

			auto e_mul = politics::get_economic_reform_multiplier(state, n);
			auto m_mul = politics::get_military_reform_multiplier(state, n);

			for(auto r : state.world.in_reform_option) {
				bool is_military = state.world.reform_get_reform_type(state.world.reform_option_get_parent_reform(r)) == uint8_t(culture::issue_category::military);

				auto reform = state.world.reform_option_get_parent_reform(r);
				auto current = state.world.nation_get_reforms(n, reform.id).id;
				auto allow = state.world.reform_option_get_allow(r);

				if(r.id.index() > current.index() && (!state.world.reform_get_is_next_step_only(reform.id) || current.index() + 1 == r.id.index()) && (!allow || trigger::evaluate(state, allow, trigger::to_generic(n.id), trigger::to_generic(n.id), 0))) {

					float base_cost = float(state.world.reform_option_get_technology_cost(r));
					float reform_factor = is_military ? m_mul : e_mul;

					if(!cheap_r || base_cost * reform_factor < cheap_cost) {
						cheap_cost = base_cost * reform_factor;
						cheap_r = r.id;
					}
				}
			}

			if(cheap_r && cheap_cost <= n.get_research_points()) {
				nations::enact_reform(state, n, cheap_r);
			}
		}
	}
}

bool will_be_crisis_primary_attacker(sys::state& state, dcon::nation_id n) {
	if(state.current_crisis == sys::crisis_type::colonial) {
		auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
		if(colonizers.end() - colonizers.begin() < 2)
			return false;

		auto defending_colonizer = (*(colonizers.begin() + 1)).get_colonizer();
		if(state.world.nation_get_in_sphere_of(defending_colonizer) == n)
			return false;
		if(state.world.nation_get_ai_rival(n) == defending_colonizer
			|| (defending_colonizer.get_in_sphere_of() && !nations::are_allied(state, n, defending_colonizer) && state.world.nation_get_ai_rival(n) == defending_colonizer.get_in_sphere_of())) {
			return true;
		} else {
			return false;
		}
	} else if(state.current_crisis == sys::crisis_type::liberation) {
		auto state_owner = state.world.state_instance_get_nation_from_state_ownership(state.crisis_state);
		auto liberated = state.world.national_identity_get_nation_from_identity_holder(state.crisis_liberation_tag);

		if(state.world.nation_get_in_sphere_of(state_owner) == n || nations::are_allied(state, n, state_owner))
			return false;
		if(state.world.nation_get_ai_rival(n) == state_owner)
			return true;
		if (state.world.nation_get_in_sphere_of(state_owner) && state.world.nation_get_ai_rival(n) == state.world.nation_get_in_sphere_of(state_owner))
			return true;
		if(state.world.nation_get_in_sphere_of(liberated) == n || nations::are_allied(state, n, liberated))
			return true;
		
		return false;
	} else {
		return false;
	}
}
bool will_be_crisis_primary_defender(sys::state& state, dcon::nation_id n) {
	if(state.current_crisis == sys::crisis_type::colonial) {
		auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
		if(colonizers.end() - colonizers.begin() < 2)
			return false;

		auto attacking_colonizer = (*colonizers.begin()).get_colonizer();

		if(state.world.nation_get_in_sphere_of(attacking_colonizer) == n)
			return false;
		if(state.world.nation_get_ai_rival(n) == attacking_colonizer
			|| (attacking_colonizer.get_in_sphere_of() && !nations::are_allied(state, n, attacking_colonizer) && state.world.nation_get_ai_rival(n) == attacking_colonizer.get_in_sphere_of())
			|| state.world.nation_get_ai_rival(n) == state.primary_crisis_attacker) {
			return true;
		} else {
			return false;
		}

	} else if(state.current_crisis == sys::crisis_type::liberation) {
		auto state_owner = state.world.state_instance_get_nation_from_state_ownership(state.crisis_state);
		auto liberated = state.world.national_identity_get_nation_from_identity_holder(state.crisis_liberation_tag);

		if(state.world.nation_get_in_sphere_of(liberated) == n || nations::are_allied(state, n, liberated))
			return false;
		if(state.world.nation_get_ai_rival(n) == liberated)
			return true;
		if(state.world.nation_get_in_sphere_of(liberated) && state.world.nation_get_ai_rival(n) == state.world.nation_get_in_sphere_of(liberated))
			return true;
		if(state.world.nation_get_in_sphere_of(state_owner) == n || nations::are_allied(state, n, state_owner))
			return true;

		return false;
	} else {
		return false;
	}
}

struct crisis_str {
	float attacker = 0.0f;
	float defender = 0.0f;
};

crisis_str estimate_crisis_str(sys::state& state) {
	float atotal = 0.0f;
	float dtotal = 0.0f;

	dcon::nation_id secondary_attacker;
	dcon::nation_id secondary_defender;

	if(state.current_crisis == sys::crisis_type::colonial) {
		auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
		if(colonizers.end() - colonizers.begin() >= 2) {
			secondary_defender = (*(colonizers.begin() + 1)).get_colonizer();
			secondary_attacker = (*(colonizers.begin())).get_colonizer();
		}
	} else if(state.current_crisis == sys::crisis_type::liberation) {
		secondary_defender = state.world.state_instance_get_nation_from_state_ownership(state.crisis_state);
		secondary_attacker = state.world.national_identity_get_nation_from_identity_holder(state.crisis_liberation_tag);
	}

	if(secondary_attacker && secondary_attacker != state.primary_crisis_attacker) {
		atotal += estimate_strength(state, secondary_attacker);
	}
	if(secondary_defender && secondary_defender != state.primary_crisis_defender) {
		dtotal += estimate_strength(state, secondary_defender);
	}
	for(auto& i : state.crisis_participants) {
		if(!i.id)
			break;
		if(!i.merely_interested) {
			if(i.supports_attacker) {
				atotal += estimate_strength(state, i.id);
			} else {
				dtotal += estimate_strength(state, i.id);
			}
		}
	}
	return crisis_str{ atotal, dtotal };
}

bool will_join_crisis_with_offer(sys::state& state, dcon::nation_id n, sys::crisis_join_offer const& offer) {
	if(offer.target == state.world.nation_get_ai_rival(n))
		return true;
	auto offer_bits = state.world.cb_type_get_type_bits(offer.wargoal_type);
	if((offer_bits & (military::cb_flag::po_demand_state | military::cb_flag::po_annex)) != 0)
		return true;

	return false;
}

bool ai_offer_cb(sys::state& state, dcon::cb_type_id t) {
	auto offer_bits = state.world.cb_type_get_type_bits(t);
	if((offer_bits & (military::cb_flag::po_demand_state | military::cb_flag::po_annex)) != 0)
		return false;
	if((offer_bits & military::cb_flag::all_allowed_states) != 0)
		return false;
	if(military::cb_requires_selection_of_a_liberatable_tag(state, t))
		return false;
	if(military::cb_requires_selection_of_a_valid_nation(state, t))
		return false;
	return true;
}

void state_target_list(std::vector<dcon::state_instance_id>& result, sys::state& state, dcon::nation_id for_nation, dcon::nation_id within) {
	result.clear();
	for(auto si : state.world.nation_get_state_ownership(within)) {
		result.push_back(si.get_state().id);
	}

	auto distance_from = state.world.nation_get_capital(for_nation).id;
	int32_t first = 0;

	if(state.world.get_nation_adjacency_by_nation_adjacency_pair(for_nation, within)) {
		int32_t last = int32_t(result.size());
		while(first < last - 1) {
			while(first < last && province::state_borders_nation(state, for_nation, result[first])) {
				++first;
			}
			while(first < last - 1 && !province::state_borders_nation(state, for_nation, result[last - 1])) {
				--last;
			}
			if(first < last - 1) {
				std::swap(result[first], result[last - 1]);
				++first;
				--last;
			}
		}

		std::sort(result.begin(), result.begin() + first, [&](dcon::state_instance_id a, dcon::state_instance_id b) {
			auto a_distance = province::sorting_distance(state, state.world.state_instance_get_capital(a), distance_from);
			auto b_distance = province::sorting_distance(state, state.world.state_instance_get_capital(b), distance_from);
			if(a_distance != b_distance)
				return a_distance < b_distance;
			else
				return a.index() < b.index();
		});
	}
	if(state.world.nation_get_total_ports(for_nation) > 0 && state.world.nation_get_total_ports(within) > 0) {
		int32_t last = int32_t(result.size());
		while(first < last - 1) {
			while(first < last && province::state_is_coastal(state, result[first])) {
				++first;
			}
			while(first < last - 1 && !province::state_is_coastal(state, result[last - 1])) {
				--last;
			}
			if(first < last - 1) {
				std::swap(result[first], result[last - 1]);
				++first;
				--last;
			}
		}
		std::sort(result.begin(), result.begin() + first, [&](dcon::state_instance_id a, dcon::state_instance_id b) {
			auto a_distance = province::sorting_distance(state, state.world.state_instance_get_capital(a), distance_from);
			auto b_distance = province::sorting_distance(state, state.world.state_instance_get_capital(b), distance_from);
			if(a_distance != b_distance)
				return a_distance < b_distance;
			else
				return a.index() < b.index();
		});
	}
	if(first < int32_t(result.size())) {
		std::sort(result.begin() + first, result.end(), [&](dcon::state_instance_id a, dcon::state_instance_id b) {
			auto a_distance = province::sorting_distance(state, state.world.state_instance_get_capital(a), distance_from);
			auto b_distance = province::sorting_distance(state, state.world.state_instance_get_capital(b), distance_from);
			if(a_distance != b_distance)
				return a_distance < b_distance;
			else
				return a.index() < b.index();
		});
	}
}

void update_crisis_leaders(sys::state& state) {
	if(state.crisis_temperature > 0.75f) { // make peace offer
		auto str_est = estimate_crisis_str(state);
		if(str_est.attacker < str_est.defender * 0.66f || str_est.defender < str_est.attacker * 0.66f) { // offer full concession
			bool defender_victory = str_est.attacker < str_est.defender * 0.66f;
			if(defender_victory && state.world.nation_get_is_player_controlled(state.primary_crisis_attacker) == false) {
				command::execute_start_crisis_peace_offer(state, state.primary_crisis_attacker, true);
				auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(state.primary_crisis_attacker);

				for(auto& par : state.crisis_participants) {
					if(!par.id) 
						break;
					if(!par.merely_interested && !par.supports_attacker && par.joined_with_offer.wargoal_type) {
						auto wg = fatten(state.world, state.world.create_wargoal());
						wg.set_peace_offer_from_peace_offer_item(pending);
						wg.set_added_by(par.id);
						wg.set_associated_state(par.joined_with_offer.wargoal_state);
						wg.set_associated_tag(par.joined_with_offer.wargoal_tag);
						wg.set_secondary_nation(par.joined_with_offer.wargoal_secondary_nation);
						wg.set_target_nation(par.joined_with_offer.target);
						wg.set_type(par.joined_with_offer.wargoal_type);
					}
				}

				command::execute_send_crisis_peace_offer(state, state.primary_crisis_attacker);
			} else if(!defender_victory && state.world.nation_get_is_player_controlled(state.primary_crisis_defender) == false) {
				command::execute_start_crisis_peace_offer(state, state.primary_crisis_attacker, true);
				auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(state.primary_crisis_defender);

				for(auto& par : state.crisis_participants) {
					if(!par.id)
						break;
					if(!par.merely_interested && par.supports_attacker && par.joined_with_offer.wargoal_type) {
						auto wg = fatten(state.world, state.world.create_wargoal());
						wg.set_peace_offer_from_peace_offer_item(pending);
						wg.set_added_by(par.id);
						wg.set_associated_state(par.joined_with_offer.wargoal_state);
						wg.set_associated_tag(par.joined_with_offer.wargoal_tag);
						wg.set_secondary_nation(par.joined_with_offer.wargoal_secondary_nation);
						wg.set_target_nation(par.joined_with_offer.target);
						wg.set_type(par.joined_with_offer.wargoal_type);
					}
				}

				command::execute_send_crisis_peace_offer(state, state.primary_crisis_defender);
			}
		} else if(str_est.attacker < str_est.defender * 0.75f && state.current_crisis == sys::crisis_type::liberation) { // defender offers WP
			if(state.world.nation_get_is_player_controlled(state.primary_crisis_defender) == false) {
				command::execute_start_crisis_peace_offer(state, state.primary_crisis_defender, true);
				command::execute_send_crisis_peace_offer(state, state.primary_crisis_defender);
			}
		}
	} else if(state.crisis_temperature > 0.2f) { // recruit nations
		auto str_est = estimate_crisis_str(state);
		if(str_est.attacker < str_est.defender && state.world.nation_get_is_player_controlled(state.primary_crisis_attacker) == false) {
			for(auto& par : state.crisis_participants) {
				if(!par.id)
					break;
				if(par.merely_interested) {
					auto other_cbs = state.world.nation_get_available_cbs(par.id);
					dcon::cb_type_id offer_cb;
					dcon::nation_id target;

					[&]() {
						for(auto& op_par : state.crisis_participants) {
							if(!op_par.id)
								break;
							if(!op_par.merely_interested && op_par.supports_attacker == false) {
								for(auto& cb : other_cbs) {
									if(cb.target == op_par.id && ai_offer_cb(state, cb.cb_type) && military::cb_conditions_satisfied(state, par.id, op_par.id, cb.cb_type)) {
										offer_cb = cb.cb_type;
										target = op_par.id;
										return;
									}
								}
								for(auto cb : state.world.in_cb_type) {
									if((cb.get_type_bits() & military::cb_flag::always) != 0) {
										if(ai_offer_cb(state, cb) && military::cb_conditions_satisfied(state, par.id, op_par.id, cb)) {
											offer_cb = cb;
											target = op_par.id;
											return;
										}
									}
								}
							}
						}
					}();

					if(offer_cb) {
						if(military::cb_requires_selection_of_a_state(state, offer_cb)) {
							std::vector < dcon::state_instance_id> potential_states;
							state_target_list(potential_states, state, par.id, target);
							for(auto s : potential_states) {
								if(military::cb_instance_conditions_satisfied(state, par.id, target, offer_cb, state.world.state_instance_get_definition(s), dcon::national_identity_id{}, dcon::nation_id{})) {

									diplomatic_message::message m;
									memset(&m, 0, sizeof(diplomatic_message::message));
									m.to = par.id;
									m.from = state.primary_crisis_attacker;
									m.data.crisis_offer.target = target;
									m.data.crisis_offer.wargoal_secondary_nation = dcon::nation_id{};
									m.data.crisis_offer.wargoal_state = state.world.state_instance_get_definition(s);
									m.data.crisis_offer.wargoal_tag = dcon::national_identity_id{};
									m.data.crisis_offer.wargoal_type = offer_cb;
									m.type = diplomatic_message::type::take_crisis_side_offer;
									diplomatic_message::post(state, m);

									break;
								}
							}
						} else {
							diplomatic_message::message m;
							memset(&m, 0, sizeof(diplomatic_message::message));
							m.to = par.id;
							m.from = state.primary_crisis_attacker;
							m.data.crisis_offer.target = target;
							m.data.crisis_offer.wargoal_secondary_nation = dcon::nation_id{};
							m.data.crisis_offer.wargoal_state = dcon::state_definition_id{};
							m.data.crisis_offer.wargoal_tag = dcon::national_identity_id{};
							m.data.crisis_offer.wargoal_type = offer_cb;
							m.type = diplomatic_message::type::take_crisis_side_offer;
							diplomatic_message::post(state, m);
						}
					}
				}
			}
		} else if(str_est.attacker < str_est.defender && state.world.nation_get_is_player_controlled(state.primary_crisis_defender) == false) {
			for(auto& par : state.crisis_participants) {
				if(!par.id)
					break;
				if(par.merely_interested) {
					auto other_cbs = state.world.nation_get_available_cbs(par.id);
					dcon::cb_type_id offer_cb;
					dcon::nation_id target;

					[&]() {
						for(auto& op_par : state.crisis_participants) {
							if(!op_par.id)
								break;
							if(!op_par.merely_interested && op_par.supports_attacker == true) {
								for(auto& cb : other_cbs) {
									if(cb.target == op_par.id && ai_offer_cb(state, cb.cb_type) && military::cb_conditions_satisfied(state, par.id, op_par.id, cb.cb_type)) {
										offer_cb = cb.cb_type;
										target = op_par.id;
										return;
									}
								}
								for(auto cb : state.world.in_cb_type) {
									if((cb.get_type_bits() & military::cb_flag::always) != 0) {
										if(ai_offer_cb(state, cb) && military::cb_conditions_satisfied(state, par.id, op_par.id, cb)) {
											offer_cb = cb;
											target = op_par.id;
											return;
										}
									}
								}
							}
						}
					}();

					if(offer_cb) {
						if(military::cb_requires_selection_of_a_state(state, offer_cb)) {
							std::vector < dcon::state_instance_id> potential_states;
							state_target_list(potential_states, state, par.id, target);
							for(auto s : potential_states) {
								if(military::cb_instance_conditions_satisfied(state, par.id, target, offer_cb, state.world.state_instance_get_definition(s), dcon::national_identity_id{}, dcon::nation_id{})) {

									diplomatic_message::message m;
									memset(&m, 0, sizeof(diplomatic_message::message));
									m.to = par.id;
									m.from = state.primary_crisis_defender;
									m.data.crisis_offer.target = target;
									m.data.crisis_offer.wargoal_secondary_nation = dcon::nation_id{};
									m.data.crisis_offer.wargoal_state = state.world.state_instance_get_definition(s);
									m.data.crisis_offer.wargoal_tag = dcon::national_identity_id{};
									m.data.crisis_offer.wargoal_type = offer_cb;
									m.type = diplomatic_message::type::take_crisis_side_offer;
									diplomatic_message::post(state, m);

									break;
								}
							}
						} else {
							diplomatic_message::message m;
							memset(&m, 0, sizeof(diplomatic_message::message));
							m.to = par.id;
							m.from = state.primary_crisis_defender;
							m.data.crisis_offer.target = target;
							m.data.crisis_offer.wargoal_secondary_nation = dcon::nation_id{};
							m.data.crisis_offer.wargoal_state = dcon::state_definition_id{};
							m.data.crisis_offer.wargoal_tag = dcon::national_identity_id{};
							m.data.crisis_offer.wargoal_type = offer_cb;
							m.type = diplomatic_message::type::take_crisis_side_offer;
							diplomatic_message::post(state, m);
						}
					}
				}
			}
		}
	}
}

bool will_accept_crisis_peace_offer(sys::state& state, dcon::nation_id to, bool is_concession, bool missing_wg) {
	if(state.crisis_temperature < 50.0f)
		return false;

	auto str_est = estimate_crisis_str(state);

	if(to == state.primary_crisis_attacker) {
		if(str_est.attacker < str_est.defender * 0.66f)
			return true;
		if(str_est.attacker < str_est.defender * 0.75f)
			return is_concession;

		if(!is_concession)
			return false;

		dcon::nation_id attacker = state.primary_crisis_attacker;
		if(state.current_crisis == sys::crisis_type::colonial) {
			auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
			if(colonizers.end() - colonizers.begin() >= 2) {
				attacker = (*(colonizers.begin())).get_colonizer();
			}
		}

		if(missing_wg)
			return false;

		return true;
	} else if(to == state.primary_crisis_defender) {
		if(str_est.defender < str_est.attacker * 0.66f)
			return true;
		if(str_est.defender < str_est.attacker * 0.75f)
			return is_concession;

		if(!is_concession)
			return false;

		if(missing_wg)
			return false;

		return true;
	}
	return false;
}

bool will_accept_crisis_peace_offer(sys::state& state, dcon::nation_id to, dcon::peace_offer_id peace) {
	if(state.crisis_temperature < 50.0f)
		return false;

	auto str_est = estimate_crisis_str(state);

	if(to == state.primary_crisis_attacker) {
		if(str_est.attacker < str_est.defender * 0.66f)
			return true;
		if(str_est.attacker < str_est.defender * 0.75f)
			return state.world.peace_offer_get_is_concession(peace);

		if(!state.world.peace_offer_get_is_concession(peace))
			return false;

		dcon::nation_id attacker = state.primary_crisis_attacker;
		if(state.current_crisis == sys::crisis_type::colonial) {
			auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
			if(colonizers.end() - colonizers.begin() >= 2) {
				attacker = (*(colonizers.begin())).get_colonizer();
			}
		}
		{
			bool missing_wg = true;
			for(auto wg : state.world.peace_offer_get_peace_offer_item(peace)) {
				if(wg.get_wargoal().get_added_by() == attacker)
					missing_wg = false;
			}
			if(missing_wg)
				return false;
		}

		for(auto& i : state.crisis_participants) {
			if(!i.id)
				break;
			if(!i.merely_interested) {
				if(i.supports_attacker && i.joined_with_offer.wargoal_type) {
					bool missing_wg = true;
					for(auto wg : state.world.peace_offer_get_peace_offer_item(peace)) {
						if(wg.get_wargoal().get_added_by() == i.id)
							missing_wg = false;
					}
					if(missing_wg)
						return false;
				}
			}
		}
		return true;

	} else if(to == state.primary_crisis_defender) {
		if(str_est.defender < str_est.attacker * 0.66f)
			return true;
		if(str_est.defender < str_est.attacker * 0.75f)
			return state.world.peace_offer_get_is_concession(peace);

		if(!state.world.peace_offer_get_is_concession(peace))
			return false;

		if(state.current_crisis == sys::crisis_type::colonial) {
			auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
			if(colonizers.end() - colonizers.begin() >= 2) {
				auto defender = (*(colonizers.begin() + 1)).get_colonizer();

				bool missing_wg = true;
				for(auto wg : state.world.peace_offer_get_peace_offer_item(peace)) {
					if(wg.get_wargoal().get_added_by() == defender)
						missing_wg = false;
				}
				if(missing_wg)
					return false;
			}
		}

		for(auto& i : state.crisis_participants) {
			if(!i.id)
				break;
			if(!i.merely_interested) {
				if(!i.supports_attacker && i.joined_with_offer.wargoal_type) {
					bool missing_wg = true;
					for(auto wg : state.world.peace_offer_get_peace_offer_item(peace)) {
						if(wg.get_wargoal().get_added_by() == i.id)
							missing_wg = false;
					}
					if(missing_wg)
						return false;
				}
			}
		}
		return true;
	}
	return false;
}

void update_war_intervention(sys::state& state) {
	for(auto& gp : state.great_nations) {
		if(state.world.nation_get_is_player_controlled(gp.nation) == false && state.world.nation_get_is_at_war(gp.nation) == false) {
			bool as_attacker = false;
			dcon::war_id intervention_target;
			[&]() {
				for(auto w : state.world.in_war) {
					if(w.get_is_great()) {
						if(command::can_intervene_in_war(state, gp.nation, w, false)) {
							for(auto par : w.get_war_participant()) {
								if(par.get_is_attacker() && military::can_use_cb_against(state, gp.nation, par.get_nation())) {
									intervention_target = w;
									return;
								}
							}
						}
						if(command::can_intervene_in_war(state, gp.nation, w, true)) {
							for(auto par : w.get_war_participant()) {
								if(!par.get_is_attacker() && military::can_use_cb_against(state, gp.nation, par.get_nation())) {
									intervention_target = w;
									as_attacker = true;
									return;
								}
							}
						}
					} else if(military::get_role(state, w, state.world.nation_get_ai_rival(gp.nation)) == military::war_role::attacker) {
						if(command::can_intervene_in_war(state, gp.nation, w, false)) {
							intervention_target = w;
							return;
						}
					}
				}
			}();
			if(intervention_target) {
				command::execute_intervene_in_war(state, gp.nation, intervention_target, as_attacker);
			}
		}
	}
}

dcon::cb_type_id pick_fabrication_type(sys::state& state, dcon::nation_id from, dcon::nation_id target) {
	static std::vector<dcon::cb_type_id> possibilities;
	possibilities.clear();

	for(auto c : state.world.in_cb_type) {
		auto bits = state.world.cb_type_get_type_bits(c);
		if((bits & (military::cb_flag::always | military::cb_flag::is_not_constructing_cb)) != 0)
			continue;
		if((bits & (military::cb_flag::po_demand_state | military::cb_flag::po_annex)) == 0)
			continue;
		// Uncivilized nations are more aggressive to westernize faster
		float infamy_limit = state.world.nation_get_is_civilized(from) ? state.defines.badboy_limit / 2.f : state.defines.badboy_limit;
		if(state.world.nation_get_infamy(from) + military::cb_infamy(state, c) > infamy_limit)
			continue;
		if(!military::cb_conditions_satisfied(state, from, target, c))
			continue;
		auto sl = state.world.nation_get_in_sphere_of(target);
		if(sl == from)
			continue;
		if(nations::are_allied(state, sl, from))
			continue;
		possibilities.push_back(c);
	}

	if(!possibilities.empty()) {
		return possibilities[rng::reduce(uint32_t(rng::get_random(state, uint32_t((from.index() << 3) ^ target.index()))), uint32_t(possibilities.size()))];
	} else {
		return dcon::cb_type_id{};
	}
}

bool valid_construction_target(sys::state& state, dcon::nation_id from, dcon::nation_id target) {
	// Copied from commands.cpp:can_fabricate_cb()
	if(from == target)
		return false;
	if(state.world.nation_get_constructing_cb_type(from))
		return false;
	auto ol = state.world.nation_get_overlord_as_subject(from);
	if(state.world.overlord_get_ruler(ol) && state.world.overlord_get_ruler(ol) != target)
		return false;
	if(state.world.nation_get_in_sphere_of(target) == from)
		return false;
	if(military::are_at_war(state, target, from))
		return false;
	auto sl = state.world.nation_get_in_sphere_of(target);
	if(sl) {
		// Fabricating on OUR sphere leader
		if(sl == from)
			return false;
		// Fabricating on spherelings of our allies
		if(nations::are_allied(state, sl, from))
			return false;
	}
	if(nations::are_allied(state, target, from))
		return false;

	if(estimate_strength(state, target) * 0.5f > estimate_strength(state, from))
		return false;
	if(state.world.nation_get_owned_province_count(target) <= 3)
		return false;
	return true;
}

void update_cb_fabrication(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(!n.get_is_player_controlled() && n.get_owned_province_count() > 0) {
			if(n.get_is_at_war())
				continue;
			// Uncivilized nations are more aggressive to westernize faster
			float infamy_limit = state.world.nation_get_is_civilized(n) ? state.defines.badboy_limit / 2.5f : state.defines.badboy_limit;
			if(n.get_infamy() > infamy_limit)
				continue;
			if(n.get_constructing_cb_type())
				continue;
			
			auto ol = n.get_overlord_as_subject().get_ruler().id;
			if(n.get_ai_rival()
				&& n.get_ai_rival().get_in_sphere_of() != n
				&& (!ol || ol == n.get_ai_rival())
				&& !military::are_at_war(state, n, n.get_ai_rival())
				&& !military::can_use_cb_against(state, n, n.get_ai_rival())) {

				auto cb = pick_fabrication_type(state, n, n.get_ai_rival());
				if(cb) {
					n.set_constructing_cb_target(n.get_ai_rival());
					n.set_constructing_cb_type(cb);
				}
			} else {
				static std::vector<dcon::nation_id> possible_targets;
				possible_targets.clear();
				for(auto i : state.world.in_nation) {
					if(valid_construction_target(state, n, i))
						possible_targets.push_back(i.id);
				}
				if(!possible_targets.empty()) {
					auto t = possible_targets[rng::reduce(uint32_t(rng::get_random(state, uint32_t(n.id.index())) >> 2), uint32_t(possible_targets.size()))];
					auto cb = pick_fabrication_type(state, n, t);
					if(cb) {
						n.set_constructing_cb_target(n.get_ai_rival());
						n.set_constructing_cb_type(cb);
					}
				}
			}
		}
	}
}

bool will_join_war(sys::state& state, dcon::nation_id n, dcon::war_id w, bool as_attacker) {
	if(!as_attacker)
		return true;
	for(auto par : state.world.war_get_war_participant(w)) {
		if(par.get_is_attacker() == !as_attacker) {
			// Could use a CB against this nation?
			if(military::can_use_cb_against(state, n, par.get_nation()))
				return true;
			// Eager to absolutely demolish our rival if possible
			if(state.world.nation_get_ai_rival(n) == par.get_nation())
				return true;
		}
	}
	return false;
}

struct possible_cb {
	dcon::nation_id target;
	dcon::nation_id secondary_nation;
	dcon::national_identity_id associated_tag;
	dcon::state_definition_id state_def;
	dcon::cb_type_id cb;
};

void sort_avilable_cbs(std::vector<possible_cb>& result, sys::state& state, dcon::nation_id n, dcon::war_id w) {
	result.clear();

	auto place_instance_in_result = [&](dcon::nation_id target, dcon::cb_type_id cb, std::vector<dcon::state_instance_id> const& target_states) {
		auto can_use = state.world.cb_type_get_can_use(cb);
		auto allowed_substates = state.world.cb_type_get_allowed_substate_regions(cb);

		if(allowed_substates) {
			if(!state.world.nation_get_is_substate(target))
				return;
			auto ruler = state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(target));
			if(can_use && !trigger::evaluate(state, can_use, trigger::to_generic(ruler), trigger::to_generic(n), trigger::to_generic(n))) {
				return;
			}
		} else {
			if(can_use && !trigger::evaluate(state, can_use, trigger::to_generic(target), trigger::to_generic(n), trigger::to_generic(n))) {
				return;
			}
		}

		auto allowed_countries = state.world.cb_type_get_allowed_countries(cb);
		auto allowed_states = state.world.cb_type_get_allowed_states(cb);

		if(!allowed_countries && allowed_states) {
			bool any_allowed = false;
			for(auto si : target_states) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si), trigger::to_generic(n), trigger::to_generic(n))) {
					if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, state.world.state_instance_get_definition(si), dcon::national_identity_id{}, dcon::nation_id{})) {
						result.push_back(possible_cb{ target, dcon::nation_id{}, dcon::national_identity_id{}, state.world.state_instance_get_definition(si), cb });
						return;
					}
				}
			}
			return;
		}

		if(allowed_substates) { // checking for whether the target is a substate is already done above
			for(auto si : target_states) {
				if(trigger::evaluate(state, allowed_substates, trigger::to_generic(si), trigger::to_generic(n), trigger::to_generic(n))) {
					if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, state.world.state_instance_get_definition(si), dcon::national_identity_id{}, dcon::nation_id{})) {
						result.push_back(possible_cb{ target, dcon::nation_id{}, dcon::national_identity_id{}, state.world.state_instance_get_definition(si), cb });
						return;
					}
				}
			}
			return;
		}

		if(allowed_countries) {
			bool liberate = (state.world.cb_type_get_type_bits(cb) & military::cb_flag::po_transfer_provinces) != 0;
			for(auto other_nation : state.world.in_nation) {
				if(other_nation != target && other_nation != n) {
					if(trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(n),
						trigger::to_generic(other_nation.id))) {
						if(allowed_states) { // check whether any state within the target is valid for free / liberate
							for(auto i = target_states.size(); i-- > 0;) {
								auto si = target_states[i];
								if(trigger::evaluate(state, allowed_states, trigger::to_generic(si), trigger::to_generic(n), trigger::to_generic(other_nation.id))) {

									if(liberate) {
										if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, state.world.state_instance_get_definition(si), other_nation.get_identity_from_identity_holder(), dcon::nation_id{})) {
											result.push_back(possible_cb{ target, dcon::nation_id{}, other_nation.get_identity_from_identity_holder(), state.world.state_instance_get_definition(si), cb });
											return;
										}
									} else {
										if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, state.world.state_instance_get_definition(si), dcon::national_identity_id{}, other_nation)) {
											result.push_back(possible_cb{ target, other_nation, dcon::national_identity_id{}, state.world.state_instance_get_definition(si), cb });
											return;
										}
									}
								}
							}
						} else { // no allowed states trigger
							if(liberate) {
								if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, dcon::state_definition_id{}, other_nation.get_identity_from_identity_holder(), dcon::nation_id{})) {
									result.push_back(possible_cb{ target, dcon::nation_id{}, other_nation.get_identity_from_identity_holder(), dcon::state_definition_id{}, cb });
									return;
								}
							} else {
								if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, dcon::state_definition_id{}, dcon::national_identity_id{}, other_nation)) {
									result.push_back(possible_cb{ target, other_nation, dcon::national_identity_id{}, dcon::state_definition_id{}, cb });
									return;
								}
							}
						}
					}
				}
			}
			
			return;
		}

		if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, dcon::state_definition_id{}, dcon::national_identity_id{}, dcon::nation_id{})) {
			result.push_back(possible_cb{ target, dcon::nation_id{}, dcon::national_identity_id{}, dcon::state_definition_id{}, cb });
		}
		return;
	};

	auto is_attacker = military::get_role(state, w, n) == military::war_role::attacker;
	for(auto par : state.world.war_get_war_participant(w)) {
		if(par.get_is_attacker() != is_attacker) {
			static std::vector<dcon::state_instance_id> target_states;
			state_target_list(target_states, state, n, par.get_nation());

			auto other_cbs = state.world.nation_get_available_cbs(n);
			for(auto& cb : other_cbs) {
				if(cb.target == par.get_nation())
					place_instance_in_result(par.get_nation(), cb.cb_type, target_states);
			}
			for(auto cb : state.world.in_cb_type) {
				if((cb.get_type_bits() & military::cb_flag::always) != 0) {
					place_instance_in_result(par.get_nation(), cb, target_states);
				}
			}
		}
	}

	std::sort(result.begin(), result.end(), [&](possible_cb const& a, possible_cb const& b) {
		if((state.world.nation_get_ai_rival(n) == a.target) != (state.world.nation_get_ai_rival(n) == b.target)) {
			return state.world.nation_get_ai_rival(n) == a.target;
		}
		
		auto a_annexes = (state.world.cb_type_get_type_bits(a.cb) & military::cb_flag::po_annex) != 0;
		auto b_annexes = (state.world.cb_type_get_type_bits(b.cb) & military::cb_flag::po_annex) != 0;
		if(a_annexes != b_annexes)
			return a_annexes;

		auto a_land = (state.world.cb_type_get_type_bits(a.cb) & military::cb_flag::po_demand_state) != 0;
		auto b_land = (state.world.cb_type_get_type_bits(b.cb) & military::cb_flag::po_demand_state) != 0;

		if(a_land < b_land)
			return a_land;

		auto rel_a = state.world.get_diplomatic_relation_by_diplomatic_pair(n, a.target);
		auto rel_b = state.world.get_diplomatic_relation_by_diplomatic_pair(n, b.target);
		if(state.world.diplomatic_relation_get_value(rel_a) != state.world.diplomatic_relation_get_value(rel_b))
			return state.world.diplomatic_relation_get_value(rel_a) < state.world.diplomatic_relation_get_value(rel_b);

		if(a.cb != b.cb)
			return a.cb.index() < b.cb.index();
		if(a.target != b.target)
			return a.target.index() < b.target.index();
		if(a.secondary_nation != b.secondary_nation)
			return a.secondary_nation.index() < b.secondary_nation.index();
		if(a.associated_tag != b.associated_tag)
			return a.associated_tag.index() < b.associated_tag.index();
		return a.state_def.index() < b.state_def.index();
	});
}

void sort_avilable_declaration_cbs(std::vector<possible_cb>& result, sys::state& state, dcon::nation_id n, dcon::nation_id target) {
	result.clear();

	auto place_instance_in_result = [&](dcon::cb_type_id cb, std::vector<dcon::state_instance_id> const& target_states) {
		auto can_use = state.world.cb_type_get_can_use(cb);
		auto allowed_substates = state.world.cb_type_get_allowed_substate_regions(cb);

		if(allowed_substates) {
			if(!state.world.nation_get_is_substate(target))
				return;
			auto ruler = state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(target));
			if(can_use && !trigger::evaluate(state, can_use, trigger::to_generic(ruler), trigger::to_generic(n), trigger::to_generic(n))) {
				return;
			}
		} else {
			if(can_use && !trigger::evaluate(state, can_use, trigger::to_generic(target), trigger::to_generic(n), trigger::to_generic(n))) {
				return;
			}
		}

		auto allowed_countries = state.world.cb_type_get_allowed_countries(cb);
		auto allowed_states = state.world.cb_type_get_allowed_states(cb);

		if(!allowed_countries && allowed_states) {
			bool any_allowed = false;
			for(auto si : target_states) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si), trigger::to_generic(n), trigger::to_generic(n))) {
					result.push_back(possible_cb{ target, dcon::nation_id{}, dcon::national_identity_id{}, state.world.state_instance_get_definition(si), cb });
					return;
				}
			}
			return;
		}

		if(allowed_substates) { // checking for whether the target is a substate is already done above
			for(auto si : target_states) {
				if(trigger::evaluate(state, allowed_substates, trigger::to_generic(si), trigger::to_generic(n), trigger::to_generic(n))) {
					result.push_back(possible_cb{ target, dcon::nation_id{}, dcon::national_identity_id{}, state.world.state_instance_get_definition(si), cb });
					return;
				}
			}
			return;
		}

		if(allowed_countries) {
			bool liberate = (state.world.cb_type_get_type_bits(cb) & military::cb_flag::po_transfer_provinces) != 0;
			for(auto other_nation : state.world.in_nation) {
				if(other_nation != target && other_nation != n) {
					if(trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(n),
						trigger::to_generic(other_nation.id))) {
						if(allowed_states) { // check whether any state within the target is valid for free / liberate
							for(auto i = target_states.size(); i-- > 0;) {
								auto si = target_states[i];
								if(trigger::evaluate(state, allowed_states, trigger::to_generic(si), trigger::to_generic(n), trigger::to_generic(other_nation.id))) {

									if(liberate) {
										result.push_back(possible_cb{ target, dcon::nation_id{}, other_nation.get_identity_from_identity_holder(), state.world.state_instance_get_definition(si), cb });
										return;
										
									} else {
										result.push_back(possible_cb{ target, other_nation, dcon::national_identity_id{}, state.world.state_instance_get_definition(si), cb });
									}
								}
							}
						} else { // no allowed states trigger
							if(liberate) {
								result.push_back(possible_cb{ target, dcon::nation_id{}, other_nation.get_identity_from_identity_holder(), dcon::state_definition_id{}, cb });
								return;
								
							} else {
								result.push_back(possible_cb{ target, other_nation, dcon::national_identity_id{}, dcon::state_definition_id{}, cb });
								return;
							}
						}
					}
				}
			}
			return;
		}

		
		result.push_back(possible_cb{ target, dcon::nation_id{}, dcon::national_identity_id{}, dcon::state_definition_id{}, cb });
	};

	
	static std::vector<dcon::state_instance_id> target_states;
	state_target_list(target_states, state, n, target);

	auto other_cbs = state.world.nation_get_available_cbs(n);
	for(auto& cb : other_cbs) {
		if(cb.target == target)
			place_instance_in_result(cb.cb_type, target_states);
	}
	for(auto cb : state.world.in_cb_type) {
		if((cb.get_type_bits() & military::cb_flag::always) != 0) {
			place_instance_in_result(cb, target_states);
		}
	}
		
	

	std::sort(result.begin(), result.end(), [&](possible_cb const& a, possible_cb const& b) {
		if((state.world.nation_get_ai_rival(n) == a.target) != (state.world.nation_get_ai_rival(n) == b.target)) {
			return state.world.nation_get_ai_rival(n) == a.target;
		}

		auto a_annexes = (state.world.cb_type_get_type_bits(a.cb) & military::cb_flag::po_annex) != 0;
		auto b_annexes = (state.world.cb_type_get_type_bits(b.cb) & military::cb_flag::po_annex) != 0;
		if(a_annexes != b_annexes)
			return a_annexes;

		auto a_land = (state.world.cb_type_get_type_bits(a.cb) & military::cb_flag::po_demand_state) != 0;
		auto b_land = (state.world.cb_type_get_type_bits(b.cb) & military::cb_flag::po_demand_state) != 0;

		if(a_land < b_land)
			return a_land;

		auto rel_a = state.world.get_diplomatic_relation_by_diplomatic_pair(n, a.target);
		auto rel_b = state.world.get_diplomatic_relation_by_diplomatic_pair(n, b.target);
		if(state.world.diplomatic_relation_get_value(rel_a) != state.world.diplomatic_relation_get_value(rel_b))
			return state.world.diplomatic_relation_get_value(rel_a) < state.world.diplomatic_relation_get_value(rel_b);

		if(a.cb != b.cb)
			return a.cb.index() < b.cb.index();
		if(a.target != b.target)
			return a.target.index() < b.target.index();
		if(a.secondary_nation != b.secondary_nation)
			return a.secondary_nation.index() < b.secondary_nation.index();
		if(a.associated_tag != b.associated_tag)
			return a.associated_tag.index() < b.associated_tag.index();
		return a.state_def.index() < b.state_def.index();
	});
}

void add_free_ai_cbs_to_war(sys::state& state, dcon::nation_id n, dcon::war_id w) {
	bool is_attacker = military::is_attacker(state, w, n);
	if(!is_attacker && military::defenders_have_status_quo_wargoal(state, w))
		return;
	if(is_attacker && military::attackers_have_status_quo_wargoal(state, w))
		return;

	bool added = false;
	do {
		added = false;
		static std::vector<possible_cb> potential;
		sort_avilable_cbs(potential, state, n, w);
		for(auto& p : potential) {
			if(!military::war_goal_would_be_duplicate(state, n, w, p.target, p.cb, p.state_def, p.associated_tag, p.secondary_nation)) {
				military::add_wargoal(state, w, n, p.target, p.cb, p.state_def, p.associated_tag, p.secondary_nation);
				nations::adjust_relationship(state, n, p.target, state.defines.addwargoal_relation_on_accept);
				added = true;
			}
		}
	} while(added);
	
}

dcon::cb_type_id pick_gw_extra_cb_type(sys::state& state, dcon::nation_id from, dcon::nation_id target) {
	static std::vector<dcon::cb_type_id> possibilities;
	possibilities.clear();

	auto free_infamy = state.defines.badboy_limit - state.world.nation_get_infamy(from);

	for(auto c : state.world.in_cb_type) {
		auto bits = state.world.cb_type_get_type_bits(c);
		if((bits & (military::cb_flag::always | military::cb_flag::is_not_constructing_cb)) != 0)
			continue;
		if((bits & (military::cb_flag::po_demand_state | military::cb_flag::po_annex)) == 0)
			continue;
		if(military::cb_infamy(state, c) * state.defines.gw_justify_cb_badboy_impact > free_infamy)
			continue;
		if(!military::cb_conditions_satisfied(state, from, target, c))
			continue;

		possibilities.push_back(c);
	}

	if(!possibilities.empty()) {
		return possibilities[rng::reduce(uint32_t(rng::get_random(state, uint32_t((from.index() << 3) ^ target.index()))), uint32_t(possibilities.size()))];
	} else {
		return dcon::cb_type_id{};
	}
}

dcon::nation_id pick_gw_target(sys::state& state, dcon::nation_id from, dcon::war_id w, bool is_attacker) {
	
	if(is_attacker && military::get_role(state, w, state.world.nation_get_ai_rival(from)) == military::war_role::defender)
		return state.world.nation_get_ai_rival(from);
	if(!is_attacker && military::get_role(state, w, state.world.nation_get_ai_rival(from)) == military::war_role::attacker)
		return state.world.nation_get_ai_rival(from);

	static std::vector<dcon::nation_id> possibilities;
	possibilities.clear();

	for(auto par : state.world.war_get_war_participant(w)) {
		if(par.get_is_attacker() != is_attacker) {
			if(state.world.get_nation_adjacency_by_nation_adjacency_pair(from, par.get_nation()))
				possibilities.push_back(par.get_nation().id);
		}
	}
	if(possibilities.empty()) {
		for(auto par : state.world.war_get_war_participant(w)) {
			if(par.get_is_attacker() != is_attacker) {
				if(nations::is_great_power(state, par.get_nation()))
					possibilities.push_back(par.get_nation().id);
			}
		}
	}
	if(!possibilities.empty()) {
		return possibilities[rng::reduce(uint32_t(rng::get_random(state, uint32_t(from.index()  ^ 3))), uint32_t(possibilities.size()))];
	} else {
		return dcon::nation_id{};
	}
}

void add_wg_to_great_war(sys::state& state, dcon::nation_id n, dcon::war_id w) {
	auto rval = rng::get_random(state, n.index() ^ w.index() << 2);
	if((rval & 1) == 0)
		return;

	if(n == state.world.war_get_primary_attacker(w) || n == state.world.war_get_primary_defender(w)) {
		if(((rval >> 1) & 1) == 0) {
			add_free_ai_cbs_to_war(state, n, w);
		}
	}

	auto totalpop = state.world.nation_get_demographics(n, demographics::total);
	auto jingoism_perc = totalpop > 0 ? state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.jingoism)) / totalpop : 0.0f;

	if(jingoism_perc < state.defines.wargoal_jingoism_requirement * state.defines.gw_wargoal_jingoism_requirement_mod)
		return;

	bool attacker = military::get_role(state, w, n) == military::war_role::attacker;
	auto spare_ws = attacker ? (military::primary_warscore(state, w) - military::attacker_peace_cost(state, w)) : (-military::primary_warscore(state, w) - military::defender_peace_cost(state, w));
	if(spare_ws < 1.0f)
		return;

	auto target = pick_gw_target(state, n, w, attacker);
	if(!target)
		return;

	auto cb = pick_gw_extra_cb_type(state, n, target);
	if(!cb)
		return;

	possible_cb instance = [&](){
		auto can_use = state.world.cb_type_get_can_use(cb);
		auto allowed_substates = state.world.cb_type_get_allowed_substate_regions(cb);

		if(allowed_substates) {
			if(!state.world.nation_get_is_substate(target))
				return possible_cb{};
			auto ruler = state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(target));
			if(can_use && !trigger::evaluate(state, can_use, trigger::to_generic(ruler), trigger::to_generic(n), trigger::to_generic(n))) {
				return possible_cb{};
			}
		} else {
			if(can_use && !trigger::evaluate(state, can_use, trigger::to_generic(target), trigger::to_generic(n), trigger::to_generic(n))) {
				return possible_cb{};
			}
		}

		auto allowed_countries = state.world.cb_type_get_allowed_countries(cb);
		auto allowed_states = state.world.cb_type_get_allowed_states(cb);

		if(!allowed_countries && allowed_states) {
			bool any_allowed = false;
			static std::vector<dcon::state_instance_id> target_states;
			state_target_list(target_states, state, n, target);
			for(auto si : target_states) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si), trigger::to_generic(n), trigger::to_generic(n))) {
					if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, state.world.state_instance_get_definition(si), dcon::national_identity_id{}, dcon::nation_id{})) {
						return possible_cb{ target, dcon::nation_id{}, dcon::national_identity_id{}, state.world.state_instance_get_definition(si), cb };
						
					}
				}
			}
			return possible_cb{};
		}

		if(allowed_substates) { // checking for whether the target is a substate is already done above
			static std::vector<dcon::state_instance_id> target_states;
			state_target_list(target_states, state, n, target);
			for(auto si : target_states) {
				if(trigger::evaluate(state, allowed_substates, trigger::to_generic(si), trigger::to_generic(n), trigger::to_generic(n))) {
					if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, state.world.state_instance_get_definition(si), dcon::national_identity_id{}, dcon::nation_id{})) {
						return possible_cb{ target, dcon::nation_id{}, dcon::national_identity_id{}, state.world.state_instance_get_definition(si), cb };
					}
				}
			}
			return possible_cb{};
		}

		if(allowed_countries) {
			bool liberate = (state.world.cb_type_get_type_bits(cb) & military::cb_flag::po_transfer_provinces) != 0;
			for(auto other_nation : state.world.in_nation) {
				if(other_nation != target && other_nation != n) {
					if(trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(n),
						trigger::to_generic(other_nation.id))) {
						if(allowed_states) { // check whether any state within the target is valid for free / liberate
							static std::vector<dcon::state_instance_id> target_states;
							state_target_list(target_states, state, n, target);
							for(auto i = target_states.size(); i-- > 0;) {
								auto si = target_states[i];
								if(trigger::evaluate(state, allowed_states, trigger::to_generic(si), trigger::to_generic(n), trigger::to_generic(other_nation.id))) {

									if(liberate) {
										if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, state.world.state_instance_get_definition(si), other_nation.get_identity_from_identity_holder(), dcon::nation_id{})) {
											return possible_cb{ target, dcon::nation_id{}, other_nation.get_identity_from_identity_holder(), state.world.state_instance_get_definition(si), cb };
										}
									} else {
										if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, state.world.state_instance_get_definition(si), dcon::national_identity_id{}, other_nation)) {
											return possible_cb{ target, other_nation, dcon::national_identity_id{}, state.world.state_instance_get_definition(si), cb };
										}
									}
								}
							}
						} else { // no allowed states trigger
							if(liberate) {
								if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, dcon::state_definition_id{}, other_nation.get_identity_from_identity_holder(), dcon::nation_id{})) {
									return possible_cb{ target, dcon::nation_id{}, other_nation.get_identity_from_identity_holder(), dcon::state_definition_id{}, cb };
								}
							} else {
								if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, dcon::state_definition_id{}, dcon::national_identity_id{}, other_nation)) {
									return possible_cb{ target, other_nation, dcon::national_identity_id{}, dcon::state_definition_id{}, cb };
								}
							}
						}
					}
				}
			}

			return possible_cb{};
		}

		if(!military::war_goal_would_be_duplicate(state, n, w, target, cb, dcon::state_definition_id{}, dcon::national_identity_id{}, dcon::nation_id{})) {
			return possible_cb{ target, dcon::nation_id{}, dcon::national_identity_id{}, dcon::state_definition_id{}, cb };
		}
		return possible_cb{};
	}();

	if(instance.target) {
		military::add_wargoal(state, w, n, target, cb, instance.state_def, instance.associated_tag, instance.secondary_nation);
		nations::adjust_relationship(state, n, target, state.defines.addwargoal_relation_on_accept);
		state.world.nation_get_infamy(n) += military::cb_infamy(state, cb) * state.defines.gw_justify_cb_badboy_impact;
	}
}

void add_gw_goals(sys::state& state) {
	for(auto w : state.world.in_war) {
		if(w.get_is_great()) {
			for(auto par : w.get_war_participant()) {
				if(par.get_nation().get_is_player_controlled() == false) {
					add_wg_to_great_war(state, par.get_nation(), w);
				}
			}
		}
	}
}

void make_peace_offers(sys::state& state) {
	auto send_offer_up_to = [&](dcon::nation_id from, dcon::nation_id to, dcon::war_id w, bool attacker, int32_t score_max, bool concession) {
		if(state.world.nation_get_peace_offer_from_pending_peace_offer(from))
			return; // offer already in flight

		command::execute_start_peace_offer(state, from, to, w, concession);
		auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(from);
		if(!pending)
			return;

		int32_t current_value = 0;
		for(auto wg : state.world.war_get_wargoals_attached(w)) {
			if((military::is_attacker(state, w, wg.get_wargoal().get_added_by()) == attacker) == !concession) {
				auto goal_cost = military::peace_cost(state, w, wg.get_wargoal().get_type(), wg.get_wargoal().get_added_by(), wg.get_wargoal().get_target_nation(), wg.get_wargoal().get_secondary_nation(), wg.get_wargoal().get_associated_state(), wg.get_wargoal().get_associated_tag());
				if(current_value + goal_cost < score_max) {
					current_value += goal_cost;
					state.world.force_create_peace_offer_item(pending, wg.get_wargoal().id);
				}
			}
		}

		command::execute_send_peace_offer(state, from);
	};

	for(auto w : state.world.in_war) {
		if(w.get_primary_attacker().get_is_player_controlled() == false || w.get_primary_defender().get_is_player_controlled() == false) {
			auto overall_score = military::primary_warscore(state, w);
			if(overall_score >= 0) { // attacker winning
				auto total_po_cost = military::attacker_peace_cost(state, w);
				if(w.get_primary_attacker().get_is_player_controlled() == false) { // attacker makes offer
					if(overall_score >= 100 || (overall_score >= 50 && overall_score >= total_po_cost * 2)) {
						send_offer_up_to(w.get_primary_attacker(), w.get_primary_defender(), w, true, int32_t(overall_score), false);
						continue;
					}
					if(w.get_primary_defender().get_is_player_controlled() == false) {
						auto war_duration = state.current_date.value - state.world.war_get_start_date(w).value;
						if(war_duration >= 365) {
							float willingness_factor = float(war_duration - 365) * 10.0f / 365.0f;
							
							if(overall_score > (total_po_cost - willingness_factor) && (-overall_score / 2 + total_po_cost - 2 * willingness_factor) < 0) {
								send_offer_up_to(w.get_primary_attacker(), w.get_primary_defender(), w, true, int32_t(total_po_cost - willingness_factor), false);
								continue;
							}
						}
					}
				} else if(w.get_primary_defender().get_is_player_controlled() == false) { // defender may surrender
					if(overall_score >= 100 || (overall_score >= 50 && overall_score >= total_po_cost * 2)) {
						send_offer_up_to(w.get_primary_defender(), w.get_primary_attacker(), w, false, int32_t(overall_score), true);
						continue;
					}
				}
			} else {
				auto total_po_cost = military::defender_peace_cost(state, w);
				if(w.get_primary_defender().get_is_player_controlled() == false) { // defender makes offer
					if(overall_score <= -100 || (overall_score <= -50 && overall_score <= -total_po_cost * 2)) {
						send_offer_up_to(w.get_primary_defender(), w.get_primary_attacker(), w, false, int32_t(-overall_score), false);
						continue;
					}
					if(w.get_primary_defender().get_is_player_controlled() == false) {
						auto war_duration = state.current_date.value - state.world.war_get_start_date(w).value;
						if(war_duration >= 365) {
							float willingness_factor = float(war_duration - 365) * 10.0f / 365.0f;

							if(-overall_score > (total_po_cost - willingness_factor) && (overall_score / 2 + total_po_cost - 2 * willingness_factor) < 0) {
								send_offer_up_to(w.get_primary_defender(), w.get_primary_attacker(), w, false, int32_t(total_po_cost - willingness_factor), false);
								continue;
							}
						}
					}
				} else if(w.get_primary_attacker().get_is_player_controlled() == false) { // attacker may surrender
					if(overall_score <= -100 || (overall_score <= -50 && overall_score <= -total_po_cost * 2)) {
						send_offer_up_to(w.get_primary_attacker(), w.get_primary_defender(), w, true, int32_t(-overall_score), true);
						continue;
					}
				}
			}
		}
	}
}

bool will_accept_peace_offer_value(sys::state& state,
	dcon::nation_id n, dcon::nation_id from,
	dcon::nation_id prime_attacker, dcon::nation_id prime_defender,
	float primary_warscore, float scoreagainst_me,
	bool offer_from_attacker, bool concession,
	int32_t overall_po_value, int32_t my_po_target,
	int32_t target_personal_po_value, int32_t potential_peace_score_against,
	int32_t my_side_against_target, int32_t my_side_peace_cost,
	int32_t war_duration) {

	bool is_attacking = !offer_from_attacker;

	auto overall_score = primary_warscore;
	if(!is_attacking)
		overall_score = -overall_score;

	int32_t personal_po_value = target_personal_po_value;

	if(concession) {
		if((is_attacking && overall_score <= -50.0f) || (!is_attacking && overall_score >= 50.0f))
			return true;
	}
	if(!concession) {
		overall_po_value = -overall_po_value;
	}

	auto personal_score_saved = personal_po_value - potential_peace_score_against;

	if((prime_attacker == n || prime_defender == n) && (prime_attacker == from || prime_defender == from)) {
		if(overall_score <= -50 && overall_score <= overall_po_value * 2)
			return true;

		if(war_duration < 365) {
			return concession && my_side_peace_cost >= -overall_po_value;
		}
		float willingness_factor = float(war_duration - 365) * 10.0f / 365.0f;
		if(overall_score >= 0) {
			if(concession && (overall_score * 2 - overall_po_value - willingness_factor) < 0)
				return true;
		} else {
			if(overall_score <= overall_po_value && (overall_score / 2 - overall_po_value - willingness_factor) < 0)
				return true;
		}

	} else if((prime_attacker == n || prime_defender == n) && concession) {

		if(scoreagainst_me > 50)
			return true;

		if((is_attacking && overall_score < 0.0f) || (!is_attacking && overall_score > 0.0f)) { // we are losing
			if(my_side_against_target - scoreagainst_me <= overall_po_value + personal_score_saved)
				return true;
		} else {
			if(my_side_against_target <= overall_po_value)
				return true;
		}
	} else {
		if(scoreagainst_me > 50 && scoreagainst_me > -overall_po_value * 2)
			return true;

		if((is_attacking && overall_score < 0.0f) || (!is_attacking && overall_score > 0.0f)) { // we are losing	
			if(scoreagainst_me + personal_score_saved - my_po_target >= -overall_po_value)
				return true;

		} else { // we are winning
			if(std::min(scoreagainst_me, 0.0f) - my_po_target >= -overall_po_value)
				return true;
		}
	}
	return false;
}

bool will_accept_peace_offer(sys::state& state, dcon::nation_id n, dcon::nation_id from, dcon::peace_offer_id p) {
	auto w = state.world.peace_offer_get_war_from_war_settlement(p);
	auto prime_attacker = state.world.war_get_primary_attacker(w);
	auto prime_defender = state.world.war_get_primary_defender(w);
	bool is_attacking = military::is_attacker(state, w, n);

	auto overall_score = military::primary_warscore(state, w);
	if(!is_attacking)
		overall_score = -overall_score;

	int32_t overall_po_value = 0;
	int32_t personal_po_value = 0;
	int32_t wg_in_offer = 0;
	int32_t my_po_target = 0;

	auto concession = state.world.peace_offer_get_is_concession(p);

	if(concession) {
		if((is_attacking && overall_score <= -50.0f) || (!is_attacking && overall_score >= 50.0f))
			return true;
	}

	for(auto wg : state.world.peace_offer_get_peace_offer_item(p)) {
		++wg_in_offer;
		auto wg_value = military::peace_cost(state, w, wg.get_wargoal().get_type(), wg.get_wargoal().get_added_by(), wg.get_wargoal().get_target_nation(), wg.get_wargoal().get_secondary_nation(), wg.get_wargoal().get_associated_state(), wg.get_wargoal().get_associated_tag());
		overall_po_value += wg_value;
		if(wg.get_wargoal().get_target_nation() == n) {
			personal_po_value += wg_value;
		}
	}
	if(!concession) {
		overall_po_value = -overall_po_value;
	}

	int32_t potential_peace_score_against = 0;
	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if(wg.get_wargoal().get_target_nation() == n || wg.get_wargoal().get_added_by() == n) {
			auto wg_value = military::peace_cost(state, w, wg.get_wargoal().get_type(), wg.get_wargoal().get_added_by(), n, wg.get_wargoal().get_secondary_nation(), wg.get_wargoal().get_associated_state(), wg.get_wargoal().get_associated_tag());

			if(wg.get_wargoal().get_target_nation() == n && (wg.get_wargoal().get_added_by() == from || from == prime_attacker || from == prime_defender)) {
				potential_peace_score_against += wg_value;
			}
			if(wg.get_wargoal().get_added_by() == n && (wg.get_wargoal().get_target_nation() == from || from == prime_attacker || from == prime_defender)) {
				my_po_target += wg_value;
			}
		}
	}
	auto personal_score_saved = personal_po_value - potential_peace_score_against;

	if((prime_attacker == n || prime_defender == n) && (prime_attacker == from || prime_defender == from)) {
		if(overall_score <= -50 && overall_score <= overall_po_value * 2)
			return true;

		auto war_duration = state.current_date.value - state.world.war_get_start_date(w).value;
		if(war_duration < 365) {
			return concession && (is_attacking ? military::attacker_peace_cost(state, w) : military::defender_peace_cost(state, w)) >= -overall_po_value;
		}
		float willingness_factor = float(war_duration - 365) * 10.0f / 365.0f;
		if(overall_score >= 0) {
			if(concession && (overall_score * 2 - overall_po_value - willingness_factor) < 0)
				return true;
		} else {
			if(overall_score <= overall_po_value && (overall_score / 2 - overall_po_value - willingness_factor) < 0)
				return true;
		}

	} else if((prime_attacker == n || prime_defender == n) && concession) {
		auto scoreagainst_me = military::directed_warscore(state, w, from, n);

		if(scoreagainst_me > 50)
			return true;

		int32_t my_side_against_target = 0;
		for(auto wg : state.world.war_get_wargoals_attached(w)) {
			if(wg.get_wargoal().get_target_nation() == from) {
				auto wg_value = military::peace_cost(state, w, wg.get_wargoal().get_type(), wg.get_wargoal().get_added_by(), n, wg.get_wargoal().get_secondary_nation(), wg.get_wargoal().get_associated_state(), wg.get_wargoal().get_associated_tag());

				my_side_against_target += wg_value;
			}
		}

		if((is_attacking && overall_score < 0.0f) || (!is_attacking && overall_score > 0.0f)) { // we are losing
			if(my_side_against_target - scoreagainst_me <= overall_po_value + personal_score_saved)
				return true;
		} else {
			if(my_side_against_target <= overall_po_value)
				return true;
		}

	} else {
		auto scoreagainst_me = military::directed_warscore(state, w, from, n);

		if(scoreagainst_me > 50 && scoreagainst_me > -overall_po_value * 2)
			return true;

		if((is_attacking && overall_score < 0.0f) || (!is_attacking && overall_score > 0.0f)) { // we are losing	
			if(scoreagainst_me + personal_score_saved - my_po_target >= -overall_po_value)
				return true;

		} else { // we are winning
			if(std::min(scoreagainst_me, 0.0f) - my_po_target >= -overall_po_value)
				return true;
		}
	}
	return false;
}

void make_war_decs(sys::state& state) {
	auto targets = ve::vectorizable_buffer<dcon::nation_id, dcon::nation_id>(state.world.nation_size());
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_id n{dcon::nation_id::value_base_t(i)};
		if(state.world.nation_get_owned_province_count(n) == 0)
			return;
		if(state.world.nation_get_is_at_war(n))
			return;
		if(state.world.nation_get_is_player_controlled(n))
			return;
		if(auto ol = state.world.nation_get_overlord_as_subject(n); state.world.overlord_get_ruler(ol))
			return;

		auto base_strength = estimate_strength(state, n);
		float best_difference = 2.f;
		for(auto adj : state.world.nation_get_nation_adjacency(n)) {
			auto other = adj.get_connected_nations(0) != n ? adj.get_connected_nations(0) : adj.get_connected_nations(1);
			auto real_target = other.get_overlord_as_subject().get_ruler() ? other.get_overlord_as_subject().get_ruler() : other;

			if(nations::are_allied(state, other, real_target))
				continue;
			if(real_target.get_in_sphere_of() == n)
				continue;
			if(military::has_truce_with(state, n, real_target))
				continue;
			if(!military::can_use_cb_against(state, n, other))
				continue;

			auto str_difference = base_strength + estimate_additional_offensive_strength(state, n, real_target) - estimate_defensive_strength(state, real_target);
			if(str_difference > best_difference) {
				best_difference = str_difference;
				targets.set(n, other.id);
			}
		}

		if(state.world.nation_get_central_ports(n) > 0) {
			// try some random nations
			for(uint32_t j = 0; j < 6; ++j) {
				auto rvalue = rng::get_random(state, uint32_t((n.index() << 3) + j));
				auto reduced_value = rng::reduce(uint32_t(rvalue), state.world.nation_size());
				dcon::nation_id other{dcon::nation_id::value_base_t(reduced_value)};
				auto real_target = fatten(state.world, other).get_overlord_as_subject().get_ruler() ? fatten(state.world, other).get_overlord_as_subject().get_ruler() : fatten(state.world, other);

				if(state.world.nation_get_central_ports(other) == 0 || state.world.nation_get_central_ports(real_target) == 0)
					continue;
				if(nations::are_allied(state, other, real_target))
					continue;
				if(real_target.get_in_sphere_of() == n)
					continue;
				if(military::has_truce_with(state, n, real_target))
					continue;
				if(!military::can_use_cb_against(state, n, other))
					continue;

				auto str_difference = base_strength + estimate_additional_offensive_strength(state, n, real_target) - estimate_defensive_strength(state, real_target);
				if(str_difference > best_difference) {
					best_difference = str_difference;
					targets.set(n, other);
				}
			}
		}
	});
	for(auto n : state.world.in_nation) {
		if(n.get_is_at_war() == false && targets.get(n)) {
			static std::vector<possible_cb> potential;
			sort_avilable_declaration_cbs(potential, state, n, targets.get(n));
			if(!potential.empty()) {
				command::execute_declare_war(state, n, targets.get(n), potential[0].cb, potential[0].state_def, potential[0].associated_tag, potential[0].secondary_nation, true);
			}
		}
	}
}

void update_budget(sys::state& state) {
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_id nid{ dcon::nation_id::value_base_t(i) };
		auto n = fatten(state.world, nid);
		if(n.get_is_player_controlled() || n.get_owned_province_count() == 0)
			return;

		if(n.get_is_at_war()) {
			n.set_land_spending(int8_t(100));
			n.set_naval_spending(int8_t(100));
		} else if(n.get_ai_is_threatened()) {
			n.set_land_spending(int8_t(75));
			n.set_naval_spending(int8_t(75));
		} else {
			n.set_land_spending(int8_t(50));
			n.set_naval_spending(int8_t(50));
		}
		n.set_education_spending(int8_t(100));
		n.set_construction_spending(int8_t(100));
		n.set_tariffs(int8_t(0));

		float poor_militancy = (state.world.nation_get_demographics(n, demographics::poor_militancy) / std::max(1.0f, state.world.nation_get_demographics(n, demographics::poor_total))) / 10.f;
		float mid_militancy = (state.world.nation_get_demographics(n, demographics::middle_militancy) / std::max(1.0f, state.world.nation_get_demographics(n, demographics::middle_total))) / 10.f;
		float rich_militancy = (state.world.nation_get_demographics(n, demographics::rich_militancy) / std::max(1.0f, state.world.nation_get_demographics(n, demographics::rich_total))) / 10.f;

		auto rules = n.get_combined_issue_rules();
		if((rules & issue_rule::expand_factory) != 0 || (rules & issue_rule::build_factory) != 0) {
			// Non-lf prioritize poor people
			int max_poor_tax = int(30.f * (1.f - poor_militancy));
			int max_mid_tax = int(60.f * (1.f - mid_militancy));
			int max_rich_tax = int(90.f * (1.f - rich_militancy));
			int max_social = int(100.f * poor_militancy);

			if(n.get_spending_level() < 1.0f || n.get_last_treasury() >= n.get_stockpiles(economy::money)) { // losing money
				if(n.get_administrative_efficiency() > 0.98f) {
					n.set_administrative_spending(int8_t(std::max(0, n.get_administrative_spending() - 2)));
				}
				if(!n.get_ai_is_threatened()) {
					n.set_military_spending(int8_t(std::max(50, n.get_military_spending() - 5)));
				}
				n.set_social_spending(int8_t(std::max(0, n.get_social_spending() - 2)));

				n.set_poor_tax(int8_t(std::clamp(n.get_poor_tax() + 2, 0, max_poor_tax)));
				n.set_middle_tax(int8_t(std::clamp(n.get_middle_tax() + 3, 0, max_mid_tax)));
				n.set_rich_tax(int8_t(std::clamp(n.get_rich_tax() + 5, 0, max_rich_tax)));
			} else if(n.get_last_treasury() < n.get_stockpiles(economy::money)) { // gaining money
				if(n.get_administrative_efficiency() < 0.98f) {
					n.set_administrative_spending(int8_t(std::min(100, n.get_administrative_spending() + 2)));
				}
				if(n.get_ai_is_threatened()) {
					n.set_military_spending(int8_t(std::min(100, n.get_military_spending() + 10)));
				} else {
					n.set_military_spending(int8_t(std::min(75, n.get_military_spending() + 10)));
				}
				n.set_social_spending(int8_t(std::min(max_social, n.get_social_spending() + 2)));

				n.set_poor_tax(int8_t(std::clamp(n.get_poor_tax() - 2, 0, max_poor_tax)));
				n.set_middle_tax(int8_t(std::clamp(n.get_middle_tax() - 3, 0, max_mid_tax)));
				n.set_rich_tax(int8_t(std::clamp(n.get_rich_tax() - 5, 0, max_rich_tax)));
			}
		} else {
			int max_poor_tax = int(90.f * (1.f - poor_militancy));
			int max_mid_tax = int(60.f * (1.f - mid_militancy));
			int max_rich_tax = int(30.f * (1.f - rich_militancy));
			int max_social = int(100.f * poor_militancy);

			// Laissez faire prioritize tax free capitalists
			if(n.get_spending_level() < 1.0f || n.get_last_treasury() > n.get_stockpiles(economy::money)) { // losing money
				if(n.get_administrative_efficiency() > 0.98f) {
					n.set_administrative_spending(int8_t(std::max(0, n.get_administrative_spending() - 2)));
				}
				if(!n.get_ai_is_threatened()) {
					n.set_military_spending(int8_t(std::max(50, n.get_military_spending() - 5)));
				}
				n.set_social_spending(int8_t(std::max(0, n.get_social_spending() - 2)));

				n.set_poor_tax(int8_t(std::clamp(n.get_poor_tax() + 5, 0, max_poor_tax)));
				n.set_middle_tax(int8_t(std::clamp(n.get_middle_tax() + 3, 0, max_mid_tax)));
				n.set_rich_tax(int8_t(std::clamp(n.get_rich_tax() + 2, 0, max_rich_tax)));
			} else if(n.get_last_treasury() > n.get_stockpiles(economy::money)) { // gaining money
				if(n.get_administrative_efficiency() < 0.98f) {
					n.set_administrative_spending(int8_t(std::min(100, n.get_administrative_spending() + 2)));
				}
				if(n.get_ai_is_threatened()) {
					n.set_military_spending(int8_t(std::min(100, n.get_military_spending() + 10)));
				} else {
					n.set_military_spending(int8_t(std::min(75, n.get_military_spending() + 10)));
				}
				n.set_social_spending(int8_t(std::min(max_social, n.get_social_spending() + 2)));

				n.set_poor_tax(int8_t(std::clamp(n.get_poor_tax() - 5, 0, max_poor_tax)));
				n.set_middle_tax(int8_t(std::clamp(n.get_middle_tax() - 3, 0, max_mid_tax)));
				n.set_rich_tax(int8_t(std::clamp(n.get_rich_tax() - 2, 0, max_rich_tax)));
			}
		}

		economy::bound_budget_settings(state, n);
	});
}

enum class fleet_activity {
	unspecified = 0,			// ai hasn't run on this unit yet
	boarding = 1,			// waiting for troops to arrive
	transporting = 2,			// moving or waiting for troops to disembark
	returning_to_base = 3,	// moving back to home port
	attacking = 4,			// trying to attack another fleet
	merging = 5,				// moving to main base to merge up
	idle = 6,					// sitting in main base with no order
	unloading = 7,			// transport arrived, waiting for units to get off
	failed_transport = 8,
};

enum class army_activity {
	unspecified = 0,
	on_guard = 1,		// hold in place
	attacking = 2,
	merging = 3,
	transport_guard = 4,
	transport_attack = 5,
	// attack_finished = 6,
	attack_gathered = 7,
	attack_transport = 8,
};

void remove_ai_data(sys::state& state, dcon::nation_id n) {
	for(auto ar : state.world.nation_get_army_control(n)) {
		ar.get_army().set_ai_activity(0);
		ar.get_army().set_ai_province(dcon::province_id{});
	}
	for(auto v : state.world.nation_get_navy_control(n)) {
		v.get_navy().set_ai_activity(0);
	}
}

void update_ships(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(!n.get_is_player_controlled() && n.get_is_at_war() == false) {
			dcon::unit_type_id best_transport;
			dcon::unit_type_id best_light;
			dcon::unit_type_id best_big;

			for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
				dcon::unit_type_id j{dcon::unit_type_id::value_base_t(i)};
				if(!n.get_active_unit(j) && !state.military_definitions.unit_base_definitions[j].active)
					continue;

				if(state.military_definitions.unit_base_definitions[j].type == military::unit_type::transport) {
					if(!best_transport || state.military_definitions.unit_base_definitions[best_transport].defence_or_hull < state.military_definitions.unit_base_definitions[j].defence_or_hull) {
						best_transport = j;
					}
				} else if(state.military_definitions.unit_base_definitions[j].type == military::unit_type::light_ship) {
					if(!best_light || state.military_definitions.unit_base_definitions[best_light].defence_or_hull < state.military_definitions.unit_base_definitions[j].defence_or_hull) {
						best_light = j;
					}
				} else if(state.military_definitions.unit_base_definitions[j].type == military::unit_type::big_ship) {
					if(!best_big || state.military_definitions.unit_base_definitions[best_big].defence_or_hull < state.military_definitions.unit_base_definitions[j].defence_or_hull) {
						best_big = j;
					}
				}
			}

			for(auto v : n.get_navy_control()) {
				static std::vector<dcon::ship_id> to_delete;
				to_delete.clear();
				for(auto shp : v.get_navy().get_navy_membership()) {
					auto type = shp.get_ship().get_type();

					if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::transport) {
						if(best_transport && type != best_transport)
							to_delete.push_back(shp.get_ship().id);
					} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::light_ship) {
						if(best_light && type != best_light)
							to_delete.push_back(shp.get_ship().id);
					} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::big_ship) {
						if(best_big && type != best_big)
							to_delete.push_back(shp.get_ship().id);
					}
				}
				for(auto s : to_delete) {
					state.world.delete_ship(s);
				}
			}
		}
	}
}

void build_ships(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(!n.get_is_player_controlled() && n.get_province_naval_construction().begin() == n.get_province_naval_construction().end()) {

			dcon::unit_type_id best_transport;
			dcon::unit_type_id best_light;
			dcon::unit_type_id best_big;

			for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
				dcon::unit_type_id j{dcon::unit_type_id::value_base_t(i)};
				if(!n.get_active_unit(j) && !state.military_definitions.unit_base_definitions[j].active)
					continue;

				if(state.military_definitions.unit_base_definitions[j].type == military::unit_type::transport) {
					if(!best_transport || state.military_definitions.unit_base_definitions[best_transport].defence_or_hull < state.military_definitions.unit_base_definitions[j].defence_or_hull) {
						best_transport = j;
					}
				} else if(state.military_definitions.unit_base_definitions[j].type == military::unit_type::light_ship) {
					if(!best_light || state.military_definitions.unit_base_definitions[best_light].defence_or_hull < state.military_definitions.unit_base_definitions[j].defence_or_hull) {
						best_light = j;
					}
				} else if(state.military_definitions.unit_base_definitions[j].type == military::unit_type::big_ship) {
					if(!best_big || state.military_definitions.unit_base_definitions[best_big].defence_or_hull < state.military_definitions.unit_base_definitions[j].defence_or_hull) {
						best_big = j;
					}
				}
			}

			int32_t num_transports = 0;
			int32_t fleet_cap_in_transports = 0;
			int32_t fleet_cap_in_small = 0;
			int32_t fleet_cap_in_big = 0;

			for(auto v : n.get_navy_control()) {
				for(auto s : v.get_navy().get_navy_membership()) {
					auto type = s.get_ship().get_type();
					if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::transport) {
						++num_transports;
						fleet_cap_in_transports += state.military_definitions.unit_base_definitions[type].supply_consumption_score;
					} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::big_ship) {
						fleet_cap_in_big += state.military_definitions.unit_base_definitions[type].supply_consumption_score;
					} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::light_ship) {
						fleet_cap_in_small += state.military_definitions.unit_base_definitions[type].supply_consumption_score;
					}
				}
			}

			static std::vector<dcon::province_id> owned_ports;
			owned_ports.clear();
			for(auto p : n.get_province_ownership()) {
				if(p.get_province().get_is_coast() && p.get_province().get_nation_from_province_control() == n) {
					owned_ports.push_back(p.get_province().id);
				}
			}
			auto cap = n.get_capital().id;
			std::sort(owned_ports.begin(), owned_ports.end(), [&](dcon::province_id a, dcon::province_id b) {
				auto a_dist = province::sorting_distance(state, a, cap);
				auto b_dist = province::sorting_distance(state, b, cap);
				if(a_dist != b_dist)
					return a_dist < b_dist;
				else
					return a.index() < b.index();
			});

			int32_t constructing_fleet_cap = 0;
			if(fleet_cap_in_transports * 3 < n.get_naval_supply_points()) {
				auto overseas_allowed = state.military_definitions.unit_base_definitions[best_transport].can_build_overseas;
				auto level_req = state.military_definitions.unit_base_definitions[best_transport].min_port_level;
				auto supply_pts = state.military_definitions.unit_base_definitions[best_transport].supply_consumption_score;

				for(uint32_t j = 0; j < owned_ports.size() && (fleet_cap_in_transports + constructing_fleet_cap) * 3 < n.get_naval_supply_points(); ++j) {
					if((overseas_allowed || !province::is_overseas(state, owned_ports[j]))
						&& state.world.province_get_building_level(owned_ports[j], economy::province_building_type::naval_base) >= level_req) {

						auto c = fatten(state.world, state.world.try_create_province_naval_construction(owned_ports[j], n));
						c.set_type(best_transport);
						constructing_fleet_cap += supply_pts;
					}
				}
			} else if(num_transports < 10) {
				auto overseas_allowed = state.military_definitions.unit_base_definitions[best_transport].can_build_overseas;
				auto level_req = state.military_definitions.unit_base_definitions[best_transport].min_port_level;
				auto supply_pts = state.military_definitions.unit_base_definitions[best_transport].supply_consumption_score;

				for(uint32_t j = 0; j < owned_ports.size() && num_transports < 10; ++j) {
					if((overseas_allowed || !province::is_overseas(state, owned_ports[j]))
						&& state.world.province_get_building_level(owned_ports[j], economy::province_building_type::naval_base) >= level_req) {

						auto c = fatten(state.world, state.world.try_create_province_naval_construction(owned_ports[j], n));
						c.set_type(best_transport);
						++num_transports;
						constructing_fleet_cap += supply_pts;
					}
				}
			}

			int32_t used_points = n.get_used_naval_supply_points();
			auto rem_free = n.get_naval_supply_points() - (fleet_cap_in_transports + constructing_fleet_cap);
			auto free_big_points = rem_free / 2 - fleet_cap_in_big;
			auto free_small_points = rem_free / 2 - fleet_cap_in_small;

			{
				auto overseas_allowed = state.military_definitions.unit_base_definitions[best_light].can_build_overseas;
				auto level_req = state.military_definitions.unit_base_definitions[best_light].min_port_level;
				auto supply_pts = state.military_definitions.unit_base_definitions[best_light].supply_consumption_score;

				for(uint32_t j = 0; j < owned_ports.size() && supply_pts <= free_small_points; ++j) {
					if((overseas_allowed || !province::is_overseas(state, owned_ports[j]))
						&& state.world.province_get_building_level(owned_ports[j], economy::province_building_type::naval_base) >= level_req) {

						auto c = fatten(state.world, state.world.try_create_province_naval_construction(owned_ports[j], n));
						c.set_type(best_light);
						free_small_points -= supply_pts;
					}
				}
			}
			{
				auto overseas_allowed = state.military_definitions.unit_base_definitions[best_big].can_build_overseas;
				auto level_req = state.military_definitions.unit_base_definitions[best_big].min_port_level;
				auto supply_pts = state.military_definitions.unit_base_definitions[best_big].supply_consumption_score;

				for(uint32_t j = 0; j < owned_ports.size() && supply_pts <= free_big_points; ++j) {
					if((overseas_allowed || !province::is_overseas(state, owned_ports[j]))
						&& state.world.province_get_building_level(owned_ports[j], economy::province_building_type::naval_base) >= level_req) {

						auto c = fatten(state.world, state.world.try_create_province_naval_construction(owned_ports[j], n));
						c.set_type(best_big);
						free_big_points -= supply_pts;
					}
				}
			}
		}
	}
}

dcon::province_id get_home_port(sys::state& state, dcon::nation_id n) {
	auto cap = state.world.nation_get_capital(n);
	int32_t max_level = -1;
	dcon::province_id result;
	float current_distance = 1.0f;
	for(auto p : state.world.nation_get_province_ownership(n)) {
		if(p.get_province().get_is_coast() && p.get_province().get_nation_from_province_control() == n) {
			if(p.get_province().get_building_level(economy::province_building_type::naval_base) > max_level) {
				max_level = p.get_province().get_building_level(economy::province_building_type::naval_base);
				result = p.get_province();
				current_distance = province::sorting_distance(state, cap, p.get_province());
			} else if(result && p.get_province().get_building_level(economy::province_building_type::naval_base) == max_level && province::sorting_distance(state, cap, p.get_province()) < current_distance) {
				current_distance = province::sorting_distance(state, cap, p.get_province());
				result = p.get_province();
			}
		}
	}
	return result;
}

void refresh_home_ports(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(!n.get_is_player_controlled() && n.get_owned_province_count() > 0) {
			n.set_ai_home_port(get_home_port(state, n));
		}
	}
}

void daily_cleanup(sys::state& state) {
	
}


bool navy_needs_repair(sys::state& state, dcon::navy_id n) {
	auto in_nation = fatten(state.world, state.world.navy_get_controller_from_navy_control(n));
	auto base_spending_level = in_nation.get_effective_naval_spending();
	float oversize_amount =
		in_nation.get_naval_supply_points() > 0
		? std::min(float(in_nation.get_used_naval_supply_points()) / float(in_nation.get_naval_supply_points()), 1.75f)
		: 1.75f;
	float over_size_penalty = oversize_amount > 1.0f ? 2.0f - oversize_amount : 1.0f;
	auto spending_level = base_spending_level * over_size_penalty;
	auto max_org = 0.25f + 0.75f * spending_level;

	for(auto shp : state.world.navy_get_navy_membership(n)) {
		if(shp.get_ship().get_strength() < 0.5f)
			return true;
		if(shp.get_ship().get_org() < 0.75f * max_org)
			return true;
	}
	return false;
}

bool naval_advantage(sys::state& state, dcon::nation_id n) {
	for(auto par : state.world.nation_get_war_participant(n)) {
		for(auto other : par.get_war().get_war_participant()) {
			if(other.get_is_attacker() != par.get_is_attacker()) {
				if(other.get_nation().get_used_naval_supply_points() > state.world.nation_get_used_naval_supply_points(n))
					return false;
			}
		}
	}
	return true;
}

void send_fleet_home(sys::state& state, dcon::navy_id n, fleet_activity moving_status = fleet_activity::returning_to_base, fleet_activity at_base = fleet_activity::idle) {
	auto v = fatten(state.world, n);
	auto home_port = v.get_controller_from_navy_control().get_ai_home_port();
	if(v.get_location_from_navy_location() == home_port) { // already home -- make sure no units are embarked
		auto attached = state.world.navy_get_army_transport(n);
		while(attached.begin() != attached.end()) {
			auto a = (*attached.begin()).get_army();

			a.set_navy_from_army_transport(dcon::navy_id{});
			a.get_path().clear();
			a.set_arrival_time(sys::date{});

			military::army_arrives_in_province(state, a, home_port, military::crossing_type::none, dcon::land_battle_id{});
		}
		v.set_ai_activity(uint8_t(at_base));
	} else if(!home_port) {
		v.set_ai_activity(uint8_t(fleet_activity::unspecified));
	} else if(auto naval_path = province::make_naval_path(state, v.get_location_from_navy_location(), home_port); naval_path.size() > 0) {
		auto new_size = uint32_t(naval_path.size());
		auto existing_path = v.get_path();
		existing_path.resize(new_size);

		for(uint32_t i = 0; i < new_size; ++i) {
			assert(naval_path[i]);
			existing_path[i] = naval_path[i];
		}
		v.set_arrival_time(military::arrival_time_to(state, v, naval_path.back()));
		v.set_ai_activity(uint8_t(moving_status));
	} else {
		v.set_ai_activity(uint8_t(fleet_activity::unspecified));
	}
}

bool set_fleet_target(sys::state& state, dcon::nation_id n, dcon::province_id start, dcon::navy_id for_navy) {
	dcon::province_id result;
	float closest = 0.0f;
	for(auto par : state.world.nation_get_war_participant(n)) {
		for(auto other : par.get_war().get_war_participant()) {
			if(other.get_is_attacker() != par.get_is_attacker()) {
				for(auto nv : other.get_nation().get_navy_control()) {
					auto loc = nv.get_navy().get_location_from_navy_location();
					auto dist = province::sorting_distance(state, start, loc);
					if(!result || dist < closest) {
						if(loc.id.index() < state.province_definitions.first_sea_province.index()) {
							result = loc.get_port_to();
						} else {
							result = loc;
						}
						closest = dist;
					}
				}
			}
		}
	}

	if(result == start)
		return true;

	if(result) {
		auto existing_path = state.world.navy_get_path(for_navy);
		auto path = province::make_naval_path(state, start, result);
		if(path.size() > 0) {
			auto new_size = std::min(uint32_t(path.size()), uint32_t(4));
			existing_path.resize(new_size);
			for(uint32_t i = new_size; i-- > 0;) {
				assert(path[path.size() - 1 - i]);
				existing_path[new_size - 1 - i] = path[path.size() - 1 - i];
			}
			state.world.navy_set_arrival_time(for_navy, military::arrival_time_to(state, for_navy, path.back()));
			state.world.navy_set_ai_activity(for_navy, uint8_t(fleet_activity::attacking));
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

void unload_units_from_transport(sys::state& state, dcon::navy_id n) {
	auto transported_armies = state.world.navy_get_army_transport(n);
	for(auto ar : transported_armies) {
		auto path = province::make_land_path(state, ar.get_army().get_location_from_army_location(), ar.get_army().get_ai_province(), ar.get_army().get_controller_from_army_control(), ar.get_army());
		if(path.size() > 0) {
			auto existing_path = ar.get_army().get_path();
			auto new_size = uint32_t(path.size());
			existing_path.resize(new_size);

			for(uint32_t i = 0; i < new_size; ++i) {
				assert(path[i]);
				existing_path[i] = path[i];
			}
			ar.get_army().set_arrival_time(military::arrival_time_to(state, ar.get_army(), path.back()));
			ar.get_army().set_dig_in(0);
			auto activity = army_activity(ar.get_army().get_ai_activity());
			if(activity == army_activity::transport_guard) {
				ar.get_army().set_ai_activity(uint8_t(army_activity::on_guard));
			} else if(activity == army_activity::transport_attack) {
				ar.get_army().set_ai_activity(uint8_t(army_activity::attack_gathered));
			}
		}
	}
	state.world.navy_set_ai_activity(n, uint8_t(fleet_activity::unloading));
}

bool merge_fleet(sys::state& state, dcon::navy_id n, dcon::province_id p, dcon::nation_id owner) {
	auto merge_target = [&]() {
		for(auto on : state.world.province_get_navy_location(p)) {
			if(on.get_navy() != n && on.get_navy().get_controller_from_navy_control() == owner) {
				auto other_mem = on.get_navy().get_navy_membership();
				if(other_mem.begin() != other_mem.end())
					return on.get_navy().id;
			}
		}
		return dcon::navy_id{};
	}();

	if(!merge_target) {
		return false;
	}

	auto regs = state.world.navy_get_navy_membership(n);
	while(regs.begin() != regs.end()) {
		auto reg = (*regs.begin()).get_ship();
		reg.set_navy_from_navy_membership(merge_target);
	}

	auto transported = state.world.navy_get_army_transport(n);
	while(transported.begin() != transported.end()) {
		auto arm = (*transported.begin()).get_army();
		arm.set_navy_from_army_transport(merge_target);
	}
	return true;
}

void pickup_idle_ships(sys::state& state) {
	for(auto n : state.world.in_navy) {
		if(n.get_battle_from_navy_battle_participation())
			continue;
		if(n.get_arrival_time())
			continue;

		auto owner = n.get_controller_from_navy_control();

		if(owner.get_is_player_controlled() || owner.get_owned_province_count() == 0)
			continue;

		auto home_port = state.world.nation_get_ai_home_port(owner);
		if(!home_port)
			continue;

		auto location = state.world.navy_get_location_from_navy_location(n);
		auto activity = fleet_activity(state.world.navy_get_ai_activity(n));

		switch(activity) {
			case fleet_activity::unspecified:
				if(location == home_port) {
					if(!merge_fleet(state, n, location, owner))
						state.world.navy_set_ai_activity(n, uint8_t(fleet_activity::idle));
				} else if(!home_port) {

				} else {
					// move to home port to merge
					send_fleet_home(state, n, fleet_activity::merging);
				}
				break;
			case fleet_activity::boarding:
			{
				bool all_loaded = true;
				for(auto ar : state.world.nation_get_army_control(owner)) {
					if(ar.get_army().get_ai_activity() == uint8_t(army_activity::transport_guard) || ar.get_army().get_ai_activity() == uint8_t(army_activity::transport_attack)) {
						if(ar.get_army().get_navy_from_army_transport() != n)
							all_loaded = false;
					}
				}

				if(all_loaded) {
					auto transporting_range = n.get_army_transport();
					if(transporting_range.begin() == transporting_range.end()) { // failed to pick up troops
						send_fleet_home(state, n);
					} else {
						auto transported_dest = (*(transporting_range.begin())).get_army().get_ai_province();

						// move to closest port or closest off_shore
						if(transported_dest.get_is_coast()) {
							auto target_prov = transported_dest.id;
							if(!province::has_naval_access_to_province(state, owner, target_prov)) {
								target_prov = state.world.province_get_port_to(target_prov);
							}
							auto naval_path = province::make_naval_path(state, location, target_prov);

							auto existing_path = n.get_path();
							auto new_size = uint32_t(naval_path.size());
							existing_path.resize(new_size);

							for(uint32_t k = 0; k < new_size; ++k) {
								assert(naval_path[k]);
								existing_path[k] = naval_path[k];
							}
							if(new_size > 0)
								n.set_arrival_time(military::arrival_time_to(state, n, naval_path.back()));
							else
								n.set_arrival_time(sys::date{});
							n.set_ai_activity(uint8_t(fleet_activity::transporting));
						} else if(auto path = province::make_path_to_nearest_coast(state, owner, transported_dest); path.empty()) {
							send_fleet_home(state, n);
						} else {
							auto target_prov = path.front();
							if(!province::has_naval_access_to_province(state, owner, target_prov)) {
								target_prov = state.world.province_get_port_to(target_prov);
							}
							auto naval_path = province::make_naval_path(state, location, target_prov);

							auto existing_path = n.get_path();
							auto new_size = uint32_t(naval_path.size());
							existing_path.resize(new_size);

							for(uint32_t k = 0; k < new_size; ++k) {
								assert(naval_path[k]);
								existing_path[k] = naval_path[k];
							}
							if(new_size > 0)
								n.set_arrival_time(military::arrival_time_to(state, n, naval_path.back()));
							else
								n.set_arrival_time(sys::date{});
							n.set_ai_activity(uint8_t(fleet_activity::transporting));
						}
					}
				}
			}
				break;
			case fleet_activity::transporting:
				unload_units_from_transport(state, n);
				break;
			case fleet_activity::failed_transport:
				if(location == home_port) {
					auto attached = state.world.navy_get_army_transport(n);
					while(attached.begin() != attached.end()) {
						auto a = (*attached.begin()).get_army();

						a.set_navy_from_army_transport(dcon::navy_id{});
						a.get_path().clear();
						a.set_arrival_time(sys::date{});

						military::army_arrives_in_province(state, a, home_port, military::crossing_type::none, dcon::land_battle_id{});
					}
					state.world.navy_set_ai_activity(n, uint8_t(fleet_activity::idle));
				} else if(home_port) {
					auto existing_path = state.world.navy_get_path(n);
					auto path = province::make_naval_path(state, location, home_port);
					if(path.size() > 0) {
						auto new_size = uint32_t(path.size());
						existing_path.resize(new_size);

						for(uint32_t i = 0; i < new_size; ++i) {
							assert(path[i]);
							existing_path[i] = path[i];
						}
						state.world.navy_set_arrival_time(n, military::arrival_time_to(state, n, path.back()));
					}
				}
				break;
			case fleet_activity::returning_to_base:
				if(location == home_port) {
					if(!merge_fleet(state, n, location, owner))
						state.world.navy_set_ai_activity(n, uint8_t(fleet_activity::idle));
				} else {
					send_fleet_home(state, n);
				}
				break;
			case fleet_activity::attacking:
				if(state.world.nation_get_is_at_war(owner) == false) {
					send_fleet_home(state, n);
				} else if(navy_needs_repair(state, n)) {
					send_fleet_home(state, n);
				} else {
					if(naval_advantage(state, owner) && set_fleet_target(state, owner, state.world.navy_get_location_from_navy_location(n), n)) {
						// do nothing -- target set successfully
					} else {
						send_fleet_home(state, n);
					}
				}
				break;
			case fleet_activity::merging:
				if(location == home_port) {
					if(!merge_fleet(state, n, location, owner))
						state.world.navy_set_ai_activity(n, uint8_t(fleet_activity::idle));
				} else {
					send_fleet_home(state, n);
				}
				break;
			case fleet_activity::idle:
				if(location != home_port) {
					state.world.navy_set_ai_activity(n, uint8_t(fleet_activity::unspecified));
				} else if(owner.get_is_at_war()) {
					if(!navy_needs_repair(state, n)) {
						if(naval_advantage(state, owner)) {
							set_fleet_target(state, owner, state.world.navy_get_location_from_navy_location(n), n);
						}
					}
				}
				break;
			case fleet_activity::unloading:
			{
				bool failed_transport = false;
				auto transporting = state.world.navy_get_army_transport(n);
				for(auto ar : transporting) {
					if(ar.get_army().get_path().size() == 0) {
						failed_transport = true;
						break;
					}
				}
				if(failed_transport) {
					// an army is stuck on the boats
					state.world.navy_set_ai_activity(n, uint8_t(fleet_activity::failed_transport));
				} else if(transporting.begin() == transporting.end()) {
					// all unloaded -> set to unspecified to send home later in this routine
					state.world.navy_set_ai_activity(n, uint8_t(fleet_activity::unspecified));
				} else {
					// do nothing, still unloading
				}
			}
				break;
		}
	}
}


enum class province_class : uint8_t {
	interior = 0,
	coast = 1,
	low_priority_border = 2,
	border = 3,
	hostile_border = 4
};

struct classified_province {
	dcon::province_id id;
	province_class c;
};

void distribute_guards(sys::state& state, dcon::nation_id n) {
	std::vector<classified_province> provinces;
	provinces.reserve(state.world.province_size());

	auto cap = state.world.nation_get_capital(n);

	for(auto c : state.world.nation_get_province_control(n)) {
		province_class cls = c.get_province().get_is_coast() ? province_class::coast : province_class::interior;
		if(c.get_province() == cap)
			cls = province_class::border;

		for(auto padj : c.get_province().get_province_adjacency()) {
			auto other = padj.get_connected_provinces(0) == c.get_province() ? padj.get_connected_provinces(1) : padj.get_connected_provinces(0);
			auto n_controller = other.get_nation_from_province_control();

			if(n_controller == n) {
				// own province
			} else if(!n_controller && !other.get_rebel_faction_from_province_rebel_control()) {
				// uncolonized or sea
			} else if(other.get_rebel_faction_from_province_rebel_control()) {
				cls = province_class::hostile_border;
				break;
			} else if(nations::are_allied(state, n, n_controller) || n_controller.get_overlord_as_subject().get_ruler() == n) {
				// allied controller
				if(uint8_t(cls) < uint8_t(province_class::low_priority_border)) {
					cls = province_class::low_priority_border;
				}
			} else if(military::are_at_war(state, n, n_controller)) {
				cls = province_class::hostile_border;
				break;
			} else { // other border
				if(uint8_t(cls) < uint8_t(province_class::border)) {
					cls = province_class::border;
				}
			}
		}
		provinces.push_back(classified_province{ c.get_province().id, cls});
	}

	std::sort(provinces.begin(), provinces.end(), [&](classified_province& a, classified_province& b) {
		if(a.c != b.c) {
			return uint8_t(a.c) > uint8_t(b.c);
		}
		auto adist = province::sorting_distance(state, a.id, cap);
		auto bdist = province::sorting_distance(state, b.id, cap);
		if(adist != bdist) {
			return adist < bdist;
		}
		return a.id.index() < b.id.index();
	});

	// form list of guards
	std::vector<dcon::army_id> guards_list;
	guards_list.reserve(state.world.army_size());
	for(auto a : state.world.nation_get_army_control(n)) {
		if(a.get_army().get_ai_activity() == uint8_t(army_activity::on_guard)) {
			guards_list.push_back(a.get_army().id);
		}
	}

	// distribute target provinces
	uint32_t end_of_stage = 0;

	for(uint8_t stage = 5; stage-- > 0 && !guards_list.empty(); ) {
		uint32_t start_of_stage = end_of_stage;

		for(; end_of_stage < provinces.size(); ++end_of_stage) {
			if(uint8_t(provinces[end_of_stage].c) != stage)
				break;
		}

		uint32_t full_loops_through = 0;
		bool guard_assigned = false;
		do {
			guard_assigned = false;
			for(uint32_t j = start_of_stage; j < end_of_stage && !guards_list.empty(); ++j) {
				auto p = provinces[j].id;
				auto p_region = state.world.province_get_connected_region_id(provinces[j].id);
				assert(p_region > 0);
				bool p_region_is_coastal = state.province_definitions.connected_region_is_coastal[p_region - 1];

				if(full_loops_through == 0 || 10.0f * (1 + full_loops_through) <= military::peacetime_attrition_limit(state, n, p)) {
					uint32_t nearest_index = 0;
					dcon::army_id nearest;
					float nearest_distance = 1.0f;
					for(uint32_t k = uint32_t(guards_list.size()) ; k-- > 0;) {
						auto guard_loc = state.world.army_get_location_from_army_location(guards_list[k]);

						/*
						// this wont work because a unit could end up in, for example, a subject's region at the end of a war
						// this region could be landlocked, resulting in this thinking that the unit can only be stationed in that
						// same region, even though it could walk out to another region

						auto g_region = state.world.province_get_connected_region_id(guard_loc);
						assert(g_region > 0);

						if(p_region != g_region && !(state.world.army_get_black_flag(guards_list[k]))  && (!p_region_is_coastal || !state.province_definitions.connected_region_is_coastal[g_region - 1]))
							continue;
						*/

						if(auto d = province::sorting_distance(state, guard_loc, p);
							!nearest || d < nearest_distance) {

							nearest_index = k;
							nearest_distance = d;
							nearest = guards_list[k];
						}
					}

					// assign nearest guard
					if(nearest) {
						state.world.army_set_ai_province(nearest, p);
						guards_list[nearest_index] = guards_list.back();
						guards_list.pop_back();

						guard_assigned = true;
					}
				}
			}
			
			++full_loops_through;
		} while(guard_assigned);

	}
}

void move_idle_guards(sys::state& state) {
	std::vector<dcon::army_id> require_transport;
	require_transport.reserve(state.world.army_size());

	for(auto ar : state.world.in_army) {
		if(ar.get_ai_activity() == uint8_t(army_activity::on_guard)
			&& ar.get_ai_province()
			&& ar.get_ai_province() != ar.get_location_from_army_location()
			&& ar.get_controller_from_army_control()
			&& ar.get_controller_from_army_control().get_is_player_controlled() == false
			&& !ar.get_arrival_time()
			&& !ar.get_battle_from_army_battle_participation()
			&& !ar.get_navy_from_army_transport()) {

			auto path = ar.get_black_flag() ? province::make_unowned_land_path(state, ar.get_location_from_army_location(), ar.get_ai_province()) : province::make_land_path(state, ar.get_location_from_army_location(), ar.get_ai_province(), ar.get_controller_from_army_control(), ar);
			if(path.size() > 0) {
				auto existing_path = ar.get_path();
				auto new_size = uint32_t(path.size());
				existing_path.resize(new_size);

				for(uint32_t i = 0; i < new_size; ++i) {
					assert(path[i]);
					existing_path[i] = path[i];
				}
				ar.set_arrival_time(military::arrival_time_to(state, ar, path.back()));
				ar.set_dig_in(0);
			} else {
				require_transport.push_back(ar.id);
			}
		}
	}

	for(uint32_t i = 0; i < require_transport.size(); ++i) {
		auto coastal_target_prov = state.world.army_get_location_from_army_location(require_transport[i]);
		auto controller = state.world.army_get_controller_from_army_control(require_transport[i]);

		// find free transport fleet

		dcon::navy_id transport_fleet;
		for(auto v : state.world.nation_get_navy_control(controller)) {
			if(v.get_navy().get_battle_from_navy_battle_participation())
				continue;

			fleet_activity activity = fleet_activity(v.get_navy().get_ai_activity());
			if(activity == fleet_activity::attacking || activity == fleet_activity::idle || activity == fleet_activity::returning_to_base) {
				auto in_transport = v.get_navy().get_army_transport();
				if(in_transport.begin() == in_transport.end()) {
					transport_fleet = v.get_navy();
					break;
				}
			}
		}

		auto regs = state.world.army_get_army_membership(require_transport[i]);

		auto tcap = military::transport_capacity(state, transport_fleet);
		tcap -= int32_t(regs.end() - regs.begin());

		if(tcap < 0 || (state.world.nation_get_is_at_war(controller) && !naval_advantage(state, controller))) {
			for(uint32_t j = uint32_t(require_transport.size()); j-- > i + 1;) {
				if(state.world.army_get_controller_from_army_control(require_transport[j]) == controller) {
					state.world.army_set_ai_province(require_transport[j], dcon::province_id{}); // stop rechecking these units
					require_transport[j] = require_transport.back();
					require_transport.pop_back();
				}
			}
			state.world.army_set_ai_province(require_transport[i], dcon::province_id{}); // stop rechecking unit
			continue;
		}

		if(!state.world.province_get_is_coast(coastal_target_prov)) {
			auto path = state.world.army_get_black_flag(require_transport[i])
				? province::make_unowned_path_to_nearest_coast(state, coastal_target_prov)
				: province::make_path_to_nearest_coast(state, controller, coastal_target_prov);
			if(path.empty()) {
				state.world.army_set_ai_province(require_transport[i], dcon::province_id{}); // stop rechecking unit
				continue; // army could not reach coast
			} else {
				coastal_target_prov = path.front();

				auto existing_path = state.world.army_get_path(require_transport[i]);
				auto new_size = uint32_t(path.size());
				existing_path.resize(new_size);

				for(uint32_t k = 0; k < new_size; ++k) {
					assert(path[k]);
					existing_path[k] = path[k];
				}
				state.world.army_set_arrival_time(require_transport[i], military::arrival_time_to(state, require_transport[i], path.back()));
				state.world.army_set_dig_in(require_transport[i], 0);
				state.world.army_set_dig_in(require_transport[i], 0);
			}
		}

		{
			auto fleet_destination = province::has_naval_access_to_province(state, controller, coastal_target_prov) ? coastal_target_prov : state.world.province_get_port_to(coastal_target_prov);
			if(fleet_destination == state.world.navy_get_location_from_navy_location(transport_fleet)) {
				state.world.navy_get_path(transport_fleet).clear();
				state.world.navy_set_arrival_time(transport_fleet, sys::date{});
				state.world.navy_set_ai_activity(transport_fleet, uint8_t(fleet_activity::boarding));
			} else if(auto fleet_path = province::make_naval_path(state, state.world.navy_get_location_from_navy_location(transport_fleet), fleet_destination); fleet_path.empty()) { // this essentially should be impossible ...
				continue;
			} else {
				auto existing_path = state.world.navy_get_path(transport_fleet);
				auto new_size = uint32_t(fleet_path.size());
				existing_path.resize(new_size);

				for(uint32_t k = 0; k < new_size; ++k) {
					assert(fleet_path[k]);
					existing_path[k] = fleet_path[k];
				}
				state.world.navy_set_arrival_time(transport_fleet, military::arrival_time_to(state, transport_fleet, fleet_path.back()));
				state.world.navy_set_ai_activity(transport_fleet, uint8_t(fleet_activity::boarding));
			}
		}

		state.world.army_set_ai_activity(require_transport[i], uint8_t(army_activity::transport_guard));

		auto destination_region = state.world.province_get_connected_region_id(state.world.army_get_ai_province(require_transport[i]));

		// scoop up other armies to transport
		for(uint32_t j = uint32_t(require_transport.size()); j-- > i + 1;) {
			if(state.world.army_get_controller_from_army_control(require_transport[j]) == controller) {
				auto jregs = state.world.army_get_army_membership(require_transport[j]);
				if(tcap >= (jregs.end() - jregs.begin())) { // check if it will fit
					if(state.world.province_get_connected_region_id(state.world.army_get_ai_province(require_transport[j])) != destination_region)
						continue;

					if(state.world.army_get_location_from_army_location(require_transport[j]) == coastal_target_prov) {
						state.world.army_set_ai_activity(require_transport[i], uint8_t(army_activity::transport_guard));
						tcap -= int32_t(jregs.end() - jregs.begin());
					} else {
						auto jpath = state.world.army_get_black_flag(require_transport[j])
							? province::make_land_path(state, state.world.army_get_location_from_army_location(require_transport[j]), coastal_target_prov, controller, require_transport[j])
							: province::make_unowned_land_path(state, state.world.army_get_location_from_army_location(require_transport[j]), coastal_target_prov);
						if(!jpath.empty()) {
							auto existing_path = state.world.army_get_path(require_transport[j]);
							auto new_size = uint32_t(jpath.size());
							existing_path.resize(new_size);

							for(uint32_t k = 0; k < new_size; ++k) {
								assert(jpath[k]);
								existing_path[k] = jpath[k];
							}
							state.world.army_set_arrival_time(require_transport[j], military::arrival_time_to(state, require_transport[j], jpath.back()));
							state.world.army_set_dig_in(require_transport[j], 0);
							state.world.army_set_ai_activity(require_transport[i], uint8_t(army_activity::transport_guard));
							tcap -= int32_t(jregs.end() - jregs.begin());
						}
					}
				}

				require_transport[j] = require_transport.back();
				require_transport.pop_back();
			}
		}
	}
}

void update_naval_transport(sys::state& state) {

	// set armies to move into transports
	for(auto ar : state.world.in_army) {
		if(ar.get_battle_from_army_battle_participation())
			continue;
		if(ar.get_navy_from_army_transport())
			continue;
		if(ar.get_arrival_time())
			continue;

		if(ar.get_ai_activity() == uint8_t(army_activity::transport_guard) || ar.get_ai_activity() == uint8_t(army_activity::transport_attack)) {
			auto controller = ar.get_controller_from_army_control();
			dcon::navy_id transports;
			for(auto v : controller.get_navy_control()) {
				if(v.get_navy().get_ai_activity() == uint8_t(fleet_activity::boarding)) {
					transports = v.get_navy();
				}
			}
			if(!transports) {
				ar.set_ai_activity(uint8_t(army_activity::on_guard));
				ar.set_ai_province(dcon::province_id{});
				continue;
			}
			if(state.world.navy_get_arrival_time(transports) || state.world.navy_get_battle_from_navy_battle_participation(transports))
				continue; // still moving

 			auto army_location = ar.get_location_from_army_location();
			auto transport_location = state.world.navy_get_location_from_navy_location(transports);
			if(transport_location  == army_location) {
				ar.set_navy_from_army_transport(transports);
				ar.set_black_flag(false);
			} else if(army_location.get_port_to() == transport_location) {
				auto existing_path = ar.get_path();
				existing_path.resize(1);
				assert(transport_location);
				existing_path[0] = transport_location;
				ar.set_arrival_time(military::arrival_time_to(state, ar, transport_location));
				ar.set_dig_in(0);
			} else { // transport arrived in inaccessible location
				ar.set_ai_activity(uint8_t(army_activity::on_guard));
				ar.set_ai_province(dcon::province_id{});
			}
		}
	}
}

bool army_ready_for_battle(sys::state& state, dcon::nation_id n, dcon::army_id a) {
	dcon::regiment_id sample_reg;
	auto regs = state.world.army_get_army_membership(a);
	if(regs.begin() != regs.end()) {
		sample_reg = (*regs.begin()).get_regiment().id;
	} else {
		return false;
	}


	auto spending_level = state.world.nation_get_effective_land_spending(n);
	auto max_org = 0.25f + 0.75f * spending_level;

	return state.world.regiment_get_org(sample_reg) > 0.7f * max_org;
}

void gather_to_battle(sys::state& state, dcon::nation_id n, dcon::province_id p) {
	for(auto ar : state.world.nation_get_army_control(n)) {
		army_activity activity = army_activity(ar.get_army().get_ai_activity());
		if(ar.get_army().get_battle_from_army_battle_participation()
			|| ar.get_army().get_navy_from_army_transport()
			|| ar.get_army().get_black_flag()
			|| ar.get_army().get_arrival_time()
			|| (activity != army_activity::on_guard && activity != army_activity::attacking && activity != army_activity::attack_gathered && activity != army_activity::attack_transport)
			|| !army_ready_for_battle(state, n, ar.get_army())) {

			continue;
		}

		auto location = ar.get_army().get_location_from_army_location();
		if(location == p)
			continue;

		auto sdist = province::sorting_distance(state, location, p);
		if(sdist > state.defines.alice_ai_gather_radius)
			continue;

		auto jpath = province::make_land_path(state, location, p, n, ar.get_army());
		if(!jpath.empty()) {

			auto existing_path = ar.get_army().get_path();
			auto new_size = uint32_t(jpath.size());
			existing_path.resize(new_size * 2);

			for(uint32_t k = 0; k < new_size; ++k) {
				assert(jpath[k]);
				existing_path[new_size + k] = jpath[k];
			}
			for(uint32_t k = 1; k < new_size; ++k) {
				assert(jpath[k]);
				existing_path[new_size - k] = jpath[k];
			}
			assert(location);
			existing_path[0] = location;
			ar.get_army().set_arrival_time(military::arrival_time_to(state, ar.get_army(), jpath.back()));
			ar.get_army().set_dig_in(0);
		}
	
	}
}

bool rebel_army_in_province(sys::state& state, dcon::province_id p) {
	for(auto ar : state.world.province_get_army_location(p)) {
		if(!ar.get_army().get_controller_from_army_control())
			return true;
	}
	return false;
}

float estimate_army_strength(sys::state& state, dcon::army_id a) {
	auto regs = state.world.army_get_army_membership(a);
	if(regs.begin() == regs.end())
		return 0.0f;
	auto last_reg = regs.end() - 1;
	return float(regs.end() - regs.begin()) * (*last_reg).get_regiment().get_org();
}

float conservative_estimate_army_strength(sys::state& state, dcon::army_id a) {
	auto regs = state.world.army_get_army_membership(a);
	if(regs.begin() == regs.end())
		return 0.0f;
	return float(regs.end() - regs.begin()) * (*regs.begin()).get_regiment().get_org();
}

float estimate_attack_force(sys::state& state, dcon::province_id target, dcon::nation_id by) {
	float strength_total = 0.f;
	for(auto ar : state.world.in_army) {
		if(ar.get_is_retreating() || ar.get_battle_from_army_battle_participation())
			continue;

		auto loc = ar.get_location_from_army_location();
		auto sdist = province::sorting_distance(state, loc, target);
		if(sdist < state.defines.alice_ai_threat_radius) {
			auto other_nation = ar.get_controller_from_army_control();
			if((by != other_nation) && (!other_nation || military::are_at_war(state, other_nation, by))) {
				strength_total += estimate_army_strength(state, ar);
			}
		}
	}
	return state.defines.alice_ai_threat_overestimate * strength_total;
}

void assign_targets(sys::state& state, dcon::nation_id n) {
	std::vector<dcon::province_id> ready_armies;
	ready_armies.reserve(state.world.province_size());

	int32_t ready_count = 0;
	for(auto ar : state.world.nation_get_army_control(n)) {
		army_activity activity = army_activity(ar.get_army().get_ai_activity());
		if(ar.get_army().get_battle_from_army_battle_participation()
			|| ar.get_army().get_navy_from_army_transport()
			|| ar.get_army().get_black_flag()
			|| ar.get_army().get_arrival_time()
			|| activity != army_activity::on_guard
			|| !army_ready_for_battle(state, n, ar.get_army())) {

			continue;
		}

		++ready_count;
		auto loc = ar.get_army().get_location_from_army_location().id;
		if(std::find(ready_armies.begin(), ready_armies.end(), loc) == ready_armies.end()) {
			ready_armies.push_back(loc);
		}
	}

	if(ready_armies.empty())
		return; // nothing to attack with

	std::vector<dcon::nation_id> at_war_with;
	at_war_with.reserve(state.world.nation_size());

	for(auto w : state.world.nation_get_war_participant(n)) {
		auto attacker = w.get_is_attacker();
		for(auto p : w.get_war().get_war_participant()) {
			if(p.get_is_attacker() != attacker) {
				if(std::find(at_war_with.begin(), at_war_with.end(), p.get_nation().id) == at_war_with.end()) {
					at_war_with.push_back(p.get_nation().id);
				}
			}
		}
	}

	struct army_target {
		float minimal_distance;
		dcon::province_id location;
	};

	std::vector<army_target> potential_targets;
	potential_targets.reserve(state.world.province_size());

	for(auto o : state.world.nation_get_province_ownership(n)) {
		if(!(o.get_province().get_nation_from_province_control())
			|| (o.get_province().get_nation_from_province_control() == n && rebel_army_in_province(state, o.get_province()))
			) {

			potential_targets.push_back(
				army_target{province::sorting_distance(state, o.get_province(), ready_armies[0]), o.get_province().id }
			);
		}
	}
	for(auto w : at_war_with) {
		for(auto o : state.world.nation_get_province_control(w)) {
			potential_targets.push_back(
				army_target{ province::sorting_distance(state, o.get_province(), ready_armies[0]), o.get_province().id }
			);
		}
	}
	for(auto& pt : potential_targets) {
		for(uint32_t i = uint32_t(ready_armies.size()); i-- > 1;) {
			auto sdist = province::sorting_distance(state, ready_armies[i], pt.location);
			if(sdist < pt.minimal_distance) {
				pt.minimal_distance = sdist;
			}
		}
	}
	std::sort(potential_targets.begin(), potential_targets.end(), [&](army_target& a, army_target& b) {
		if(a.minimal_distance != b.minimal_distance)
			return a.minimal_distance < b.minimal_distance;
		else
			return a.location.index() < b.location.index();
	});

	// organize attack stacks
	int32_t max_attacks_to_make = (ready_count + 3) / 4;
	auto const psize = potential_targets.size();
	for(uint32_t i = 0; i < psize && max_attacks_to_make > 0; ++i) {
		if(!potential_targets[i].location)
			continue; // target has been removed as too close by some earlier iteration

		auto target_attack_force = estimate_attack_force(state, potential_targets[i].location, n);
		std::sort(ready_armies.begin(), ready_armies.end(), [&](dcon::province_id a, dcon::province_id b) {
			auto adist = province::direct_distance(state, a, potential_targets[i].location);
			auto bdist = province::direct_distance(state, b, potential_targets[i].location);
			if(adist != bdist)
				return adist > bdist;
			else
				return a.index() < b.index();
		});

		// make list of attackers
		float a_force_str = 0.f;
		int32_t k = int32_t(ready_armies.size());
		for(; k-- > 0 && a_force_str <= target_attack_force;) {
			for(auto ar : state.world.province_get_army_location(ready_armies[k])) {
				if(ar.get_army().get_battle_from_army_battle_participation()
					|| n != ar.get_army().get_controller_from_army_control()
					|| ar.get_army().get_navy_from_army_transport()
					|| ar.get_army().get_black_flag()
					|| ar.get_army().get_arrival_time()
					|| army_activity(ar.get_army().get_ai_activity()) != army_activity::on_guard
					|| !army_ready_for_battle(state, n, ar.get_army())) {

					continue;
				}

				a_force_str += conservative_estimate_army_strength(state, ar.get_army());
			}
		}

		if(a_force_str < target_attack_force) {
			return; // end assigning attackers completely
		}

		// find central province
		dcon::province_id central_province;

		glm::vec3 accumulated{0.0f, 0.0f, 0.0f};
		float minimal_distance = 2.0f;

		for(int32_t m = int32_t(ready_armies.size()); m-- > k + 1; ) {
			accumulated += state.world.province_get_mid_point_b(ready_armies[m]);
		}
		auto magnitude = math::sqrt((accumulated.x * accumulated.x + accumulated.y * accumulated.y) + accumulated.z * accumulated.z);
		if(magnitude > 0.00001f)
			accumulated /= magnitude;
		
		province::for_each_land_province(state, [&](dcon::province_id p) {
			if(!province::has_safe_access_to_province(state, n, p))
				return;
			auto pmid = state.world.province_get_mid_point_b(p);
			if(auto dist = -((accumulated.x * pmid.x + accumulated.y * pmid.y) + accumulated.z * pmid.z); dist < minimal_distance) {
				minimal_distance = dist;
				central_province = p;
			}
		});
		if(!central_province)
			continue;

		// issue safe-move gather command
		for(int32_t m = int32_t(ready_armies.size()); m-- > k + 1; ) {
			for(auto ar : state.world.province_get_army_location(ready_armies[m])) {
				if(ar.get_army().get_battle_from_army_battle_participation()
					|| n != ar.get_army().get_controller_from_army_control()
					|| ar.get_army().get_navy_from_army_transport()
					|| ar.get_army().get_black_flag()
					|| ar.get_army().get_arrival_time()
					|| army_activity(ar.get_army().get_ai_activity()) != army_activity::on_guard
					|| !army_ready_for_battle(state, n, ar.get_army())) {

					continue;
				}

				if(ready_armies[m] == central_province) {
					ar.get_army().set_ai_province(potential_targets[i].location);
					ar.get_army().set_ai_activity(uint8_t(army_activity::attacking));
				} else if(auto path = province::make_safe_land_path(state, ready_armies[m], central_province, n); !path.empty()) {
					auto existing_path = ar.get_army().get_path();
					auto new_size = uint32_t(path.size());
					existing_path.resize(new_size);

					for(uint32_t q = 0; q < new_size; ++q) {
						assert(path[q]);
						existing_path[q] = path[q];
					}
					ar.get_army().set_arrival_time(military::arrival_time_to(state, ar.get_army(), path.back()));
					ar.get_army().set_dig_in(0);
					ar.get_army().set_ai_province(potential_targets[i].location);
					ar.get_army().set_ai_activity(uint8_t(army_activity::attacking));
				}
			}
		}

		ready_armies.resize(k + 1);
		--max_attacks_to_make;

		// remove subsequent targets that are too close
		for(uint32_t j = i + 1; j < psize; ++j) {
			if(province::sorting_distance(state, potential_targets[j].location, potential_targets[i].location) < state.defines.alice_ai_attack_target_radius)
				potential_targets[j].location = dcon::province_id{};
		}
	}
}

void make_attacks(sys::state& state) {
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_id n{dcon::nation_id::value_base_t(i)};
		if(state.world.nation_is_valid(n)) {
			assign_targets(state, n);
		}
	});
}

void make_defense(sys::state& state) {
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_id n{dcon::nation_id::value_base_t(i)};
		if(state.world.nation_is_valid(n)) {
			distribute_guards(state, n);
		}
	});
}

void move_gathered_attackers(sys::state& state) {
	static std::vector<dcon::army_id> require_transport;
	require_transport.clear();

	for(auto ar : state.world.in_army) {
		if(ar.get_ai_activity() == uint8_t(army_activity::attack_transport)) {
			if(!ar.get_arrival_time()
				&& !ar.get_battle_from_army_battle_participation()
				&& !ar.get_navy_from_army_transport()
				&& std::find(require_transport.begin(), require_transport.end(), ar.id) == require_transport.end()) {

				// try to transport
				if(province::has_access_to_province(state, ar.get_controller_from_army_control(), ar.get_ai_province())) {
					require_transport.push_back(ar.id);
				} else {
					ar.set_ai_activity(uint8_t(army_activity::on_guard));
					ar.set_ai_province(dcon::province_id{});
				}
			}
		} else if(ar.get_ai_activity() ==  uint8_t(army_activity::attack_gathered)) {
			if(!ar.get_arrival_time()
				&& !ar.get_battle_from_army_battle_participation()
				&& !ar.get_navy_from_army_transport()) {

				if(ar.get_location_from_army_location() == ar.get_ai_province()) { // attack finished ?
					if(ar.get_location_from_army_location().get_nation_from_province_control() && !military::are_at_war(state, ar.get_location_from_army_location().get_nation_from_province_control(), ar.get_controller_from_army_control())) {

						ar.set_ai_activity(uint8_t(army_activity::on_guard));
						ar.set_ai_province(dcon::province_id{});
					}
				} else {
					if(province::has_access_to_province(state, ar.get_controller_from_army_control(), ar.get_ai_province())) {
						if(auto path = province::make_land_path(state, ar.get_location_from_army_location(), ar.get_ai_province(), ar.get_controller_from_army_control(), ar); path.size() > 0) {
							
							auto existing_path = ar.get_path();
							auto new_size = uint32_t(path.size());
							existing_path.resize(new_size);

							for(uint32_t i = 0; i < new_size; ++i) {
								assert(path[i]);
								existing_path[i] = path[i];
							}
							ar.set_arrival_time(military::arrival_time_to(state, ar, path.back()));
							ar.set_dig_in(0);
						} else {
							ar.set_ai_activity(uint8_t(army_activity::on_guard));
							ar.set_ai_province(dcon::province_id{});
						}
					} else {
						ar.set_ai_activity(uint8_t(army_activity::on_guard));
						ar.set_ai_province(dcon::province_id{});
					}
				}
			}
		} else if(ar.get_ai_activity() == uint8_t(army_activity::attacking)
			&& ar.get_ai_province() != ar.get_location_from_army_location()
			&& !ar.get_arrival_time()
			&& !ar.get_battle_from_army_battle_participation()
			&& !ar.get_navy_from_army_transport()) {

			bool all_gathered = true;
			for(auto o : ar.get_controller_from_army_control().get_army_control()) {
				if(o.get_army().get_ai_province() == ar.get_ai_province()) {
					if(ar.get_location_from_army_location() != o.get_army().get_location_from_army_location()) {
						// an army with the same target on a different location
						if(o.get_army().get_path().size() > 0 && o.get_army().get_path()[0] == ar.get_location_from_army_location()) {
							all_gathered = false;
							break;
						}
					} else {
						// on same location
						if(o.get_army().get_battle_from_army_battle_participation()) { // is in a battle
							all_gathered = false;
							break;
						}
					}
				}
			}

			if(all_gathered) {
				if(province::has_access_to_province(state, ar.get_controller_from_army_control(), ar.get_ai_province())) {
					if(ar.get_ai_province() == ar.get_location_from_army_location()) {
						for(auto o : ar.get_location_from_army_location().get_army_location()) {
							if(o.get_army().get_ai_province() == ar.get_ai_province()
								&& o.get_army().get_path().size() == 0) {

								o.get_army().set_ai_activity(uint8_t(army_activity::attack_gathered));
							}
						}
					} else if(auto path = province::make_land_path(state, ar.get_location_from_army_location(), ar.get_ai_province(), ar.get_controller_from_army_control(), ar); path.size() > 0) {

						for(auto o : ar.get_location_from_army_location().get_army_location()) {
							if(o.get_army().get_ai_province() == ar.get_ai_province()
								&& o.get_army().get_path().size() == 0) {

								auto existing_path = o.get_army().get_path();
								auto new_size = uint32_t(path.size());
								existing_path.resize(new_size);

								for(uint32_t i = 0; i < new_size; ++i) {
									assert(path[i]);
									existing_path[i] = path[i];
								}
								o.get_army().set_arrival_time(military::arrival_time_to(state, o.get_army(), path.back()));
								o.get_army().set_dig_in(0);
								o.get_army().set_ai_activity(uint8_t(army_activity::attack_gathered));
							}
						}
					} else {
						for(auto o : ar.get_location_from_army_location().get_army_location()) {
							if(o.get_army().get_ai_province() == ar.get_ai_province()
								&& o.get_army().get_path().size() == 0) {

								require_transport.push_back(o.get_army().id);
								ar.set_ai_activity(uint8_t(army_activity::attack_transport));
							}
						}
					}
				} else {
					ar.set_ai_activity(uint8_t(army_activity::on_guard));
					ar.set_ai_province(dcon::province_id{});
				}
			}
		}
	}

	for(uint32_t i = 0; i < require_transport.size(); ++i) {
		auto coastal_target_prov = state.world.army_get_location_from_army_location(require_transport[i]);
		auto controller = state.world.army_get_controller_from_army_control(require_transport[i]);

		// find free transport fleet

		dcon::navy_id transport_fleet;
		for(auto v : state.world.nation_get_navy_control(controller)) {
			if(v.get_navy().get_battle_from_navy_battle_participation())
				continue;

			fleet_activity activity = fleet_activity(v.get_navy().get_ai_activity());
			if(activity == fleet_activity::attacking || activity == fleet_activity::idle || activity == fleet_activity::returning_to_base) {
				auto in_transport = v.get_navy().get_army_transport();
				if(in_transport.begin() == in_transport.end()) {
					transport_fleet = v.get_navy();
					break;
				}
			}
		}

		auto regs = state.world.army_get_army_membership(require_transport[i]);

		auto tcap = military::transport_capacity(state, transport_fleet);
		tcap -= int32_t(regs.end() - regs.begin());

		if(tcap < 0 || (state.world.nation_get_is_at_war(controller) && !naval_advantage(state, controller))) {
			for(uint32_t j = uint32_t(require_transport.size()); j-- > i + 1;) {
				if(state.world.army_get_controller_from_army_control(require_transport[j]) == controller) {
					state.world.army_set_ai_activity(require_transport[j], uint8_t(army_activity::on_guard));
					state.world.army_set_ai_province(require_transport[j], dcon::province_id{}); // stop rechecking these units
					require_transport[j] = require_transport.back();
					require_transport.pop_back();
				}
			}
			state.world.army_set_ai_activity(require_transport[i], uint8_t(army_activity::on_guard));
			state.world.army_set_ai_province(require_transport[i], dcon::province_id{}); // stop rechecking these units
			continue;
		}

		if(!state.world.province_get_is_coast(coastal_target_prov)) {
			auto path = province::make_path_to_nearest_coast(state, controller, coastal_target_prov);
			if(path.empty()) {
				state.world.army_set_ai_activity(require_transport[i], uint8_t(army_activity::on_guard));
				state.world.army_set_ai_province(require_transport[i], dcon::province_id{});
				continue; // army could not reach coast
			} else {
				coastal_target_prov = path.front();

				auto existing_path = state.world.army_get_path(require_transport[i]);
				auto new_size = uint32_t(path.size());
				existing_path.resize(new_size);

				for(uint32_t k = 0; k < new_size; ++k) {
					assert(path[k]);
					existing_path[k] = path[k];
				}
				state.world.army_set_arrival_time(require_transport[i], military::arrival_time_to(state, require_transport[i], path.back()));
				state.world.army_set_dig_in(require_transport[i], 0);
			}
		}

		{
			auto fleet_destination = province::has_naval_access_to_province(state, controller, coastal_target_prov) ? coastal_target_prov : state.world.province_get_port_to(coastal_target_prov);
			if(fleet_destination == state.world.navy_get_location_from_navy_location(transport_fleet)) {
				state.world.navy_get_path(transport_fleet).clear();
				state.world.navy_set_arrival_time(transport_fleet, sys::date{});
				state.world.navy_set_ai_activity(transport_fleet, uint8_t(fleet_activity::boarding));
			} else if(auto fleet_path = province::make_naval_path(state, state.world.navy_get_location_from_navy_location(transport_fleet), fleet_destination); fleet_path.empty()) {
				continue;
			} else {
				auto existing_path = state.world.navy_get_path(transport_fleet);
				auto new_size = uint32_t(fleet_path.size());
				existing_path.resize(new_size);

				for(uint32_t k = 0; k < new_size; ++k) {
					assert(fleet_path[k]);
					existing_path[k] = fleet_path[k];
				}
				state.world.navy_set_arrival_time(transport_fleet, military::arrival_time_to(state, transport_fleet, fleet_path.back()));
				state.world.navy_set_ai_activity(transport_fleet, uint8_t(fleet_activity::boarding));
			}
		}

		state.world.army_set_ai_activity(require_transport[i], uint8_t(army_activity::transport_attack));

		auto destination_region = state.world.province_get_connected_region_id(state.world.army_get_ai_province(require_transport[i]));

		// scoop up other armies to transport
		for(uint32_t j = uint32_t(require_transport.size()); j-- > i + 1;) {
			if(state.world.army_get_controller_from_army_control(require_transport[j]) == controller) {
				auto jregs = state.world.army_get_army_membership(require_transport[j]);
				if(tcap >= (jregs.end() - jregs.begin())) { // check if it will fit
					if(state.world.province_get_connected_region_id(state.world.army_get_ai_province(require_transport[j])) != destination_region)
						continue;

					if(state.world.army_get_location_from_army_location(require_transport[j]) == coastal_target_prov) {
						state.world.army_set_ai_activity(require_transport[i], uint8_t(army_activity::transport_attack));
						tcap -= int32_t(jregs.end() - jregs.begin());
					} else {
						auto jpath = state.world.army_get_black_flag(require_transport[j])
							? province::make_land_path(state, state.world.army_get_location_from_army_location(require_transport[j]), coastal_target_prov, controller, require_transport[j])
							: province::make_unowned_land_path(state, state.world.army_get_location_from_army_location(require_transport[j]), coastal_target_prov);
						if(!jpath.empty()) {
							auto existing_path = state.world.army_get_path(require_transport[j]);
							auto new_size = uint32_t(jpath.size());
							existing_path.resize(new_size);

							for(uint32_t k = 0; k < new_size; ++k) {
								assert(jpath[k]);
								existing_path[k] = jpath[k];
							}
							state.world.army_set_arrival_time(require_transport[j], military::arrival_time_to(state, require_transport[j], jpath.back()));
							state.world.army_set_dig_in(require_transport[j], 0);
							state.world.army_set_ai_activity(require_transport[i], uint8_t(army_activity::transport_attack));
							tcap -= int32_t(jregs.end() - jregs.begin());
						}
					}
				}

				require_transport[j] = require_transport.back();
				require_transport.pop_back();
			}
		}
	}
}

void update_land_constructions(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(n.get_is_player_controlled() || n.get_owned_province_count() == 0)
			continue;

		auto constructions = state.world.nation_get_province_land_construction(n);
		if(constructions.begin() != constructions.end())
			continue;

		int32_t num_frontline = 0;
		int32_t num_support = 0;

		bool can_make_inf = state.world.nation_get_active_unit(n, state.military_definitions.infantry) || state.military_definitions.unit_base_definitions[state.military_definitions.infantry].active;
		bool can_make_art = state.world.nation_get_active_unit(n, state.military_definitions.artillery) || state.military_definitions.unit_base_definitions[state.military_definitions.artillery].active;

		for(auto ar : state.world.nation_get_army_control(n)) {
			for(auto r : ar.get_army().get_army_membership()) {
				auto type = r.get_regiment().get_type();
				auto etype = state.military_definitions.unit_base_definitions[type].type;
				if(etype == military::unit_type::support || etype == military::unit_type::special) {
					++num_support;
				} else {
					++num_frontline;
				}
				if(can_make_inf && type == state.military_definitions.irregular) { // free ai upgrades
					r.get_regiment().set_type(state.military_definitions.infantry);
				}
			}
		}

		const std::function<dcon::unit_type_id()> decide_type = can_make_art
			? std::function<dcon::unit_type_id()>([&]() {
			if(num_frontline > num_support) {
				++num_support;
				return state.military_definitions.artillery;
			} else {
				++num_frontline;
				return can_make_inf ? state.military_definitions.infantry : state.military_definitions.irregular;
			}
			})
			: std::function<dcon::unit_type_id()>([&]() {
				return can_make_inf ? state.military_definitions.infantry : state.military_definitions.irregular;
			});

		for(auto p : state.world.nation_get_province_ownership(n)) {
			if(p.get_province().get_nation_from_province_control() != n)
				continue;

			if(p.get_province().get_is_colonial()) {
				float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_colony_multiplier;
				float minimum = state.defines.pop_min_size_for_regiment;

				for(auto pop : p.get_province().get_pop_location()) {
					if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
						if(pop.get_pop().get_size() >= minimum) {
							auto amount = int32_t((pop.get_pop().get_size() / divisor) + 1);
							auto regs = pop.get_pop().get_regiment_source();
							auto building = pop.get_pop().get_province_land_construction();
							auto num_to_make = amount - ((regs.end() - regs.begin()) + (building.end() - building.begin()));

							while(num_to_make > 0) {
								auto c = fatten(state.world, state.world.try_create_province_land_construction(pop.get_pop().id, n));
								c.set_type(decide_type());
								--num_to_make;
							}
						}
					}
				}
			} else if(!p.get_province().get_is_owner_core()) {
				float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_noncore_multiplier;
				float minimum = state.defines.pop_min_size_for_regiment;

				dcon::pop_id non_preferred;
				for(auto pop : p.get_province().get_pop_location()) {
					if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
						if(pop.get_pop().get_size() >= minimum) {
							auto amount = int32_t((pop.get_pop().get_size() / divisor) + 1);
							auto regs = pop.get_pop().get_regiment_source();
							auto building = pop.get_pop().get_province_land_construction();
							auto num_to_make = amount - ((regs.end() - regs.begin()) + (building.end() - building.begin()));

							while(num_to_make > 0) {
								auto c = fatten(state.world, state.world.try_create_province_land_construction(pop.get_pop().id, n));
								c.set_type(decide_type());
								--num_to_make;
							}
						}
					}
				}
			} else {
				float divisor = state.defines.pop_size_per_regiment;
				float minimum = state.defines.pop_min_size_for_regiment;

				dcon::pop_id non_preferred;
				for(auto pop : p.get_province().get_pop_location()) {
					if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
						if(pop.get_pop().get_size() >= minimum) {
							auto amount = int32_t((pop.get_pop().get_size() / divisor) + 1);
							auto regs = pop.get_pop().get_regiment_source();
							auto building = pop.get_pop().get_province_land_construction();
							auto num_to_make = amount - ((regs.end() - regs.begin()) + (building.end() - building.begin()));

							while(num_to_make > 0) {
								auto c = fatten(state.world, state.world.try_create_province_land_construction(pop.get_pop().id, n));
								c.set_type(decide_type());
								--num_to_make;
							}
						}
					}
				}
			}
		}
	}
}

void new_units_and_merging(sys::state& state) {
	for(auto ar : state.world.in_army) {
		auto controller = ar.get_controller_from_army_control();
		if(controller
			&& !controller.get_is_player_controlled()
			&& !ar.get_battle_from_army_battle_participation()
			&& !ar.get_navy_from_army_transport()
			&& !ar.get_arrival_time()) {

			auto location = ar.get_location_from_army_location();

			if(ar.get_black_flag() || army_activity(ar.get_ai_activity()) == army_activity::unspecified) {
				auto regs = ar.get_army_membership();
				if(regs.begin() == regs.end()) {
					// empty army -- cleanup will get it
				} else if(regs.end() - regs.begin() > 1) {
					// existing multi-unit formation
					ar.set_ai_activity(uint8_t(army_activity::on_guard));
				} else {
					bool is_art = state.military_definitions.artillery == (*regs.begin()).get_regiment().get_type();
					dcon::province_id target_location;
					float nearest_distance = 1.0f;

					// find army to merge with
					for(auto o : controller.get_army_control()) {
						auto other_location = o.get_army().get_location_from_army_location();
						auto sdist = province::sorting_distance(state, other_location, location);
						if(army_activity(o.get_army().get_ai_activity()) == army_activity::on_guard
							&& other_location.get_connected_region_id() == location.get_connected_region_id()
							&& (!target_location || sdist < nearest_distance)) {

							int32_t num_support = 0;
							int32_t num_frontline = 0;
							for(auto r : o.get_army().get_army_membership()) {
								auto type = r.get_regiment().get_type();
								auto etype = state.military_definitions.unit_base_definitions[type].type;
								if(etype == military::unit_type::support || etype == military::unit_type::special) {
									++num_support;
								} else {
									++num_frontline;
								}
							}

							if((is_art && num_support < 5) || (!is_art && num_frontline < 5)) {
								target_location = other_location;
								nearest_distance = sdist;
							}
						}
					}

					if(target_location) {
						if(target_location == location) {
							ar.set_ai_province(target_location);
							ar.set_ai_activity(uint8_t(army_activity::merging));
						} else if(auto path = province::make_land_path(state, location, target_location, controller, ar); path.size() > 0) {
							auto existing_path = ar.get_path();
							auto new_size = uint32_t(path.size());
							existing_path.resize(new_size);

							for(uint32_t i = 0; i < new_size; ++i) {
								assert(path[i]);
								existing_path[i] = path[i];
							}
							ar.set_arrival_time(military::arrival_time_to(state, ar, path.back()));
							ar.set_dig_in(0);
							ar.set_ai_province(target_location);
							ar.set_ai_activity(uint8_t(army_activity::merging));
						} else {
							ar.set_ai_activity(uint8_t(army_activity::on_guard));
						}
					} else {
						ar.set_ai_activity(uint8_t(army_activity::on_guard));
					}
				}
			} else if(army_activity(ar.get_ai_activity()) == army_activity::merging) {
				auto regs = ar.get_army_membership();
				if(regs.begin() == regs.end()) {
					// empty army -- cleanup will get it
					continue;
				}
				bool is_art = state.military_definitions.artillery == (*regs.begin()).get_regiment().get_type();
				for(auto o : location.get_army_location()) {
					if(o.get_army().get_ai_activity() == uint8_t(army_activity::on_guard)
						&& o.get_army().get_controller_from_army_control() == controller) {

						int32_t num_support = 0;
						int32_t num_frontline = 0;
						for(auto r : o.get_army().get_army_membership()) {
							auto type = r.get_regiment().get_type();
							auto etype = state.military_definitions.unit_base_definitions[type].type;
							if(etype == military::unit_type::support || etype == military::unit_type::special) {
								++num_support;
							} else {
								++num_frontline;
							}
						}

						if((is_art && num_support < 5) || (!is_art && num_frontline < 5)) {
							(*regs.begin()).get_regiment().set_army_from_army_membership(o.get_army());
							break;
						}
					}
				}
				ar.set_ai_activity(uint8_t(army_activity::unspecified)); // if merging fails, this will try to find a merge target again
			}
		}
	}

}

void general_ai_unit_tick(sys::state& state) {
	auto v = state.current_date.value;
	auto r = v % 8;

	switch(r) {
		case 0:
			pickup_idle_ships(state);
			break;
		case 1:
			move_idle_guards(state);
			break;
		case 2:
			new_units_and_merging(state);
			break;
		case 3:
			move_gathered_attackers(state);
			break;
		case 4:
			update_naval_transport(state);
			break;
		case 5:
			move_idle_guards(state);
			break;
		case 6:
			break;
		case 7:
			move_gathered_attackers(state);
			break;
	}
}

}
