#include "ai_influence.hpp"
#include "economy_stats.hpp"
#include "demographics.hpp"

namespace ai {

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
			gprl.set_status(status & ~nations::influence::priority_mask);
			if((status & nations::influence::level_mask) == nations::influence::level_in_sphere) {
				gprl.set_status(status | nations::influence::priority_one);
			}
		}
	}

	for(auto& n : state.great_nations) {
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

			auto natid = state.world.nation_get_identity_from_identity_holder(n.nation);
			auto holdscores = false;
			for(auto prov_owner : state.world.nation_get_province_ownership(t)) {
				auto prov = prov_owner.get_province();

				for(auto core : prov.get_core_as_province()) {
					if(core.get_identity() == natid) {
						holdscores = true;
						break;
					}
				}

				if(holdscores)
					break;
			}

			if(holdscores) {
				continue; // holds our cores
			}

			float weight = 0.0f;

			for(auto c : state.world.in_commodity) {
				if(auto d = economy::demand(state, n.nation, c); d > 0.001f) {
					auto cweight = std::min(
						1.0f,
						economy::supply(state, n.nation, c) / d)
						* (1.0f - economy::demand_satisfaction(state, n.nation, c));
					weight += cweight;
				}
			}

			//We probably don't want to fight a forever lasting sphere war, let's find some other uncontested nations
			if(t.get_in_sphere_of()) {
				weight /= 4.0f;
			}

			//Prioritize primary culture before culture groups; should ensure Prussia spheres all of the NGF first before trying to contest Austria
			if(t.get_primary_culture() == state.world.nation_get_primary_culture(n.nation)) {
				weight += 1.0f;
				weight *= 4000.0f;
			}

			else if(t.get_primary_culture().get_group_from_culture_group_membership() == state.world.nation_get_primary_culture(n.nation).get_group_from_culture_group_membership()) {
				weight *= 4.0f;
			}
			//Focus on gaining influence against nations we have active wargoals against so we can remove their protector, even if it's us
			if(military::can_use_cb_against(state, n.nation, t) && t.get_in_sphere_of()) {
				weight += 1.0f;
				weight *= 1000.0f;
			}
			//If it doesn't neighbor us or a friendly sphere and isn't coastal, please don't sphere it, we don't want sphere gore
			bool is_reachable = false;
			for(auto adj : state.world.nation_get_nation_adjacency(t)) {
				auto casted_adj = adj.get_connected_nations(0) != t ? adj.get_connected_nations(0) : adj.get_connected_nations(1);
				if(casted_adj == n.nation) {
					is_reachable = true;
					break;
				}
				if(casted_adj.get_in_sphere_of() == n.nation) {
					is_reachable = true;
					break;
				}
			};

			//Is coastal? Technically reachable
			if(state.world.nation_get_central_ports(t) > 0) {
				is_reachable = true;
			}

			//Prefer neighbors
			if(state.world.get_nation_adjacency_by_nation_adjacency_pair(n.nation, t.id)) {
				weight *= 10.0f;
				is_reachable = true;
			}

			if(!is_reachable) {
				weight *= 0.0f;
			}

			targets.push_back(weighted_nation{ t.id, weight });
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
			auto& cur_status = state.world.gp_relationship_get_status(rel);
			state.world.gp_relationship_set_status(rel, cur_status | nations::influence::priority_three);
		}
		for(; i < 4 && i < targets.size(); ++i) {
			auto rel = state.world.get_gp_relationship_by_gp_influence_pair(targets[i].id, n.nation);
			if(!rel)
				rel = state.world.force_create_gp_relationship(targets[i].id, n.nation);
			auto& cur_status = state.world.gp_relationship_get_status(rel);
			state.world.gp_relationship_set_status(rel, cur_status | nations::influence::priority_two);
		}
		for(; i < 6 && i < targets.size(); ++i) {
			auto rel = state.world.get_gp_relationship_by_gp_influence_pair(targets[i].id, n.nation);
			if(!rel)
				rel = state.world.force_create_gp_relationship(targets[i].id, n.nation);
			auto& cur_status = state.world.gp_relationship_get_status(rel);
			state.world.gp_relationship_set_status(rel, cur_status | nations::influence::priority_one);
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
				assert(command::can_increase_opinion(state, gprl.get_great_power(), gprl.get_influence_target()));
				command::execute_increase_opinion(state, gprl.get_great_power(), gprl.get_influence_target());
			} else if(state.defines.removefromsphere_influence_cost <= gprl.get_influence() && current_sphere /* && current_sphere != gprl.get_great_power()*/ && clevel == nations::influence::level_friendly) { // condition taken care of by check above
				assert(command::can_remove_from_sphere(state, gprl.get_great_power(), gprl.get_influence_target(), gprl.get_influence_target().get_in_sphere_of()));
				command::execute_remove_from_sphere(state, gprl.get_great_power(), gprl.get_influence_target(), gprl.get_influence_target().get_in_sphere_of());
			} else if(state.defines.addtosphere_influence_cost <= gprl.get_influence() && !current_sphere && clevel == nations::influence::level_friendly) {
				assert(command::can_add_to_sphere(state, gprl.get_great_power(), gprl.get_influence_target()));
				command::execute_add_to_sphere(state, gprl.get_great_power(), gprl.get_influence_target());
				//De-sphere countries we have wargoals against, desphering countries need to check for going over infamy
			} else if(military::can_use_cb_against(state, gprl.get_great_power(), gprl.get_influence_target())
				&& state.defines.removefromsphere_influence_cost <= gprl.get_influence()
				&& current_sphere
				&& clevel == nations::influence::level_friendly
				&& (state.world.nation_get_infamy(gprl.get_great_power()) + state.defines.removefromsphere_infamy_cost) < state.defines.badboy_limit
			) {
				assert(command::can_remove_from_sphere(state, gprl.get_great_power(), gprl.get_influence_target(), gprl.get_influence_target().get_in_sphere_of()));
				command::execute_remove_from_sphere(state, gprl.get_great_power(), gprl.get_influence_target(), gprl.get_influence_target().get_in_sphere_of());
			}
		}
	}
}
}
