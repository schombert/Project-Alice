#pragma once
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "triggers.hpp"
#include "prng.hpp"

namespace pop_demographics {

constexpr inline uint32_t count_special_keys = 0;

dcon::pop_demographics_key to_key(sys::state const& state, dcon::ideology_id v);
dcon::pop_demographics_key to_key(sys::state const& state, dcon::issue_option_id v);
uint32_t size(sys::state const& state);

constexpr inline float pop_mc_scaling = 10.0f / float(std::numeric_limits<uint16_t>::max());
constexpr inline float pop_u16_scaling = 1.0f / float(std::numeric_limits<uint16_t>::max());
constexpr inline float pop_u8_scaling = 1.0f / float(std::numeric_limits<uint8_t>::max());
constexpr inline float inv_pop_mc_scaling = float(std::numeric_limits<uint16_t>::max()) / 10.0f;
constexpr inline float inv_pop_u16_scaling = float(std::numeric_limits<uint16_t>::max());
constexpr inline float inv_pop_u8_scaling = float(std::numeric_limits<uint8_t>::max());

inline uint8_t to_pu8(float v) {
	return uint8_t(v * inv_pop_u8_scaling + 0.49f);
}
inline uint16_t to_pu16(float v) {
	return uint16_t(v * inv_pop_u16_scaling + 0.49f);
}
inline uint16_t to_pmc(float v) {
	return uint16_t(v * inv_pop_mc_scaling + 0.49f);
}
inline ve::int_vector to_pu8(ve::fp_vector v) {
	return ve::to_int(v * inv_pop_u8_scaling + 0.49f);
}
inline ve::int_vector to_pu16(ve::fp_vector v) {
	return ve::to_int(v * inv_pop_u16_scaling + 0.49f);
}
inline ve::int_vector to_pmc(ve::fp_vector v) {
	return ve::to_int(v * inv_pop_mc_scaling + 0.49f);
}

inline float from_pu8(uint8_t v) {
	return float(v) * pop_u8_scaling;
}
inline float from_pu16(uint16_t v) {
	return float(v) * pop_u16_scaling;
}
inline float from_pmc(uint16_t v) {
	return float(v) * pop_mc_scaling;
}

inline ve::fp_vector from_pu8(ve::int_vector v) {
	return ve::to_float(v) * pop_u8_scaling;
}
inline ve::fp_vector from_pu16(ve::int_vector v) {
	return ve::to_float(v) * pop_u16_scaling;
}
inline ve::fp_vector from_pmc(ve::int_vector v) {
	return ve::to_float(v) * pop_mc_scaling;
}


void regenerate_is_primary_or_accepted(sys::state& state);
float get_demo(sys::state const& state, dcon::pop_id p, dcon::pop_demographics_key k);
void set_demo(sys::state& state, dcon::pop_id p, dcon::pop_demographics_key k, float v);
float get_militancy(sys::state const& state, dcon::pop_id p);
void set_militancy(sys::state& state, dcon::pop_id p, float v);
float get_consciousness(sys::state const& state, dcon::pop_id p);
void set_consciousness(sys::state& state, dcon::pop_id p, float v);
float get_literacy(sys::state const& state, dcon::pop_id p);
template<typename P, typename V>
void set_literacy(sys::state& state, P p, V v);
float get_employment(sys::state const& state, dcon::pop_id p);
float get_raw_employment(sys::state const& state, dcon::pop_id p);
void set_employment(sys::state& state, dcon::pop_id p, float v);
void set_raw_employment(sys::state& state, dcon::pop_id p, float v);
float get_life_needs(sys::state const& state, dcon::pop_id p);
float get_everyday_needs(sys::state const& state, dcon::pop_id p);
float get_luxury_needs(sys::state const& state, dcon::pop_id p);
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
float get_social_reform_desire(sys::state const& state, dcon::pop_id p);
void set_social_reform_desire(sys::state& state, dcon::pop_id p, float v);
float get_political_reform_desire(sys::state const& state, dcon::pop_id p);
void set_political_reform_desire(sys::state& state, dcon::pop_id p, float v);

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

} // namespace pop_demographics
namespace demographics {

constexpr inline dcon::demographics_key total(0);
constexpr inline dcon::demographics_key employable(1);
constexpr inline dcon::demographics_key employed(2);
constexpr inline dcon::demographics_key consciousness(3);
constexpr inline dcon::demographics_key militancy(4);
constexpr inline dcon::demographics_key literacy(5);
constexpr inline dcon::demographics_key political_reform_desire(6);
constexpr inline dcon::demographics_key social_reform_desire(7);
constexpr inline dcon::demographics_key poor_militancy(8);
constexpr inline dcon::demographics_key middle_militancy(9);
constexpr inline dcon::demographics_key rich_militancy(10);
constexpr inline dcon::demographics_key poor_life_needs(11);
constexpr inline dcon::demographics_key middle_life_needs(12);
constexpr inline dcon::demographics_key rich_life_needs(13);
constexpr inline dcon::demographics_key poor_everyday_needs(14);
constexpr inline dcon::demographics_key middle_everyday_needs(15);
constexpr inline dcon::demographics_key rich_everyday_needs(16);
constexpr inline dcon::demographics_key poor_luxury_needs(17);
constexpr inline dcon::demographics_key middle_luxury_needs(18);
constexpr inline dcon::demographics_key rich_luxury_needs(19);
constexpr inline dcon::demographics_key poor_total(20);
constexpr inline dcon::demographics_key middle_total(21);
constexpr inline dcon::demographics_key rich_total(22);
constexpr inline dcon::demographics_key non_colonial_literacy(23);
constexpr inline dcon::demographics_key non_colonial_total(24);
constexpr inline dcon::demographics_key primary_or_accepted(25);
constexpr inline uint32_t count_special_keys = 26;

dcon::demographics_key to_key(sys::state const& state, dcon::ideology_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::issue_option_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::pop_type_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::culture_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::religion_id v);
dcon::demographics_key to_employment_key(sys::state const& state, dcon::pop_type_id v);

uint32_t size(sys::state const& state);

void regenerate_jingoism_support(sys::state& state, dcon::nation_id n);
void regenerate_from_pop_data_full(sys::state& state);
void alt_regenerate_from_pop_data_full(sys::state& state);
void regenerate_from_pop_data_daily(sys::state& state);
void alt_regenerate_from_pop_data_daily(sys::state& state);

void alt_demographics_update_extras(sys::state& state);

struct ideology_buffer {
	tagged_vector<ve::vectorizable_buffer<uint8_t, dcon::pop_id>, dcon::ideology_id> temp_buffers;
	uint32_t size = 0;

	ideology_buffer(sys::state& state) : size(0) {
		for(uint32_t i = 0; i < state.world.ideology_size(); ++i) {
			temp_buffers.emplace_back(uint32_t(0));
		}
	}

	void update(sys::state& state, uint32_t s) {
		if(size < s) {
			size = s;
			state.world.for_each_ideology(
					[&](dcon::ideology_id i) { temp_buffers[i] = ve::vectorizable_buffer<uint8_t, dcon::pop_id>(s);  /*state.world.pop_make_vectorizable_float_buffer();*/ });
		}
	}
};

struct issues_buffer {
	tagged_vector<ve::vectorizable_buffer<uint8_t, dcon::pop_id>, dcon::issue_option_id> temp_buffers;
	uint32_t size = 0;

	issues_buffer(sys::state& state) : size(0) {
		for(uint32_t i = 0; i < state.world.issue_option_size(); ++i) {
			temp_buffers.emplace_back(uint32_t(0));
		}
	}

	void update(sys::state& state, uint32_t s) {
		if(size < s) {
			size = s;
			state.world.for_each_issue_option(
					[&](dcon::issue_option_id i) { temp_buffers[i] = ve::vectorizable_buffer<uint8_t, dcon::pop_id>(s); /*state.world.pop_make_vectorizable_float_buffer();*/ });
		}
	}
};



struct pop_promotion_demotion_data {
	float amount = 0.0f;
	dcon::pop_type_id target = dcon::pop_type_id{};
};
enum class promotion_type : bool {
	demotion = 0,
	promotion = 1
};

struct promotion_demotion_weights {
	tagged_vector<float, dcon::pop_type_id> pop_weights;
	float total_weights;

	promotion_demotion_weights(size_t size) {
		pop_weights = tagged_vector<float, dcon::pop_type_id>(size);
		total_weights = 0.0f;
	}
};
template<promotion_type PROMOTION_TYPE>
float get_single_promotion_demotion_target_weight(sys::state& state, dcon::pop_id p, dcon::pop_type_id target_type) {

	auto ptype = state.world.pop_get_poptype(p);
	auto strata = state.world.pop_type_get_strata(ptype);
	auto loc = state.world.pop_get_province_from_pop_location(p);
	auto si = state.world.province_get_state_membership(loc);
	auto nf = state.world.state_instance_get_owner_focus(si);
	auto promoted_type = state.world.national_focus_get_promotion_type(nf);
	auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);

	bool is_state_capital = state.world.state_instance_get_capital(state.world.province_get_state_membership(loc)) == loc;

	if (target_type == ptype) {
		return 0.0f; //don't promote to the same type
	}
	else if (!is_state_capital && state.world.pop_type_get_state_capital_only(target_type)) {
		return 0.0f; //don't promote if the pop is not in the state capital
	}
	else if ((PROMOTION_TYPE == promotion_type::promotion && state.world.pop_type_get_strata(target_type) >= strata) //if the selected type is higher strata
		|| (PROMOTION_TYPE == promotion_type::demotion && state.world.pop_type_get_strata(target_type) <= strata)) {   //if the selected type is lower strata

		if (auto mfn = state.world.pop_type_get_promotion_fns(ptype, target_type); mfn != 0) {
			using ftype = float(*)(int32_t);
			ftype fn = (ftype)mfn;
			float llvm_result = fn(p.index());
#ifdef CHECK_LLVM_RESULTS
			float interp_result = 0.0f;
			if (auto mtrigger = state.world.pop_type_get_promotion(ptype, target_type); mtrigger) {
				interp_result = trigger::evaluate_additive_modifier(state, mtrigger, trigger::to_generic(p), trigger::to_generic(p), 0);
			}
			assert(llvm_result == interp_result);
#endif
			auto chance = std::max(0.0f, llvm_result + (target_type == promoted_type ? promotion_bonus : 0.0f));
			return chance;
		}
		else {
			if (auto mtrigger = state.world.pop_type_get_promotion(ptype, target_type); mtrigger) {
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

	bool is_state_capital = state.world.state_instance_get_capital(state.world.province_get_state_membership(loc)) == loc;

	if ((PROMOTION_TYPE == promotion_type::promotion && promoted_type && state.world.pop_type_get_strata(promoted_type) >= strata && (is_state_capital || state.world.pop_type_get_state_capital_only(promoted_type) == false))
		|| (PROMOTION_TYPE == promotion_type::demotion && promoted_type && state.world.pop_type_get_strata(promoted_type) <= strata && (is_state_capital || state.world.pop_type_get_state_capital_only(promoted_type) == false))) {

		float chance = 0.0f;
		if (auto mfn = state.world.pop_type_get_promotion_fns(ptype, promoted_type); mfn != 0) {
			using ftype = float(*)(int32_t);
			ftype fn = (ftype)mfn;
			float llvm_result = fn(p.index());
#ifdef CHECK_LLVM_RESULTS
			float interp_result = 0.0f;
			if (auto mtrigger = state.world.pop_type_get_promotion(ptype, promoted_type); mtrigger) {
				interp_result = trigger::evaluate_additive_modifier(state, mtrigger, trigger::to_generic(p), trigger::to_generic(p), 0);
			}
			assert(llvm_result == interp_result);
#endif
			chance = llvm_result + promotion_bonus;
		}
		else {
			if (auto mtrigger = state.world.pop_type_get_promotion(ptype, promoted_type); mtrigger) {
				chance = trigger::evaluate_additive_modifier(state, mtrigger, trigger::to_generic(p), trigger::to_generic(p), 0) + promotion_bonus;
			}
		}

		if (chance > 0) {
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



struct promotion_buffer {
	ve::vectorizable_buffer<float, dcon::pop_id> amounts;
	ve::vectorizable_buffer<dcon::pop_type_id, dcon::pop_id> types;
	uint32_t size = 0;
	uint32_t reserved = 0;

	promotion_buffer() : amounts(0), types(0), size(0) { }
	void update(uint32_t s) {
		size = s;
		if(reserved < s) {
			reserved = s;
			amounts = ve::vectorizable_buffer<float, dcon::pop_id>(s);
			types = ve::vectorizable_buffer<dcon::pop_type_id, dcon::pop_id>(s);
		}
	}
};

struct assimilation_buffer {
	ve::vectorizable_buffer<float, dcon::pop_id> amounts;
	uint32_t size = 0;
	uint32_t reserved = 0;

	assimilation_buffer() : amounts(0), size(0) { }
	void update(uint32_t s) {
		size = s;
		if(reserved < s) {
			reserved = s;
			amounts = ve::vectorizable_buffer<float, dcon::pop_id>(s);
		}
	}
};

struct migration_buffer {
	ve::vectorizable_buffer<float, dcon::pop_id> amounts;
	ve::vectorizable_buffer<dcon::province_id, dcon::pop_id> destinations;
	uint32_t size = 0;
	uint32_t reserved = 0;

	migration_buffer() : amounts(0), destinations(0), size(0) { }
	void update(uint32_t s) {
		size = s;
		if(reserved < s) {
			reserved = s;
			amounts = ve::vectorizable_buffer<float, dcon::pop_id>(s);
			destinations = ve::vectorizable_buffer<dcon::province_id, dcon::pop_id>(s);
		}
	}
};

void update_consciousness(sys::state& state, uint32_t offset, uint32_t divisions);
void update_militancy(sys::state& state, uint32_t offset, uint32_t divisions);
void update_ideologies(sys::state& state, uint32_t offset, uint32_t divisions, ideology_buffer& ibuf);
void update_issues(sys::state& state, uint32_t offset, uint32_t divisions, issues_buffer& ibuf);
void update_growth(sys::state& state, uint32_t offset, uint32_t divisions);
void update_type_changes(sys::state& state, uint32_t offset, uint32_t divisions, promotion_buffer& promotion_buf, promotion_buffer& demotion_buf);
void update_assimilation(sys::state& state, uint32_t offset, uint32_t divisions, assimilation_buffer& pbuf);
void update_internal_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);
void update_colonial_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);
void update_immigration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);

float get_estimated_literacy_change(sys::state& state, dcon::nation_id n);
float get_estimated_mil_change(sys::state& state, dcon::nation_id n);
float get_estimated_con_change(sys::state& state, dcon::nation_id n);

// bureacracy has to travel around the realm
inline constexpr float administration_base_push = 0.9f;
inline constexpr float administration_additional_province_weight = 0.1f;

struct province_migration_weight_explanation {
	float base_multiplier;
	float base_weight;
	float modifier;
	float wage_multiplier;
	float result;
};
province_migration_weight_explanation explain_province_internal_migration_weight(sys::state& state, dcon::pop_id p, dcon::province_id pid);

void apply_ideologies(sys::state& state, uint32_t offset, uint32_t divisions, ideology_buffer& pbuf);
void apply_issues(sys::state& state, uint32_t offset, uint32_t divisions, issues_buffer& pbuf);
void apply_type_changes(sys::state& state, uint32_t offset, uint32_t divisions, promotion_buffer& promotion_buf, promotion_buffer& demotion_buf);
void apply_assimilation(sys::state& state, uint32_t offset, uint32_t divisions, assimilation_buffer& pbuf);
void apply_internal_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);
void apply_colonial_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);
void apply_immigration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);

void remove_size_zero_pops(sys::state& state);
void remove_small_pops(sys::state& state);

float get_pop_starvation_penalty_scale(sys::state& state, dcon::pop_id pop, float growth_modifiers);
float get_pop_growth_modifiers(sys::state& state, dcon::pop_id pop);
float get_pop_starvation_factor(sys::state& state, dcon::pop_id ids);
float popgrowth_from_life_rating(sys::state& state, float life_rating);
float get_monthly_pop_growth_factor(sys::state& state, dcon::pop_id ids);
float get_monthly_pop_increase(sys::state& state, dcon::pop_id);
int64_t get_monthly_pop_increase(sys::state& state, dcon::nation_id n);
int64_t get_monthly_pop_increase(sys::state& state, dcon::state_instance_id n);
int64_t get_monthly_pop_increase(sys::state& state, dcon::province_id n);

float get_estimated_literacy_change(sys::state& state, dcon::pop_id n);
float get_estimated_mil_change(sys::state& state, dcon::pop_id n);
float get_estimated_con_change(sys::state& state, dcon::pop_id n);
float get_estimated_type_change(sys::state& state, dcon::pop_id n);
float get_estimated_promotion(sys::state& state, dcon::pop_id n);
float get_estimated_demotion(sys::state& state, dcon::pop_id n);
float get_estimated_assimilation(sys::state& state, dcon::pop_id n);
float get_estimated_internal_migration(sys::state& state, dcon::pop_id n);
float get_estimated_colonial_migration(sys::state& state, dcon::pop_id n);
float get_estimated_emigration(sys::state& state, dcon::pop_id n);
void estimate_directed_immigration(sys::state& state, dcon::nation_id n, std::vector<float>& national_amounts);

float calculate_nation_sol(sys::state& state, dcon::nation_id nation_id);
void reduce_pop_size_safe(sys::state& state, dcon::pop_id pop_id, int32_t amount);

void modify_militancy(sys::state& state, dcon::nation_id n, float v);

} // namespace demographics
