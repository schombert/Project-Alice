#include "demographics.hpp"
#include "dcon_generated.hpp"
#include "system_state.hpp"

namespace pop_demographics {

dcon::pop_demographics_key to_key(sys::state const& state, dcon::ideology_id v) {
	return dcon::pop_demographics_key(dcon::pop_demographics_key::value_base_t(v.index()));
}
dcon::pop_demographics_key to_key(sys::state const& state, dcon::issue_option_id v) {
	return dcon::pop_demographics_key(dcon::pop_demographics_key::value_base_t(state.world.ideology_size() + v.index()));
}
uint32_t size(sys::state const& state) {
	return state.world.ideology_size() + state.world.issue_option_size();
}

}
namespace demographics {

//constexpr inline uint32_t count_special_keys = 6;

dcon::demographics_key to_key(sys::state const& state, dcon::ideology_id v) {
	return dcon::demographics_key(dcon::pop_demographics_key::value_base_t(count_special_keys + v.index()));
}
dcon::demographics_key to_key(sys::state const& state, dcon::issue_option_id v) {
	return dcon::demographics_key(dcon::pop_demographics_key::value_base_t(count_special_keys + state.world.ideology_size() + v.index()));
}
dcon::demographics_key to_key(sys::state const& state, dcon::pop_type_id v) {
	return dcon::demographics_key(dcon::pop_demographics_key::value_base_t(count_special_keys + state.world.ideology_size() + state.world.issue_option_size() + v.index()));
}
dcon::demographics_key to_key(sys::state const& state, dcon::culture_id v) {
	return dcon::demographics_key(dcon::pop_demographics_key::value_base_t(count_special_keys + state.world.ideology_size() + state.world.issue_option_size() + state.world.pop_type_size() + v.index()));
}
dcon::demographics_key to_key(sys::state const& state, dcon::religion_id v) {
	return dcon::demographics_key(dcon::pop_demographics_key::value_base_t(count_special_keys + state.world.ideology_size() + state.world.issue_option_size() + state.world.pop_type_size() + state.world.culture_size() + v.index()));
}

uint32_t size(sys::state const& state) {
	return count_special_keys + state.world.ideology_size() + state.world.issue_option_size() +
		state.world.pop_type_size() + state.world.culture_size() + state.world.religion_size();
}

template<typename F>
void sum_over_demographics(sys::state& state, dcon::demographics_key key, F const& source) {
	//clear province
	province::for_each_land_province(state, [&](dcon::province_id pi) {
		state.world.province_set_demographics(pi, key, 0.0f); // TODO: perform clears as a vector op.
	});
	//sum in province
	state.world.for_each_pop([&](dcon::pop_id p) {
		auto location = state.world.pop_get_province_from_pop_location(p);
		state.world.province_get_demographics(location, key) += source(state, p);
	});
	//clear state
	state.world.for_each_state_instance([&](dcon::state_instance_id si) {
		state.world.state_instance_set_demographics(si, key, 0.0f); // TODO: perform clears as a vector op.
	});
	//sum in state
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto location = state.world.province_get_state_membership(p);
		state.world.state_instance_get_demographics(location, key) += state.world.province_get_demographics(p, key);
	});
	//clear nation
	state.world.for_each_nation([&](dcon::nation_id ni) {
		state.world.nation_set_demographics(ni, key, 0.0f); // TODO: perform clears as a vector op.
	});
	//sum in nation
	state.world.for_each_state_instance([&](dcon::state_instance_id s) {
		auto location = state.world.state_instance_get_nation_from_state_ownership(s);
		state.world.nation_get_demographics(location, key) += state.world.state_instance_get_demographics(s, key);
	});
}

void regenerate_from_pop_data(sys::state& state) {

	// TODO: regenerate pop political and social reform desire

	concurrency::parallel_for(uint32_t(0), size(state), [&](uint32_t index) {
		dcon::demographics_key key{ dcon::demographics_key::value_base_t(index) };
		if(index < count_special_keys) {
			switch(index) {
				case 0: //constexpr inline dcon::demographics_key total(0);
					sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) { return state.world.pop_get_size(p); });
					break;
				case 1: //constexpr inline dcon::demographics_key employable(1);
					sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
						return state.world.pop_type_get_has_unemployment(state.world.pop_get_poptype(p)) ?
							state.world.pop_get_size(p) : 0.0f;
					});
					break;
				case 2: //constexpr inline dcon::demographics_key employed(2);
					sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
						return state.world.pop_get_employment(p);
					});
					break;
				case 3: //constexpr inline dcon::demographics_key consciousness(3);
					sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
						return state.world.pop_get_consciousness(p) * state.world.pop_get_size(p);
					});
					break;
				case 4: //constexpr inline dcon::demographics_key militancy(4);
					sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
						return state.world.pop_get_militancy(p) * state.world.pop_get_size(p);
					});
					break;
				case 5: //constexpr inline dcon::demographics_key literacy(5);
					sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
						return state.world.pop_get_literacy(p) * state.world.pop_get_size(p);
					});
					break;
				case 6: //constexpr inline dcon::demographics_key political_reform_desire(6);
					sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
						return state.world.pop_get_political_reform_desire(p);
					});
					break;
				case 7: //constexpr inline dcon::demographics_key social_reform_desire(7);
					sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
						return state.world.pop_get_social_reform_desire(p);
					});
					break;
				case 8:	//constexpr inline dcon::demographics_key poor_militancy(8);
					sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
						return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::poor) ?
							state.world.pop_get_militancy(p) * state.world.pop_get_size(p) : 0.0f;
					});
					break;
				case 9:	//constexpr inline dcon::demographics_key middle_militancy(9);
					sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
						return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::middle) ?
							state.world.pop_get_militancy(p) * state.world.pop_get_size(p) : 0.0f;
					});
					break;
				case 10:	//constexpr inline dcon::demographics_key rich_militancy(10);
					sum_over_demographics(state, key, [](sys::state const& state, dcon::pop_id p) {
						return state.world.pop_type_get_strata(state.world.pop_get_poptype(p)) == uint8_t(culture::pop_strata::rich) ?
							state.world.pop_get_militancy(p) * state.world.pop_get_size(p) : 0.0f;
					});
					break;
			}
		} else if(key.index() < to_key(state, dcon::issue_option_id(0)).index()) { // ideology
			dcon::issue_option_id pkey{ dcon::issue_option_id::value_base_t( index - count_special_keys ) };
			auto pdemo_key = pop_demographics::to_key(state, pkey);
			sum_over_demographics(state, key, [pdemo_key](sys::state const& state, dcon::pop_id p) {
				return state.world.pop_get_demographics(p, pdemo_key);
			});
		} else if(key.index() < to_key(state, dcon::pop_type_id(0)).index()) { // issue option
			dcon::ideology_id pkey{ dcon::ideology_id::value_base_t(index - (count_special_keys + state.world.ideology_size()) ) };
			auto pdemo_key = pop_demographics::to_key(state, pkey);
			sum_over_demographics(state, key, [pdemo_key](sys::state const& state, dcon::pop_id p) {
				return state.world.pop_get_demographics(p, pdemo_key);
			});
		} else if(key.index() < to_key(state, dcon::culture_id(0)).index()) { // pop type
			dcon::pop_type_id pkey{ dcon::pop_type_id::value_base_t(index - (count_special_keys + state.world.ideology_size() + state.world.issue_option_size())) };
			sum_over_demographics(state, key, [pkey](sys::state const& state, dcon::pop_id p) {
				return state.world.pop_get_poptype(p) == pkey ? state.world.pop_get_size(p) : 0.0f;
			});
		} else if(key.index() < to_key(state, dcon::religion_id(0)).index()) { // culture
			dcon::culture_id pkey{ dcon::culture_id::value_base_t(index - (count_special_keys + state.world.ideology_size() + state.world.issue_option_size() + state.world.pop_type_size())) };
			sum_over_demographics(state, key, [pkey](sys::state const& state, dcon::pop_id p) {
				return state.world.pop_get_culture(p) == pkey ? state.world.pop_get_size(p) : 0.0f;
			});
		} else { // religion
			dcon::religion_id pkey{ dcon::religion_id::value_base_t(index - (count_special_keys + state.world.ideology_size() + state.world.issue_option_size() + state.world.pop_type_size() + state.world.culture_size())) };
			sum_over_demographics(state, key, [pkey](sys::state const& state, dcon::pop_id p) {
				return state.world.pop_get_religion(p) == pkey ? state.world.pop_get_size(p) : 0.0f;
			});
		}
	});

	//
	// calculate values derived from demographics
	//
	concurrency::parallel_for(uint32_t(0), uint32_t(14), [&](uint32_t index) {
		switch(index) {
			case 0:
			{
				province::for_each_land_province(state, [&](dcon::province_id p) {
					// schombert: there is a faster way to do this. Instead of figuring out the max province by province
					// it would be better to go culture by culture, storing the temporary max share per province in a
					// temporary buffer. Why? Because by going through things one culture at a time would be much easier
					// on the prefetcher given the typical number of cultures
					dcon::culture_id max_id;
					float max_value = 0.0f;
					state.world.for_each_culture([&](dcon::culture_id c) {
						if(auto v = state.world.province_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.province_set_dominant_culture(p, max_id);
				});
				break;
			}
			case 1:
			{
				state.world.for_each_state_instance([&](dcon::state_instance_id p) {
					dcon::culture_id max_id;
					float max_value = 0.0f;
					state.world.for_each_culture([&](dcon::culture_id c) {
						if(auto v = state.world.state_instance_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.state_instance_set_dominant_culture(p, max_id);
				});
				break;
			}
			case 2:
			{
				state.world.for_each_nation([&](dcon::nation_id p) {
					dcon::culture_id max_id;
					float max_value = 0.0f;
					state.world.for_each_culture([&](dcon::culture_id c) {
						if(auto v = state.world.nation_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.nation_set_dominant_culture(p, max_id);
				});
				break;
			}
			case 3:
			{
				province::for_each_land_province(state, [&](dcon::province_id p) {
					dcon::religion_id max_id;
					float max_value = 0.0f;
					state.world.for_each_religion([&](dcon::religion_id c) {
						if(auto v = state.world.province_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.province_set_dominant_religion(p, max_id);
				});
				break;
			}
			case 4:
			{
				state.world.for_each_state_instance([&](dcon::state_instance_id p) {
					dcon::religion_id max_id;
					float max_value = 0.0f;
					state.world.for_each_religion([&](dcon::religion_id c) {
						if(auto v = state.world.state_instance_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.state_instance_set_dominant_religion(p, max_id);
				});
				break;
			}
			case 5:
			{
				state.world.for_each_nation([&](dcon::nation_id p) {
					dcon::religion_id max_id;
					float max_value = 0.0f;
					state.world.for_each_religion([&](dcon::religion_id c) {
						if(auto v = state.world.nation_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.nation_set_dominant_religion(p, max_id);
				});
				break;
			}
			case 6:
			{
				province::for_each_land_province(state, [&](dcon::province_id p) {
					dcon::ideology_id max_id;
					float max_value = 0.0f;
					state.world.for_each_ideology([&](dcon::ideology_id c) {
						if(auto v = state.world.province_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.province_set_dominant_ideology(p, max_id);
				});
				break;
			}
			case 7:
			{
				state.world.for_each_state_instance([&](dcon::state_instance_id p) {
					dcon::ideology_id max_id;
					float max_value = 0.0f;
					state.world.for_each_ideology([&](dcon::ideology_id c) {
						if(auto v = state.world.state_instance_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.state_instance_set_dominant_ideology(p, max_id);
				});
				break;
			}
			case 8:
			{
				state.world.for_each_nation([&](dcon::nation_id p) {
					dcon::ideology_id max_id;
					float max_value = 0.0f;
					state.world.for_each_ideology([&](dcon::ideology_id c) {
						if(auto v = state.world.nation_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.nation_set_dominant_ideology(p, max_id);
				});
				break;
			}
			case 9:
			{
				province::for_each_land_province(state, [&](dcon::province_id p) {
					dcon::issue_option_id max_id;
					float max_value = 0.0f;
					state.world.for_each_issue_option([&](dcon::issue_option_id c) {
						if(auto v = state.world.province_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.province_set_dominant_issue_option(p, max_id);
				});
				break;
			}
			case 10:
			{
				state.world.for_each_state_instance([&](dcon::state_instance_id p) {
					dcon::issue_option_id max_id;
					float max_value = 0.0f;
					state.world.for_each_issue_option([&](dcon::issue_option_id c) {
						if(auto v = state.world.state_instance_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.state_instance_set_dominant_issue_option(p, max_id);
				});
				break;
			}
			case 11:
			{
				state.world.for_each_nation([&](dcon::nation_id p) {
					dcon::issue_option_id max_id;
					float max_value = 0.0f;
					state.world.for_each_issue_option([&](dcon::issue_option_id c) {
						if(auto v = state.world.nation_get_demographics(p, to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.nation_set_dominant_issue_option(p, max_id);
				});
				break;
			}
			case 12:
			{
				state.world.for_each_pop([&](dcon::pop_id p) {
					dcon::issue_option_id max_id;
					float max_value = 0.0f;
					state.world.for_each_issue_option([&](dcon::issue_option_id c) {
						if(auto v = state.world.pop_get_demographics(p, pop_demographics::to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.pop_set_dominant_issue_option(p, max_id);
				});
				break;
			}
			case 13:
			{
				state.world.for_each_pop([&](dcon::pop_id p) {
					dcon::ideology_id max_id;
					float max_value = 0.0f;
					state.world.for_each_ideology([&](dcon::ideology_id c) {
						if(auto v = state.world.pop_get_demographics(p, pop_demographics::to_key(state, c)); v > max_value) {
							max_value = v;
							max_id = c;
						}
					});
					state.world.pop_set_dominant_ideology(p, max_id);
				});
				break;
			}
			default:
				break;
		}
	});
}


}
