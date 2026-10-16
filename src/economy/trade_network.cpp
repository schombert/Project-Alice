#include "trade_network.hpp"
#include "province.hpp"
#include "province_templates.hpp"
#include "system_state.hpp"
#include <set>

namespace trade_network {

/*
We want to have at the very least two connections as the baseline:

Havana to Port-au-Prince
	This connection represents local trade connections which are important to make smaller and less populated nations playable.
New York to London
	This connection represents more global trade connections between industrial powerhouses with a lot of population.

At the start of the game:

Population of Cuba is ~250k male adults
Population of Haiti ~160k male adults
Distance between Havana and Port-au-Prince is roughly 1'300 km

Population of New York State is ~600k male adults
Population of South East England is ~900k male adults
Distance between New York and London is roughly 7'000 km

But actual connection between London and New York is handled via additional multiplier due to them being "regional trade centers".
So we are free to artificially decrease the target distance by some factor to weed out excess trade routes.
We don't want to have too many trade routes: initial trade routes count should be something like 8k for the starting scenario.
We use an arbitrary value of CLASSIFIER people to distinguish "global" nodes which participate in the global scoring from local nodes.
Population of big nodes is "clamped" down to this value during calculations of local connectivity.

Local connectivity is tricky because distances and population numbers between states vary a lot and we can't just assume certain values.
So for every state we want to find LOCALLY good candidates, which means collection of some stats on local distances to potential candidates.

Explanation of global multiplier:
We want to pursue 2 contradictory goals.
- Avoid rampant growth of trade routes with growth of population.
- Increase world connectivity as the world grows.
Additive factor ensures that there is at least some growth which depends on population growth.
Averaging with world_pop_1836 / world_population (a value which decreases over time) reduces this multiplier as the game goes on.
*/

constexpr float world_pop_1836 = 245'494'464.f;
constexpr float CLASSIFIER_BIG = 500'000.f;
constexpr float london_to_ny = 7'000.f / 2.f;
constexpr float havana_to_pac = 1'600.f;
constexpr float london_pop = 600'000.f;
constexpr float ny_pop = 900'000.f;
constexpr float havana_pop = 250'000.f;
constexpr float pac_pop = 160'000.f;
constexpr float CLASSIFIER_SMALL = std::max(pac_pop, havana_pop);

struct parent_link {
	dcon::market_id leaf;
	dcon::market_id parent;
	float dist;
};

float local_base_trade_distance(sys::state const& state, dcon::state_instance_id origin, dcon::province_id sampled_province, float baseline_population) {
	auto local_base_distance = 20'000.f;
	// gather stats required for local connectivity
	state.world.for_each_state_instance([&](auto sid) {
		if(sid == origin)
			return;
		auto target_capital = state.world.state_instance_get_capital(sid);
		auto distance_approximation = province::direct_distance_km(state, sampled_province, target_capital) + 1.f;
		float score_target = state.world.state_instance_get_demographics(sid, demographics::total);
		if(score_target > baseline_population * 0.75f) {
			local_base_distance = std::min(local_base_distance, distance_approximation);
		}
	});
	return local_base_distance * 2.f;
}

float get_local_average_population(sys::state const& state, dcon::state_instance_id origin, dcon::province_id sampled_province, float baseline_population, float base_distance) {
	auto local_average_population = baseline_population;
	auto local_count = 1.f;
	state.world.for_each_state_instance([&](auto sid) {
		if(sid == origin)
			return;
		auto target_capital = state.world.state_instance_get_capital(sid);
		auto distance_approximation = province::direct_distance_km(state, sampled_province, target_capital) + 1.f;
		float score_target = state.world.state_instance_get_demographics(sid, demographics::total);
		if(distance_approximation < base_distance) {
			local_count += 1.f;
			local_average_population += score_target;
		}
	});
	return std::max(baseline_population, local_average_population / local_count);
}



void recalculate_markets_distance(sys::state& state) {
	state.world.execute_parallel_over_market([&](auto markets) {
		auto sids = state.world.market_get_zone_from_local_market(markets);
		auto population = ve::apply([&](auto sid) {
			return state.world.state_instance_get_demographics(sid, demographics::total);
			}, sids
		);
		auto naval_base = ve::to_float(ve::apply([&](auto sid) {
			return military::state_naval_base_level(state, sid);
			}, sids
		));

		auto railroads = ve::to_float(ve::apply([&](auto sid) {
			return military::state_railroad_level(state, sid);
			}, sids
		));

		auto throughput = 100.f + 8000.f * naval_base + 1000.f * railroads + population / 50.f;

		state.world.market_set_max_throughput(markets, throughput);
	});

	static tagged_vector<fixed_bool_t, dcon::trade_route_id> to_delete;
	to_delete.resize(state.world.trade_route_size());
	std::fill(to_delete.begin(), to_delete.end(), false);

	// distance is calculated in km now
	// speed is accounted for in calculation of transportation supply

	state.world.execute_parallel_over_trade_route([&](auto routes) {
		// recalculate effective distance
		auto markets_0 = state.world.trade_route_get_origin(routes);
		auto markets_1 = state.world.trade_route_get_target(routes);

		auto sids_0 = state.world.market_get_zone_from_local_market(markets_0);
		auto sids_1 = state.world.market_get_zone_from_local_market(markets_1);

		ve::apply([&](auto sid_0, auto sid_1, auto route) {
			if(state.world.trade_route_get_is_sea_route(route)) {
				std::vector<dcon::province_id> path{ };
				dcon::province_id p_prev{ };

				auto coast_0 = province::state_get_coastal_capital(state, sid_0);
				auto coast_1 = province::state_get_coastal_capital(state, sid_1);
				// if no coastal caapital on either one of them, then delete the sea trade route as it dosent make sense to keep
				if(!coast_1 || !coast_0) {
					to_delete[route] = true;
					return;
				}
				auto total_distance_km = sea_trade_route_distance(state, coast_0, coast_1);
				state.world.trade_route_set_distance_km(route, total_distance_km);
			} else {
				std::vector<dcon::province_id> path{ };
				dcon::province_id p_prev{ };

				auto market_0_center = state.world.state_instance_get_capital(sid_0);
				auto market_1_center = state.world.state_instance_get_capital(sid_1);
				path = province::make_land_trade_path(state, market_0_center, market_1_center);

				p_prev = market_0_center;

				auto ps = path.size();
				auto total_distance_km = 0.f;
				auto worst_movement_cost = 0.f;
				auto min_railroad_level = 0.f;

				for(size_t i = 0; i < ps; i++) {
					auto p_current = path[i];
					auto adj = state.world.get_province_adjacency_by_province_pair(p_prev, p_current);
					auto bits = state.world.province_adjacency_get_type(adj);

					float distance = province::distance_km(state, adj);
					float sum_mods =
						state.world.province_get_modifier_values(p_current, sys::provincial_mod_offsets::movement_cost)
						+ state.world.province_get_modifier_values(p_prev, sys::provincial_mod_offsets::movement_cost);

					// no magical speed-up... for now
					float local_terrain_mod = std::max(0.f, sum_mods / 2.f - 1.f);

					// rivers ignore terrain
					if(bits & province::border::river_connection_bit) {
						local_terrain_mod = 0.f;
					}

					float local_distance_km = distance * std::max(0.1f, (1.f + local_terrain_mod));

					total_distance_km += local_distance_km;

					p_prev = p_current;
				}

				state.world.trade_route_set_distance_km(route, total_distance_km);
			}
		}, sids_0, sids_1, routes);
	});
	// delete marked trade routes
	for(auto i = state.world.trade_route_size(); i-- > 0;) {
		dcon::trade_route_id trade_route{ dcon::trade_route_id::value_base_t(i) };
		if(to_delete[trade_route]) {
			state.world.delete_trade_route(trade_route);
		}
	}
}

float sea_trade_route_distance(sys::state const& state, dcon::province_id a, dcon::province_id b) {
	std::vector<dcon::province_id> path{ };
	dcon::province_id p_prev{ };
	path = province::make_sea_trade_route_path(state, a, b);
	auto ps = path.size();
	auto distance_km = 1.f;
	p_prev = a;
	for(int i = int(path.size()) - 1; i >= 0; i--) {
		auto p_current = path[i];
		auto adj = state.world.get_province_adjacency_by_province_pair(p_prev, p_current);
		// sea routes ignore modifiers
		float local_distance = province::distance_km(state, adj);
		distance_km += local_distance;
		p_prev = p_current;
	}
	return distance_km;
}

trade_candidate_information process_trade_partner_candidate(
	sys::state const& state, sea_trade_candidate_data origin, sea_trade_candidate_data target,
	float local_base_distance, float local_base_population, float global_mult
) {
	trade_candidate_information result {};
	if(target.state_instance == origin.state_instance) {
		result.reason = candidate_failure_reason::same_state;
		return result;
	}
	if(!target.coast) {
		result.reason = candidate_failure_reason::not_coastal;
		return result;
	}

	bool route_already_exists = false;
	state.world.market_for_each_trade_route_as_origin(origin.market, [&](auto route) {
		auto route_destination = state.world.trade_route_get_target(route);
		auto route_owner = state.world.trade_route_get_owner(route);
		auto sea_route = state.world.trade_route_get_is_sea_route(route);
		if(route_destination == target.market && route_owner == origin.market && sea_route) {
			route_already_exists = true;
		}
	});
	if(route_already_exists) {
		result.reason = candidate_failure_reason::already_exists;
		return result;
	}

	result.potentially_valid =  true;
	
	float mult = 1.f;
	mult += std::min(origin.naval_base_level, target.naval_base_level) * naval_base_level_to_market_attractiveness;
	auto distance_approximation = province::direct_distance_km(state, origin.coast, target.coast) + 1.f;

	float score_origin = origin.population;
	float score_target = target.population;

	auto approx_big_score_distance = std::min(1.f, london_to_ny / distance_approximation);
	auto approx_small_score_distance = local_base_distance / distance_approximation;

	score_origin *= global_mult;
	score_target *= global_mult;

	auto pop_score_origin_big = std::min(4.f, (score_origin > CLASSIFIER_BIG ? score_origin : 0.f) / london_pop);
	auto pop_score_target_big = std::min(4.f, (score_target > CLASSIFIER_BIG ? score_target : 0.f) / ny_pop);

	auto pop_score_origin_small = std::min(2.f, score_origin / local_base_population);
	auto pop_score_target_small = std::min(2.f, score_target / local_base_population);

	float score_big_approx = (mult * pop_score_origin_big * (0.001f + pop_score_target_big)) * approx_big_score_distance;
	float score_small_approx = (mult * pop_score_origin_small * (0.001f + pop_score_target_small)) * approx_small_score_distance * approx_small_score_distance * approx_small_score_distance;

	result.approx_big_score = score_big_approx;
	result.approx_score = score_small_approx;

	if(!(score_big_approx >= 1.f || score_small_approx >= 1.f)) {
		result.reason = candidate_failure_reason::low_approximate_score;
		return result;
	}

	auto distance_km = sea_trade_route_distance(state, origin.coast, target.coast);
	auto big_score_distance = std::min(1.f, london_to_ny / distance_km);
	auto small_score_distance = local_base_distance / distance_km;
	float score_big = (mult * pop_score_origin_big * (0.001f + pop_score_target_big)) * big_score_distance;
	float score_small = (mult * pop_score_origin_small * (0.001f + pop_score_target_small)) * small_score_distance * small_score_distance * small_score_distance;

	result.big_score = score_big;
	result.score = score_small;

	if(result.big_score <= 1.f && result.score <= 1.f) {
		result.reason = candidate_failure_reason::none;
	}

	return result;
}


void generate_sea_trade_routes(sys::state& state) {
	// buffer for "capitals" of connected regions:
	std::array<dcon::state_instance_id, 4000> capital_of_region = {};
	std::array<float, 10000> population_of_region = { };
	std::vector<sea_trade_candidate_data> candidates { };

	state.world.for_each_state_instance([&](auto candidate) {
		// auto capital = state.world.state_instance_get_capital(candidate);
		auto capital = province::state_get_coastal_capital(state, candidate);
		if(capital) {
			auto connected_region = state.world.province_get_connected_coast_id(capital);
			auto size = state.world.state_instance_get_demographics(
				candidate, demographics::total
			);

			population_of_region[connected_region] += size;

			if(!capital_of_region[connected_region]) {
				capital_of_region[connected_region] = candidate;
			} else {
				auto current_population = state.world.state_instance_get_demographics(
					capital_of_region[connected_region], demographics::total
				);
				if(size > current_population) {
					capital_of_region[connected_region] = candidate;
				}
			}
		}
	});

	float world_population = 0.f;
	state.world.for_each_nation([&](auto nation) {
		world_population += state.world.nation_get_demographics(nation, demographics::total);
	});
	//state.console_log("WORLD POP : " + std::to_string(world_population));
	const float global_multiplier = 0.1f + 0.9f * world_pop_1836 / world_population;


	state.world.for_each_state_instance([&](auto candidate) {
		sea_trade_candidate_data data { };
		data.coast = province::state_get_coastal_capital(state, candidate);
		data.market = state.world.state_instance_get_market_from_local_market(candidate);
		data.naval_base_level = (float)(military::state_naval_base_level(state, candidate));
		data.owner = state.world.state_instance_get_nation_from_state_ownership(candidate);
		data.population = state.world.state_instance_get_demographics(candidate, demographics::total);
		data.state_instance = candidate;

		if(!data.coast) {
			return;
		}

		data.connected_coastal_region = state.world.province_get_connected_coast_id(data.coast);
		if(data.state_instance == capital_of_region[data.connected_coastal_region]) {
			auto region_population = population_of_region[data.connected_coastal_region];
			data.population = std::max(data.population, region_population * 0.3f);
		}

		candidates.push_back(data);
	});

	for(auto& origin : candidates) {
		auto state_owner_capital = state.world.nation_get_capital(origin.owner);
		auto state_owner_capital_state = state.world.province_get_state_membership(state_owner_capital);
		auto local_base_distance = local_base_trade_distance(state, origin.state_instance, origin.coast, origin.population);
		auto local_average_population = get_local_average_population(state, origin.state_instance, origin.coast, origin.population, local_base_distance);

		for(auto& target : candidates) {
			auto origin_is_regional_capital = capital_of_region[origin.connected_coastal_region] == origin.state_instance;
			auto target_is_regional_capital = capital_of_region[target.connected_coastal_region] == target.state_instance;
			auto origin_is_capital = state_owner_capital_state == origin.state_instance;
			auto target_is_capital = state_owner_capital_state == target.state_instance;

			auto capital_vs_region = origin_is_capital && target_is_regional_capital;
			auto region_vs_capital = target_is_capital && origin_is_regional_capital;
			auto capital_and_connected_region = capital_vs_region || region_vs_capital;

			bool same_owner = target.owner == origin.owner;
			bool different_region = origin.connected_coastal_region != target.connected_coastal_region;
			bool must_connect = same_owner && different_region && capital_and_connected_region;
			auto score = process_trade_partner_candidate(state, origin, target, local_base_distance, local_average_population, global_multiplier);
			must_connect = must_connect || score.score >= 1.f || score.big_score >= 1.f;

			if(must_connect && score.potentially_valid) {
				{
					auto new_route = state.world.force_create_trade_route(origin.market, target.market);
					state.world.trade_route_set_is_sea_route(new_route, true);
					state.world.trade_route_set_owner(new_route, origin.market);
				}
				{
					auto opposite_route = state.world.force_create_trade_route(target.market, origin.market);
					state.world.trade_route_set_is_sea_route(opposite_route, true);
					state.world.trade_route_set_owner(opposite_route, origin.market);
				}
			}
		}
	}

	std::vector<float> nation_to_max_population = { };
	nation_to_max_population.resize(state.world.nation_size());

	state.world.for_each_state_instance([&](auto candidate) {
		auto capital = province::state_get_coastal_capital(state, candidate);
		if(capital) {
			auto connected_region = state.world.province_get_connected_coast_id(capital);
			auto owner = state.world.province_get_nation_from_province_ownership(capital);
			auto size = population_of_region[connected_region];
			nation_to_max_population[owner.index()] = std::max(nation_to_max_population[owner.index()], size);
		}
	});

	// connect to each other coastal connectivity components:
	std::vector<parent_link> best_parent;
	std::vector<bool> parent_found;
	parent_found.resize(state.world.market_size());

	state.world.for_each_state_instance([&](auto origin) {
		if(!province::state_is_coastal(state, origin))
			return;
		auto origin_market = state.world.state_instance_get_market_from_local_market(origin);
		auto origin_capital = state.world.state_instance_get_capital(origin);
		auto origin_owner = state.world.state_instance_get_nation_from_state_ownership(origin);
		auto origin_connected_region = state.world.province_get_connected_coast_id(origin_capital);
		auto origin_connected_region_population = population_of_region[origin_connected_region];
		if(origin != capital_of_region[origin_connected_region]) {
			return;
		}

		auto origin_coast = province::state_get_coastal_capital(state, origin);

		parent_found[origin_market.index()] = false;

		bool origin_is_major_node = origin_connected_region_population > 0.7f * nation_to_max_population[origin_owner.index()];

		state.world.for_each_state_instance([&](auto target) {
			if(origin == target) {
				return;
			}
			if(!province::state_is_coastal(state, target))
				return;
			auto target_market = state.world.state_instance_get_market_from_local_market(target);
			auto target_capital = state.world.state_instance_get_capital(target);
			auto target_owner = state.world.state_instance_get_nation_from_state_ownership(target);
			auto target_coast = province::state_get_coastal_capital(state, target);
			auto target_connected_region = state.world.province_get_connected_coast_id(target_coast);
			auto target_connected_region_population = population_of_region[target_connected_region];

			if(target != capital_of_region[target_connected_region]) {
				return;
			}

			if(target_owner != origin_owner) {
				return;
			}

			bool route_already_exists = false;
			state.world.market_for_each_trade_route_as_origin(origin_market, [&](auto route) {
				auto target = state.world.trade_route_get_target(route);
				//auto owner = state.world.trade_route_get_owner(route);
				//auto sea_route = state.world.trade_route_get_is_sea_route(route);
				if(target == target_market) {
					route_already_exists = true;
				}
			});
			if(route_already_exists) {
				return;
			}

			bool target_is_major_node = target_connected_region_population > 0.7f * nation_to_max_population[target_owner.index()];

			if(origin_is_major_node && target_is_major_node) {
				auto new_route_to = state.world.force_create_trade_route(origin_market, target_market);
				auto new_route_from = state.world.force_create_trade_route(target_market, origin_market);
				state.world.trade_route_set_is_sea_route(new_route_to, true);
				state.world.trade_route_set_is_sea_route(new_route_from, true);
				state.world.trade_route_set_owner(new_route_to, origin_market);
				state.world.trade_route_set_owner(new_route_from, origin_market);
				return;
			}

			if(origin_is_major_node) {
				best_parent.push_back({
					target_market,
					origin_market,
					province::direct_distance(state, target_capital, origin_capital)
				});
				return;
			}

			if(target_is_major_node) {
				best_parent.push_back({
					origin_market,
					target_market,
					province::direct_distance(state, target_capital, origin_capital)
				});
				return;
			}
		});
	});

	std::sort(best_parent.begin(), best_parent.end(), [&](parent_link& a, parent_link& b) {
		if(a.dist < b.dist) {
			return true;
		}
		if(a.dist > b.dist) {
			return false;
		}

		return (a.leaf.index() < b.leaf.index());
	});

	for(unsigned i = 0; i < best_parent.size(); i++) {
		if(parent_found[best_parent[i].leaf.index()]) {
			continue;
		}

		parent_found[best_parent[i].leaf.index()] = true;

		auto new_route_to = state.world.force_create_trade_route(best_parent[i].leaf, best_parent[i].parent);
		auto new_route_from = state.world.force_create_trade_route(best_parent[i].parent, best_parent[i].leaf);
		state.world.trade_route_set_is_sea_route(new_route_to, true);
		state.world.trade_route_set_is_sea_route(new_route_from, true);
		state.world.trade_route_set_owner(new_route_to, best_parent[i].parent);
		state.world.trade_route_set_owner(new_route_from, best_parent[i].parent);
	}
}

void generate_initial_trade_routes(sys::state& state) {
	// land trade routes
	state.world.for_each_state_instance([&](auto sid) {
		auto owner = state.world.state_instance_get_nation_from_state_ownership(sid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		std::set<dcon::state_instance_id::value_base_t> trade_route_candidates;

		province::for_each_province_in_state_instance(state, sid, [&](auto prov) {
			// try to create trade routes to neighbors
			for(auto adj : state.world.province_get_province_adjacency(prov)) {
				if((adj.get_type() & (province::border::impassible_bit | province::border::coastal_bit)) == 0) {
					auto other =
						adj.get_connected_provinces(0) != prov
						? adj.get_connected_provinces(0)
						: adj.get_connected_provinces(1);

					if(!other.get_state_membership())
						continue;
					if(other.get_state_membership() == sid)
						continue;
					if(trade_route_candidates.contains(other.get_state_membership().id.value))
						continue;

					trade_route_candidates.insert(other.get_state_membership().id.value);
				}
			}

			/*
			Create trade routes through lakes.
			Lake coasts have both impassible bit and coastal bit
			*/
			for(auto adj : state.world.province_get_province_adjacency(prov)) {
				auto bits = adj.get_type();
				if((bits & province::border::impassible_bit) == 0) {
					continue;
				}
				if((bits & province::border::coastal_bit) == 0) {
					continue;
				}

				auto through =
					adj.get_connected_provinces(0) != prov
					? adj.get_connected_provinces(0)
					: adj.get_connected_provinces(1);

				auto area = state.map_state.map_data.province_area_km2[province::to_map_id(through)];
				// larger than the caspian sea
				if(area > 500'000.f) {
					continue;
				}

				for(auto adj2 : state.world.province_get_province_adjacency(through)) {
					auto bits2 = adj2.get_type();
					if((bits2 & province::border::impassible_bit) == 0) {
						continue;
					}
					if((bits2 & province::border::coastal_bit) == 0) {
						continue;
					}
					auto other =
						adj2.get_connected_provinces(0) != through
						? adj2.get_connected_provinces(0)
						: adj2.get_connected_provinces(1);
					if(!other.get_state_membership())
						continue;
					if(other.get_state_membership() == sid)
						continue;
					if(trade_route_candidates.contains(other.get_state_membership().id.value))
						continue;

					trade_route_candidates.insert(other.get_state_membership().id.value);
				}
			}
		});

		for(auto candidate_trade_partner_val : trade_route_candidates) {
			auto si = dcon::state_instance_id{ uint16_t(candidate_trade_partner_val - 1) };
			auto target_market = state.world.state_instance_get_market_from_local_market(si);
			auto new_route = state.world.force_create_trade_route(market, target_market);
			auto opposite_route = state.world.force_create_trade_route(target_market, market);
			state.world.trade_route_set_is_sea_route(new_route, false);
			state.world.trade_route_set_is_sea_route(opposite_route, false);
			state.world.trade_route_set_owner(new_route, market);
			state.world.trade_route_set_owner(opposite_route, market);
		}
	});

	generate_sea_trade_routes(state);

	recalculate_markets_distance(state);
}


}
