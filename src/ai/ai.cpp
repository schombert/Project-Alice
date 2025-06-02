#include "ai.hpp"
#include "ai_types.hpp"
#include "ai_campaign_values.hpp"
#include "system_state.hpp"
#include "demographics.hpp"
#include "economy_stats.hpp"
#include "economy_production.hpp"
#include "economy_government.hpp"
#include "construction.hpp"
#include "effects.hpp"
#include "gui_effect_tooltips.hpp"
#include "math_fns.hpp"
#include "military.hpp"
#include "politics.hpp"
#include "prng.hpp"
#include "province_templates.hpp"
#include "triggers.hpp"


namespace ai {

void take_ai_decisions(sys::state& state) {
	using decision_nation_pair = std::pair<dcon::decision_id, dcon::nation_id>;
	concurrency::combinable<std::vector<decision_nation_pair, dcon::cache_aligned_allocator<decision_nation_pair>>> decisions_taken;

	// execute in staggered blocks
	uint32_t d_block_size = state.world.decision_size() / 32;
	uint32_t block_index = 0;
	auto d_block_end = state.world.decision_size();
	//uint32_t block_index = (state.current_date.value & 31);
	//auto d_block_end = block_index == 31 ? state.world.decision_size() : d_block_size * (block_index + 1);
	concurrency::parallel_for(d_block_size * block_index, d_block_end, [&](uint32_t i) {
		auto d = dcon::decision_id{ dcon::decision_id::value_base_t(i) };
		auto e = state.world.decision_get_effect(d);
		if(e) {
			auto potential = state.world.decision_get_potential(d);
			auto allow = state.world.decision_get_allow(d);
			auto ai_will_do = state.world.decision_get_ai_will_do(d);
			ve::execute_serial_fast<dcon::nation_id>(state.world.nation_size(), [&](auto ids) {
				// AI-only, not dead nations
				ve::mask_vector filter_a = !state.world.nation_get_is_player_controlled(ids)
					&& state.world.nation_get_owned_province_count(ids) != 0;
				if(ve::compress_mask(filter_a).v != 0) {
					// empty allow assumed to be an "always = yes"
					ve::mask_vector filter_b = potential
						? filter_a && (trigger::evaluate(state, potential, trigger::to_generic(ids), trigger::to_generic(ids), 0))
						: filter_a;
					if(ve::compress_mask(filter_b).v != 0) {
						ve::mask_vector filter_c = allow
							? filter_b && (trigger::evaluate(state, allow, trigger::to_generic(ids), trigger::to_generic(ids), 0))
							: filter_b;
						if(ve::compress_mask(filter_c).v != 0) {
							ve::mask_vector filter_d = ai_will_do
								? filter_c && (trigger::evaluate_multiplicative_modifier(state, ai_will_do, trigger::to_generic(ids), trigger::to_generic(ids), 0) > 0.0f)
								: filter_c;
							ve::apply([&](dcon::nation_id n, bool passed_filter) {
								if(passed_filter) {
									decisions_taken.local().push_back(decision_nation_pair(d, n));
								}
							}, ids, filter_d);
						}
					}
				}
			});
		}
	});
	// combination and final execution
	auto total_vector = decisions_taken.combine([](auto& a, auto& b) {
		std::vector<decision_nation_pair, dcon::cache_aligned_allocator<decision_nation_pair>> result(a.begin(), a.end());
		result.insert(result.end(), b.begin(), b.end());
		return result;
	});
	// ensure total deterministic ordering
	std::sort(total_vector.begin(), total_vector.end(), [&](auto a, auto b) {
		auto na = a.second;
		auto nb = b.second;
		if(na != nb)
			return na.index() < nb.index();
		return a.first.index() < b.first.index();
	});
	// assumption 1: no duplicate pair of <n, d>
	for(const auto& v : total_vector) {
		auto n = v.second;
		auto d = v.first;
		auto e = state.world.decision_get_effect(d);
		if(trigger::evaluate(state, state.world.decision_get_potential(d), trigger::to_generic(n), trigger::to_generic(n), 0)
		&& trigger::evaluate(state, state.world.decision_get_allow(d), trigger::to_generic(n), trigger::to_generic(n), 0)) {
			effect::execute(state, e, trigger::to_generic(n), trigger::to_generic(n), 0, uint32_t(state.current_date.value), uint32_t(n.index() << 4 ^ d.index()));
			notification::post(state, notification::message{
				[e, n, d, when = state.current_date](sys::state& state, text::layout_base& contents) {
					text::add_line(state, contents, "msg_decision_1", text::variable_type::x, n, text::variable_type::y, state.world.decision_get_name(d));
					text::add_line(state, contents, "msg_decision_2");
					ui::effect_description(state, contents, e, trigger::to_generic(n), trigger::to_generic(n), 0, uint32_t(when.value), uint32_t(n.index() << 4 ^ d.index()));
				},
				"msg_decision_title",
				n, dcon::nation_id{}, dcon::nation_id{},
				sys::message_base_type::decision
			});
		}
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
				if(pid != state.world.nation_get_ruling_party(n) && politics::political_party_is_active(state, n, pid) && (gov.get_ideologies_allowed() & ::culture::to_bits(state.world.political_party_get_ideology(pid))) != 0) {
					auto support = estimate_pop_party_support(state, n, pid);
					if(support > max_support) {
						target = pid;
						max_support = support;
					}
				}
			}

			assert(target != state.world.nation_get_ruling_party(n)); // Fires if some nation has no available parties
			if(target) {
				politics::appoint_ruling_party(state, n, target);
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
			// Perhaps wrong logic
			&& col.get_state() != state.world.state_instance_get_definition(state.crisis_state_instance)
			&& (!state.crisis_war || n.get_is_at_war() == false)
			 ) {

			if(state.crisis_attacker_wargoals.size() > 0) {
				auto first_wg = state.crisis_attacker_wargoals.at(0);
				if(first_wg.state == col.get_state()) {
					continue;
				}
			}

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
								support += pop_demographics::get_demo(state, plid.get_pop(), pop_demographics::to_key(state, io)) * weigth;
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

void remove_ai_data(sys::state& state, dcon::nation_id n) {
	for(auto ar : state.world.nation_get_army_control(n)) {
		ar.get_army().set_ai_activity(0);
		ar.get_army().set_ai_province(dcon::province_id{});
	}
	for(auto v : state.world.nation_get_navy_control(n)) {
		v.get_navy().set_ai_activity(0);
	}
}

bool unit_on_ai_control(sys::state& state, dcon::army_id a) {
	auto fat_id = dcon::fatten(state.world, a);
	return fat_id.get_controller_from_army_control().get_is_player_controlled()
		? fat_id.get_is_ai_controlled()
		: true;
}
/*bool unit_on_ai_control(sys::state& state, dcon::navy_id a) {
	auto fat_id = dcon::fatten(state.world, a);
	return fat_id.get_controller_from_navy_control().get_is_player_controlled()
		? fat_id.get_is_ai_controlled()
		: true;
}*/

bool will_upgrade_ships(sys::state& state, dcon::nation_id n) {
	auto fid = dcon::fatten(state.world, n);

	auto total = 0;
	auto unfull = 0;

	for(auto v : state.world.nation_get_navy_control(n)) {
		if(!v.get_navy().get_battle_from_navy_battle_participation()) {
			for(auto shp : v.get_navy().get_navy_membership()) {
				total++;
				if(shp.get_ship().get_strength() < 1.f)
					unfull++;

			}
		}
	}

	return unfull <= total * 0.1f;
}

void update_ships(sys::state& state) {
	static std::vector<dcon::ship_id> to_delete;
	to_delete.clear();

	for(auto n : state.world.in_nation) {
		if(n.get_is_player_controlled())
			continue;
		// Landlocked nation shouldn't keep fleet
		if(n.get_is_at_war() == false && nations::is_landlocked(state, n)) {
			for(auto v : n.get_navy_control()) {
				if(!v.get_navy().get_battle_from_navy_battle_participation()) {
					for(auto shp : v.get_navy().get_navy_membership()) {
						to_delete.push_back(shp.get_ship().id);
						state.world.delete_ship(shp.get_ship());
					}
				}
			}
		} else if(n.get_is_at_war() == false) {
			dcon::unit_type_id best_transport = military::get_best_transport(state, n);
			dcon::unit_type_id best_light = military::get_best_light_ship(state, n);
			dcon::unit_type_id best_big = military::get_best_big_ship(state, n);
			
			for(auto v : n.get_navy_control()) {
				if(!v.get_navy().get_battle_from_navy_battle_participation()) {
					auto trange = v.get_navy().get_army_transport();
					bool transporting = trange.begin() != trange.end();

					for(auto shp : v.get_navy().get_navy_membership()) {
						auto type = shp.get_ship().get_type();

						// Upgrade ships, don't delete them
						if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::transport && !transporting) {
							if(best_transport && type != best_transport && will_upgrade_ships(state, n)) {
								shp.get_ship().set_type(best_transport);
								shp.get_ship().set_strength(0.01f);
							}
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::light_ship) {
							if(best_light && type != best_light && will_upgrade_ships(state, n)) {
								shp.get_ship().set_type(best_light);
								shp.get_ship().set_strength(0.01f);
							}
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::big_ship) {
							if(best_big && type != best_big && will_upgrade_ships(state, n)) {
								shp.get_ship().set_type(best_big);
								shp.get_ship().set_strength(0.01f);
							}
						}
					}
				}
			}
		}
	}

	for(auto s : to_delete) {
		state.world.delete_ship(s);
	}
}

void build_ships(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(!n.get_is_player_controlled() && n.get_province_naval_construction().begin() == n.get_province_naval_construction().end()) {
			auto disarm = n.get_disarmed_until();
			if(disarm && state.current_date < disarm)
				continue;

			dcon::unit_type_id best_transport;
			dcon::unit_type_id best_light;
			dcon::unit_type_id best_big;

			for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
				dcon::unit_type_id j{ dcon::unit_type_id::value_base_t(i) };
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
			if(best_transport) {
				if(fleet_cap_in_transports * 3 < n.get_naval_supply_points()) {
					auto overseas_allowed = state.military_definitions.unit_base_definitions[best_transport].can_build_overseas;
					auto level_req = state.military_definitions.unit_base_definitions[best_transport].min_port_level;
					auto supply_pts = state.military_definitions.unit_base_definitions[best_transport].supply_consumption_score;

					for(uint32_t j = 0; j < owned_ports.size() && (fleet_cap_in_transports + constructing_fleet_cap) * 3 < n.get_naval_supply_points(); ++j) {
						if((overseas_allowed || !province::is_overseas(state, owned_ports[j]))
							&& state.world.province_get_building_level(owned_ports[j], uint8_t(economy::province_building_type::naval_base)) >= level_req) {
							assert(command::can_start_naval_unit_construction(state, n, owned_ports[j], best_transport));
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
							&& state.world.province_get_building_level(owned_ports[j], uint8_t(economy::province_building_type::naval_base)) >= level_req) {
							assert(command::can_start_naval_unit_construction(state, n, owned_ports[j], best_transport));
							auto c = fatten(state.world, state.world.try_create_province_naval_construction(owned_ports[j], n));
							c.set_type(best_transport);
							++num_transports;
							constructing_fleet_cap += supply_pts;
						}
					}
				}
			}

			int32_t used_points = n.get_used_naval_supply_points();
			auto rem_free = n.get_naval_supply_points() - (fleet_cap_in_transports + fleet_cap_in_small + fleet_cap_in_big + constructing_fleet_cap);
			fleet_cap_in_small = std::max(fleet_cap_in_small, 1);
			fleet_cap_in_big = std::max(fleet_cap_in_big, 1);

			auto free_big_points = best_light ? rem_free * fleet_cap_in_small / (fleet_cap_in_small + fleet_cap_in_big) : rem_free;
			auto free_small_points = best_big ? rem_free * fleet_cap_in_big / (fleet_cap_in_small + fleet_cap_in_big) : rem_free;

			if(best_light) {
				auto overseas_allowed = state.military_definitions.unit_base_definitions[best_light].can_build_overseas;
				auto level_req = state.military_definitions.unit_base_definitions[best_light].min_port_level;
				auto supply_pts = state.military_definitions.unit_base_definitions[best_light].supply_consumption_score;

				for(uint32_t j = 0; j < owned_ports.size() && supply_pts <= free_small_points; ++j) {
					if((overseas_allowed || !province::is_overseas(state, owned_ports[j]))
						&& state.world.province_get_building_level(owned_ports[j], uint8_t(economy::province_building_type::naval_base)) >= level_req) {
						assert(command::can_start_naval_unit_construction(state, n, owned_ports[j], best_light));
						auto c = fatten(state.world, state.world.try_create_province_naval_construction(owned_ports[j], n));
						c.set_type(best_light);
						free_small_points -= supply_pts;
					}
				}
			}
			if(best_big) {
				auto overseas_allowed = state.military_definitions.unit_base_definitions[best_big].can_build_overseas;
				auto level_req = state.military_definitions.unit_base_definitions[best_big].min_port_level;
				auto supply_pts = state.military_definitions.unit_base_definitions[best_big].supply_consumption_score;

				for(uint32_t j = 0; j < owned_ports.size() && supply_pts <= free_big_points; ++j) {
					if((overseas_allowed || !province::is_overseas(state, owned_ports[j]))
						&& state.world.province_get_building_level(owned_ports[j], uint8_t(economy::province_building_type::naval_base)) >= level_req) {
						assert(command::can_start_naval_unit_construction(state, n, owned_ports[j], best_big));
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
			if(p.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base)) > max_level) {
				max_level = p.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base));
				result = p.get_province();
				current_distance = province::sorting_distance(state, cap, p.get_province());
			} else if(result && p.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base)) == max_level && province::sorting_distance(state, cap, p.get_province()) < current_distance) {
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
	/*
	auto in_nation = fatten(state.world, state.world.navy_get_controller_from_navy_control(n));
	auto base_spending_level = in_nation.get_effective_naval_spending();
	float oversize_amount =
		in_nation.get_naval_supply_points() > 0
		? std::min(float(in_nation.get_used_naval_supply_points()) / float(in_nation.get_naval_supply_points()), 1.75f)
		: 1.75f;
	float over_size_penalty = oversize_amount > 1.0f ? 2.0f - oversize_amount : 1.0f;
	auto spending_level = base_spending_level * over_size_penalty;
	auto max_org = 0.25f + 0.75f * spending_level;
	*/

	for(auto shp : state.world.navy_get_navy_membership(n)) {
		if(shp.get_ship().get_strength() < 0.5f)
			return true;
		//if(shp.get_ship().get_org() < 0.75f * max_org)
		//	return true;
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
	if(v.get_location_from_navy_location() == home_port) {
		v.set_ai_activity(uint8_t(at_base));
	} else if(!home_port) {
		v.set_ai_activity(uint8_t(fleet_activity::unspecified));
	} else if(auto naval_path = province::make_naval_path(state, v.get_location_from_navy_location(), home_port); naval_path.size() > 0) {
		auto new_size = uint32_t(naval_path.size());
		auto existing_path = v.get_path();
		existing_path.resize(new_size);

		for(uint32_t i = 0; i < new_size; ++i) {
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
	auto location = state.world.navy_get_location_from_navy_location(n);


	for(auto ar : transported_armies) {
		auto path = province::make_land_path(state, location, ar.get_army().get_ai_province(), ar.get_army().get_controller_from_army_control(), ar.get_army());
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
		dcon::navy_id largest;
		int32_t largest_size = 0;
		for(auto on : state.world.province_get_navy_location(p)) {
			if(on.get_navy() != n && on.get_navy().get_controller_from_navy_control() == owner) {
				auto other_mem = on.get_navy().get_navy_membership();
				if(auto sz = int32_t(other_mem.end() - other_mem.begin()); sz > largest_size) {
					largest =  on.get_navy().id;
					largest_size = sz;
				}
			}
		}
		return largest;
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

		if(!owner || owner.get_is_player_controlled() || owner.get_owned_province_count() == 0)
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
							existing_path[k] = naval_path[k];
						}
						if(new_size > 0) {
							n.set_arrival_time(military::arrival_time_to(state, n, naval_path.back()));
							n.set_ai_activity(uint8_t(fleet_activity::transporting));
						} else {
							n.set_arrival_time(sys::date{});
							send_fleet_home(state, n);
						}

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
							existing_path[k] = naval_path[k];
						}
						if(new_size > 0) {
							n.set_arrival_time(military::arrival_time_to(state, n, naval_path.back()));
							n.set_ai_activity(uint8_t(fleet_activity::transporting));
						} else {
							n.set_arrival_time(sys::date{});
							send_fleet_home(state, n);
						}
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
				if(!merge_fleet(state, n, location, owner))
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
					bool valid_attacker = true;
					auto self_ships = state.world.navy_get_navy_membership(n);
					int32_t self_sz = int32_t(self_ships.end() - self_ships.begin());
					for(auto o : owner.get_navy_control()) {
						if(o.get_navy() != n) {
							if(o.get_navy().get_ai_activity() == uint8_t(fleet_activity::attacking)) {
								valid_attacker = false;
								break;
							}
							auto orange = o.get_navy().get_navy_membership();
							if(int32_t(orange.end() - orange.begin()) >= self_sz) {
								valid_attacker = false;
								break;
							}
						}
					}
					if(valid_attacker && naval_advantage(state, owner)) {
						set_fleet_target(state, owner, state.world.navy_get_location_from_navy_location(n), n);
					}
				}
			}
			break;
		case fleet_activity::unloading:
		{
			bool failed_transport = true;

			auto transporting = state.world.navy_get_army_transport(n);
			for(auto ar : transporting) {
				if(ar.get_army().get_path().size() != 0) {
					failed_transport = false;
				}
			}

			if(transporting.begin() == transporting.end()) {
				// all unloaded -> set to unspecified to send home later in this routine
				state.world.navy_set_ai_activity(n, uint8_t(fleet_activity::unspecified));
			} else if(failed_transport) {
				// an army is stuck on the boats
				state.world.navy_set_ai_activity(n, uint8_t(fleet_activity::failed_transport));
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
	threat_border = 4,
	hostile_border = 5,
	count = 6
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
			auto ovr = n_controller.get_overlord_as_subject().get_ruler();

			if(n_controller == n) {
				// own province
			} else if(!n_controller && !other.get_rebel_faction_from_province_rebel_control()) {
				// uncolonized or sea
			} else if(other.get_rebel_faction_from_province_rebel_control()) {
				cls = province_class::hostile_border;
				break;
			} else if(military::are_at_war(state, n, n_controller)) {
				cls = province_class::hostile_border;
				break;
			} else if(nations::are_allied(state, n, n_controller) || (ovr && ovr == n) || (ovr && nations::are_allied(state, n, ovr))) {
				// allied controller or subject of allied controller or our "parent" overlord
				if(uint8_t(cls) < uint8_t(province_class::low_priority_border)) {
					cls = province_class::low_priority_border;
				}
			} else {
				/* We will target POTENTIAL enemies of the nation;
				   we could also check if the CB can be used on us, but
				   that is expensive, so instead we use available_cbs! */
				bool is_threat = false;
				if(n_controller) {
					is_threat |= n_controller.get_ai_rival() == n;
					is_threat |= state.world.nation_get_ai_rival(n) == n_controller.id;
					if(ovr) {
						/* subjects cannot negotiate by themselves, but the overlord may */
						is_threat |= ovr.get_ai_rival() == n;
						is_threat |= state.world.nation_get_ai_rival(n) == ovr.id;
						//
						is_threat |= ovr.get_constructing_cb_target() == n;
						for(auto cb : ovr.get_available_cbs())
							is_threat |= cb.target == n;
					} else {
						is_threat |= n_controller.get_constructing_cb_target() == n;
						for(auto cb : n_controller.get_available_cbs())
							is_threat |= cb.target == n;
					}
				}
				if(is_threat) {
					if(uint8_t(cls) < uint8_t(province_class::threat_border)) {
						cls = province_class::threat_border;
					}
				} else { // other border
					if(uint8_t(cls) < uint8_t(province_class::border)) {
						cls = province_class::border;
					}
				}
			}
		}
		provinces.push_back(classified_province{ c.get_province().id, cls });
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

	for(uint8_t stage = uint8_t(province_class::count); stage-- > 0 && !guards_list.empty(); ) {
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

				if(10.0f * (1 + full_loops_through) <= military::peacetime_attrition_limit(state, n, p)) {
					uint32_t nearest_index = 0;
					dcon::army_id nearest;
					float nearest_distance = 1.0f;
					for(uint32_t k = uint32_t(guards_list.size()); k-- > 0;) {
						auto guard_loc = state.world.army_get_location_from_army_location(guards_list[k]);

						if(military::relative_attrition_amount(state, guards_list[k], p) >= 2.f)
							continue; //too heavy

						/*
						// this wont work because a unit could end up in, for example, a subject's region at the end of a war
						// this region could be landlocked, resulting in this thinking that the unit can only be stationed in that
						// same region, even though it could walk out to another region

						auto g_region = state.world.province_get_connected_region_id(guard_loc);
						assert(g_region > 0);

						if(p_region != g_region && !(state.world.army_get_black_flag(guards_list[k]))  && (!p_region_is_coastal || !state.province_definitions.connected_region_is_coastal[g_region - 1]))
							continue;
						*/

						if(auto d = province::sorting_distance(state, guard_loc, p); !nearest || d < nearest_distance) {

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

dcon::navy_id find_transport_fleet(sys::state& state, dcon::nation_id controller) {
	int32_t n_size = 0;
	dcon::navy_id transport_fleet;

	for(auto v : state.world.nation_get_navy_control(controller)) {
		if(v.get_navy().get_battle_from_navy_battle_participation())
			continue;
		auto members = v.get_navy().get_navy_membership();

		auto tsize = int32_t(members.end() - members.begin());
		if(tsize <= n_size || tsize <= 1)
			continue;

		n_size = tsize;
		transport_fleet = dcon::navy_id{};

		fleet_activity activity = fleet_activity(v.get_navy().get_ai_activity());
		if(activity == fleet_activity::attacking || activity == fleet_activity::idle || activity == fleet_activity::returning_to_base) {
			auto in_transport = v.get_navy().get_army_transport();
			if(in_transport.begin() == in_transport.end()) {
				transport_fleet = v.get_navy();
			}
		}
	}

	return transport_fleet;
}

void move_idle_guards(sys::state& state) {
	std::vector<dcon::army_id> require_transport;
	require_transport.reserve(state.world.army_size());

	for(auto ar : state.world.in_army) {
		if(ar.get_ai_activity() == uint8_t(army_activity::on_guard)
			&& ar.get_ai_province()
			&& ar.get_ai_province() != ar.get_location_from_army_location()
			&& ar.get_controller_from_army_control()
			&& unit_on_ai_control(state, ar)
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
				//Units delegated to the AI won't transport themselves on their own
				if(!ar.get_controller_from_army_control().get_is_player_controlled())
					require_transport.push_back(ar.id);
			}
		}
	}

	for(uint32_t i = 0; i < require_transport.size(); ++i) {
		auto coastal_target_prov = state.world.army_get_location_from_army_location(require_transport[i]);
		auto controller = state.world.army_get_controller_from_army_control(require_transport[i]);

		dcon::navy_id transport_fleet = find_transport_fleet(state, controller);

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
			if(transport_location == army_location) {
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

// MP compliant
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

float estimate_balanced_composition_factor(sys::state& state, dcon::army_id a) {
	if(state.cheat_data.disable_ai) {
		return 0.0f;
	}
	auto regs = state.world.army_get_army_membership(a);
	if(regs.begin() == regs.end())
		return 0.0f;
	// account composition
	// Ideal composition: 4/1/4 (1 cavalry for each 4 infantry and 1 infantry for each arty)
	float total_str = 0.f;
	float str_art = 0.f;
	float str_inf = 0.f;
	float str_cav = 0.f;
	for(const auto reg : regs) {
		float str = reg.get_regiment().get_strength() * reg.get_regiment().get_org();
		if(auto utid = reg.get_regiment().get_type(); utid) {
			switch(state.military_definitions.unit_base_definitions[utid].type) {
			case military::unit_type::infantry:
				str_inf += str;
				break;
			case military::unit_type::cavalry:
				str_cav += str;
				break;
			case military::unit_type::support:
			case military::unit_type::special:
				str_art += str;
				break;
			default:
				break;
			}
		}
		total_str += str;
	}
	if(total_str == 0.f)
		return 0.f;
	// provide continous function for each military unit composition
	// such that 4x times the infantry (we min with arty for equality reasons) and 1/4th of cavalry
	float min_cav = std::min(str_cav, str_inf * (1.f / 4.f)); // more cavalry isn't bad (if the rest of the composition is 4x/y/4x), just don't underestimate it!
	float scale = 1.f - math::sin(std::abs(std::min(str_art / total_str, str_inf / total_str) - (4.f * min_cav / total_str)));
	return total_str * scale;
}

float estimate_army_defensive_strength(sys::state& state, dcon::army_id a) {
	if(state.cheat_data.disable_ai) {
		return 0.0f;
	}
	float scale = state.world.army_get_controller_from_army_control(a) ? 1.f : 0.5f;
	// account general
	if(auto gen = state.world.army_get_general_from_army_leadership(a); gen) {
		auto n = state.world.army_get_controller_from_army_control(a);
		if(!n)
			n = state.world.national_identity_get_nation_from_identity_holder(state.national_definitions.rebel_id);
		auto back = military::get_leader_background_wrapper(state, gen);
		auto pers = military::get_leader_personality_wrapper(state, gen);
		float morale = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::org_regain)
			+ state.world.leader_trait_get_morale(back)
			+ state.world.leader_trait_get_morale(pers) + 1.0f;
		float org = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_organisation)
			+ state.world.leader_trait_get_organisation(back)
			+ state.world.leader_trait_get_organisation(pers) + 1.0f;
		float def = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_defense_modifier)
			+ state.world.leader_trait_get_defense(back)
			+ state.world.leader_trait_get_defense(pers) + 1.0f;
		scale += def * morale * org;
		scale += state.world.nation_get_has_gas_defense(n) ? 10.f : 0.f;
	}
	// terrain defensive bonus
	float terrain_bonus = state.world.province_get_modifier_values(state.world.army_get_location_from_army_location(a), sys::provincial_mod_offsets::defense);
	scale += terrain_bonus;
	float defender_fort = 1.0f + 0.1f * state.world.province_get_building_level(state.world.army_get_location_from_army_location(a), uint8_t(economy::province_building_type::fort));
	scale += defender_fort;
	// composition bonus
	float strength = estimate_balanced_composition_factor(state, a);
	return std::max(0.1f, strength * scale);
}

float estimate_army_offensive_strength(sys::state& state, dcon::army_id a) {
	if(state.cheat_data.disable_ai) {
		return 0.0f;
	}
	float scale = state.world.army_get_controller_from_army_control(a) ? 1.f : 0.5f;
	// account general
	if(auto gen = state.world.army_get_general_from_army_leadership(a); gen) {
		auto n = state.world.army_get_controller_from_army_control(a);
		if(!n)
			n = state.world.national_identity_get_nation_from_identity_holder(state.national_definitions.rebel_id);
		auto back = military::get_leader_background_wrapper(state, gen);
		auto pers = military::get_leader_personality_wrapper(state, gen);
		float morale = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::org_regain)
			+ state.world.leader_trait_get_morale(back)
			+ state.world.leader_trait_get_morale(pers) + 1.0f;
		float org = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_organisation)
			+ state.world.leader_trait_get_organisation(back)
			+ state.world.leader_trait_get_organisation(pers) + 1.0f;
		float atk = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_attack_modifier)
			+ state.world.leader_trait_get_attack(back)
			+ state.world.leader_trait_get_attack(pers) + 1.0f;
		scale += atk * morale * org;
		scale += state.world.nation_get_has_gas_attack(n) ? 10.f : 0.f;
	}
	// composition bonus
	float strength = estimate_balanced_composition_factor(state, a);
	return std::max(0.1f, strength * scale);
}

float estimate_enemy_defensive_force(sys::state& state, dcon::province_id target, dcon::nation_id by) {
	if(state.cheat_data.disable_ai) {
		return 0.0f;
	}
	float strength_total = 0.f;
	if(state.world.nation_get_is_at_war(by)) {
		for(auto ar : state.world.in_army) {
			if(ar.get_is_retreating()
			|| ar.get_battle_from_army_battle_participation()
			|| ar.get_controller_from_army_control() == by)
				continue;
			auto loc = ar.get_location_from_army_location();
			auto sdist = province::sorting_distance(state, loc, target);
			if(sdist < state.defines.alice_ai_threat_radius) {
				auto other_nation = ar.get_controller_from_army_control();
				if(!other_nation || military::are_at_war(state, other_nation, by)) {
					strength_total += estimate_army_defensive_strength(state, ar);
				}
			}
		}
	} else { // not at war -- rebel fighting
		for(auto ar : state.world.province_get_army_location(target)) {
			auto other_nation = ar.get_army().get_controller_from_army_control();
			if(!other_nation) {
				strength_total += estimate_army_defensive_strength(state, ar.get_army());
			}
		}
	}
	return state.defines.alice_ai_offensive_strength_overestimate * strength_total;
}

void assign_targets(sys::state& state, dcon::nation_id n) {
	struct a_str {
		dcon::province_id p;
		float str = 0.0f;
	};
	std::vector<a_str> ready_armies;
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
		if(std::find_if(ready_armies.begin(), ready_armies.end(), [loc](a_str const& v) { return loc == v.p; }) == ready_armies.end()) {
			ready_armies.push_back(a_str{ loc, 0.0f });
		}
	}

	if(ready_armies.empty())
		return; // nothing to attack with

	struct army_target {
		float minimal_distance;
		dcon::province_id location;
		float strength_estimate = 0.0f;
	};

	/* Ourselves */
	std::vector<army_target> potential_targets;
	potential_targets.reserve(state.world.province_size());
	for(auto o : state.world.nation_get_province_ownership(n)) {
		if(!o.get_province().get_nation_from_province_control()
			|| military::rebel_army_in_province(state, o.get_province())
			) {
			potential_targets.push_back(
				army_target{ province::sorting_distance(state, o.get_province(), ready_armies[0].p), o.get_province().id, 0.0f }
			);
		}
	}
	/* Nations we're at war with OR hostile to */
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
	for(auto w : at_war_with) {
		for(auto o : state.world.nation_get_province_control(w)) {
			potential_targets.push_back(
				army_target{ province::sorting_distance(state, o.get_province(), ready_armies[0].p), o.get_province().id, 0.0f }
			);
		}
		for(auto o : state.world.nation_get_province_ownership(w)) {
			if(!o.get_province().get_nation_from_province_control()) {
				potential_targets.push_back(
					army_target{ province::sorting_distance(state, o.get_province(), ready_armies[0].p), o.get_province().id,0.0f }
				);
			}
		}
	}
	/* Our allies (mainly our substates, vassals) - we need to care of them! */
	for(const auto ovr : state.world.nation_get_overlord_as_ruler(n)) {
		auto w = ovr.get_subject();
		for(auto o : state.world.nation_get_province_ownership(w)) {
			if(!o.get_province().get_nation_from_province_control()
				|| military::rebel_army_in_province(state, o.get_province())
				) {
				potential_targets.push_back(
					army_target{ province::sorting_distance(state, o.get_province(), ready_armies[0].p), o.get_province().id, 0.0f }
				);
			}
		}
	}

	for(auto& pt : potential_targets) {
		for(uint32_t i = uint32_t(ready_armies.size()); i-- > 1;) {
			auto sdist = province::sorting_distance(state, ready_armies[i].p, pt.location);
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
	bool is_at_war = state.world.nation_get_is_at_war(n);
	int32_t min_ready_count = std::min(ready_count, 3); //Atleast 3 attacks
	int32_t max_attacks_to_make = is_at_war ? std::max(min_ready_count, (ready_count + 1) / 3) : ready_count; // not at war -- allow all stacks to attack rebels
	auto const psize = potential_targets.size();

	for(uint32_t i = 0; i < psize && max_attacks_to_make > 0; ++i) {
		if(!potential_targets[i].location)
			continue; // target has been removed as too close by some earlier iteration
		if(potential_targets[i].strength_estimate == 0.0f)
			potential_targets[i].strength_estimate = estimate_enemy_defensive_force(state, potential_targets[i].location, n) + 0.00001f;

		auto target_attack_force = potential_targets[i].strength_estimate;
		std::sort(ready_armies.begin(), ready_armies.end(), [&](a_str const& a, a_str const& b) {
			auto adist = province::sorting_distance(state, a.p, potential_targets[i].location);
			auto bdist = province::sorting_distance(state, b.p, potential_targets[i].location);
			if(adist != bdist)
				return adist > bdist;
			else
				return a.p.index() < b.p.index();
		});

		// make list of attackers
		float a_force_str = 0.f;
		int32_t k = int32_t(ready_armies.size());
		for(; k-- > 0 && a_force_str <= target_attack_force;) {
			if(ready_armies[k].str == 0.0f) {
				for(auto ar : state.world.province_get_army_location(ready_armies[k].p)) {
					if(ar.get_army().get_battle_from_army_battle_participation()
						|| n != ar.get_army().get_controller_from_army_control()
						|| ar.get_army().get_navy_from_army_transport()
						|| ar.get_army().get_black_flag()
						|| ar.get_army().get_arrival_time()
						|| army_activity(ar.get_army().get_ai_activity()) != army_activity::on_guard
						|| !army_ready_for_battle(state, n, ar.get_army())) {

						continue;
					}

					ready_armies[k].str += estimate_army_offensive_strength(state, ar.get_army());
				}
				ready_armies[k].str += 0.00001f;
			}
			a_force_str += ready_armies[k].str;
		}

		if(a_force_str < target_attack_force) {
			return; // end assigning attackers completely
		}

		// find central province
		dcon::province_id central_province;

		glm::vec3 accumulated{ 0.0f, 0.0f, 0.0f };
		float minimal_distance = 2.0f;

		for(int32_t m = int32_t(ready_armies.size()); m-- > k + 1; ) {
			accumulated += state.world.province_get_mid_point_b(ready_armies[m].p);
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
			assert(m >= 0 && m < int32_t(ready_armies.size()));
			for(auto ar : state.world.province_get_army_location(ready_armies[m].p)) {
				if(ar.get_army().get_battle_from_army_battle_participation()
					|| n != ar.get_army().get_controller_from_army_control()
					|| ar.get_army().get_navy_from_army_transport()
					|| ar.get_army().get_black_flag()
					|| ar.get_army().get_arrival_time()
					|| army_activity(ar.get_army().get_ai_activity()) != army_activity::on_guard
					|| !army_ready_for_battle(state, n, ar.get_army())) {

					continue;
				}

				if(ready_armies[m].p == central_province) {
					ar.get_army().set_ai_province(potential_targets[i].location);
					ar.get_army().set_ai_activity(uint8_t(army_activity::attacking));
				} else if(auto path = province::make_safe_land_path(state, ready_armies[m].p, central_province, n); !path.empty()) {
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
		if(is_at_war) {
			for(uint32_t j = i + 1; j < psize; ++j) {
				if(province::sorting_distance(state, potential_targets[j].location, potential_targets[i].location) < state.defines.alice_ai_attack_target_radius)
					potential_targets[j].location = dcon::province_id{};
			}
		}
	}
}

void make_attacks(sys::state& state) {
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_id n{ dcon::nation_id::value_base_t(i) };
		if(state.world.nation_is_valid(n)) {
			assign_targets(state, n);
		}
	});
}

void make_defense(sys::state& state) {
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_id n{ dcon::nation_id::value_base_t(i) };
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
		} else if(ar.get_ai_activity() == uint8_t(army_activity::attack_gathered)) {
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

		dcon::navy_id transport_fleet = find_transport_fleet(state, controller);

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

bool will_upgrade_regiments(sys::state& state, dcon::nation_id n) {
	auto fid = dcon::fatten(state.world, n);

	auto total = fid.get_active_regiments();
	auto unfull = 0;

	for(auto ar : state.world.nation_get_army_control(n)) {
		for(auto r : ar.get_army().get_army_membership()) {
			if(r.get_regiment().get_strength() < 0.8f) {
				unfull++;

				if(unfull > total * 0.1f) {
					return false;
				}
			}
		}
	}

	return true;
}

void update_land_constructions(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(n.get_is_player_controlled() || n.get_owned_province_count() == 0)
			continue;
		auto disarm = n.get_disarmed_until();
		if(disarm && state.current_date < disarm)
			continue;

		static std::vector<dcon::province_land_construction_id> hopeless_construction;
		hopeless_construction.clear();

		state.world.nation_for_each_province_land_construction(n, [&](dcon::province_land_construction_id plcid) {
			auto fat_plc = dcon::fatten(state.world, plcid);
			auto prov = fat_plc.get_pop().get_province_from_pop_location();
			if(prov.get_nation_from_province_control() != n)
				hopeless_construction.push_back(plcid);
		});

		for(auto item : hopeless_construction) {
			state.world.delete_province_land_construction(item);
		}			

		auto constructions = state.world.nation_get_province_land_construction(n);
		if(constructions.begin() != constructions.end())
			continue;

		int32_t num_frontline = 0;
		int32_t num_support = 0;

		// Nation-wide best unit types
		auto inf_type = military::get_best_infantry(state, n);
		auto art_type = military::get_best_artillery(state, n);
		military::unit_definition art_def;
		if (art_type)
			art_def = state.military_definitions.unit_base_definitions[art_type];
		bool art_req_pc = art_def.primary_culture;
		auto cav_type = military::get_best_cavalry(state, n);

		for(auto ar : state.world.nation_get_army_control(n)) {
			for(auto r : ar.get_army().get_army_membership()) {
				auto type = r.get_regiment().get_type();
				auto etype = state.military_definitions.unit_base_definitions[type].type;
				if(etype == military::unit_type::support || etype == military::unit_type::special) {
					++num_support;
				} else {
					++num_frontline;
				}

				/* AI units upgrade
				* AI upgrades units only if less than 10% of the army is currently under 80% strength (requiring supplies for reinforcement)
				*/
				if(will_upgrade_regiments(state, n)) {
					auto primary_culture = r.get_regiment().get_pop_from_regiment_source().get_culture() == n.get_primary_culture();

					// AI can upgrade into primary-culture-specific units such as guards
					if(primary_culture) {
						auto pc_adj_inf_type = military::get_best_infantry(state, n, primary_culture);
						auto pc_adj_art_type = military::get_best_artillery(state, n, primary_culture);
						auto pc_adj_cav_type = military::get_best_cavalry(state, n, primary_culture);

						if(etype == military::unit_type::infantry && pc_adj_inf_type && military::is_infantry_better(state, n, type, pc_adj_inf_type)) {
							r.get_regiment().set_type(pc_adj_inf_type);
							r.get_regiment().set_strength(0.01f);
						} else if(etype == military::unit_type::support && pc_adj_art_type && military::is_artillery_better(state, n, type, pc_adj_art_type)) {
							r.get_regiment().set_type(pc_adj_art_type);
							r.get_regiment().set_strength(0.01f);
						} else if(etype == military::unit_type::cavalry && pc_adj_cav_type && military::is_cavalry_better(state, n, type, pc_adj_cav_type)) {
							r.get_regiment().set_type(pc_adj_cav_type);
							r.get_regiment().set_strength(0.01f);
						}
					}
					// Keep non-primary-culture units as nation-wide best units
					else {
						if(etype == military::unit_type::infantry && inf_type && military::is_infantry_better(state, n, type, inf_type)) {
							r.get_regiment().set_type(inf_type);
							r.get_regiment().set_strength(0.01f);
						} else if(etype == military::unit_type::support && art_type && military::is_artillery_better(state, n, type, art_type)) {
							r.get_regiment().set_type(art_type);
							r.get_regiment().set_strength(0.01f);
						} else if(etype == military::unit_type::cavalry && cav_type && military::is_cavalry_better(state, n, type, cav_type)) {
							r.get_regiment().set_type(cav_type);
							r.get_regiment().set_strength(0.01f);
						}
					}
				}
			}
		}

		const auto decide_type = [&](bool pc) {
			if(art_type && (!art_req_pc || (art_req_pc && pc))) {
				if(num_frontline > num_support) {
					++num_support;
					return art_type;
				} else {
					++num_frontline;
					return inf_type;
				}
			} else {
				return inf_type;
			}
		};

		auto num_to_build_nation = calculate_desired_army_size(state, n) - num_frontline - num_support;

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
							auto num_to_make_local = amount - ((regs.end() - regs.begin()) + (building.end() - building.begin()));
							while(num_to_make_local > 0 && num_to_build_nation > 0) {
								auto t = decide_type(pop.get_pop().get_is_primary_or_accepted_culture());
								assert(command::can_start_land_unit_construction(state, n, pop.get_province(), pop.get_pop().get_culture(), t));
								auto c = fatten(state.world, state.world.try_create_province_land_construction(pop.get_pop().id, n));
								c.set_type(t);
								--num_to_make_local;
								--num_to_build_nation;
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
							auto num_to_make_local = amount - ((regs.end() - regs.begin()) + (building.end() - building.begin()));
							while(num_to_make_local > 0 && num_to_build_nation > 0) {
								auto t = decide_type(pop.get_pop().get_is_primary_or_accepted_culture());
								assert(command::can_start_land_unit_construction(state, n, pop.get_province(), pop.get_pop().get_culture(), t));
								auto c = fatten(state.world, state.world.try_create_province_land_construction(pop.get_pop().id, n));
								c.set_type(t);
								--num_to_make_local;
								--num_to_build_nation;
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
							auto num_to_make_local = amount - ((regs.end() - regs.begin()) + (building.end() - building.begin()));
							while(num_to_make_local > 0 && num_to_build_nation > 0) {
								auto t = decide_type(pop.get_pop().get_is_primary_or_accepted_culture());
								assert(command::can_start_land_unit_construction(state, n, pop.get_province(), pop.get_pop().get_culture(), t));
								auto c = fatten(state.world, state.world.try_create_province_land_construction(pop.get_pop().id, n));
								c.set_type(t);
								--num_to_make_local;
								--num_to_build_nation;
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
			&& unit_on_ai_control(state, ar)
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
					auto type = (*regs.begin()).get_regiment().get_type();
					auto etype = state.military_definitions.unit_base_definitions[type].type;
					auto is_art = etype == military::unit_type::support;
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
								auto stype = r.get_regiment().get_type();
								auto setype = state.military_definitions.unit_base_definitions[stype].type;
								if(setype == military::unit_type::support || setype == military::unit_type::special) {
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
				auto type = (*regs.begin()).get_regiment().get_type();
				auto etype = state.military_definitions.unit_base_definitions[type].type;
				auto is_art = etype == military::unit_type::support;
				for(auto o : location.get_army_location()) {
					if(o.get_army().get_ai_activity() == uint8_t(army_activity::on_guard)
						&& o.get_army().get_controller_from_army_control() == controller) {

						int32_t num_support = 0;
						int32_t num_frontline = 0;
						for(auto r : o.get_army().get_army_membership()) {
							auto stype = r.get_regiment().get_type();
							auto setype = state.military_definitions.unit_base_definitions[stype].type;
							if(setype == military::unit_type::support || setype == military::unit_type::special) {
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

float estimate_rebel_strength(sys::state& state, dcon::province_id p) {
	float v = 0.f;
	for(auto ar : state.world.province_get_army_location(p))
		if(ar.get_army().get_controller_from_army_rebel_control())
			v += estimate_army_defensive_strength(state, ar.get_army());
	return v;
}

}
