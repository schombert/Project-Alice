#include "nations.hpp"
#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "modifiers.hpp"
#include "politics.hpp"
#include "system_state.hpp"
#include "ve_scalar_extensions.hpp"
#include "triggers.hpp"
#include "politics.hpp"

namespace nations {

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

// returns whether a culture is on the accepted list OR is the primary culture
template<typename T, typename U>
auto nation_accepts_culture(sys::state const& state, T ids, U cul_ids) {
	auto is_accepted = ve::apply([&state](dcon::nation_id n, dcon::culture_id c) {
		if(n)
			return state.world.nation_get_accepted_cultures(n).contains(c);
		else
			return false;
	}, ids, cul_ids);
	return (state.world.nation_get_primary_culture(ids) == cul_ids) || is_accepted;
}

template<typename T>
auto primary_culture_group(sys::state const& state, T ids) {
	auto cultures = state.world.nation_get_primary_culture(ids);
	return state.world.culture_get_group_from_culture_group_membership(cultures);
}

template<typename T>
auto owner_of_pop(sys::state const& state, T pop_ids) {
	auto location = state.world.pop_get_province_from_pop_location(pop_ids);
	return state.world.province_get_nation_from_province_ownership(location);
}

template<typename T>
auto central_reb_controlled_fraction(sys::state const& state, T ids) {
	auto cpc = ve::to_float(state.world.nation_get_central_province_count(ids));
	auto reb_count = ve::to_float(state.world.nation_get_central_rebel_controlled(ids));
	return ve::select(cpc != 0.0f, reb_count / cpc, decltype(cpc)());
}

template<typename T>
auto central_blockaded_fraction(sys::state const& state, T ids) {
	auto cpc = ve::to_float(state.world.nation_get_central_ports(ids));
	auto b_count = ve::to_float(state.world.nation_get_central_blockaded(ids));
	return ve::select(cpc != 0.0f, b_count / cpc, decltype(cpc)());
}

template<typename T>
auto central_has_crime_fraction(sys::state const& state, T ids) {
	auto cpc = ve::to_float(state.world.nation_get_central_province_count(ids));
	auto crim_count = ve::to_float(state.world.nation_get_central_crime_count(ids));
	return ve::select(cpc != 0.0f, crim_count / cpc, decltype(cpc)());
}

template<typename T>
auto occupied_provinces_fraction(sys::state const& state, T ids) {
	auto cpc = ve::to_float(state.world.nation_get_owned_province_count(ids));
	auto occ_count = ve::to_float(state.world.nation_get_occupied_count(ids));
	return ve::select(cpc != 0.0f, occ_count / cpc, decltype(cpc)());
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

	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_allies_count(ids, ve::int_vector());
	});
	state.world.for_each_diplomatic_relation([&](dcon::diplomatic_relation_id id) {
		if(state.world.diplomatic_relation_get_are_allied(id)) {
			state.world.nation_get_allies_count(state.world.diplomatic_relation_get_related_nations(id, 0)) += uint16_t(1);
			state.world.nation_get_allies_count(state.world.diplomatic_relation_get_related_nations(id, 1)) += uint16_t(1);
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
	if(!state.diplomatic_cached_values_out_of_date)
		return;

	state.diplomatic_cached_values_out_of_date = false;

	restore_cached_values(state);
}

void restore_unsaved_values(sys::state& state) {
	state.world.nation_resize_demand_satisfaction(state.world.commodity_size());

	state.world.for_each_gp_relationship([&](dcon::gp_relationship_id rel) {
		if((influence::level_mask & state.world.gp_relationship_get_status(rel)) == influence::level_in_sphere) {
			auto t = state.world.gp_relationship_get_influence_target(rel);
			auto gp = state.world.gp_relationship_get_great_power(rel);
			state.world.nation_set_in_sphere_of(t, gp);
		}
	});

	restore_cached_values(state);


	// NOTE: relies on naval supply being set
	update_colonial_points(state);
}

void generate_initial_state_instances(sys::state& state) {
	for(int32_t i = 0; i < state.province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		auto owner = state.world.province_get_nation_from_province_ownership(pid);
		if(owner && !(state.world.province_get_state_membership(pid))) {
			auto state_instance = fatten(state.world, state.world.create_state_instance());
			auto abstract_state = state.world.province_get_state_from_abstract_state_membership(pid);

			state_instance.set_definition(abstract_state);
			state_instance.set_capital(pid);
			state.world.force_create_state_ownership(state_instance, owner);

			for(auto mprov : state.world.state_definition_get_abstract_state_membership(abstract_state)) {
				if(mprov.get_province().get_nation_from_province_ownership() == owner) {
					mprov.get_province().set_state_membership(state_instance);
				}
			}
		}
	}
}

bool can_release_as_vassal(sys::state const& state, dcon::nation_id n, dcon::national_identity_id releasable) {
	if(!state.world.national_identity_get_is_not_releasable(releasable) && !identity_has_holder(state, releasable)) {
		bool owns_a_core = false;
		bool not_on_capital = true;
		state.world.national_identity_for_each_core(releasable, [&](dcon::core_id core) {
			auto province = state.world.core_get_province(core);
			owns_a_core |= state.world.province_get_nation_from_province_ownership(province) == n;
			not_on_capital &= state.world.nation_get_capital(n) != province;
		});
		return owns_a_core && not_on_capital;
	} else {
		return false;
	}
}

bool identity_has_holder(sys::state const& state, dcon::national_identity_id ident) {
	auto fat_ident = dcon::fatten(state.world, ident);
	return bool(fat_ident.get_nation_from_identity_holder().id);
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

dcon::text_sequence_id name_from_tag(sys::state const& state, dcon::national_identity_id tag) {
	auto holder = state.world.national_identity_get_nation_from_identity_holder(tag);
	if(holder)
		return state.world.nation_get_name(holder);
	else
		return state.world.national_identity_get_name(tag);
}

void update_administrative_efficiency(sys::state& state) {
	/*
	- national administrative efficiency: = (the-nation's-national-administrative-efficiency-modifier + efficiency-modifier-from-technologies + 1) x number-of-non-colonial-bureaucrat-population / (total-non-colonial-population x (sum-of-the-administrative_multiplier-for-social-issues-marked-as-being-administrative x define:BUREAUCRACY_PERCENTAGE_INCREMENT + define:MAX_BUREAUCRACY_PERCENTAGE) )
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto admin_mod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::administrative_efficiency_modifier);
		ve::fp_vector issue_sum;
		for(auto i : state.culture_definitions.social_issues) {
			issue_sum = issue_sum + state.world.issue_option_get_administrative_multiplier(state.world.nation_get_issues(ids, i));
		}
		auto from_issues = issue_sum * state.defines.bureaucracy_percentage_increment + state.defines.max_bureaucracy_percentage;

		auto non_colonial = state.world.nation_get_non_colonial_population(ids);
		auto total = ve::select(non_colonial > 0.0f, (admin_mod + 1.0f) * state.world.nation_get_non_colonial_bureaucrats(ids) / (non_colonial * from_issues), 0.0f);

		state.world.nation_set_administrative_efficiency(ids, ve::min(total, 1.0f));
	});
}

float daily_research_points(sys::state& state, dcon::nation_id n) {
	/*
	Let pop-sum = for each pop type (research-points-from-type x 1^(fraction of population / optimal fraction))
	Then, the daily research points earned by a nation is: (national-modifier-research-points-modifier + tech-research-modifier + 1) x (national-modifier-to-research-points) + pop-sum)
	*/
	auto rp_mod_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::research_points_modifier);
	auto rp_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::research_points);

	float sum_from_pops = 0;
	state.world.for_each_pop_type([&](dcon::pop_type_id t) {
		auto rp = state.world.pop_type_get_research_points(t);
		if(rp > 0) {
			sum_from_pops += rp * std::min(1.0f, state.world.nation_get_demographics(n, demographics::to_key(state, t)) / (state.world.nation_get_demographics(n, demographics::total) * state.world.pop_type_get_research_optimum(t)));
		}
	});

	return (sum_from_pops + rp_mod) * (rp_mod_mod + 1.0f);
}

void update_research_points(sys::state& state) {
	/*
	Let pop-sum = for each pop type (research-points-from-type x 1^(fraction of population / optimal fraction))
	Then, the daily research points earned by a nation is: (national-modifier-research-points-modifier + tech-research-modifier + 1) x (national-modifier-to-research-points) + pop-sum)
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto rp_mod_mod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::research_points_modifier);
		auto rp_mod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::research_points);

		ve::fp_vector sum_from_pops;
		state.world.for_each_pop_type([&](dcon::pop_type_id t) {
			auto rp = state.world.pop_type_get_research_points(t);
			if(rp > 0) {
				sum_from_pops = ve::multiply_and_add(
					rp,
					ve::min(1.0f, state.world.nation_get_demographics(ids, demographics::to_key(state, t)) / (state.world.nation_get_demographics(ids, demographics::total) * state.world.pop_type_get_research_optimum(t))),
					sum_from_pops);
			}
		});
		auto amount = ve::select(state.world.nation_get_owned_province_count(ids) != 0, (sum_from_pops + rp_mod) * (rp_mod_mod + 1.0f), 0.0f);
		/*
		If a nation is not currently researching a tech (or is an unciv), research points will be banked, up to a total of 365 x daily research points, for civs, or define:MAX_RESEARCH_POINTS for uncivs.
		*/
		auto current_points = state.world.nation_get_research_points(ids);
		auto capped_value = ve::min(amount + current_points, ve::select(state.world.nation_get_is_civilized(ids), amount * 365.0f, state.defines.max_research_points));
		state.world.nation_set_research_points(ids, capped_value);
	});
}

void update_industrial_scores(sys::state& state) {
	/*
	Is the sum of the following two components:
	- For each state: (fraction of factory workers in each state (types marked with can work factory = yes) to the total-workforce x building level of factories in the state (capped at 1)) x total-factory-levels
	- For each country that the nation is invested in: define:INVESTMENT_SCORE_FACTOR x the amount invested x 0.01
	*/

	state.world.for_each_nation([&, iweight = state.defines.investment_score_factor](dcon::nation_id n) {
		float sum = 0;
		if(state.world.nation_get_owned_province_count(n) != 0) {
			for(auto si : state.world.nation_get_state_ownership(n)) {
				float total_level = 0;
				float worker_total =
					si.get_state().get_demographics(demographics::to_key(state, state.culture_definitions.primary_factory_worker))
					+ si.get_state().get_demographics(demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
				float total_factory_capacity = 0;
				province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
					for(auto f : state.world.province_get_factory_location(p)) {
						total_factory_capacity += float(f.get_factory().get_level() * f.get_factory().get_building_type().get_base_workforce());
						total_level += float(f.get_factory().get_level());
					}
				});
				if(total_factory_capacity > 0)
					sum += 4.0f * total_level * std::max(std::min(1.0f, worker_total / total_factory_capacity), 0.05f);
			}
			for(auto ur : state.world.nation_get_unilateral_relationship_as_source(n)) {
				sum += ur.get_foreign_investment() * iweight * 0.05f;
			}
		}
		state.world.nation_set_industrial_score(n, uint16_t(sum));
	});
}

void update_military_scores(sys::state& state) {
	/*
	The first part  is complicated enough that I am going to simplify things slightly, and ignore how mobilization can interact with this:
	First, we need to know the total number of recruitable regiments
	We also need to know the average land unit score, which we define here as (attack + defense + national land attack modifier + national land defense modifier) x discipline
	Then we take the lesser of the number of regiments in the field x 4 or the number of recruitable regiments and multiply it by define:DISARMAMENT_ARMY_HIT (if disarmed) multiply that by the average land unit score, multiply again by (national-modifier-to-supply-consumption + 1), and then divide by 7.

	To that we add for each capital ship: (hull points + national-naval-defense-modifier) x (gun power + national-naval-attack-modifier) / 250

	And then we add one point either per leader or per regiment, whichever is greater.
	*/
	state.world.execute_serial_over_nation([&, disarm = state.defines.disarmament_army_hit](auto n) {
		float sum = 0;
		auto recruitable = ve::to_float(state.world.nation_get_recruitable_regiments(n));
		auto active_regs = ve::to_float(state.world.nation_get_active_regiments(n));
		auto is_disarmed = ve::apply([&](dcon::nation_id i) { return state.world.nation_get_disarmed_until(i) < state.current_date; }, n);
		auto disarm_factor = ve::select(is_disarmed, ve::fp_vector(disarm), ve::fp_vector(1.0f));
		auto supply_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::supply_consumption) + 1.0f;
		auto avg_land_score = state.world.nation_get_averge_land_unit_score(n);
		auto num_leaders = ve::apply([&](dcon::nation_id i) {
			auto gen_range = state.world.nation_get_leader_loyalty(i);
			return float((gen_range.end() - gen_range.begin()));
		}, n);
		state.world.nation_set_military_score(n, ve::to_int(
			(ve::min(recruitable, active_regs * 4.0f) * avg_land_score) * ((disarm_factor * supply_mod) / 7.0f)
			+ state.world.nation_get_capital_ship_score(n)
			+ ve::max(num_leaders, active_regs)
		));
	});
}

float prestige_score(sys::state const& state, dcon::nation_id n) {
	return state.world.nation_get_prestige(n) + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::permanent_prestige);
}

void update_rankings(sys::state& state) {
	uint32_t to_sort_count = 0;
	state.world.for_each_nation([&](dcon::nation_id n) {
		state.nations_by_rank[to_sort_count] = n;
		++to_sort_count;
	});
	std::sort(state.nations_by_rank.begin(), state.nations_by_rank.begin() + to_sort_count, [&](dcon::nation_id a, dcon::nation_id b) {
		auto fa = fatten(state.world, a);
		auto fb = fatten(state.world, b);
		if((fa.get_owned_province_count() != 0) != (fb.get_owned_province_count() != 0)) {
			return (fa.get_owned_province_count() != 0);
		}
		if(fa.get_is_civilized() != fb.get_is_civilized())
			return fa.get_is_civilized();
		if(bool(fa.get_overlord_as_subject()) != bool(fa.get_overlord_as_subject()))
			return !bool(fa.get_overlord_as_subject());
		auto a_score = fa.get_military_score() + fa.get_industrial_score() + prestige_score(state, a);
		auto b_score = fb.get_military_score() + fb.get_industrial_score() + prestige_score(state, b);
		if(a_score != b_score)
			return a_score > b_score;
		return a.index() > b.index();
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
	std::sort(state.nations_by_industrial_score.begin(), state.nations_by_industrial_score.begin() + to_sort_count, [&](dcon::nation_id a, dcon::nation_id b) {
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
	std::sort(state.nations_by_military_score.begin(), state.nations_by_military_score.begin() + to_sort_count, [&](dcon::nation_id a, dcon::nation_id b) {
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
	std::sort(state.nations_by_prestige_score.begin(), state.nations_by_prestige_score.begin() + to_sort_count, [&](dcon::nation_id a, dcon::nation_id b) {
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
	return state.world.nation_get_rank(id) <= uint16_t(state.defines.great_nations_count);
}

status get_status(sys::state& state, dcon::nation_id n) {
	if(is_great_power(state, n)) {
		return status::great_power;
	} else if(state.world.nation_get_rank(n) <= uint16_t(state.defines.great_nations_count + 8)) {
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
	- number of national focuses: the lesser of total-accepted-and-primary-culture-population / define:NATIONAL_FOCUS_DIVIDER and 1 + the number of national focuses provided by technology.
	*/
	float relevant_pop = state.world.nation_get_demographics(n, demographics::to_key(state, state.world.nation_get_primary_culture(n)));
	for(auto ac : state.world.nation_get_accepted_cultures(n)) {
		relevant_pop += state.world.nation_get_demographics(n, demographics::to_key(state, ac));
	}

	return std::max(1, std::min(int32_t(relevant_pop / state.defines.national_focus_divider), int32_t(1 + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_national_focus))));
}

int32_t national_focuses_in_use(sys::state& state, dcon::nation_id n) {
	// TODO
	return 0;
}

float diplomatic_points(sys::state const& state, dcon::nation_id n) {
	return state.world.nation_get_diplomatic_points(n);
}

int32_t free_colonial_points(sys::state const& state, dcon::nation_id n) {
	// TODO
	return 0;
}

int32_t max_colonial_points(sys::state const& state, dcon::nation_id n) {
	return int32_t(state.world.nation_get_colonial_points(n));
}

void update_colonial_points(sys::state& state) {
	state.world.for_each_nation([&](dcon::nation_id n) {
		/*
		Only nations with rank at least define:COLONIAL_RANK get colonial points.
		*/
		if(state.world.nation_get_rank(n) <= state.defines.colonial_rank) {
			float points = 0.0f;
			/*
			Colonial points come from three sources:
			- naval bases: (1) determined by level and the building definition, except you get only define:COLONIAL_POINTS_FOR_NON_CORE_BASE (a flat rate) for naval bases not in a core province and not connected by land to the capital.
			*/
			for(auto p : state.world.nation_get_province_ownership(n)) {
				auto nb_rank = state.world.province_get_naval_base_level(p.get_province());
				if(nb_rank > 0) {
					if(p.get_province().get_is_owner_core() || p.get_province().get_connected_region_id() == state.world.province_get_connected_region_id(state.world.nation_get_capital(n))) {

						points += float(state.economy_definitions.naval_base_definition.colonial_points[nb_rank]);
					} else {
						points += state.defines.colonial_points_for_non_core_base;
					}
				}
			}
			/*
			- units: the colonial points they grant x (1.0 - the fraction the nation's naval supply consumption is over that provided by its naval bases) x define:COLONIAL_POINTS_FROM_SUPPLY_FACTOR
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

			/*
			- points from technologies/inventions
			*/
			state.world.for_each_technology([&](dcon::technology_id t) {
				if(state.world.nation_get_active_technologies(n, t)) {
					points += float(state.world.technology_get_colonial_points(t));
				}
			});
			state.world.nation_set_colonial_points(n, points);
		} else {
			state.world.nation_set_colonial_points(n, 0.0f);
		}
	});
}

bool can_expand_colony(sys::state& state, dcon::nation_id n) {
	for(auto cols : state.world.nation_get_colonization_as_colonizer(n)) {
		auto state_colonization = state.world.state_definition_get_colonization(cols.get_state());
		auto num_colonizers = state_colonization.end() - state_colonization.begin();
		if(num_colonizers == 1) {
			/*
			If you have put in a colonist in a region and it goes at least define:COLONIZATION_DAYS_FOR_INITIAL_INVESTMENT without any other colonizers, it then moves into phase 3 with define:COLONIZATION_INTEREST_LEAD points.
			*/
			if(state.current_date >= cols.get_last_investment() + int32_t(state.defines.colonization_days_for_initial_investment)) {
				if(free_colonial_points(state, n) >= int32_t(state.defines.colonization_create_protectorate_cost)) {
					return true;
				}
			}
		} else {
			/*
			If you have put a colonist in the region, and colonization is in phase 1 or 2, you can invest if it has been at least define:COLONIZATION_DAYS_BETWEEN_INVESTMENT since your last investment, you have enough colonial points, and the state remains in range.
			*/
			if(state.current_date >= cols.get_last_investment() + int32_t(state.defines.colonization_days_between_investment)) {
				/*
				Investing in a colony costs define:COLONIZATION_INVEST_COST_INITIAL + define:COLONIZATION_INTEREST_COST_NEIGHBOR_MODIFIER (if a province adjacent to the region is owned) to place the initial colonist. Further steps cost define:COLONIZATION_INTEREST_COST while in phase 1. In phase two, each point of investment cost define:COLONIZATION_INFLUENCE_COST up to the fourth point. After reaching the fourth point, further points cost define:COLONIZATION_EXTRA_GUARD_COST x (points - 4) + define:COLONIZATION_INFLUENCE_COST.
				*/
				auto points = cols.get_level() < 4
					? int32_t(state.defines.colonization_interest_cost)
					: int32_t(state.defines.colonization_extra_guard_cost * (cols.get_level() - 4) + state.defines.colonization_influence_cost);
				if(free_colonial_points(state, n) >= points) {
					return true;
				}
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
		auto act_date = it.get_penalty_expires_date();
		if(!act_date || act_date <= state.current_date || (it.get_status() & influence::is_banned) == 0) {
			if(it.get_influence() >= state.defines.increaseopinion_influence_cost && (influence::level_mask & it.get_status()) != influence::level_in_sphere) {
				return true;
			} else if(!(it.get_influence_target().get_in_sphere_of()) && it.get_influence() >= state.defines.addtosphere_influence_cost) {
				return true;
			} else if(it.get_influence_target().get_in_sphere_of() && (influence::level_mask & it.get_status()) == influence::level_friendly && it.get_influence() >= state.defines.removefromsphere_influence_cost) {
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
	//At least define:MIN_DELAY_BETWEEN_REFORMS months must have passed since the last issue option change (for any type of issue).
	auto last_date = state.world.nation_get_last_issue_or_reform_change(n);
	if(bool(last_date) && (last_date + int32_t(state.defines.min_delay_between_reforms * 30.0f)) > state.current_date)
		return false;

	if(state.world.nation_get_is_civilized(n)) {
		/*
		### When a social/political reform is possible
		These are only available for civ nations. If it is "next step only" either the previous or next issue option must be in effect And it's `allow` trigger must be satisfied. Then. for each ideology, we test its `add_social_reform` or `remove_social_reform` (depending if we are increasing or decreasing, and substituting `political_reform` here as necessary), computing its modifier additively, and then adding the result x the fraction of the upperhouse that the ideology has to a running total. If the running total is > 0.5, the issue option can be implemented.
		*/
		return has_political_reform_available(state, n) || has_social_reform_available(state, n);
	}
	/*
	### When an economic/military reform is possible
	These are only available for unciv nations. Some of the rules are the same as for social/political reforms:  If it is "next step only" either the previous or next issue option must be in effect. And it's `allow` trigger must be satisfied. Where things are different: Each reform also has a cost in research points. This cost, however, can vary. The actual cost you must pay is multiplied by what I call the "reform factor" + 1. The reform factor is (sum of ideology-in-upper-house x add-reform-triggered-modifier) x define:ECONOMIC_REFORM_UH_FACTOR + the nation's self_unciv_economic/military_modifier + the unciv_economic/military_modifier of the nation it is in the sphere of (if any).
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
	for(uint32_t i = state.world.decision_size(); i-- > 0; ) {
		dcon::decision_id did{ dcon::decision_id::value_base_t(i) };
		if(n != state.local_player_nation || !state.world.decision_get_hide_notification(did)) {
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

std::vector<dcon::political_party_id> get_active_political_parties(sys::state& state, dcon::nation_id n) {
	std::vector<dcon::political_party_id> parties{};
	auto identity = state.world.nation_get_identity_from_identity_holder(n);
	auto start = state.world.national_identity_get_political_party_first(identity).id.index();
	auto end = start + state.world.national_identity_get_political_party_count(identity);

	for(int32_t i = start; i < end; i++) {
		auto pid = dcon::political_party_id(uint16_t(i));
		if(politics::political_party_is_active(state, pid)) {
			parties.push_back(pid);
		}
	}
	return parties;
}

void update_monthly_points(sys::state& state) {
	/*
	- Prestige: a nation with a prestige modifier gains that amount of prestige per month (on the 1st)
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto pmod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::prestige);
		state.world.nation_set_prestige(ids, state.world.nation_get_prestige(ids) + pmod);
	});
	/*
	- Infamy: a nation with a badboy modifier gains that amount of infamy per month
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto imod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::badboy);
		state.world.nation_set_infamy(ids, ve::max(state.world.nation_get_infamy(ids) + imod, 0.0f));
	});
	/*
	- War exhaustion: a nation with a war exhaustion modifier gains that much at the start of the month, and every month its war exhaustion is capped to its maximum-war-exhaustion modifier at most.
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto wmod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::war_exhaustion);
		auto wmax_mod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::max_war_exhaustion);
		state.world.nation_set_war_exhaustion(ids,
			ve::min(state.world.nation_get_war_exhaustion(ids) + wmod, wmax_mod));
	});
	/*
	- Monthly plurality increase: plurality increases by average consciousness / 45 per month.
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto pmod = state.world.nation_get_demographics(ids, demographics::consciousness) / ve::max(state.world.nation_get_demographics(ids, demographics::total), 1.0f) * 0.0222f;
		state.world.nation_set_plurality(ids, ve::min(state.world.nation_get_plurality(ids) + pmod, 100.0f));
	});
	/*
	- Monthly diplo-points: (1 + national-modifier-to-diplo-points + diplo-points-from-technology) x define:BASE_MONTHLY_DIPLOPOINTS (to a maximum of 9)
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto bmod = state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::diplomatic_points_modifier) + 1.0f;
		auto dmod = bmod * state.defines.base_monthly_diplopoints;

		state.world.nation_set_diplomatic_points(ids, ve::min(state.world.nation_get_diplomatic_points(ids) + dmod, 9.0f));
	});
	/*
	- Monthly suppression point gain: define:SUPPRESS_BUREAUCRAT_FACTOR x fraction-of-population-that-are-bureaucrats x define:SUPPRESSION_POINTS_GAIN_BASE x (suppression-points-from-technology + national-suppression-points-modifier + 1) (to a maximum of define:MAX_SUPPRESSION)
	*/
	state.world.execute_serial_over_nation([&](auto ids) {
		auto bmod = (state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::suppression_points_modifier) + 1.0f);
		auto cmod = (bmod * state.defines.suppression_points_gain_base) * (state.world.nation_get_demographics(ids, demographics::to_key(state, state.culture_definitions.bureaucrat)) / ve::max(state.world.nation_get_demographics(ids, demographics::total), 1.0f) * state.defines.suppress_bureaucrat_factor);

		state.world.nation_set_suppression_points(ids, ve::min(state.world.nation_get_suppression_points(ids) + cmod, state.defines.max_suppression));
	});
}

float get_treasury(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_stockpiles(n, economy::money);
}

float get_bank_funds(sys::state& state, dcon::nation_id n) {
	return 0.0f;
}

float get_debt(sys::state& state, dcon::nation_id n) {
	return 0.0f;
}

float tariff_efficiency(sys::state& state, dcon::nation_id n) {
	auto eff_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::tariff_efficiency_modifier);
	auto adm_eff = state.world.nation_get_administrative_efficiency(n);
	return std::min(state.defines.base_tariff_efficiency + eff_mod + adm_eff, 1.f);
}

float tax_efficiency(sys::state& state, dcon::nation_id n) {
	auto eff_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::tax_efficiency);
	return eff_mod + state.defines.base_country_tax_efficiency;
}

bool is_involved_in_crisis(sys::state const& state, dcon::nation_id n) {
	if(n == state.primary_crisis_attacker)
		return true;
	if(n == state.primary_crisis_defender)
		return true;
	for(size_t i = state.crisis_participants.size(); i-- > 0; ) {
		if(state.crisis_participants[i].id == n)
			return true;
	}

	return false;
}

void adjust_relationship(sys::state& state, dcon::nation_id a, dcon::nation_id b, float delta) {
	if(auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(a, b)) {
		state.world.diplomatic_relation_get_value(rel) += delta;
	} else {
		auto nrel = state.world.try_create_diplomatic_relation(a, b);
		state.world.diplomatic_relation_set_value(nrel, delta);
	}
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
	state.world.for_each_issue([&](dcon::issue_id t) {
		state.world.nation_set_issues(n, t, state.world.nation_get_issues(base, t));
	});
	if(!state.world.nation_get_is_civilized(base)) {
		state.world.for_each_reform([&](dcon::reform_id t) {
			state.world.nation_set_reforms(n, t, state.world.nation_get_reforms(base, t));
		});
	}
	state.world.nation_set_last_issue_or_reform_change(n, sys::date{});
	culture::update_nation_issue_rules(state, n);
	state.world.for_each_ideology([&](dcon::ideology_id i) {
		state.world.nation_set_upper_house(n, i, state.world.nation_get_upper_house(base, i));
	});
	state.world.nation_set_is_substate(n, false);
	for(int32_t i = 0; i < state.national_definitions.num_allocated_national_flags; ++i) {
		state.world.nation_set_flag_variables(n, dcon::national_flag_id{ dcon::national_flag_id::value_base_t(i) }, false);
	}
	state.world.nation_set_is_substate(n, false);
	for(int32_t i = 0; i < state.national_definitions.num_allocated_national_variables; ++i) {
		state.world.nation_set_variables(n, dcon::national_variable_id{ dcon::national_variable_id::value_base_t(i) }, 0.0f);
	}
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
		state.world.nation_set_unit_stats(n, dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) }, state.world.nation_get_unit_stats(base, dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) }));
		state.world.nation_set_active_unit(n, dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) }, state.world.nation_get_active_unit(base, dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) }));
	}
	for(uint32_t i = 0; i < state.culture_definitions.crimes.size(); ++i) {
		state.world.nation_set_active_crime(n, dcon::crime_id{ dcon::crime_id::value_base_t(i) }, state.world.nation_get_active_crime(base, dcon::crime_id{ dcon::crime_id::value_base_t(i) }));
	}
	state.world.for_each_factory_type([&](dcon::factory_type_id t) {
		state.world.nation_set_active_building(n, t, state.world.nation_get_active_building(base, t));
	});
	state.world.nation_set_has_gas_attack(n, state.world.nation_get_has_gas_attack(base));
	state.world.nation_set_has_gas_defense(n, state.world.nation_get_has_gas_defense(base));
	state.world.nation_set_max_railroad_level(n, state.world.nation_get_max_railroad_level(base));
	state.world.nation_set_max_fort_level(n, state.world.nation_get_max_fort_level(base));
	state.world.nation_set_max_naval_base_level(n, state.world.nation_get_max_naval_base_level(base));
	state.world.nation_set_election_ends(n, sys::date{ 0 });
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
	state.world.nation_set_tariffs(n, int8_t(0));

	auto base_ruling_ideology = state.world.political_party_get_ideology(state.world.nation_get_ruling_party(base));

	auto identity = state.world.nation_get_identity_from_identity_holder(n);
	auto start = state.world.national_identity_get_political_party_first(identity).id.index();
	auto end = start + state.world.national_identity_get_political_party_count(identity);

	for(int32_t i = start; i < end; i++) {
		auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(i));
		if(politics::political_party_is_active(state, pid) && state.world.political_party_get_ideology(pid) == base_ruling_ideology) {
			state.world.nation_set_ruling_party(n, pid);
			break;
		}
	}
	if(!state.world.nation_get_ruling_party(n)) {
		for(int32_t i = start; i < end; i++) {
			auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(i));
			if(politics::political_party_is_active(state, pid)) {
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
				state.world.nation_get_accepted_cultures(n).push_back(c.get_member());
			}
		}
	}

	politics::update_displayed_identity(state, n);
}

void cleanup_nation(sys::state& state, dcon::nation_id n) {
	auto old_ident = state.world.nation_get_identity_from_identity_holder(n);

	state.world.delete_nation(n);
	auto new_ident_holder = state.world.create_nation();
	state.world.try_create_identity_holder(new_ident_holder, old_ident);

	//cleanup:
	for(uint32_t i = state.world.leader_size(); i-- > 0;) {
		dcon::leader_id l{ dcon::leader_id::value_base_t(i) };
		if(!state.world.leader_get_nation_from_leader_loyalty(l)) {
			state.world.delete_leader(l);
		}
	}
	for(uint32_t i = state.world.army_size(); i-- > 0;) {
		dcon::army_id l{ dcon::army_id::value_base_t(i) };
		if(!state.world.army_get_controller_from_army_control(l)) { // TODO: handle rebel controlled armies
			state.world.delete_army(l);
		}
	}
	for(uint32_t i = state.world.navy_size(); i-- > 0;) {
		dcon::navy_id l{ dcon::navy_id::value_base_t(i) };
		if(!state.world.navy_get_controller_from_navy_control(l)) {
			state.world.delete_navy(l);
		}
	}

	state.diplomatic_cached_values_out_of_date = true; // refresh stored counts of allies, vassals, etc

	if(n == state.local_player_nation) { // TODO: player defeated; notify and end game
		state.local_player_nation = dcon::nation_id{};
	}
}

void adjust_prestige(sys::state& state, dcon::nation_id n, float delta) {
	auto prestige_multiplier = 1.0f + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::prestige);
	auto new_prestige = std::max(0.0f, state.world.nation_get_prestige(n) + (delta > 0 ? (delta * prestige_multiplier) : delta));
	state.world.nation_set_prestige(n, new_prestige);
}

void destroy_diplomatic_relationships(sys::state& state, dcon::nation_id n) {
	{
		auto gp_relationships = state.world.nation_get_gp_relationship_as_great_power(n);
		while(gp_relationships.begin() != gp_relationships.end()) {
			state.world.delete_gp_relationship(*(gp_relationships.begin()));
		}
	}
	{
		auto gp_relationships = state.world.nation_get_gp_relationship_as_influence_target(n);
		while(gp_relationships.begin() != gp_relationships.end()) {
			state.world.delete_gp_relationship(*(gp_relationships.begin()));
		}
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
			released_vassal = [&]() {
				for(auto it = ov_rel.begin(); it != ov_rel.end(); ++it) {
					if((*it).get_subject().get_is_substate() == false) {
						release_vassal(state, *it);
						return true;
					}
				}
				return false;
			}();
		}
	}
}
void release_vassal(sys::state& state, dcon::overlord_id rel) {
	auto vas = state.world.overlord_get_subject(rel);
	state.world.nation_set_is_substate(vas, false);
	politics::update_displayed_identity(state, vas);
	// TODO: notify player
	state.world.delete_overlord(rel);
}
void break_alliance(sys::state& state, dcon::diplomatic_relation_id rel) {
	if(state.world.diplomatic_relation_get_are_allied(rel)) {
		// TODO: notify player
		state.world.diplomatic_relation_set_are_allied(rel, false);
	}
}

}
