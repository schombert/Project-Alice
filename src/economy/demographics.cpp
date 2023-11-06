#include "demographics.hpp"
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "nations.hpp"

namespace pop_demographics {

dcon::pop_demographics_key to_key(sys::state const& state, dcon::ideology_id v) {
	return dcon::pop_demographics_key(dcon::pop_demographics_key::value_base_t(v.index() + count_special_keys));
}
dcon::pop_demographics_key to_key(sys::state const& state, dcon::issue_option_id v) {
	return dcon::pop_demographics_key(
			dcon::pop_demographics_key::value_base_t(state.world.ideology_size() + v.index() + count_special_keys));
}
uint32_t size(sys::state const& state) {
	return state.world.ideology_size() + state.world.issue_option_size() + count_special_keys;
}

void regenerate_is_primary_or_accepted(sys::state& state) {
	state.world.for_each_pop([&](dcon::pop_id p) {
		state.world.pop_set_is_primary_or_accepted_culture(p, false);
		auto n = nations::owner_of_pop(state, p);
		if(state.world.nation_get_primary_culture(n) == state.world.pop_get_culture(p)) {
			state.world.pop_set_is_primary_or_accepted_culture(p, true);
			return;
		}
		auto accepted = state.world.nation_get_accepted_cultures(n);
		for(auto c : accepted) {
			if(c == state.world.pop_get_culture(p)) {
				state.world.pop_set_is_primary_or_accepted_culture(p, true);
				return;
			}
		}
	});
}

} // namespace pop_demographics
namespace demographics {

inline constexpr float small_pop_size = 100.0f;

dcon::demographics_key to_key(sys::state const& state, dcon::ideology_id v) {
	return dcon::demographics_key(dcon::pop_demographics_key::value_base_t(count_special_keys + v.index()));
}
dcon::demographics_key to_key(sys::state const& state, dcon::issue_option_id v) {
	return dcon::demographics_key(
			dcon::pop_demographics_key::value_base_t(count_special_keys + state.world.ideology_size() + v.index()));
}
dcon::demographics_key to_key(sys::state const& state, dcon::pop_type_id v) {
	return dcon::demographics_key(dcon::pop_demographics_key::value_base_t(
			count_special_keys + state.world.ideology_size() + state.world.issue_option_size() + v.index()));
}
dcon::demographics_key to_key(sys::state const& state, dcon::culture_id v) {
	return dcon::demographics_key(
			dcon::pop_demographics_key::value_base_t(count_special_keys + state.world.ideology_size() +
																							 state.world.issue_option_size() + state.world.pop_type_size() + v.index()));
}
dcon::demographics_key to_key(sys::state const& state, dcon::religion_id v) {
	return dcon::demographics_key(dcon::pop_demographics_key::value_base_t(
			count_special_keys + state.world.ideology_size() + state.world.issue_option_size() + state.world.pop_type_size() +
			state.world.culture_size() + v.index()));
}
dcon::demographics_key to_employment_key(sys::state const& state, dcon::pop_type_id v) {
	return dcon::demographics_key(dcon::pop_demographics_key::value_base_t(
			count_special_keys + state.world.ideology_size() + state.world.issue_option_size() + state.world.pop_type_size() +
			state.world.culture_size() + state.world.religion_size() + v.index()));
}

uint32_t size(sys::state const& state) {
	return count_special_keys + state.world.ideology_size() + state.world.issue_option_size() +
				 uint32_t(2) * state.world.pop_type_size() + state.world.culture_size() + state.world.religion_size();
}

template<typename F>
void sum_over_demographics(sys::state& state, dcon::demographics_key key, F const& source) {
	// clear province
	province::ve_for_each_land_province(state, [&](auto pi) { state.world.province_set_demographics(pi, key, ve::fp_vector()); });
	// sum in province
	state.world.for_each_pop([&](dcon::pop_id p) {
		auto location = state.world.pop_get_province_from_pop_location(p);
		state.world.province_get_demographics(location, key) += source(state, p);
	});
	// clear state
	state.world.execute_serial_over_state_instance(
			[&](auto si) { state.world.state_instance_set_demographics(si, key, ve::fp_vector()); });
	// sum in state
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto location = state.world.province_get_state_membership(p);
		state.world.state_instance_get_demographics(location, key) += state.world.province_get_demographics(p, key);
	});
	// clear nation
	state.world.execute_serial_over_nation([&](auto ni) { state.world.nation_set_demographics(ni, key, ve::fp_vector()); });
	// sum in nation
	state.world.for_each_state_instance([&](dcon::state_instance_id s) {
		auto location = state.world.state_instance_get_nation_from_state_ownership(s);
		state.world.nation_get_demographics(location, key) += state.world.state_instance_get_demographics(s, key);
	});
}

void regenerate_from_pop_data(sys::state& state) {

	concurrency::parallel_for(uint32_t(0), size(state), [&](uint32_t index) {
		dcon::demographics_key key{dcon::demographics_key::value_base_t(index)};
		if(index < count_special_keys) {
			switch(index) {
			case 0: // constexpr inline dcon::demographics_key total(0);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) { return state.world.pop_get_size(p); });
				break;
			case 1: // constexpr inline dcon::demographics_key employable(1);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_has_unemployment(state.world.pop_get_poptype(p)) ? state.world.pop_get_size(p) : 0.0f;
				});
				break;
			case 2: // constexpr inline dcon::demographics_key employed(2);
				sum_over_demographics(state, key,
						[](sys::state const& state, dcon::pop_id p) { return state.world.pop_get_employment(p); });
				break;
			case 3: // constexpr inline dcon::demographics_key consciousness(3);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_get_consciousness(p) * state.world.pop_get_size(p);
				});
				break;
			case 4: // constexpr inline dcon::demographics_key militancy(4);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_get_militancy(p) * state.world.pop_get_size(p);
				});
				break;
			case 5: // constexpr inline dcon::demographics_key literacy(5);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_get_literacy(p) * state.world.pop_get_size(p);
				});
				break;
			case 6: // constexpr inline dcon::demographics_key political_reform_desire(6);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					if(state.world.province_get_is_colonial(state.world.pop_get_province_from_pop_location(p)) == false) {
						auto movement = state.world.pop_get_movement_from_pop_movement_membership(p);
						if(movement) {
							auto opt = state.world.movement_get_associated_issue_option(movement);
							auto optpar = state.world.issue_option_get_parent_issue(opt);
							if(opt && state.world.issue_get_issue_type(optpar) == uint8_t(culture::issue_type::political))
								return state.world.pop_get_size(p);
						}
						return 0.0f;
					} else
						return 0.0f;
				});
				break;
			case 7: // constexpr inline dcon::demographics_key social_reform_desire(7);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					if(state.world.province_get_is_colonial(state.world.pop_get_province_from_pop_location(p)) == false) {
						auto movement = state.world.pop_get_movement_from_pop_movement_membership(p);
						if(movement) {
							auto opt = state.world.movement_get_associated_issue_option(movement);
							auto optpar = state.world.issue_option_get_parent_issue(opt);
							if(opt && state.world.issue_get_issue_type(optpar) == uint8_t(culture::issue_type::social))
								return state.world.pop_get_size(p);
						}
						return 0.0f;
					} else
						return 0.0f;
				});
				break;
			case 8: // constexpr inline dcon::demographics_key poor_militancy(8);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::poor)
										 ? state.world.pop_get_militancy(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 9: // constexpr inline dcon::demographics_key middle_militancy(9);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::middle)
										 ? state.world.pop_get_militancy(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 10: // constexpr inline dcon::demographics_key rich_militancy(10);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::rich)
										 ? state.world.pop_get_militancy(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 11: // constexpr inline dcon::demographics_key poor_life_needs(11);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::poor)
										 ? state.world.pop_get_life_needs_satisfaction(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 12: // constexpr inline dcon::demographics_key middle_life_needs(12);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::middle)
										 ? state.world.pop_get_life_needs_satisfaction(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 13: // constexpr inline dcon::demographics_key rich_life_needs(13);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::rich)
										 ? state.world.pop_get_life_needs_satisfaction(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 14: // constexpr inline dcon::demographics_key poor_everyday_needs(14);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::poor)
										 ? state.world.pop_get_everyday_needs_satisfaction(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 15: // constexpr inline dcon::demographics_key middle_everyday_needs(15);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::middle)
										 ? state.world.pop_get_everyday_needs_satisfaction(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 16: // constexpr inline dcon::demographics_key rich_everyday_needs(16);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::rich)
										 ? state.world.pop_get_everyday_needs_satisfaction(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 17: // constexpr inline dcon::demographics_key poor_luxury_needs(17);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::poor)
										 ? state.world.pop_get_luxury_needs_satisfaction(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 18: // constexpr inline dcon::demographics_key middle_luxury_needs(18);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::middle)
										 ? state.world.pop_get_luxury_needs_satisfaction(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 19: // constexpr inline dcon::demographics_key rich_luxury_needs(19);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::rich)
										 ? state.world.pop_get_luxury_needs_satisfaction(p) * state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 20: // constexpr inline dcon::demographics_key poor_total(20);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::poor)
										 ? state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 21: // constexpr inline dcon::demographics_key middle_total(21);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::middle)
										 ? state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			case 22: // constexpr inline dcon::demographics_key rich_total(22);
				sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::rich)
										 ? state.world.pop_get_size(p)
										 : 0.0f;
				});
				break;
			}
		} else if(key.index() < to_key(state, dcon::issue_option_id(0)).index()) { // ideology
			dcon::ideology_id pkey{dcon::ideology_id::value_base_t(index - count_special_keys)};
			auto pdemo_key = pop_demographics::to_key(state, pkey);
			sum_over_demographics(state, key, [pdemo_key](sys::state const& state, dcon::pop_id p) {
				return state.world.pop_get_demographics(p, pdemo_key) * state.world.pop_get_size(p);
			});
		} else if(key.index() < to_key(state, dcon::pop_type_id(0)).index()) { // issue option
			dcon::issue_option_id pkey{dcon::issue_option_id::value_base_t(index - (count_special_keys + state.world.ideology_size()))};
			auto pdemo_key = pop_demographics::to_key(state, pkey);
			sum_over_demographics(state, key, [pdemo_key](sys::state const& state, dcon::pop_id p) {
				return state.world.pop_get_demographics(p, pdemo_key) * state.world.pop_get_size(p);
			});
		} else if(key.index() < to_key(state, dcon::culture_id(0)).index()) { // pop type
			dcon::pop_type_id pkey{dcon::pop_type_id::value_base_t(
					index - (count_special_keys + state.world.ideology_size() + state.world.issue_option_size()))};
			sum_over_demographics(state, key, [pkey](sys::state const& state, dcon::pop_id p) {
				return state.world.pop_get_poptype(p) == pkey ? state.world.pop_get_size(p) : 0.0f;
			});
		} else if(key.index() < to_key(state, dcon::religion_id(0)).index()) { // culture
			dcon::culture_id pkey{
					dcon::culture_id::value_base_t(index - (count_special_keys + state.world.ideology_size() +
																										 state.world.issue_option_size() + state.world.pop_type_size()))};
			sum_over_demographics(state, key, [pkey](sys::state const& state, dcon::pop_id p) {
				return state.world.pop_get_culture(p) == pkey ? state.world.pop_get_size(p) : 0.0f;
			});
		} else if(key.index() < to_employment_key(state, dcon::pop_type_id(0)).index()) { // religion
			dcon::religion_id pkey{dcon::religion_id::value_base_t(
					index - (count_special_keys + state.world.ideology_size() + state.world.issue_option_size() +
											state.world.pop_type_size() + state.world.culture_size()))};
			sum_over_demographics(state, key, [pkey](sys::state const& state, dcon::pop_id p) {
				return state.world.pop_get_religion(p) == pkey ? state.world.pop_get_size(p) : 0.0f;
			});
		} else { // employment amounts
			dcon::pop_type_id pkey{dcon::pop_type_id::value_base_t(
					index - (count_special_keys + state.world.ideology_size() + state.world.issue_option_size() +
											state.world.pop_type_size() + state.world.culture_size() + state.world.religion_size()))};
			if(state.world.pop_type_get_has_unemployment(pkey)) {
				sum_over_demographics(state, key, [pkey](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_get_poptype(p) == pkey ? state.world.pop_get_employment(p) : 0.0f;
				});
			} else {
				sum_over_demographics(state, key, [pkey](sys::state const& state, dcon::pop_id p) {
					return state.world.pop_get_poptype(p) == pkey ? state.world.pop_get_size(p) : 0.0f;
				});
			}
		}
	});

	//
	// calculate values derived from demographics
	//
	concurrency::parallel_for(uint32_t(0), uint32_t(17), [&](uint32_t index) {
		switch(index) {
		case 0: {
			static ve::vectorizable_buffer<float, dcon::province_id> max_buffer = state.world.province_make_vectorizable_float_buffer();

			ve::execute_serial<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()),
					[&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_culture([&](dcon::culture_id c) {
				ve::execute_serial<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()), [&, k = to_key(state, c)](auto p) {
					auto v = state.world.province_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.province_set_dominant_culture(p,
							ve::select(mask, ve::tagged_vector<dcon::culture_id>(c), state.world.province_get_dominant_culture(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 1: {
			static ve::vectorizable_buffer<float, dcon::state_instance_id> max_buffer(uint32_t(1));
			static uint32_t old_count = 1;

			auto new_count = state.world.state_instance_size();
			if(new_count > old_count) {
				max_buffer = state.world.state_instance_make_vectorizable_float_buffer();
				old_count = new_count;
			}
			state.world.execute_serial_over_state_instance([&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_culture([&](dcon::culture_id c) {
				state.world.execute_serial_over_state_instance([&, k = to_key(state, c)](auto p) {
					auto v = state.world.state_instance_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.state_instance_set_dominant_culture(p,
							ve::select(mask, ve::tagged_vector<dcon::culture_id>(c), state.world.state_instance_get_dominant_culture(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 2: {
			static ve::vectorizable_buffer<float, dcon::nation_id> max_buffer(uint32_t(1));
			static uint32_t old_count = 1;

			auto new_count = state.world.nation_size();
			if(new_count > old_count) {
				max_buffer = state.world.nation_make_vectorizable_float_buffer();
				old_count = new_count;
			}
			state.world.execute_serial_over_nation([&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_culture([&](dcon::culture_id c) {
				state.world.execute_serial_over_nation([&, k = to_key(state, c)](auto p) {
					auto v = state.world.nation_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.nation_set_dominant_culture(p,
							ve::select(mask, ve::tagged_vector<dcon::culture_id>(c), state.world.nation_get_dominant_culture(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 3: {
			static ve::vectorizable_buffer<float, dcon::province_id> max_buffer = state.world.province_make_vectorizable_float_buffer();

			ve::execute_serial<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()),
					[&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_religion([&](dcon::religion_id c) {
				ve::execute_serial<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()), [&, k = to_key(state, c)](auto p) {
					auto v = state.world.province_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.province_set_dominant_religion(p,
							ve::select(mask, ve::tagged_vector<dcon::religion_id>(c), state.world.province_get_dominant_religion(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 4: {
			static ve::vectorizable_buffer<float, dcon::state_instance_id> max_buffer(uint32_t(1));
			static uint32_t old_count = 1;

			auto new_count = state.world.state_instance_size();
			if(new_count > old_count) {
				max_buffer = state.world.state_instance_make_vectorizable_float_buffer();
				old_count = new_count;
			}
			state.world.execute_serial_over_state_instance([&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_religion([&](dcon::religion_id c) {
				state.world.execute_serial_over_state_instance([&, k = to_key(state, c)](auto p) {
					auto v = state.world.state_instance_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.state_instance_set_dominant_religion(p,
							ve::select(mask, ve::tagged_vector<dcon::religion_id>(c), state.world.state_instance_get_dominant_religion(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 5: {
			static ve::vectorizable_buffer<float, dcon::nation_id> max_buffer(uint32_t(1));
			static uint32_t old_count = 1;

			auto new_count = state.world.nation_size();
			if(new_count > old_count) {
				max_buffer = state.world.nation_make_vectorizable_float_buffer();
				old_count = new_count;
			}
			state.world.execute_serial_over_nation([&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_religion([&](dcon::religion_id c) {
				state.world.execute_serial_over_nation([&, k = to_key(state, c)](auto p) {
					auto v = state.world.nation_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.nation_set_dominant_religion(p,
							ve::select(mask, ve::tagged_vector<dcon::religion_id>(c), state.world.nation_get_dominant_religion(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 6: {
			static ve::vectorizable_buffer<float, dcon::province_id> max_buffer = state.world.province_make_vectorizable_float_buffer();

			ve::execute_serial<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()),
					[&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_ideology([&](dcon::ideology_id c) {
				ve::execute_serial<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()), [&, k = to_key(state, c)](auto p) {
					auto v = state.world.province_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.province_set_dominant_ideology(p,
							ve::select(mask, ve::tagged_vector<dcon::ideology_id>(c), state.world.province_get_dominant_ideology(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 7: {
			static ve::vectorizable_buffer<float, dcon::state_instance_id> max_buffer(uint32_t(1));
			static uint32_t old_count = 1;

			auto new_count = state.world.state_instance_size();
			if(new_count > old_count) {
				max_buffer = state.world.state_instance_make_vectorizable_float_buffer();
				old_count = new_count;
			}
			state.world.execute_serial_over_state_instance([&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_ideology([&](dcon::ideology_id c) {
				state.world.execute_serial_over_state_instance([&, k = to_key(state, c)](auto p) {
					auto v = state.world.state_instance_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.state_instance_set_dominant_ideology(p,
							ve::select(mask, ve::tagged_vector<dcon::ideology_id>(c), state.world.state_instance_get_dominant_ideology(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 8: {
			static ve::vectorizable_buffer<float, dcon::nation_id> max_buffer(uint32_t(1));
			static uint32_t old_count = 1;

			auto new_count = state.world.nation_size();
			if(new_count > old_count) {
				max_buffer = state.world.nation_make_vectorizable_float_buffer();
				old_count = new_count;
			}
			state.world.execute_serial_over_nation([&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_ideology([&](dcon::ideology_id c) {
				state.world.execute_serial_over_nation([&, k = to_key(state, c)](auto p) {
					auto v = state.world.nation_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.nation_set_dominant_ideology(p,
							ve::select(mask, ve::tagged_vector<dcon::ideology_id>(c), state.world.nation_get_dominant_ideology(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 9: {
			static ve::vectorizable_buffer<float, dcon::province_id> max_buffer = state.world.province_make_vectorizable_float_buffer();

			ve::execute_serial<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()),
					[&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_issue_option([&](dcon::issue_option_id c) {
				ve::execute_serial<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()), [&, k = to_key(state, c)](auto p) {
					auto v = state.world.province_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.province_set_dominant_issue_option(p,
							ve::select(mask, ve::tagged_vector<dcon::issue_option_id>(c), state.world.province_get_dominant_issue_option(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 10: {
			static ve::vectorizable_buffer<float, dcon::state_instance_id> max_buffer(uint32_t(1));
			static uint32_t old_count = 1;

			auto new_count = state.world.state_instance_size();
			if(new_count > old_count) {
				max_buffer = state.world.state_instance_make_vectorizable_float_buffer();
				old_count = new_count;
			}
			state.world.execute_serial_over_state_instance([&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_issue_option([&](dcon::issue_option_id c) {
				state.world.execute_serial_over_state_instance([&, k = to_key(state, c)](auto p) {
					auto v = state.world.state_instance_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.state_instance_set_dominant_issue_option(p, ve::select(mask, ve::tagged_vector<dcon::issue_option_id>(c), state.world.state_instance_get_dominant_issue_option(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 11: {
			static ve::vectorizable_buffer<float, dcon::nation_id> max_buffer(uint32_t(1));
			static uint32_t old_count = 1;

			auto new_count = state.world.nation_size();
			if(new_count > old_count) {
				max_buffer = state.world.nation_make_vectorizable_float_buffer();
				old_count = new_count;
			}
			state.world.execute_serial_over_nation([&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_issue_option([&](dcon::issue_option_id c) {
				state.world.execute_serial_over_nation([&, k = to_key(state, c)](auto p) {
					auto v = state.world.nation_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.nation_set_dominant_issue_option(p,
						ve::select(mask, ve::tagged_vector<dcon::issue_option_id>(c), state.world.nation_get_dominant_issue_option(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 12: {
			static ve::vectorizable_buffer<float, dcon::pop_id> max_buffer(uint32_t(1));
			static uint32_t old_count = 1;

			auto new_count = state.world.pop_size();
			if(new_count > old_count) {
				max_buffer = state.world.pop_make_vectorizable_float_buffer();
				old_count = new_count;
			}
			state.world.execute_serial_over_pop([&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_issue_option([&](dcon::issue_option_id c) {
				state.world.execute_serial_over_pop([&, k = pop_demographics::to_key(state, c)](auto p) {
					auto v = state.world.pop_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.pop_set_dominant_issue_option(p,
						ve::select(mask, ve::tagged_vector<dcon::issue_option_id>(c), state.world.pop_get_dominant_issue_option(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 13: {
			static ve::vectorizable_buffer<float, dcon::pop_id> max_buffer(uint32_t(1));
			static uint32_t old_count = 1;

			auto new_count = state.world.pop_size();
			if(new_count > old_count) {
				max_buffer = state.world.pop_make_vectorizable_float_buffer();
				old_count = new_count;
			}
			state.world.execute_serial_over_pop([&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_ideology([&](dcon::ideology_id c) {
				state.world.execute_serial_over_pop([&, k = pop_demographics::to_key(state, c)](auto p) {
					auto v = state.world.pop_get_demographics(p, k);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max;
					state.world.pop_set_dominant_ideology(p,
						ve::select(mask, ve::tagged_vector<dcon::ideology_id>(c), state.world.pop_get_dominant_ideology(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		case 14: {
			// clear nation
			state.world.execute_serial_over_nation(
					[&](auto ni) { state.world.nation_set_non_colonial_population(ni, ve::fp_vector()); });
			// sum in nation
			state.world.for_each_state_instance([&](dcon::state_instance_id s) {
				if(!state.world.province_get_is_colonial(state.world.state_instance_get_capital(s))) {
					auto location = state.world.state_instance_get_nation_from_state_ownership(s);
					state.world.nation_get_non_colonial_population(location) +=
							state.world.state_instance_get_demographics(s, demographics::total);
				}
			});
			break;
		}
		case 15: {
			// clear nation
			state.world.execute_serial_over_nation(
					[&](auto ni) { state.world.nation_set_non_colonial_bureaucrats(ni, ve::fp_vector()); });
			// sum in nation
			state.world.for_each_state_instance(
					[&, k = demographics::to_key(state, state.culture_definitions.bureaucrat)](dcon::state_instance_id s) {
						if(!state.world.province_get_is_colonial(state.world.state_instance_get_capital(s))) {
							auto location = state.world.state_instance_get_nation_from_state_ownership(s);
							state.world.nation_get_non_colonial_bureaucrats(location) += state.world.state_instance_get_demographics(s, k);
						}
					});
			break;
		}
		case 16:
		{
			static ve::vectorizable_buffer<float, dcon::province_id> max_buffer = state.world.province_make_vectorizable_float_buffer();

			ve::execute_serial<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()),
					[&](auto p) { state.world.province_set_dominant_accepted_culture(p, dcon::culture_id{}); });
			ve::execute_serial<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()),
					[&](auto p) { max_buffer.set(p, ve::fp_vector()); });
			state.world.for_each_culture([&](dcon::culture_id c) {
				ve::execute_serial<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()), [&, key = to_key(state, c)](auto p) {
					auto v = state.world.province_get_demographics(p, key);
					auto old_max = max_buffer.get(p);
					auto mask = v > old_max &&  nations::nation_accepts_culture(state, state.world.province_get_nation_from_province_ownership(p), c);
					state.world.province_set_dominant_accepted_culture(p,
							ve::select(mask, ve::tagged_vector<dcon::culture_id>(c), state.world.province_get_dominant_accepted_culture(p)));
					max_buffer.set(p, ve::select(mask, v, old_max));
				});
			});
			break;
		}
		default:
			break;
		}
	});
}

inline constexpr uint32_t executions_per_block = 16 / ve::vector_size;

template<typename F>
void execute_staggered_blocks(uint32_t offset, uint32_t divisions, uint32_t max, F&& functor) {
	auto block_index = 16 * offset;
	auto const block_advance = 16 * divisions;

	assert(divisions > 10);

	while(block_index < max) {
		for(uint32_t i = 0; i < executions_per_block; ++i) {
			functor(ve::contiguous_tags<dcon::pop_id>(block_index + i * ve::vector_size));
		}
		block_index += block_advance;
	}
}

template<typename F>
void pexecute_staggered_blocks(uint32_t offset, uint32_t divisions, uint32_t max, F&& functor) {
	concurrency::parallel_for(16 * offset, max, 16 * divisions, [&](uint32_t index) {
		for(uint32_t i = 0; i < executions_per_block; ++i) {
			functor(ve::contiguous_tags<dcon::pop_id>(index + i * ve::vector_size));
		}
	});
}

void update_militancy(sys::state& state, uint32_t offset, uint32_t divisions) {
	/*
	Let us define the local pop militancy modifier as the province's militancy modifier + the nation's militancy modifier + the
	nation's core pop militancy modifier (for non-colonial states, not just core provinces).

	Each pop has its militancy adjusted by the
	local-militancy-modifier
	+ (technology-separatism-modifier + 1) x define:MIL_NON_ACCEPTED (if the pop is not of a primary or accepted culture)
	- (pop-life-needs-satisfaction - 0.5) x define:MIL_NO_LIFE_NEED
	- (pop-everyday-needs-satisfaction - 0.5)^0 x define:MIL_LACK_EVERYDAY_NEED
	+ (pop-everyday-needs-satisfaction - 0.5)v0 x define:MIL_HAS_EVERYDAY_NEED
	+ (pop-luxury-needs-satisfaction - 0.5)v0 x define:MIL_HAS_LUXURY_NEED
	+ pops-support-for-conservatism x define:MIL_IDEOLOGY / 100
	+ pops-support-for-the-ruling-party-ideology x define:MIL_RULING_PARTY / 100
	- (if the pop has an attached regiment, applied at most once) leader-reliability-trait / 1000 + define:MIL_WAR_EXHAUSTION x
	national-war-exhaustion x (sum of support-for-each-issue x issues-war-exhaustion-effect) / 100.0
	+ (for pops not in colonies) pops-social-issue-support x define:MIL_REQUIRE_REFORM
	+ (for pops not in colonies) pops-political-issue-support x define:MIL_REQUIRE_REFORM
	*/

	auto const conservatism_key = pop_demographics::to_key(state, state.culture_definitions.conservative);

	execute_staggered_blocks(offset, divisions, state.world.pop_size(), [&](auto ids) {
		auto loc = state.world.pop_get_province_from_pop_location(ids);
		auto owner = state.world.province_get_nation_from_province_ownership(loc);
		auto ruling_party = state.world.nation_get_ruling_party(owner);
		auto ruling_ideology = state.world.political_party_get_ideology(ruling_party);

		auto lx_mod = ve::max(state.world.pop_get_luxury_needs_satisfaction(ids) - 0.5f, 0.0f) * state.defines.mil_has_luxury_need;
		auto con_sup = (state.world.pop_get_demographics(ids, conservatism_key) * state.defines.mil_ideology);
		auto ruling_sup = ve::apply(
				[&](dcon::pop_id p, dcon::ideology_id i) {
					return i ? state.world.pop_get_demographics(p, pop_demographics::to_key(state, i)) * state.defines.mil_ruling_party
									 : 0.0f;
				},
				ids, ruling_ideology);
		auto ref_mod = ve::select(state.world.province_get_is_colonial(loc), 0.0f,
				(state.world.pop_get_social_reform_desire(ids) + state.world.pop_get_political_reform_desire(ids)) *
						state.defines.mil_require_reform);

		auto sub_t = (lx_mod + ruling_sup) + (con_sup + ref_mod);

		auto pmod = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::pop_militancy_modifier);
		auto omod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_pop_militancy_modifier);
		auto cmod = ve::select(state.world.province_get_is_colonial(loc), 0.0f,
				state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::core_pop_militancy_modifier));

		auto local_mod = (pmod + omod) + cmod;

		auto sep_mod = ve::select(state.world.pop_get_is_primary_or_accepted_culture(ids), 0.0f,
				(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::seperatism) + 1.0f) *
						state.defines.mil_non_accepted);
		auto ln_mod = ve::min((state.world.pop_get_life_needs_satisfaction(ids) - 0.5f), 0.0f) * state.defines.mil_no_life_need;
		auto en_mod_a =
				ve::min(0.0f, (state.world.pop_get_everyday_needs_satisfaction(ids) - 0.5f)) * state.defines.mil_lack_everyday_need;
		auto en_mod_b =
				ve::max(0.0f, (state.world.pop_get_everyday_needs_satisfaction(ids) - 0.5f)) * state.defines.mil_has_everyday_need;

		auto old_mil = state.world.pop_get_militancy(ids);

		state.world.pop_set_militancy(ids,
				ve::min(ve::max(0.0f, ve::select(owner != dcon::nation_id{}, (sub_t + (local_mod + old_mil)) + ((sep_mod - ln_mod) + (en_mod_b - en_mod_a)), 0.0f)), 10.0f));
	});
}

float get_estimated_mil_change(sys::state& state, dcon::pop_id ids) {
	auto const conservatism_key = pop_demographics::to_key(state, state.culture_definitions.conservative);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);
	auto ruling_party = state.world.nation_get_ruling_party(owner);
	auto ruling_ideology = state.world.political_party_get_ideology(ruling_party);

	float lx_mod = std::max(state.world.pop_get_luxury_needs_satisfaction(ids) - 0.5f, 0.0f) * state.defines.mil_has_luxury_need;
	float con_sup = (state.world.pop_get_demographics(ids, conservatism_key) * state.defines.mil_ideology);
	float ruling_sup = ruling_ideology
		? state.world.pop_get_demographics(ids, pop_demographics::to_key(state, ruling_ideology)) * state.defines.mil_ruling_party
		: 0.0f;
	float ref_mod = state.world.province_get_is_colonial(loc) ? 0.0f :
			(state.world.pop_get_social_reform_desire(ids) + state.world.pop_get_political_reform_desire(ids)) *
					state.defines.mil_require_reform;

	float sub_t = (lx_mod + ruling_sup) + (con_sup + ref_mod);

	float pmod = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::pop_militancy_modifier);
	float omod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_pop_militancy_modifier);
	float cmod = ve::select(state.world.province_get_is_colonial(loc), 0.0f,
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::core_pop_militancy_modifier));

	float local_mod = (pmod + omod) + cmod;

	float sep_mod = ve::select(state.world.pop_get_is_primary_or_accepted_culture(ids), 0.0f,
			(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::seperatism) + 1.0f) *
					state.defines.mil_non_accepted);
	float ln_mod = std::min((state.world.pop_get_life_needs_satisfaction(ids) - 0.5f), 0.0f) * state.defines.mil_no_life_need;
	float en_mod_a =
			std::min(0.0f, (state.world.pop_get_everyday_needs_satisfaction(ids) - 0.5f)) * state.defines.mil_lack_everyday_need;
	float en_mod_b =
			std::max(0.0f, (state.world.pop_get_everyday_needs_satisfaction(ids) - 0.5f)) * state.defines.mil_has_everyday_need;

	return (sub_t + local_mod) + ((sep_mod - ln_mod) + (en_mod_b - en_mod_a));
}

float get_estimated_mil_change(sys::state& state, dcon::nation_id n) {
	float sum = 0.0f;
	for(auto prov : dcon::fatten(state.world, n).get_province_ownership()) {
		for(auto pop : prov.get_province().get_pop_location()) {
			sum += pop.get_pop().get_size() * get_estimated_mil_change(state, pop.get_pop());
		}
	}
	return sum / state.world.nation_get_demographics(n, demographics::total);
}

void update_consciousness(sys::state& state, uint32_t offset, uint32_t divisions) {
	// local consciousness modifier = province-pop-consciousness-modifier + national-pop-consciousness-modifier +
	// national-core-pop-consciousness-modifier (in non-colonial states)
	/*
	the daily change in consciousness is:
	(pop-luxury-needs-satisfaction x define:CON_LUXURY_GOODS
	+ define:CON_POOR_CLERGY or define:CON_MIDRICH_CLERGY x clergy-fraction-in-province
	+ national-plurality x 0v((national-literacy-consciousness-impact-modifier + 1) x define:CON_LITERACY x pop-literacy)) x
	define:CON_COLONIAL_FACTOR if colonial
	+ national-non-accepted-pop-consciousness-modifier (if not a primary or accepted culture)
	+ local consciousnesses modifier
	*/

	auto const clergy_key = demographics::to_key(state, state.culture_definitions.clergy);

	execute_staggered_blocks(offset, divisions, state.world.pop_size(), [&](auto ids) {
		auto loc = state.world.pop_get_province_from_pop_location(ids);
		auto owner = state.world.province_get_nation_from_province_ownership(loc);
		auto cfrac =
				state.world.province_get_demographics(loc, clergy_key) / state.world.province_get_demographics(loc, demographics::total);
		auto types = state.world.pop_get_poptype(ids);

		auto lx_mod = state.world.pop_get_luxury_needs_satisfaction(ids) * state.defines.con_luxury_goods;
		auto cl_mod = cfrac * ve::select(state.world.pop_type_get_strata(types) == int32_t(culture::pop_strata::poor),
															ve::fp_vector{state.defines.con_poor_clergy}, ve::fp_vector{state.defines.con_midrich_clergy});
		auto lit_mod = (state.world.nation_get_plurality(owner) / 10.0f) *
			 (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::literacy_con_impact) + 1.0f) *
			state.defines.con_literacy * state.world.pop_get_literacy(ids) *
			 ve::select(state.world.province_get_is_colonial(loc), ve::fp_vector{state.defines.con_colonial_factor}, 1.0f);

		auto pmod = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::pop_consciousness_modifier);
		auto omod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_pop_consciousness_modifier);
		auto cmod = ve::select(state.world.province_get_is_colonial(loc), 0.0f,
				state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::core_pop_consciousness_modifier));

		auto local_mod = (pmod + omod) + cmod;

		auto sep_mod = ve::select(state.world.pop_get_is_primary_or_accepted_culture(ids), 0.0f,
				state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::non_accepted_pop_consciousness_modifier));

		auto old_con = state.world.pop_get_consciousness(ids);

		state.world.pop_set_consciousness(ids,
				ve::min(ve::max(ve::select(owner != dcon::nation_id{}, ((old_con + lx_mod) + (cl_mod + lit_mod)) + (local_mod + sep_mod), 0.0f), 0.0f), 10.f));
	});
}

float get_estimated_con_change(sys::state& state, dcon::pop_id ids) {
	auto const clergy_key = demographics::to_key(state, state.culture_definitions.clergy);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);
	float cfrac =
			state.world.province_get_demographics(loc, clergy_key) / state.world.province_get_demographics(loc, demographics::total);
	auto types = state.world.pop_get_poptype(ids);

	float lx_mod = state.world.pop_get_luxury_needs_satisfaction(ids) * state.defines.con_luxury_goods;
	float cl_mod = cfrac * ve::select(state.world.pop_type_get_strata(types) == int32_t(culture::pop_strata::poor),
														state.defines.con_poor_clergy, state.defines.con_midrich_clergy);
	float lit_mod = (state.world.nation_get_plurality(owner) / 10.0f) *
		 (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::literacy_con_impact) + 1.0f) *
		 state.defines.con_literacy * state.world.pop_get_literacy(ids) *
		ve::select(state.world.province_get_is_colonial(loc), state.defines.con_colonial_factor, 1.0f);

	float pmod = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::pop_consciousness_modifier);
	float omod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_pop_consciousness_modifier);
	float cmod = ve::select(state.world.province_get_is_colonial(loc), 0.0f,
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::core_pop_consciousness_modifier));

	float local_mod = (pmod + omod) + cmod;

	float sep_mod = ve::select(state.world.pop_get_is_primary_or_accepted_culture(ids), 0.0f,
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::non_accepted_pop_consciousness_modifier));

	return (lx_mod + (cl_mod + lit_mod)) + (local_mod + sep_mod);
}

float get_estimated_con_change(sys::state& state, dcon::nation_id n) {
	float sum = 0.0f;
	for(auto prov : dcon::fatten(state.world, n).get_province_ownership()) {
		for(auto pop : prov.get_province().get_pop_location()) {
			sum += pop.get_pop().get_size() * get_estimated_con_change(state, pop.get_pop());
		}
	}
	return sum / state.world.nation_get_demographics(n, demographics::total);
}


void update_literacy(sys::state& state, uint32_t offset, uint32_t divisions) {
	/*
	the literacy of each pop changes by:
	0.01
	x define:LITERACY_CHANGE_SPEED
	x education-spending
	x ((total-province-clergy-population / total-province-population - define:BASE_CLERGY_FOR_LITERACY) /
	(define:MAX_CLERGY_FOR_LITERACY
	- define:BASE_CLERGY_FOR_LITERACY))^1 x (national-modifier-to-education-efficiency + 1.0) x (tech-education-efficiency + 1.0).
	Literacy cannot drop below 0.01.
	*/

	auto const clergy_key = demographics::to_key(state, state.culture_definitions.clergy);

	execute_staggered_blocks(offset, divisions, state.world.pop_size(), [&](auto ids) {
		auto loc = state.world.pop_get_province_from_pop_location(ids);
		auto owner = state.world.province_get_nation_from_province_ownership(loc);
		auto cfrac =
				state.world.province_get_demographics(loc, clergy_key) / state.world.province_get_demographics(loc, demographics::total);

		auto tmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency) + 1.0f;
		auto nmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency_modifier) + 1.0f;
		auto espending =
				(ve::to_float(state.world.nation_get_education_spending(owner)) / 100.0f) * state.world.nation_get_spending_level(owner);
		auto cmod = ve::max(0.0f, ve::min(1.0f, (cfrac - state.defines.base_clergy_for_literacy) /
																	(state.defines.max_clergy_for_literacy - state.defines.base_clergy_for_literacy)));

		auto old_lit = state.world.pop_get_literacy(ids);
		auto new_lit = ve::min(
				ve::max(old_lit + (0.01f * state.defines.literacy_change_speed) * ((espending * cmod) * (tmod * nmod)), 0.01f), 1.0f);

		state.world.pop_set_literacy(ids, ve::select(owner != dcon::nation_id{}, new_lit, old_lit));
	});
}

float get_estimated_literacy_change(sys::state& state, dcon::pop_id ids) {
	auto const clergy_key = demographics::to_key(state, state.culture_definitions.clergy);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);
	auto cfrac =
		state.world.province_get_demographics(loc, clergy_key) / state.world.province_get_demographics(loc, demographics::total);

	auto tmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency) + 1.0f;
	auto nmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency_modifier) + 1.0f;
	auto espending =
		(float(state.world.nation_get_education_spending(owner)) / 100.0f) * state.world.nation_get_spending_level(owner);
	auto cmod = std::max(0.0f, std::min(1.0f, (cfrac - state.defines.base_clergy_for_literacy) /
		(state.defines.max_clergy_for_literacy - state.defines.base_clergy_for_literacy)));

	auto old_lit = state.world.pop_get_literacy(ids);
	auto new_lit = std::min(std::max(old_lit + (0.01f * state.defines.literacy_change_speed) * ((espending * cmod) * (tmod * nmod)), 0.01f), 1.0f);

	return new_lit - old_lit;
}

float get_estimated_literacy_change(sys::state& state, dcon::nation_id n) {
	float sum = 0.0f;
	for(auto prov : dcon::fatten(state.world, n).get_province_ownership()) {
		for(auto pop : prov.get_province().get_pop_location()) {
			sum += pop.get_pop().get_size() * get_estimated_literacy_change(state, pop.get_pop());
		}
	}
	return sum / state.world.nation_get_demographics(n, demographics::total);
}

inline constexpr float ideology_change_rate = 0.10f;

void update_ideologies(sys::state& state, uint32_t offset, uint32_t divisions, ideology_buffer& ibuf) {
	/*
	For ideologies after their enable date (actual discovery / activation is irrelevant), and not restricted to civs only for pops
	in an unciv, the attraction modifier is computed *multiplicatively*. Then, these values are collectively normalized.
	*/

	auto new_pop_count = state.world.pop_size();
	ibuf.update(state, new_pop_count);

	// clear totals
	execute_staggered_blocks(offset, divisions, new_pop_count, [&](auto ids) { ibuf.totals.set(ids, ve::fp_vector{}); });

	// update
	state.world.for_each_ideology([&](dcon::ideology_id i) {
		if(state.world.ideology_get_enabled(i)) {
			auto const i_key = pop_demographics::to_key(state, i);
			if(state.world.ideology_get_is_civilized_only(i)) {
				pexecute_staggered_blocks(offset, divisions, new_pop_count, [&](auto ids) {
					auto owner = nations::owner_of_pop(state, ids);

					auto amount = ve::apply(
							[&](dcon::pop_id pid, dcon::pop_type_id ptid, dcon::nation_id o) {
								if(state.world.nation_get_is_civilized(o)) {
									auto ptrigger = state.world.pop_type_get_ideology(ptid, i);
									return ptrigger ? trigger::evaluate_multiplicative_modifier(state, ptrigger, trigger::to_generic(pid),
											trigger::to_generic(pid), 0) : 0.0f;
								} else
									return 0.0f;
							},
							ids, state.world.pop_get_poptype(ids), owner);

					ibuf.temp_buffers[i].set(ids, amount);
					ibuf.totals.set(ids, ibuf.totals.get(ids) + amount);
				});
			} else {
				pexecute_staggered_blocks(offset, divisions, new_pop_count, [&](auto ids) {
					auto owner = nations::owner_of_pop(state, ids);

					auto amount = ve::apply(
							[&](dcon::pop_id pid, dcon::pop_type_id ptid, dcon::nation_id o) {
								auto ptrigger = state.world.pop_type_get_ideology(ptid, i);
								return ptrigger ? trigger::evaluate_multiplicative_modifier(state, ptrigger, trigger::to_generic(pid),
										trigger::to_generic(pid), 0) : 0.0f;
							},
							ids, state.world.pop_get_poptype(ids), owner);

					ibuf.temp_buffers[i].set(ids, amount);
					ibuf.totals.set(ids, ibuf.totals.get(ids) + amount);
				});
			}
		}
	});
}

void apply_ideologies(sys::state& state, uint32_t offset, uint32_t divisions, ideology_buffer& pbuf) {

	/*
	If the normalized value is greater than twice the pop's current support for the ideology: add 0.25 to the pop's current
	support for the ideology If the normalized value is greater than the pop's current support for the ideology: add 0.05 to the
	pop's current support for the ideology If the normalized value is greater than half the pop's current support for the
	ideology: do nothing Otherwise: subtract 0.25 from the pop's current support for the ideology (to a minimum of zero)
	*/

	state.world.for_each_ideology([&](dcon::ideology_id i) {
		if(state.world.ideology_get_enabled(i)) {
			auto const i_key = pop_demographics::to_key(state, i);

			execute_staggered_blocks(offset, divisions, std::min(state.world.pop_size(), pbuf.size), [&](auto ids) {
				auto ttotal = pbuf.totals.get(ids);
				auto avalue = pbuf.temp_buffers[i].get(ids) / ttotal;
				auto current = state.world.pop_get_demographics(ids, i_key);

				state.world.pop_set_demographics(ids, i_key,
						ve::select(ttotal > 0.0f, ideology_change_rate * avalue + (1.0f - ideology_change_rate) * current, current));
			});
		}
	});
}

inline constexpr float issues_change_rate = 0.10f;

void update_issues(sys::state& state, uint32_t offset, uint32_t divisions, issues_buffer& ibuf) {
	/*
	As with ideologies, the attraction modifier for each issue is computed *multiplicatively* and then are collectively
	normalized. Then we zero the attraction for any issue that is not currently possible (i.e. its trigger condition is not met or
	it is not the next/previous step for a next-step type issue, and for uncivs only the party issues are valid here)
	*/

	auto new_pop_count = state.world.pop_size();
	ibuf.update(state, new_pop_count);

	// clear totals
	execute_staggered_blocks(offset, divisions, new_pop_count, [&](auto ids) { ibuf.totals.set(ids, ve::fp_vector{}); });

	// update
	state.world.for_each_issue_option([&](dcon::issue_option_id iid) {
		auto opt = fatten(state.world, iid);
		auto allow = opt.get_allow();
		auto parent_issue = opt.get_parent_issue();
		auto const i_key = pop_demographics::to_key(state, iid);
		auto is_party_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::party);
		auto is_social_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::social);
		auto is_political_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::political);
		auto has_modifier = is_social_issue || is_political_issue;
		auto modifier_key =
				is_social_issue ? sys::national_mod_offsets::social_reform_desire : sys::national_mod_offsets::political_reform_desire;

		pexecute_staggered_blocks(offset, divisions, new_pop_count, [&](auto ids) {
			auto owner = nations::owner_of_pop(state, ids);
			auto current_issue_setting = state.world.nation_get_issues(owner, parent_issue);
			auto allowed_by_owner =
				(state.world.nation_get_is_civilized(owner) || ve::mask_vector(is_party_issue))
				&& (ve::mask_vector(!state.world.issue_get_is_next_step_only(parent_issue)) ||
						(ve::tagged_vector<int32_t>(current_issue_setting) == iid.index()) ||
						(ve::tagged_vector<int32_t>(current_issue_setting) == iid.index() - 1) ||
						(ve::tagged_vector<int32_t>(current_issue_setting) == iid.index() + 1));
			auto owner_modifier =
					has_modifier ? (state.world.nation_get_modifier_values(owner, modifier_key) + 1.0f) : ve::fp_vector(1.0f);

			auto amount = owner_modifier * ve::select(allowed_by_owner,
				ve::apply([&](dcon::pop_id pid, dcon::pop_type_id ptid, dcon::nation_id o) {
					 if(auto mtrigger = state.world.pop_type_get_issues(ptid, iid); mtrigger) {
						 return trigger::evaluate_multiplicative_modifier(state, mtrigger,  trigger::to_generic(pid), trigger::to_generic(pid), 0);
					 } else {
						return 0.0f;
					 }
				 },  ids, state.world.pop_get_poptype(ids), owner),
				0.0f);

			ibuf.temp_buffers[iid].set(ids, amount);
			ibuf.totals.set(ids, ibuf.totals.get(ids) + amount);
		});
	});
}

void apply_issues(sys::state& state, uint32_t offset, uint32_t divisions, issues_buffer& pbuf) {
	/*
	Then, like with ideologies, we check how much the normalized attraction is above and below the current support, with a couple
	of differences. First, for political or social issues, we multiply the magnitude of the adjustment by
	(national-political-reform-desire-modifier + 1) or (national-social-reform-desire-modifier + 1) as appropriate. Secondly, the
	base magnitude of the change is either (national-issue-change-speed-modifier + 1.0) x 0.25 or
	(national-issue-change-speed-modifier + 1.0) x 0.05 (instead of a fixed 0.05 or 0.25). Finally, there is an additional "bin"
	at 5x more or less where the adjustment is a flat 1.0.
	*/

	state.world.for_each_issue_option([&](dcon::issue_option_id i) {
		auto const i_key = pop_demographics::to_key(state, i);

		execute_staggered_blocks(offset, divisions, std::min(state.world.pop_size(), pbuf.size), [&](auto ids) {
			auto ttotal = pbuf.totals.get(ids);
			auto avalue = pbuf.temp_buffers[i].get(ids) / ttotal;
			auto current = state.world.pop_get_demographics(ids, i_key);
			auto owner = nations::owner_of_pop(state, ids);
			auto owner_rate_modifier =
					(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::issue_change_speed) + 1.0f);

			state.world.pop_set_demographics(ids, i_key,
					ve::select(ttotal > 0.0f,
							issues_change_rate * owner_rate_modifier * avalue + (1.0f - issues_change_rate * owner_rate_modifier) * current,
							current));
		});
	});
}

void update_growth(sys::state& state, uint32_t offset, uint32_t divisions) {
	/*
	Province pop-growth factor: Only owned provinces grow. To calculate the pop growth in a province: First, calculate the
	modified life rating of the province. This is done by taking the intrinsic life rating and then multiplying by (1 + the
	provincial modifier for life rating). The modified life rating is capped at 40. Take that value, if it is greater than
	define:MIN_LIFE_RATING_FOR_GROWTH, subtract define:MIN_LIFE_RATING_FOR_GROWTH from it, and then multiply by
	define:LIFE_RATING_GROWTH_BONUS. If it is less than define:MIN_LIFE_RATING_FOR_GROWTH, treat it as zero. Now, take that value
	and add it to define:BASE_POPGROWTH. This gives us the growth factor for the province.

	The amount a pop grows is determine by first computing the growth modifier sum: (pop-life-needs -
	define:LIFE_NEED_STARVATION_LIMIT) x province-pop-growth-factor x 4 + province-growth-modifier + tech-pop-growth-modifier +
	national-growth-modifier x 0.1. Then divide that by define:SLAVE_GROWTH_DIVISOR if the pop is a slave, and multiply the pop's
	size to determine how much the pop grows by (growth is computed and applied during the pop's monthly tick).
	*/

	execute_staggered_blocks(offset, divisions, state.world.pop_size(), [&](auto ids) {
		auto loc = state.world.pop_get_province_from_pop_location(ids);
		auto owner = state.world.province_get_nation_from_province_ownership(loc);

		auto base_life_rating = ve::to_float(state.world.province_get_life_rating(loc));
		auto mod_life_rating = ve::min(
				base_life_rating * (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::life_rating) + 1.0f),
				40.0f);
		auto lr_factor =
				ve::max((mod_life_rating - state.defines.min_life_rating_for_growth) * state.defines.life_rating_growth_bonus, 0.0f);
		auto province_factor = lr_factor + state.defines.base_popgrowth;

		auto ln_factor = state.world.pop_get_life_needs_satisfaction(ids) - state.defines.life_need_starvation_limit;
		auto mod_sum = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::population_growth) +
									 state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::pop_growth);

		auto total_factor = ln_factor * province_factor * 4.0f + mod_sum * 0.1f;
		auto old_size = state.world.pop_get_size(ids);
		auto new_size = old_size * total_factor + old_size;

		auto type = state.world.pop_get_poptype(ids);

		state.world.pop_set_size(ids,
				ve::select((owner != dcon::nation_id{}) && (type != state.culture_definitions.slaves), new_size, old_size));
	});
}

float get_monthly_pop_increase(sys::state& state, dcon::pop_id ids) {
	auto type = state.world.pop_get_poptype(ids);
	if(type == state.culture_definitions.slaves)
		return 0.0f;

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);

	auto base_life_rating = float(state.world.province_get_life_rating(loc));
	auto mod_life_rating = std::min(
			base_life_rating * (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::life_rating) + 1.0f), 40.0f);
	auto lr_factor =
			std::max((mod_life_rating - state.defines.min_life_rating_for_growth) * state.defines.life_rating_growth_bonus, 0.0f);
	auto province_factor = lr_factor + state.defines.base_popgrowth;

	auto ln_factor = state.world.pop_get_life_needs_satisfaction(ids) - state.defines.life_need_starvation_limit;
	auto mod_sum = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::population_growth) +
								 state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::pop_growth);

	auto total_factor = ln_factor * province_factor * 4.0f + mod_sum * 0.1f;
	auto old_size = state.world.pop_get_size(ids);

	return old_size * total_factor;
}
int64_t get_monthly_pop_increase(sys::state& state, dcon::nation_id n) {
	float t = 0.0f;
	for(auto prov : state.world.nation_get_province_ownership(n)) {
		for(auto pop : prov.get_province().get_pop_location()) {
			t += get_monthly_pop_increase(state, pop.get_pop());
		}
	}
	return int64_t(t);
}

int64_t get_monthly_pop_increase(sys::state& state, dcon::state_instance_id n) {
	float t = 0.0f;
	province::for_each_province_in_state_instance(state, n, [&](dcon::province_id prov) {
		for(auto pop : state.world.province_get_pop_location(prov)) {
			t += get_monthly_pop_increase(state, pop.get_pop());
		}
	});
	return int64_t(t);
}

int64_t get_monthly_pop_increase(sys::state& state, dcon::province_id n) {
	float t = 0.0f;
	for(auto pop : state.world.province_get_pop_location(n)) {
		t += get_monthly_pop_increase(state, pop.get_pop());
	}
	return int64_t(t);
}

void update_type_changes(sys::state& state, uint32_t offset, uint32_t divisions, promotion_buffer& pbuf) {
	pbuf.update(state.world.pop_size());

	/*
	Pops appear to "promote" into other pops of the same or greater strata. Likewise they "demote" into pops of the same or lesser
	strata. (Thus both promotion and demotion can move pops within the same strata?).
	*/

	pexecute_staggered_blocks(offset, divisions, state.world.pop_size(), [&](auto ids) {
		pbuf.amounts.set(ids, 0.0f);
		auto owners = nations::owner_of_pop(state, ids);
		auto promotion_chances = trigger::evaluate_additive_modifier(state, state.culture_definitions.promotion_chance,
				trigger::to_generic(ids), trigger::to_generic(ids), 0);
		auto demotion_chances = trigger::evaluate_additive_modifier(state, state.culture_definitions.demotion_chance,
				trigger::to_generic(ids), trigger::to_generic(ids), 0);
		ve::apply(
				[&](dcon::pop_id p, dcon::nation_id owner, float promotion_chance, float demotion_chance) {
					/*
					Promotion amount:
					Compute the promotion modifier *additively*. If it it non-positive, there is no promotion for the day. Otherwise,
					if there is a national focus to to a pop type present in the state and the pop in question could possibly promote
					into that type, add the national focus effect to the promotion modifier. Conversely, pops of the focused type, are
					not allowed to promote out. Then multiply this value by national-administrative-efficiency x
					define:PROMOTION_SCALE x pop-size to find out how many promote (although at least one person will promote per day
					if the result is positive).

					Demotion amount:
					Compute the demotion modifier *additively*. If it it non-positive, there is no demotion for the day. Otherwise, if
					there is a national focus to to a pop type present in the state and the pop in question could possibly demote into
					that type, add the national focus effect to the demotion modifier. Then multiply this value by
					define:PROMOTION_SCALE x pop-size to find out how many demote (although at least one person will demote per day if
					the result is positive).
					*/

					if(!owner)
						return;

					auto loc = state.world.pop_get_province_from_pop_location(p);
					auto si = state.world.province_get_state_membership(loc);
					auto nf = state.world.state_instance_get_owner_focus(si);
					auto promoted_type = state.world.national_focus_get_promotion_type(nf);
					auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);
					auto ptype = state.world.pop_get_poptype(p);
					auto strata = state.world.pop_type_get_strata(ptype);

					if(promoted_type) {
						if(promoted_type == ptype) {
							promotion_chance = 0.0f;
						} else if(state.world.pop_type_get_strata(promoted_type) >= strata) {
							promotion_chance += promotion_bonus;
						} else if(state.world.pop_type_get_strata(promoted_type) <= strata) {
							demotion_chance += promotion_bonus;
						}
					}

					if(promotion_chance <= 0.0f && demotion_chance <= 0.0f)
						return; // skip this pop

					float current_size = state.world.pop_get_size(p);

					bool promoting = promotion_chance >= demotion_chance;
					float base_amount = promoting
						? (std::ceil(promotion_chance * state.world.nation_get_administrative_efficiency(owner) * state.defines.promotion_scale * current_size))
						: (std::ceil(demotion_chance * state.defines.promotion_scale * current_size));

					/*if(current_size < small_pop_size && base_amount > 0.0f) {
						pbuf.amounts.set(p, current_size);
					} else*/ if(base_amount >= 0.001f) {
						auto transfer_amount = std::min(current_size, base_amount);
						pbuf.amounts.set(p, transfer_amount);
					}

					tagged_vector<float, dcon::pop_type_id> weights(state.world.pop_type_size());

					/*
					The promotion and demotion factors seem to be computed additively (by taking the sum of all true conditions). If
					there is a national focus set towards a pop type in the state, that is also added into the chances to promote into
					that type. If the net weight for the boosted pop type is > 0, that pop type always seems to be selected as the
					promotion type. Otherwise, the type is chosen at random, proportionally to the weights. If promotion to farmer is
					selected for a mine province, or vice versa, it is treated as selecting laborer instead (or vice versa). This
					obviously has the effect of making those pop types even more likely than they otherwise would be.

					As for demotion, there appear to an extra wrinkle. Pops do not appear to demote into pop types if there is more
					unemployment in that demotion target than in their current pop type. Otherwise, the national focus appears to have
					the same effect with respect to demotion.
					*/

					bool is_state_capital = state.world.state_instance_get_capital(state.world.province_get_state_membership(loc)) == loc;

					if(promoting && promoted_type && state.world.pop_type_get_strata(promoted_type) >= strata &&
							(is_state_capital || state.world.pop_type_get_state_capital_only(promoted_type) == false)) {
						auto promote_mod = state.world.pop_type_get_promotion(ptype, promoted_type);
						if(promote_mod) {
							auto chance =
									trigger::evaluate_additive_modifier(state, promote_mod, trigger::to_generic(p), trigger::to_generic(p), 0) +
									promotion_bonus;
							if(chance > 0) {
								pbuf.types.set(p, promoted_type);
								return; // early exit
							}
						}
					} else if(!promoting && promoted_type && state.world.pop_type_get_strata(promoted_type) <= strata &&
										(is_state_capital || state.world.pop_type_get_state_capital_only(promoted_type) == false)) {
						auto promote_mod = state.world.pop_type_get_promotion(ptype, promoted_type);
						if(promote_mod) {
							auto chance =
									trigger::evaluate_additive_modifier(state, promote_mod, trigger::to_generic(p), trigger::to_generic(p), 0) +
									promotion_bonus;
							if(chance > 0) {
								pbuf.types.set(p, promoted_type);
								return; // early exit
							}
						}
					}

					float chances_total = 0.0f;
					state.world.for_each_pop_type([&](dcon::pop_type_id target_type) {
						if(target_type == ptype) {
							weights[target_type] = 0.0f;
						} else if(!is_state_capital && state.world.pop_type_get_state_capital_only(target_type)) {
							weights[target_type] = 0.0f;
						} else if(promoting && state.world.pop_type_get_strata(promoted_type) >= strata) {
							auto promote_mod = state.world.pop_type_get_promotion(ptype, target_type);
							if(promote_mod) {
								auto chance = std::max(trigger::evaluate_additive_modifier(state, promote_mod, trigger::to_generic(p),
																					 trigger::to_generic(p), 0) +
																					 (target_type == promoted_type ? promotion_bonus : 0.0f),
										0.0f);
								chances_total += chance;
								weights[target_type] = chance;
							} else {
								weights[target_type] = 0.0f;
							}
						} else if(!promoting && state.world.pop_type_get_strata(promoted_type) <= strata) {
							auto promote_mod = state.world.pop_type_get_promotion(ptype, target_type);
							if(promote_mod) {
								auto chance = std::max(trigger::evaluate_additive_modifier(state, promote_mod, trigger::to_generic(p),
																					 trigger::to_generic(p), 0) +
																					 (target_type == promoted_type ? promotion_bonus : 0.0f),
										0.0f);
								chances_total += chance;
								weights[target_type] = chance;
							} else {
								weights[target_type] = 0.0f;
							}
						} else {
							weights[target_type] = 0.0f;
						}
					});

					if(chances_total > 0.0f) {
						auto rvalue = float(rng::get_random(state, uint32_t(p.index())) & 0xFFFF) / float(0xFFFF + 1);
						for(uint32_t i = state.world.pop_type_size(); i-- > 0;) {
							dcon::pop_type_id pr{dcon::pop_type_id::value_base_t(i)};
							rvalue -= weights[pr] / chances_total;
							if(rvalue < 0.0f) {
								pbuf.types.set(p, pr);
								return;
							}
						}
						pbuf.amounts.set(p, 0.0f);
						pbuf.types.set(p, dcon::pop_type_id{});
					} else {
						pbuf.amounts.set(p, 0.0f);
						pbuf.types.set(p, dcon::pop_type_id{});
					}
				},
				ids, owners, promotion_chances, demotion_chances);
	});
}

float get_estimated_type_change(sys::state& state, dcon::pop_id ids) {
	auto owner = nations::owner_of_pop(state, ids);
	auto promotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.promotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);
	auto demotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.demotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto si = state.world.province_get_state_membership(loc);
	auto nf = state.world.state_instance_get_owner_focus(si);
	auto promoted_type = state.world.national_focus_get_promotion_type(nf);
	auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);
	auto ptype = state.world.pop_get_poptype(ids);
	auto strata = state.world.pop_type_get_strata(ptype);

	if(promoted_type) {
		if(promoted_type == ptype) {
			promotion_chance = 0.0f;
		} else if(state.world.pop_type_get_strata(promoted_type) >= strata) {
			promotion_chance += promotion_bonus;
		} else if(state.world.pop_type_get_strata(promoted_type) <= strata) {
			demotion_chance += promotion_bonus;
		}
	}

	if(promotion_chance <= 0.0f && demotion_chance <= 0.0f)
		return 0.0f; // skip this pop

	float current_size = state.world.pop_get_size(ids);

	bool promoting = promotion_chance >= demotion_chance;
	return std::min(current_size, promoting
			? (std::ceil(promotion_chance * state.world.nation_get_administrative_efficiency(owner) *
				state.defines.promotion_scale * current_size))
			: (std::ceil(demotion_chance * state.defines.promotion_scale * current_size)));
}


float get_estimated_promotion(sys::state& state, dcon::pop_id ids) {
	auto owner = nations::owner_of_pop(state, ids);
	auto promotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.promotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);
	auto demotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.demotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto si = state.world.province_get_state_membership(loc);
	auto nf = state.world.state_instance_get_owner_focus(si);
	auto promoted_type = state.world.national_focus_get_promotion_type(nf);
	auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);
	auto ptype = state.world.pop_get_poptype(ids);
	auto strata = state.world.pop_type_get_strata(ptype);

	if(promoted_type) {
		if(promoted_type == ptype) {
			promotion_chance = 0.0f;
		} else if(state.world.pop_type_get_strata(promoted_type) >= strata) {
			promotion_chance += promotion_bonus;
		} else if(state.world.pop_type_get_strata(promoted_type) <= strata) {
			demotion_chance += promotion_bonus;
		}
	}

	if(promotion_chance <= 0.0f && demotion_chance <= 0.0f)
		return 0.0f; // skip this pop

	float current_size = state.world.pop_get_size(ids);

	bool promoting = promotion_chance >= demotion_chance;
	return std::min(current_size, promoting
			? (std::ceil(promotion_chance * state.world.nation_get_administrative_efficiency(owner) *
				state.defines.promotion_scale * current_size))
			: 0.0f);
}
float get_estimated_demotion(sys::state& state, dcon::pop_id ids) {
	auto owner = nations::owner_of_pop(state, ids);
	auto promotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.promotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);
	auto demotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.demotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto si = state.world.province_get_state_membership(loc);
	auto nf = state.world.state_instance_get_owner_focus(si);
	auto promoted_type = state.world.national_focus_get_promotion_type(nf);
	auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);
	auto ptype = state.world.pop_get_poptype(ids);
	auto strata = state.world.pop_type_get_strata(ptype);

	if(promoted_type) {
		if(promoted_type == ptype) {
			promotion_chance = 0.0f;
		} else if(state.world.pop_type_get_strata(promoted_type) >= strata) {
			promotion_chance += promotion_bonus;
		} else if(state.world.pop_type_get_strata(promoted_type) <= strata) {
			demotion_chance += promotion_bonus;
		}
	}

	if(promotion_chance <= 0.0f && demotion_chance <= 0.0f)
		return 0.0f; // skip this pop

	float current_size = state.world.pop_get_size(ids);

	bool promoting = promotion_chance >= demotion_chance;
	return std::min(current_size, promoting
			? 0.0f
			: (std::ceil(demotion_chance * state.defines.promotion_scale * current_size)));
}

void update_assimilation(sys::state& state, uint32_t offset, uint32_t divisions, assimilation_buffer& pbuf) {
	pbuf.update(state.world.pop_size());

	/*
	- cultural assimilation -- For a pop to assimilate, there must be a pop of the same strata of either a primary culture
	(preferred) or accepted culture in the province to assimilate into. (schombert notes: not sure if it is worthwhile preserving
	this limitation)
	*/

	pexecute_staggered_blocks(offset, divisions, state.world.pop_size(), [&](auto ids) {
		pbuf.amounts.set(ids, 0.0f);
		auto loc = state.world.pop_get_province_from_pop_location(ids);
		auto owners = state.world.province_get_nation_from_province_ownership(loc);
		auto assimilation_chances = ve::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.assimilation_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);

		ve::apply(
				[&](dcon::pop_id p, dcon::province_id location, dcon::nation_id owner, float assimilation_chance) {
					// no assimilation in unowned provinces
					if(!owner)
						return; // early exit

					// slaves do not assimilate
					if(state.world.pop_get_poptype(p) == state.culture_definitions.slaves)
						return; // early exit

					// pops of an accepted culture do not assimilate
					if(state.world.pop_get_is_primary_or_accepted_culture(p))
						return; // early exit

					// pops in an overseas and colonial province do not assimilate
					if(state.world.province_get_is_colonial(location) && province::is_overseas(state, location))
						return; // early exit

					if(state.world.province_get_dominant_culture(location) == state.world.pop_get_culture(p))
						return;

					/*
					Amount: define:ASSIMILATION_SCALE x (provincial-assimilation-rate-modifier + 1) x
					(national-assimilation-rate-modifier + 1) x pop-size x assimilation chance factor (computed additively, and always
					at least 0.01).
					*/

					float current_size = state.world.pop_get_size(p);
					float base_amount =
							state.defines.assimilation_scale *
							std::max(0.0f, (state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::assimilation_rate) + 1.0f)) *
							std::max(0.0f, (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_assimilation_rate) + 1.0f)) *
							assimilation_chance * current_size;

					/*
					In a colonial province, assimilation numbers for pops with an *non* "overseas"-type culture group are reduced by a
					factor of 100. In a non-colonial province, assimilation numbers for pops with an *non* "overseas"-type culture
					group are reduced by a factor of 10.
					*/

					auto pc = state.world.pop_get_culture(p);
					
					if(!state.world.culture_group_get_is_overseas(state.world.culture_get_group_from_culture_group_membership(pc))) {
						base_amount /= 10.0f;
					}

					/*
					All pops have their assimilation numbers reduced by a factor of 100 per core in the province sharing their primary
					culture.
					*/

					for(auto core : state.world.province_get_core(location)) {
						if(core.get_identity().get_primary_culture() == pc) {
							base_amount /= 100.0f;
						}
					}

					/*
					If the pop size is less than 100 or thereabouts, they seem to get all assimilated if there is any assimilation.
					*/

					/*if(current_size < small_pop_size && base_amount >= 0.001f) {
						pbuf.amounts.set(p, current_size);
					} else*/ if(base_amount >= 0.001f) {
						auto transfer_amount = std::min(current_size, std::ceil(base_amount));
						pbuf.amounts.set(p, transfer_amount);
					}
				},
				ids, loc, owners, assimilation_chances);
	});
}

float get_estimated_assimilation(sys::state& state, dcon::pop_id ids) {
	auto location = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(location);
	auto assimilation_chances = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.assimilation_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);

	// slaves do not assimilate
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves)
		return 0.0f; // early exit

	// pops of an accepted culture do not assimilate
	if(state.world.pop_get_is_primary_or_accepted_culture(ids))
		return 0.0f; // early exit

	// pops in an overseas and colonial province do not assimilate
	if(state.world.province_get_is_colonial(location) && province::is_overseas(state, location))
		return 0.0f; // early exit

	float current_size = state.world.pop_get_size(ids);
	float base_amount =
		state.defines.assimilation_scale *
		std::max(0.0f, (state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::assimilation_rate) + 1.0f)) *
		std::max(0.0f, (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_assimilation_rate) + 1.0f)) *
			assimilation_chances * current_size;

	/*
	In a colonial province, assimilation numbers for pops with an *non* "overseas"-type culture group are reduced by a
	factor of 100. In a non-colonial province, assimilation numbers for pops with an *non* "overseas"-type culture
	group are reduced by a factor of 10.
	*/

	auto pc = state.world.pop_get_culture(ids);
	if(!state.world.culture_group_get_is_overseas(state.world.culture_get_group_from_culture_group_membership(pc))) {
		base_amount /= 10.0f;
	}
	

	/*
	All pops have their assimilation numbers reduced by a factor of 100 per core in the province sharing their primary
	culture.
	*/

	for(auto core : state.world.province_get_core(location)) {
		if(core.get_identity().get_primary_culture() == pc) {
			base_amount /= 100.0f;
		}
	}

	/*
	If the pop size is less than 100 or thereabouts, they seem to get all assimilated if there is any assimilation.
	*/

	/*if(current_size < 100.0f && base_amount >= 0.001f) {
		return current_size;
	} else*/ if(base_amount >= 0.001f) {
		return std::min(current_size, std::ceil(base_amount));
	} else {
		return 0.0f;
	}
}

namespace impl {
dcon::province_id get_province_target_in_nation(sys::state& state, dcon::nation_id n, dcon::pop_id p) {
	/*
	Destination for internal migration: colonial provinces are not valid targets, nor are non state capital provinces for pop
	types restricted to capitals. Valid provinces are weighted according to the product of the factors, times the value of the
	immigration focus
	+ 1.0 if it is present, times the provinces immigration attractiveness modifier + 1.0. The pop is then distributed more or
	less evenly over those provinces with positive attractiveness in proportion to their attractiveness, or dumped somewhere at
	random if no provinces are attractive.
	*/
	auto weights_buffer = state.world.province_make_vectorizable_float_buffer();
	float total_weight = 0.0f;

	auto modifier = state.world.pop_type_get_migration_target(state.world.pop_get_poptype(p));
	if(!modifier)
		return dcon::province_id{};

	bool limit_to_capitals = state.world.pop_type_get_state_capital_only(state.world.pop_get_poptype(p));
	for(auto loc : state.world.nation_get_province_ownership(n)) {
		if(loc.get_province().get_is_colonial() == false) {
			if(!limit_to_capitals || loc.get_province().get_state_membership().get_capital().id == loc.get_province().id) {
				auto weight = trigger::evaluate_multiplicative_modifier(state, modifier, trigger::to_generic(loc.get_province().id),
													trigger::to_generic(p), 0) *
											(loc.get_province().get_modifier_values(sys::provincial_mod_offsets::immigrant_attract) + 1.0f);

				if(weight > 0.0f) {
					weights_buffer.set(loc.get_province(), weight);
					total_weight += weight;
				}
			}
		}
	}

	if(total_weight <= 0.0f)
		return dcon::province_id{};

	auto rvalue = float(rng::get_random(state, (uint32_t(p.index()) << 2) | uint32_t(1)) & 0xFFFF) / float(0xFFFF + 1);
	for(auto loc : state.world.nation_get_province_ownership(n)) {
		rvalue -= weights_buffer.get(loc.get_province()) / total_weight;
		if(rvalue < 0.0f) {
			return loc.get_province();
		}
	}

	return dcon::province_id{};
}
dcon::province_id get_colonial_province_target_in_nation(sys::state& state, dcon::nation_id n, dcon::pop_id p) {
	/*
	 *only* colonial provinces are valid targets, and pops belonging to cultures with "overseas" = false set will not colonially
	 *migrate outside the same continent. The same trigger seems to be used as internal migration for weighting the colonial
	 *provinces.
	 */
	auto weights_buffer = state.world.province_make_vectorizable_float_buffer();
	float total_weight = 0.0f;

	auto modifier = state.world.pop_type_get_migration_target(state.world.pop_get_poptype(p));
	if(!modifier)
		return dcon::province_id{};

	auto overseas_culture = state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(p));
	auto home_continent = state.world.province_get_continent(state.world.pop_get_province_from_pop_location(p));

	bool limit_to_capitals = state.world.pop_type_get_state_capital_only(state.world.pop_get_poptype(p));
	for(auto loc : state.world.nation_get_province_ownership(n)) {
		if(loc.get_province().get_is_colonial() == true) {
			if((overseas_culture || loc.get_province().get_continent() == home_continent) &&
					(!limit_to_capitals || loc.get_province().get_state_membership().get_capital().id == loc.get_province().id)) {
				auto weight = trigger::evaluate_multiplicative_modifier(state, modifier, trigger::to_generic(loc.get_province().id),
													trigger::to_generic(p), 0) *
											(loc.get_province().get_modifier_values(sys::provincial_mod_offsets::immigrant_attract) + 1.0f);

				if(weight > 0.0f) {
					if(!limit_to_capitals || loc.get_province().get_state_membership().get_capital().id == loc.get_province().id) {
						weights_buffer.set(loc.get_province(), weight);
						total_weight += weight;
					}
				}
			}
		}
	}

	if(total_weight <= 0.0f)
		return dcon::province_id{};

	auto rvalue = float(rng::get_random(state, (uint32_t(p.index()) << 2) | uint32_t(2)) & 0xFFFF) / float(0xFFFF + 1);
	for(auto loc : state.world.nation_get_province_ownership(n)) {
		rvalue -= weights_buffer.get(loc.get_province()) / total_weight;
		if(rvalue < 0.0f) {
			return loc.get_province();
		}
	}

	return dcon::province_id{};
}

dcon::nation_id get_immigration_target(sys::state& state, dcon::nation_id owner, dcon::pop_id p, sys::date day) {
	/*
	Country targets for external migration: must be a country with its capital on a different continent from the source country
	*or* an adjacent country (same continent, but non adjacent, countries are not targets). Each country target is then weighted:
	First, the product of the country migration target modifiers (including the base value) is computed, and any results less than
	0.01 are increased to that value. That value is then multiplied by (1.0 + the nations immigrant attractiveness modifier).
	Assuming that there are valid targets for immigration, the nations with the top three values are selected as the possible
	targets. The pop (or, rather, the part of the pop that is migrating) then goes to one of those three targets, selected at
	random according to their relative attractiveness weight. The final provincial destination for the pop is then selected as if
	doing normal internal migration.
	*/

	auto modifier = state.world.pop_type_get_country_migration_target(state.world.pop_get_poptype(p));
	if(!modifier)
		return dcon::nation_id{};

	dcon::nation_id top_nations[3] = {dcon::nation_id{}, dcon::nation_id{}, dcon::nation_id{}};
	float top_weights[3] = {0.0f, 0.0f, 0.0f};

	auto home_continent = state.world.province_get_continent(state.world.pop_get_province_from_pop_location(p));

	state.world.for_each_nation([&](dcon::nation_id inner) {
		if(state.world.nation_get_owned_province_count(inner) == 0)
			return; // ignore dead nations
		if(state.world.nation_get_is_civilized(inner) == false)
			return; // ignore unciv nations
		if(inner == owner)
			return; // ignore self
		if(state.world.province_get_continent(state.world.nation_get_capital(inner)) == home_continent &&
				!state.world.get_nation_adjacency_by_nation_adjacency_pair(owner, inner)) {
			return; // ignore same continent, non-adjacent nations
		}

		auto weight =
				trigger::evaluate_multiplicative_modifier(state, modifier, trigger::to_generic(inner), trigger::to_generic(p), 0) *
				std::max(0.0f, (state.world.nation_get_modifier_values(inner, sys::national_mod_offsets::global_immigrant_attract) + 1.0f));

		if(weight > top_weights[2]) {
			top_weights[2] = weight;
			top_nations[2] = inner;
			if(top_weights[2] > top_weights[1]) {
				std::swap(top_weights[1], top_weights[2]);
				std::swap(top_nations[1], top_nations[2]);
			}
			if(top_weights[1] > top_weights[0]) {
				std::swap(top_weights[1], top_weights[0]);
				std::swap(top_nations[1], top_nations[0]);
			}
		}
	});

	float total_weight = top_weights[0] + top_weights[1] + top_weights[2];
	if(total_weight <= 0.0f)
		return dcon::nation_id{};

	auto rvalue = float(rng::get_random(state, uint32_t(day.value), (uint32_t(p.index()) << 2) | uint32_t(3)) & 0xFFFF) / float(0xFFFF + 1);
	for(uint32_t i = 0; i < 3; ++i) {
		rvalue -= top_weights[i] / total_weight;
		if(rvalue < 0.0f) {
			return top_nations[i];
		}
	}

	return dcon::nation_id{};
}

} // namespace impl

void update_internal_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf) {
	pbuf.update(state.world.pop_size());

	pexecute_staggered_blocks(offset, divisions, state.world.pop_size(), [&](auto ids) {
		pbuf.amounts.set(ids, 0.0f);
		/*
		For non-slave, non-colonial pops in provinces with a total population > 100, some pops may migrate within the nation. This
		is done by calculating the migration chance factor *additively*. If it is non negative, pops may migrate, and we multiply
		it by (province-immigrant-push-modifier + 1) x define:IMMIGRATION_SCALE x pop-size to find out how many migrate.
		*/

		auto loc = state.world.pop_get_province_from_pop_location(ids);
		auto owners = state.world.province_get_nation_from_province_ownership(loc);
		auto pop_sizes = state.world.pop_get_size(ids);
		auto amounts = ve::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.migration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0),  0.0f) *  pop_sizes * ve::max((state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f), 0.0f) *  state.defines.immigration_scale;

		ve::apply(
				[&](dcon::pop_id p, dcon::province_id location, dcon::nation_id owner, float amount, float pop_size) {
					if(amount <= 0.0f)
						return; // early exit
					if(!owner)
						return; // early exit
					if(state.world.province_get_is_colonial(location))
						return; // early exit
					if(state.world.pop_get_poptype(p) == state.culture_definitions.slaves)
						return; // early exit

					auto dest = impl::get_province_target_in_nation(state, owner, p);

					//if(pop_size < small_pop_size) {
					//	pbuf.amounts.set(p, pop_size);
					// else {
						pbuf.amounts.set(p, std::min(pop_size, std::ceil(amount)));
					//}
					pbuf.destinations.set(p, dest);
				},
				ids, loc, owners, amounts, pop_sizes);
	});
}

float get_estimated_internal_migration(sys::state& state, dcon::pop_id ids) {

	auto loc = state.world.pop_get_province_from_pop_location(ids);

	if(state.world.province_get_is_colonial(loc))
		return 0.0f; // early exit
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves)
		return 0.0f; // early exit

	auto owners = state.world.province_get_nation_from_province_ownership(loc);
	auto pop_sizes = state.world.pop_get_size(ids);
	auto amount = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.migration_chance,
		 trigger::to_generic(ids), trigger::to_generic(ids), 0),  0.0f) * pop_sizes * std::max(0.0f, (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f)) * state.defines.immigration_scale;

	
	if(amount <= 0.0f)
		return 0.0f; // early exit

	auto pop_size = state.world.pop_get_size(ids);
	return std::min(pop_size, std::ceil(amount));
}

void update_colonial_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf) {
	pbuf.update(state.world.pop_size());

	pexecute_staggered_blocks(offset, divisions, state.world.pop_size(), [&](auto ids) {
		pbuf.amounts.set(ids, 0.0f);

		/*
		If a nation has colonies, non-factory worker, non-rich pops in provinces with a total population > 100 may move to the
		colonies. This is done by calculating the colonial migration chance factor *additively*. If it is non negative, pops may
		migrate, and we multiply it by (province-immigrant-push-modifier + 1) x (colonial-migration-from-tech + 1) x
		define:IMMIGRATION_SCALE x pop-size to find out how many migrate.
		*/

		auto loc = state.world.pop_get_province_from_pop_location(ids);
		auto owners = state.world.province_get_nation_from_province_ownership(loc);
		auto pop_sizes = state.world.pop_get_size(ids);
		auto amounts = ve::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.colonialmigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0),  0.0f) *
			 pop_sizes *
			ve::max((state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f), 0.0f) *
			ve::max((state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::colonial_migration) + 1.0f), 0.0f) *
			state.defines.immigration_scale;

		ve::apply(
				[&](dcon::pop_id p, dcon::province_id location, dcon::nation_id owner, float amount, float pop_size) {
					if(amount <= 0.0f)
						return; // early exit
					if(!owner)
						return; // early exit
					if(state.world.nation_get_is_colonial_nation(owner) == false)
						return; // early exit
					auto pt = state.world.pop_get_poptype(p);
					if(state.world.pop_type_get_strata(pt) == uint8_t(culture::pop_strata::rich))
						return; // early exit
					if(state.world.province_get_is_colonial(location))
						return; // early exit
					if(pt == state.culture_definitions.slaves || pt == state.culture_definitions.primary_factory_worker ||
							pt == state.culture_definitions.secondary_factory_worker)
						return; // early exit

					//if(pop_size < small_pop_size) {
					//	pbuf.amounts.set(p, pop_size);
					//} else {
					pbuf.amounts.set(p, std::min(pop_size, std::ceil(amount)));
					//}

					auto dest = impl::get_colonial_province_target_in_nation(state, owner, p);
					pbuf.destinations.set(p, dest);
				},
				ids, loc, owners, amounts, pop_sizes);
	});
}

float get_estimated_colonial_migration(sys::state& state, dcon::pop_id ids) {
	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);

	if(state.world.nation_get_is_colonial_nation(owner) == false)
		return 0.0f; // early exit
	auto pt = state.world.pop_get_poptype(ids);
	if(state.world.pop_type_get_strata(pt) == uint8_t(culture::pop_strata::rich))
		return 0.0f; // early exit
	if(state.world.province_get_is_colonial(loc))
		return 0.0f; // early exit
	if(pt == state.culture_definitions.slaves || pt == state.culture_definitions.primary_factory_worker ||
			pt == state.culture_definitions.secondary_factory_worker)
		return 0.0f; // early exit

	
	auto pop_sizes = state.world.pop_get_size(ids);
	auto amounts = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.colonialmigration_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0),  0.0f)
		* pop_sizes
		* std::max((state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f), 0.0f)
		* std::max((state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::colonial_migration) + 1.0f), 0.0f)
		* state.defines.immigration_scale;

	if(amounts <= 0.0f)
		return 0.0f; // early exit
				
	auto pop_size = state.world.pop_get_size(ids);
	return std::min(pop_size, std::ceil(amounts));
}

void update_immigration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf) {
	pbuf.update(state.world.pop_size());

	pexecute_staggered_blocks(offset, divisions, state.world.pop_size(), [&](auto ids) {
		pbuf.amounts.set(ids, 0.0f);

		/*
		pops in a civ nation that are not in a colony any which do not belong to an `overseas` culture group in provinces with a
		total population > 100 may emigrate. This is done by calculating the emigration migration chance factor *additively*. If
		it is non negative, pops may migrate, and we multiply it by (province-immigrant-push-modifier + 1) x
		1v(province-immigrant-push-modifier + 1) x define:IMMIGRATION_SCALE x pop-size to find out how many migrate.
		*/

		auto loc = state.world.pop_get_province_from_pop_location(ids);
		auto owners = state.world.province_get_nation_from_province_ownership(loc);
		auto pop_sizes = state.world.pop_get_size(ids);
		auto impush = (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f);
		auto trigger_amount = ve::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.emigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);
		auto amounts = trigger_amount
			* pop_sizes
			* ve::max(impush, 0.0f)
			* ve::max(impush, 1.0f)
			* state.defines.immigration_scale;

		ve::apply(
				[&](dcon::pop_id p, dcon::province_id location, dcon::nation_id owner, float amount, float pop_size) {

					if(amount <= 0.0f)
						return; // early exit
					if(!owner)
						return; // early exit
					if(state.world.nation_get_is_civilized(owner) == false)
						return; // early exit
					if(state.world.province_get_is_colonial(location))
						return; // early exit
					if(state.world.pop_get_poptype(p) == state.culture_definitions.slaves)
						return; // early exit
					if(state.world.culture_group_get_is_overseas(
								 state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(p))) == false) {
						return; // early exit
					}

					//if(pop_size < small_pop_size) {
					//	pbuf.amounts.set(p, pop_size);
					//} else {
						pbuf.amounts.set(p, std::min(pop_size, std::ceil(amount)));
					//}

					auto ndest = impl::get_immigration_target(state, owner, p, state.current_date);
					auto dest = impl::get_province_target_in_nation(state, ndest, p);

					pbuf.destinations.set(p, dest);
				},
				ids, loc, owners, amounts, pop_sizes);
	});
}

void estimate_directed_immigration(sys::state& state, dcon::nation_id n, std::vector<float>& national_amounts) {
	auto sz = state.world.nation_size();
	national_amounts.resize(sz);
	for(auto& v : national_amounts) {
		v = 0.0f;
	}

	auto ymd_date = state.current_date.to_ymd(state.start_date);
	auto month_start = sys::year_month_day{ ymd_date.year, ymd_date.month, uint16_t(1) };
	auto next_month_start = ymd_date.month != 12 ? sys::year_month_day{ ymd_date.year, uint16_t(ymd_date.month + 1), uint16_t(1) } : sys::year_month_day{ ymd_date.year + 1, uint16_t(1), uint16_t(1) };
	auto const days_in_month = uint32_t(sys::days_difference(month_start, next_month_start));

	for(auto ids : state.world.in_pop) {
		auto loc = state.world.pop_get_province_from_pop_location(ids);
		auto owners = state.world.province_get_nation_from_province_ownership(loc);

		auto section = uint64_t(ids.id.index()) / 16;
		auto tranche = int32_t(section / days_in_month);
		auto day_of_month = tranche - 10;
		if(day_of_month <= 0)
			day_of_month += days_in_month;
		int32_t day_adjustment = day_of_month - int32_t(ymd_date.day);
		auto est_amount = get_estimated_emigration(state, ids);
		if(est_amount > 0.0f) {
			auto target = impl::get_immigration_target(state, owners, ids, state.current_date + day_adjustment);
			if(owners == n) {
				if(target && uint32_t(target.index()) < sz) {
					national_amounts[uint32_t(target.index())] -= est_amount;
				}
			} else if(target == n) {
				if(uint32_t(owners.index()) < sz) {
					national_amounts[uint32_t(owners.index())] += est_amount;
				}
			}
		}
	}
}

float get_estimated_emigration(sys::state& state, dcon::pop_id ids) {

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owners = state.world.province_get_nation_from_province_ownership(loc);

	if(state.world.nation_get_is_civilized(owners) == false)
		return 0.0f; // early exit
	if(state.world.province_get_is_colonial(loc))
		return 0.0f; // early exit
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves)
		return 0.0f; // early exit
	if(state.world.culture_group_get_is_overseas(
				 state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(ids))) == false) {
		return 0.0f; // early exit
	}

	auto pop_sizes = state.world.pop_get_size(ids);
	auto impush = (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f);
	auto trigger_result = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.emigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);
	auto amounts = trigger_result * pop_sizes * std::max(impush, 0.0f) * std::max(impush, 1.0f) * state.defines.immigration_scale;

	if(amounts <= 0.0f)
		return 0.0f; // early exit

	return std::min(pop_sizes, std::ceil(amounts));
}

namespace impl {
dcon::pop_id find_or_make_pop(sys::state& state, dcon::province_id loc, dcon::culture_id cid, dcon::religion_id rid,
		dcon::pop_type_id ptid) {
	bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(loc));
	if(is_mine && ptid == state.culture_definitions.farmers) {
		ptid = state.culture_definitions.laborers;
	} else if(!is_mine && ptid == state.culture_definitions.laborers) {
		ptid = state.culture_definitions.farmers;
	}
	// TODO: fix state capital only type pops ?
	for(auto pl : state.world.province_get_pop_location(loc)) {
		if(pl.get_pop().get_culture() == cid && pl.get_pop().get_religion() == rid && pl.get_pop().get_poptype() == ptid) {
			return pl.get_pop();
		}
	}
	auto np = fatten(state.world, state.world.create_pop());
	state.world.force_create_pop_location(np, loc);
	np.set_culture(cid);
	np.set_religion(rid);
	np.set_poptype(ptid);

	{
		auto n = state.world.province_get_nation_from_province_ownership(loc);
		if(state.world.nation_get_primary_culture(n) == cid) {
			np.set_is_primary_or_accepted_culture(true);
		} else {
			auto accepted = state.world.nation_get_accepted_cultures(n);
			for(auto c : accepted) {
				if(c == cid) {
					np.set_is_primary_or_accepted_culture(true);
					break;
				}
			}
		}
	}

	{ // initial ideology
		float totals = 0.0f;
		state.world.for_each_ideology([&](dcon::ideology_id i) {
			if(state.world.ideology_get_enabled(i)) {
				auto ptrigger = state.world.pop_type_get_ideology(ptid, i);
				auto const i_key = pop_demographics::to_key(state, i);
				auto owner = nations::owner_of_pop(state, np);
				if(state.world.ideology_get_is_civilized_only(i)) {
					if(state.world.nation_get_is_civilized(owner)) {
						auto amount = ptrigger ? trigger::evaluate_multiplicative_modifier(state, ptrigger, trigger::to_generic(np.id),
								trigger::to_generic(owner), 0) : 0.0f;
						state.world.pop_set_demographics(np, i_key, amount);
						totals += amount;
					}
				} else {
					auto amount = ptrigger ? trigger::evaluate_multiplicative_modifier(state, ptrigger, trigger::to_generic(np.id),
							trigger::to_generic(owner), 0) : 0.0f;
					state.world.pop_set_demographics(np, i_key, amount);
					totals += amount;
				}
			}
		});
		if(totals > 0) {
			state.world.for_each_ideology([&](dcon::ideology_id i) {
				auto const i_key = pop_demographics::to_key(state, i);
				state.world.pop_get_demographics(np, i_key) /= totals;
			});
		}
	}
	{ // initial issues
		float totals = 0.0f;
		state.world.for_each_issue_option([&](dcon::issue_option_id iid) {
			auto opt = fatten(state.world, iid);
			auto allow = opt.get_allow();
			auto parent_issue = opt.get_parent_issue();
			auto const i_key = pop_demographics::to_key(state, iid);
			auto is_party_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::party);
			auto is_social_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::social);
			auto is_political_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::political);
			auto has_modifier = is_social_issue || is_political_issue;
			auto modifier_key =
					is_social_issue ? sys::national_mod_offsets::social_reform_desire : sys::national_mod_offsets::political_reform_desire;

			auto owner = nations::owner_of_pop(state, np);
			auto current_issue_setting = state.world.nation_get_issues(owner, parent_issue);
			auto allowed_by_owner =
					(state.world.nation_get_is_civilized(owner) || is_party_issue) &&
					(!state.world.issue_get_is_next_step_only(parent_issue) || (current_issue_setting.id.index() == iid.index()) || (current_issue_setting.id.index() == iid.index() - 1) || (current_issue_setting.id.index() == iid.index() + 1));
			auto owner_modifier = has_modifier ? (state.world.nation_get_modifier_values(owner, modifier_key) + 1.0f) : 1.0f;
			if(allowed_by_owner) {
				if(auto mtrigger = state.world.pop_type_get_issues(ptid, iid); mtrigger) {
					auto amount = owner_modifier * trigger::evaluate_multiplicative_modifier(state, mtrigger, trigger::to_generic(np.id),
																						 trigger::to_generic(owner), 0);

					state.world.pop_set_demographics(np, i_key, amount);
					totals += amount;
				}
			}
		});
		if(totals > 0) {
			state.world.for_each_issue_option([&](dcon::issue_option_id i) {
				auto const i_key = pop_demographics::to_key(state, i);
				state.world.pop_get_demographics(np, i_key) /= totals;
			});
		}
	}
	return np;
}
} // namespace impl

void apply_type_changes(sys::state& state, uint32_t offset, uint32_t divisions, promotion_buffer& pbuf) {
	execute_staggered_blocks(offset, divisions, std::min(state.world.pop_size(), pbuf.size), [&](auto ids) {
		ve::apply(
				[&](dcon::pop_id p) {
					if(pbuf.amounts.get(p) > 0.0f && pbuf.types.get(p)) {
						auto target_pop = impl::find_or_make_pop(state, state.world.pop_get_province_from_pop_location(p),
								state.world.pop_get_culture(p), state.world.pop_get_religion(p), pbuf.types.get(p));
						state.world.pop_get_size(p) -= pbuf.amounts.get(p);
						state.world.pop_get_size(target_pop) += pbuf.amounts.get(p);
					}
				},
				ids);
	});
}

void apply_assimilation(sys::state& state, uint32_t offset, uint32_t divisions, assimilation_buffer& pbuf) {
	execute_staggered_blocks(offset, divisions, std::min(state.world.pop_size(), pbuf.size), [&](auto ids) {
		auto locs = state.world.pop_get_province_from_pop_location(ids);
		ve::apply(
				[&](dcon::pop_id p, dcon::province_id l, dcon::culture_id dac) {
					if(pbuf.amounts.get(p) > 0.0f) {
						//auto o = nations::owner_of_pop(state, p);
						auto cul = dac ? dac : state.world.province_get_dominant_culture(l);
						auto rel = dac
							? state.world.nation_get_religion(nations::owner_of_pop(state, p))
							: state.world.province_get_dominant_religion(l);
						assert(state.world.pop_get_poptype(p));
						auto target_pop = impl::find_or_make_pop(state, l, cul, rel, state.world.pop_get_poptype(p));
						state.world.pop_get_size(p) -= pbuf.amounts.get(p);
						state.world.pop_get_size(target_pop) += pbuf.amounts.get(p);
					}
				},
				ids, locs, state.world.province_get_dominant_accepted_culture(locs));
	});
}

void apply_internal_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf) {
	execute_staggered_blocks(offset, divisions, std::min(state.world.pop_size(), pbuf.size), [&](auto ids) {
		ve::apply(
				[&](dcon::pop_id p) {
					if(pbuf.amounts.get(p) > 0.0f && pbuf.destinations.get(p)) {
						assert(state.world.pop_get_poptype(p));
						auto target_pop = impl::find_or_make_pop(state, pbuf.destinations.get(p), state.world.pop_get_culture(p),
								state.world.pop_get_religion(p), state.world.pop_get_poptype(p));
						state.world.pop_get_size(p) -= pbuf.amounts.get(p);
						state.world.pop_get_size(target_pop) += pbuf.amounts.get(p);
						state.world.province_get_daily_net_migration(state.world.pop_get_province_from_pop_location(p)) -=
								pbuf.amounts.get(p);
						state.world.province_get_daily_net_migration(pbuf.destinations.get(p)) += pbuf.amounts.get(p);
					}
				},
				ids);
	});
}

void apply_colonial_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf) {
	execute_staggered_blocks(offset, divisions, std::min(state.world.pop_size(), pbuf.size), [&](auto ids) {
		ve::apply(
				[&](dcon::pop_id p) {
					if(pbuf.amounts.get(p) > 0.0f && pbuf.destinations.get(p)) {
						assert(state.world.pop_get_poptype(p));
						auto target_pop = impl::find_or_make_pop(state, pbuf.destinations.get(p), state.world.pop_get_culture(p),
								state.world.pop_get_religion(p), state.world.pop_get_poptype(p));
						state.world.pop_get_size(p) -= pbuf.amounts.get(p);
						state.world.pop_get_size(target_pop) += pbuf.amounts.get(p);
						state.world.province_get_daily_net_migration(state.world.pop_get_province_from_pop_location(p)) -=
								pbuf.amounts.get(p);
						state.world.province_get_daily_net_migration(pbuf.destinations.get(p)) += pbuf.amounts.get(p);
					}
				},
				ids);
	});
}

void apply_immigration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf) {
	execute_staggered_blocks(offset, divisions, std::min(state.world.pop_size(), pbuf.size), [&](auto ids) {
		ve::apply(
				[&](dcon::pop_id p) {
					auto amount = pbuf.amounts.get(p);
					if(amount > 0.0f && pbuf.destinations.get(p)) {
						assert(state.world.pop_get_poptype(p));
						auto target_pop = impl::find_or_make_pop(state, pbuf.destinations.get(p), state.world.pop_get_culture(p),
								state.world.pop_get_religion(p), state.world.pop_get_poptype(p));

						state.world.pop_get_size(p) -= amount;
						state.world.pop_get_size(target_pop) += amount;
						state.world.province_get_daily_net_immigration(state.world.pop_get_province_from_pop_location(p)) -= amount;
						state.world.province_get_daily_net_immigration(pbuf.destinations.get(p)) += amount;
						state.world.province_set_last_immigration(pbuf.destinations.get(p), state.current_date);
					}
				},
				ids);
	});
}

void remove_size_zero_pops(sys::state& state) {
	// IMPORTANT: we count down here so that we can delete as we go, compacting from the end
	for(auto last = state.world.pop_size(); last-- > 0;) {
		dcon::pop_id m{dcon::pop_id::value_base_t(last)};
		if(state.world.pop_get_size(m) < 1.0f) {
			state.world.delete_pop(m);
		}
	}
}

void remove_small_pops(sys::state& state) {
	// IMPORTANT: we count down here so that we can delete as we go, compacting from the end
	for(auto last = state.world.pop_size(); last-- > 0;) {
		dcon::pop_id m{ dcon::pop_id::value_base_t(last) };
		if(state.world.pop_get_size(m) < 20.0f) {
			state.world.delete_pop(m);
		}
	}
}


} // namespace demographics
