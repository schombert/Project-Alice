#include "province.hpp"
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include <vector>

namespace province {

template<typename T>
auto is_overseas(sys::state const& state, T ids) {
	auto owners = state.world.province_get_nation_from_province_ownership(ids);
	auto owner_cap = state.world.nation_get_capital(owners);
	return (state.world.province_get_continent(ids) != state.world.province_get_continent(owner_cap))
		&& (state.world.province_get_connected_region_id(ids) != state.world.province_get_connected_region_id(owner_cap));
}

template<typename F>
void for_each_land_province(sys::state& state, F const& func) {
	int32_t last = state.province_definitions.first_sea_province.index();
	for(int32_t i = 0; i < last; ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		func(pid);
	}
}

template<typename F>
void ve_for_each_land_province(sys::state& state, F const& func) {
	int32_t last = state.province_definitions.first_sea_province.index();
	ve::execute_serial<dcon::province_id>(uint32_t(last), func);
}

template<typename F>
void for_each_sea_province(sys::state& state, F const& func) {
	int32_t first = state.province_definitions.first_sea_province.index();
	for(int32_t i = first; i < int32_t(state.world.province_size()); ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		func(pid);
	}
}

template<typename F>
void for_each_province_in_state_instance(sys::state& state, dcon::state_instance_id s, F const& func) {
	auto d = state.world.state_instance_get_definition(s);
	auto o = state.world.state_instance_get_nation_from_state_ownership(s);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == o) {
			func(p.get_province().id);
		}
	}
}

bool nations_are_adjacent(sys::state& state, dcon::nation_id a, dcon::nation_id b) {
	auto it = state.world.get_nation_adjacency_by_nation_adjacency_pair(a, b);
	return bool(it);
}

void update_connected_regions(sys::state& state) {
	if(!state.adjacency_data_out_of_date)
		return;

	state.adjacency_data_out_of_date = false;

	state.world.nation_adjacency_resize(0);

	state.world.for_each_province([&](dcon::province_id id) {
		state.world.province_set_connected_region_id(id, 0);
	});
	// TODO get a better allocator
	static std::vector<dcon::province_id> to_fill_list;
	uint16_t current_fill_id = 0;

	to_fill_list.reserve(state.world.province_size());

	for(int32_t i = state.province_definitions.first_sea_province.index(); i-- > 0; ) {
		dcon::province_id id{ dcon::province_id::value_base_t(i) };
		if(state.world.province_get_connected_region_id(id) == 0) {
			++current_fill_id;

			to_fill_list.push_back(id);

			while(!to_fill_list.empty()) {
				auto current_id = to_fill_list.back();
				to_fill_list.pop_back();

				state.world.province_set_connected_region_id(current_id, current_fill_id);
				for(auto rel : state.world.province_get_province_adjacency(current_id)) {
					if((rel.get_type() & (province::border::coastal_bit | province::border::impassible_bit)) == 0) { // not entering sea, not impassible
						auto owner_a = rel.get_connected_provinces(0).get_nation_from_province_ownership();
						auto owner_b = rel.get_connected_provinces(1).get_nation_from_province_ownership();
						if(owner_a == owner_b) { // both have the same owner
							if(rel.get_connected_provinces(0).get_connected_region_id() == 0)
								to_fill_list.push_back(rel.get_connected_provinces(0));
							if(rel.get_connected_provinces(1).get_connected_region_id() == 0)
								to_fill_list.push_back(rel.get_connected_provinces(1));
						} else {
							state.world.try_create_nation_adjacency(owner_a, owner_b);
						}
					}
				}
			}

			to_fill_list.clear();
		}
	}
}


void restore_unsaved_values(sys::state& state) {
	//clear nation values that cache province information

	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_owned_province_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_central_province_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_central_blockaded(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_central_rebel_controlled(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_rebel_controlled_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_central_ports(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_central_crime_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_total_ports(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_occupied_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_owned_state_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_is_colonial_nation(ids, ve::mask_vector());
	});
	for(int32_t i = 0; i < state.province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		[&]() {
			for(auto adj : state.world.province_get_province_adjacency(pid)) {
				if((state.world.province_adjacency_get_type(adj) & province::border::coastal_bit) != 0) {
					state.world.province_set_is_coast(pid, true);
					return;
				}
			}
		}();
		auto owner = state.world.province_get_nation_from_province_ownership(pid);
		if(owner) {
			bool owner_core = false;
			for(auto c : state.world.province_get_core(pid)) {
				if(c.get_identity().get_nation_from_identity_holder() == owner) {
					owner_core = true;
					break;
				}
			}
			state.world.province_set_is_owner_core(pid, owner_core);

			state.world.nation_get_owned_province_count(owner) += uint16_t(1);

			bool reb_controlled = bool(state.world.province_get_rebel_faction_from_province_rebel_control(pid));

			if(reb_controlled) {
				state.world.nation_get_rebel_controlled_count(owner) += uint16_t(1);
			}
			if(state.world.province_get_is_coast(pid)) {
				state.world.nation_get_total_ports(owner) += uint16_t(1);
			}
			if(auto c = state.world.province_get_nation_from_province_control(pid); bool(c) && c != owner) {
				state.world.nation_get_occupied_count(owner) += uint16_t(1);
			}
			if(state.world.province_get_is_colonial(pid)) {
				state.world.nation_set_is_colonial_nation(owner, true);
			}
			if(!is_overseas(state, pid)) {
				state.world.nation_get_central_province_count(owner) += uint16_t(1);

				if(military::province_is_blockaded(state, pid)) {
					state.world.nation_get_central_blockaded(owner) += uint16_t(1);
				}
				if(state.world.province_get_is_coast(pid)) {
					state.world.nation_get_central_ports(owner) += uint16_t(1);
				}
				if(reb_controlled) {
					state.world.nation_get_central_rebel_controlled(owner) += uint16_t(1);
				}
				if(state.world.province_get_crime(pid)) {
					state.world.nation_get_central_crime_count(owner) += uint16_t(1);
				}
			}
		} else {
			state.world.province_set_is_owner_core(pid, false);
		}
	}
	state.world.for_each_state_instance([&](dcon::state_instance_id s) {
		auto owner = state.world.state_instance_get_nation_from_state_ownership(s);
		state.world.nation_get_owned_state_count(owner) += uint16_t(1);
	});
}

/*
// We can probably do without this
void update_state_administrative_efficiency(sys::state& state) {

	//- state administrative efficiency: = define:NONCORE_TAX_PENALTY x number-of-non-core-provinces + (bureaucrat-tax-efficiency x total-number-of-primary-or-accepted-culture-bureaucrats / population-of-state)v1 / x (sum-of-the-administrative_multiplier-for-social-issues-marked-as-being-administrative x define:BUREAUCRACY_PERCENTAGE_INCREMENT + define:MAX_BUREAUCRACY_PERCENTAGE)), all clamped between 0 and 1.

	state.world.for_each_state_instance([&](dcon::state_instance_id si) {
		auto owner = state.world.state_instance_get_nation_from_state_ownership(si);

		auto admin_mod = state.world.nation_get_static_modifier_values(owner, sys::national_mod_offsets::administrative_efficiency - sys::provincial_mod_offsets::count) + state.world.nation_get_fluctuating_modifier_values(owner, sys::national_mod_offsets::administrative_efficiency - sys::provincial_mod_offsets::count);

		float issue_sum = 0.0f;
		for(auto i : state.culture_definitions.social_issues) {
			issue_sum = issue_sum + state.world.issue_option_get_administrative_multiplier(state.world.nation_get_issues(owner, i));
		}
		auto from_issues = issue_sum * state.defines.bureaucracy_percentage_increment + state.defines.max_bureaucracy_percentage;
		float non_core_effect = 0.0f;
		float bsum = 0.0f;
		for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
			if(!state.world.province_get_is_owner_core(p)) {
				non_core_effect += state.defines.noncore_tax_penalty;
			}
			for(auto po : state.world.province_get_pop_location(p)) {
				if(po.get_pop().get_is_primary_or_accepted_culture()) {
					bsum += po.get_pop().get_size();
				}
			}
		});
		auto total_pop = state.world.state_instance_get_demographics(si, demographics::total);
		auto total = total_pop > 0 ? std::clamp((non_core_effect + state.culture_definitions.bureaucrat_tax_efficiency * bsum / total_pop) / from_issues, 0.0f, 1.0f) : 0.0f;

		state.world.state_instance_set_administrative_efficiency(si, total);

	});
}
*/

bool has_railroads_being_built(sys::state& state, dcon::province_id id) {
	return false;
}

bool can_build_railroads(sys::state& state, dcon::province_id id) {
	auto nation = state.world.province_get_nation_from_province_ownership(id);
	int32_t current_rails_lvl = state.world.province_get_railroad_level(id);
	int32_t max_local_rails_lvl = state.world.nation_get_max_railroad_level(nation);
	int32_t min_build_railroad = state.world.province_get_modifier_values(id, sys::provincial_mod_offsets::min_build_railroad);

	return !has_railroads_being_built(state, id) && (max_local_rails_lvl - current_rails_lvl - min_build_railroad > 0);
}

float monthly_net_pop_growth(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float monthly_net_pop_promotion_and_demotion(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float monthly_net_pop_internal_migration(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float monthly_net_pop_external_migration(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float rgo_maximum_employment(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float rgo_employment(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float rgo_income(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float rgo_production_quantity(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float internal_get_state_admin_efficiency(sys::state& state, dcon::state_instance_id si) {
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);

	auto admin_mod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::administrative_efficiency );

	float issue_sum = 0.0f;
	for(auto i : state.culture_definitions.social_issues) {
		issue_sum = issue_sum + state.world.issue_option_get_administrative_multiplier(state.world.nation_get_issues(owner, i));
	}
	auto from_issues = issue_sum * state.defines.bureaucracy_percentage_increment + state.defines.max_bureaucracy_percentage;
	float non_core_effect = 0.0f;
	float bsum = 0.0f;
	for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
		if(!state.world.province_get_is_owner_core(p)) {
			non_core_effect += state.defines.noncore_tax_penalty;
		}
		for(auto po : state.world.province_get_pop_location(p)) {
			if(po.get_pop().get_is_primary_or_accepted_culture()) {
				bsum += po.get_pop().get_size();
			}
		}
	});
	auto total_pop = state.world.state_instance_get_demographics(si, demographics::total);
	auto total = total_pop > 0 ? std::clamp((non_core_effect + state.culture_definitions.bureaucrat_tax_efficiency * bsum / total_pop) / from_issues, 0.0f, 1.0f) : 0.0f;

	return total;
}
float crime_fighting_efficiency(sys::state& state, dcon::province_id id) {
	// TODO
	/*
	Crime is apparently the single place where the following value matters:
	- state administrative efficiency: = define:NONCORE_TAX_PENALTY x number-of-non-core-provinces + (bureaucrat-tax-efficiency x total-number-of-primary-or-accepted-culture-bureaucrats / population-of-state)v1 / x (sum-of-the-administrative_multiplier-for-social-issues-marked-as-being-administrative x define:BUREAUCRACY_PERCENTAGE_INCREMENT + define:MAX_BUREAUCRACY_PERCENTAGE)), all clamped between 0 and 1.
	The crime fighting percent of a province is then calculated as: (state-administration-efficiency x define:ADMIN_EFFICIENCY_CRIMEFIGHT_PERCENT + administration-spending-setting x (1 - ADMIN_EFFICIENCY_CRIMEFIGHT_PERCENT)) x (define:MAX_CRIMEFIGHTING_PERCENT - define:MIN_CRIMEFIGHTING_PERCENT) + define:MIN_CRIMEFIGHTING_PERCENT
	*/
	// we have agreed to replace admin spending with national admin efficiency

	auto si = state.world.province_get_state_membership(id);
	auto owner = state.world.province_get_nation_from_province_ownership(id);
	if(si && owner)
		return (internal_get_state_admin_efficiency(state, si) * state.defines.admin_efficiency_crimefight_percent + (1 - state.defines.admin_efficiency_crimefight_percent) * state.world.nation_get_administrative_efficiency(owner)) * (state.defines.max_crimefight_percent - state.defines.min_crimefight_percent) + state.defines.min_crimefight_percent;
	else
		return 0.0f;
}
float state_admin_efficiency(sys::state& state, dcon::state_instance_id id) {
	// TODO
	return 0.0f;
}

}
