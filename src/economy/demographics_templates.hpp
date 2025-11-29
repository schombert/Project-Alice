#pragma once

#include "system_state.hpp"
#include "demographics.hpp"

namespace pop_demographics {
template<typename P, typename V>
void set_life_needs(sys::state& state, P p, V v) {
	state.world.pop_set_ulife_needs_satisfaction(p, to_pu8(v));
}
template<typename P, typename V>
void set_everyday_needs(sys::state& state, P p, V v) {
	state.world.pop_set_ueveryday_needs_satisfaction(p, to_pu8(v));
}
template<typename P, typename V>
void set_luxury_needs(sys::state& state, P p, V v) {
	state.world.pop_set_uluxury_needs_satisfaction(p, to_pu8(v));
}

template<typename T>
auto get_employment(sys::state const& state, T p) {
	auto ival = state.world.pop_get_uemployment(p);
	return from_pu8(ival) * state.world.pop_get_size(p);
}
template<typename T>
auto get_raw_employment(sys::state const& state, T p) {
	auto ival = state.world.pop_get_uemployment(p);
	return from_pu8(ival);
}
template<typename T>
auto get_social_reform_desire(sys::state const& state, T p) {
	auto ival = state.world.pop_get_usocial_reform_desire(p);
	return from_pu8(ival);
}
template<typename T>
auto get_political_reform_desire(sys::state const& state, T p) {
	auto ival = state.world.pop_get_upolitical_reform_desire(p);
	return from_pu8(ival);
}
template<typename T>
auto get_militancy(sys::state const& state, T p) {
	auto ival = state.world.pop_get_umilitancy(p);
	return from_pmc(ival);
}
template<typename T>
auto get_consciousness(sys::state const& state, T p) {
	auto ival = state.world.pop_get_uconsciousness(p);
	return from_pmc(ival);
}
template<typename T>
auto get_literacy(sys::state const& state, T p) {
	auto ival = state.world.pop_get_uliteracy(p);
	return from_pu16(ival);
}
template<typename P, typename V>
void set_literacy(sys::state& state, P p, V v) {
	state.world.pop_set_uliteracy(p, to_pu16(v));
}
template<typename T>
auto get_life_needs(sys::state const& state, T p) {
	auto ival = state.world.pop_get_ulife_needs_satisfaction(p);
	return from_pu8(ival);
}
template<typename T>
auto get_everyday_needs(sys::state const& state, T p) {
	auto ival = state.world.pop_get_ueveryday_needs_satisfaction(p);
	return from_pu8(ival);
}
template<typename T>
auto get_luxury_needs(sys::state const& state, T p) {
	auto ival = state.world.pop_get_uluxury_needs_satisfaction(p);
	return from_pu8(ival);
}
template<typename T>
auto get_demo(sys::state const& state, T p, dcon::pop_demographics_key k) {
	auto ival = state.world.pop_get_udemographics(p, k);
	return from_pu8(ival);
}

}

namespace demographics {

template<promotion_type PROMOTION_TYPE>
float get_single_promotion_demotion_target_weight(sys::state& state, dcon::pop_id p, dcon::pop_type_id target_type) {

	auto ptype = state.world.pop_get_poptype(p);
	auto strata = state.world.pop_type_get_strata(ptype);
	auto loc = state.world.pop_get_province_from_pop_location(p);
	auto si = state.world.province_get_state_membership(loc);
	auto nf = state.world.state_instance_get_owner_focus(si);
	auto promoted_type = state.world.national_focus_get_promotion_type(nf);
	auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);

	if(target_type == ptype) {
		return 0.0f; //don't promote to the same type
	} else if((PROMOTION_TYPE == promotion_type::promotion && state.world.pop_type_get_strata(target_type) >= strata) //if the selected type is higher strata
		|| (PROMOTION_TYPE == promotion_type::demotion && state.world.pop_type_get_strata(target_type) <= strata)) {   //if the selected type is lower strata

		if(auto mfn = state.world.pop_type_get_promotion_fns(ptype, target_type); mfn != 0) {
			using ftype = float(*)(int32_t);
			ftype fn = (ftype)mfn;
			float llvm_result = fn(p.index());
#ifdef CHECK_LLVM_RESULTS
			float interp_result = 0.0f;
			if(auto mtrigger = state.world.pop_type_get_promotion(ptype, target_type); mtrigger) {
				interp_result = trigger::evaluate_additive_modifier(state, mtrigger, trigger::to_generic(p), trigger::to_generic(p), 0);
			}
			assert(llvm_result == interp_result);
#endif
			auto chance = std::max(0.0f, llvm_result + (target_type == promoted_type ? promotion_bonus : 0.0f));
			return chance;
		} else {
			if(auto mtrigger = state.world.pop_type_get_promotion(ptype, target_type); mtrigger) {
				auto chance = std::max(0.0f, trigger::evaluate_additive_modifier(state, mtrigger, trigger::to_generic(p), trigger::to_generic(p), 0) + (target_type == promoted_type ? promotion_bonus : 0.0f));
				return chance;
			}
		}

	}
	return 0.0f;

}

template<promotion_type PROMOTION_TYPE>
promotion_demotion_weights get_pop_promotion_demotion_weights(sys::state& state, dcon::pop_id p) {

	promotion_demotion_weights weights(state.world.pop_type_size());

	state.world.for_each_pop_type([&](dcon::pop_type_id target_type) {

		float weight = get_single_promotion_demotion_target_weight<PROMOTION_TYPE>(state, p, target_type);

		weights.pop_weights[target_type] = weight;
		weights.total_weights += weight;

	});
	return weights;
}

template<promotion_type PROMOTION_TYPE>
pop_promotion_demotion_data get_promotion_demotion_data(sys::state& state, dcon::pop_id p, float promotion_or_demotion_chance) {
	auto loc = state.world.pop_get_province_from_pop_location(p);
	auto si = state.world.province_get_state_membership(loc);
	auto nf = state.world.state_instance_get_owner_focus(si);
	auto promoted_type = state.world.national_focus_get_promotion_type(nf);
	auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);
	auto ptype = state.world.pop_get_poptype(p);
	auto strata = state.world.pop_type_get_strata(ptype);
	float current_size = state.world.pop_get_size(p);

	pop_promotion_demotion_data output_data{};


	float base_amount = std::ceil(promotion_or_demotion_chance * state.defines.promotion_scale * current_size);

	if constexpr(PROMOTION_TYPE == promotion_type::demotion) {
		if(ptype == state.culture_definitions.artisans) {
			base_amount *= 10.f;
		}
	}

	if(base_amount >= 0.001f) {
		auto transfer_amount = std::min(current_size, base_amount);
		output_data.amount = transfer_amount;
	}


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


	if((PROMOTION_TYPE == promotion_type::promotion && promoted_type && state.world.pop_type_get_strata(promoted_type) >= strata)
		|| (PROMOTION_TYPE == promotion_type::demotion && promoted_type && state.world.pop_type_get_strata(promoted_type) <= strata)) {

		float chance = 0.0f;
		if(auto mfn = state.world.pop_type_get_promotion_fns(ptype, promoted_type); mfn != 0) {
			using ftype = float(*)(int32_t);
			ftype fn = (ftype)mfn;
			float llvm_result = fn(p.index());
#ifdef CHECK_LLVM_RESULTS
			float interp_result = 0.0f;
			if(auto mtrigger = state.world.pop_type_get_promotion(ptype, promoted_type); mtrigger) {
				interp_result = trigger::evaluate_additive_modifier(state, mtrigger, trigger::to_generic(p), trigger::to_generic(p), 0);
			}
			assert(llvm_result == interp_result);
#endif
			chance = llvm_result + promotion_bonus;
		} else {
			if(auto mtrigger = state.world.pop_type_get_promotion(ptype, promoted_type); mtrigger) {
				chance = trigger::evaluate_additive_modifier(state, mtrigger, trigger::to_generic(p), trigger::to_generic(p), 0) + promotion_bonus;
			}
		}

		if(chance > 0) {
			output_data.target = promoted_type;
			return output_data; // early exit
		}

	}
	promotion_demotion_weights weights = get_pop_promotion_demotion_weights<PROMOTION_TYPE>(state, p);

	if(weights.total_weights > 0.0f) {
		auto rvalue = float(rng::get_random(state, uint32_t(p.index())) & 0xFFFF) / float(0xFFFF + 1);
		for(uint32_t i = state.world.pop_type_size(); i-- > 0;) {
			dcon::pop_type_id pr{ dcon::pop_type_id::value_base_t(i) };
			rvalue -= weights.pop_weights[pr] / weights.total_weights;
			if(rvalue < 0.0f) {
				output_data.target = pr;
				return output_data;
			}
		}
	}
	output_data.amount = 0.0f;
	output_data.target = dcon::pop_type_id{ };
	return output_data;

}

}
