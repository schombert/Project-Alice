#include "economy_pops.hpp"
#include "economy_production.hpp"
#include "price.hpp"
#include "province_templates.hpp"
#include "economy_templates.hpp"
#include "demographics.hpp"
#include "money.hpp"
#include "economy_constants.hpp"
#include "economy_templates_pure.hpp"
#include "economy_pops_constants.hpp"
#include "policy_execution.hpp"
#include "advanced_province_buildings.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace economy {
namespace pops {

namespace {
	uint8_t debug_bitfield_byte(auto const& getter, int32_t byte_index) {
		uint8_t value = 0;
		for(int32_t bit = 0; bit < 8; ++bit) {
			if(getter(byte_index * 8 + bit)) {
				value = uint8_t(value | (uint8_t(1) << bit));
			}
		}
		return value;
	}

	template<typename Tag>
	uint32_t wage_active_lanes(ve::partial_contiguous_tags<Tag> const& tags) {
		return tags.subcount;
	}

	template<typename T>
	uint32_t wage_active_lanes(T const&) {
		return ve::vector_size;
	}

	inline bool wage_debug_enabled() {
		static bool enabled = [] {
			auto const* value = std::getenv("ALICE_DEBUG_WAGE_ASSERT");
			return value && value[0] == '1';
		}();
		return enabled;
	}

	inline bool wage_trace_producers_enabled() {
		static bool enabled = [] {
			auto const* value = std::getenv("ALICE_DEBUG_WAGE_PRODUCERS");
			return value && value[0] == '1';
		}();
		return enabled;
	}

	inline bool wage_compare_enabled() {
		static bool enabled = [] {
			auto const* value = std::getenv("ALICE_DEBUG_WAGE_COMPARE");
			return value && value[0] == '1';
		}();
		return enabled;
	}

	inline bool wage_value_matches(float lhs, float rhs, float tolerance) {
		if(std::isnan(lhs) || std::isnan(rhs)) {
			return std::isnan(lhs) && std::isnan(rhs);
		}
		if(std::isinf(lhs) || std::isinf(rhs)) {
			return std::isinf(lhs) && std::isinf(rhs) && std::signbit(lhs) == std::signbit(rhs);
		}
		return std::fabs(lhs - rhs) <= tolerance;
	}

	inline void wage_abort_after_log() {
		std::fflush(stderr);
		std::abort();
	}
}
#ifndef NDEBUG
void debug_check_pop_savings_phase(sys::state const& state, char const* phase_name) {
	if(!wage_debug_enabled() && !wage_trace_producers_enabled()) {
		return;
	}
	double total_savings = 0.0;
	float maximum_savings = 0.0f;
	dcon::pop_id maximum_pop{};
	state.world.execute_serial_over_pop([&](auto pops) {
		ve::apply([&](dcon::pop_id pop) {
			auto savings = state.world.pop_get_savings(pop);
			if(std::isfinite(savings) && savings >= 0.f) {
				total_savings += double(savings);
				if(savings > maximum_savings) {
					maximum_savings = savings;
					maximum_pop = pop;
				}
				return;
			}
			auto date = state.current_date.to_ymd(state.start_date);
			auto province = state.world.pop_get_province_from_pop_location(pop);
			auto pop_type = state.world.pop_get_poptype(pop);
			std::fprintf(stderr,
				"SAVINGS_PHASE_INVALID\n"
				"phase=%s\n"
				"date=%d.%d.%d\n"
				"pop_id=%d\n"
				"province_id=%d\n"
				"pop_type_id=%d\n"
				"savings=%g\n"
				"\n",
				phase_name,
				date.year, int(date.month), int(date.day),
				pop.index(),
				province.index(),
				pop_type.id.index(),
				savings);
			wage_abort_after_log();
		}, pops);
	});
	if(wage_trace_producers_enabled()) {
		auto const date = state.current_date.to_ymd(state.start_date);
		std::fprintf(stderr,
			"SAVINGS_PHASE_TOTAL phase=%s date=%d.%d.%d total=%.17g max=%.9g max_pop=%d\n",
			phase_name, date.year, int(date.month), int(date.day), total_savings,
			maximum_savings, maximum_pop.index());
		std::fflush(stderr);
	}
}
#endif


template<typename VALUE, typename POPS>
VALUE investment_rate(const sys::state& state, POPS ids) {
	using BOOL_VALUE = typename std::conditional_t<std::same_as<POPS, dcon::pop_id>, bool, ve::mask_vector>;

	auto provs = state.world.pop_get_province_from_pop_location(ids);
	auto states = state.world.province_get_state_membership(provs);
	auto markets = state.world.state_instance_get_market_from_local_market(states);
	auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
	auto pop_type = state.world.pop_get_poptype(ids);
	auto nation_rules = state.world.nation_get_combined_issue_rules(nations);
	auto allows_investment_mask = (nation_rules & can_invest) != 0;
	auto nation_allows_investment = allows_investment_mask;

	auto capitalists_mask = pop_type == state.culture_definitions.capitalists;
	auto middle_class_investors_mask = pop_type == state.culture_definitions.artisans || pop_type == state.culture_definitions.secondary_factory_worker;
	auto farmers_mask = pop_type == state.culture_definitions.farmers;
	auto landowners_mask = pop_type == state.culture_definitions.aristocrat;

	auto invest_ratio_capitalists = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::capitalist_reinvestment);
	auto invest_ratio_landowners = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::aristocrat_reinvestment);
	auto invest_ratio_middle_class = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::middle_class_reinvestment);
	auto invest_ratio_farmers = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::farmers_reinvestment);

	auto investment_ratio =
		adaptive_ve::select<BOOL_VALUE, VALUE>(
			nation_allows_investment && capitalists_mask,
			invest_ratio_capitalists + state.defines.alice_invest_capitalist,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			nation_allows_investment && landowners_mask,
			invest_ratio_landowners + state.defines.alice_invest_aristocrat,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			nation_allows_investment && middle_class_investors_mask,
			invest_ratio_middle_class + state.defines.alice_invest_middle_class,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			nation_allows_investment && farmers_mask,
			invest_ratio_farmers + state.defines.alice_invest_farmer,
			0.0f
		);
	return investment_ratio;
}


// handle bank savings
// Note that farmers and middle_class don't do bank savings by default
// - that doens't mean they don't have savings.
// They don't use banks for savings without modifier (from tech, from example).
template<typename VALUE, typename POPS>
VALUE bank_saving_rate(const sys::state& state, POPS ids) {
	using BOOL_VALUE = typename std::conditional_t<std::same_as<POPS, dcon::pop_id>, bool, ve::mask_vector>;

	auto provs = state.world.pop_get_province_from_pop_location(ids);
	auto states = state.world.province_get_state_membership(provs);
	auto markets = state.world.state_instance_get_market_from_local_market(states);
	auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
	auto pop_type = state.world.pop_get_poptype(ids);

	auto capitalists_mask = pop_type == state.culture_definitions.capitalists;
	auto middle_class_investors_mask = pop_type == state.culture_definitions.artisans || pop_type == state.culture_definitions.secondary_factory_worker;
	auto farmers_mask = pop_type == state.culture_definitions.farmers;
	auto landowners_mask = pop_type == state.culture_definitions.aristocrat;

	auto bank_saving_ratio_capitalists = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::capitalist_savings);
	auto bank_saving_ratio_landowners = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::aristocrat_savings);
	auto bank_saving_ratio_middle_class = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::middle_class_savings);
	auto bank_saving_ratio_farmers = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::farmers_savings);

	auto bank_saving_ratio =
		adaptive_ve::select<BOOL_VALUE, VALUE>(
			capitalists_mask,
			bank_saving_ratio_capitalists + state.defines.alice_save_capitalist,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			landowners_mask,
			bank_saving_ratio_landowners + state.defines.alice_save_aristocrat,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			middle_class_investors_mask,
			bank_saving_ratio_middle_class + state.defines.alice_save_middle_class,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			farmers_mask,
			bank_saving_ratio_farmers + state.defines.alice_save_farmer,
			0.0f
		);

	return bank_saving_ratio;
}

template<typename VALUE, typename POPS>
VALUE adjusted_subsistence_score(
	const sys::state& state,
	POPS p
) {
	return state.world.province_get_subsistence_score(p)
		* state.world.province_get_subsistence_employment(p)
		/ (state.world.province_get_demographics(p, demographics::total) + 1.f);
}

template<typename POPS>
auto prepare_pop_budget_templated(
	const sys::state& state, POPS ids
) {
	using VALUE = typename std::conditional_t<std::same_as<POPS, dcon::pop_id>, float, ve::fp_vector>;
	using BOOL_VALUE = typename std::conditional_t<std::same_as<POPS, dcon::pop_id>, bool, ve::mask_vector>;

	vectorized_pops_budget<VALUE> result{ };

	auto pop_size = state.world.pop_get_size(ids);
	auto savings = state.world.pop_get_savings(ids);
	auto provs = state.world.pop_get_province_from_pop_location(ids);
	auto states = state.world.province_get_state_membership(provs);
	auto markets = state.world.state_instance_get_market_from_local_market(states);
	auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
	auto pop_type = state.world.pop_get_poptype(ids);
	auto strata = state.world.pop_type_get_strata(pop_type);

	VALUE life_costs = ve::apply(
		[&](dcon::market_id m, dcon::pop_type_id pt) {
			return state.world.market_get_life_needs_costs(m, pt);
		}, markets, pop_type
	);
	VALUE everyday_costs = ve::apply(
		[&](dcon::market_id m, dcon::pop_type_id pt) {
			return state.world.market_get_everyday_needs_costs(m, pt);
		}, markets, pop_type
	);
	VALUE luxury_costs = ve::apply(
		[&](dcon::market_id m, dcon::pop_type_id pt) {
			return state.world.market_get_luxury_needs_costs(m, pt);
		}, markets, pop_type
	);

	if constexpr(std::same_as<POPS, dcon::pop_id>) {
		result.can_use_free_services = state.world.pop_get_is_primary_or_accepted_culture(ids) ? 1.f : 0.f;
	} else {
		result.can_use_free_services = adaptive_ve::select<BOOL_VALUE, VALUE>(state.world.pop_get_is_primary_or_accepted_culture(ids), ve::fp_vector{ 1.f }, ve::fp_vector{ 0.f });
	}

	// we want to focus on life needs first if we are poor AND our satisfaction is low

	VALUE total_cost_needs = 0.00001f + (life_costs + everyday_costs + luxury_costs) * pop_size / state.defines.alice_needs_scaling_factor;
	VALUE is_rich = adaptive_ve::min<VALUE>(
		3.f,
		adaptive_ve::max<VALUE>(0.f, savings - total_cost_needs) / total_cost_needs
	);

	VALUE base_life_costs = (0.00001f + life_costs * pop_size / state.defines.alice_needs_scaling_factor);
	VALUE is_poor = adaptive_ve::max<VALUE>(0.01f, 1.f - 4.f * savings / base_life_costs);
	//VALUE current_life = pop_demographics::get_life_needs(state, ids);
	is_poor = adaptive_ve::min<VALUE>(1.f, adaptive_ve::max<VALUE>(0.f, is_poor));

	// prepare desired spending rate for every category

	VALUE life_spending_ratio = state.defines.alice_needs_lf_spend * (1.f - is_poor) + is_poor;
	VALUE housing_spending_ratio = 0.3f;
	VALUE everyday_spending_ratio = state.defines.alice_needs_ev_spend * (1.f - is_poor);
	VALUE luxury_spending_ratio = state.defines.alice_needs_lx_spend * (1.f - is_poor);
	VALUE education_spending_ratio = (0.2f) * (1.f - is_poor);
	VALUE investment_ratio = adaptive_ve::max<VALUE>(investment_rate<VALUE>(state, ids), 0.0f);
	VALUE banking_ratio = adaptive_ve::max<VALUE>(bank_saving_rate<VALUE>(state, ids), 0.0f);

	VALUE total_spending_ratio =
		life_spending_ratio
		+ housing_spending_ratio
		+ everyday_spending_ratio
		+ luxury_spending_ratio
		+ education_spending_ratio
		+ investment_ratio
		+ banking_ratio;

	if constexpr(std::same_as<POPS, dcon::pop_id>) {
		total_spending_ratio = total_spending_ratio < 1.f ? 1.f : total_spending_ratio;
	} else {
		total_spending_ratio = adaptive_ve::select<BOOL_VALUE, VALUE>(total_spending_ratio < 1.f, ve::fp_vector{ 1.f }, total_spending_ratio);
	}

	// normalize:

	life_spending_ratio = life_spending_ratio / total_spending_ratio;
	housing_spending_ratio = housing_spending_ratio / total_spending_ratio;
	everyday_spending_ratio = everyday_spending_ratio / total_spending_ratio;
	luxury_spending_ratio = luxury_spending_ratio / total_spending_ratio;
	education_spending_ratio = education_spending_ratio / total_spending_ratio;
	investment_ratio = investment_ratio / total_spending_ratio;
	banking_ratio = banking_ratio / total_spending_ratio;

	// set actual budgets

	VALUE spend_on_life_needs = life_spending_ratio * savings;
	VALUE spend_on_housing = housing_spending_ratio * savings;
	VALUE spend_on_everyday_needs = everyday_spending_ratio * savings;
	VALUE spend_on_luxury_needs = luxury_spending_ratio * savings;
	VALUE spend_on_education = education_spending_ratio * savings;
	VALUE spend_on_investments = investment_ratio * savings;
	VALUE spend_on_bank_savings = banking_ratio * savings;

	// upload data to structure
	// here we do logic which can't be made uniform

	VALUE satisfaction = state.world.pop_get_satisfaction(ids);


	// ##########
	// life needs
	// ##########

	VALUE old_life = pop_demographics::get_life_needs(state, ids);
	VALUE subsistence = adjusted_subsistence_score<VALUE, decltype(provs)>(state, provs);
	BOOL_VALUE rgo_worker = state.world.pop_type_get_is_paid_rgo_worker(pop_type);
	subsistence = adaptive_ve::select<BOOL_VALUE, VALUE>(rgo_worker, subsistence, 0.f);
	VALUE available_subsistence = adaptive_ve::min<VALUE>(subsistence_score_life, subsistence);
	subsistence = subsistence - available_subsistence;
	VALUE qol_from_subsistence = available_subsistence / subsistence_score_life;
	// Households reduce discretionary quantity in a downturn, but basic demand
	// never disappears.  The floor prevents a zero-satisfaction feedback loop
	// where starving POPs stop demanding the goods required to recover.
	VALUE demand_scale_life = adaptive_ve::min<VALUE>(1.f,
		adaptive_ve::max<VALUE>(0.60f, 0.60f + 0.40f * satisfaction));
	result.life_needs.demand_scale = demand_scale_life;// * demand_scale_life + 0.01f;
	result.life_needs.required =
		result.life_needs.demand_scale
		* life_costs
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
	auto zero_life_costs = result.life_needs.required == 0;
	/*
	auto rich_but_life_needs_are_not_satisfied = adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_life_costs,
		1.f,
		adaptive_ve::min<VALUE>
			(
				2.f,
				adaptive_ve::max<VALUE>(0.f, spend_on_life_needs - result.life_needs.required * 5.f) / result.life_needs.required
			)
	);
	*/
	result.life_needs.spent = adaptive_ve::min<VALUE>(spend_on_life_needs, result.life_needs.required * (1.f + is_rich));
	result.life_needs.satisfied_with_money_ratio = adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_life_costs,
		10.f,
		result.life_needs.spent
		/ result.life_needs.required
	);
	// subsistence gives free "level of consumption"
	result.life_needs.satisfied_for_free_ratio = qol_from_subsistence / (1.f + result.life_needs.demand_scale);
	result.spent_total = result.spent_total + result.life_needs.spent;
	savings = savings - result.life_needs.spent;


	// ##############
	// housing
	// ##############
	auto housing_price = state.world.province_get_service_price(provs, services::list::urban_housing);
	// Rural households are covered by the subsistence economy. Only the urban
	// share of a province participates in the explicit urban-housing market;
	// charging every POP made rural demand overwhelm the seeded city stock.
	// Housing is supplied by the city stock that landlords have actually built
	// and put on the market.  The maximum private size is merely developable
	// land; treating it as homes made a future construction project satisfy POPs
	// before a single dwelling existed.
	VALUE urban_capacity = state.world.province_get_advanced_province_building_private_size(
		provs, advanced_province_buildings::list::local_cities_and_towns);
	VALUE province_population = state.world.province_get_demographics(
		provs, demographics::total);
	VALUE urban_share = adaptive_ve::min<VALUE>(1.f,
		adaptive_ve::max<VALUE>(0.f, urban_capacity / (province_population + 1.f)));
	result.housing.demand_scale = urban_share;
	result.housing.required = pop_size * urban_share * housing_price;
	auto zero_housing_costs = result.housing.required == 0;
	//result.housing.spent = adaptive_ve::min<VALUE>(savings, adaptive_ve::min<VALUE>(spend_on_housing, result.housing.required));
	result.housing.spent = adaptive_ve::min<VALUE>(savings, adaptive_ve::min<VALUE>(result.housing.required * 1.5f, spend_on_housing));
	result.housing.satisfied_for_free_ratio = 0.f;
	result.housing.satisfied_with_money_ratio = safe_spending_ratio(
		zero_housing_costs,
		result.housing.spent,
		result.housing.required,
		VALUE{ 1.f }
	);
	result.spent_total = result.spent_total + result.housing.spent;
	savings = savings - result.housing.spent;


	// ##############
	// everyday needs
	// ##############

	auto old_everyday = pop_demographics::get_everyday_needs(state, ids);
	auto demand_scale_everyday = adaptive_ve::min<VALUE>(1.20f,
		adaptive_ve::max<VALUE>(0.35f, 0.35f + 0.65f * old_everyday / base_qol));
	result.everyday_needs.demand_scale = demand_scale_everyday;// * demand_scale_everyday + 0.01f;
	result.everyday_needs.required =
		result.everyday_needs.demand_scale
		* everyday_costs
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
	auto zero_everyday_costs = result.everyday_needs.required == 0;
	auto rich_but_everyday_needs_are_not_satisfied = 0.f;
	/*
	adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_everyday_costs,
		1.f,
		adaptive_ve::min<VALUE>
		(
			5.f,
			adaptive_ve::max<VALUE>(0.f, spend_on_everyday_needs - result.everyday_needs.required * 5.f) / result.everyday_needs.required
		)
	);
	*/
	result.everyday_needs.spent = adaptive_ve::min<VALUE>(savings, adaptive_ve::min<VALUE>(spend_on_everyday_needs, result.everyday_needs.required * (1.f + is_rich)));
	result.everyday_needs.satisfied_with_money_ratio = adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_everyday_costs,
		10.f,
		result.everyday_needs.spent
		/ result.everyday_needs.required
	);
	result.everyday_needs.satisfied_for_free_ratio = 0.f;
	result.spent_total = result.spent_total + result.everyday_needs.spent;
	savings = savings - result.everyday_needs.spent;



	// ############
	// luxury needs
	// ############

	auto old_luxury = pop_demographics::get_luxury_needs(state, ids);
	auto demand_scale_luxury = adaptive_ve::min<VALUE>(1.50f,
		adaptive_ve::max<VALUE>(0.05f, 0.05f + 0.95f * old_luxury / base_qol));
	result.luxury_needs.demand_scale = demand_scale_luxury;// * demand_scale_luxury + 0.01f;
	result.luxury_needs.required =
		result.luxury_needs.demand_scale
		* luxury_costs
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
	auto zero_luxury_costs = result.luxury_needs.required == 0;
	auto rich_but_luxury_needs_are_not_satisfied = 0.f;
	/*
	adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_luxury_costs,
		1.f,
		adaptive_ve::min<VALUE>
		(
			5.f,
			adaptive_ve::max<VALUE>(0.f, spend_on_luxury_needs - result.luxury_needs.required * 5.f) / result.luxury_needs.required
		)
	);
	*/
	result.luxury_needs.spent = adaptive_ve::min<VALUE>(savings, adaptive_ve::min<VALUE>(spend_on_luxury_needs, result.luxury_needs.required * (1.f + is_rich)));
	result.luxury_needs.satisfied_for_free_ratio = 0.f;
	result.luxury_needs.satisfied_with_money_ratio = adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_luxury_costs,
		10.f,
		result.luxury_needs.spent
		/ result.luxury_needs.required
	);
	result.spent_total = result.spent_total + result.luxury_needs.spent;
	savings = savings - result.luxury_needs.spent;



	// #########
	// education
	// #########

	auto education_price = state.world.province_get_service_price(provs, services::list::education);

	auto literacy = pop_demographics::get_literacy(state, ids);
	result.education.demand_scale = literacy * literacy / 0.5f + 0.1f;
	auto required_education = result.education.demand_scale * pop_size;
	result.education.required = required_education * education_price;

	// if education is crazy expensive and impossible to access, we want to spend 0 because it's hopeless

	auto scale_from_being_rich = adaptive_ve::select<BOOL_VALUE, VALUE>(
		required_education == 0.f,
		1.f,
		adaptive_ve::max<VALUE>
		(
			0.f,
			adaptive_ve::min<VALUE>
			(
				10.f,
				adaptive_ve::max<VALUE>(0.f, spend_on_education - result.education.required * 5.f)
				/ result.education.required
			) - 0.1f
		)
	);

	auto education_scale_nation = adaptive_ve::select<BOOL_VALUE, VALUE>(result.can_use_free_services > 0.f, 1.f, 0.f);

	auto personal_desired_spending = result.education.required * scale_from_being_rich;
	auto can_actually_spend = adaptive_ve::min<VALUE>(savings, spend_on_education);
	auto total_personal_spending = adaptive_ve::min<VALUE>(can_actually_spend, personal_desired_spending);
	auto education_scale_private = scale_from_being_rich * adaptive_ve::select<BOOL_VALUE, VALUE>(personal_desired_spending > 0.f, total_personal_spending / personal_desired_spending, 0.f);

	//auto probability_to_get_education_for_free = state.world.province_get_service_satisfaction_for_free(provs, services::list::education);
	auto expected_help_from_nation = adaptive_ve::select<BOOL_VALUE, VALUE>(result.can_use_free_services > 0.f, result.education.required, 0.f);
	auto total_expected_spending = expected_help_from_nation + total_personal_spending;

	auto potentially_free_ratio = adaptive_ve::select<BOOL_VALUE, VALUE>(total_expected_spending > 0.f, expected_help_from_nation / total_expected_spending, 0.f);

	//auto supposed_to_spend = adaptive_ve::min<VALUE>(savings, adaptive_ve::min<VALUE>(spend_on_education, result.education.required * rich_but_uneducated));
	//auto potentially_free_ratio = expected_help_from_nation / adaptive_ve::max<VALUE>(1.f, rich_but_uneducated);
	//auto ratio_of_free_education = decltype(potentially_free_ratio)(0.f);
	//ratio_of_free_education = adaptive_ve::select<BOOL_VALUE, VALUE>(result.can_use_free_services > 0.f, potentially_free_ratio, ratio_of_free_education);

	result.education.satisfied_for_free_ratio = education_scale_nation;
	result.education.spent = total_personal_spending;
	result.education.satisfied_with_money_ratio = education_scale_private;
	result.spent_total = result.spent_total + result.education.spent;
	savings = savings - result.education.spent;



	// ###########
	// investments
	// ###########

	result.investments.required = 0.f;
	result.investments.satisfied_with_money_ratio = 1.f;
	result.investments.satisfied_for_free_ratio = 0.f;
	result.investments.spent = spend_on_investments;
	result.investments.demand_scale = 0.f;
	result.spent_total = result.spent_total + result.investments.spent;
	savings = savings - result.investments.spent;

	// #####
	// banks
	// #####

	result.bank_savings.required = 0.f;
	result.bank_savings.satisfied_with_money_ratio = 1.f;
	result.bank_savings.satisfied_for_free_ratio = 0.f;
	result.bank_savings.spent = spend_on_bank_savings;
	result.bank_savings.demand_scale = 0.f;
	result.spent_total = result.spent_total + result.bank_savings.spent;
	savings = savings - result.bank_savings.spent;

	result.remaining_savings = savings;

	return result;
}


void update_consumption(
	sys::state& state,
	ve::vectorizable_buffer<float, dcon::nation_id>& invention_count,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_life,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_housing,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_everyday,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_luxury,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_education_private,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_education_public,
	ve::vectorizable_buffer<float, dcon::pop_id>& demand_life,
	ve::vectorizable_buffer<float, dcon::pop_id>& demand_housing,
	ve::vectorizable_buffer<float, dcon::pop_id>& demand_everyday,
	ve::vectorizable_buffer<float, dcon::pop_id>& demand_luxury,
	ve::vectorizable_buffer<float, dcon::pop_id>& demand_paid_education,
	ve::vectorizable_buffer<float, dcon::pop_id>& subsistence_ratio
) {
	uint32_t total_commodities = state.world.commodity_size();

	state.ui_state.last_tick_investment_pool_change = 0;

	// satisfaction buffers
	// they store how well pops satisfy their needs
	// we store them per pop now
	// because iteration per state
	// and per pop of each state is way too slow
	// we start with filling them with according subsistence values
	// and then attempt to buy the rest

	ve::fp_vector total_spendings{};

	// temporary buffers for actual pop demand
	auto demand_education_public_allowed = state.world.pop_make_vectorizable_float_buffer();

	auto to_bank = state.world.pop_make_vectorizable_float_buffer();
	auto to_investments = state.world.pop_make_vectorizable_float_buffer();

	state.world.execute_parallel_over_pop([&](auto ids) {
		auto pop_size = state.world.pop_get_size(ids);

		// get all data into vectors
		vectorized_pops_budget<ve::fp_vector> data = prepare_pop_budget_templated(state, ids);

		// "for free" in the context of life/everyday/luxury needs means subsistence/gifts
		// so we apply it directly there, it's not registered as demand
		// "for free" in the context of education means that someone paid for you
		// so it will be registered as demand

		buffer_life.set(ids, data.life_needs.satisfied_with_money_ratio);
		buffer_housing.set(ids, data.housing.satisfied_with_money_ratio);
		buffer_everyday.set(ids, data.everyday_needs.satisfied_with_money_ratio);
		buffer_luxury.set(ids, data.luxury_needs.satisfied_with_money_ratio);
		buffer_education_private.set(ids, data.education.satisfied_with_money_ratio);
		buffer_education_public.set(ids, data.education.satisfied_for_free_ratio);

		subsistence_ratio.set(ids, data.life_needs.satisfied_for_free_ratio);

		auto multiplier = pop_size / state.defines.alice_needs_scaling_factor;

		demand_life.set(ids, multiplier * data.life_needs.demand_scale * data.life_needs.satisfied_with_money_ratio);
		demand_housing.set(ids, pop_size * data.housing.demand_scale * data.housing.satisfied_with_money_ratio);
		demand_everyday.set(ids, multiplier * data.everyday_needs.demand_scale * data.everyday_needs.satisfied_with_money_ratio);
		demand_luxury.set(ids, multiplier * data.luxury_needs.demand_scale * data.luxury_needs.satisfied_with_money_ratio);
		demand_education_public_allowed.set(
			ids,
			pop_size * data.education.demand_scale
			* data.education.satisfied_for_free_ratio
			* data.can_use_free_services
		);
		demand_paid_education.set(ids, pop_size * data.education.demand_scale * data.education.satisfied_with_money_ratio);

		to_bank.set(ids, data.bank_savings.spent);
		to_investments.set(ids, data.investments.spent);

		// we do save savings here because a part of education is given for free
		// which leads to some part of wealth not being spent most of the time
		state.world.pop_set_savings(ids, ve::max(0.f, data.remaining_savings));
	});

	// services are at province level:

	concurrency::parallel_for(int32_t(0), state.province_definitions.first_sea_province.index(), [&](uint32_t raw_province_id) {
		auto pid = dcon::province_id{ dcon::province_id::value_base_t(raw_province_id) };
		state.world.province_for_each_pop_location(pid, [&](auto location) {
			auto pop = state.world.pop_location_get_pop(location);

			auto demand_allow_public = demand_education_public_allowed.get(pop);
			auto demand_forbid_public = demand_paid_education.get(pop);

			auto old_allow = state.world.province_get_service_demand_allowed_public_supply(pid, services::list::education);
			auto old_forbid = state.world.province_get_service_demand_forbidden_public_supply(pid, services::list::education);

			state.world.province_set_service_demand_allowed_public_supply(pid, services::list::education, old_allow + demand_allow_public);
			state.world.province_set_service_demand_forbidden_public_supply(pid, services::list::education, old_forbid + demand_forbid_public);

			auto housing_old = state.world.province_get_service_demand_forbidden_public_supply(pid, services::list::urban_housing);
			state.world.province_set_service_demand_forbidden_public_supply(pid, services::list::urban_housing, housing_old + demand_housing.get(pop));
		});
	});

	// commodities (actually groups of commodities) are at market level

	concurrency::parallel_for(uint32_t(0), state.world.market_size(), [&](uint32_t raw_market_id) {
		auto m = dcon::market_id{ dcon::market_id::value_base_t(raw_market_id) };
		if(!state.world.market_is_valid(m)) return;
		auto zone = state.world.market_get_zone_from_local_market(m);
		province::for_each_province_in_state_instance(state, zone, [&](auto pid) {
			state.world.province_for_each_pop_location(pid, [&](auto location) {
				auto pop = state.world.pop_location_get_pop(location);
				auto pop_type = state.world.pop_get_poptype(pop);

				auto life = demand_life.get(pop);
				auto everyday = demand_everyday.get(pop);
				auto luxury = demand_luxury.get(pop);

				auto old_life = state.world.market_get_life_needs_scale(m, pop_type);
				auto old_everyday = state.world.market_get_everyday_needs_scale(m, pop_type);
				auto old_luxury = state.world.market_get_luxury_needs_scale(m, pop_type);

				state.world.market_set_life_needs_scale(m, pop_type, old_life + life);
				state.world.market_set_everyday_needs_scale(m, pop_type, old_everyday + everyday);
				state.world.market_set_luxury_needs_scale(m, pop_type, old_luxury + luxury);
			});
		});
	});

	// investments are at national level:

	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t raw_nation_id) {
		auto n = dcon::nation_id{ dcon::nation_id::value_base_t(raw_nation_id) };
		if(!state.world.nation_is_valid(n)) return;
		state.world.nation_for_each_province_ownership(n, [&](auto poid) {
			state.world.province_for_each_pop_location(state.world.province_ownership_get_province(poid), [&](auto location) {
				auto pop = state.world.pop_location_get_pop(location);

				auto investment = to_investments.get(pop);
				auto current_inv = state.world.nation_get_private_investment(n);
				state.world.nation_set_private_investment(n, current_inv + investment);

				if(n == state.local_player_nation) {
					state.ui_state.last_tick_investment_pool_change += investment;
				}

				auto banking = to_bank.get(pop);
				auto current_bank = state.world.nation_get_national_bank(n);
				state.world.nation_set_national_bank(n, current_bank + banking);
			});
		});
	});

	// iterate over all (market, pop type, trade good) triples to finalise this calculation and register demand to actual commodities
	state.world.execute_parallel_over_market([&](auto ids) {
		auto states = state.world.market_get_zone_from_local_market(ids);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
		auto invention_factor = state.defines.invention_impact_on_demand * invention_count.get(nations) + 1.f;

		ve::fp_vector life_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_life_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_life_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_life_needs) + 1.0f
		};
		ve::fp_vector everyday_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
		};
		ve::fp_vector luxury_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_luxury_needs) + 1.0f,
		};

		for(const auto t : state.world.in_pop_type) {
			auto scale_life = state.world.market_get_life_needs_scale(ids, t);
			auto scale_everyday = state.world.market_get_everyday_needs_scale(ids, t);
			auto scale_luxury = state.world.market_get_luxury_needs_scale(ids, t);

			auto strata = t.get_strata();

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

				auto life_weight =
					state.world.market_get_life_needs_weights(ids, cid);
				auto everyday_weight =
					state.world.market_get_everyday_needs_weights(ids, cid);
				auto luxury_weight =
					state.world.market_get_luxury_needs_weights(ids, cid);
				auto base_life =
					state.world.pop_type_get_life_needs(t, cid);
				auto base_everyday =
					state.world.pop_type_get_everyday_needs(t, cid);
				auto base_luxury =
					state.world.pop_type_get_luxury_needs(t, cid);

				auto demand_life =
					base_life
					* scale_life
					* life_mul[strata]
					* life_weight
					* state.defines.alice_lf_needs_scale;
				auto demand_everyday =
					base_everyday
					* scale_everyday
					* everyday_mul[strata]
					* everyday_weight
					* state.defines.alice_ev_needs_scale
					* invention_factor;
				auto demand_luxury =
					base_luxury
					* scale_luxury
					* luxury_mul[strata]
					* luxury_weight
					* state.defines.alice_lx_needs_scale
					* invention_factor;

				register_demand(state, ids, cid, demand_life);
				register_demand(state, ids, cid, demand_everyday);
				register_demand(state, ids, cid, demand_luxury);
			}
		}
	});
}

float estimate_artisan_income(sys::state const& state, dcon::province_id pid, dcon::pop_type_id ptid, float size) {
	auto const artisan_type = state.culture_definitions.artisans;
	auto key = demographics::to_key(state, artisan_type);

	if(ptid != artisan_type) {
		return 0.f;
	}

	auto artisan_profit = state.world.province_get_artisan_profit(pid);
	auto current_bank = state.world.province_get_artisan_bank(pid);
	auto total = artisan_profit + current_bank;
	auto dividents = total > 0.f ? total * 0.1f : 0.f;

	auto num_artisans = state.world.province_get_demographics(pid, key);
	auto per_artisan = num_artisans > 0.f ? dividents / num_artisans : 0.f;
	return size * per_artisan;
}

float estimate_artisan_income(sys::state const& state, dcon::pop_id pop) {
	return estimate_artisan_income(
		state,
		state.world.pop_get_province_from_pop_location(pop),
		state.world.pop_get_poptype(pop),
		state.world.pop_get_size(pop)
	);
}

void update_income_artisans(sys::state& state) {
	auto const artisan_type = state.culture_definitions.artisans;
	auto key = demographics::to_key(state, artisan_type);

	state.world.execute_parallel_over_province([&](auto pids) {
		auto artisan_profit = state.world.province_get_artisan_profit(pids);
		auto current_bank = state.world.province_get_artisan_bank(pids);
		state.world.province_set_artisan_bank(pids, current_bank + artisan_profit);
		state.world.province_set_artisan_profit(pids, 0.f);

		auto new_bank = state.world.province_get_artisan_bank(pids);
		auto dividents = ve::select(new_bank > 0.f, new_bank * 0.1f, 0.f);
		state.world.province_set_artisan_bank(pids, new_bank - dividents);

		auto num_artisans = state.world.province_get_demographics(pids, key);
		auto per_artisan = ve::select(num_artisans > 0.f, dividents / num_artisans, 0.f);

		ve::apply([&](auto province, auto payment) {
			for(auto pl : state.world.province_get_pop_location(province)) {
				if(artisan_type == pl.get_pop().get_poptype()) {
					auto pop = pl.get_pop();
					pop.set_savings(pop.get_savings() + pop.get_size() * payment);
#ifndef NDEBUG
					assert(
						std::isfinite(pop.get_savings())
						&& pop.get_savings() >= 0
					);
#endif
				}
			}
		}, pids, per_artisan);
	});
}


constexpr inline float national_elite_trade_weight_multiplier = 1000.f;

float estimate_local_trade_income(sys::state const& state, dcon::province_id pid, dcon::market_id mid, dcon::pop_type_id ptid, float size) {
	auto sids = state.world.market_get_zone_from_local_market(mid);
	auto nation = state.world.province_get_nation_from_province_control(pid);
	auto const capis_def = state.culture_definitions.capitalists;
	auto capis_key = demographics::to_key(state, capis_def);
	auto elites = state.world.nation_get_demographics(nation, capis_key);
	auto local_population_province = state.world.province_get_demographics(pid, demographics::total);
	auto local_population_market = state.world.state_instance_get_demographics(sids, demographics::total);
	auto total = local_population_market + elites * national_elite_trade_weight_multiplier * local_population_province / (local_population_market + 1.f);

	if(total == 0.f) {
		return 0.f;
	}

	auto balance = state.world.market_get_stockpile(mid, economy::money);
	auto trade_dividents = balance > 0.f ? balance * trade_dividents_rate : 0.f;

	return size / total * trade_dividents;
}

/*
Currently ignores income from national trade.
*/
float estimate_trade_income(sys::state const& state, dcon::pop_id pop) {
	auto provs = state.world.pop_get_province_from_pop_location(pop);
	auto states = state.world.province_get_state_membership(provs);
	auto markets = state.world.state_instance_get_market_from_local_market(states);

	return estimate_local_trade_income(
		state,
		provs,
		markets,
		state.world.pop_get_poptype(pop),
		state.world.pop_get_size(pop)
	);
}

void update_income_non_labor(sys::state& state) {
	/*

	Sum up tokens of eligible pops.
	Sum up cash.
	Set "price" of token
	Distribute cash according to tokens

	Additional national tokens rules:
		COULD BE TOO CPU EXPENSIVE (PROBABLY DOES NOT WORTH IT): Primary/accepted culture: +1 token
	Capital: +1 token
	100'000 city size: +1 token

	1) Rent income:
		Eligible pops:
			Capitalists
			Aristocrats
		Eligible scopes:
			Local

	2) Trade income:
		Group 1
			Note:
				Represents local people who benefit from trade
			Eligible pops:
				For now EVERYONE (because otherwise cash would be accumulated in a random island without eligible pops)
				Later it would be nice to include "important" pops in the national capital
			Eligible scopes:
				Local
		Group 2 (weight: x100)
			Note:
				Represents national elites which conduct trade
			Eligible pops:
				Capitalists
			Eligible scopes:
				Nation

	3) RGO income:
		Eligible pops:
			Capitalists
			Aristocrats
			RGO workers
		Eligible scopes:
			(TODO) Nation
			Local

	4) Factory income:
		Eligible pops:
			Capitalists
		Eligible scopes:
			(TODO) Nation
			Local
	*/

	static ve::vectorizable_buffer<float, dcon::market_id> market_rent_tokens(uint32_t(1));
	static ve::vectorizable_buffer<float, dcon::market_id> market_trade_tokens(uint32_t(1));
	static ve::vectorizable_buffer<float, dcon::market_id> market_rgo_tokens(uint32_t(1));
	static ve::vectorizable_buffer<float, dcon::market_id> market_factory_tokens(uint32_t(1));

	static ve::vectorizable_buffer<float, dcon::market_id> market_rent_money(uint32_t(1));
	static ve::vectorizable_buffer<float, dcon::market_id> market_rgo_money(uint32_t(1));
	static ve::vectorizable_buffer<float, dcon::market_id> market_factory_money(uint32_t(1));

	static ve::vectorizable_buffer<float, dcon::nation_id> nation_trade_tokens(uint32_t(1));
	static ve::vectorizable_buffer<float, dcon::nation_id> nation_trade_money(uint32_t(1));
	static ve::vectorizable_buffer<float, dcon::nation_id> nation_rgo_tokens(uint32_t(1));
	static ve::vectorizable_buffer<float, dcon::nation_id> nation_factory_tokens(uint32_t(1));
	{
		static uint32_t old_count = 1;
		auto new_count = state.world.market_size();
		if(new_count > old_count) {
			market_rent_tokens = state.world.market_make_vectorizable_float_buffer();
			market_trade_tokens = state.world.market_make_vectorizable_float_buffer();
			market_rgo_tokens = state.world.market_make_vectorizable_float_buffer();
			market_factory_tokens = state.world.market_make_vectorizable_float_buffer();

			market_rent_money = state.world.market_make_vectorizable_float_buffer();
			market_rgo_money = state.world.market_make_vectorizable_float_buffer();
			market_factory_money = state.world.market_make_vectorizable_float_buffer();

			old_count = new_count;
		}

		static uint32_t old_count_nation = 1;
		auto new_count_nation = state.world.nation_size();

		if(new_count_nation > old_count_nation) {
			nation_trade_tokens = state.world.nation_make_vectorizable_float_buffer();
			nation_trade_money = state.world.nation_make_vectorizable_float_buffer();
			nation_rgo_tokens = state.world.nation_make_vectorizable_float_buffer();
			nation_factory_tokens = state.world.nation_make_vectorizable_float_buffer();
			old_count_nation = new_count_nation;
		}
	}

	/*
	Clear tokens count
	*/


	auto const capis_def = state.culture_definitions.capitalists;
	auto capis_key = demographics::to_key(state, capis_def);

	state.world.execute_serial_over_market([&](auto mid_vector){
		market_rent_tokens.set(mid_vector, 0.f);
		market_trade_tokens.set(mid_vector, 0.f);
		market_rgo_tokens.set(mid_vector, 0.f);
		market_factory_tokens.set(mid_vector, 0.f);

		market_rent_money.set(mid_vector, 0.f);
		market_rgo_money.set(mid_vector, 0.f);
		market_factory_money.set(mid_vector, 0.f);
	});

	state.world.execute_serial_over_nation([&](auto nation_vector){
		nation_trade_money.set(nation_vector, 0.f);
		nation_trade_tokens.set(nation_vector, state.world.nation_get_demographics(nation_vector, capis_key) * national_elite_trade_weight_multiplier);
	});

	/*

	Calculate tokens and available cash for markets
	Give the cash only when pops are of size at least X to avoid issues with demography values being different from the actual values

	*/

	constexpr float min_registered_token_size = 2.f;

	constexpr float expected_share = trade_dividents_rate;

	auto const artisan_def = state.culture_definitions.artisans;
	auto artisan_key = demographics::to_key(state, artisan_def);

	auto const clerks_def = state.culture_definitions.secondary_factory_worker;
	auto clerks_key = demographics::to_key(state, clerks_def);


	auto const aristo_def = state.culture_definitions.aristocrat;
	auto aristo_key = demographics::to_key(state, aristo_def);

	auto const bur_def = state.culture_definitions.bureaucrat;
	auto bur_key = demographics::to_key(state, bur_def);

	province::for_each_market_province_parallel_over_market(state, [&](dcon::market_id mid, dcon::state_instance_id sid, dcon::province_id pid){

		// distribute private teaching profits right now
		auto education_funds = state.world.province_get_advanced_province_building_private_savings(pid, advanced_province_buildings::list::schools_and_universities);
		auto eligible_pops = 0.f;
		state.world.province_for_each_pop_location(pid, [&](auto pop_location){
			auto pop = state.world.pop_location_get_pop(pop_location);
			auto size = state.world.pop_get_size(pop);
			auto pt = state.world.pop_get_poptype(pop);

			if(
				pt == state.culture_definitions.secondary_factory_worker
				|| pt == state.culture_definitions.bureaucrat
				|| pt == state.culture_definitions.clergy
			) {
				eligible_pops += size;
			}
		});

		if(eligible_pops > 0.f && education_funds > 0.f) {
			auto per_pop = education_funds / eligible_pops * expected_share;
			state.world.province_for_each_pop_location(pid, [&](auto pop_location){
				auto pop = state.world.pop_location_get_pop(pop_location);
				auto size = state.world.pop_get_size(pop);
				auto pt = state.world.pop_get_poptype(pop);

				if(
					pt == state.culture_definitions.secondary_factory_worker
					|| pt == state.culture_definitions.bureaucrat
					|| pt == state.culture_definitions.clergy
				) {
					auto income = size * per_pop;
					auto current = state.world.pop_get_savings(pop);

					assert(std::isfinite(income));

					state.world.pop_set_savings(pop, current + income);
				}
			});

			state.world.province_set_advanced_province_building_private_savings(pid, advanced_province_buildings::list::schools_and_universities, education_funds * (1.f - expected_share));
		}



		// TOKENS

		auto artisans = state.world.province_get_demographics(pid, artisan_key);
		auto clerks = state.world.province_get_demographics(pid, clerks_key);
		auto capis = state.world.province_get_demographics(pid, capis_key);
		auto nation = state.world.province_get_nation_from_province_control(pid);
		auto national_trade_elites = state.world.nation_get_demographics(nation, capis_key);
		auto aristo = state.world.province_get_demographics(pid, aristo_key);
		auto bur = state.world.province_get_demographics(pid, bur_key);
		auto total_pop = state.world.province_get_demographics(pid, demographics::total);

		auto total_state_pops = state.world.state_instance_get_demographics(sid, demographics::total);
		auto local_province_weight = total_pop / (total_state_pops + 1.f);

		auto rgo_workers = ve::fp_vector{ 0.f };
		state.world.for_each_pop_type([&](dcon::pop_type_id ptid) {
			if(
				state.world.pop_type_get_is_paid_rgo_worker(ptid)
			) {
				rgo_workers = rgo_workers + state.world.province_get_demographics(pid, demographics::to_key(state, ptid));
			}
		});
		
		{
			auto current = market_rent_tokens.get(mid);
			market_rent_tokens.set(mid, current + aristo + capis);
		}

		{
			auto current = market_trade_tokens.get(mid);
			market_trade_tokens.set(mid, current + total_pop + local_province_weight * nation_trade_tokens.get(nation));
		}

		{
			auto capis_share = state.world.province_get_capitalists_share(pid);
			auto aristo_share = state.world.province_get_landowners_share(pid);
			auto current = market_rgo_tokens.get(mid);
			market_rgo_tokens.set(mid, current + capis * capis_share + aristo * aristo_share + rgo_workers * (1.f - capis_share - aristo_share));
		}

		{
			auto current = market_factory_tokens.get(mid);
			market_factory_tokens.set(mid, current + capis);
		}

		// MONEY

		{
			auto total = market_rent_money.get(mid);
			auto current_money = state.world.province_get_advanced_province_building_private_savings(pid, advanced_province_buildings::list::local_cities_and_towns);
			if (current_money > 0.f)
				market_rent_money.set(mid, total + current_money);
		}

		{
			auto total = market_rgo_money.get(mid);
			auto current_money = state.world.province_get_rgo_bank(pid);
			if(current_money > 0.f)
				market_rgo_money.set(mid, total + current_money);
		}

		{
			auto total = market_factory_money.get(mid);
			auto current_money = state.world.province_get_factory_bank(pid);
			if(current_money > 0.f)
				market_factory_money.set(mid, total + current_money);
		}
	});


	/*
	Calculate money pool for nation
	*/

	province::for_each_nation_controlled_province_parallel_over_nation(state, [&](dcon::nation_id nation, dcon::province_id province) {
		auto states = state.world.province_get_state_membership(province);
		auto market = state.world.state_instance_get_market_from_local_market(states);
		auto valid_market = market != dcon::market_id{ };
		if(
			!valid_market
			|| state.world.market_get_stockpile(market, economy::money) <= 0.f
		) {
			return;
		}
		auto local_population_province = state.world.province_get_demographics(province, demographics::total);
		auto local_population_market = state.world.state_instance_get_demographics(states, demographics::total);
		auto local_province_weight = local_population_province / (local_population_market + 1.f);
		auto national_elites_weight = nation_trade_tokens.get(nation);
		auto local_weight = market_trade_tokens.get(market);

		auto total_money = state.world.market_get_stockpile(market, economy::money);
		auto validated_money =local_weight > min_registered_token_size ? total_money : 0.f;

		auto current = nation_trade_money.get(nation);
		auto to_add = validated_money * local_province_weight * national_elites_weight / (local_weight + 1.f);
		nation_trade_money.set(nation, current + to_add);
	});

	/*

	Now we know the worth of every token and can distribute money in vectorised way
	Guess there are enough calculations to make it parallel, but it requires benchmarking

	*/

	state.world.execute_parallel_over_pop([&](auto pop_vector) {

		auto pop_type = state.world.pop_get_poptype(pop_vector);
		auto size = state.world.pop_get_size(pop_vector);

		auto province = state.world.pop_get_province_from_pop_location(pop_vector);
		auto nation = state.world.province_get_nation_from_province_ownership(province);
		auto capis_share = state.world.province_get_capitalists_share(province);
		auto aristo_share = state.world.province_get_landowners_share(province);
		auto zone = state.world.province_get_state_membership(province);
		auto market = state.world.state_instance_get_market_from_local_market(zone);

		ve::fp_vector total_income = 0.f;

		auto valid_market = market != dcon::market_id{ };

#ifndef NDEBUG
		ve::fp_vector from_rgo = 0.f;
		ve::fp_vector from_rent = 0.f;
		ve::fp_vector from_market = 0.f;
		ve::fp_vector from_market_national = 0.f;
		ve::fp_vector from_factories = 0.f;
#endif // !NDEBUG

		{
			auto candidates = ve::select(valid_market, market_rent_tokens.get(market), 0.f);
			auto total_money = ve::select(valid_market, market_rent_money.get(market), 0.f);
			auto income = ve::select((pop_type == aristo_def || pop_type == capis_def) && candidates > min_registered_token_size, total_money / candidates * size, 0.f);
#ifndef NDEBUG
			ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, income);
			from_rent = income * expected_share;
#endif // !NDEBUG
			total_income = total_income + income;
		}

		{
			auto candidates = ve::select(valid_market, market_trade_tokens.get(market), 0.f);
			auto total_money = ve::select(valid_market && state.world.market_get_stockpile(market, economy::money) > 0, state.world.market_get_stockpile(market, economy::money), 0.f);
			auto income = ve::select(candidates > min_registered_token_size, total_money / candidates * size, 0.f);
#ifndef NDEBUG
			ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, income);
			from_market = income * expected_share;
#endif // !NDEBUG

			total_income = total_income + income;
		}

		{
			auto candidates = ve::select(valid_market, nation_trade_tokens.get(nation), 0.f);
			auto total_money = ve::select(valid_market, nation_trade_money.get(nation), 0.f);
			auto income = ve::select((pop_type == capis_def) && candidates > min_registered_token_size && size > 0.f, total_money / candidates * size * national_elite_trade_weight_multiplier, 0.f);
#ifndef NDEBUG
			ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, income);
			from_market_national = income * expected_share;
#endif // !NDEBUG

			total_income = total_income + income;
		}

		{
			auto candidates = ve::select(valid_market, market_rgo_tokens.get(market), 0.f);
			auto total_money = ve::select(valid_market, market_rgo_money.get(market), 0.f);
			auto weight =
				ve::select(pop_type == capis_def, capis_share, 0.f)
				+ ve::select(pop_type == aristo_def, aristo_share, 0.f)
				+ ve::select(state.world.pop_type_get_is_paid_rgo_worker(pop_type), 1.f - capis_share - aristo_share, 0.f);
			auto income = ve::select(candidates > min_registered_token_size, total_money / candidates * size * weight, 0.f);
#ifndef NDEBUG
			ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, income);
			from_rgo = income * expected_share;
#endif // !NDEBUG

			total_income = total_income + income;
		}

		{
			auto candidates = ve::select(valid_market, market_factory_tokens.get(market), 0.f);
			auto total_money = ve::select(valid_market, market_factory_money.get(market), 0.f);
			auto income = ve::select((pop_type == capis_def) && candidates > min_registered_token_size && size > 0.f, total_money / candidates * size, 0.f);
#ifndef NDEBUG
			ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, income);
			from_factories = income * expected_share;
#endif // !NDEBUG
			total_income = total_income + income;
		}

		auto initial_savings = state.world.pop_get_savings(pop_vector);
#ifndef NDEBUG
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, total_income);
		auto total = from_rgo + from_rent + from_market + from_factories + from_market_national;
#endif // !NDEBUG
		state.world.pop_set_savings(pop_vector, initial_savings + total_income * expected_share);
	});

	/*

	Reduce values if there were any candidates

	*/

	state.world.execute_serial_over_market([&](auto mid_vector){
		auto trade_tokens = market_trade_tokens.get(mid_vector);
		auto current = state.world.market_get_stockpile(mid_vector, economy::money);
		state.world.market_set_stockpile(mid_vector, economy::money, ve::select(current > 0.f && trade_tokens > min_registered_token_size, current * (1.f - expected_share), current));
	});

	state.world.execute_parallel_over_province([&](auto pid_vector){
		auto zone = state.world.province_get_state_membership(pid_vector);
		auto market = state.world.state_instance_get_market_from_local_market(zone);
		auto valid_market = market != dcon::market_id{ };

		{
			auto current_money = state.world.province_get_advanced_province_building_private_savings(pid_vector, advanced_province_buildings::list::local_cities_and_towns);
			state.world.province_set_advanced_province_building_private_savings(
				pid_vector,
				advanced_province_buildings::list::local_cities_and_towns,
				ve::select(valid_market && market_rent_tokens.get(market) > min_registered_token_size && current_money > 0.f, current_money* (1.f - expected_share), current_money)
			);
		}
		{
			auto current_money = state.world.province_get_rgo_bank(pid_vector);
			state.world.province_set_rgo_bank(
				pid_vector,
				ve::select(valid_market && market_rgo_tokens.get(market) > min_registered_token_size && current_money > 0.f, current_money* (1.f - expected_share), current_money)
			);
		}
		{
			auto current_money = state.world.province_get_factory_bank(pid_vector);
			state.world.province_set_factory_bank(
				pid_vector,
				ve::select(valid_market && market_factory_tokens.get(market) > min_registered_token_size && current_money > 0.f, current_money* (1.f - expected_share), current_money)
			);
		}
	});
}


money_from_nation estimate_income_from_nation(sys::state const& state, dcon::pop_id pop) {
	auto capitalists_key = demographics::to_key(state, state.culture_definitions.capitalists);
	auto aristocracy_key = demographics::to_key(state, state.culture_definitions.aristocrat);

	auto prov = state.world.pop_get_province_from_pop_location(pop);
	auto owner = state.world.province_get_nation_from_province_ownership(prov);
	auto population = state.world.nation_get_demographics(owner, demographics::total);
	auto unemployed = population - state.world.nation_get_demographics(owner, demographics::employed);
	auto capitalists = state.world.nation_get_demographics(owner, capitalists_key);
	auto aristocrats = state.world.nation_get_demographics(owner, aristocracy_key);
	auto investors = capitalists + aristocrats;

	auto states = state.world.province_get_state_membership(prov);
	auto markets = state.world.state_instance_get_market_from_local_market(states);
	auto owner_spending = state.world.nation_get_spending_level(owner);

	auto size = state.world.pop_get_size(pop);
	auto adj_size = size / state.defines.alice_needs_scaling_factor;

	auto budget = state.world.nation_get_last_base_budget(owner);

	auto social_budget =
		owner_spending
		* budget
		* float(state.world.nation_get_social_spending(owner))
		/ 100.f;

	auto investment_budget =
		owner_spending
		* budget
		* float(state.world.nation_get_domestic_investment_spending(owner))
		/ 100.f;

	auto const p_level = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::pension_level);
	auto const unemp_level = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::unemployment_benefit);

	auto pension_ratio = p_level * population > 0.f ? p_level * population / (p_level * population + unemp_level * unemployed) : 0.f;
	auto unemployment_ratio = unemp_level * unemployed > 0.f ? unemp_level * unemployed / (p_level * population + unemp_level * unemployed) : 0.f;

	auto const pension_per_person =
		pension_ratio
		* social_budget
		/ (population + 1.f);

	auto const benefits_per_person =
		unemployment_ratio
		* social_budget
		/ (unemployed + 1.f);
	auto const social_execution = nations::policy_execution::effective_policy(
		state, owner, prov,
		nations::policy_execution::policy_kind::social_benefits).effective_execution;

	auto const payment_per_investor =
		investment_budget
		/ (investors + 1.f);

	auto const m_spending = owner_spending * float(state.world.nation_get_military_spending(owner)) / 100.0f;

	auto types = state.world.pop_get_poptype(pop);

	auto ln_types = state.world.pop_type_get_life_needs_income_type(types);
	auto en_types = state.world.pop_type_get_everyday_needs_income_type(types);
	auto lx_types = state.world.pop_type_get_luxury_needs_income_type(types);

	auto ln_costs = state.world.market_get_life_needs_costs(markets, types);
	auto en_costs = state.world.market_get_everyday_needs_costs(markets, types);
	auto lx_costs = state.world.market_get_luxury_needs_costs(markets, types);

	auto total_costs = ln_costs + en_costs + lx_costs;

	auto is_military_requires_life_needs = ln_types == int32_t(culture::income_type::military);
	auto is_military_requires_everyday_needs = en_types == int32_t(culture::income_type::military);
	auto is_military_requires_luxury_needs = lx_types == int32_t(culture::income_type::military);
	auto is_military = is_military_requires_life_needs || is_military_requires_everyday_needs || is_military_requires_luxury_needs;
	auto is_investor = (types == state.culture_definitions.capitalists) || (types == state.culture_definitions.aristocrat);

	auto mil_pay = 0.f;
	mil_pay += is_military_requires_life_needs ? m_spending * adj_size * ln_costs * payouts_spending_multiplier : 0.0f;
	mil_pay += is_military_requires_everyday_needs ? m_spending * adj_size * en_costs * payouts_spending_multiplier : 0.0f;
	mil_pay += is_military_requires_luxury_needs ? m_spending * adj_size * lx_costs * payouts_spending_multiplier : 0.0f;

	return {
		.pension = social_execution * pension_per_person * size,
		.unemployment = is_military ? 0.f : social_execution * benefits_per_person * (size - pop_demographics::get_employment(state, pop)),
		.military = mil_pay,
		.investment = size * price_properties::labor::min * 0.05f + (is_investor ? payment_per_investor * size : 0.f)
	};
}

inline constexpr float investment_divident_rate = 0.001f;

void update_income_national_subsidy(sys::state& state){
	auto capitalists_key = demographics::to_key(state, state.culture_definitions.capitalists);
	auto aristocracy_key = demographics::to_key(state, state.culture_definitions.aristocrat);

	state.world.execute_parallel_over_pop([&](auto ids) {
		auto provs = state.world.pop_get_province_from_pop_location(ids);
		auto owners = state.world.province_get_nation_from_province_ownership(provs);
		auto population = state.world.nation_get_demographics(owners, demographics::total);
		auto unemployed = population - state.world.nation_get_demographics(owners, demographics::employed);
		auto capitalists = state.world.nation_get_demographics(owners, capitalists_key);
		auto aristocrats = state.world.nation_get_demographics(owners, aristocracy_key);
		auto investors = capitalists + aristocrats;

		auto states = state.world.province_get_state_membership(provs);
		auto markets = state.world.state_instance_get_market_from_local_market(states);
		auto owner_spending = state.world.nation_get_spending_level(owners);

		auto pop_savings = state.world.pop_get_savings(ids);

		auto pop_of_type = state.world.pop_get_size(ids);
		auto adj_pop_of_type = pop_of_type / state.defines.alice_needs_scaling_factor;

		auto budget = state.world.nation_get_last_base_budget(owners);

		auto social_budget =
			owner_spending
			* budget
			* ve::to_float(state.world.nation_get_social_spending(owners))
			/ 100.f;


		auto investment_dividents = (state.world.nation_get_private_investment(owners) + state.world.nation_get_national_bank(owners)) * investment_divident_rate;
		auto investment_budget =
			owner_spending
			* budget
			* ve::to_float(state.world.nation_get_domestic_investment_spending(owners))
			/ 100.f;

		auto const p_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::pension_level);
		auto const unemp_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::unemployment_benefit);

		auto pension_ratio = ve::select(p_level * population > 0.f, p_level * population / (p_level * population + unemp_level * unemployed), 0.f);
		auto unemployment_ratio = ve::select(unemp_level * unemployed > 0.f, unemp_level * unemployed / (p_level * population + unemp_level * unemployed), 0.f);

		// +1.f is here to avoid division by zero
		// which should never happen
		// but i want to be extra sure

		auto const pension_per_person =
			pension_ratio
			* social_budget
			/ (population + 1.f);

		auto const benefits_per_person =
			unemployment_ratio
			* social_budget
			/ (unemployed + 1.f);
		auto const social_execution = ve::apply(
			[&](dcon::nation_id nation, dcon::province_id province) {
				return nations::policy_execution::effective_policy(
					state, nation, province,
					nations::policy_execution::policy_kind::social_benefits).effective_execution;
			}, owners, provs);

		auto const payment_per_investor =
			ve::select(
				investors > 0.f, 
				(investment_dividents + investment_budget)
				/ investors,
				0.f
			);

		auto const m_spending = owner_spending * ve::to_float(state.world.nation_get_military_spending(owners)) / 100.0f;

		auto types = state.world.pop_get_poptype(ids);

		auto ln_types = state.world.pop_type_get_life_needs_income_type(types);
		auto en_types = state.world.pop_type_get_everyday_needs_income_type(types);
		auto lx_types = state.world.pop_type_get_luxury_needs_income_type(types);

		auto ln_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_life_needs_costs(n, pt) : 0.0f; },
				types, markets);
		auto en_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_everyday_needs_costs(n, pt) : 0.0f; },
				types, markets);
		auto lx_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_luxury_needs_costs(n, pt) : 0.0f; },
				types, markets);

		auto total_costs = ln_costs + en_costs + lx_costs;

		auto acc_m = ve::select(ln_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * ln_costs * payouts_spending_multiplier, 0.0f);


		auto acc_u = social_execution * pension_per_person * pop_of_type;

		acc_m = acc_m + ve::select(en_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * en_costs * payouts_spending_multiplier, 0.0f);

		acc_u = acc_u + ve::select(
			types == state.culture_definitions.capitalists,
			payment_per_investor
			* pop_of_type,
			0.0f
		);
		acc_u = acc_u + ve::select(
			types == state.culture_definitions.aristocrat,
			payment_per_investor
			* pop_of_type,
			0.0f
		);

		acc_m = acc_m + ve::select(lx_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * lx_costs * payouts_spending_multiplier, 0.0f);

		auto not_military = !((ln_types == int32_t(culture::income_type::military)) & (en_types == int32_t(culture::income_type::military)) & (lx_types == int32_t(culture::income_type::military)));
		auto employment = pop_demographics::get_employment(state, ids);
		acc_u = acc_u + ve::select(
			not_military,
			social_execution * benefits_per_person
			* (pop_of_type - employment),
			0.0f
		);

		// Subsistence is represented as needs coverage, not cash.  Minting a
		// guaranteed income here had no counterparty and continuously expanded the
		// money supply even when no good or service had been produced.
		// Clamp to zero: a pop created between debug checks (a migration/type-change
		// split in demographics.cpp) can carry a few ULPs of negative float noise in
		// from its source pop's split arithmetic, which this non-negative sum would
		// otherwise propagate as a hard invariant violation for a value that is
		// smaller than any unit of currency by many orders of magnitude.
		state.world.pop_set_savings(ids, ve::max(0.0f,
			state.inflation * state.world.pop_get_savings(ids) + acc_u + acc_m));
#ifndef NDEBUG
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_m);
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_u);
#endif
	});

	// Undelivered transfers remain in the treasury. This keeps weak state
	// capacity from becoming a hidden money sink while local execution still
	// determines which POPs actually receive benefits.
	state.world.execute_serial_over_nation([&](auto ids) {
		auto const execution = ve::apply(
			[&](dcon::nation_id nation) {
				return nations::policy_execution::average_effective_policy(
					state, nation,
					nations::policy_execution::policy_kind::social_benefits);
			}, ids);
		auto const social_budget =
			state.world.nation_get_spending_level(ids)
			* state.world.nation_get_last_base_budget(ids)
			* ve::to_float(state.world.nation_get_social_spending(ids)) / 100.f;
		auto const has_benefits =
			state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::pension_level)
			+ state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::unemployment_benefit) > 0.f;
		auto const money = state.world.nation_get_stockpiles(ids, economy::money);
		state.world.nation_set_stockpiles(ids, economy::money,
			money + ve::select(has_benefits, social_budget * (1.f - execution), 0.f));
	});

	// remove investment dividents:
	state.world.execute_serial_over_nation([&](auto ids) {
		auto investment = state.world.nation_get_private_investment(ids);
		state.world.nation_set_private_investment(ids, investment * (1.f - investment_divident_rate));
		auto bank = state.world.nation_get_national_bank(ids);
		state.world.nation_set_national_bank(ids, bank * (1.f - investment_divident_rate));
	});
}

std::vector<labor_ratio_wage> estimate_wage(sys::state const& state, dcon::province_id pid, dcon::pop_type_id ptid, bool accepted, float size) {
	float no_education_wage =
		state.world.province_get_labor_price(pid, labor::no_education)
		* state.world.province_get_labor_supply_sold(pid, labor::no_education);
	float basic_education_wage =
		state.world.province_get_labor_price(pid, labor::basic_education)
		* state.world.province_get_labor_supply_sold(pid, labor::basic_education); // craftsmen
	float high_education_wage =
		state.world.province_get_labor_price(pid, labor::high_education)
		* state.world.province_get_labor_supply_sold(pid, labor::high_education); // clerks, clergy and bureaucrats
	float guild_education_wage =
		state.world.province_get_labor_price(pid, labor::guild_education)
		* state.world.province_get_labor_supply_sold(pid, labor::guild_education); // artisans
	float high_education_and_accepted_wage =
		state.world.province_get_labor_price(pid, labor::high_education_and_accepted)
		* state.world.province_get_labor_supply_sold(pid, labor::high_education_and_accepted); // clerks, clergy and bureaucrats of accepted culture

	if(state.world.pop_type_get_is_paid_rgo_worker(ptid)) {
		auto no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::rgo_worker_no_education);
		return { {labor::no_education, no_education, no_education * size * no_education_wage } };
	} else if(state.culture_definitions.primary_factory_worker == ptid) {
		auto no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_no_education);
		auto basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_basic_education);
		return {
			{labor::no_education, no_education, no_education * size * no_education_wage },
			{labor::basic_education, basic_education, basic_education * size * basic_education_wage }
		};
	} else if(state.culture_definitions.secondary_factory_worker == ptid || state.culture_definitions.bureaucrat == ptid || state.culture_definitions.clergy == ptid) {
		if(accepted) {
			auto no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_no_education);
			auto basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_basic_education);
			auto high_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education);
			auto high_education_accepted = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education_accepted);
			return {
				{labor::no_education, no_education, no_education * size * no_education_wage },
				{labor::basic_education, basic_education, basic_education * size * basic_education_wage },
				{labor::high_education, high_education, high_education * size * high_education_wage },
				{labor::high_education_and_accepted, high_education_accepted, high_education_accepted * size * high_education_and_accepted_wage }
			};
		} else {
			auto no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_no_education);
			auto basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_basic_education);
			auto high_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_high_education);
			return {
				{labor::no_education, no_education, no_education * size * no_education_wage },
				{labor::basic_education, basic_education, basic_education * size * basic_education_wage },
				{labor::high_education, high_education, high_education * size * high_education_wage },
			};
		}
	}
	return {};
}

std::vector<labor_ratio_wage> estimate_wage(sys::state const& state, dcon::pop_id pop) {
	return estimate_wage(
		state,
		state.world.pop_get_province_from_pop_location(pop),
		state.world.pop_get_poptype(pop),
		state.world.pop_get_is_primary_or_accepted_culture(pop),
		state.world.pop_get_size(pop)
	);
}

float estimate_total_wage(sys::state const& state, dcon::pop_id pop) {
	float total = 0.f;
	auto list = estimate_wage(state, pop);
	for(auto& item : list) {
		total += item.wage;
	}
	return total;
}


float estimate_slave_income(sys::state const& state, dcon::province_id pid, dcon::pop_type_id ptid, float size) {
	float no_education_wage =
		state.world.province_get_labor_price(pid, labor::no_education)
		* state.world.province_get_labor_supply_sold(pid, labor::no_education);
	float rgo_workers_wage =
		state.world.province_get_pop_labor_distribution(pid, pop_labor::rgo_worker_no_education)
		* no_education_wage;
	auto income_from_slaves = 0.f;
	for(auto pl : state.world.province_get_pop_location(pid)) {
		if(pl.get_pop().get_poptype() == state.culture_definitions.slaves) {
			income_from_slaves += pl.get_pop().get_size() * rgo_workers_wage;
		}
	}

	float aristocrats_share = state.world.province_get_landowners_share(pid);
	float num_aristocrat = state.world.province_get_demographics(
		pid,
		demographics::to_key(state, state.culture_definitions.aristocrat)
	);
	if(income_from_slaves >= 0.f && num_aristocrat > 0.f && state.culture_definitions.aristocrat == ptid) {
		return size * income_from_slaves / num_aristocrat;
	} else {
		return 0.f;
	}
}

float estimate_slave_income(sys::state const& state, dcon::pop_id pop) {
	return estimate_slave_income(
		state,
		state.world.pop_get_province_from_pop_location(pop),
		state.world.pop_get_poptype(pop),
		state.world.pop_get_size(pop)
	);
}


//local merchants take a cut from most local monetary operations
inline constexpr float local_market_cut_baseline = 0.01f;
float market_cut(sys::state const& state, dcon::market_id market, float no_education_wage) {
	auto modified = local_market_cut_baseline - state.world.market_get_stockpile(market, economy::money) / (no_education_wage + 0.000001f) / 100'000.f;
	return std::clamp(modified, 0.f, 0.1f);
}

void update_income_wages(sys::state& state){

	static auto buffer_rgo_workers_wage = state.world.province_make_vectorizable_float_buffer();
	static auto buffer_primary_workers_wage = state.world.province_make_vectorizable_float_buffer();
	static auto buffer_high_not_accepted_workers_wage = state.world.province_make_vectorizable_float_buffer();
	static auto buffer_high_accepted_workers_wage = state.world.province_make_vectorizable_float_buffer();

	province::ve_parallel_for_each_land_province(state, [&](auto pid) {
		auto no_education_price = state.world.province_get_labor_price(pid, labor::no_education);
		auto no_education_sold = state.world.province_get_labor_supply_sold(pid, labor::no_education);
		auto no_education_wage = no_education_price * no_education_sold;
		auto basic_education_price = state.world.province_get_labor_price(pid, labor::basic_education);
		auto basic_education_sold = state.world.province_get_labor_supply_sold(pid, labor::basic_education);
		auto basic_education_wage = basic_education_price * basic_education_sold; // craftsmen
		auto high_education_price = state.world.province_get_labor_price(pid, labor::high_education);
		auto high_education_sold = state.world.province_get_labor_supply_sold(pid, labor::high_education);
		auto high_education_wage = high_education_price * high_education_sold; // clerks, clergy and bureaucrats
		auto guild_education_price = state.world.province_get_labor_price(pid, labor::guild_education);
		auto guild_education_sold = state.world.province_get_labor_supply_sold(pid, labor::guild_education);
		auto guild_education_wage = guild_education_price * guild_education_sold; // artisans
		auto high_education_and_accepted_price = state.world.province_get_labor_price(pid, labor::high_education_and_accepted);
		auto high_education_and_accepted_sold = state.world.province_get_labor_supply_sold(pid, labor::high_education_and_accepted);
		auto high_education_and_accepted_wage = high_education_and_accepted_price * high_education_and_accepted_sold; // clerks, clergy and bureaucrats of accepted culture

		auto rgo_worker_no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::rgo_worker_no_education);

		auto rgo_workers_wage =
			rgo_worker_no_education
			* no_education_wage;

		buffer_rgo_workers_wage.set(pid, rgo_workers_wage);

		auto primary_no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_no_education);
		auto primary_basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_basic_education);
		auto primary_workers_wage =
			primary_no_education
			* no_education_wage
			+
			primary_basic_education
			* basic_education_wage;

		buffer_primary_workers_wage.set(pid, primary_workers_wage);

		auto high_not_accepted_no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_no_education);
		auto high_not_accepted_basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_basic_education);
		auto high_not_accepted_high_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_high_education);
		auto high_not_accepted_workers_wage =
			high_not_accepted_no_education
			* no_education_wage
			+
			high_not_accepted_basic_education
			* basic_education_wage
			+
			high_not_accepted_high_education
			* high_education_wage;

		buffer_high_not_accepted_workers_wage.set(pid, high_not_accepted_workers_wage);

		auto high_accepted_no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_no_education);
		auto high_accepted_basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_basic_education);
		auto high_accepted_high_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education);
		auto high_accepted_high_education_accepted = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education_accepted);
		auto high_accepted_workers_wage =
			high_accepted_no_education
			* no_education_wage
			+
			high_accepted_basic_education
			* basic_education_wage
			+
			high_accepted_high_education
			* high_education_wage
			+
			high_accepted_high_education_accepted
			* high_education_and_accepted_wage;

		buffer_high_accepted_workers_wage.set(pid, high_accepted_workers_wage);

#ifndef NDEBUG
		if(wage_trace_producers_enabled()) {
			ve::apply([&](dcon::province_id province,
				float lane_no_education_price,
				float lane_no_education_sold,
				float lane_no_education_wage,
				float lane_basic_education_price,
				float lane_basic_education_sold,
				float lane_basic_education_wage,
				float lane_high_education_price,
				float lane_high_education_sold,
				float lane_high_education_wage,
				float lane_guild_education_price,
				float lane_guild_education_sold,
				float lane_guild_education_wage,
				float lane_high_education_and_accepted_price,
				float lane_high_education_and_accepted_sold,
				float lane_high_education_and_accepted_wage,
				float lane_rgo_worker_no_education,
				float lane_primary_no_education,
				float lane_primary_basic_education,
				float lane_high_not_accepted_no_education,
				float lane_high_not_accepted_basic_education,
				float lane_high_not_accepted_high_education,
				float lane_high_accepted_no_education,
				float lane_high_accepted_basic_education,
				float lane_high_accepted_high_education,
				float lane_high_accepted_high_education_accepted,
				float lane_rgo_workers_wage,
				float lane_primary_workers_wage,
				float lane_high_not_accepted_workers_wage,
				float lane_high_accepted_workers_wage
			) {
				auto check = [](float v) { return !std::isfinite(v) || v < 0.f; };
				if(
					check(lane_no_education_price) || check(lane_no_education_sold) || check(lane_no_education_wage) ||
					check(lane_basic_education_price) || check(lane_basic_education_sold) || check(lane_basic_education_wage) ||
					check(lane_high_education_price) || check(lane_high_education_sold) || check(lane_high_education_wage) ||
					check(lane_guild_education_price) || check(lane_guild_education_sold) || check(lane_guild_education_wage) ||
					check(lane_high_education_and_accepted_price) || check(lane_high_education_and_accepted_sold) || check(lane_high_education_and_accepted_wage) ||
					check(lane_rgo_worker_no_education) || check(lane_primary_no_education) || check(lane_primary_basic_education) ||
					check(lane_high_not_accepted_no_education) || check(lane_high_not_accepted_basic_education) || check(lane_high_not_accepted_high_education) ||
					check(lane_high_accepted_no_education) || check(lane_high_accepted_basic_education) || check(lane_high_accepted_high_education) ||
					check(lane_high_accepted_high_education_accepted) || check(lane_rgo_workers_wage) || check(lane_primary_workers_wage) ||
					check(lane_high_not_accepted_workers_wage) || check(lane_high_accepted_workers_wage)
				) {
					auto date = state.current_date.to_ymd(state.start_date);
					std::fprintf(stderr,
						"WAGE_PRODUCER_INVALID date=%d.%d.%d province_id=%d province_null=%s "
						"no_price=%g no_sold=%g no_wage=%g basic_price=%g basic_sold=%g basic_wage=%g "
						"high_price=%g high_sold=%g high_wage=%g guild_price=%g guild_sold=%g guild_wage=%g "
						"high_acc_price=%g high_acc_sold=%g high_acc_wage=%g "
						"dist_rgo=%g dist_primary_no=%g dist_primary_basic=%g dist_hna_no=%g dist_hna_basic=%g dist_hna_high=%g "
						"dist_ha_no=%g dist_ha_basic=%g dist_ha_high=%g dist_ha_high_acc=%g "
						"buf_rgo=%g buf_primary=%g buf_hna=%g buf_ha=%g\n",
						date.year, int(date.month), int(date.day),
						province.index(), province ? "false" : "true",
						lane_no_education_price, lane_no_education_sold, lane_no_education_wage,
						lane_basic_education_price, lane_basic_education_sold, lane_basic_education_wage,
						lane_high_education_price, lane_high_education_sold, lane_high_education_wage,
						lane_guild_education_price, lane_guild_education_sold, lane_guild_education_wage,
						lane_high_education_and_accepted_price, lane_high_education_and_accepted_sold, lane_high_education_and_accepted_wage,
						lane_rgo_worker_no_education, lane_primary_no_education, lane_primary_basic_education,
						lane_high_not_accepted_no_education, lane_high_not_accepted_basic_education, lane_high_not_accepted_high_education,
						lane_high_accepted_no_education, lane_high_accepted_basic_education, lane_high_accepted_high_education, lane_high_accepted_high_education_accepted,
						lane_rgo_workers_wage, lane_primary_workers_wage, lane_high_not_accepted_workers_wage, lane_high_accepted_workers_wage);
					wage_abort_after_log();
				}
			},
			pid,
			no_education_price, no_education_sold, no_education_wage,
			basic_education_price, basic_education_sold, basic_education_wage,
			high_education_price, high_education_sold, high_education_wage,
			guild_education_price, guild_education_sold, guild_education_wage,
			high_education_and_accepted_price, high_education_and_accepted_sold, high_education_and_accepted_wage,
			rgo_worker_no_education,
			primary_no_education, primary_basic_education,
			high_not_accepted_no_education, high_not_accepted_basic_education, high_not_accepted_high_education,
			high_accepted_no_education, high_accepted_basic_education, high_accepted_high_education, high_accepted_high_education_accepted,
			rgo_workers_wage, primary_workers_wage, high_not_accepted_workers_wage, high_accepted_workers_wage);
		}
#endif

		// RGOS and slaves cashback
		auto profit_from_slaves = ve::apply([&](dcon::province_id province, float earning_per_slave) {
			auto slaves_profit = 0.f;
			float payment_per_aristocrat = 0.f;
			for(auto pl : state.world.province_get_pop_location(province)) {
				if(pl.get_pop().get_poptype() == state.culture_definitions.slaves) {
					slaves_profit += pl.get_pop().get_size() * earning_per_slave;
				}
			}
			return slaves_profit;
		}, pid, rgo_workers_wage);
		auto old_rgo_cash = state.world.province_get_rgo_bank(pid);
		state.world.province_set_rgo_bank(pid, old_rgo_cash + profit_from_slaves);
	});

	state.world.execute_parallel_over_pop([&](auto pops) {
		auto savings = state.world.pop_get_savings(pops);
		auto pop_type = state.world.pop_get_poptype(pops);
		auto size = state.world.pop_get_size(pops);
		auto culture = state.world.pop_get_culture(pops);
		auto accepted = state.world.pop_get_is_primary_or_accepted_culture(pops);
		auto province = state.world.pop_get_province_from_pop_location(pops);

		auto high_education =
			(pop_type == state.culture_definitions.secondary_factory_worker)
			|| (pop_type == state.culture_definitions.bureaucrat)
			|| (pop_type == state.culture_definitions.clergy);

		auto wage_per_person = ve::select(
			state.world.pop_type_get_is_paid_rgo_worker(pop_type),
			buffer_rgo_workers_wage.get(province),
			ve::select(
				pop_type == state.culture_definitions.primary_factory_worker,
				buffer_primary_workers_wage.get(province),
				ve::select(
					accepted && high_education,
					buffer_high_accepted_workers_wage.get(province),
					ve::select(
						high_education,
						buffer_high_not_accepted_workers_wage.get(province),
						ve::fp_vector{0.f}
					)
				)
			)
		);
		auto wage = size * wage_per_person;
#ifndef NDEBUG
		if(wage_compare_enabled() || wage_debug_enabled()) {
			auto province_count = state.world.province_size();
			auto vector_accepted = ve::mask_vector(accepted);
			auto vector_high_education = ve::mask_vector(high_education);
			auto vector_rgo_paid = ve::mask_vector(state.world.pop_type_get_is_paid_rgo_worker(pop_type));
			auto vector_primary_worker = ve::mask_vector(pop_type == state.culture_definitions.primary_factory_worker);
			auto vector_high_accepted_buffer = ve::fp_vector(buffer_high_accepted_workers_wage.get(province));
			auto vector_high_not_accepted_buffer = ve::fp_vector(buffer_high_not_accepted_workers_wage.get(province));
			auto vector_primary_buffer = ve::fp_vector(buffer_primary_workers_wage.get(province));
			auto vector_rgo_buffer = ve::fp_vector(buffer_rgo_workers_wage.get(province));
			auto vector_pop_type_values = static_cast<ve::int_vector>(pop_type);
			ve::apply_with_indices([&](uint32_t lane,
				dcon::pop_id pop,
				float lane_selected_wage_per_person,
				float lane_wage,
				float lane_total,
				bool lane_vector_accepted,
				bool lane_vector_high_education,
				bool lane_vector_rgo_paid,
				bool lane_vector_primary_worker,
				int32_t lane_vector_pop_type_internal,
				float lane_vector_rgo_buffer,
				float lane_vector_primary_buffer,
				float lane_vector_high_accepted_buffer,
				float lane_vector_high_not_accepted_buffer) {
				auto lane_is_active = lane < wage_active_lanes(pops);
				auto lane_has_real_pop = lane_is_active && bool(pop);
				auto lane_savings = lane_has_real_pop ? state.world.pop_get_savings(pop) : 0.f;
				auto lane_pop_type = lane_has_real_pop ? state.world.pop_get_poptype(pop) : dcon::pop_type_id{};
				auto lane_culture = lane_has_real_pop ? state.world.pop_get_culture(pop) : dcon::culture_id{};
				auto lane_accepted = lane_has_real_pop ? state.world.pop_get_is_primary_or_accepted_culture(pop) : false;
				auto lane_province = lane_has_real_pop ? state.world.pop_get_province_from_pop_location(pop) : dcon::province_id{};
				auto lane_size = lane_has_real_pop ? state.world.pop_get_size(pop) : 0.f;
				auto lane_high_education =
					lane_pop_type == state.culture_definitions.secondary_factory_worker
					|| lane_pop_type == state.culture_definitions.bureaucrat
					|| lane_pop_type == state.culture_definitions.clergy;
				auto lane_rgo_paid = lane_has_real_pop ? state.world.pop_type_get_is_paid_rgo_worker(lane_pop_type) : false;
				auto lane_primary_worker = lane_pop_type == state.culture_definitions.primary_factory_worker;
				auto lane_accepted_high_education = lane_accepted && lane_high_education;
				auto lane_rgo_buffer = bool(lane_province) ? buffer_rgo_workers_wage.get(lane_province) : 0.f;
				auto lane_primary_buffer = bool(lane_province) ? buffer_primary_workers_wage.get(lane_province) : 0.f;
				auto lane_high_not_accepted_buffer = bool(lane_province) ? buffer_high_not_accepted_workers_wage.get(lane_province) : 0.f;
				auto lane_high_accepted_buffer = bool(lane_province) ? buffer_high_accepted_workers_wage.get(lane_province) : 0.f;
				if(lane_has_real_pop && wage_compare_enabled()) {
					auto scalar_total_wage = estimate_total_wage(state, pop);
					auto scalar_selected_wage_per_person = lane_size != 0.f ? scalar_total_wage / lane_size : 0.f;
					bool mismatch =
						!wage_value_matches(scalar_total_wage, lane_wage, 1e-5f * std::max(1.f, std::fabs(scalar_total_wage)))
						|| (std::isfinite(scalar_total_wage) != std::isfinite(lane_wage))
						|| ((scalar_total_wage < 0.f) != (lane_wage < 0.f));
					if(mismatch) {
						auto date = state.current_date.to_ymd(state.start_date);
						auto lane_vector_original_pop_type = pop_type[lane];
						auto lane_vector_original_index = lane_vector_original_pop_type.index();
						auto lane_vector_internal_index = lane_vector_pop_type_internal;
						auto lane_vector_original_byte = lane_vector_original_index >> 3;
						auto lane_vector_internal_byte = lane_vector_internal_index >> 3;
						auto lane_vector_original_bit = uint32_t(lane_vector_original_index) & 7u;
						auto lane_vector_internal_bit = uint32_t(lane_vector_internal_index) & 7u;
						auto paid_rgo_getter = [&](int32_t id_index) {
							return state.world.pop_type_get_is_paid_rgo_worker(dcon::pop_type_id(dcon::pop_type_id::value_base_t(id_index)));
						};
						auto lane_scalar_original_test = paid_rgo_getter(lane_vector_original_index);
						auto lane_scalar_internal_test = lane_vector_internal_index >= 0 ? paid_rgo_getter(lane_vector_internal_index) : false;
						auto lane_original_byte_value = debug_bitfield_byte(paid_rgo_getter, lane_vector_original_byte);
						auto lane_internal_byte_value = lane_vector_internal_index >= 0 ? debug_bitfield_byte(paid_rgo_getter, lane_vector_internal_byte) : uint8_t(0);
						std::fprintf(stderr,
							"WAGE_VECTOR_SCALAR_MISMATCH date=%d.%d.%d pop_id=%d lane=%u province_id=%d pop_type_id=%d size=%g "
							"scalar_total_wage=%g scalar_wage_per_person=%g vector_total_wage=%g vector_wage_per_person=%g "
							"rgo_paid=%s primary_worker=%s accepted=%s high_education=%s accepted_high_education=%s "
							"buffer_rgo=%g buffer_primary=%g buffer_high_not_accepted=%g buffer_high_accepted=%g "
							"vector_accepted=%s vector_high_education=%s vector_rgo_paid=%s vector_primary_worker=%s vector_pop_type_internal=%d "
							"vector_pop_type_original=%d original_test=%s internal_test=%s original_byte=%d original_bit=%u original_byte_value=0x%02x internal_byte=%d internal_bit=%u internal_byte_value=0x%02x "
							"vector_buffer_rgo=%g vector_buffer_primary=%g vector_buffer_high_not_accepted=%g vector_buffer_high_accepted=%g\n",
							date.year, int(date.month), int(date.day),
							pop.index(), lane, lane_province.index(), lane_pop_type.index(), lane_size,
							scalar_total_wage, scalar_selected_wage_per_person, lane_wage, lane_selected_wage_per_person,
							lane_rgo_paid ? "true" : "false",
							lane_primary_worker ? "true" : "false",
							lane_accepted ? "true" : "false",
							lane_high_education ? "true" : "false",
							lane_accepted_high_education ? "true" : "false",
							lane_rgo_buffer,
							lane_primary_buffer,
							lane_high_not_accepted_buffer,
							lane_high_accepted_buffer,
							lane_vector_accepted ? "true" : "false",
							lane_vector_high_education ? "true" : "false",
							lane_vector_rgo_paid ? "true" : "false",
							lane_vector_primary_worker ? "true" : "false",
							lane_vector_pop_type_internal,
							lane_vector_original_index,
							lane_scalar_original_test ? "true" : "false",
							lane_scalar_internal_test ? "true" : "false",
							lane_vector_original_byte,
							lane_vector_original_bit,
							lane_original_byte_value,
							lane_vector_internal_byte,
							lane_vector_internal_bit,
							lane_internal_byte_value,
							lane_vector_rgo_buffer,
							lane_vector_primary_buffer,
							lane_vector_high_not_accepted_buffer,
							lane_vector_high_accepted_buffer);
						wage_abort_after_log();
					}
				}

				if(!wage_debug_enabled()) {
					return;
				}

				if(std::isfinite(lane_total) && lane_total >= 0.f) {
					return;
				}

				auto date = state.current_date.to_ymd(state.start_date);
				char const* category = nullptr;
				if(!lane_is_active || !bool(pop)) {
					category = "F_INACTIVE_OR_NULL_LANE";
				} else if(!std::isfinite(lane_savings) || lane_savings < 0.f) {
					category = "A_SAVINGS_ALREADY_INVALID";
				} else if(!std::isfinite(lane_size) || lane_size < 0.f) {
					category = "B_SIZE_INVALID";
				} else if(!std::isfinite(lane_selected_wage_per_person)) {
					category = "C_SELECTED_WAGE_INVALID";
				} else if(
					!std::isfinite(lane_rgo_buffer) || lane_rgo_buffer < 0.f ||
					!std::isfinite(lane_primary_buffer) || lane_primary_buffer < 0.f ||
					!std::isfinite(lane_high_not_accepted_buffer) || lane_high_not_accepted_buffer < 0.f ||
					!std::isfinite(lane_high_accepted_buffer) || lane_high_accepted_buffer < 0.f
				) {
					category = "D_CANDIDATE_BUFFER_INVALID";
				} else if(std::isfinite(lane_selected_wage_per_person) && lane_selected_wage_per_person < 0.f) {
					category = "E_NEGATIVE_SELECTED_WAGE";
				} else {
					category = "G_FINITE_INPUTS_OVERFLOW";
				}

				float scalar_total_wage = lane_has_real_pop ? estimate_total_wage(state, pop) : 0.f;
				std::fprintf(stderr,
					"WAGE_INVALID_BEGIN\n"
					"category=%s\n"
					"date=%d.%d.%d\n"
					"lane=%u\n"
					"lane_is_active=%s\n"
					"pop_id=%d\n"
					"pop_raw_index=%d\n"
					"pop_is_null=%s\n"
					"province_id=%d\n"
					"province_is_null=%s\n"
					"province_in_range=%s\n"
					"pop_type_id=%d\n"
					"culture_id=%d\n"
					"accepted=%s\n"
					"pop_size=%g\n"
					"savings_before=%g\n"
					"high_education=%s\n"
					"paid_rgo_worker=%s\n"
					"primary_worker=%s\n"
					"accepted_high_education=%s\n"
					"selected_high_education_only=%s\n"
					"buffer_rgo_workers_wage=%g\n"
					"buffer_primary_workers_wage=%g\n"
					"buffer_high_not_accepted_workers_wage=%g\n"
					"buffer_high_accepted_workers_wage=%g\n"
					"selected_wage_per_person=%g\n"
					"final_wage=%g\n"
					"wage_plus_savings=%g\n"
					"scalar_estimate_total_wage=%g\n"
					"WAGE_INVALID_END\n",
					category,
					date.year, int(date.month), int(date.day),
					lane,
					lane_is_active ? "true" : "false",
					pop.index(),
					pop.index(),
					bool(pop) ? "false" : "true",
					lane_province.index(),
					bool(lane_province) ? "false" : "true",
					(bool(lane_province) && size_t(lane_province.index()) < province_count) ? "true" : "false",
					lane_pop_type.index(),
					lane_culture.index(),
					lane_accepted ? "true" : "false",
					lane_size,
					lane_savings,
					lane_high_education ? "true" : "false",
					lane_rgo_paid ? "true" : "false",
					lane_primary_worker ? "true" : "false",
					lane_accepted_high_education ? "true" : "false",
					(lane_high_education && !lane_accepted_high_education) ? "true" : "false",
					lane_rgo_buffer,
					lane_primary_buffer,
					lane_high_not_accepted_buffer,
					lane_high_accepted_buffer,
					lane_selected_wage_per_person,
					lane_wage,
					lane_total,
					scalar_total_wage);
				wage_abort_after_log();
			},
			pops,
			wage_per_person,
			wage,
			wage + savings,
			vector_accepted,
			vector_high_education,
			vector_rgo_paid,
			vector_primary_worker,
			vector_pop_type_values,
			vector_rgo_buffer,
			vector_primary_buffer,
			vector_high_accepted_buffer,
			vector_high_not_accepted_buffer);
		}
#endif
		state.world.pop_set_savings(pops, wage + savings);
#ifndef NDEBUG
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, wage + savings);
#endif // !NDEBUG
	});
}

float estimate_next_day_raw_income(
	sys::state const& state,
	dcon::pop_id pop
) {
	auto estimated =
		estimate_artisan_income(state, pop)
		+ estimate_slave_income(state, pop)
		+ estimate_trade_income(state, pop)
		+ estimate_total_wage(state, pop);

	auto from_nation = estimate_income_from_nation(state, pop);

	estimated +=
		from_nation.investment
		+ from_nation.military
		+ from_nation.pension
		+ from_nation.unemployment;

	return estimated;
}

float estimate_next_day_budget_before_taxes(
	sys::state const& state,
	dcon::pop_id pop
) {
	auto current = state.world.pop_get_savings(pop);
	current -= prepare_pop_budget(state, pop).spent_total;

	auto estimated = current
		+ estimate_artisan_income(state, pop)
		+ estimate_slave_income(state, pop)
		+ estimate_trade_income(state, pop)
		+ estimate_total_wage(state, pop);

	auto from_nation = estimate_income_from_nation(state, pop);

	estimated +=
		from_nation.investment
		+ from_nation.military
		+ from_nation.pension
		+ from_nation.unemployment;

	return estimated;
}

float estimate_trade_spending(
	sys::state const& state,
	dcon::pop_id pop
) {
	auto next_day = state.world.pop_get_savings(pop);
	return market_tax * next_day;
}

float estimate_tax_spending(
	sys::state const& state,
	dcon::pop_id pop,
	float tax_rate
) {
	auto next_day = estimate_next_day_raw_income(state, pop);
	return next_day * (1.f - market_tax) * tax_rate;
}

float estimate_pop_demand_internal_life(
	sys::state const& state, dcon::commodity_id c, dcon::pop_id pop,
	pops::vectorized_pops_budget<float>& budget,
	float mult_per_strata[3], float need_weight, float invention_factor
) {
	auto pop_type = state.world.pop_get_poptype(pop);
	auto strata = state.world.pop_type_get_strata(pop_type);
	auto pop_size = state.world.pop_get_size(pop);
	return budget.life_needs.demand_scale
		* budget.life_needs.satisfied_with_money_ratio
		* need_weight
		* mult_per_strata[strata]
		* state.defines.alice_lf_needs_scale
		* state.world.pop_type_get_life_needs(pop_type, c)
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
}
float estimate_pop_demand_internal_everyday(
	sys::state const& state, dcon::commodity_id c, dcon::pop_id pop,
	pops::vectorized_pops_budget<float>& budget,
	float mult_per_strata[3], float need_weight, float invention_factor
) {
	auto pop_type = state.world.pop_get_poptype(pop);
	auto strata = state.world.pop_type_get_strata(pop_type);
	auto pop_size = state.world.pop_get_size(pop);
	return budget.everyday_needs.demand_scale
		* budget.everyday_needs.satisfied_with_money_ratio
		* need_weight
		* mult_per_strata[strata]
		* state.defines.alice_ev_needs_scale
		* state.world.pop_type_get_everyday_needs(pop_type, c)
		* pop_size
		/ state.defines.alice_needs_scaling_factor
		* invention_factor;
}
float estimate_pop_demand_internal_luxury(
	sys::state const& state, dcon::commodity_id c, dcon::pop_id pop,
	pops::vectorized_pops_budget<float>& budget,
	float mult_per_strata[3], float need_weight, float invention_factor
) {
	auto pop_type = state.world.pop_get_poptype(pop);
	auto strata = state.world.pop_type_get_strata(pop_type);
	auto pop_size = state.world.pop_get_size(pop);
	return budget.luxury_needs.demand_scale
		* budget.luxury_needs.satisfied_with_money_ratio
		* need_weight
		* mult_per_strata[strata]
		* state.defines.alice_lx_needs_scale
		* state.world.pop_type_get_luxury_needs(pop_type, c)
		* pop_size
		/ state.defines.alice_needs_scaling_factor
		* invention_factor;
}

float estimate_pop_spending_life(sys::state const& state, dcon::pop_id pop, dcon::commodity_id cid) {
	auto pid = state.world.pop_get_province_from_pop_location(pop);
	auto nation = state.world.province_get_nation_from_province_ownership(pid);
	auto zone = state.world.province_get_state_membership(pid);
	auto market = state.world.state_instance_get_market_from_local_market(zone);
	auto budget = prepare_pop_budget(state, pop);
	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;
	auto weight = state.world.market_get_life_needs_weights(market, cid);
	float mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_life_needs) + 1.0f
	};
	auto demand = pops::estimate_pop_demand_internal_life(
		state, cid, pop, budget, mul, weight, invention_factor
	);
	auto actually_bought = state.world.market_get_actual_probability_to_buy(market, cid);
	auto cost = economy::price(state, market, cid);
	return demand * actually_bought * cost;
}

float estimate_pop_spending_everyday(sys::state const& state, dcon::pop_id pop, dcon::commodity_id cid) {
	auto pid = state.world.pop_get_province_from_pop_location(pop);
	auto nation = state.world.province_get_nation_from_province_ownership(pid);
	auto zone = state.world.province_get_state_membership(pid);
	auto market = state.world.state_instance_get_market_from_local_market(zone);
	auto budget = prepare_pop_budget(state, pop);
	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;
	auto weight = state.world.market_get_everyday_needs_weights(market, cid);
	float mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
	};
	auto demand = pops::estimate_pop_demand_internal_everyday(
		state, cid, pop, budget, mul, weight, invention_factor
	);
	auto actually_bought = state.world.market_get_actual_probability_to_buy(market, cid);
	auto cost = economy::price(state, market, cid);
	return demand * actually_bought * cost;
}

float estimate_pop_spending_luxury(sys::state const& state, dcon::pop_id pop, dcon::commodity_id cid) {
	auto pid = state.world.pop_get_province_from_pop_location(pop);
	auto nation = state.world.province_get_nation_from_province_ownership(pid);
	auto zone = state.world.province_get_state_membership(pid);
	auto market = state.world.state_instance_get_market_from_local_market(zone);
	auto budget = prepare_pop_budget(state, pop);
	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;
	auto weight = state.world.market_get_luxury_needs_weights(market, cid);
	float mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_luxury_needs) + 1.0f
	};
	auto demand = pops::estimate_pop_demand_internal_luxury(
		state, cid, pop, budget, mul, weight, invention_factor
	);
	auto actually_bought = state.world.market_get_actual_probability_to_buy(market, cid);
	auto cost = economy::price(state, market, cid);
	return demand * actually_bought * cost;
}

vectorized_pops_budget<float> prepare_pop_budget(const sys::state& state, dcon::pop_id ids) {
	return prepare_pop_budget_templated(state, ids);
}

}

float estimate_pops_consumption(sys::state const& state, dcon::commodity_id c, dcon::province_id p) {
	auto zone = state.world.province_get_state_membership(p);
	auto market = state.world.state_instance_get_market_from_local_market(zone);

	auto satisfaction = state.world.market_get_actual_probability_to_buy(market, c);

	auto nation = state.world.province_get_nation_from_province_ownership(p);

	auto weight_life = state.world.market_get_life_needs_weights(market, c);
	auto weight_everyday = state.world.market_get_everyday_needs_weights(market, c);
	auto weight_luxury = state.world.market_get_luxury_needs_weights(market, c);

	float life_mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_life_needs) + 1.0f
	};
	float everyday_mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
	};
	float luxury_mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_luxury_needs) + 1.0f,
	};

	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;

	float total = 0.f;
	state.world.province_for_each_pop_location(p, [&](auto location) {
		dcon::pop_id pop = state.world.pop_location_get_pop(location);

		auto pop_type = state.world.pop_get_poptype(pop);
		auto strata = state.world.pop_type_get_strata(pop_type);

		pops::vectorized_pops_budget<float> budget = pops::prepare_pop_budget(state, pop);

		auto consumption_life = pops::estimate_pop_demand_internal_life(
			state, c, pop, budget, life_mul, weight_life, invention_factor
		);
		auto consumption_everyday = pops::estimate_pop_demand_internal_everyday(
			state, c, pop, budget, everyday_mul, weight_everyday, invention_factor
		);
		auto consumption_luxury = pops::estimate_pop_demand_internal_luxury(
			state, c, pop, budget, luxury_mul, weight_luxury, invention_factor
		);

		total += consumption_life + consumption_everyday + consumption_luxury;
	});

	return total * satisfaction;
}
}
