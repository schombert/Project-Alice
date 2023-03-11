#include "nations.hpp"
#include "system_state.hpp"
#include "ve_scalar_extensions.hpp"

namespace nations {

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

void restore_unsaved_values(sys::state& state) {
	state.world.for_each_gp_relationship([&](dcon::gp_relationship_id rel) {
		if((influence::level_mask & state.world.gp_relationship_get_status(rel)) == influence::level_in_sphere) {
			auto t = state.world.gp_relationship_get_influence_target(rel);
			auto gp = state.world.gp_relationship_get_great_power(rel);
			state.world.nation_set_in_sphere_of(t, gp);
		}
	});

	state.world.for_each_state_instance([&](dcon::state_instance_id sid) {
		auto owner = state.world.state_instance_get_nation_from_state_ownership(sid);
		auto base_state = state.world.state_instance_get_definition(sid);
		for(auto mprov : state.world.state_definition_get_abstract_state_membership(base_state)) {
			if(mprov.get_province().get_nation_from_province_ownership() == owner) {
				mprov.get_province().set_state_membership(sid);
			}
		}
	});

	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_allies_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_vassals_count(ids, ve::int_vector());
	});

	state.world.for_each_diplomatic_relation([&](dcon::diplomatic_relation_id id) {
		if(state.world.diplomatic_relation_get_are_allied(id)) {
			state.world.nation_get_allies_count(state.world.diplomatic_relation_get_related_nations(id,0)) += uint16_t(1);
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
	// TODO: implement function
	return false;
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
		auto admin_mod = state.world.nation_get_static_modifier_values(ids, sys::national_mod_offsets::administrative_efficiency - sys::provincial_mod_offsets::count) + state.world.nation_get_fluctuating_modifier_values(ids, sys::national_mod_offsets::administrative_efficiency - sys::provincial_mod_offsets::count);

		ve::fp_vector issue_sum;
		for(auto i : state.culture_definitions.social_issues) {
			issue_sum = issue_sum + state.world.issue_option_get_administrative_multiplier(state.world.nation_get_issues(ids, i));
		}
		auto from_issues = issue_sum * state.defines.bureaucracy_percentage_increment + state.defines.max_bureaucracy_percentage;

		auto total = (admin_mod + 1.0f) * state.world.nation_get_non_colonial_bureaucrats(ids) / (state.world.nation_get_non_colonial_population(ids) * from_issues);

		state.world.nation_set_administrative_efficiency(ids, ve::min(total, 1.0f));
	});
}

float daily_research_points(sys::state& state, dcon::nation_id n) {
	auto rp_mod_mod = state.world.nation_get_static_modifier_values(n, sys::national_mod_offsets::research_points_modifier - sys::provincial_mod_offsets::count) + state.world.nation_get_fluctuating_modifier_values(n, sys::national_mod_offsets::research_points_modifier - sys::provincial_mod_offsets::count);

	auto rp_mod = state.world.nation_get_static_modifier_values(n, sys::national_mod_offsets::research_points - sys::provincial_mod_offsets::count) + state.world.nation_get_fluctuating_modifier_values(n, sys::national_mod_offsets::research_points - sys::provincial_mod_offsets::count);

	float sum_from_pops = 0;
	state.world.for_each_pop_type([&](dcon::pop_type_id t) {
		auto rp = state.world.pop_type_get_research_points(t);
		if(rp > 0) {
			sum_from_pops += rp * std::min(1.0f, state.world.nation_get_demographics(n, demographics::to_key(state, t)) / (state.world.nation_get_demographics(n, demographics::total) * state.world.pop_type_get_research_optimum(t)) );
		}
	});

	return (sum_from_pops + rp_mod) * (rp_mod_mod + 1.0f);
}
void update_research_points(sys::state& state) {
	state.world.execute_serial_over_nation([&](auto ids) {
		auto rp_mod_mod = state.world.nation_get_static_modifier_values(ids, sys::national_mod_offsets::research_points_modifier - sys::provincial_mod_offsets::count) + state.world.nation_get_fluctuating_modifier_values(ids, sys::national_mod_offsets::research_points_modifier - sys::provincial_mod_offsets::count);

		auto rp_mod = state.world.nation_get_static_modifier_values(ids, sys::national_mod_offsets::research_points - sys::provincial_mod_offsets::count) + state.world.nation_get_fluctuating_modifier_values(ids, sys::national_mod_offsets::research_points - sys::provincial_mod_offsets::count);

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
		auto amount = (sum_from_pops + rp_mod) * (rp_mod_mod + 1.0f);
		state.world.nation_set_research_points(ids, amount + state.world.nation_get_research_points(ids));
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

		for(auto si : state.world.nation_get_state_ownership(n)) {
			float worker_total =
				si.get_state().get_demographics(demographics::to_key(state, state.culture_definitions.primary_factory_worker))
				+ si.get_state().get_demographics(demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
			float total_factory_capacity = 0;
			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				for(auto f : state.world.province_get_factory_location(p)) {
					total_factory_capacity += float(f.get_factory().get_level() * f.get_factory().get_building_type().get_base_workforce());
				}
			});
			if(total_factory_capacity > 0)
				sum += std::min(1.0f, worker_total / total_factory_capacity);
		}
		for(auto ur : state.world.nation_get_unilateral_relationship_as_source(n)) {
			sum += ur.get_foreign_investment() * iweight * 0.02f;
		}
		state.world.nation_set_industrial_score(n, uint16_t(sum * 10.0f));
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
		auto supply_mod = state.world.nation_get_static_modifier_values(n, sys::national_mod_offsets::supply_consumption - sys::provincial_mod_offsets::count)
			+ state.world.nation_get_fluctuating_modifier_values(n, sys::national_mod_offsets::supply_consumption - sys::provincial_mod_offsets::count)
			+ 1.0f;
		auto avg_land_score = state.world.nation_get_averge_land_unit_score(n);
		auto num_leaders = ve::apply([&](dcon::nation_id i) {
			auto gen_range = state.world.nation_get_general_loyalty(i);
			auto ad_range = state.world.nation_get_admiral_loyalty(i);
			return float((gen_range.end() - gen_range.begin()) + (ad_range.end() - ad_range.begin()));
		}, n);
		state.world.nation_set_military_score(n, ve::to_int(
			(ve::min(recruitable, active_regs * 4.0f) * avg_land_score) * ((disarm_factor * supply_mod) / 7.0f)
			+ state.world.nation_get_capital_ship_score(n)
			+ ve::max(num_leaders, active_regs)
		));
	});
}

float prestige_score(sys::state const& state, dcon::nation_id n) {
	return state.world.nation_get_prestige(n) + state.world.nation_get_static_modifier_values(n, sys::national_mod_offsets::permanent_prestige - sys::provincial_mod_offsets::count);
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
		if(fa.get_is_civilized() && !fb.get_is_civilized())
			return true;
		if(!fa.get_is_civilized() && fb.get_is_civilized())
			return false;
		if(bool(fa.get_overlord_as_subject()) && !bool(fa.get_overlord_as_subject()))
			return false;
		if(!bool(fa.get_overlord_as_subject()) && bool(fa.get_overlord_as_subject()))
			return true;
		auto a_score = fa.get_military_score() + fa.get_industrial_score() + prestige_score(state, a);
		auto b_score = fb.get_military_score() + fb.get_industrial_score() + prestige_score(state, b);
		if(a_score != b_score)
			return a_score > b_score;
		return a.index() > b.index();
	});
	for(uint32_t i = 0; i < to_sort_count; ++i) {
		state.world.nation_set_rank(state.nations_by_rank[i], uint16_t(i + 1));
	}
}

void update_ui_rankings(sys::state& state) {
	uint32_t to_sort_count = 0;
	state.world.for_each_nation([&](dcon::nation_id n) {
		state.nations_by_industrial_score[to_sort_count] = n;
		state.nations_by_military_score[to_sort_count] = n;
		state.nations_by_prestige_score[to_sort_count] = n;
		++to_sort_count;
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

bool is_greate_power(sys::state const& state, dcon::nation_id n) {
	return state.world.nation_get_rank(n) <= uint16_t(state.defines.great_nations_count);
}

}

