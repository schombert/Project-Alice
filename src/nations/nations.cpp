#include "nations.hpp"
#include "nations_templates.hpp"
#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "modifiers.hpp"
#include "politics.hpp"
#include "system_state.hpp"
#include "container_types.hpp"
#include "ve_scalar_extensions.hpp"
#include "triggers.hpp"
#include "politics.hpp"
#include "events.hpp"
#include "prng.hpp"
#include "effects.hpp"
#include "province_templates.hpp"
#include "rebels.hpp"
#include "set"
#include "economy_government.hpp"
#include "economy_production.hpp"
#include "economy_stats.hpp"

namespace nations {

constexpr float max_prestige = 2147483648.f;

namespace influence {

int32_t get_level(sys::state& state, dcon::nation_id gp, dcon::nation_id target) {
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(target, gp);
	return state.world.gp_relationship_get_status(rel) & influence::level_mask;
}

} // namespace influence

template auto nation_accepts_culture<ve::tagged_vector<dcon::nation_id>, dcon::culture_id>(sys::state const&, ve::tagged_vector<dcon::nation_id>, dcon::culture_id);
template auto primary_culture_group<ve::tagged_vector<dcon::nation_id>>(sys::state const&, ve::tagged_vector<dcon::nation_id>);
template auto owner_of_pop<ve::tagged_vector<dcon::pop_id>>(sys::state const&, ve::tagged_vector<dcon::pop_id>);
template auto central_blockaded_fraction<ve::tagged_vector<dcon::nation_id>>(sys::state const&, ve::tagged_vector<dcon::nation_id>);
template auto central_reb_controlled_fraction<ve::tagged_vector<dcon::nation_id>>(sys::state const&, ve::tagged_vector<dcon::nation_id>);
template auto central_has_crime_fraction<ve::tagged_vector<dcon::nation_id>>(sys::state const&, ve::tagged_vector<dcon::nation_id>);
template auto occupied_provinces_fraction<ve::tagged_vector<dcon::nation_id>>(sys::state const&, ve::tagged_vector<dcon::nation_id>);

std::vector<dcon::nation_id> nation_get_subjects(sys::state& state, dcon::nation_id n) {
	std::vector<dcon::nation_id> subjects;
	for(auto s : state.world.in_nation) {
		auto rel = state.world.nation_get_overlord_as_subject(s);
		auto overlord = state.world.overlord_get_ruler(rel);

		if(overlord != n) {
			continue;
		}

		subjects.push_back(s);
	}

	return subjects;
}

int64_t get_monthly_pop_increase_of_nation(sys::state& state, dcon::nation_id n) {
	/* TODO -
	 * This should return the differance of the population of a nation between this month and next month, or this month and last
	 * month, depending which one is better to implement Used in gui/topbar_subwindows/gui_population_window.hpp - Return value is
	 * divided by 30
	 */
	int64_t estimated_change = 0;
	for(auto p : state.world.nation_get_province_ownership(n)) {
		for(auto pl : state.world.province_get_pop_location(p.get_province())) {
			auto growth = int64_t(demographics::get_monthly_pop_increase(state, pl.get_pop()));
			auto colonial_migration = -int64_t(demographics::get_estimated_colonial_migration(state, pl.get_pop()));
			auto emigration = -int64_t(demographics::get_estimated_emigration(state, pl.get_pop()));
			auto total = int64_t(growth) + colonial_migration + emigration;
			estimated_change += total;
		}
	}
	return estimated_change;
}

dcon::nation_id get_nth_great_power(sys::state const& state, uint16_t n) {
	uint16_t count = 0;
	for(uint16_t i = 0; i < uint16_t(state.nations_by_rank.size()); ++i) {
		if(is_great_power(state, state.nations_by_rank[i])) {
			if(count == n)
				return state.nations_by_rank[i];
			++count;
		}
	}
	return dcon::nation_id{};
}

dcon::nation_id owner_of_pop(sys::state const& state, dcon::pop_id pop_ids) {
	auto location = state.world.pop_get_province_from_pop_location(pop_ids);
	return state.world.province_get_nation_from_province_ownership(location);
}

void restore_state_instances(sys::state& state) {
	state.world.for_each_state_instance([&](dcon::state_instance_id sid) {
		auto owner = state.world.state_instance_get_nation_from_state_ownership(sid);
		auto base_state = state.world.state_instance_get_definition(sid);
		for(auto mprov : state.world.state_definition_get_abstract_state_membership(base_state)) {
			if(mprov.get_province().get_nation_from_province_ownership() == owner) {
				mprov.get_province().set_state_membership(sid);
			}
		}
	});
}

void restore_cached_values(sys::state& state) {

	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_allies_count(ids, ve::int_vector()); });
	state.world.for_each_diplomatic_relation([&](dcon::diplomatic_relation_id id) {
		if(state.world.diplomatic_relation_get_are_allied(id)) {
			auto related_nation_0 = state.world.diplomatic_relation_get_related_nations(id, 0);
			auto& allies_count_0 = state.world.nation_get_allies_count(related_nation_0);
			state.world.nation_set_allies_count(related_nation_0, uint16_t(allies_count_0 + uint16_t(1)));

			auto related_nation_1 = state.world.diplomatic_relation_get_related_nations(id, 1);
			auto& allies_count_1 = state.world.nation_get_allies_count(related_nation_1);
			state.world.nation_set_allies_count(related_nation_1, uint16_t(allies_count_1 + uint16_t(1)));
		}
	});

	state.world.for_each_nation([&](dcon::nation_id n) {
		int32_t total = 0;
		int32_t substates_total = 0;
		for(auto v : state.world.nation_get_overlord_as_ruler(n)) {
			++total;
			if(v.get_subject().get_is_substate())
				++substates_total;
		}
		state.world.nation_set_vassals_count(n, uint16_t(total));
		state.world.nation_set_substates_count(n, uint16_t(substates_total));
	});
}

void update_cached_values(sys::state& state) {
	if(state.diplomatic_cached_values_out_of_date) {
		state.diplomatic_cached_values_out_of_date = false;
		restore_cached_values(state);
	}
}

void restore_unsaved_values(sys::state& state) {
	state.world.market_resize_demand_satisfaction(state.world.commodity_size());
	state.world.market_resize_supply_sold_ratio(state.world.commodity_size());
	state.world.market_resize_direct_demand_satisfaction(state.world.commodity_size());

	for(auto n : state.world.in_nation)
		n.set_is_great_power(false);

	for(auto& gp : state.great_nations) {
		state.world.nation_set_is_great_power(gp.nation, true);
	}

	state.world.for_each_gp_relationship([&](dcon::gp_relationship_id rel) {
		if((influence::level_mask & state.world.gp_relationship_get_status(rel)) == influence::level_in_sphere) {
			auto t = state.world.gp_relationship_get_influence_target(rel);
			auto gp = state.world.gp_relationship_get_great_power(rel);
			state.world.nation_set_in_sphere_of(t, gp);
		}
	});

	state.world.execute_serial_over_nation([&](auto ids) {
		auto treasury = state.world.nation_get_stockpiles(ids, economy::money);
		state.world.nation_set_last_treasury(ids, treasury);
	});

	restore_cached_values(state);
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

	state.world.execute_parallel_over_trade_route([&](auto routes) {
		// recalculate effective distance
		auto markets_0 = ve::apply([&](auto route) { return state.world.trade_route_get_connected_markets(route, 0); }, routes);
		auto markets_1 = ve::apply([&](auto route) { return state.world.trade_route_get_connected_markets(route, 1); }, routes);

		auto sids_0 = state.world.market_get_zone_from_local_market(markets_0);
		auto sids_1 = state.world.market_get_zone_from_local_market(markets_1);

		ve::apply([&](auto sid_0, auto sid_1, auto route) {
			if (state.world.trade_route_get_is_sea_route(route)) {
				std::vector<dcon::province_id> path{ };
				dcon::province_id p_prev{ };

				auto coast_0 = province::state_get_coastal_capital(state, sid_0);
				auto coast_1 = province::state_get_coastal_capital(state, sid_1);
				auto owner_0 = state.world.province_get_nation_from_province_ownership(coast_0);
				auto owner_1 = state.world.province_get_nation_from_province_ownership( coast_1);
				auto transport_0 = military::get_best_transport(state, owner_0, false, false);
				auto transport_1 = military::get_best_transport(state, owner_1, false, false);
				auto stats_0 = state.world.nation_get_unit_stats(owner_0, transport_0);
				auto stats_1 = state.world.nation_get_unit_stats(owner_1, transport_1);

				auto speed = std::max(1.f, std::max(stats_0.maximum_speed, stats_1.maximum_speed));

				path = province::make_unowned_naval_path(state, coast_0, coast_1);
				p_prev = coast_0;

				auto ps = path.size();
				auto effective_distance = 0.f;
				auto worst_movement_cost = 0.f;

				for(size_t i = 0; i < ps; i++) {
					auto p_current = path[i];
					auto adj = state.world.get_province_adjacency_by_province_pair(p_prev, p_current);
					float distance = province::distance(state, adj);
					float sum_mods =
						state.world.province_get_modifier_values(p_current, sys::provincial_mod_offsets::movement_cost)
						+ state.world.province_get_modifier_values(p_prev, sys::provincial_mod_offsets::movement_cost);
					effective_distance += std::max(0.01f, distance * std::max(0.01f, (sum_mods * 2.f + 1.0f)));
					if(sum_mods > worst_movement_cost)
						worst_movement_cost = std::max(0.01f, sum_mods);

					p_prev = p_current;
				}

				if(effective_distance == 0.f) {
					// no path, remove sea connection
					state.world.trade_route_set_sea_distance(route, 99999.f);
				} else {
					assert(speed > 0.f);
					state.world.trade_route_set_sea_distance(route, effective_distance / speed);
				}

			} else {
				state.world.trade_route_set_sea_distance(route, 99999.f);
			}

			if(state.world.trade_route_get_is_land_route(route)) {
				std::vector<dcon::province_id> path{ };
				dcon::province_id p_prev{ };

				auto market_0_center = state.world.state_instance_get_capital(sid_0);
				auto market_1_center = state.world.state_instance_get_capital(sid_1);
				path = province::make_unowned_path(state, market_0_center, market_1_center);

				auto owner_0 = state.world.province_get_nation_from_province_ownership(market_0_center);
				auto owner_1 = state.world.province_get_nation_from_province_ownership(market_1_center);
				auto cav_0 = military::get_best_cavalry(state, owner_0, false, false);
				auto cav_1 = military::get_best_cavalry(state, owner_1, false, false);
				auto stats_0 = state.world.nation_get_unit_stats(owner_0, cav_0);
				auto stats_1 = state.world.nation_get_unit_stats(owner_1, cav_1);

				auto speed = std::max(1.f, std::max(stats_0.maximum_speed, stats_1.maximum_speed));
				p_prev = market_0_center;

				auto ps = path.size();
				auto effective_distance = 0.f;
				auto worst_movement_cost = 0.f;
				auto min_railroad_level = 0.f;

				for(size_t i = 0; i < ps; i++) {
					auto p_current = path[i];
					auto adj = state.world.get_province_adjacency_by_province_pair(p_prev, p_current);
					float distance = province::distance(state, adj);
					float sum_mods =
						state.world.province_get_modifier_values(p_current, sys::provincial_mod_offsets::movement_cost)
						+ state.world.province_get_modifier_values(p_prev, sys::provincial_mod_offsets::movement_cost);
					float local_effective_distance = distance * std::max(0.01f, sum_mods * 3.f);
					auto railroad_origin = state.world.province_get_building_level(p_prev, uint8_t(economy::province_building_type::railroad));
					auto railroad_target = state.world.province_get_building_level(p_current, uint8_t(economy::province_building_type::railroad));
					if(railroad_origin > 0 && railroad_target > 0) {
						local_effective_distance = local_effective_distance / 2.f;
					}
					local_effective_distance -= 0.03f * std::min(railroad_target, railroad_origin) * local_effective_distance;
					effective_distance += std::max(0.01f, local_effective_distance);
					if(sum_mods > worst_movement_cost)
						worst_movement_cost = std::max(0.01f, sum_mods);

					p_prev = p_current;
				}

				if(effective_distance == 0.f) {
					// no path, remove sea connection
					state.world.trade_route_set_land_distance(route, 99999.f);
				} else {
					assert(speed > 0.f);
					state.world.trade_route_set_land_distance(route, effective_distance / speed);
				}

				state.world.trade_route_set_land_distance(route, effective_distance / speed);
			} else {
				state.world.trade_route_set_land_distance(route, 99999.f);
			}
		}, sids_0, sids_1, routes);
	});
}

struct parent_link {
	dcon::market_id leaf;
	dcon::market_id parent;
	float dist;
};

void generate_sea_trade_routes(sys::state& state) {
	float total_transport_speed = 0.f;
	float total_amount_of_transports = 0.f;

	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id j{ dcon::unit_type_id::value_base_t(i) };
		if(state.military_definitions.unit_base_definitions[j].type == military::unit_type::transport) {
			total_transport_speed += state.military_definitions.unit_base_definitions[j].maximum_speed;
			total_amount_of_transports += 1.f;
		}
	}

	auto base_speed = total_transport_speed / total_amount_of_transports;

	// buffer for "capitals" of connected regions:
	std::array<dcon::state_instance_id, 4000> capital_of_region = {};
	std::array<float, 10000> population_of_region = { };
	std::vector<float> nation_to_max_population = { };
	nation_to_max_population.resize(state.world.nation_size());

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

	state.world.for_each_state_instance([&](auto candidate) {
		auto capital = province::state_get_coastal_capital(state, candidate);
		if(capital) {
			auto connected_region = state.world.province_get_connected_coast_id(capital);
			auto owner = state.world.province_get_nation_from_province_ownership(capital);
			auto size = population_of_region[connected_region];
			nation_to_max_population[owner.index()] = std::max(nation_to_max_population[owner.index()], size);
		}
	});

	// baseline: connection between new york and london

	constexpr float M = 0.17f * 0.000'000'1f;

	float world_population = 0.f;
	state.world.for_each_nation([&](auto nation) {
		world_population += state.world.nation_get_demographics(nation, demographics::total);
	});

	state.world.for_each_state_instance([&](auto origin) {
		if(!province::state_is_coastal(state, origin))
			return;

		auto market = state.world.state_instance_get_market_from_local_market(origin);

		auto capital = state.world.state_instance_get_capital(origin);
		auto owner = state.world.state_instance_get_nation_from_state_ownership(origin);
		auto state_owner_capital = state.world.nation_get_capital(owner);
		auto state_owner_capital_state = state.world.province_get_state_membership(state_owner_capital);

		auto naval_base_origin = military::state_naval_base_level(state, origin);
		auto population_origin = state.world.state_instance_get_demographics(origin, demographics::total);
		auto coast_0 = province::state_get_coastal_capital(state, origin);
		auto connected_region = state.world.province_get_connected_coast_id(coast_0);
		auto connected_region_size = 0.f;

		state.world.for_each_state_instance([&](auto sid) {
			if(sid == origin)
				return;
			if(!province::state_is_coastal(state, sid))
				return;

			auto coast_1 = province::state_get_coastal_capital(state, sid);

			auto target_market = state.world.state_instance_get_market_from_local_market(sid);
			auto route = state.world.get_trade_route_by_province_pair(market, target_market);
			if(route) {
				state.world.trade_route_set_is_sea_route(route, true);
				return;
			}

			bool same_owner = false;
			bool different_region = false;
			bool capital_and_connected_region = false;

			auto target_owner = state.world.state_instance_get_nation_from_state_ownership(sid);
			if(target_owner == owner) {
				same_owner = true;
			}

			auto capital_target = state.world.state_instance_get_capital(sid);
			auto connected_region_target = state.world.province_get_connected_coast_id(coast_1);
			if(connected_region != connected_region_target) {
				different_region = true;
			}

			if(
				(
					capital_of_region[connected_region_target] == sid
					&&
					state_owner_capital_state == origin
				)
				||
				(
					state_owner_capital_state == sid
					&&
					capital_of_region[connected_region] == origin
				)
			) {
				capital_and_connected_region = true;
			}

			auto naval_base_target = military::state_naval_base_level(state, sid);
			auto state_target_owner_capital = state.world.nation_get_capital(target_owner);
			auto state_target_owner_capital_state = state.world.province_get_state_membership(state_target_owner_capital);
			auto continent_target = state.world.province_get_continent(coast_1);
			auto continent_origin = state.world.province_get_continent(state_owner_capital);

			float mult = 1.f;
			mult += std::min(naval_base_origin, naval_base_target) * naval_base_level_to_market_attractiveness;
			bool must_connect = same_owner && different_region && capital_and_connected_region;
			
			auto distance_approximation = province::direct_distance(state, coast_0, coast_1) / base_speed;


			float score_origin = population_origin;
			float score_target = state.world.state_instance_get_demographics(sid, demographics::total);
			if(capital_of_region[connected_region_target] == sid && capital_of_region[connected_region] == origin) {
				score_origin = population_of_region[connected_region];
				score_target = population_of_region[connected_region_target];
				mult *= 20.f;
			}

			float score_approximation = mult * M * score_origin * score_target / distance_approximation / distance_approximation / distance_approximation;

			if(!(score_approximation >= 1.f || must_connect)) {
				return;
			}

			auto distance = 9999.f;
			{
				std::vector<dcon::province_id> path{ };
				auto speed = base_speed;
				dcon::province_id p_prev{ };
				path = province::make_unowned_naval_path(state, coast_0, coast_1);
				p_prev = coast_0;

				auto ps = path.size();
				auto effective_distance = 0.f;
				auto worst_movement_cost = 0.f;

				for(size_t i = 0; i < ps; i++) {
					auto p_current = path[i];
					auto adj = state.world.get_province_adjacency_by_province_pair(p_prev, p_current);
					float local_distance = province::distance(state, adj);
					float sum_mods =
						state.world.province_get_modifier_values(p_current, sys::provincial_mod_offsets::movement_cost)
						+ state.world.province_get_modifier_values(p_prev, sys::provincial_mod_offsets::movement_cost);
					effective_distance += std::max(0.01f, local_distance * std::max(0.01f, (sum_mods * 2.f + 1.0f)));
					if(sum_mods > worst_movement_cost)
						worst_movement_cost = std::max(0.01f, sum_mods);

					p_prev = p_current;
				}
				distance = effective_distance / speed;
			}

			float score = mult * M * score_origin * score_target / distance / distance / distance;

			if(score >= 1.f || must_connect) {
				auto new_route = state.world.force_create_trade_route(market, target_market);
				state.world.trade_route_set_is_sea_route(new_route, true);
			}
		});
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


			auto route = state.world.get_trade_route_by_province_pair(origin_market, target_market);
			if(route) {
				state.world.trade_route_set_is_sea_route(route, true);
				return;
			}

			bool target_is_major_node = target_connected_region_population > 0.7f * nation_to_max_population[target_owner.index()];

			if(origin_is_major_node && target_is_major_node) {
				auto new_route = state.world.force_create_trade_route(origin_market, target_market);
				state.world.trade_route_set_is_sea_route(new_route, true);
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

		auto new_route = state.world.force_create_trade_route(best_parent[i].leaf, best_parent[i].parent);
		state.world.trade_route_set_is_sea_route(new_route, true);
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
		});

		for(auto candidate_trade_partner_val : trade_route_candidates) {
			auto si = dcon::state_instance_id{ uint16_t(candidate_trade_partner_val - 1) };
			auto target_market = state.world.state_instance_get_market_from_local_market(si);
			auto new_route = state.world.force_create_trade_route(market, target_market);

			if(province::state_is_coastal(state, sid) && province::state_is_coastal(state, si)) {
				state.world.trade_route_set_is_sea_route(new_route, true);
			}
			state.world.trade_route_set_is_land_route(new_route, true);
		}
	});

	generate_sea_trade_routes(state);

	recalculate_markets_distance(state);
}

void generate_initial_state_instances(sys::state& state) {
	for(int32_t i = 0; i < state.province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id pid{dcon::province_id::value_base_t(i)};
		auto owner = state.world.province_get_nation_from_province_ownership(pid);
		if(owner && !(state.world.province_get_state_membership(pid))) {
			auto state_instance = fatten(state.world, state.world.create_state_instance());
			auto new_market = state.world.create_market();
			auto new_local_market = state.world.force_create_local_market(new_market, state_instance);

			auto abstract_state = state.world.province_get_state_from_abstract_state_membership(pid);

			state_instance.set_definition(abstract_state);
			state_instance.set_capital(pid);
			state.world.force_create_state_ownership(state_instance, owner);

			for(auto mprov : state.world.state_definition_get_abstract_state_membership(abstract_state)) {
				auto prov = mprov.get_province();
				if(prov.get_nation_from_province_ownership() == owner) {
					prov.set_state_membership(state_instance);
				}
			}
		}
	}

	for(auto si : state.world.in_state_instance) {
		auto cap = si.get_capital();
		assert(cap);
		auto slave = cap.get_is_slave();
		auto colonial = cap.get_is_colonial();

		province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
			state.world.province_set_is_colonial(p, colonial);
			state.world.province_set_is_slave(p, slave);
		});
	}
}

bool can_release_as_vassal(sys::state const& state, dcon::nation_id n, dcon::national_identity_id releasable) {
	auto target_nation = state.world.national_identity_get_nation_from_identity_holder(releasable);
	if(!state.world.national_identity_get_is_not_releasable(releasable) &&
			state.world.nation_get_owned_province_count(target_nation) == 0) {
		bool owns_a_core = false;
		bool not_on_capital = true;
		state.world.national_identity_for_each_core(releasable, [&](dcon::core_id core) {
			auto province = state.world.core_get_province(core);
			owns_a_core |= state.world.province_get_nation_from_province_ownership(province) == n;
			not_on_capital &= state.world.nation_get_capital(n) != province;
		});
		return owns_a_core && not_on_capital && !state.world.nation_get_is_at_war(n);
	} else {
		return false;
	}
}

bool identity_has_holder(sys::state const& state, dcon::national_identity_id ident) {
	auto fat_ident = dcon::fatten(state.world, ident);
	return bool(fat_ident.get_nation_from_identity_holder().id);
}

bool are_allied(sys::state& state, dcon::nation_id a, dcon::nation_id b) {
	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
	return state.world.diplomatic_relation_get_are_allied(rel);
}

bool is_landlocked(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_total_ports(n) == 0;
}

dcon::nation_id get_relationship_partner(sys::state const& state, dcon::diplomatic_relation_id rel_id, dcon::nation_id query) {
	auto fat_id = dcon::fatten(state.world, rel_id);
	return fat_id.get_related_nations(0) == query ? fat_id.get_related_nations(1) : fat_id.get_related_nations(0);
}

bool global_national_state::is_global_flag_variable_set(dcon::global_flag_id id) const {
	if(id)
		return dcon::bit_vector_test(global_flag_variables.data(), id.index());
	return false;
}

void global_national_state::set_global_flag_variable(dcon::global_flag_id id, bool state) {
	if(id)
		dcon::bit_vector_set(global_flag_variables.data(), id.index(), state);
}

dcon::text_key name_from_tag(sys::state& state, dcon::national_identity_id tag) {
	auto holder = state.world.national_identity_get_nation_from_identity_holder(tag);
	if(holder)
		return text::get_name(state, holder);
	else
		return state.world.national_identity_get_name(tag);
}

// updates ONLY national admin
void update_national_administrative_efficiency(sys::state& state) {
	/*
	- national administrative efficiency: = (the-nation's-national-administrative-efficiency-modifier +
	efficiency-modifier-from-technologies + 1) x number-of-non-colonial-bureaucrat-population / (total-non-colonial-population x
	(sum-of-the-administrative_multiplier-for-social-issues-marked-as-being-administrative x
	define:BUREAUCRACY_PERCENTAGE_INCREMENT + define:MAX_BUREAUCRACY_PERCENTAGE) )
	*/

	state.world.execute_serial_over_nation([&](auto ids) {
		auto admin_mod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::administrative_efficiency_modifier);
		state.world.nation_set_administrative_efficiency(ids, ve::max(0.05f, ve::min(1.f, 0.05f + admin_mod)));
	});
}

void update_administrative_efficiency(sys::state& state) {
	

	// replaced with control ratio at capital which is doing the same thing but better
	// prepare buffers
	auto control_buffer = state.world.province_make_vectorizable_float_buffer();
	state.world.execute_serial_over_province([&](auto ids) {
		control_buffer.set(ids, state.world.province_get_control_scale(ids));
	});
	auto coastal_capital_buffer = ve::vectorizable_buffer<dcon::province_id, dcon::state_instance_id>(state.world.state_instance_size());
	state.world.execute_parallel_over_state_instance([&](auto ids) {
		ve::apply([&](auto sid) {
			coastal_capital_buffer.set(sid, province::state_get_coastal_capital(state, sid));
		}, ids);
	});

	// propagate control better for state capitals via market connections
	state.world.for_each_trade_route([&](dcon::trade_route_id trid) {
		auto distance = state.world.trade_route_get_distance(trid);
		auto A_market = state.world.trade_route_get_connected_markets(trid, 0);
		auto B_market = state.world.trade_route_get_connected_markets(trid, 1);
		auto A_state_instance = state.world.market_get_zone_from_local_market(A_market);
		auto B_state_instance = state.world.market_get_zone_from_local_market(B_market);
		auto A_owner = state.world.state_instance_get_nation_from_state_ownership(A_state_instance);
		auto B_owner = state.world.state_instance_get_nation_from_state_ownership(B_state_instance);
		float propagation_multiplier = 0.05f;
		auto sphere_A = state.world.nation_get_in_sphere_of(A_owner);
		auto sphere_B = state.world.nation_get_in_sphere_of(B_owner);
		auto overlord_A = state.world.overlord_get_ruler(
			state.world.nation_get_overlord_as_subject(A_owner)
		);
		auto overlord_B = state.world.overlord_get_ruler(
			state.world.nation_get_overlord_as_subject(B_owner)
		);
		auto leader_A = (overlord_A) ? overlord_A : ((sphere_A) ? sphere_A : A_owner);
		auto leader_B = (overlord_B) ? overlord_B : ((sphere_B) ? sphere_B : B_owner);
		if(leader_A != leader_B) {
			return;
		}
		if(A_owner != B_owner) {
			propagation_multiplier /= 2.f;
		}
		// propagate control between ports
		if(state.world.trade_route_get_is_sea_route(trid)) {
			auto port_A = coastal_capital_buffer.get(A_state_instance);
			auto port_B = coastal_capital_buffer.get(B_state_instance);

			// check that ports are not occupied
			bool interrupted = false;
			if(
				state.world.province_get_nation_from_province_control(port_A)
				!=
				state.world.province_get_nation_from_province_ownership(port_A)
			) {
				interrupted = true;
			}
			if(
				state.world.province_get_nation_from_province_control(port_B)
				!=
				state.world.province_get_nation_from_province_ownership(port_B)
			) {
				interrupted = true;
			}

			if(!interrupted) {
				auto port_A_control = control_buffer.get(port_A);
				auto port_B_control = control_buffer.get(port_B);

				auto naval_base_level_A = military::state_naval_base_level(state, A_state_instance);
				auto naval_base_level_B = military::state_naval_base_level(state, B_state_instance);

				auto naval_base_multiplier = (1.f + naval_base_level_A * 2.f + naval_base_level_B * 2.f);

				auto naval_shift_of_control =
					(port_B_control - port_A_control)
					* std::min(0.1f, propagation_multiplier / (distance + 1.f) * naval_base_multiplier);
				state.world.province_set_control_scale(port_A, state.world.province_get_control_scale(port_A) + naval_shift_of_control);
				state.world.province_set_control_scale(port_B, state.world.province_get_control_scale(port_B) - naval_shift_of_control);
			}
		}
		// propagate along land trade routes
		{
			auto capital_A = state.world.state_instance_get_capital(A_state_instance);
			auto capital_B = state.world.state_instance_get_capital(B_state_instance);

			// check that capitals are not occupied
			bool interrupted = false;
			if(
				state.world.province_get_nation_from_province_control(capital_A)
				!=
				state.world.province_get_nation_from_province_ownership(capital_A)
			) {
				interrupted = true;
			}
			if(
				state.world.province_get_nation_from_province_control(capital_B)
				!=
				state.world.province_get_nation_from_province_ownership(capital_B)
			) {
				interrupted = true;
			}

			if(!interrupted) {
				auto capital_A_control = control_buffer.get(capital_A);
				auto capital_B_control = control_buffer.get(capital_B);

				auto land_shift_of_control =
					(capital_B_control - capital_A_control)
					* std::min(0.1f, propagation_multiplier / (distance + 1.f) / 2.f);
				state.world.province_set_control_scale(capital_A, state.world.province_get_control_scale(capital_A) + land_shift_of_control);
				state.world.province_set_control_scale(capital_B, state.world.province_get_control_scale(capital_B) - land_shift_of_control);
			}
		}
	});

	// propagate control inside states
	// sadly a lot of states have separate "islands" inside which require us to do it
	state.world.for_each_state_instance([&](auto sid) {
		auto capital = state.world.state_instance_get_capital(sid);
		province::for_each_province_in_state_instance(state, sid, [&](auto pid) {
			auto change = control_buffer.get(capital) - control_buffer.get(pid);
			state.world.province_set_control_scale(capital, state.world.province_get_control_scale(capital) - change * 0.01f);
			state.world.province_set_control_scale(pid, state.world.province_get_control_scale(pid) + change * 0.01f);
		});
	});

	// propagate control for provinces
	state.world.for_each_province_adjacency([&](auto paid) {
		auto A = state.world.province_adjacency_get_connected_provinces(paid, 0);
		auto B = state.world.province_adjacency_get_connected_provinces(paid, 1);
		bool interrupted = false;
		if(
			state.world.province_get_nation_from_province_control(A)
			!=
			state.world.province_get_nation_from_province_ownership(A)
		) {
			interrupted = true;
		}
		if(
			state.world.province_get_nation_from_province_control(B)
			!=
			state.world.province_get_nation_from_province_ownership(B)
		) {
			interrupted = true;
		}
		if(interrupted) {
			return;
		}
		auto movement_A = 1.f + std::max(0.f, (
			state.world.province_get_modifier_values(A, sys::provincial_mod_offsets::movement_cost) + 1.f
		));
		auto movement_B = 1.f + std::max(0.f, (
			state.world.province_get_modifier_values(B, sys::provincial_mod_offsets::movement_cost) + 1.f
		));
		auto distance = state.world.province_adjacency_get_distance(paid) * (movement_A * movement_B);
		auto A_owner = state.world.province_get_nation_from_province_ownership(A);
		auto B_owner = state.world.province_get_nation_from_province_ownership(B);
		float propagation_multiplier = 0.05f;
		auto sphere_A = state.world.nation_get_in_sphere_of(A_owner);
		auto sphere_B = state.world.nation_get_in_sphere_of(B_owner);
		auto overlord_A = state.world.overlord_get_ruler(
			state.world.nation_get_overlord_as_subject(A_owner)
		);
		auto overlord_B = state.world.overlord_get_ruler(
			state.world.nation_get_overlord_as_subject(B_owner)
		);
		auto leader_A = (overlord_A) ? overlord_A : ((sphere_A) ? sphere_A : A_owner);
		auto leader_B = (overlord_B) ? overlord_B : ((sphere_B) ? sphere_B : B_owner);
		if(leader_A != leader_B) {
			return;
		}
		if(A_owner != B_owner) {
			propagation_multiplier /= 2.f;
		}

		auto A_control = control_buffer.get(A);
		auto B_control = control_buffer.get(B);

		auto railroad_A = state.world.province_get_building_level(A, uint8_t(economy::province_building_type::railroad));
		auto railroad_B = state.world.province_get_building_level(B, uint8_t(economy::province_building_type::railroad));

		auto rail_multiplier = 1.f + railroad_A + railroad_B;

		auto land_shift_of_control = (B_control - A_control) * std::min(0.1f, propagation_multiplier / (distance + 1.f) * rail_multiplier);
		state.world.province_set_control_scale(A, state.world.province_get_control_scale(A) + land_shift_of_control);
		state.world.province_set_control_scale(B, state.world.province_get_control_scale(B) - land_shift_of_control);
	});

	// add friction to control expansion:
	state.world.execute_serial_over_province([&](auto pids) {
		auto population = state.world.province_get_demographics(pids, demographics::total);
		auto control = ve::max(0.f, state.world.province_get_control_scale(pids));

		auto available_control = control * 0.1f;
		auto consumed_control = ve::min(available_control, population);
		// slow down to avoid sudden drops in taxes
		state.world.province_set_control_ratio(
			pids,
			0.99f * state.world.province_get_control_ratio(pids)
			+ 0.01f * ve::select(population == 0.f, 0.f, consumed_control / population)			
		);
		auto supply = ve::max(
			0.f, 
			state.world.province_get_modifier_values(pids, sys::provincial_mod_offsets::supply_limit) + 1.f
		);
		auto movement = ve::max(
			0.f, 
			state.world.province_get_modifier_values(pids, sys::provincial_mod_offsets::movement_cost) + 1.f
		);
		auto attrition = ve::max(
			0.f,
			state.world.province_get_modifier_values(pids, sys::provincial_mod_offsets::max_attrition) + 1.f
		);
		auto decay = 0.01f / (1.f + supply) * (1.f + movement) * (1.f + attrition);
		state.world.province_set_control_scale(pids, ve::max(control * (1.f - decay) - consumed_control, 0.f));
	});
}

float daily_research_points(sys::state& state, dcon::nation_id n) {
	/*
	Let pop-sum = for each pop type (research-points-from-type x 1^(fraction of population / optimal fraction))
	Then, the daily research points earned by a nation is: (national-modifier-research-points-modifier + tech-research-modifier +
	1) x (national-modifier-to-research-points) + pop-sum)
	*/
	auto rp_mod_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::research_points_modifier);
	auto rp_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::research_points);

	float sum_from_pops = 0;
	auto total_pop = state.world.nation_get_demographics(n, demographics::total);
	if(total_pop <= 0.0f)
		return 0.0f;

	state.world.for_each_pop_type([&](dcon::pop_type_id t) {
		auto rp = state.world.pop_type_get_research_points(t);
		if(rp > 0) {
			sum_from_pops += rp * std::min(1.0f, state.world.nation_get_demographics(n, demographics::to_key(state, t)) /
																							 (total_pop * state.world.pop_type_get_research_optimum(t)));
		}
	});

	return std::max(0.0f, (sum_from_pops + rp_mod) * (rp_mod_mod + 1.0f));
}

float priority_national(sys::state& state, dcon::nation_id n, dcon::factory_type_id ftid) {
	float total_priority = 0.f;
	state.world.for_each_factory_type([&](auto factory_type_id) {
		total_priority =
			total_priority
			+ state.world.nation_get_factory_type_experience_priority_national(n, factory_type_id);
	});

	if(total_priority == 0.f) {
		return 0.f;
	} else {
		return state.world.nation_get_factory_type_experience_priority_national(n, ftid) / total_priority;
	}
}

float priority_private(sys::state& state, dcon::nation_id n, dcon::factory_type_id ftid) {
	float total_priority = 0.f;
	state.world.for_each_factory_type([&](auto factory_type_id) {
		total_priority =
			total_priority
			+ state.world.nation_get_factory_type_experience_priority_private(n, factory_type_id);
	});

	if(total_priority == 0.f) {
		return 0.f;
	} else {
		return state.world.nation_get_factory_type_experience_priority_private(n, ftid) / total_priority;
	}
}

void update_research_points(sys::state& state) {
	/*
	Let pop-sum = for each pop type (research-points-from-type x 1^(fraction of population / optimal fraction))
	Then, the daily research points earned by a nation is: (national-modifier-research-points-modifier + tech-research-modifier +
	1) x (national-modifier-to-research-points) + pop-sum)
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto rp_mod_mod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::research_points_modifier);
		auto rp_mod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::research_points);

		ve::fp_vector sum_from_pops;
		auto total_pop = state.world.nation_get_demographics(ids, demographics::total);

		state.world.for_each_pop_type([&](dcon::pop_type_id t) {
			auto rp = state.world.pop_type_get_research_points(t);
			if(rp > 0) {
				sum_from_pops = ve::multiply_and_add(
					rp,
					ve::min(
						1.0f,
						state.world.nation_get_demographics(
							ids, demographics::to_key(state, t)
						)
						/
						(total_pop * state.world.pop_type_get_research_optimum(t))
					),
					sum_from_pops
				);
			}
		});
		auto amount =
			ve::select(
				total_pop > 0.0f
				&&
				state.world.nation_get_owned_province_count(ids) != 0,
				ve::max(
					(sum_from_pops + rp_mod) * (rp_mod_mod + 1.0f),
					0.0f
				),
				0.0f
			);
		/*
		If a nation is not currently researching a tech (or is an unciv),
		research points will be banked, up to a total of 365 x
		daily research points, for civs, or define:MAX_RESEARCH_POINTS for uncivs.
		*/
		auto current_points = state.world.nation_get_research_points(ids);
		auto capped_value = ve::min(
			amount + current_points,
			ve::select(
				state.world.nation_get_is_civilized(ids),
				ve::select(
					state.world.nation_get_current_research(ids) == dcon::technology_id{},
					amount * 365.0f,
					amount + current_points
				),
				state.defines.max_research_points
			)
		);
		state.world.nation_set_research_points(ids, capped_value);

		ve::fp_vector total_priority_national { };
		ve::fp_vector total_priority_private { };

		state.world.for_each_factory_type([&](auto factory_type_id) {
			total_priority_national =
				total_priority_national
				+ state.world.nation_get_factory_type_experience_priority_national(ids, factory_type_id);
			total_priority_private =
				total_priority_private
				+ state.world.nation_get_factory_type_experience_priority_private(ids, factory_type_id);
		});

		state.world.for_each_factory_type([&](auto factory_type_id) {
			auto national_p = state.world.nation_get_factory_type_experience_priority_national(ids, factory_type_id);
			auto private_p = state.world.nation_get_factory_type_experience_priority_private(ids, factory_type_id);
			auto priority =
				ve::select(total_priority_national > 0.f, national_p / total_priority_national, 0.f)
				+
				ve::select(total_priority_private > 0.f, private_p / total_priority_private, 0.f);

			auto exp = state.world.nation_get_factory_type_experience(ids, factory_type_id);

			state.world.nation_set_factory_type_experience(ids, factory_type_id, (exp * 0.999f) + (priority * amount));
		});
	});
}

float get_foreign_investment(sys::state& state, dcon::nation_id n) {
	float v = 0.0f;
	for(auto i : state.world.nation_get_unilateral_relationship_as_target(n)) {
		v += i.get_foreign_investment();
	}
	return v;
}

float get_foreign_investment_as_gp(sys::state& state, dcon::nation_id n) {
	float v = 0.0f;
	for(auto i : state.world.nation_get_unilateral_relationship_as_source(n)) {
		v += i.get_foreign_investment();
	}
	return v;
}

void update_industrial_scores(sys::state& state) {
	/*
	Is the sum of the following two components:
	- For each state: (fraction of factory workers in each state (types marked with can work factory = yes) to the total-workforce
	x building level of factories in the state (capped at 1)) x total-factory-levels
	- For each country that the nation is invested in: define:INVESTMENT_SCORE_FACTOR x the amount invested x 0.05
	*/

	state.world.for_each_nation([&, iweight = state.defines.investment_score_factor](dcon::nation_id n) {
		float sum = 0;
		if(state.world.nation_get_owned_province_count(n) != 0) {
			for(auto si : state.world.nation_get_state_ownership(n)) {
				province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
					for(auto f : state.world.province_get_factory_location(p)) {
						sum += 4.f * economy::get_factory_level(state, f.get_factory()) * economy::factory_total_employment_score(state, f.get_factory());
					}
				});
			}
			sum += nations::get_foreign_investment_as_gp(state, n) * iweight; /* investment factor is already multiplied by 0.05f on scenario creation */
		}

		state.world.nation_set_industrial_score(n, uint16_t(sum));
	});
}

void update_military_scores(sys::state& state) {
	/*
	The first part  is complicated enough that I am going to simplify things slightly, and ignore how mobilization can interact
	with this: First, we need to know the total number of recruitable regiments We also need to know the average land unit score,
	which we define here as (attack + defense + national land attack modifier + national land defense modifier) x discipline Then
	we take the lesser of the number of regiments in the field x 4 or the number of recruitable regiments and multiply it by
	define:DISARMAMENT_ARMY_HIT (if disarmed) multiply that by the average land unit score, multiply again by
	(national-modifier-to-supply-consumption + 1), and then divide by 7.

	To that we add for each capital ship: (hull points + national-naval-defense-modifier) x (gun power +
	national-naval-attack-modifier) / 250

	And then we add one point either per leader or per regiment, whichever is greater.
	*/
	float lp_factor = state.defines.alice_military_score_leadership_factor;
	state.world.execute_serial_over_nation([&, disarm = state.defines.disarmament_army_hit](auto n) {
		auto recruitable = ve::to_float(state.world.nation_get_recruitable_regiments(n));
		auto active_regs = ve::to_float(state.world.nation_get_active_regiments(n));
		auto is_disarmed = ve::apply([&](dcon::nation_id i) { return state.world.nation_get_disarmed_until(i) < state.current_date; }, n);
		auto disarm_factor = ve::select(is_disarmed, ve::fp_vector(disarm), ve::fp_vector(1.0f));
		auto supply_mod = ve::max(state.world.nation_get_modifier_values(n, sys::national_mod_offsets::supply_consumption) + 1.0f, 0.1f);
		auto avg_land_score = state.world.nation_get_averge_land_unit_score(n);
		state.world.nation_set_military_score(n, ve::to_int((ve::min(recruitable, active_regs * 4.0f) * avg_land_score) * ((disarm_factor * supply_mod) / 7.0f) + state.world.nation_get_capital_ship_score(n) + active_regs));
	});
}

float prestige_score(sys::state const& state, dcon::nation_id n) {
	return std::max(0.0f, state.world.nation_get_prestige(n) +
				 state.world.nation_get_modifier_values(n, sys::national_mod_offsets::permanent_prestige));
}

void update_rankings(sys::state& state) {
	uint32_t to_sort_count = 0;
	state.world.for_each_nation([&](dcon::nation_id n) {
		state.nations_by_rank[to_sort_count] = n;
		++to_sort_count;
	});
	std::sort(state.nations_by_rank.begin(), state.nations_by_rank.begin() + to_sort_count,
			[&](dcon::nation_id a, dcon::nation_id b) {
				auto fa = fatten(state.world, a);
				auto fb = fatten(state.world, b);
				if((fa.get_owned_province_count() != 0) != (fb.get_owned_province_count() != 0)) {
					return (fa.get_owned_province_count() != 0);
				}
				if(fa.get_is_civilized() != fb.get_is_civilized())
					return fa.get_is_civilized();
				if(bool(fa.get_overlord_as_subject().get_ruler()) != bool(fa.get_overlord_as_subject().get_ruler()))
					return !bool(fa.get_overlord_as_subject().get_ruler());
				auto a_score = fa.get_military_score() + fa.get_industrial_score() + prestige_score(state, a);
				auto b_score = fb.get_military_score() + fb.get_industrial_score() + prestige_score(state, b);
				if(a_score != b_score)
					return a_score > b_score;
				return a.index() > b.index(); // create a total order
			});
	if(to_sort_count < state.nations_by_rank.size()) {
		state.nations_by_rank[to_sort_count] = dcon::nation_id{};
	}
	for(uint32_t i = 0; i < to_sort_count; ++i) {
		state.world.nation_set_rank(state.nations_by_rank[i], uint16_t(i + 1));
	}
}

void update_ui_rankings(sys::state& state) {
	uint32_t to_sort_count = 0;
	state.world.for_each_nation([&](dcon::nation_id n) {
		if(state.world.nation_get_owned_province_count(n) != 0) {
			state.nations_by_industrial_score[to_sort_count] = n;
			state.nations_by_military_score[to_sort_count] = n;
			state.nations_by_prestige_score[to_sort_count] = n;
			++to_sort_count;
		}
	});
	std::sort(state.nations_by_industrial_score.begin(), state.nations_by_industrial_score.begin() + to_sort_count,
			[&](dcon::nation_id a, dcon::nation_id b) {
				auto fa = fatten(state.world, a);
				auto fb = fatten(state.world, b);
				if(fa.get_is_civilized() && !fb.get_is_civilized())
					return true;
				if(!fa.get_is_civilized() && fb.get_is_civilized())
					return false;
				if(bool(fa.get_overlord_as_subject()) && !bool(fa.get_overlord_as_subject()))
					return false;
				if(!bool(fa.get_overlord_as_subject()) && bool(fa.get_overlord_as_subject()))
					return true;
				auto a_score = fa.get_industrial_score();
				auto b_score = fb.get_industrial_score();
				if(a_score != b_score)
					return a_score > b_score;
				return a.index() > b.index();
			});
	std::sort(state.nations_by_military_score.begin(), state.nations_by_military_score.begin() + to_sort_count,
			[&](dcon::nation_id a, dcon::nation_id b) {
				auto fa = fatten(state.world, a);
				auto fb = fatten(state.world, b);
				if(fa.get_is_civilized() && !fb.get_is_civilized())
					return true;
				if(!fa.get_is_civilized() && fb.get_is_civilized())
					return false;
				if(bool(fa.get_overlord_as_subject()) && !bool(fa.get_overlord_as_subject()))
					return false;
				if(!bool(fa.get_overlord_as_subject()) && bool(fa.get_overlord_as_subject()))
					return true;
				auto a_score = fa.get_military_score();
				auto b_score = fb.get_military_score();
				if(a_score != b_score)
					return a_score > b_score;
				return a.index() > b.index();
			});
	std::sort(state.nations_by_prestige_score.begin(), state.nations_by_prestige_score.begin() + to_sort_count,
			[&](dcon::nation_id a, dcon::nation_id b) {
				auto fa = fatten(state.world, a);
				auto fb = fatten(state.world, b);
				if(fa.get_is_civilized() && !fb.get_is_civilized())
					return true;
				if(!fa.get_is_civilized() && fb.get_is_civilized())
					return false;
				if(bool(fa.get_overlord_as_subject()) && !bool(fa.get_overlord_as_subject()))
					return false;
				if(!bool(fa.get_overlord_as_subject()) && bool(fa.get_overlord_as_subject()))
					return true;
				auto a_score = prestige_score(state, a);
				auto b_score = prestige_score(state, b);
				if(a_score != b_score)
					return a_score > b_score;
				return a.index() > b.index();
			});
	for(uint32_t i = 0; i < to_sort_count; ++i) {
		state.world.nation_set_industrial_rank(state.nations_by_industrial_score[i], uint16_t(i + 1));
		state.world.nation_set_military_rank(state.nations_by_military_score[i], uint16_t(i + 1));
		state.world.nation_set_prestige_rank(state.nations_by_prestige_score[i], uint16_t(i + 1));
	}
}

bool is_great_power(sys::state const& state, dcon::nation_id id) {
	return state.world.nation_get_is_great_power(id);
}

void update_great_powers(sys::state& state) {
	bool at_least_one_added = false;

	for(auto i = state.great_nations.size(); i-- > 0;) {
		if(state.world.nation_get_rank(state.great_nations[i].nation) <= uint16_t(state.defines.great_nations_count)) {
			// is still a gp
			state.great_nations[i].last_greatness = state.current_date;
		} else if(state.great_nations[i].last_greatness + int32_t(state.defines.greatness_days) < state.current_date ||
							state.world.nation_get_owned_province_count(state.great_nations[i].nation) == 0) {

			auto n = state.great_nations[i].nation;
			state.great_nations[i] = state.great_nations.back();
			state.great_nations.pop_back();
			at_least_one_added = true;

			state.world.nation_set_is_great_power(n, false);

			event::fire_fixed_event(state, state.national_definitions.on_lost_great_nation, trigger::to_generic(n),
					event::slot_type::nation, n, -1, event::slot_type::none);

			// kill gp relationships
			auto rels = state.world.nation_get_gp_relationship_as_great_power(n);
			while(rels.begin() != rels.end()) {
				auto rel = *(rels.begin());
				if(rel.get_influence_target().get_in_sphere_of() == n)
					rel.get_influence_target().set_in_sphere_of(dcon::nation_id{});
				state.world.delete_gp_relationship(rel);
			}

			notification::post(state, notification::message{
				[n](sys::state& state, text::layout_base& contents) {
					text::add_line(state, contents, "msg_lost_gp_1", text::variable_type::x, n);
				},
				"msg_lost_gp_title",
				n, dcon::nation_id{}, dcon::nation_id{},
				sys::message_base_type::lose_great_power
			});
		}
	}

	for(uint32_t i = 0; i < uint32_t(state.defines.great_nations_count) && state.great_nations.size() < size_t(state.defines.great_nations_count); ++i) {
		auto n = state.nations_by_rank[i];
		if(n && !state.world.nation_get_is_great_power(n) && state.world.nation_get_owned_province_count(n) > 0) {
			at_least_one_added = true;
			state.world.nation_set_is_great_power(n, true);
			state.great_nations.push_back(sys::great_nation(state.current_date, n));
			state.world.nation_set_state_from_flashpoint_focus(n, dcon::state_instance_id{});

			state.world.nation_set_in_sphere_of(n, dcon::nation_id{});
			auto rng = state.world.nation_get_gp_relationship_as_influence_target(n);
			while(rng.begin() != rng.end()) {
				state.world.delete_gp_relationship(*(rng.begin()));
			}

			event::fire_fixed_event(state, state.national_definitions.on_new_great_nation, trigger::to_generic(n), event::slot_type::nation, n, -1, event::slot_type::none);

			notification::post(state, notification::message{
				[n](sys::state& state, text::layout_base& contents) {
					text::add_line(state, contents, "msg_new_gp_1", text::variable_type::x, n);
				},
				"msg_new_gp_title",
				n, dcon::nation_id{}, dcon::nation_id{},
				sys::message_base_type::become_great_power
			});
		}
	}
	if(at_least_one_added) {
		std::sort(state.great_nations.begin(), state.great_nations.end(), [&](sys::great_nation& a, sys::great_nation& b) {
			return state.world.nation_get_rank(a.nation) < state.world.nation_get_rank(b.nation);
		});
	}
}

status get_status(sys::state& state, dcon::nation_id n) {
	if(is_great_power(state, n)) {
		return status::great_power;
	} else if(state.world.nation_get_rank(n) <= uint16_t(state.defines.colonial_rank)) {
		return status::secondary_power;
	} else if(state.world.nation_get_is_civilized(n)) {
		return status::civilized;
	} else {
		auto civ_progress = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::civilization_progress_modifier);
		if(civ_progress < 0.15f) {
			return status::primitive;
		} else if(civ_progress < 0.5f) {
			return status::uncivilized;
		} else {
			return status::westernizing;
		}
	}
}

sys::date get_research_end_date(sys::state& state, dcon::technology_id tech_id, dcon::nation_id n) {
	sys::date curr = state.current_date;
	if(state.world.technology_get_leadership_cost(tech_id)) {
		auto monthly = military::calculate_monthly_leadership_points(state, n);
		auto total = int32_t((culture::effective_technology_lp_cost(state, curr.to_ymd(state.start_date).year, n, tech_id) - state.world.nation_get_research_points(n)) / (monthly / 30.f));
		return curr + total;
	}
	else {
		auto daily = nations::daily_research_points(state, n);
		auto total = int32_t((culture::effective_technology_rp_cost(state, curr.to_ymd(state.start_date).year, n, tech_id) - state.world.nation_get_research_points(n)) / daily);
		return curr + total;
	}
}

dcon::technology_id current_research(sys::state const& state, dcon::nation_id n) {
	return state.world.nation_get_current_research(n);
}

float suppression_points(sys::state const& state, dcon::nation_id n) {
	return state.world.nation_get_suppression_points(n);
}

float leadership_points(sys::state const& state, dcon::nation_id n) {
	return state.world.nation_get_leadership_points(n);
}

int32_t max_national_focuses(sys::state& state, dcon::nation_id n) {
	/*
	- number of national focuses: the lesser of total-accepted-and-primary-culture-population / define:NATIONAL_FOCUS_DIVIDER and
	1 + the number of national focuses provided by technology.
	*/
	float relevant_pop =
			state.world.nation_get_demographics(n, demographics::to_key(state, state.world.nation_get_primary_culture(n)));
	for(auto ac : state.world.in_culture) {
		if(state.world.nation_get_accepted_cultures(n, ac))
			relevant_pop += state.world.nation_get_demographics(n, demographics::to_key(state, ac));
	}

	return std::max(1, std::min(int32_t(relevant_pop / state.defines.national_focus_divider),
												 int32_t(1 + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_national_focus))));
}

int32_t national_focuses_in_use(sys::state& state, dcon::nation_id n) {
	int32_t total = 0;
	if(state.world.nation_get_state_from_flashpoint_focus(n))
		++total;
	for(auto si : state.world.nation_get_state_ownership(n)) {
		if(si.get_state().get_owner_focus())
			++total;
	}
	return total;
}

float diplomatic_points(sys::state const& state, dcon::nation_id n) {
	return state.world.nation_get_diplomatic_points(n);
}

float monthly_diplomatic_points(sys::state const& state, dcon::nation_id n) {
	auto bmod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::diplomatic_points_modifier) + 1.0f;
	auto dmod = bmod * state.defines.base_monthly_diplopoints;
	return dmod;
}

float colonial_points_from_ships(sys::state& state, dcon::nation_id n) {
	float points = 0.f;
	/*
	Ships: the colonial points they grant x (1.0 - the fraction the nation's naval supply consumption is over that provided
	by its naval bases) x define:COLONIAL_POINTS_FROM_SUPPLY_FACTOR
	*/
	int32_t unit_sum = 0;
	for(auto nv : state.world.nation_get_navy_control(n)) {
		for(auto shp : nv.get_navy().get_navy_membership()) {
			unit_sum += state.military_definitions.unit_base_definitions[shp.get_ship().get_type()].colonial_points;
		}
	}
	float base_supply = std::max(1.0f, float(military::naval_supply_points(state, n)));
	float used_supply = float(military::naval_supply_points_used(state, n));
	float pts_factor = used_supply > base_supply ? std::max(0.0f, 2.0f - used_supply / base_supply) : 1.0f;
	points += unit_sum * pts_factor * state.defines.colonial_points_from_supply_factor;
	return points;
}

float colonial_points_from_naval_bases(sys::state& state, dcon::nation_id n) {
	float points = 0.f;
	/*
	Naval bases: determined by level and the building definition, except you get only define:
	COLONIAL_POINTS_FOR_NON_CORE_BASE (a flat rate) for naval bases not in a core province and not connected by land to
	the capital.
	*/
	for(auto p : state.world.nation_get_province_ownership(n)) {
		auto nb_rank = state.world.province_get_building_level(p.get_province(), uint8_t(economy::province_building_type::naval_base));
		if(nb_rank > 0) {
			if(p.get_province().get_connected_region_id() == state.world.province_get_connected_region_id(state.world.nation_get_capital(n))
				|| p.get_province().get_is_owner_core()) {
				if(p.get_province().get_is_owner_core()) {
					points += float(state.economy_definitions.building_definitions[int32_t(economy::province_building_type::naval_base)].colonial_points[nb_rank - 1]);
				} else {
					points += state.defines.colonial_points_for_non_core_base;
				}
			}
		}
	}
	/*
	Flat rate for overseas coastal states
	*/
	for(auto si : state.world.nation_get_state_ownership(n)) {
		auto scap = si.get_state().get_capital();
		if(scap.get_connected_region_id() != state.world.province_get_connected_region_id(state.world.nation_get_capital(n))) {
			if(province::state_is_coastal_non_core_nb(state, si.get_state())) {
				points += 1.0f;
			}
		}
	}
	return points;
}

float colonial_points_from_technology(sys::state& state, dcon::nation_id n) {
	float points = 0.f;
	state.world.for_each_technology([&](dcon::technology_id t) {
		if(state.world.nation_get_active_technologies(n, t))
			points += float(state.world.technology_get_colonial_points(t));
	});
	return points;
}

float used_colonial_points(sys::state& state, dcon::nation_id n) {
	float points = 0.f;
	/*
	Add total amount invested in colonization (the race stage, not colony states)
	*/
	for(auto col : state.world.nation_get_colonization_as_colonizer(n)) {
		points += float(col.get_points_invested());
	}
	/*
	Add for each colonial province COLONIZATION_COLONY_PROVINCE_MAINTAINANCE
	Add infrastructure value of the province x COLONIZATION_COLONY_RAILWAY_MAINTAINANCE
	*/
	for(auto prov : state.world.nation_get_province_ownership(n)) {
		if(prov.get_province().get_is_colonial()) {
			points += state.defines.colonization_colony_province_maintainance;
			points += state.economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].infrastructure *
				prov.get_province().get_building_level(uint8_t(economy::province_building_type::railroad)) * state.defines.colonization_colony_railway_maintainance;
		}
	}
	return points;
}

int32_t free_colonial_points(sys::state& state, dcon::nation_id n) {
	/*
	Testing: Add COLONIZATION_COLONY_INDUSTRY_MAINTAINANCE per factory in a colony
	If we have done things correctly, no such thing should exist
	*/
	return max_colonial_points(state, n) - int32_t(used_colonial_points(state, n));
}

int32_t max_colonial_points(sys::state& state, dcon::nation_id n) {
	/*
	Only nations with rank at least define: COLONIAL_RANK get colonial points.
	*/
	if(state.world.nation_get_rank(n) <= state.defines.colonial_rank) {
		float points = 0.0f;
		return int32_t(colonial_points_from_naval_bases(state, n)) + int32_t(colonial_points_from_ships(state, n)) + int32_t(colonial_points_from_technology(state, n));
	} else {
		return 0;
	}
}

bool can_expand_colony(sys::state& state, dcon::nation_id n) {
	for(auto cols : state.world.nation_get_colonization_as_colonizer(n)) {
		auto state_colonization = state.world.state_definition_get_colonization(cols.get_state());
		auto num_colonizers = state_colonization.end() - state_colonization.begin();
		if(cols.get_state().get_colonization_stage() == uint8_t(3)) {
			return true;
		} else {
			if(province::can_invest_in_colony(state, n, cols.get_state())) {
				return true;
			}
		}
	}
	return false;
}

bool is_losing_colonial_race(sys::state& state, dcon::nation_id n) {
	for(auto cols : state.world.nation_get_colonization_as_colonizer(n)) {
		auto lvl = cols.get_level();
		for(auto ocol : state.world.state_definition_get_colonization(cols.get_state())) {
			if(lvl < ocol.get_level())
				return true;
		}
	}
	return false;
}

bool sphereing_progress_is_possible(sys::state& state, dcon::nation_id n) {
	for(auto it : state.world.nation_get_gp_relationship_as_great_power(n)) {
		if((it.get_status() & influence::is_banned) == 0) {
			if(it.get_influence() >= state.defines.increaseopinion_influence_cost
				&& (influence::level_mask & it.get_status()) != influence::level_in_sphere
				&& (influence::level_mask & it.get_status()) != influence::level_friendly) {
				return true;
			} else if(!(it.get_influence_target().get_in_sphere_of()) &&
								it.get_influence() >= state.defines.addtosphere_influence_cost) {
				return true;
			} else if(it.get_influence_target().get_in_sphere_of() &&
								(influence::level_mask & it.get_status()) == influence::level_friendly &&
								it.get_influence() >= state.defines.removefromsphere_influence_cost) {
				return true;
			}
		}
	}
	return false;
}

bool has_political_reform_available(sys::state& state, dcon::nation_id n) {
	for(auto i : state.culture_definitions.political_issues) {
		auto current = state.world.nation_get_issues(n, i);
		for(auto o : state.world.issue_get_options(i)) {
			if(o && politics::can_enact_political_reform(state, n, o)) {
				return true;
			}
		}
	}
	return false;
}

bool has_social_reform_available(sys::state& state, dcon::nation_id n) {
	for(auto i : state.culture_definitions.social_issues) {
		auto current = state.world.nation_get_issues(n, i);
		for(auto o : state.world.issue_get_options(i)) {
			if(o && politics::can_enact_social_reform(state, n, o)) {
				return true;
			}
		}
	}
	return false;
}

bool has_reform_available(sys::state& state, dcon::nation_id n) {
	// At least define:MIN_DELAY_BETWEEN_REFORMS months must have passed since the last issue option change (for any type of
	// issue).
	auto last_date = state.world.nation_get_last_issue_or_reform_change(n);
	if(bool(last_date) && (last_date + int32_t(state.defines.min_delay_between_reforms * 30.0f)) > state.current_date)
		return false;

	if(state.world.nation_get_is_civilized(n)) {
		/*
		### When a social/political reform is possible
		These are only available for civ nations. If it is "next step only" either the previous or next issue option must be in
		effect And it's `allow` trigger must be satisfied. Then. for each ideology, we test its `add_social_reform` or
		`remove_social_reform` (depending if we are increasing or decreasing, and substituting `political_reform` here as
		necessary), computing its modifier additively, and then adding the result x the fraction of the upperhouse that the
		ideology has to a running total. If the running total is > 0.5, the issue option can be implemented.
		*/
		return has_political_reform_available(state, n) || has_social_reform_available(state, n);
	}
	/*
	### When an economic/military reform is possible
	These are only available for unciv nations. Some of the rules are the same as for social/political reforms:  If it is "next
	step only" either the previous or next issue option must be in effect. And it's `allow` trigger must be satisfied. Where
	things are different: Each reform also has a cost in research points. This cost, however, can vary. The actual cost you must
	pay is multiplied by what I call the "reform factor" + 1. The reform factor is (sum of ideology-in-upper-house x
	add-reform-triggered-modifier) x define:ECONOMIC_REFORM_UH_FACTOR + the nation's self_unciv_economic/military_modifier + the
	unciv_economic/military_modifier of the nation it is in the sphere of (if any).
	*/
	else {
		auto stored_rp = state.world.nation_get_research_points(n);
		for(auto i : state.culture_definitions.military_issues) {
			auto current = state.world.nation_get_reforms(n, i);
			for(auto o : state.world.reform_get_options(i)) {
				if(o && politics::can_enact_military_reform(state, n, o)) {
					return true;
				}
			}
		}
		for(auto i : state.culture_definitions.economic_issues) {
			auto current = state.world.nation_get_reforms(n, i);
			for(auto o : state.world.reform_get_options(i)) {
				if(o && politics::can_enact_economic_reform(state, n, o)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool has_decision_available(sys::state& state, dcon::nation_id n) {
	for(uint32_t i = state.world.decision_size(); i-- > 0;) {
		dcon::decision_id did{dcon::decision_id::value_base_t(i)};
		if(!state.world.decision_get_hide_notification(did)) {
			auto lim = state.world.decision_get_potential(did);
			if(!lim || trigger::evaluate(state, lim, trigger::to_generic(n), trigger::to_generic(n), 0)) {
				auto allow = state.world.decision_get_allow(did);
				if(!allow || trigger::evaluate(state, allow, trigger::to_generic(n), trigger::to_generic(n), 0)) {
					return true;
				}
			}
		}
	}
	return false;
}

void get_active_political_parties(sys::state& state, dcon::nation_id n, std::vector<dcon::political_party_id>& parties) {
	auto identity = state.world.nation_get_identity_from_identity_holder(n);
	auto start = state.world.national_identity_get_political_party_first(identity).id.index();
	auto end = start + state.world.national_identity_get_political_party_count(identity);
	for(int32_t i = start; i < end; i++) {
		auto pid = dcon::political_party_id(uint16_t(i));
		if(politics::political_party_is_active(state, n, pid)) {
			parties.push_back(pid);
		}
	}
}

void monthly_adjust_relationship(sys::state& state, dcon::nation_id a, dcon::nation_id b, float delta) {
	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
	if(!rel) {
		rel = state.world.force_create_diplomatic_relation(a, b);
	}
	auto& val = state.world.diplomatic_relation_get_value(rel);
	state.world.diplomatic_relation_set_value(rel, std::clamp(val + delta, -200.0f, std::max(val, 100.0f)));
}

void update_revanchism(sys::state& state) {
	/*
	- revanchism: you get one point per unowned core if your primary culture is the dominant culture (culture with the most
	population) in the province, 0.25 points if it is not the dominant culture, and then that total is divided by the total number
	of your cores to get your revanchism percentage
	*/
	for(auto n : state.world.in_nation) {
		auto owned = n.get_province_ownership();
		if(owned.begin() != owned.end()) {
			auto pc = n.get_primary_culture();
			int32_t total_num_cores = 0;
			float rpts = 0.0f;
			for(auto core : n.get_identity_from_identity_holder().get_core()) {
				++total_num_cores;
				if(core.get_province().get_nation_from_province_ownership() != n) {
					if(core.get_province().get_dominant_culture() == pc)
						rpts += 1.0f;
					else
						rpts += 0.25f;
				}
			}
			if(total_num_cores > 0) {
				n.set_revanchism(rpts / float(total_num_cores));
			} else {
				n.set_revanchism(0.0f);
			}
		}
	}
}

void update_monthly_points(sys::state& state) {
	/*
	- Prestige: a nation with a prestige modifier gains that amount of prestige per month (on the 1st)
	*/

	// Removed monthly prestige update: this is because technologies have a prestige effect that is supposed to act as a multiplier to earned prestige
	// while the other prestige modifiers are supposed to add monthly prestige
	// they need to be separated out from each other (even though they have the same name)
	// until we do that, removing ticking prestige is the easier fix
	
	//state.world.execute_serial_over_nation([&](auto ids) {
	//	auto pmod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::prestige);
	//	state.world.nation_set_prestige(ids, state.world.nation_get_prestige(ids) + pmod);
	//});
	/*
	- Infamy: a nation with a badboy modifier gains that amount of infamy per month
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto imod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::badboy);
		state.world.nation_set_infamy(ids, ve::max(state.world.nation_get_infamy(ids) + imod, 0.0f));
	});
	/*
	- War exhaustion: a nation with a war exhaustion modifier gains that much at the start of the month, and every month its war
	exhaustion is capped to its maximum-war-exhaustion modifier at most.
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto wmod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::war_exhaustion);
		auto wmax_mod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::max_war_exhaustion);
		state.world.nation_set_war_exhaustion(ids,
				ve::max(ve::min(state.world.nation_get_war_exhaustion(ids) + wmod, wmax_mod), 0.0f));
	});
	/*
	- Monthly plurality increase: plurality increases by average consciousness / 45 per month.
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto pmod = state.world.nation_get_demographics(ids, demographics::consciousness) /
								ve::max(state.world.nation_get_demographics(ids, demographics::total), 1.0f) * 0.0222f;
		state.world.nation_set_plurality(ids, ve::max(ve::min(state.world.nation_get_plurality(ids) + pmod, 100.0f), 0.f));
	});
	/*
	- Monthly diplo-points: (1 + national-modifier-to-diplo-points + diplo-points-from-technology) x
	define:BASE_MONTHLY_DIPLOPOINTS (to a maximum of 9)
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto bmod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::diplomatic_points_modifier) + 1.0f;
		auto dmod = bmod * state.defines.base_monthly_diplopoints;

		state.world.nation_set_diplomatic_points(ids, ve::max(ve::min(state.world.nation_get_diplomatic_points(ids) + dmod, 9.0f), 0.f));
	});
	/*
	- Monthly suppression point gain: define:SUPPRESS_BUREAUCRAT_FACTOR x fraction-of-population-that-are-bureaucrats x
	define:SUPPRESSION_POINTS_GAIN_BASE x (suppression-points-from-technology + national-suppression-points-modifier + 1) (to a
	maximum of define:MAX_SUPPRESSION)
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto bmod = (state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::suppression_points_modifier) + 1.0f);
		auto cmod = (bmod * state.defines.suppression_points_gain_base) *
								(state.world.nation_get_demographics(ids, demographics::to_key(state, state.culture_definitions.bureaucrat)) /
										ve::max(state.world.nation_get_demographics(ids, demographics::total), 1.0f) *
										state.defines.suppress_bureaucrat_factor);

		state.world.nation_set_suppression_points(ids, ve::max(ve::min(state.world.nation_get_suppression_points(ids) + cmod, state.defines.max_suppression), 0.f));
	});
	/*
	- Monthly relations adjustment = +0.25 for subjects/overlords, -0.01 for being at war, +0.05 if adjacent and both are at
	peace, +0.025 for having military access, -0.15 for being able to use a CB against each other (-0.30 if it goes both ways)
	- Once relations are at 100, monthly increases cannot take them higher
	*/
	for(auto so : state.world.in_overlord) {
		monthly_adjust_relationship(state, so.get_ruler(), so.get_subject(), 0.25f);
	}
	for(auto an : state.world.in_nation_adjacency) {
		if(an.get_connected_nations(0).get_is_at_war() == false && an.get_connected_nations(1).get_is_at_war() == false)
			monthly_adjust_relationship(state, an.get_connected_nations(0), an.get_connected_nations(1), 0.05f);
		if(military::can_use_cb_against(state, an.get_connected_nations(0), an.get_connected_nations(1))) {
			monthly_adjust_relationship(state, an.get_connected_nations(0), an.get_connected_nations(1), -0.15f);
		}
		if(military::can_use_cb_against(state, an.get_connected_nations(1), an.get_connected_nations(0))) {
			monthly_adjust_relationship(state, an.get_connected_nations(0), an.get_connected_nations(1), -0.15f);
		}
	}
	for(auto i : state.world.in_unilateral_relationship) {
		if(i.get_military_access()) {
			monthly_adjust_relationship(state, i.get_source(), i.get_target(), 0.025f);
		}
	}
	for(auto w : state.world.in_war) {
		for(auto n : w.get_war_participant()) {
			for(auto m : w.get_war_participant()) {
				if(n.get_is_attacker() != m.get_is_attacker()) {
					monthly_adjust_relationship(state, n.get_nation(), m.get_nation(), -0.005f);
				}
			}
		}
	}

	update_revanchism(state);
}

float get_treasury(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_stockpiles(n, economy::money);
}

float get_bank_funds(sys::state& state, dcon::nation_id n) {
	return economy::max_loan(state, n);
}

float get_debt(sys::state& state, dcon::nation_id n) {
	auto v = state.world.nation_get_stockpiles(n, economy::money);
	return v < 0.0f ? -v : 0.0f;
}

// estimates rate of tariffs collected in a market
float tariff_efficiency(sys::state& state, dcon::nation_id n, dcon::market_id m) {
	auto sid = state.world.market_get_zone_from_local_market(m);
	auto pid = state.world.state_instance_get_capital(sid);
	auto eff_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::tariff_efficiency_modifier);
	auto adm_eff = state.world.province_get_control_scale(pid);
	return std::clamp((state.defines.base_tariff_efficiency + eff_mod) * adm_eff, 0.f, 1.f);
}

float tax_efficiency(sys::state& state, dcon::nation_id n) {
	auto eff_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::tax_efficiency);
	return std::clamp(state.defines.base_country_tax_efficiency + eff_mod, 0.01f, 1.f);
}

bool is_involved_in_crisis(sys::state const& state, dcon::nation_id n) {
	if(n == state.primary_crisis_attacker)
		return true;
	if(n == state.primary_crisis_defender)
		return true;
	for(auto& par : state.crisis_participants) {
		if(!par.id)
			return false;
		if(par.id == n)
			return true;
	}

	return false;
}
bool is_committed_in_crisis(sys::state const& state, dcon::nation_id n) {
	if(n == state.primary_crisis_attacker)
		return true;
	if(n == state.primary_crisis_defender)
		return true;
	for(auto& par : state.crisis_participants) {
		if(!par.id)
			return false;
		if(par.id == n)
			return !par.merely_interested;
	}
	return false;
}
void switch_all_players(sys::state& state, dcon::nation_id new_n, dcon::nation_id old_n) {
	if(state.network_mode == sys::network_mode_type::single_player) {
		state.world.nation_set_is_player_controlled(new_n, true);
		state.world.nation_set_is_player_controlled(old_n, false);
		state.local_player_nation = new_n;
	} else {
		auto p = network::find_country_players(state, old_n);
		// move ALL players which are on the current nation, to the new nation
		for(auto player : p) {
			state.world.force_create_player_nation(new_n, player);
		}
		if(!p.empty()) {
			state.world.nation_set_is_player_controlled(new_n, true);
			state.world.nation_set_is_player_controlled(old_n, false);
		}

		if(state.network_mode == sys::network_mode_type::host) {
			for(auto& client : state.network_state.clients) {
				if(!client.is_active())
					continue;
				if(client.playing_as == old_n) {
					client.playing_as = new_n;
				}
			}

			network::write_player_nations(state);
		}
		if(state.local_player_nation == old_n) {
			state.local_player_nation = new_n;
		}
		// We will also re-assign all chat messages from this nation to the new one
		for(auto& msg : state.ui_state.chat_messages)
			if(bool(msg.source) && msg.source == old_n)
				msg.source = new_n;
	}

}




void adjust_relationship(sys::state& state, dcon::nation_id a, dcon::nation_id b, float delta) {
	if(state.world.nation_get_owned_province_count(a) == 0 || state.world.nation_get_owned_province_count(a) == 0)
		return;

	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
	if(!rel) {
		rel = state.world.force_create_diplomatic_relation(a, b);
	}
	auto& val = state.world.diplomatic_relation_get_value(rel);
	state.world.diplomatic_relation_set_value(rel, std::clamp(val + delta, -200.0f, 200.0f));
}

void create_nation_based_on_template(sys::state& state, dcon::nation_id n, dcon::nation_id base) {
	state.world.nation_set_is_civilized(n, state.world.nation_get_is_civilized(base));
	state.world.nation_set_national_value(n, state.world.nation_get_national_value(base));
	state.world.nation_set_tech_school(n, state.world.nation_get_tech_school(base));
	state.world.nation_set_government_type(n, state.world.nation_get_government_type(base));
	state.world.nation_set_plurality(n, state.world.nation_get_plurality(base));
	state.world.nation_set_prestige(n, 0.0f);
	state.world.nation_set_infamy(n, 0.0f);
	state.world.nation_set_revanchism(n, 0.0f);
	state.world.for_each_technology([&](dcon::technology_id t) {
		state.world.nation_set_active_technologies(n, t, state.world.nation_get_active_technologies(base, t));
	});
	state.world.for_each_invention([&](dcon::invention_id t) {
		state.world.nation_set_active_inventions(n, t, state.world.nation_get_active_inventions(base, t));
	});
	state.world.for_each_issue(
			[&](dcon::issue_id t) { state.world.nation_set_issues(n, t, state.world.nation_get_issues(base, t)); });
	if(!state.world.nation_get_is_civilized(base)) {
		state.world.for_each_reform(
				[&](dcon::reform_id t) { state.world.nation_set_reforms(n, t, state.world.nation_get_reforms(base, t)); });
	}
	state.world.nation_set_last_issue_or_reform_change(n, sys::date{});
	culture::update_nation_issue_rules(state, n);
	state.world.for_each_ideology(
			[&](dcon::ideology_id i) { state.world.nation_set_upper_house(n, i, state.world.nation_get_upper_house(base, i)); });
	state.world.nation_set_is_substate(n, false);
	//for(int32_t i = 0; i < state.national_definitions.num_allocated_national_flags; ++i) {
	//	state.world.nation_set_flag_variables(n, dcon::national_flag_id{dcon::national_flag_id::value_base_t(i)}, false);
	//}
	//for(int32_t i = 0; i < state.national_definitions.num_allocated_national_variables; ++i) {
	//	state.world.nation_set_variables(n, dcon::national_variable_id{dcon::national_variable_id::value_base_t(i)}, 0.0f);
	//}
	state.world.for_each_commodity([&](dcon::commodity_id t) {
		state.world.nation_set_rgo_goods_output(n, t, state.world.nation_get_rgo_goods_output(base, t));
		state.world.nation_set_factory_goods_output(n, t, state.world.nation_get_factory_goods_output(base, t));
		state.world.nation_set_rgo_size(n, t, state.world.nation_get_rgo_size(base, t));
		state.world.nation_set_factory_goods_throughput(n, t, state.world.nation_get_factory_goods_throughput(base, t));
	});
	state.world.for_each_rebel_type([&](dcon::rebel_type_id t) {
		state.world.nation_set_rebel_org_modifier(n, t, state.world.nation_get_rebel_org_modifier(base, t));
	});
	for(uint32_t i = 0; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		state.world.nation_set_unit_stats(n, dcon::unit_type_id{dcon::unit_type_id::value_base_t(i)},
				state.world.nation_get_unit_stats(base, dcon::unit_type_id{dcon::unit_type_id::value_base_t(i)}));
		state.world.nation_set_active_unit(n, dcon::unit_type_id{dcon::unit_type_id::value_base_t(i)},
				state.world.nation_get_active_unit(base, dcon::unit_type_id{dcon::unit_type_id::value_base_t(i)}));
	}
	for(uint32_t i = 0; i < state.culture_definitions.crimes.size(); ++i) {
		state.world.nation_set_active_crime(n, dcon::crime_id{dcon::crime_id::value_base_t(i)},
				state.world.nation_get_active_crime(base, dcon::crime_id{dcon::crime_id::value_base_t(i)}));
	}
	state.world.for_each_factory_type([&](dcon::factory_type_id t) {
		state.world.nation_set_active_building(n, t, state.world.nation_get_active_building(base, t));
	});
	state.world.for_each_commodity([&](dcon::commodity_id t) {
		state.world.nation_set_unlocked_commodities(n, t, state.world.nation_get_unlocked_commodities(base, t));
	});
	state.world.nation_set_has_gas_attack(n, state.world.nation_get_has_gas_attack(base));
	state.world.nation_set_has_gas_defense(n, state.world.nation_get_has_gas_defense(base));
	for(auto t = economy::province_building_type::railroad; t != economy::province_building_type::last; t = economy::province_building_type(uint8_t(t) + 1)) {
		state.world.nation_set_max_building_level(n, uint8_t(t), state.world.nation_get_max_building_level(base, uint8_t(t)));
	}
	state.world.nation_set_election_ends(n, sys::date{0});
	state.world.nation_set_education_spending(n, int8_t(100));
	state.world.nation_set_military_spending(n, int8_t(100));
	state.world.nation_set_administrative_spending(n, int8_t(100));
	state.world.nation_set_social_spending(n, int8_t(100));
	state.world.nation_set_land_spending(n, int8_t(100));
	state.world.nation_set_naval_spending(n, int8_t(100));
	state.world.nation_set_construction_spending(n, int8_t(100));
	state.world.nation_set_effective_land_spending(n, 1.0f);
	state.world.nation_set_effective_naval_spending(n, 1.0f);
	state.world.nation_set_effective_construction_spending(n, 1.0f);
	state.world.nation_set_spending_level(n, 1.0f);
	state.world.nation_set_poor_tax(n, int8_t(50));
	state.world.nation_set_middle_tax(n, int8_t(50));
	state.world.nation_set_rich_tax(n, int8_t(50));
	state.world.nation_set_tariffs_import(n, int8_t(5));
	state.world.nation_set_tariffs_export(n, int8_t(5));

	auto base_ruling_ideology = state.world.political_party_get_ideology(state.world.nation_get_ruling_party(base));

	auto identity = state.world.nation_get_identity_from_identity_holder(n);
	auto start = state.world.national_identity_get_political_party_first(identity).id.index();
	auto end = start + state.world.national_identity_get_political_party_count(identity);

	for(int32_t i = start; i < end; i++) {
		auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(i));
		if(politics::political_party_is_active(state, n, pid) && state.world.political_party_get_ideology(pid) == base_ruling_ideology) {
			state.world.nation_set_ruling_party(n, pid);
			break;
		}
	}
	if(!state.world.nation_get_ruling_party(n)) {
		for(int32_t i = start; i < end; i++) {
			auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(i));
			if(politics::political_party_is_active(state, n, pid)) {
				state.world.nation_set_ruling_party(n, pid);
				break;
			}
		}
	}

	// populate key values based on national identity
	auto tag = fatten(state.world, state.world.nation_get_identity_from_identity_holder(n));
	state.world.nation_set_primary_culture(n, tag.get_primary_culture());
	state.world.nation_set_religion(n, tag.get_religion());
	if(auto cg = tag.get_culture_group_from_cultural_union_of(); cg) {
		for(auto c : cg.get_culture_group_membership()) {
			if(c.get_member().id != tag.get_primary_culture().id) {
				state.world.nation_set_accepted_cultures(n, c.get_member(), true);
			}
		}
	}

	politics::update_displayed_identity(state, n);
}

void run_gc(sys::state& state) {
	//cleanup (will set gc pending)
	for(const auto n : state.world.in_nation) {
		if(n.get_marked_for_gc()) {
			n.set_marked_for_gc(false);
			if(auto lprovs = n.get_province_ownership(); lprovs.begin() == lprovs.end()) {
				nations::cleanup_nation(state, n);
			}
		}
	}
	if(state.national_definitions.gc_pending) {
		state.national_definitions.gc_pending = false;
		for(uint32_t i = state.world.rebel_faction_size(); i-- > 0; ) {
			dcon::rebel_faction_id rf{dcon::rebel_faction_id::value_base_t(i) };
			auto within = state.world.rebel_faction_get_ruler_from_rebellion_within(rf);
			if(!within)
				state.world.delete_rebel_faction(rf);
		}
	}
}

void cleanup_nation(sys::state& state, dcon::nation_id n) {
	auto old_ident = state.world.nation_get_identity_from_identity_holder(n);

	auto control = state.world.nation_get_province_control(n);
	while(control.begin() != control.end()) {
		province::set_province_controller(state, (*control.begin()).get_province(), (*control.begin()).get_province().get_nation_from_province_ownership());
	}

	auto leaders = state.world.nation_get_leader_loyalty(n);
	while(leaders.begin() != leaders.end()) {
		state.world.delete_leader((*leaders.begin()).get_leader());
	}

	auto ss_range = state.world.nation_get_overlord_as_ruler(n);
	while(ss_range.begin() != ss_range.end()) {
		auto subj = (*ss_range.begin()).get_subject();
		subj.set_is_substate(false);
		nations::release_vassal(state, (*ss_range.begin()));
	}

	auto ol = state.world.nation_get_overlord_as_subject(n);
	if(state.world.overlord_get_ruler(ol)) {
		release_vassal(state, ol);
	}

	//auto armies = state.world.nation_get_army_control(n);
	//while(armies.begin() != armies.end()) {
	//	military::cleanup_army(state, (*armies.begin()).get_army());
	//}

	//auto navies = state.world.nation_get_navy_control(n);
	//while(navies.begin() != navies.end()) {
	//	military::cleanup_navy(state, (*navies.begin()).get_navy());
	//}

	//auto rebels = state.world.nation_get_rebellion_within(n);
	//while(rebels.begin() != rebels.end()) {
	//	rebel::delete_faction(state, (*rebels.begin()).get_rebels());
	//}

	auto diprel = state.world.nation_get_diplomatic_relation(n);
	while(diprel.begin() != diprel.end()) {
		state.world.delete_diplomatic_relation(*diprel.begin());
	}

	auto uni_diprel = state.world.nation_get_unilateral_relationship_as_source(n);
	while(uni_diprel.begin() != uni_diprel.end()) {
		state.world.delete_unilateral_relationship(*uni_diprel.begin());
	}

	auto uni_diprelb = state.world.nation_get_unilateral_relationship_as_target(n);
	while(uni_diprelb.begin() != uni_diprelb.end()) {
		state.world.delete_unilateral_relationship(*uni_diprelb.begin());
	}

	auto movements = state.world.nation_get_movement_within(n);
	while(movements.begin() != movements.end()) {
		state.world.delete_movement((*movements.begin()).get_movement());
	}

	// transfer flags and variables to new holder
	state.world.delete_nation(n);
	auto new_ident_holder = state.world.create_nation();
	state.world.try_create_identity_holder(new_ident_holder, old_ident);

	for(auto o : state.world.in_nation) {
		if(o.get_in_sphere_of() == n) {
			notification::post(state, notification::message{
				[o, n](sys::state& state, text::layout_base& contents) {
					text::add_line(state, contents, "msg_rem_sphere_1", text::variable_type::x, n, text::variable_type::y, o);
				},
				"msg_rem_sphere_title",
				n, o, dcon::nation_id{},
				sys::message_base_type::rem_sphere
			});
			o.set_in_sphere_of(dcon::nation_id{});
		}
	}

	state.national_definitions.gc_pending = true;
	state.diplomatic_cached_values_out_of_date = true; // refresh stored counts of allies, vassals, etc
	politics::update_displayed_identity(state, n);

	if(n == state.local_player_nation) {
		// Player was defeated, show end screen
		game_scene::switch_scene(state, game_scene::scene_id::end_screen);
	}
}

void adjust_prestige(sys::state& state, dcon::nation_id n, float delta) {
	float prestige_multiplier = 1.0f + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::prestige);
	float v = state.world.nation_get_prestige(n) + (delta > 0 ? (delta * prestige_multiplier) : delta);
	float new_prestige = std::clamp(v, 0.f, max_prestige);
	state.world.nation_set_prestige(n, new_prestige);
}

bool destroy_vassal_relationships(sys::state& state, dcon::nation_id n) {
	auto ov_rel = state.world.nation_get_overlord_as_ruler(n);
	for(auto it = ov_rel.begin(); it != ov_rel.end(); ++it) {
		if((*it).get_subject().get_is_substate() == false) {
			release_vassal(state, *it);
			return true;
		}
	}
	return false;
}

void destroy_diplomatic_relationships(sys::state& state, dcon::nation_id n) {
	{
		auto gp_relationships = state.world.nation_get_gp_relationship_as_great_power(n);
		while(gp_relationships.begin() != gp_relationships.end()) {
			auto i = (*gp_relationships.begin()).get_influence_target();
			if(i.get_in_sphere_of() == n)
				i.set_in_sphere_of(dcon::nation_id{});
			state.world.delete_gp_relationship(*(gp_relationships.begin()));
		}
	}
	{
		auto gp_relationships = state.world.nation_get_gp_relationship_as_influence_target(n);
		while(gp_relationships.begin() != gp_relationships.end()) {
			state.world.delete_gp_relationship(*(gp_relationships.begin()));
		}
		state.world.nation_set_in_sphere_of(n, dcon::nation_id{});
	}
	{
		for(auto rel : state.world.nation_get_diplomatic_relation(n)) {
			break_alliance(state, rel);
		}
	}
	{
		auto ov_rel = state.world.nation_get_overlord_as_ruler(n);
		bool released_vassal = true;
		while(released_vassal) {
			released_vassal = destroy_vassal_relationships(state, n);
		}
	}
}
void release_vassal(sys::state& state, dcon::overlord_id rel) {
	auto vas = state.world.overlord_get_subject(rel);
	auto ol = state.world.overlord_get_ruler(rel);
	if(ol) {
		if(state.world.nation_get_is_substate(vas)) {
			state.world.nation_set_is_substate(vas, false);
			state.world.nation_get_substates_count(ol)--;
		}
		state.world.nation_get_vassals_count(ol)--;
		state.world.delete_overlord(rel);
		politics::update_displayed_identity(state, vas);
		// TODO: notify player
	}
}

void make_vassal(sys::state& state, dcon::nation_id subject, dcon::nation_id overlord) {
	if(subject == overlord)
		return;
	if(state.world.nation_get_owned_province_count(subject) == 0 || state.world.nation_get_owned_province_count(overlord) == 0)
		return;

	auto current_ol = state.world.nation_get_overlord_as_subject(subject);
	auto current_ruler = state.world.overlord_get_ruler(current_ol);

	if(current_ruler && current_ruler != overlord) {
		release_vassal(state, current_ol);
	}
	if(current_ruler == overlord) {
		if(state.world.nation_get_is_substate(subject)) {
			state.world.nation_set_is_substate(subject, false);
			state.world.nation_get_substates_count(current_ruler)--;
		}
	} else {
		state.world.force_create_overlord(subject, overlord);
		state.world.nation_get_vassals_count(overlord)++;
		politics::update_displayed_identity(state, subject);
	}
}
void make_substate(sys::state& state, dcon::nation_id subject, dcon::nation_id overlord) {
	if(subject == overlord)
		return;
	if(state.world.nation_get_owned_province_count(subject) == 0 || state.world.nation_get_owned_province_count(overlord) == 0)
		return;

	auto current_ol = state.world.nation_get_overlord_as_subject(subject);
	auto current_ruler = state.world.overlord_get_ruler(current_ol);

	if(current_ruler && current_ruler != overlord) {
		release_vassal(state, current_ol);
	}
	if(current_ruler == overlord) {
		if(!state.world.nation_get_is_substate(subject)) {
			state.world.nation_set_is_substate(subject, true);
			state.world.nation_get_substates_count(current_ruler)++;
		}
	} else {
		state.world.force_create_overlord(subject, overlord);
		state.world.nation_set_is_substate(subject, true);
		state.world.nation_get_vassals_count(overlord)++;
		state.world.nation_get_substates_count(current_ruler)++;
		politics::update_displayed_identity(state, subject);
	}
}

void break_alliance(sys::state& state, dcon::diplomatic_relation_id rel) {
	if(state.world.diplomatic_relation_get_are_allied(rel)) {
		state.world.diplomatic_relation_set_are_allied(rel, false);
		state.world.nation_get_allies_count(state.world.diplomatic_relation_get_related_nations(rel, 0))--;
		state.world.nation_get_allies_count(state.world.diplomatic_relation_get_related_nations(rel, 1))--;
	}
}

void break_alliance(sys::state& state, dcon::nation_id a, dcon::nation_id b) {
	if(auto r = state.world.get_diplomatic_relation_by_diplomatic_pair(a, b); r) {
		if(state.world.diplomatic_relation_get_are_allied(r)) {
			break_alliance(state, r);
			if(a != state.local_player_nation) {
				notification::post(state, notification::message{
					[from = text::get_name(state, a), to = text::get_name(state, b)](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "msg_alliance_ends_1", text::variable_type::x, to, text::variable_type::y, from);
					},
					"msg_alliance_ends_title",
					a, b, dcon::nation_id{},
					sys::message_base_type::alliance_ends
				});
			}
		}
	}
}
void make_alliance(sys::state& state, dcon::nation_id a, dcon::nation_id b) {
	if(a == b)
		return;
	auto r = state.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
	if(!r) {
		r = state.world.force_create_diplomatic_relation(a, b);
	}
	if(!state.world.diplomatic_relation_get_are_allied(r)) {
		state.world.nation_get_allies_count(a)++;
		state.world.nation_get_allies_count(b)++;
		state.world.diplomatic_relation_set_are_allied(r, true);
	}

	if(a != state.local_player_nation && b != state.local_player_nation) {
		notification::post(state, notification::message{
			[from = a, to = b](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_alliance_starts_1", text::variable_type::x, to, text::variable_type::y, from);
			},
			"msg_alliance_starts_title",
			a, b, dcon::nation_id{},
			sys::message_base_type::alliance_starts
		});
	}
}

bool other_nation_is_influencing(sys::state& state, dcon::nation_id target, dcon::gp_relationship_id rel) {
	for(auto orel : state.world.nation_get_gp_relationship_as_influence_target(target)) {
		if(orel != rel && orel.get_influence() > 0.0f)
			return true;
	}
	return false;
}

bool can_accumulate_influence_with(sys::state& state, dcon::nation_id gp, dcon::nation_id target, dcon::gp_relationship_id rel) {
	if((state.world.gp_relationship_get_status(rel) & influence::is_banned) != 0)
		return false;
	if(military::has_truce_with(state, gp, target))
		return false;
	if(military::are_at_war(state, gp, target))
		return false;
	if(state.world.gp_relationship_get_influence(rel) >= state.defines.max_influence
		&& !other_nation_is_influencing(state, target, rel))
		return false;
	return true;
}

float get_base_shares(sys::state& state, dcon::gp_relationship_id gp, float total_gain, int32_t total_influence_shares) {
	if(total_influence_shares == 0)
		return 0.f;
	switch(state.world.gp_relationship_get_status(gp) & influence::priority_mask) {
	case influence::priority_one:
		return total_gain / float(total_influence_shares);
	case influence::priority_two:
		return 2.0f * total_gain / float(total_influence_shares);
	case influence::priority_three:
		return 3.0f * total_gain / float(total_influence_shares);
	default:
	case influence::priority_zero:
		return 0.0f;
	}
}

bool has_sphere_neighbour(sys::state& state, dcon::nation_id n, dcon::nation_id target) {
	for(auto g : state.world.nation_get_nation_adjacency(target)) {
		if(g.get_connected_nations(0) != target && g.get_connected_nations(0).get_in_sphere_of() == n)
			return true;
		if(g.get_connected_nations(1) != target && g.get_connected_nations(1).get_in_sphere_of() == n)
			return true;
	}
	return false;
}

float get_avg_non_colonial_literacy(sys::state& state, dcon::nation_id n) {
	auto literacy = state.world.nation_get_demographics(n, demographics::non_colonial_literacy);
	auto total_pop = state.world.nation_get_demographics(n, demographics::non_colonial_total);
	return total_pop > 0.0f ? literacy / total_pop : 0.0f;;
}

float get_avg_total_literacy(sys::state& state, dcon::nation_id n) {
	auto literacy = state.world.nation_get_demographics(n, demographics::literacy);
	auto total_pop = std::max(1.0f, state.world.nation_get_demographics(n, demographics::total));
	return total_pop > 0.0f ? literacy / total_pop : 0.0f;;
}

void update_influence(sys::state& state) {
	for(auto rel : state.world.in_gp_relationship) {
		if(rel.get_penalty_expires_date() == state.current_date) {
			rel.set_status(rel.get_status() & ~(influence::is_banned | influence::is_discredited));
		}
	}

	for(auto& grn : state.great_nations) {
		dcon::nation_fat_id n = fatten(state.world, grn.nation);
		if(!is_great_power(state, n))
			continue; // skip

		int32_t total_influence_shares = 0;
		for(auto rel : n.get_gp_relationship_as_great_power()) {
			if(can_accumulate_influence_with(state, n, rel.get_influence_target(), rel)) {
				switch(rel.get_status() & influence::priority_mask) {
				case influence::priority_one:
					total_influence_shares += 1;
					break;
				case influence::priority_two:
					total_influence_shares += 2;
					break;
				case influence::priority_three:
					total_influence_shares += 3;
					break;
				default:
				case influence::priority_zero:
					break;
				}
			}
		}

		if(total_influence_shares > 0) {
			/*
			The nation gets a daily increase of define:BASE_GREATPOWER_DAILY_INFLUENCE x (national-modifier-to-influence-gain + 1)
			x (technology-modifier-to-influence + 1). This is then divided among the nations they are accumulating influence with
			in proportion to their priority (so a target with priority 2 receives 2 shares instead of 1, etc).
			*/
			float total_gain = state.defines.base_greatpower_daily_influence *
				(1.0f + n.get_modifier_values(sys::national_mod_offsets::influence_modifier)) *
				(1.0f + n.get_modifier_values(sys::national_mod_offsets::influence));

			/*
			This influence value does not translate directly into influence with the target nation. Instead it is first multiplied
			by the following factor: 1 + define:DISCREDIT_INFLUENCE_GAIN_FACTOR (if discredited) +
			define:NEIGHBOUR_BONUS_INFLUENCE_PERCENT (if the nations are adjacent) +
			define:SPHERE_NEIGHBOUR_BONUS_INFLUENCE_PERCENT (if some member of the influencing nation's sphere is adjacent but not
			the influencing nation itself) + define:OTHER_CONTINENT_BONUS_INFLUENCE_PERCENT (if the influencing nation and the
			target have capitals on different continents) + define:PUPPET_BONUS_INFLUENCE_PERCENT (if the target is a vassal of
			the influencer) + relation-value / define:RELATION_INFLUENCE_MODIFIER + define:INVESTMENT_INFLUENCE_DEFENCE x
			fraction-of-influencer's-foreign-investment-out-of-total-foreign-investment +
			define:LARGE_POPULATION_INFLUENCE_PENALTY x target-population / define:LARGE_POPULATION_INFLUENCE_PENALTY_CHUNK (if
			the target nation has population greater than define:LARGE_POPULATION_LIMIT) + (1 - target-score / influencer-score)^0
			*/

			float gp_score = n.get_industrial_score() + n.get_military_score() + prestige_score(state, n);

			for(auto rel : n.get_gp_relationship_as_great_power()) {
				if(can_accumulate_influence_with(state, n, rel.get_influence_target(), rel)) {
					float base_shares = get_base_shares(state, rel, total_gain, total_influence_shares);
					if(base_shares <= 0.0f)
						continue; // skip calculations for priority zero nations

					float total_fi = nations::get_foreign_investment(state, rel.get_influence_target());
					auto gp_invest = state.world.unilateral_relationship_get_foreign_investment(
						state.world.get_unilateral_relationship_by_unilateral_pair(rel.get_influence_target(), n));

					float discredit_factor =
							(rel.get_status() & influence::is_discredited) != 0 ? state.defines.discredit_influence_gain_factor : 0.0f;
					float neighbor_factor = bool(state.world.get_nation_adjacency_by_nation_adjacency_pair(n, rel.get_influence_target()))
																			? state.defines.neighbour_bonus_influence_percent
																			: 0.0f;
					float sphere_neighbor_factor = nations::has_sphere_neighbour(state, n, rel.get_influence_target())
						? state.defines.sphere_neighbour_bonus_influence_percent
						: 0.0f;
					float continent_factor = n.get_capital().get_continent() != rel.get_influence_target().get_capital().get_continent()
																			 ? state.defines.other_continent_bonus_influence_percent
																			 : 0.0f;
					float puppet_factor = rel.get_influence_target().get_overlord_as_subject().get_ruler() == n
																		? state.defines.puppet_bonus_influence_percent
																		: 0.0f;
					float relationship_factor = state.world.diplomatic_relation_get_value(state.world.get_diplomatic_relation_by_diplomatic_pair(n, rel.get_influence_target())) / state.defines.relation_influence_modifier;

					float investment_factor = total_fi > 0.0f ? state.defines.investment_influence_defense * gp_invest / total_fi : 0.0f;
					float pop_factor =
							rel.get_influence_target().get_demographics(demographics::total) > state.defines.large_population_limit
									? state.defines.large_population_influence_penalty *
												rel.get_influence_target().get_demographics(demographics::total) /
												state.defines.large_population_influence_penalty_chunk
									: 0.0f;
					float score_factor = gp_score > 0.0f
						? std::max(1.0f - (rel.get_influence_target().get_industrial_score() + rel.get_influence_target().get_military_score() + prestige_score(state, rel.get_influence_target())) / gp_score,  0.0f)
						: 0.0f;

					float total_multiplier = 1.0f + discredit_factor + neighbor_factor + sphere_neighbor_factor + continent_factor + puppet_factor + relationship_factor + investment_factor + pop_factor + score_factor;

					auto gain_amount = base_shares * total_multiplier;

					/*
					Any influence that accumulates beyond the max (define:MAX_INFLUENCE) will be subtracted from the influence of
					the great power with the most influence (other than the influencing nation).
					*/
					
					rel.set_influence(rel.get_influence() + std::max(0.0f, gain_amount));
					if(rel.get_influence() > state.defines.max_influence) {
						auto overflow = rel.get_influence() - state.defines.max_influence;
						rel.set_influence(state.defines.max_influence);
						dcon::gp_relationship_id other_rel;
						for(auto orel : rel.get_influence_target().get_gp_relationship_as_influence_target()) {
							if(orel != rel) {
								if(orel.get_influence() > state.world.gp_relationship_get_influence(other_rel)) {
									other_rel = orel;
								}
							}
						}

						if(other_rel) {
							auto& orl_i = state.world.gp_relationship_get_influence(other_rel);
							state.world.gp_relationship_set_influence(other_rel, std::max(0.0f, orl_i - overflow));
						}
					}
				}
			}
		}
	}
}

bool can_put_flashpoint_focus_in_state(sys::state& state, dcon::state_instance_id s, dcon::nation_id fp_nation) {
	auto fp_focus_nation = fatten(state.world, fp_nation);
	auto si = fatten(state.world, s);

	auto fp_ident = fp_focus_nation.get_identity_from_identity_holder();

	auto owner = si.get_nation_from_state_ownership();

	if(owner == fp_nation)
		return false;

	if(nations::nation_accepts_culture(state, owner, fp_ident.get_primary_culture()))
		return false;
	if(fp_ident.get_is_not_releasable())
		return false;

	if(fp_focus_nation.get_rank() > uint16_t(state.defines.colonial_rank)) {
		auto d = si.get_definition();
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == owner) {
				if(state.world.get_core_by_prov_tag_key(p.get_province(), fp_ident))
					return true;
			}
		}
	}

	return false;
}
// updates the flashpoint tags for state instances, which is saved data, so should be run on update, and not in fill_unsaved_data
void update_flashpoint_tags(sys::state& state) {
	for(auto si : state.world.in_state_instance) {
		auto owner = si.get_nation_from_state_ownership();
		auto owner_tag = owner.get_identity_from_identity_holder();

		auto owner_accepts_culture = [&](dcon::culture_id c) {
			return owner.get_primary_culture() == c || nations::nation_accepts_culture(state, owner, c);
			};

		if(auto fp_focus_nation = si.get_nation_from_flashpoint_focus(); fp_focus_nation) {
			if(can_put_flashpoint_focus_in_state(state, si, fp_focus_nation)) {
				si.set_flashpoint_tag(fp_focus_nation.get_identity_from_identity_holder());
				continue; // done, skip remainder
			} else {
				// remove focus
				si.set_nation_from_flashpoint_focus(dcon::nation_id{});
			}
		}

		dcon::national_identity_id qualifying_tag;

		auto d = si.get_definition();
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == owner) {
				for(auto cores : p.get_province().get_core()) {
					if(!cores.get_identity().get_is_not_releasable()
						&& !owner_accepts_culture(cores.get_identity().get_primary_culture())
						&& (!qualifying_tag
							|| si.get_demographics(demographics::to_key(state, cores.get_identity().get_primary_culture())) >
							si.get_demographics(demographics::to_key(state, state.world.national_identity_get_primary_culture(qualifying_tag))))) {

						qualifying_tag = cores.get_identity();
					}
				}
			}
		}

		si.set_flashpoint_tag(qualifying_tag);

	}
}

void monthly_flashpoint_update(sys::state& state) {
	/*
	Whether a state contains a flashpoint depends on: whether a province in the state contains a core other than that of its owner
	and of a tag that is marked as releasable (i.e. not a cultural union core), and which has a primary culture that is not the
	primary culture or an accepted culture of its owner. If any core qualifies, the state is considered to be a flashpoint, and
	its default flashpoint tag will be the qualifying core whose culture has the greatest population in the state.
	*/


	// set which nations contain such states
	state.world.execute_serial_over_nation(
			[&](auto ids) { state.world.nation_set_has_flash_point_state(ids, ve::mask_vector(false)); });
	for(auto si : state.world.in_state_instance) {
		if(si.get_flashpoint_tag()) {
			si.get_nation_from_state_ownership().set_has_flash_point_state(true);
		}
	}

	// set which of those nations are targeted by some cb
	state.world.execute_serial_over_nation(
			[&](auto ids) { state.world.nation_set_is_target_of_some_cb(ids, ve::mask_vector(false)); });

	for(auto target : state.world.in_nation) {
		if(target.get_has_flash_point_state()) {
			// check all other nations to see if they hold a cb
			for(auto actor : state.world.in_nation) {
				auto owned = actor.get_province_ownership();
				if(actor != target && owned.begin() != owned.end()) {
					if(military::can_use_cb_against(state, actor, target)) {
						target.set_is_target_of_some_cb(true);
						break;
					}
				}
			}
		}
	}
}

void daily_update_flashpoint_tension(sys::state& state) {
	for(auto si : state.world.in_state_instance) {
		if(si.get_flashpoint_tag()) {
			float total_increase = 0.0f;
			/*
			- If at least one nation has a CB on the owner of a flashpoint state, the tension increases by define:TENSION_FROM_CB
			per day.
			*/
			if(si.get_nation_from_state_ownership().get_is_target_of_some_cb()) {
				total_increase += state.defines.tension_from_cb;
			}
			/*
			- If there is an independence movement within the nation owning the state for the independence tag, the tension will
			increase by movement-radicalism x define:TENSION_FROM_MOVEMENT x
			fraction-of-population-in-state-with-same-culture-as-independence-tag x movement-support / 4000, up to a maximum of
			define:TENSION_FROM_MOVEMENT_MAX per day.
			*/
			auto mov = rebel::get_movement_by_independence(state, si.get_nation_from_state_ownership(), si.get_flashpoint_tag());
			if(mov) {
				auto radicalism = state.world.movement_get_radicalism(mov);
				auto support = state.world.movement_get_pop_support(mov);
				auto state_pop = si.get_demographics(demographics::total);
				auto pop_of_culture = si.get_demographics(demographics::to_key(state, si.get_flashpoint_tag().get_primary_culture()));
				if(state_pop > 0) {
					total_increase += std::min(state.defines.tension_from_movement_max,
							state.defines.tension_from_movement * radicalism * pop_of_culture * support / (state_pop * 4000.0f));
				}
			}

			/*
			- Any flashpoint focus increases the tension by the amount listed in it per day.
			*/
			if(si.get_nation_from_flashpoint_focus()) {
				total_increase += state.national_definitions.flashpoint_amount;
			}

			/*
			- Tension increases by define:TENSION_DECAY per day (this is negative, so this is actually a daily decrease).
			*/
			total_increase += state.defines.tension_decay;

			/*
			- Tension increased by define:TENSION_WHILE_CRISIS per day while a crisis is ongoing.
			*/
			if(state.current_crisis_state == sys::crisis_state::inactive) {
				total_increase += state.defines.tension_while_crisis;
			}

			/*
			- If the state is owned by a great power, tension is increased by define:RANK_X_TENSION_DECAY per day
			*/
			if(auto rank = si.get_nation_from_state_ownership().get_rank();
					uint16_t(1) <= rank && rank <= uint16_t(int32_t(state.defines.great_nations_count))) {
				static float rank_amounts[8] = {state.defines.rank_1_tension_decay, state.defines.rank_2_tension_decay,
						state.defines.rank_3_tension_decay, state.defines.rank_4_tension_decay, state.defines.rank_5_tension_decay,
						state.defines.rank_6_tension_decay, state.defines.rank_7_tension_decay, state.defines.rank_8_tension_decay};

				total_increase += rank_amounts[rank - 1];
			}

			/*
			- For each great power at war or disarmed on the same continent as either the owner or the state, tension is increased
			by define:AT_WAR_TENSION_DECAY per day.
			*/
			for(auto& gp : state.great_nations) {
				if(state.world.nation_get_is_at_war(gp.nation) ||
						(state.world.nation_get_disarmed_until(gp.nation) &&
								state.current_date <= state.world.nation_get_disarmed_until(gp.nation))) {
					auto continent = state.world.province_get_continent(state.world.nation_get_capital(gp.nation));
					if(si.get_capital().get_continent() == continent ||
							si.get_nation_from_state_ownership().get_capital().get_continent() == continent) {
						total_increase += state.defines.at_war_tension_decay;
						break;
					}
				}
			}

			/*
			- Tension ranges between 0 and 100
			*/
			si.set_flashpoint_tension(std::clamp(si.get_flashpoint_tension() + total_increase, 0.0f, 100.0f));
		} else {
			si.set_flashpoint_tension(0.0f);
		}
	}
}

void crisis_add_wargoal(std::vector<sys::full_wg>& list, sys::full_wg wg) {
	assert(wg.cb /* && wg.added_by*/ && wg.target_nation);
	for(unsigned i = 0; i < list.size(); i++) {
		if(!list[i].cb) {
			list[i] = wg;
			break;
		}
	}
}

void cleanup_crisis(sys::state& state) {
	for(unsigned i = 0; i < state.crisis_attacker_wargoals.size(); i++) {
		if(state.crisis_attacker_wargoals[i].cb) {
			state.crisis_attacker_wargoals[i] = sys::full_wg{};
		}
		else {
			break;
		}
	}
	for(unsigned i = 0; i < state.crisis_defender_wargoals.size(); i++) {
		if(state.crisis_defender_wargoals[i].cb) {
			state.crisis_defender_wargoals[i] = sys::full_wg{};
		}
		else {
			break;
		}
	}

	state.current_crisis = sys::crisis_type::none;
	state.crisis_state_instance = dcon::state_instance_id{};
	state.last_crisis_end_date = state.current_date;
	state.current_crisis_state = sys::crisis_state::inactive;
	state.crisis_last_checked_gp = 0;
	state.crisis_attacker = dcon::nation_id{};
	state.crisis_defender = dcon::nation_id{};

	for(auto& par : state.crisis_participants) {
		if(par.id) {
			par.id = dcon::nation_id{};
			par.merely_interested = false;
			par.supports_attacker = false;
		} else {
			break;
		}
	}

	state.crisis_temperature = 0.0f;
	state.crisis_war = dcon::war_id{};
	state.primary_crisis_attacker = dcon::nation_id{};
	state.primary_crisis_defender = dcon::nation_id{};
}

void add_as_primary_crisis_defender(sys::state& state, dcon::nation_id n) {
	state.primary_crisis_defender = n;

	notification::post(state, notification::message{
		[n](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_crisis_defender_1", text::variable_type::x, n);

		},
		"msg_crisis_defender_title",
		n, dcon::nation_id{}, dcon::nation_id{},
		sys::message_base_type::crisis_defender_backer
	});
}

void add_as_primary_crisis_attacker(sys::state& state, dcon::nation_id n) {
	state.primary_crisis_attacker = n;

	notification::post(state, notification::message{
		[n](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_crisis_attacker_1", text::variable_type::x, n);

		},
		"msg_crisis_attacker_title",
		n, dcon::nation_id{}, dcon::nation_id{},
		sys::message_base_type::crisis_attacker_backer
	});
}

void ask_to_defend_in_crisis(sys::state& state, dcon::nation_id n) {
	if(state.world.nation_get_is_at_war(n)) { // ineligible
		reject_crisis_participation(state);
	} else {
		diplomatic_message::message m;
		memset(&m, 0, sizeof(diplomatic_message::message));
		m.type = diplomatic_message::type::be_crisis_primary_defender;
		m.to = n;
		m.from = state.crisis_defender;
		diplomatic_message::post(state, m);
	}
}

void ask_to_attack_in_crisis(sys::state& state, dcon::nation_id n) {
	if(state.world.nation_get_is_at_war(n)) { // ineligible
		reject_crisis_participation(state);
	} else {
		diplomatic_message::message m;
		memset(&m, 0, sizeof(diplomatic_message::message));
		m.type = diplomatic_message::type::be_crisis_primary_attacker;
		m.to = n;
		m.from = state.crisis_attacker;
		if(state.current_crisis == sys::crisis_type::liberation) {
			auto from = state.world.state_instance_get_flashpoint_tag(state.crisis_state_instance);
			m.from = from.get_nation_from_identity_holder();
		}
		diplomatic_message::post(state, m);
	}
}

void reject_crisis_participation(sys::state& state) {
	++state.crisis_last_checked_gp;
	if(state.crisis_last_checked_gp < state.great_nations.size() &&
			(state.great_nations[state.crisis_last_checked_gp].nation == state.primary_crisis_attacker ||
					state.great_nations[state.crisis_last_checked_gp].nation == state.primary_crisis_defender)) {
		++state.crisis_last_checked_gp;
	}
	if(state.current_crisis_state == sys::crisis_state::finding_attacker && !state.primary_crisis_attacker) {
		if(state.crisis_last_checked_gp >= state.great_nations.size()) {
			// If initiated by some non-GP country and no GP backer - let the initiator run the crisis
			if(state.crisis_attacker) {
				state.primary_crisis_attacker = state.crisis_attacker;
			}
			// else - fizzle
			else {
				notification::post(state, notification::message{
					[](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "msg_crisis_fizzle_1");
					},
					"msg_crisis_fizzle_title",
					state.local_player_nation,dcon::nation_id{}, dcon::nation_id{},
					sys::message_base_type::crisis_fizzle
				});

				cleanup_crisis(state);
			}
		} else {
			ask_to_attack_in_crisis(state, state.great_nations[state.crisis_last_checked_gp].nation);
		}
	} else if(state.current_crisis_state == sys::crisis_state::finding_defender && !state.primary_crisis_defender) {
		if(state.crisis_last_checked_gp >= state.great_nations.size()) {
			// TODO: notify resolution

			// If initiated against some non-GP country and no GP backer - let the target run the crisis
			if(state.crisis_defender) {
				state.primary_crisis_defender = state.crisis_defender;
			}
			// else - fizzle
			else {
				notification::post(state, notification::message{
					[](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "msg_crisis_fizzle_2");
					},
					"msg_crisis_fizzle_title",
					state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
					sys::message_base_type::crisis_fizzle
				});

				auto first_wg = state.crisis_attacker_wargoals.at(0);
				military::implement_war_goal(state, dcon::war_id{}, first_wg.cb, first_wg.added_by,
							first_wg.target_nation, first_wg.secondary_nation, first_wg.state,
							first_wg.wg_tag);

				cleanup_crisis(state);
			}
		} else {
			ask_to_defend_in_crisis(state, state.great_nations[state.crisis_last_checked_gp].nation);
		}
	}
}

void cleanup_crisis_peace_offer(sys::state& state, dcon::peace_offer_id peace) {
	auto wg = state.world.peace_offer_get_peace_offer_item(peace);
	while(wg.begin() != wg.end()) {
		state.world.delete_wargoal((*wg.begin()).get_wargoal());
	}
	state.world.delete_peace_offer(peace);
}

void accept_crisis_peace_offer(sys::state& state, dcon::nation_id from, dcon::nation_id to, dcon::peace_offer_id peace) {

	military::implement_peace_offer(state, peace);

	cleanup_crisis_peace_offer(state, peace);
	cleanup_crisis(state);
}

void crisis_state_transition(sys::state& state, sys::crisis_state new_state) {
	state.current_crisis_state = new_state;

	if(new_state == sys::crisis_state::finding_attacker) {
		if(!state.primary_crisis_attacker) {
			state.crisis_last_checked_gp = state.great_nations[0].nation != state.primary_crisis_defender ? 0 : 1;
			ask_to_attack_in_crisis(state, state.great_nations[state.crisis_last_checked_gp].nation);
		}
	}
	else if(new_state == sys::crisis_state::finding_defender) {
		if(!state.primary_crisis_defender) {
			state.crisis_last_checked_gp = state.great_nations[0].nation != state.primary_crisis_attacker ? 0 : 1;
			ask_to_defend_in_crisis(state, state.great_nations[state.crisis_last_checked_gp].nation);
		}
	}
	else if(new_state == sys::crisis_state::heating_up) {
		state.crisis_last_checked_gp = 0;

		/*
		A GP that is not disarmed and not at war with war exhaustion less than define:CRISIS_INTEREST_WAR_EXHAUSTION_LIMIT and
		is either on the same continent as the crisis target, crisis attacker, or state (or with a few other special cases: I
		think European GPs are interested in everything, and I think north and south America are considered one continent
		here) is considered to be interested. When a GP becomes interested it gets a `on_crisis_declare_interest` event.
		*/
		/*
		When a GP becomes interested it gets a `on_crisis_declare_interest` event.
		*/
		state.crisis_participants[0].id = state.primary_crisis_attacker;
		state.crisis_participants[0].supports_attacker = true;
		state.crisis_participants[0].merely_interested = false;
		state.crisis_participants[1].id = state.primary_crisis_defender;
		state.crisis_participants[1].supports_attacker = false;
		state.crisis_participants[1].merely_interested = false;

		auto first_wg = state.crisis_attacker_wargoals.at(0);

		auto crisis_state_continent = [&]() {
			if(auto p = state.world.state_definition_get_abstract_state_membership(first_wg.state); p.begin() != p.end()) {
				return (*p.begin()).get_province().get_continent().id;
			}
			return dcon::modifier_id{};
			}();

		auto crisis_defender_continent =
			state.world.province_get_continent(state.world.nation_get_capital(state.primary_crisis_defender));
		uint32_t added_count = 2;

		for(auto& gp : state.great_nations) {
			if(gp.nation != state.primary_crisis_attacker && gp.nation != state.primary_crisis_defender &&
					!state.world.nation_get_is_at_war(gp.nation) &&
					state.world.nation_get_war_exhaustion(gp.nation) < state.defines.crisis_interest_war_exhaustion_limit) {
				auto cap_con = state.world.province_get_continent(state.world.nation_get_capital(gp.nation));
				if(cap_con == state.province_definitions.europe || cap_con == crisis_state_continent ||
						cap_con == crisis_defender_continent ||
						(cap_con == state.province_definitions.north_america &&
							crisis_state_continent == state.province_definitions.south_america) ||
						(cap_con == state.province_definitions.north_america &&
							crisis_defender_continent == state.province_definitions.south_america) ||
						(cap_con == state.province_definitions.south_america &&
							crisis_state_continent == state.province_definitions.north_america) ||
						(cap_con == state.province_definitions.south_america &&
							crisis_defender_continent == state.province_definitions.north_america)) {

					/*
					// apparently the event takes care of adding people
					state.crisis_participants[added_count].id = gp.nation;
					state.crisis_participants[added_count].supports_attacker = false;
					state.crisis_participants[added_count].merely_interested = true;
					++added_count;
					*/

					event::fire_fixed_event(state, state.national_definitions.on_crisis_declare_interest, trigger::to_generic(gp.nation),
							event::slot_type::nation, gp.nation, -1, event::slot_type::none);
				}
			}
		}

		// auto join ais
		dcon::nation_id secondary_attacker = state.crisis_attacker;
		dcon::nation_id secondary_defender = state.crisis_defender;

		for(auto& i : state.crisis_participants) {
			if(i.id && i.merely_interested == true && state.world.nation_get_is_player_controlled(i.id) == false) {
				if(state.world.nation_get_ai_rival(i.id) == state.primary_crisis_attacker
					|| nations::are_allied(state, i.id, state.primary_crisis_defender)
					|| state.world.nation_get_ai_rival(i.id) == secondary_attacker
					|| nations::are_allied(state, i.id, secondary_defender)
					|| state.world.nation_get_in_sphere_of(secondary_defender) == i.id) {

					i.merely_interested = false;
					i.supports_attacker = false;

					notification::post(state, notification::message{
						[source = i.id](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_crisis_vol_join_2", text::variable_type::x, source);
						},
						"msg_crisis_vol_join_title",
						i.id, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::crisis_voluntary_join
					});
				} else if(state.world.nation_get_ai_rival(i.id) == state.primary_crisis_defender
					|| nations::are_allied(state, i.id, state.primary_crisis_attacker)
					|| state.world.nation_get_ai_rival(i.id) == secondary_defender
					|| nations::are_allied(state, i.id, secondary_attacker)
					|| state.world.nation_get_in_sphere_of(secondary_attacker) == i.id) {

					i.merely_interested = false;
					i.supports_attacker = true;

					notification::post(state, notification::message{
						[source = i.id](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_crisis_vol_join_1", text::variable_type::x, source);
						},
						"msg_crisis_vol_join_title",
						i.id, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::crisis_voluntary_join
					});
				}
			}
			if(!i.id)
				break;;
		}
	}
}

void update_crisis(sys::state& state) {
	/*
	A crisis may not start until define:CRISIS_COOLDOWN_MONTHS months after the last crisis or crisis war has ended.
	*/

	if(state.great_nations.size() <= 2)
		return; // not enough nations obviously

	// filter out invalid crises
	// TODO: Go over each wargoal. If both sides have no valid wargoals - cleanup
	/*if(state.current_crisis == sys::crisis_type::colonial) {
		auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
		auto num_colonizers = colonizers.end() - colonizers.begin();
		if(num_colonizers < 2) {
			cleanup_crisis(state);
		}
	} else if(state.current_crisis == sys::crisis_type::liberation) {
		auto state_owner = state.world.state_instance_get_nation_from_state_ownership(state.current_crisis_state);
		if(state_owner == state.primary_crisis_attacker ||
				state.world.nation_get_identity_from_identity_holder(state_owner) == state.crisis_liberation_tag) {
			cleanup_crisis(state);
		}
	}*/

	// Current crisis state = no crisis
	if(state.current_crisis_state == sys::crisis_state::inactive && !state.crisis_war && (!state.last_crisis_end_date ||
			state.last_crisis_end_date + 31 * int32_t(state.defines.crisis_cooldown_months) < state.current_date)) {
		// try to start a crisis
		// determine type if any

		/*
		Liberation crisis.
		When a crisis becomes possible, we first check each of the three states with the highest tension > 50 where neither the
		owner of the state nor the nation associated with the flashpoint (if any) is at war. I believe the probability of a crisis
		happening in any of those states is 0.001 x define:CRISIS_BASE_CHANCE x state-tension / 100. If this turns into a crisis,
		the tension in the state is immediately zeroed.
		*/
		std::vector<dcon::state_instance_id> most_likely_states;
		for(auto si : state.world.in_state_instance) {
			auto owner_war = si.get_nation_from_state_ownership().get_is_at_war();
			auto ft = si.get_flashpoint_tag();
			auto ften = si.get_flashpoint_tension();
			auto ihold_at_war = si.get_flashpoint_tag().get_nation_from_identity_holder().get_is_at_war();

			if(si.get_nation_from_state_ownership().get_is_at_war() == false && si.get_flashpoint_tag() &&
					si.get_flashpoint_tension() > 50.0f &&
					si.get_flashpoint_tag().get_nation_from_identity_holder().get_is_at_war() == false) {
				most_likely_states.push_back(si);
			}
		}
		std::sort(most_likely_states.begin(), most_likely_states.end(), [&](dcon::state_instance_id a, dcon::state_instance_id b) {
			auto tension_diff =
					state.world.state_instance_get_flashpoint_tension(a) - state.world.state_instance_get_flashpoint_tension(b);
			if(tension_diff != 0.0f) {
				return tension_diff > 0.0f;
			} else {
				return a.index() < b.index(); // create a total order
			}
		});
		for(uint32_t i = 0; i < 3 && i < most_likely_states.size(); ++i) {
			auto chance = uint32_t(state.defines.crisis_base_chance *
														 state.world.state_instance_get_flashpoint_tension(most_likely_states[i])); // out of 10,000
			auto rvalue = rng::get_random(state, uint32_t(most_likely_states[i].index())) % 10000;
			if(rvalue < chance) {
				// Start liberation crisis
				state.crisis_state_instance = most_likely_states[i];
				auto crisis_liberation_tag = state.world.state_instance_get_flashpoint_tag(state.crisis_state_instance);
				state.world.state_instance_set_flashpoint_tension(state.crisis_state_instance, 0.0f);
				state.current_crisis = sys::crisis_type::liberation;
				auto crisis_state_instance = most_likely_states[i];
				state.world.state_instance_set_flashpoint_tension(crisis_state_instance, 0.0f);
				auto owner = state.world.state_instance_get_nation_from_state_ownership(crisis_state_instance);
				if (owner && state.world.nation_get_is_great_power(owner)) {
					state.primary_crisis_defender = owner;
				}
				state.crisis_defender = owner;

				crisis_add_wargoal(state.crisis_attacker_wargoals, sys::full_wg{
						dcon::nation_id{}, // added_by;
						owner, // target_nation;
						dcon::nation_id{}, //  secondary_nation;
						crisis_liberation_tag, // wg_tag;
						state.world.state_instance_get_definition(crisis_state_instance), // state;
						state.military_definitions.crisis_liberate // cb
				});

				crisis_state_transition(state, sys::crisis_state::finding_attacker);
				notification::post(state, notification::message{
					[st = crisis_state_instance](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "msg_new_crisis_1", text::variable_type::x, st);
					},
					"msg_new_crisis_title",
					state.local_player_nation,
					crisis_liberation_tag.get_nation_from_identity_holder(),
					owner,
					sys::message_base_type::crisis_starts
				});

				break;
			}
		}
		if(state.current_crisis_state == sys::crisis_state::inactive) {
			/*
			Failing that, any contested colonial region where neither colonizer is at war will become a crisis and the colonial
			"temperature" has reached 100.
			*/
			// try colonial crisis
			for(auto sd : state.world.in_state_definition) {
				if(sd.get_colonization_temperature() >= 100.0f) {
					auto colonizers = sd.get_colonization();
					auto num_colonizers = colonizers.end() - colonizers.begin();
					if(num_colonizers == 2) {
						auto attacking_colonizer = (*colonizers.begin()).get_colonizer();
						auto defending_colonizer = (*(colonizers.begin() + 1)).get_colonizer();
						state.current_crisis = sys::crisis_type::colonial;
						sd.set_colonization_temperature(0.0f);

						// In colonial crisis there should be two simmetrical wargoals: attacker vs defender and defender vs attacker.

						crisis_add_wargoal(state.crisis_attacker_wargoals, sys::full_wg{
							attacking_colonizer, // added_by;
								defending_colonizer, // target_nation;
								dcon::nation_id{}, //  secondary_nation;
								dcon::national_identity_id{}, // wg_tag;
								sd, // state;
								state.military_definitions.crisis_colony // cb
						});

						crisis_add_wargoal(state.crisis_defender_wargoals, sys::full_wg{
							defending_colonizer, // added_by;
								attacking_colonizer, // target_nation;
								dcon::nation_id{}, //  secondary_nation;
								dcon::national_identity_id{}, // wg_tag;
								sd, // state;
								state.military_definitions.crisis_colony // cb
						});

						state.crisis_attacker = attacking_colonizer;
						if(attacking_colonizer.get_is_great_power()) {
							state.primary_crisis_attacker = attacking_colonizer;
						}
						state.crisis_defender = defending_colonizer;
						if(defending_colonizer.get_is_great_power()) {
							state.primary_crisis_defender = defending_colonizer;
						}

						crisis_state_transition(state, sys::crisis_state::finding_attacker);
						notification::post(state, notification::message{
							[st = sd](sys::state& state, text::layout_base& contents) {
								text::add_line(state, contents, "msg_new_crisis_2", text::variable_type::x, st);
							},
							"msg_new_crisis_title",
							state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
							sys::message_base_type::crisis_starts
						});
						break;
					}
				}
			}
		}

		if(state.current_crisis_state == sys::crisis_state::inactive) {
			state.last_crisis_end_date = state.current_date;
			return;
		}

		state.crisis_temperature = 0.0f;
		//crisis_state_transition(state, sys::crisis_state::finding_attacker);
	}
	// Current crisis state = sys::crisis_state::finding_attacker
	else if(state.current_crisis_state == sys::crisis_state::finding_attacker) {
		if(state.primary_crisis_attacker) { // found an attacker
			crisis_state_transition(state, sys::crisis_state::finding_defender);
		}
	}
	// Current crisis state = sys::crisis_state::finding_defender
	else if(state.current_crisis_state == sys::crisis_state::finding_defender) {
		if(state.primary_crisis_defender) { // found defender
			crisis_state_transition(state, sys::crisis_state::heating_up);
		}
	}
	// Current crisis state = sys::crisis_state::heating_up
	else if(state.current_crisis_state == sys::crisis_state::heating_up) {
		/*
		Every day where there is at least one defending GP and one attacking GP, the crisis temperature increases by
		define:CRISIS_TEMPERATURE_INCREASE x define:CRISIS_TEMPERATURE_PARTICIPANT_FACTOR x the ratio of GPs currently involved in
		the crisis to the total number of interested GPs and participating GPs.
		*/

		int32_t total = 0;
		int32_t participants = 0;
		for(auto& par : state.crisis_participants) {
			if(par.id) {
				++total;
				if(!par.merely_interested)
					++participants;
			} else {
				break;
			}
		}

		assert(total != 0);
		state.crisis_temperature += state.defines.crisis_temperature_increase * state.defines.crisis_temperature_participant_factor *
																float(participants) / float(total);
		// Start crisis war
		if(state.crisis_temperature >= 100) {
			dcon::war_id war;

			assert(state.crisis_attacker_wargoals.size() > 0);

			auto first_wg = state.crisis_attacker_wargoals.at(0);
			war = military::create_war(state, state.crisis_attacker,
						state.crisis_defender,
						first_wg.cb, first_wg.state,
						first_wg.wg_tag, first_wg.secondary_nation);

			if(state.crisis_attacker != state.primary_crisis_attacker) {
				military::add_to_war(state, war, state.primary_crisis_attacker, true);
				state.world.war_set_primary_attacker(war, state.primary_crisis_attacker);
			}
			if(state.crisis_defender != state.primary_crisis_defender) {
				military::add_to_war(state, war, state.primary_crisis_defender, false);
				state.world.war_set_primary_defender(war, state.primary_crisis_defender);
			}

			for(auto& par : state.crisis_participants) {
				if(par.id) {
					if(!par.merely_interested && par.id != state.primary_crisis_attacker && par.id != state.primary_crisis_defender) {
						military::add_to_war(state, war, par.id, par.supports_attacker);
					}
				} else {
					break;
				}
			}

			for(auto wg : state.crisis_attacker_wargoals) {
				if(!wg.cb) {
					break;
				}
				if(wg.cb == first_wg.cb && wg.state == first_wg.state && wg.wg_tag == first_wg.wg_tag && wg.secondary_nation == first_wg.secondary_nation) {
					continue;
				}
				military::add_wargoal(state, war, wg.added_by, wg.target_nation, wg.cb,
						wg.state, wg.wg_tag, wg.secondary_nation);
			}
			for(auto wg : state.crisis_defender_wargoals) {
				if(!wg.cb) {
					break;
				}
				military::add_wargoal(state, war, wg.added_by, wg.target_nation, wg.cb,
						wg.state, wg.wg_tag, wg.secondary_nation);
			}
			
			/*
			If the crisis becomes a war, any interested GP which did not take a side loses
			(years-after-start-date x define:CRISIS_DID_NOT_TAKE_SIDE_PRESTIGE_FACTOR_YEAR +
			define:CRISIS_DID_NOT_TAKE_SIDE_PRESTIGE_FACTOR_BASE) as a fraction of their prestige.
			*/

			float p_factor = state.defines.crisis_did_not_take_side_prestige_factor_base +
											 state.defines.crisis_did_not_take_side_prestige_factor_year * float(state.current_date.value) / float(365);

			for(auto& par : state.crisis_participants) {
				if(!par.id)
					break;

				if(par.merely_interested) {
					nations::adjust_prestige(state, par.id, nations::prestige_score(state, par.id) * p_factor);
					// TODO: notify
				}
			}

			cleanup_crisis(state);
			state.world.war_set_is_crisis_war(war, true);

			if(state.military_definitions.great_wars_enabled) {
				int32_t gp_attackers = 0;
				int32_t gp_defenders = 0;

				for(auto par : state.world.war_get_war_participant(war)) {
					if(nations::is_great_power(state, par.get_nation())) {
						if(par.get_is_attacker())
							++gp_attackers;
						else
							++gp_defenders;
					}
				}

				if(gp_attackers >= 2 && gp_defenders >= 2) {
					state.world.war_set_is_great(war, true);
					state.world.war_set_name(war, state.lookup_key(std::string_view{ "great_war_name" }));
				}
			}

			state.crisis_war = war;

			notification::post(state, notification::message{
				[pa = state.world.war_get_primary_attacker(war), pd = state.world.war_get_primary_defender(war), name = state.world.war_get_name(war), tag = state.world.war_get_over_tag(war), st = state.world.war_get_over_state(war)](sys::state& state, text::layout_base& contents) {

					text::substitution_map sub;

					text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
					text::add_to_substitution_map(sub, text::variable_type::second, text::get_adjective(state, pd));
					text::add_to_substitution_map(sub, text::variable_type::second_country, pd);
					text::add_to_substitution_map(sub, text::variable_type::first, text::get_adjective(state, pa));
					text::add_to_substitution_map(sub, text::variable_type::third, tag);
					text::add_to_substitution_map(sub, text::variable_type::state, st);
					text::add_to_substitution_map(sub, text::variable_type::country_adj, state.world.national_identity_get_adjective(tag));

					std::string resolved_war_name = text::resolve_string_substitution(state, name, sub);
					text::add_line(state, contents, "msg_crisis_escalates_1", text::variable_type::x, std::string_view{resolved_war_name});
				},
				"msg_crisis_escalates_title",
				state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
				sys::message_base_type::crisis_becomes_war
			});
		}
	}
}

void update_pop_acceptance(sys::state& state, dcon::nation_id n) {
	auto pc = state.world.nation_get_primary_culture(n);
	for(auto pr : state.world.nation_get_province_ownership(n)) {
		for(auto pop : pr.get_province().get_pop_location()) {
			[&]() {
				if(pc == pop.get_pop().get_culture()) {
					pop.get_pop().set_is_primary_or_accepted_culture(true);
					return;
				}
				if(state.world.nation_get_accepted_cultures(n, pop.get_pop().get_culture()) == true) {
					pop.get_pop().set_is_primary_or_accepted_culture(true);
					return;
				}
				pop.get_pop().set_is_primary_or_accepted_culture(false);
			}();
		}
	}
}

void liberate_nation_from(sys::state& state, dcon::national_identity_id liberated, dcon::nation_id from) {
	if(!liberated)
		return;
	auto holder = state.world.national_identity_get_nation_from_identity_holder(liberated);
	bool nation_was_created = false;
	if(!holder) {
		holder = state.world.create_nation();
		state.world.nation_set_identity_from_identity_holder(holder, liberated);
		nation_was_created = true;
	}
	auto lprovs = state.world.nation_get_province_ownership(holder);
	if(lprovs.begin() == lprovs.end()) {
		nations::create_nation_based_on_template(state, holder, from);
		nation_was_created = true;
	}
	for(auto c : state.world.national_identity_get_core(liberated)) {
		if(c.get_province().get_nation_from_province_ownership() == from) {
			province::change_province_owner(state, c.get_province(), holder);
		}
	}

	state.world.nation_set_capital(holder, province::pick_capital(state, holder));
	if(state.world.province_get_nation_from_province_ownership(state.world.nation_get_capital(from)) != from) {
		state.world.nation_set_capital(from, province::pick_capital(state, from));
	}
}

void release_nation_from(sys::state& state, dcon::national_identity_id liberated, dcon::nation_id from) {
	if(!liberated)
		return;
	auto holder = state.world.national_identity_get_nation_from_identity_holder(liberated);
	auto source_tag = state.world.nation_get_identity_from_identity_holder(from);
	bool nation_was_created = false;
	if(!holder) {
		holder = state.world.create_nation();
		state.world.nation_set_identity_from_identity_holder(holder, liberated);
		nation_was_created = true;
	}
	auto lprovs = state.world.nation_get_province_ownership(holder);
	if(lprovs.begin() == lprovs.end()) {
		nations::create_nation_based_on_template(state, holder, from);
		nation_was_created = true;
	}
	for(auto c : state.world.national_identity_get_core(liberated)) {
		if(c.get_province().get_nation_from_province_ownership() == from &&
				!(state.world.get_core_by_prov_tag_key(c.get_province(), source_tag))) {
			province::change_province_owner(state, c.get_province(), holder);
		}
	}
	if(state.world.province_get_nation_from_province_ownership(state.world.nation_get_capital(from)) != from) {
		state.world.nation_set_capital(from, province::pick_capital(state, from));
	}
}

void remove_cores_from_owned(sys::state& state, dcon::nation_id n, dcon::national_identity_id tag) {
	for(auto prov : state.world.nation_get_province_ownership(n)) {
		if(auto core = state.world.get_core_by_prov_tag_key(prov.get_province(), tag); core) {
			state.world.delete_core(core);
		}
	}
}

void perform_nationalization(sys::state& state, dcon::nation_id n) {
	for(auto rel : state.world.nation_get_unilateral_relationship_as_target(n)) {
		if(rel.get_foreign_investment() > 0.0f) {
			event::fire_fixed_event(state, state.national_definitions.on_my_factories_nationalized,
					trigger::to_generic(rel.get_source().id), event::slot_type::nation, rel.get_source(), trigger::to_generic(n),
					event::slot_type::nation);
			rel.set_foreign_investment(0.0f);
		}
	}
}

void adjust_influence(sys::state& state, dcon::nation_id great_power, dcon::nation_id target, float delta) {
	if(great_power == target)
		return;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(target, great_power);
	if(!rel) {
		rel = state.world.force_create_gp_relationship(target, great_power);
	}
	auto& inf = state.world.gp_relationship_get_influence(rel);
	state.world.gp_relationship_set_influence(rel, std::clamp(inf + delta, 0.0f, state.defines.max_influence));
}

void adjust_influence_with_overflow(sys::state& state, dcon::nation_id great_power, dcon::nation_id target, float delta) {
	if(state.world.nation_get_owned_province_count(great_power) == 0 || state.world.nation_get_owned_province_count(target) == 0)
		return;
	if(great_power == target)
		return;
	if(state.world.nation_get_is_great_power(target) || !state.world.nation_get_is_great_power(great_power))
		return;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(target, great_power);
	if(!rel) {
		rel = state.world.force_create_gp_relationship(target, great_power);
	}
	auto& inf = state.world.gp_relationship_get_influence(rel);
	state.world.gp_relationship_set_influence(rel, inf + delta);

	while(inf < 0) {
		if(state.world.nation_get_in_sphere_of(target) == great_power) {
			state.world.gp_relationship_set_influence(rel, inf + state.defines.addtosphere_influence_cost);
			state.world.nation_set_in_sphere_of(target, dcon::nation_id{});

			auto& l = state.world.gp_relationship_get_status(rel);
			state.world.gp_relationship_set_status(rel, uint8_t(nations::influence::decrease_level(l)));
		} else {
			state.world.gp_relationship_set_influence(rel, inf + state.defines.increaseopinion_influence_cost);

			auto& l = state.world.gp_relationship_get_status(rel);
			state.world.gp_relationship_set_status(rel, uint8_t(nations::influence::decrease_level(l)));
		}
	}

	while(inf > state.defines.max_influence) {
		if(state.world.nation_get_in_sphere_of(target) != great_power) {
			state.world.gp_relationship_set_influence(rel, inf - state.defines.removefromsphere_influence_cost);
			auto affected_gp = state.world.nation_get_in_sphere_of(target);
			state.world.nation_set_in_sphere_of(target, dcon::nation_id{});
			// if the target was in a previous GP's sphere, update their state
			if(bool(affected_gp))
			{
				auto orel = state.world.get_gp_relationship_by_gp_influence_pair(target, affected_gp);
				auto& l = state.world.gp_relationship_get_status(orel);
				state.world.gp_relationship_set_status(orel, uint8_t(nations::influence::decrease_level(l)));
			}
		} else if((state.world.gp_relationship_get_status(rel) & influence::level_mask) == influence::level_friendly) {
			state.world.nation_set_in_sphere_of(target, great_power);
			state.world.gp_relationship_set_influence(rel, inf - state.defines.addtosphere_influence_cost);
			auto& l = state.world.gp_relationship_get_status(rel);
			state.world.gp_relationship_set_status(rel, uint8_t(nations::influence::increase_level(l)));
		} else {
			state.world.gp_relationship_set_influence(rel, inf - state.defines.increaseopinion_influence_cost);

			auto& l = state.world.gp_relationship_get_status(rel);
			state.world.gp_relationship_set_status(rel, uint8_t(nations::influence::increase_level(l)));
		}
	}
}

void adjust_foreign_investment(sys::state& state, dcon::nation_id great_power, dcon::nation_id target, float delta) {
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, great_power);
	if(!rel) {
		rel = state.world.force_create_unilateral_relationship(target, great_power);
	}
	auto& invest = state.world.unilateral_relationship_get_foreign_investment(rel);
	state.world.unilateral_relationship_set_foreign_investment(rel, std::max(0.0f, invest + delta));
}

float get_yesterday_income(sys::state& state, dcon::nation_id n) {
	/* TODO -
	 * This is a temporary function (the contents of it), what it should return is yesterdays income
	 * code below should be replaced with more appropriate when avaliable
	 * return value is passed to text::fp_currency{}
	 */
	float sum = 0;
	auto info = economy::explain_tax_income(state, n);
	sum += info.poor;
	sum += info.mid;
	sum += info.rich;
	sum += economy::estimate_gold_income(state, n);
	return sum;
}

void make_civilized(sys::state& state, dcon::nation_id n) {
	if(state.world.nation_get_is_civilized(n))
		return;

	/*
	The nation gains technologies. Specifically take the fraction of military reforms (for land and naval) or econ reforms
	(otherwise) applied, clamped to the defines:UNCIV_TECH_SPREAD_MIN and defines:UNCIV_TECH_SPREAD_MAX values, and multiply how
	far the sphere leader (or first GP) is down each tech column, rounded up, to give unciv nations their techs when they
	westernize. The nation gets an `on_civilize` event. Political and social reforms: First setting in all categories?
	*/
	int32_t military_reforms_active_count = 0;
	int32_t econ_reforms_active_count = 0;
	int32_t total_military_reforms_count = 0;
	int32_t total_econ_reforms_count = 0;

	for(auto r : state.world.in_reform) {
		auto current_option = state.world.nation_get_reforms(n, r);
		auto& opts = state.world.reform_get_options(r);
		for(uint32_t i = 0; i < opts.size(); ++i) {
			if(opts[i]) {
				if(r.get_reform_type() == uint8_t(culture::issue_type::military)) {
					++total_military_reforms_count;
					if(opts[i] == current_option)
						military_reforms_active_count += int32_t(i);
				} else if(r.get_reform_type() == uint8_t(culture::issue_type::economic)) {
					++total_econ_reforms_count;
					if(opts[i] == current_option)
						econ_reforms_active_count += int32_t(i);
				}
			}
		}
	}

	assert(total_military_reforms_count != 0);
	assert(total_econ_reforms_count != 0);

	float mil_tech_fraction = std::clamp(float(military_reforms_active_count) / float(total_military_reforms_count),
			state.defines.unciv_tech_spread_min, state.defines.unciv_tech_spread_max);
	float econ_tech_fraction = std::clamp(float(econ_reforms_active_count) / float(total_econ_reforms_count),
			state.defines.unciv_tech_spread_min, state.defines.unciv_tech_spread_max);

	dcon::nation_id model = state.world.nation_get_in_sphere_of(n);
	if(!model)
		model = state.nations_by_rank[0];

	for(uint32_t idx = 0; idx < state.world.technology_size();) {
		// start: this tech must have a new index:
		auto this_group = state.world.technology_get_folder_index(dcon::technology_id{dcon::technology_id::value_base_t(idx)});
		bool is_military = state.culture_definitions.tech_folders[this_group].category == culture::tech_category::army ||
											 state.culture_definitions.tech_folders[this_group].category == culture::tech_category::navy;

		// find out how many techs of this index the model has
		int32_t model_tech_count = 0;
		for(uint32_t sidx = idx; sidx < state.world.technology_size(); ++sidx) {
			auto sid = dcon::technology_id{dcon::technology_id::value_base_t(sidx)};
			if(state.world.technology_get_folder_index(sid) != this_group)
				break;

			if(state.world.nation_get_active_technologies(model, sid)) {
				++model_tech_count;
			} else {
				break;
			}
		}

		// try to give the nation proportionally many
		float target_amount = float(model_tech_count) * (is_military ? mil_tech_fraction : econ_tech_fraction);
		int32_t target_count = int32_t(std::ceil(target_amount));

		for(uint32_t sidx = idx; sidx < state.world.technology_size(); ++sidx) {
			auto sid = dcon::technology_id{dcon::technology_id::value_base_t(sidx)};

			if(state.world.technology_get_folder_index(sid) != this_group)
				break;

			if(target_amount > 0) {
				if(!state.world.nation_get_active_technologies(n, sid))
					culture::apply_technology(state, n, sid);
				--target_amount;
			} else {
				break;
			}
		}

		// advance to next group or end
		for(; idx < state.world.technology_size(); ++idx) {
			if(state.world.technology_get_folder_index(dcon::technology_id{dcon::technology_id::value_base_t(idx)}) != this_group)
				break;
		}
	}

	state.world.nation_set_is_civilized(n, true);
	for(auto o : state.culture_definitions.political_issues) {
		state.world.nation_set_issues(n, o, state.world.issue_get_options(o)[0]);
	}
	for(auto o : state.culture_definitions.social_issues) {
		state.world.nation_set_issues(n, o, state.world.issue_get_options(o)[0]);
	}
	for(auto r : state.world.in_reform) {
		state.world.nation_set_reforms(n, r, dcon::reform_option_id{});
	}
	sys::update_single_nation_modifiers(state, n);
	culture::update_nation_issue_rules(state, n);

	event::fire_fixed_event(state, state.national_definitions.on_civilize, trigger::to_generic(n), event::slot_type::nation, n, -1,
			event::slot_type::none);
}
void make_uncivilized(sys::state& state, dcon::nation_id n) {
	if(!state.world.nation_get_is_civilized(n))
		return;

	state.world.nation_set_is_civilized(n, false);

	for(auto o : state.culture_definitions.military_issues) {
		state.world.nation_set_reforms(n, o, state.world.reform_get_options(o)[0]);
	}
	for(auto o : state.culture_definitions.economic_issues) {
		state.world.nation_set_reforms(n, o, state.world.reform_get_options(o)[0]);
	}
	for(auto o : state.culture_definitions.political_issues) {
		state.world.nation_set_issues(n, o, dcon::issue_option_id{});
	}
	for(auto o : state.culture_definitions.social_issues) {
		state.world.nation_set_issues(n, o, dcon::issue_option_id{});
	}
	for(auto r : state.world.in_reform) {
		state.world.nation_set_reforms(n, r, dcon::reform_option_id{});
	}
	sys::update_single_nation_modifiers(state, n);
	culture::update_nation_issue_rules(state, n);
}

void enact_reform(sys::state& state, dcon::nation_id source, dcon::reform_option_id r) {
	/*
	For military/economic reforms:
	- Run the `on_execute` member
	*/
	auto e = state.world.reform_option_get_on_execute_effect(r);
	if(e) {
		auto t = state.world.reform_option_get_on_execute_trigger(r);
		if(!t || trigger::evaluate(state, t, trigger::to_generic(source), trigger::to_generic(source), 0))
			effect::execute(state, e, trigger::to_generic(source), trigger::to_generic(source), 0, uint32_t(state.current_date.value),
					uint32_t(source.index()));
	}

	// - Subtract research points (see discussion of when the reform is possible for how many)
	bool is_military = state.world.reform_get_reform_type(state.world.reform_option_get_parent_reform(r)) ==uint8_t(culture::issue_category::military);
	if(is_military) {
		float base_cost = float(state.world.reform_option_get_technology_cost(r));
		float reform_factor = politics::get_military_reform_multiplier(state, source);

		state.world.nation_set_research_points(source, state.world.nation_get_research_points(source) - base_cost * reform_factor);

	} else {
		float base_cost = float(state.world.reform_option_get_technology_cost(r));
		float reform_factor = politics::get_economic_reform_multiplier(state, source);

		state.world.nation_set_research_points(source, state.world.nation_get_research_points(source) - base_cost * reform_factor);
	}

	/*
	In general:
	- Increase the share of conservatives in the upper house by defines:CONSERVATIVE_INCREASE_AFTER_REFORM (and then normalize
	again)
	*/

	for(auto id : state.world.in_ideology) {
		auto& upper_house = state.world.nation_get_upper_house(source, id);
		if(id == state.culture_definitions.conservative) {
			state.world.nation_set_upper_house(source, id, upper_house + state.defines.conservative_increase_after_reform * 100.0f);
		}
		state.world.nation_set_upper_house(source, id, upper_house / (1.0f + state.defines.conservative_increase_after_reform));
	}
	state.world.nation_set_reforms(source, state.world.reform_option_get_parent_reform(r), r);

	culture::update_nation_issue_rules(state, source);
	sys::update_single_nation_modifiers(state, source);
}

void enact_issue(sys::state& state, dcon::nation_id source, dcon::issue_option_id i) {

	auto e = state.world.issue_option_get_on_execute_effect(i);
	if(e) {
		auto t = state.world.issue_option_get_on_execute_trigger(i);
		if(!t || trigger::evaluate(state, t, trigger::to_generic(source), trigger::to_generic(source), 0))
			effect::execute(state, e, trigger::to_generic(source), trigger::to_generic(source), 0, uint32_t(state.current_date.value),
					uint32_t(source.index()));
	}

	/*
	For political and social based reforms:
	- Every issue-based movement with greater popular support than the movement supporting the given issue (if there is such a
	movement; all movements if there is no such movement) has its radicalism increased by 3v(support-of-that-movement /
	support-of-movement-behind-issue (or 1 if there is no such movement) - 1.0) x defines:WRONG_REFORM_RADICAL_IMPACT.
	*/
	auto winner = rebel::get_movement_by_position(state, source, i);
	float winner_support = winner ? state.world.movement_get_pop_support(winner) : 1.0f;
	for(auto m : state.world.nation_get_movement_within(source)) {
		if(m.get_movement().get_associated_issue_option() && m.get_movement().get_associated_issue_option() != i &&
				m.get_movement().get_pop_support() > winner_support) {

			auto& cur_radicalism =  m.get_movement().get_transient_radicalism();
			m.get_movement().set_transient_radicalism(cur_radicalism + std::min(3.0f, m.get_movement().get_pop_support() / winner_support - 1.0f) * state.defines.wrong_reform_radical_impact);
		}
	}
	if(winner) {
		state.world.delete_movement(winner);
	}

	/*
	- For every ideology, the pop gains defines:MIL_REFORM_IMPACT x pop-support-for-that-ideology x ideology's support for doing
	the opposite of the reform (calculated in the same way as determining when the upper house will support the reform or repeal)
	militancy
	*/
	auto issue = state.world.issue_option_get_parent_issue(i);
	auto current = state.world.nation_get_issues(source, issue.id).id;
	bool is_social = state.world.issue_get_issue_type(issue) == uint8_t(culture::issue_category::social);

	if(state.world.issue_get_is_next_step_only(issue)) {
		for(auto id : state.world.in_ideology) {
			auto condition = is_social
				? (i.index() > current.index() ? state.world.ideology_get_remove_social_reform(id) : state.world.ideology_get_add_social_reform(id))
				: (i.index() > current.index() ? state.world.ideology_get_remove_political_reform(id) : state.world.ideology_get_add_political_reform(id));
			if(condition) {
				auto factor =
					trigger::evaluate_additive_modifier(state, condition, trigger::to_generic(source), trigger::to_generic(source), 0);
				auto const idsupport_key = pop_demographics::to_key(state, id);
				if(factor > 0) {
					for(auto pr : state.world.nation_get_province_ownership(source)) {
						for(auto pop : pr.get_province().get_pop_location()) {
							auto base_mil = pop_demographics::get_militancy(state, pop.get_pop());
							pop_demographics::set_militancy(state, pop.get_pop(), std::clamp(base_mil + pop_demographics::get_demo(state, pop.get_pop(), idsupport_key) * factor * state.defines.mil_reform_impact, 0.0f, 10.0f));
						}
					}
				}
			}
		}
	}

	/*
	- Each pop in the nation gains defines:CON_REFORM_IMPACT x pop support of the issue consciousness

	- If the pop is part of a movement for some other issue (or for independence), it gains defines:WRONG_REFORM_MILITANCY_IMPACT
	militancy. All other pops lose defines:WRONG_REFORM_MILITANCY_IMPACT militancy.
	*/

	auto const isupport_key = pop_demographics::to_key(state, i);
	for(auto pr : state.world.nation_get_province_ownership(source)) {
		for(auto pop : pr.get_province().get_pop_location()) {
			auto base_con = pop_demographics::get_consciousness(state, pop.get_pop());
			auto adj_con = base_con + pop_demographics::get_demo(state, pop.get_pop(), isupport_key) * state.defines.con_reform_impact;
			pop_demographics::set_consciousness(state, pop.get_pop(), std::clamp(adj_con, 0.0f, 10.0f));

			if(auto m = pop.get_pop().get_movement_from_pop_movement_membership(); m && m.get_pop_support() > winner_support) {
				auto base_mil = pop_demographics::get_militancy(state, pop.get_pop());
				pop_demographics::set_militancy(state, pop.get_pop(), std::clamp(base_mil + state.defines.wrong_reform_militancy_impact, 0.0f, 10.0f));
			} else {
				auto base_mil = pop_demographics::get_militancy(state, pop.get_pop());
				pop_demographics::set_militancy(state, pop.get_pop(), std::clamp(base_mil - state.defines.wrong_reform_militancy_impact, 0.0f, 10.0f));
			}
		}
	}

	/*
	In general:
	- Increase the share of conservatives in the upper house by defines:CONSERVATIVE_INCREASE_AFTER_REFORM (and then normalize
	again)
	- If slavery is forbidden (rule slavery_allowed is false), remove all slave states and free all slaves.
	*/
	for(auto id : state.world.in_ideology) {
		auto& upper_house = state.world.nation_get_upper_house(source, id);
		if(id == state.culture_definitions.conservative) {
			state.world.nation_set_upper_house(source, id, upper_house + state.defines.conservative_increase_after_reform * 100.0f);
		}
		state.world.nation_set_upper_house(source, id, upper_house / (1.0f + state.defines.conservative_increase_after_reform));
	}

	state.world.nation_set_issues(source, issue, i);

	culture::update_nation_issue_rules(state, source);
	sys::update_single_nation_modifiers(state, source);

	state.world.nation_set_last_issue_or_reform_change(source, state.current_date);
}

} // namespace nations
