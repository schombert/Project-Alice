#include <cstring>
#include "catch.hpp"
#include "parsers_declarations.hpp"
#include "dcon_generated.hpp"
#include "nations.hpp"
#include "container_types.hpp"
#include "system_state.hpp"
#include "serialization.hpp"
#include "prng.hpp"

TEST_CASE("prng_simple", "[determinism]") {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack
	game_state->game_seed = 64273;
	game_state->current_date.value = 49963;
	auto r1 = rng::get_random(*game_state, ~0x6a3f);
	auto r2 = rng::get_random(*game_state, ~0x6a3f);
	REQUIRE(r1 == r2);
}

TEST_CASE("prng_low_entropy", "[determinism]") {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack
	game_state->game_seed = 1;
	game_state->current_date.value = 1;
	auto r1 = rng::get_random(*game_state, 1);
	auto r2 = rng::get_random(*game_state, 1);
	REQUIRE(r1 == r2);
}

#define UNOPTIMIZABLE_FLOAT(name, value) \
	char name##_storage[sizeof(float)]; \
	new (&name##_storage) float(value); \
	const float *name##_p = std::launder(reinterpret_cast<const float*>(&name##_storage)); \
	const float name = *name##_p;

TEST_CASE("fp_mul", "[determinism]") {
	UNOPTIMIZABLE_FLOAT(f1, 2.f);
	UNOPTIMIZABLE_FLOAT(f2, 4.f);
	REQUIRE(f1 * f2 == 8.f);
}
TEST_CASE("fp_fmadd", "[determinism]") {
	UNOPTIMIZABLE_FLOAT(f1, 2.f);
	UNOPTIMIZABLE_FLOAT(f2, 4.f);
	UNOPTIMIZABLE_FLOAT(f3, 3.f);
	UNOPTIMIZABLE_FLOAT(f4, 2.f);
	
	float fmadd_1 = f1 * f2 + f3 * f4; //equation
	float fmadd_2 = f3 * f4 + f1 * f2; //reverse products
	float fmadd_3 = f1 * (f2 + (f3 * f4) / f1); //factorization by f1
	float fmadd_4 = f2 * (f1 + (f3 * f4) / f2); //factorization by f2
	float fmadd_5 = f3 * ((f1 * f2) / f3 + f4); //factorization by f3
	float fmadd_6 = f4 * ((f1 * f2) / f4 + f3); //factorization by f4

	REQUIRE(fmadd_1 == fmadd_2);
	REQUIRE(fmadd_1 == fmadd_3);
	REQUIRE(fmadd_1 == fmadd_4);
	REQUIRE(fmadd_1 == fmadd_5);
	REQUIRE(fmadd_1 == fmadd_6);
}

TEST_CASE("fp_fprec", "[determinism]") {
	UNOPTIMIZABLE_FLOAT(f1, 2.f);
	UNOPTIMIZABLE_FLOAT(f2, 4.f);
	UNOPTIMIZABLE_FLOAT(f3, 3.f);
	
	float fprec_1 = f1 * f2 / f3; //equation
	float fprec_2 = (f1 * f2) / f3; //same
	float fprec_3 = f1 * (f2 / f3); //same
	float fprec_4 = (f1 / f3) * f2; //same

	REQUIRE(fprec_1 == fprec_2);
	REQUIRE(fprec_1 == fprec_3);
	REQUIRE(fprec_1 == fprec_4);
}

TEST_CASE("math_fns", "[determinism]") {
	for(float k = 1.f; k <= 128.f; k += 1.f) {
		UNOPTIMIZABLE_FLOAT(f1, k);
		REQUIRE(math::sqrt(f1) == Approx(std::sqrt(f1)).margin(0.01f));
	}
	for(float k = -1.f; k <= 1.f; k += 0.01f) {
		UNOPTIMIZABLE_FLOAT(f1, k);
		REQUIRE(math::acos(f1) == Approx(std::acos(f1)).margin(0.018f));
	}
	for(float k = -math::pi; k <= math::pi; k += 0.01f) {
		UNOPTIMIZABLE_FLOAT(f1, k);
		REQUIRE(math::cos(f1) == Approx(std::cos(f1)).margin(0.01f));
		REQUIRE(math::sin(f1) == Approx(std::sin(f1)).margin(0.01f));
	}
}

#undef UNOPTIMIZABLE_FLOAT

void compare_game_states(sys::state& ws1, sys::state& ws2) {	// obj 
	// obj national_identity
	REQUIRE(std::memcmp(&ws1.world.national_identity.m_color.values, &ws2.world.national_identity.m_color.values, sizeof(ws1.world.national_identity.m_color.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_identity.m_name.values, &ws2.world.national_identity.m_name.values, sizeof(ws1.world.national_identity.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_identity.m_adjective.values, &ws2.world.national_identity.m_adjective.values, sizeof(ws1.world.national_identity.m_adjective.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_identity.m_identifying_int.values, &ws2.world.national_identity.m_identifying_int.values, sizeof(ws1.world.national_identity.m_identifying_int.values)) == 0);
	REQUIRE(ws1.world.national_identity.m_unit_names_first.size == ws2.world.national_identity.m_unit_names_first.size);
	REQUIRE(std::memcmp(ws1.world.national_identity.m_unit_names_first.values, ws2.world.national_identity.m_unit_names_first.values, sizeof(decltype(ws1.world.national_identity.m_unit_names_first.vptr(0))) * ws1.world.national_identity.m_unit_names_first.size) == 0);
	REQUIRE(ws1.world.national_identity.m_unit_names_count.size == ws2.world.national_identity.m_unit_names_count.size);
	REQUIRE(std::memcmp(ws1.world.national_identity.m_unit_names_count.values, ws2.world.national_identity.m_unit_names_count.values, sizeof(decltype(ws1.world.national_identity.m_unit_names_count.vptr(0))) * ws1.world.national_identity.m_unit_names_count.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_identity.m_political_party_first.values, &ws2.world.national_identity.m_political_party_first.values, sizeof(ws1.world.national_identity.m_political_party_first.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_identity.m_political_party_count.values, &ws2.world.national_identity.m_political_party_count.values, sizeof(ws1.world.national_identity.m_political_party_count.values)) == 0);
	REQUIRE(ws1.world.national_identity.m_government_flag_type.size == ws2.world.national_identity.m_government_flag_type.size);
	REQUIRE(std::memcmp(ws1.world.national_identity.m_government_flag_type.values, ws2.world.national_identity.m_government_flag_type.values, sizeof(decltype(ws1.world.national_identity.m_government_flag_type.vptr(0))) * ws1.world.national_identity.m_government_flag_type.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_identity.m_primary_culture.values, &ws2.world.national_identity.m_primary_culture.values, sizeof(ws1.world.national_identity.m_primary_culture.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_identity.m_religion.values, &ws2.world.national_identity.m_religion.values, sizeof(ws1.world.national_identity.m_religion.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_identity.m_capital.values, &ws2.world.national_identity.m_capital.values, sizeof(ws1.world.national_identity.m_capital.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_identity.m_is_not_releasable.values, &ws2.world.national_identity.m_is_not_releasable.values, sizeof(ws1.world.national_identity.m_is_not_releasable.values)) == 0);
	// obj political_party
	REQUIRE(std::memcmp(&ws1.world.political_party.m_name.values, &ws2.world.political_party.m_name.values, sizeof(ws1.world.political_party.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.political_party.m_start_date.values, &ws2.world.political_party.m_start_date.values, sizeof(ws1.world.political_party.m_start_date.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.political_party.m_end_date.values, &ws2.world.political_party.m_end_date.values, sizeof(ws1.world.political_party.m_end_date.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.political_party.m_ideology.values, &ws2.world.political_party.m_ideology.values, sizeof(ws1.world.political_party.m_ideology.values)) == 0);
	REQUIRE(ws1.world.political_party.m_party_issues.size == ws2.world.political_party.m_party_issues.size);
	REQUIRE(std::memcmp(ws1.world.political_party.m_party_issues.values, ws2.world.political_party.m_party_issues.values, sizeof(decltype(ws1.world.political_party.m_party_issues.vptr(0))) * ws1.world.political_party.m_party_issues.size) == 0);
	// obj religion
	REQUIRE(std::memcmp(&ws1.world.religion.m_name.values, &ws2.world.religion.m_name.values, sizeof(ws1.world.religion.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.religion.m_color.values, &ws2.world.religion.m_color.values, sizeof(ws1.world.religion.m_color.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.religion.m_icon.values, &ws2.world.religion.m_icon.values, sizeof(ws1.world.religion.m_icon.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.religion.m_is_pagan.values, &ws2.world.religion.m_is_pagan.values, sizeof(ws1.world.religion.m_is_pagan.values)) == 0);
	// obj culture_group
	REQUIRE(std::memcmp(&ws1.world.culture_group.m_name.values, &ws2.world.culture_group.m_name.values, sizeof(ws1.world.culture_group.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.culture_group.m_is_overseas.values, &ws2.world.culture_group.m_is_overseas.values, sizeof(ws1.world.culture_group.m_is_overseas.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.culture_group.m_leader.values, &ws2.world.culture_group.m_leader.values, sizeof(ws1.world.culture_group.m_leader.values)) == 0);
	// obj culture
	REQUIRE(std::memcmp(&ws1.world.culture.m_name.values, &ws2.world.culture.m_name.values, sizeof(ws1.world.culture.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.culture.m_color.values, &ws2.world.culture.m_color.values, sizeof(ws1.world.culture.m_color.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.culture.m_radicalism.values, &ws2.world.culture.m_radicalism.values, sizeof(ws1.world.culture.m_radicalism.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.culture.m_first_names.values, &ws2.world.culture.m_first_names.values, sizeof(ws1.world.culture.m_first_names.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.culture.m_last_names.values, &ws2.world.culture.m_last_names.values, sizeof(ws1.world.culture.m_last_names.values)) == 0);
	// obj culture_group_membership
	// culture_group_membership.member is unique
	REQUIRE(std::memcmp(&ws1.world.culture_group_membership.m_group.values, &ws2.world.culture_group_membership.m_group.values, sizeof(ws1.world.culture_group_membership.m_group.values)) == 0);
	// obj cultural_union_of
	// cultural_union_of.identity is unique
	// cultural_union_of.culture_group is unique
	// obj commodity
	REQUIRE(std::memcmp(&ws1.world.commodity.m_name.values, &ws2.world.commodity.m_name.values, sizeof(ws1.world.commodity.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_color.values, &ws2.world.commodity.m_color.values, sizeof(ws1.world.commodity.m_color.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_cost.values, &ws2.world.commodity.m_cost.values, sizeof(ws1.world.commodity.m_cost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_commodity_group.values, &ws2.world.commodity.m_commodity_group.values, sizeof(ws1.world.commodity.m_commodity_group.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_is_available_from_start.values, &ws2.world.commodity.m_is_available_from_start.values, sizeof(ws1.world.commodity.m_is_available_from_start.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_is_mine.values, &ws2.world.commodity.m_is_mine.values, sizeof(ws1.world.commodity.m_is_mine.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_overseas_penalty.values, &ws2.world.commodity.m_overseas_penalty.values, sizeof(ws1.world.commodity.m_overseas_penalty.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_rgo_amount.values, &ws2.world.commodity.m_rgo_amount.values, sizeof(ws1.world.commodity.m_rgo_amount.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_rgo_workforce.values, &ws2.world.commodity.m_rgo_workforce.values, sizeof(ws1.world.commodity.m_rgo_workforce.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_artisan_inputs.values, &ws2.world.commodity.m_artisan_inputs.values, sizeof(ws1.world.commodity.m_artisan_inputs.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_artisan_output_amount.values, &ws2.world.commodity.m_artisan_output_amount.values, sizeof(ws1.world.commodity.m_artisan_output_amount.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_icon.values, &ws2.world.commodity.m_icon.values, sizeof(ws1.world.commodity.m_icon.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_key_factory.values, &ws2.world.commodity.m_key_factory.values, sizeof(ws1.world.commodity.m_key_factory.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_current_price.values, &ws2.world.commodity.m_current_price.values, sizeof(ws1.world.commodity.m_current_price.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_global_market_pool.values, &ws2.world.commodity.m_global_market_pool.values, sizeof(ws1.world.commodity.m_global_market_pool.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_total_production.values, &ws2.world.commodity.m_total_production.values, sizeof(ws1.world.commodity.m_total_production.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_total_real_demand.values, &ws2.world.commodity.m_total_real_demand.values, sizeof(ws1.world.commodity.m_total_real_demand.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_total_consumption.values, &ws2.world.commodity.m_total_consumption.values, sizeof(ws1.world.commodity.m_total_consumption.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.commodity.m_producer_payout_fraction.values, &ws2.world.commodity.m_producer_payout_fraction.values, sizeof(ws1.world.commodity.m_producer_payout_fraction.values)) == 0);
	REQUIRE(ws1.world.commodity.m_price_record.size == ws2.world.commodity.m_price_record.size);
	REQUIRE(std::memcmp(ws1.world.commodity.m_price_record.values, ws2.world.commodity.m_price_record.values, sizeof(decltype(ws1.world.commodity.m_price_record.vptr(0))) * ws1.world.commodity.m_price_record.size) == 0);
	// obj modifier
	REQUIRE(std::memcmp(&ws1.world.modifier.m_name.values, &ws2.world.modifier.m_name.values, sizeof(ws1.world.modifier.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.modifier.m_province_values.values, &ws2.world.modifier.m_province_values.values, sizeof(ws1.world.modifier.m_province_values.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.modifier.m_national_values.values, &ws2.world.modifier.m_national_values.values, sizeof(ws1.world.modifier.m_national_values.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.modifier.m_icon.values, &ws2.world.modifier.m_icon.values, sizeof(ws1.world.modifier.m_icon.values)) == 0);
	// obj factory_type
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_name.values, &ws2.world.factory_type.m_name.values, sizeof(ws1.world.factory_type.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_description.values, &ws2.world.factory_type.m_description.values, sizeof(ws1.world.factory_type.m_description.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_construction_costs.values, &ws2.world.factory_type.m_construction_costs.values, sizeof(ws1.world.factory_type.m_construction_costs.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_construction_time.values, &ws2.world.factory_type.m_construction_time.values, sizeof(ws1.world.factory_type.m_construction_time.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_is_available_from_start.values, &ws2.world.factory_type.m_is_available_from_start.values, sizeof(ws1.world.factory_type.m_is_available_from_start.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_is_coastal.values, &ws2.world.factory_type.m_is_coastal.values, sizeof(ws1.world.factory_type.m_is_coastal.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_inputs.values, &ws2.world.factory_type.m_inputs.values, sizeof(ws1.world.factory_type.m_inputs.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_efficiency_inputs.values, &ws2.world.factory_type.m_efficiency_inputs.values, sizeof(ws1.world.factory_type.m_efficiency_inputs.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_base_workforce.values, &ws2.world.factory_type.m_base_workforce.values, sizeof(ws1.world.factory_type.m_base_workforce.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_output.values, &ws2.world.factory_type.m_output.values, sizeof(ws1.world.factory_type.m_output.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_output_amount.values, &ws2.world.factory_type.m_output_amount.values, sizeof(ws1.world.factory_type.m_output_amount.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_bonus_1_trigger.values, &ws2.world.factory_type.m_bonus_1_trigger.values, sizeof(ws1.world.factory_type.m_bonus_1_trigger.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_bonus_1_amount.values, &ws2.world.factory_type.m_bonus_1_amount.values, sizeof(ws1.world.factory_type.m_bonus_1_amount.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_bonus_2_trigger.values, &ws2.world.factory_type.m_bonus_2_trigger.values, sizeof(ws1.world.factory_type.m_bonus_2_trigger.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_bonus_2_amount.values, &ws2.world.factory_type.m_bonus_2_amount.values, sizeof(ws1.world.factory_type.m_bonus_2_amount.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_bonus_3_trigger.values, &ws2.world.factory_type.m_bonus_3_trigger.values, sizeof(ws1.world.factory_type.m_bonus_3_trigger.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory_type.m_bonus_3_amount.values, &ws2.world.factory_type.m_bonus_3_amount.values, sizeof(ws1.world.factory_type.m_bonus_3_amount.values)) == 0);
	// obj ideology_group
	REQUIRE(std::memcmp(&ws1.world.ideology_group.m_name.values, &ws2.world.ideology_group.m_name.values, sizeof(ws1.world.ideology_group.m_name.values)) == 0);
	// obj ideology
	REQUIRE(std::memcmp(&ws1.world.ideology.m_name.values, &ws2.world.ideology.m_name.values, sizeof(ws1.world.ideology.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ideology.m_color.values, &ws2.world.ideology.m_color.values, sizeof(ws1.world.ideology.m_color.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ideology.m_is_civilized_only.values, &ws2.world.ideology.m_is_civilized_only.values, sizeof(ws1.world.ideology.m_is_civilized_only.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ideology.m_activation_date.values, &ws2.world.ideology.m_activation_date.values, sizeof(ws1.world.ideology.m_activation_date.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ideology.m_add_political_reform.values, &ws2.world.ideology.m_add_political_reform.values, sizeof(ws1.world.ideology.m_add_political_reform.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ideology.m_remove_political_reform.values, &ws2.world.ideology.m_remove_political_reform.values, sizeof(ws1.world.ideology.m_remove_political_reform.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ideology.m_add_social_reform.values, &ws2.world.ideology.m_add_social_reform.values, sizeof(ws1.world.ideology.m_add_social_reform.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ideology.m_remove_social_reform.values, &ws2.world.ideology.m_remove_social_reform.values, sizeof(ws1.world.ideology.m_remove_social_reform.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ideology.m_add_military_reform.values, &ws2.world.ideology.m_add_military_reform.values, sizeof(ws1.world.ideology.m_add_military_reform.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ideology.m_add_economic_reform.values, &ws2.world.ideology.m_add_economic_reform.values, sizeof(ws1.world.ideology.m_add_economic_reform.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ideology.m_enabled.values, &ws2.world.ideology.m_enabled.values, sizeof(ws1.world.ideology.m_enabled.values)) == 0);
	// obj ideology_group_membership
	// ideology_group_membership.ideology is unique
	REQUIRE(std::memcmp(&ws1.world.ideology_group_membership.m_ideology_group.values, &ws2.world.ideology_group_membership.m_ideology_group.values, sizeof(ws1.world.ideology_group_membership.m_ideology_group.values)) == 0);
	// obj issue
	REQUIRE(std::memcmp(&ws1.world.issue.m_name.values, &ws2.world.issue.m_name.values, sizeof(ws1.world.issue.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue.m_options.values, &ws2.world.issue.m_options.values, sizeof(ws1.world.issue.m_options.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue.m_is_next_step_only.values, &ws2.world.issue.m_is_next_step_only.values, sizeof(ws1.world.issue.m_is_next_step_only.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue.m_is_administrative.values, &ws2.world.issue.m_is_administrative.values, sizeof(ws1.world.issue.m_is_administrative.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue.m_issue_type.values, &ws2.world.issue.m_issue_type.values, sizeof(ws1.world.issue.m_issue_type.values)) == 0);
	// obj issue_option
	REQUIRE(std::memcmp(&ws1.world.issue_option.m_name.values, &ws2.world.issue_option.m_name.values, sizeof(ws1.world.issue_option.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue_option.m_movement_name.values, &ws2.world.issue_option.m_movement_name.values, sizeof(ws1.world.issue_option.m_movement_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue_option.m_rules.values, &ws2.world.issue_option.m_rules.values, sizeof(ws1.world.issue_option.m_rules.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue_option.m_modifier.values, &ws2.world.issue_option.m_modifier.values, sizeof(ws1.world.issue_option.m_modifier.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue_option.m_war_exhaustion_effect.values, &ws2.world.issue_option.m_war_exhaustion_effect.values, sizeof(ws1.world.issue_option.m_war_exhaustion_effect.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue_option.m_administrative_multiplier.values, &ws2.world.issue_option.m_administrative_multiplier.values, sizeof(ws1.world.issue_option.m_administrative_multiplier.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue_option.m_allow.values, &ws2.world.issue_option.m_allow.values, sizeof(ws1.world.issue_option.m_allow.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue_option.m_on_execute_trigger.values, &ws2.world.issue_option.m_on_execute_trigger.values, sizeof(ws1.world.issue_option.m_on_execute_trigger.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue_option.m_on_execute_effect.values, &ws2.world.issue_option.m_on_execute_effect.values, sizeof(ws1.world.issue_option.m_on_execute_effect.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.issue_option.m_parent_issue.values, &ws2.world.issue_option.m_parent_issue.values, sizeof(ws1.world.issue_option.m_parent_issue.values)) == 0);
	// obj reform
	REQUIRE(std::memcmp(&ws1.world.reform.m_name.values, &ws2.world.reform.m_name.values, sizeof(ws1.world.reform.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.reform.m_options.values, &ws2.world.reform.m_options.values, sizeof(ws1.world.reform.m_options.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.reform.m_is_next_step_only.values, &ws2.world.reform.m_is_next_step_only.values, sizeof(ws1.world.reform.m_is_next_step_only.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.reform.m_reform_type.values, &ws2.world.reform.m_reform_type.values, sizeof(ws1.world.reform.m_reform_type.values)) == 0);
	// obj reform_option
	REQUIRE(std::memcmp(&ws1.world.reform_option.m_name.values, &ws2.world.reform_option.m_name.values, sizeof(ws1.world.reform_option.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.reform_option.m_rules.values, &ws2.world.reform_option.m_rules.values, sizeof(ws1.world.reform_option.m_rules.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.reform_option.m_modifier.values, &ws2.world.reform_option.m_modifier.values, sizeof(ws1.world.reform_option.m_modifier.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.reform_option.m_technology_cost.values, &ws2.world.reform_option.m_technology_cost.values, sizeof(ws1.world.reform_option.m_technology_cost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.reform_option.m_war_exhaustion_effect.values, &ws2.world.reform_option.m_war_exhaustion_effect.values, sizeof(ws1.world.reform_option.m_war_exhaustion_effect.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.reform_option.m_allow.values, &ws2.world.reform_option.m_allow.values, sizeof(ws1.world.reform_option.m_allow.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.reform_option.m_on_execute_trigger.values, &ws2.world.reform_option.m_on_execute_trigger.values, sizeof(ws1.world.reform_option.m_on_execute_trigger.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.reform_option.m_on_execute_effect.values, &ws2.world.reform_option.m_on_execute_effect.values, sizeof(ws1.world.reform_option.m_on_execute_effect.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.reform_option.m_parent_reform.values, &ws2.world.reform_option.m_parent_reform.values, sizeof(ws1.world.reform_option.m_parent_reform.values)) == 0);
	// obj cb_type
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_name.values, &ws2.world.cb_type.m_name.values, sizeof(ws1.world.cb_type.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_short_desc.values, &ws2.world.cb_type.m_short_desc.values, sizeof(ws1.world.cb_type.m_short_desc.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_shortest_desc.values, &ws2.world.cb_type.m_shortest_desc.values, sizeof(ws1.world.cb_type.m_shortest_desc.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_long_desc.values, &ws2.world.cb_type.m_long_desc.values, sizeof(ws1.world.cb_type.m_long_desc.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_war_name.values, &ws2.world.cb_type.m_war_name.values, sizeof(ws1.world.cb_type.m_war_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_type_bits.values, &ws2.world.cb_type.m_type_bits.values, sizeof(ws1.world.cb_type.m_type_bits.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_months.values, &ws2.world.cb_type.m_months.values, sizeof(ws1.world.cb_type.m_months.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_truce_months.values, &ws2.world.cb_type.m_truce_months.values, sizeof(ws1.world.cb_type.m_truce_months.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_sprite_index.values, &ws2.world.cb_type.m_sprite_index.values, sizeof(ws1.world.cb_type.m_sprite_index.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_allowed_states.values, &ws2.world.cb_type.m_allowed_states.values, sizeof(ws1.world.cb_type.m_allowed_states.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_allowed_states_in_crisis.values, &ws2.world.cb_type.m_allowed_states_in_crisis.values, sizeof(ws1.world.cb_type.m_allowed_states_in_crisis.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_allowed_substate_regions.values, &ws2.world.cb_type.m_allowed_substate_regions.values, sizeof(ws1.world.cb_type.m_allowed_substate_regions.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_allowed_countries.values, &ws2.world.cb_type.m_allowed_countries.values, sizeof(ws1.world.cb_type.m_allowed_countries.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_can_use.values, &ws2.world.cb_type.m_can_use.values, sizeof(ws1.world.cb_type.m_can_use.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_on_add.values, &ws2.world.cb_type.m_on_add.values, sizeof(ws1.world.cb_type.m_on_add.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_on_po_accepted.values, &ws2.world.cb_type.m_on_po_accepted.values, sizeof(ws1.world.cb_type.m_on_po_accepted.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_badboy_factor.values, &ws2.world.cb_type.m_badboy_factor.values, sizeof(ws1.world.cb_type.m_badboy_factor.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_prestige_factor.values, &ws2.world.cb_type.m_prestige_factor.values, sizeof(ws1.world.cb_type.m_prestige_factor.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_peace_cost_factor.values, &ws2.world.cb_type.m_peace_cost_factor.values, sizeof(ws1.world.cb_type.m_peace_cost_factor.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_penalty_factor.values, &ws2.world.cb_type.m_penalty_factor.values, sizeof(ws1.world.cb_type.m_penalty_factor.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_break_truce_prestige_factor.values, &ws2.world.cb_type.m_break_truce_prestige_factor.values, sizeof(ws1.world.cb_type.m_break_truce_prestige_factor.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_break_truce_infamy_factor.values, &ws2.world.cb_type.m_break_truce_infamy_factor.values, sizeof(ws1.world.cb_type.m_break_truce_infamy_factor.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_break_truce_militancy_factor.values, &ws2.world.cb_type.m_break_truce_militancy_factor.values, sizeof(ws1.world.cb_type.m_break_truce_militancy_factor.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_good_relation_prestige_factor.values, &ws2.world.cb_type.m_good_relation_prestige_factor.values, sizeof(ws1.world.cb_type.m_good_relation_prestige_factor.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_good_relation_infamy_factor.values, &ws2.world.cb_type.m_good_relation_infamy_factor.values, sizeof(ws1.world.cb_type.m_good_relation_infamy_factor.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_good_relation_militancy_factor.values, &ws2.world.cb_type.m_good_relation_militancy_factor.values, sizeof(ws1.world.cb_type.m_good_relation_militancy_factor.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_construction_speed.values, &ws2.world.cb_type.m_construction_speed.values, sizeof(ws1.world.cb_type.m_construction_speed.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.cb_type.m_tws_battle_factor.values, &ws2.world.cb_type.m_tws_battle_factor.values, sizeof(ws1.world.cb_type.m_tws_battle_factor.values)) == 0);
	// obj leader_trait
	REQUIRE(std::memcmp(&ws1.world.leader_trait.m_name.values, &ws2.world.leader_trait.m_name.values, sizeof(ws1.world.leader_trait.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader_trait.m_organisation.values, &ws2.world.leader_trait.m_organisation.values, sizeof(ws1.world.leader_trait.m_organisation.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader_trait.m_morale.values, &ws2.world.leader_trait.m_morale.values, sizeof(ws1.world.leader_trait.m_morale.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader_trait.m_attack.values, &ws2.world.leader_trait.m_attack.values, sizeof(ws1.world.leader_trait.m_attack.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader_trait.m_defense.values, &ws2.world.leader_trait.m_defense.values, sizeof(ws1.world.leader_trait.m_defense.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader_trait.m_reconnaissance.values, &ws2.world.leader_trait.m_reconnaissance.values, sizeof(ws1.world.leader_trait.m_reconnaissance.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader_trait.m_speed.values, &ws2.world.leader_trait.m_speed.values, sizeof(ws1.world.leader_trait.m_speed.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader_trait.m_experience.values, &ws2.world.leader_trait.m_experience.values, sizeof(ws1.world.leader_trait.m_experience.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader_trait.m_reliability.values, &ws2.world.leader_trait.m_reliability.values, sizeof(ws1.world.leader_trait.m_reliability.values)) == 0);
	// obj pop_type
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_name.values, &ws2.world.pop_type.m_name.values, sizeof(ws1.world.pop_type.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_sprite.values, &ws2.world.pop_type.m_sprite.values, sizeof(ws1.world.pop_type.m_sprite.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_color.values, &ws2.world.pop_type.m_color.values, sizeof(ws1.world.pop_type.m_color.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_strata.values, &ws2.world.pop_type.m_strata.values, sizeof(ws1.world.pop_type.m_strata.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_state_capital_only.values, &ws2.world.pop_type.m_state_capital_only.values, sizeof(ws1.world.pop_type.m_state_capital_only.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_voting_forbidden.values, &ws2.world.pop_type.m_voting_forbidden.values, sizeof(ws1.world.pop_type.m_voting_forbidden.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_everyday_needs_income_type.values, &ws2.world.pop_type.m_everyday_needs_income_type.values, sizeof(ws1.world.pop_type.m_everyday_needs_income_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_luxury_needs_income_type.values, &ws2.world.pop_type.m_luxury_needs_income_type.values, sizeof(ws1.world.pop_type.m_luxury_needs_income_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_life_needs_income_type.values, &ws2.world.pop_type.m_life_needs_income_type.values, sizeof(ws1.world.pop_type.m_life_needs_income_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_research_optimum.values, &ws2.world.pop_type.m_research_optimum.values, sizeof(ws1.world.pop_type.m_research_optimum.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_research_points.values, &ws2.world.pop_type.m_research_points.values, sizeof(ws1.world.pop_type.m_research_points.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_has_unemployment.values, &ws2.world.pop_type.m_has_unemployment.values, sizeof(ws1.world.pop_type.m_has_unemployment.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_migration_target.values, &ws2.world.pop_type.m_migration_target.values, sizeof(ws1.world.pop_type.m_migration_target.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop_type.m_country_migration_target.values, &ws2.world.pop_type.m_country_migration_target.values, sizeof(ws1.world.pop_type.m_country_migration_target.values)) == 0);
	REQUIRE(ws1.world.pop_type.m_issues.size == ws2.world.pop_type.m_issues.size);
	REQUIRE(std::memcmp(ws1.world.pop_type.m_issues.values, ws2.world.pop_type.m_issues.values, sizeof(decltype(ws1.world.pop_type.m_issues.vptr(0))) * ws1.world.pop_type.m_issues.size) == 0);
	REQUIRE(ws1.world.pop_type.m_ideology.size == ws2.world.pop_type.m_ideology.size);
	REQUIRE(std::memcmp(ws1.world.pop_type.m_ideology.values, ws2.world.pop_type.m_ideology.values, sizeof(decltype(ws1.world.pop_type.m_ideology.vptr(0))) * ws1.world.pop_type.m_ideology.size) == 0);
	REQUIRE(ws1.world.pop_type.m_promotion.size == ws2.world.pop_type.m_promotion.size);
	REQUIRE(std::memcmp(ws1.world.pop_type.m_promotion.values, ws2.world.pop_type.m_promotion.values, sizeof(decltype(ws1.world.pop_type.m_promotion.vptr(0))) * ws1.world.pop_type.m_promotion.size) == 0);
	REQUIRE(ws1.world.pop_type.m_life_needs.size == ws2.world.pop_type.m_life_needs.size);
	REQUIRE(std::memcmp(ws1.world.pop_type.m_life_needs.values, ws2.world.pop_type.m_life_needs.values, sizeof(decltype(ws1.world.pop_type.m_life_needs.vptr(0))) * ws1.world.pop_type.m_life_needs.size) == 0);
	REQUIRE(ws1.world.pop_type.m_everyday_needs.size == ws2.world.pop_type.m_everyday_needs.size);
	REQUIRE(std::memcmp(ws1.world.pop_type.m_everyday_needs.values, ws2.world.pop_type.m_everyday_needs.values, sizeof(decltype(ws1.world.pop_type.m_everyday_needs.vptr(0))) * ws1.world.pop_type.m_everyday_needs.size) == 0);
	REQUIRE(ws1.world.pop_type.m_luxury_needs.size == ws2.world.pop_type.m_luxury_needs.size);
	REQUIRE(std::memcmp(ws1.world.pop_type.m_luxury_needs.values, ws2.world.pop_type.m_luxury_needs.values, sizeof(decltype(ws1.world.pop_type.m_luxury_needs.vptr(0))) * ws1.world.pop_type.m_luxury_needs.size) == 0);
	// obj rebel_type
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_name.values, &ws2.world.rebel_type.m_name.values, sizeof(ws1.world.rebel_type.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_title.values, &ws2.world.rebel_type.m_title.values, sizeof(ws1.world.rebel_type.m_title.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_description.values, &ws2.world.rebel_type.m_description.values, sizeof(ws1.world.rebel_type.m_description.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_army_name.values, &ws2.world.rebel_type.m_army_name.values, sizeof(ws1.world.rebel_type.m_army_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_icon.values, &ws2.world.rebel_type.m_icon.values, sizeof(ws1.world.rebel_type.m_icon.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_break_alliance_on_win.values, &ws2.world.rebel_type.m_break_alliance_on_win.values, sizeof(ws1.world.rebel_type.m_break_alliance_on_win.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_area.values, &ws2.world.rebel_type.m_area.values, sizeof(ws1.world.rebel_type.m_area.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_defection.values, &ws2.world.rebel_type.m_defection.values, sizeof(ws1.world.rebel_type.m_defection.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_independence.values, &ws2.world.rebel_type.m_independence.values, sizeof(ws1.world.rebel_type.m_independence.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_ideology.values, &ws2.world.rebel_type.m_ideology.values, sizeof(ws1.world.rebel_type.m_ideology.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_occupation_multiplier.values, &ws2.world.rebel_type.m_occupation_multiplier.values, sizeof(ws1.world.rebel_type.m_occupation_multiplier.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_defect_delay.values, &ws2.world.rebel_type.m_defect_delay.values, sizeof(ws1.world.rebel_type.m_defect_delay.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_culture_restriction.values, &ws2.world.rebel_type.m_culture_restriction.values, sizeof(ws1.world.rebel_type.m_culture_restriction.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_culture_group_restriction.values, &ws2.world.rebel_type.m_culture_group_restriction.values, sizeof(ws1.world.rebel_type.m_culture_group_restriction.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_will_rise.values, &ws2.world.rebel_type.m_will_rise.values, sizeof(ws1.world.rebel_type.m_will_rise.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_spawn_chance.values, &ws2.world.rebel_type.m_spawn_chance.values, sizeof(ws1.world.rebel_type.m_spawn_chance.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_movement_evaluation.values, &ws2.world.rebel_type.m_movement_evaluation.values, sizeof(ws1.world.rebel_type.m_movement_evaluation.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_siege_won_trigger.values, &ws2.world.rebel_type.m_siege_won_trigger.values, sizeof(ws1.world.rebel_type.m_siege_won_trigger.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_siege_won_effect.values, &ws2.world.rebel_type.m_siege_won_effect.values, sizeof(ws1.world.rebel_type.m_siege_won_effect.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_demands_enforced_trigger.values, &ws2.world.rebel_type.m_demands_enforced_trigger.values, sizeof(ws1.world.rebel_type.m_demands_enforced_trigger.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_type.m_demands_enforced_effect.values, &ws2.world.rebel_type.m_demands_enforced_effect.values, sizeof(ws1.world.rebel_type.m_demands_enforced_effect.values)) == 0);
	REQUIRE(ws1.world.rebel_type.m_government_change.size == ws2.world.rebel_type.m_government_change.size);
	REQUIRE(std::memcmp(ws1.world.rebel_type.m_government_change.values, ws2.world.rebel_type.m_government_change.values, sizeof(decltype(ws1.world.rebel_type.m_government_change.vptr(0))) * ws1.world.rebel_type.m_government_change.size) == 0);
	// obj province
	REQUIRE(std::memcmp(&ws1.world.province.m_name.values, &ws2.world.province.m_name.values, sizeof(ws1.world.province.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_continent.values, &ws2.world.province.m_continent.values, sizeof(ws1.world.province.m_continent.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_climate.values, &ws2.world.province.m_climate.values, sizeof(ws1.world.province.m_climate.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_terrain.values, &ws2.world.province.m_terrain.values, sizeof(ws1.world.province.m_terrain.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_life_rating.values, &ws2.world.province.m_life_rating.values, sizeof(ws1.world.province.m_life_rating.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_rgo.values, &ws2.world.province.m_rgo.values, sizeof(ws1.world.province.m_rgo.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_artisan_production.values, &ws2.world.province.m_artisan_production.values, sizeof(ws1.world.province.m_artisan_production.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_fort_level.values, &ws2.world.province.m_fort_level.values, sizeof(ws1.world.province.m_fort_level.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_naval_base_level.values, &ws2.world.province.m_naval_base_level.values, sizeof(ws1.world.province.m_naval_base_level.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_railroad_level.values, &ws2.world.province.m_railroad_level.values, sizeof(ws1.world.province.m_railroad_level.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_is_slave.values, &ws2.world.province.m_is_slave.values, sizeof(ws1.world.province.m_is_slave.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_is_colonial.values, &ws2.world.province.m_is_colonial.values, sizeof(ws1.world.province.m_is_colonial.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_crime.values, &ws2.world.province.m_crime.values, sizeof(ws1.world.province.m_crime.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_mid_point.values, &ws2.world.province.m_mid_point.values, sizeof(ws1.world.province.m_mid_point.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_mid_point_b.values, &ws2.world.province.m_mid_point_b.values, sizeof(ws1.world.province.m_mid_point_b.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_port_to.values, &ws2.world.province.m_port_to.values, sizeof(ws1.world.province.m_port_to.values)) == 0);
	REQUIRE(ws1.world.province.m_party_loyalty.size == ws2.world.province.m_party_loyalty.size);
	REQUIRE(std::memcmp(ws1.world.province.m_party_loyalty.values, ws2.world.province.m_party_loyalty.values, sizeof(decltype(ws1.world.province.m_party_loyalty.vptr(0))) * ws1.world.province.m_party_loyalty.size) == 0);
	REQUIRE(ws1.world.province.m_modifier_values.size == ws2.world.province.m_modifier_values.size);
	REQUIRE(std::memcmp(ws1.world.province.m_modifier_values.values, ws2.world.province.m_modifier_values.values, sizeof(decltype(ws1.world.province.m_modifier_values.vptr(0))) * ws1.world.province.m_modifier_values.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_current_modifiers.values, &ws2.world.province.m_current_modifiers.values, sizeof(ws1.world.province.m_current_modifiers.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_nationalism.values, &ws2.world.province.m_nationalism.values, sizeof(ws1.world.province.m_nationalism.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_connected_region_id.values, &ws2.world.province.m_connected_region_id.values, sizeof(ws1.world.province.m_connected_region_id.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_state_membership.values, &ws2.world.province.m_state_membership.values, sizeof(ws1.world.province.m_state_membership.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_is_coast.values, &ws2.world.province.m_is_coast.values, sizeof(ws1.world.province.m_is_coast.values)) == 0);
	REQUIRE(ws1.world.province.m_demographics.size == ws2.world.province.m_demographics.size);
	REQUIRE(std::memcmp(ws1.world.province.m_demographics.values, ws2.world.province.m_demographics.values, sizeof(decltype(ws1.world.province.m_demographics.vptr(0))) * ws1.world.province.m_demographics.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_dominant_culture.values, &ws2.world.province.m_dominant_culture.values, sizeof(ws1.world.province.m_dominant_culture.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_dominant_religion.values, &ws2.world.province.m_dominant_religion.values, sizeof(ws1.world.province.m_dominant_religion.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_dominant_ideology.values, &ws2.world.province.m_dominant_ideology.values, sizeof(ws1.world.province.m_dominant_ideology.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_dominant_issue_option.values, &ws2.world.province.m_dominant_issue_option.values, sizeof(ws1.world.province.m_dominant_issue_option.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_last_control_change.values, &ws2.world.province.m_last_control_change.values, sizeof(ws1.world.province.m_last_control_change.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_last_immigration.values, &ws2.world.province.m_last_immigration.values, sizeof(ws1.world.province.m_last_immigration.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_is_owner_core.values, &ws2.world.province.m_is_owner_core.values, sizeof(ws1.world.province.m_is_owner_core.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_rgo_production_scale.values, &ws2.world.province.m_rgo_production_scale.values, sizeof(ws1.world.province.m_rgo_production_scale.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_artisan_production_scale.values, &ws2.world.province.m_artisan_production_scale.values, sizeof(ws1.world.province.m_artisan_production_scale.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_rgo_actual_production.values, &ws2.world.province.m_rgo_actual_production.values, sizeof(ws1.world.province.m_rgo_actual_production.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_artisan_actual_production.values, &ws2.world.province.m_artisan_actual_production.values, sizeof(ws1.world.province.m_artisan_actual_production.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_rgo_full_profit.values, &ws2.world.province.m_rgo_full_profit.values, sizeof(ws1.world.province.m_rgo_full_profit.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_artisan_full_profit.values, &ws2.world.province.m_artisan_full_profit.values, sizeof(ws1.world.province.m_artisan_full_profit.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_rgo_employment.values, &ws2.world.province.m_rgo_employment.values, sizeof(ws1.world.province.m_rgo_employment.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_rgo_size.values, &ws2.world.province.m_rgo_size.values, sizeof(ws1.world.province.m_rgo_size.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_daily_net_migration.values, &ws2.world.province.m_daily_net_migration.values, sizeof(ws1.world.province.m_daily_net_migration.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_daily_net_immigration.values, &ws2.world.province.m_daily_net_immigration.values, sizeof(ws1.world.province.m_daily_net_immigration.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_siege_progress.values, &ws2.world.province.m_siege_progress.values, sizeof(ws1.world.province.m_siege_progress.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_former_controller.values, &ws2.world.province.m_former_controller.values, sizeof(ws1.world.province.m_former_controller.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province.m_former_rebel_controller.values, &ws2.world.province.m_former_rebel_controller.values, sizeof(ws1.world.province.m_former_rebel_controller.values)) == 0);
	// obj province_adjacency
	REQUIRE(std::memcmp(&ws1.world.province_adjacency.m_connected_provinces.values, &ws2.world.province_adjacency.m_connected_provinces.values, sizeof(ws1.world.province_adjacency.m_connected_provinces.values)) == 0);
	// province_adjacency.province_pair is unique
	REQUIRE(std::memcmp(&ws1.world.province_adjacency.m_distance.values, &ws2.world.province_adjacency.m_distance.values, sizeof(ws1.world.province_adjacency.m_distance.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province_adjacency.m_type.values, &ws2.world.province_adjacency.m_type.values, sizeof(ws1.world.province_adjacency.m_type.values)) == 0);
	// obj nation_adjacency
	REQUIRE(std::memcmp(&ws1.world.nation_adjacency.m_connected_nations.values, &ws2.world.nation_adjacency.m_connected_nations.values, sizeof(ws1.world.nation_adjacency.m_connected_nations.values)) == 0);
	// nation_adjacency.nation_adjacency_pair is unique
	// obj regiment
	// regiment.name is unique
	// regiment.type is unique
	REQUIRE(std::memcmp(&ws1.world.regiment.m_strength.values, &ws2.world.regiment.m_strength.values, sizeof(ws1.world.regiment.m_strength.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.regiment.m_pending_damage.values, &ws2.world.regiment.m_pending_damage.values, sizeof(ws1.world.regiment.m_pending_damage.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.regiment.m_org.values, &ws2.world.regiment.m_org.values, sizeof(ws1.world.regiment.m_org.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.regiment.m_pending_split.values, &ws2.world.regiment.m_pending_split.values, sizeof(ws1.world.regiment.m_pending_split.values)) == 0);
	// obj ship
	// ship.name is unique
	// ship.type is unique
	REQUIRE(std::memcmp(&ws1.world.ship.m_strength.values, &ws2.world.ship.m_strength.values, sizeof(ws1.world.ship.m_strength.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ship.m_org.values, &ws2.world.ship.m_org.values, sizeof(ws1.world.ship.m_org.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.ship.m_pending_split.values, &ws2.world.ship.m_pending_split.values, sizeof(ws1.world.ship.m_pending_split.values)) == 0);
	// obj army
	// army.name is unique
	REQUIRE(std::memcmp(&ws1.world.army.m_black_flag.values, &ws2.world.army.m_black_flag.values, sizeof(ws1.world.army.m_black_flag.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.army.m_is_retreating.values, &ws2.world.army.m_is_retreating.values, sizeof(ws1.world.army.m_is_retreating.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.army.m_dig_in.values, &ws2.world.army.m_dig_in.values, sizeof(ws1.world.army.m_dig_in.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.army.m_path.values, &ws2.world.army.m_path.values, sizeof(ws1.world.army.m_path.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.army.m_arrival_time.values, &ws2.world.army.m_arrival_time.values, sizeof(ws1.world.army.m_arrival_time.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.army.m_ai_activity.values, &ws2.world.army.m_ai_activity.values, sizeof(ws1.world.army.m_ai_activity.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.army.m_ai_province.values, &ws2.world.army.m_ai_province.values, sizeof(ws1.world.army.m_ai_province.values)) == 0);
	// obj navy
	// navy.name is unique
	REQUIRE(std::memcmp(&ws1.world.navy.m_path.values, &ws2.world.navy.m_path.values, sizeof(ws1.world.navy.m_path.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.navy.m_arrival_time.values, &ws2.world.navy.m_arrival_time.values, sizeof(ws1.world.navy.m_arrival_time.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.navy.m_is_retreating.values, &ws2.world.navy.m_is_retreating.values, sizeof(ws1.world.navy.m_is_retreating.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.navy.m_ai_activity.values, &ws2.world.navy.m_ai_activity.values, sizeof(ws1.world.navy.m_ai_activity.values)) == 0);
	// obj army_transport
	// army_transport.army is unique
	REQUIRE(std::memcmp(&ws1.world.army_transport.m_navy.values, &ws2.world.army_transport.m_navy.values, sizeof(ws1.world.army_transport.m_navy.values)) == 0);
	// obj army_control
	// army_control.army is unique
	REQUIRE(std::memcmp(&ws1.world.army_control.m_controller.values, &ws2.world.army_control.m_controller.values, sizeof(ws1.world.army_control.m_controller.values)) == 0);
	// obj army_rebel_control
	// army_rebel_control.army is unique
	REQUIRE(std::memcmp(&ws1.world.army_rebel_control.m_controller.values, &ws2.world.army_rebel_control.m_controller.values, sizeof(ws1.world.army_rebel_control.m_controller.values)) == 0);
	// obj army_location
	// army_location.army is unique
	REQUIRE(std::memcmp(&ws1.world.army_location.m_location.values, &ws2.world.army_location.m_location.values, sizeof(ws1.world.army_location.m_location.values)) == 0);
	// obj army_membership
	// army_membership.regiment is unique
	REQUIRE(std::memcmp(&ws1.world.army_membership.m_army.values, &ws2.world.army_membership.m_army.values, sizeof(ws1.world.army_membership.m_army.values)) == 0);
	// obj regiment_source
	// regiment_source.regiment is unique
	REQUIRE(std::memcmp(&ws1.world.regiment_source.m_pop.values, &ws2.world.regiment_source.m_pop.values, sizeof(ws1.world.regiment_source.m_pop.values)) == 0);
	// obj navy_control
	// navy_control.navy is unique
	REQUIRE(std::memcmp(&ws1.world.navy_control.m_controller.values, &ws2.world.navy_control.m_controller.values, sizeof(ws1.world.navy_control.m_controller.values)) == 0);
	// obj navy_location
	// navy_location.navy is unique
	REQUIRE(std::memcmp(&ws1.world.navy_location.m_location.values, &ws2.world.navy_location.m_location.values, sizeof(ws1.world.navy_location.m_location.values)) == 0);
	// obj navy_membership
	// navy_membership.ship is unique
	REQUIRE(std::memcmp(&ws1.world.navy_membership.m_navy.values, &ws2.world.navy_membership.m_navy.values, sizeof(ws1.world.navy_membership.m_navy.values)) == 0);
	// obj naval_battle
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_start_date.values, &ws2.world.naval_battle.m_start_date.values, sizeof(ws1.world.naval_battle.m_start_date.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_war_attacker_is_attacker.values, &ws2.world.naval_battle.m_war_attacker_is_attacker.values, sizeof(ws1.world.naval_battle.m_war_attacker_is_attacker.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_dice_rolls.values, &ws2.world.naval_battle.m_dice_rolls.values, sizeof(ws1.world.naval_battle.m_dice_rolls.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_attacker_big_ships.values, &ws2.world.naval_battle.m_attacker_big_ships.values, sizeof(ws1.world.naval_battle.m_attacker_big_ships.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_attacker_small_ships.values, &ws2.world.naval_battle.m_attacker_small_ships.values, sizeof(ws1.world.naval_battle.m_attacker_small_ships.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_attacker_transport_ships.values, &ws2.world.naval_battle.m_attacker_transport_ships.values, sizeof(ws1.world.naval_battle.m_attacker_transport_ships.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_attacker_big_ships_lost.values, &ws2.world.naval_battle.m_attacker_big_ships_lost.values, sizeof(ws1.world.naval_battle.m_attacker_big_ships_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_attacker_small_ships_lost.values, &ws2.world.naval_battle.m_attacker_small_ships_lost.values, sizeof(ws1.world.naval_battle.m_attacker_small_ships_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_attacker_transport_ships_lost.values, &ws2.world.naval_battle.m_attacker_transport_ships_lost.values, sizeof(ws1.world.naval_battle.m_attacker_transport_ships_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_defender_big_ships.values, &ws2.world.naval_battle.m_defender_big_ships.values, sizeof(ws1.world.naval_battle.m_defender_big_ships.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_defender_small_ships.values, &ws2.world.naval_battle.m_defender_small_ships.values, sizeof(ws1.world.naval_battle.m_defender_small_ships.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_defender_transport_ships.values, &ws2.world.naval_battle.m_defender_transport_ships.values, sizeof(ws1.world.naval_battle.m_defender_transport_ships.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_defender_big_ships_lost.values, &ws2.world.naval_battle.m_defender_big_ships_lost.values, sizeof(ws1.world.naval_battle.m_defender_big_ships_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_defender_small_ships_lost.values, &ws2.world.naval_battle.m_defender_small_ships_lost.values, sizeof(ws1.world.naval_battle.m_defender_small_ships_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_defender_transport_ships_lost.values, &ws2.world.naval_battle.m_defender_transport_ships_lost.values, sizeof(ws1.world.naval_battle.m_defender_transport_ships_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_attacker_loss_value.values, &ws2.world.naval_battle.m_attacker_loss_value.values, sizeof(ws1.world.naval_battle.m_attacker_loss_value.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_defender_loss_value.values, &ws2.world.naval_battle.m_defender_loss_value.values, sizeof(ws1.world.naval_battle.m_defender_loss_value.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.naval_battle.m_slots.values, &ws2.world.naval_battle.m_slots.values, sizeof(ws1.world.naval_battle.m_slots.values)) == 0);
	// obj naval_battle_location
	// naval_battle_location.battle is unique
	REQUIRE(std::memcmp(&ws1.world.naval_battle_location.m_location.values, &ws2.world.naval_battle_location.m_location.values, sizeof(ws1.world.naval_battle_location.m_location.values)) == 0);
	// obj naval_battle_in_war
	// naval_battle_in_war.battle is unique
	REQUIRE(std::memcmp(&ws1.world.naval_battle_in_war.m_war.values, &ws2.world.naval_battle_in_war.m_war.values, sizeof(ws1.world.naval_battle_in_war.m_war.values)) == 0);
	// obj navy_battle_participation
	// navy_battle_participation.navy is unique
	REQUIRE(std::memcmp(&ws1.world.navy_battle_participation.m_battle.values, &ws2.world.navy_battle_participation.m_battle.values, sizeof(ws1.world.navy_battle_participation.m_battle.values)) == 0);
	// obj attacking_admiral
	// attacking_admiral.battle is unique
	// attacking_admiral.admiral is unique
	// obj defending_admiral
	// defending_admiral.battle is unique
	// defending_admiral.admiral is unique
	// obj land_battle
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_start_date.values, &ws2.world.land_battle.m_start_date.values, sizeof(ws1.world.land_battle.m_start_date.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_war_attacker_is_attacker.values, &ws2.world.land_battle.m_war_attacker_is_attacker.values, sizeof(ws1.world.land_battle.m_war_attacker_is_attacker.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_dice_rolls.values, &ws2.world.land_battle.m_dice_rolls.values, sizeof(ws1.world.land_battle.m_dice_rolls.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_defender_bonus.values, &ws2.world.land_battle.m_defender_bonus.values, sizeof(ws1.world.land_battle.m_defender_bonus.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_combat_width.values, &ws2.world.land_battle.m_combat_width.values, sizeof(ws1.world.land_battle.m_combat_width.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_attacker_infantry.values, &ws2.world.land_battle.m_attacker_infantry.values, sizeof(ws1.world.land_battle.m_attacker_infantry.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_attacker_cav.values, &ws2.world.land_battle.m_attacker_cav.values, sizeof(ws1.world.land_battle.m_attacker_cav.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_attacker_support.values, &ws2.world.land_battle.m_attacker_support.values, sizeof(ws1.world.land_battle.m_attacker_support.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_attacker_infantry_lost.values, &ws2.world.land_battle.m_attacker_infantry_lost.values, sizeof(ws1.world.land_battle.m_attacker_infantry_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_attacker_cav_lost.values, &ws2.world.land_battle.m_attacker_cav_lost.values, sizeof(ws1.world.land_battle.m_attacker_cav_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_attacker_support_lost.values, &ws2.world.land_battle.m_attacker_support_lost.values, sizeof(ws1.world.land_battle.m_attacker_support_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_defender_infantry.values, &ws2.world.land_battle.m_defender_infantry.values, sizeof(ws1.world.land_battle.m_defender_infantry.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_defender_cav.values, &ws2.world.land_battle.m_defender_cav.values, sizeof(ws1.world.land_battle.m_defender_cav.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_defender_support.values, &ws2.world.land_battle.m_defender_support.values, sizeof(ws1.world.land_battle.m_defender_support.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_defender_infantry_lost.values, &ws2.world.land_battle.m_defender_infantry_lost.values, sizeof(ws1.world.land_battle.m_defender_infantry_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_defender_cav_lost.values, &ws2.world.land_battle.m_defender_cav_lost.values, sizeof(ws1.world.land_battle.m_defender_cav_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_defender_support_lost.values, &ws2.world.land_battle.m_defender_support_lost.values, sizeof(ws1.world.land_battle.m_defender_support_lost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_attacker_loss_value.values, &ws2.world.land_battle.m_attacker_loss_value.values, sizeof(ws1.world.land_battle.m_attacker_loss_value.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_defender_loss_value.values, &ws2.world.land_battle.m_defender_loss_value.values, sizeof(ws1.world.land_battle.m_defender_loss_value.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_attacker_back_line.values, &ws2.world.land_battle.m_attacker_back_line.values, sizeof(ws1.world.land_battle.m_attacker_back_line.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_attacker_front_line.values, &ws2.world.land_battle.m_attacker_front_line.values, sizeof(ws1.world.land_battle.m_attacker_front_line.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_defender_back_line.values, &ws2.world.land_battle.m_defender_back_line.values, sizeof(ws1.world.land_battle.m_defender_back_line.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_defender_front_line.values, &ws2.world.land_battle.m_defender_front_line.values, sizeof(ws1.world.land_battle.m_defender_front_line.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.land_battle.m_reserves.values, &ws2.world.land_battle.m_reserves.values, sizeof(ws1.world.land_battle.m_reserves.values)) == 0);
	// obj land_battle_location
	// land_battle_location.battle is unique
	REQUIRE(std::memcmp(&ws1.world.land_battle_location.m_location.values, &ws2.world.land_battle_location.m_location.values, sizeof(ws1.world.land_battle_location.m_location.values)) == 0);
	// obj land_battle_in_war
	// land_battle_in_war.battle is unique
	REQUIRE(std::memcmp(&ws1.world.land_battle_in_war.m_war.values, &ws2.world.land_battle_in_war.m_war.values, sizeof(ws1.world.land_battle_in_war.m_war.values)) == 0);
	// obj army_battle_participation
	// army_battle_participation.army is unique
	REQUIRE(std::memcmp(&ws1.world.army_battle_participation.m_battle.values, &ws2.world.army_battle_participation.m_battle.values, sizeof(ws1.world.army_battle_participation.m_battle.values)) == 0);
	// obj attacking_general
	// attacking_general.battle is unique
	// attacking_general.general is unique
	// obj defending_general
	// defending_general.battle is unique
	// defending_general.general is unique
	// obj leader
	// leader.name is unique
	REQUIRE(std::memcmp(&ws1.world.leader.m_personality.values, &ws2.world.leader.m_personality.values, sizeof(ws1.world.leader.m_personality.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader.m_background.values, &ws2.world.leader.m_background.values, sizeof(ws1.world.leader.m_background.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader.m_since.values, &ws2.world.leader.m_since.values, sizeof(ws1.world.leader.m_since.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader.m_prestige.values, &ws2.world.leader.m_prestige.values, sizeof(ws1.world.leader.m_prestige.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.leader.m_is_admiral.values, &ws2.world.leader.m_is_admiral.values, sizeof(ws1.world.leader.m_is_admiral.values)) == 0);
	// obj army_leadership
	// army_leadership.army is unique
	// army_leadership.general is unique
	// obj navy_leadership
	// navy_leadership.navy is unique
	// navy_leadership.admiral is unique
	// obj leader_loyalty
	REQUIRE(std::memcmp(&ws1.world.leader_loyalty.m_nation.values, &ws2.world.leader_loyalty.m_nation.values, sizeof(ws1.world.leader_loyalty.m_nation.values)) == 0);
	// leader_loyalty.leader is unique
	// obj war
	REQUIRE(std::memcmp(&ws1.world.war.m_primary_attacker.values, &ws2.world.war.m_primary_attacker.values, sizeof(ws1.world.war.m_primary_attacker.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war.m_primary_defender.values, &ws2.world.war.m_primary_defender.values, sizeof(ws1.world.war.m_primary_defender.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war.m_start_date.values, &ws2.world.war.m_start_date.values, sizeof(ws1.world.war.m_start_date.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war.m_name.values, &ws2.world.war.m_name.values, sizeof(ws1.world.war.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war.m_over_state.values, &ws2.world.war.m_over_state.values, sizeof(ws1.world.war.m_over_state.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war.m_over_tag.values, &ws2.world.war.m_over_tag.values, sizeof(ws1.world.war.m_over_tag.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war.m_is_great.values, &ws2.world.war.m_is_great.values, sizeof(ws1.world.war.m_is_great.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war.m_is_crisis_war.values, &ws2.world.war.m_is_crisis_war.values, sizeof(ws1.world.war.m_is_crisis_war.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war.m_number_of_battles.values, &ws2.world.war.m_number_of_battles.values, sizeof(ws1.world.war.m_number_of_battles.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war.m_attacker_battle_score.values, &ws2.world.war.m_attacker_battle_score.values, sizeof(ws1.world.war.m_attacker_battle_score.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war.m_defender_battle_score.values, &ws2.world.war.m_defender_battle_score.values, sizeof(ws1.world.war.m_defender_battle_score.values)) == 0);
	// obj peace_offer
	REQUIRE(std::memcmp(&ws1.world.peace_offer.m_target.values, &ws2.world.peace_offer.m_target.values, sizeof(ws1.world.peace_offer.m_target.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.peace_offer.m_is_concession.values, &ws2.world.peace_offer.m_is_concession.values, sizeof(ws1.world.peace_offer.m_is_concession.values)) == 0);
	// obj pending_peace_offer
	// pending_peace_offer.nation is unique
	// pending_peace_offer.peace_offer is unique
	// obj war_settlement
	REQUIRE(std::memcmp(&ws1.world.war_settlement.m_war.values, &ws2.world.war_settlement.m_war.values, sizeof(ws1.world.war_settlement.m_war.values)) == 0);
	// war_settlement.peace_offer is unique
	// obj wargoal
	REQUIRE(std::memcmp(&ws1.world.wargoal.m_added_by.values, &ws2.world.wargoal.m_added_by.values, sizeof(ws1.world.wargoal.m_added_by.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.wargoal.m_target_nation.values, &ws2.world.wargoal.m_target_nation.values, sizeof(ws1.world.wargoal.m_target_nation.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.wargoal.m_type.values, &ws2.world.wargoal.m_type.values, sizeof(ws1.world.wargoal.m_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.wargoal.m_associated_tag.values, &ws2.world.wargoal.m_associated_tag.values, sizeof(ws1.world.wargoal.m_associated_tag.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.wargoal.m_secondary_nation.values, &ws2.world.wargoal.m_secondary_nation.values, sizeof(ws1.world.wargoal.m_secondary_nation.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.wargoal.m_associated_state.values, &ws2.world.wargoal.m_associated_state.values, sizeof(ws1.world.wargoal.m_associated_state.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.wargoal.m_ticking_war_score.values, &ws2.world.wargoal.m_ticking_war_score.values, sizeof(ws1.world.wargoal.m_ticking_war_score.values)) == 0);
	// obj war_participant
	REQUIRE(std::memcmp(&ws1.world.war_participant.m_war.values, &ws2.world.war_participant.m_war.values, sizeof(ws1.world.war_participant.m_war.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war_participant.m_nation.values, &ws2.world.war_participant.m_nation.values, sizeof(ws1.world.war_participant.m_nation.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.war_participant.m_is_attacker.values, &ws2.world.war_participant.m_is_attacker.values, sizeof(ws1.world.war_participant.m_is_attacker.values)) == 0);
	// obj wargoals_attached
	REQUIRE(std::memcmp(&ws1.world.wargoals_attached.m_war.values, &ws2.world.wargoals_attached.m_war.values, sizeof(ws1.world.wargoals_attached.m_war.values)) == 0);
	// wargoals_attached.wargoal is unique
	// obj peace_offer_item
	REQUIRE(std::memcmp(&ws1.world.peace_offer_item.m_peace_offer.values, &ws2.world.peace_offer_item.m_peace_offer.values, sizeof(ws1.world.peace_offer_item.m_peace_offer.values)) == 0);
	// peace_offer_item.wargoal is unique
	// obj state_definition
	REQUIRE(std::memcmp(&ws1.world.state_definition.m_name.values, &ws2.world.state_definition.m_name.values, sizeof(ws1.world.state_definition.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_definition.m_colonization_temperature.values, &ws2.world.state_definition.m_colonization_temperature.values, sizeof(ws1.world.state_definition.m_colonization_temperature.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_definition.m_colonization_stage.values, &ws2.world.state_definition.m_colonization_stage.values, sizeof(ws1.world.state_definition.m_colonization_stage.values)) == 0);
	// obj state_instance
	REQUIRE(std::memcmp(&ws1.world.state_instance.m_definition.values, &ws2.world.state_instance.m_definition.values, sizeof(ws1.world.state_instance.m_definition.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_instance.m_flashpoint_tag.values, &ws2.world.state_instance.m_flashpoint_tag.values, sizeof(ws1.world.state_instance.m_flashpoint_tag.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_instance.m_flashpoint_tension.values, &ws2.world.state_instance.m_flashpoint_tension.values, sizeof(ws1.world.state_instance.m_flashpoint_tension.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_instance.m_capital.values, &ws2.world.state_instance.m_capital.values, sizeof(ws1.world.state_instance.m_capital.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_instance.m_owner_focus.values, &ws2.world.state_instance.m_owner_focus.values, sizeof(ws1.world.state_instance.m_owner_focus.values)) == 0);
	REQUIRE(ws1.world.state_instance.m_demographics.size == ws2.world.state_instance.m_demographics.size);
	REQUIRE(std::memcmp(ws1.world.state_instance.m_demographics.values, ws2.world.state_instance.m_demographics.values, sizeof(decltype(ws1.world.state_instance.m_demographics.vptr(0))) * ws1.world.state_instance.m_demographics.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_instance.m_dominant_culture.values, &ws2.world.state_instance.m_dominant_culture.values, sizeof(ws1.world.state_instance.m_dominant_culture.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_instance.m_dominant_religion.values, &ws2.world.state_instance.m_dominant_religion.values, sizeof(ws1.world.state_instance.m_dominant_religion.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_instance.m_dominant_ideology.values, &ws2.world.state_instance.m_dominant_ideology.values, sizeof(ws1.world.state_instance.m_dominant_ideology.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_instance.m_dominant_issue_option.values, &ws2.world.state_instance.m_dominant_issue_option.values, sizeof(ws1.world.state_instance.m_dominant_issue_option.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_instance.m_naval_base_is_taken.values, &ws2.world.state_instance.m_naval_base_is_taken.values, sizeof(ws1.world.state_instance.m_naval_base_is_taken.values)) == 0);
	// obj colonization
	REQUIRE(std::memcmp(&ws1.world.colonization.m_state.values, &ws2.world.colonization.m_state.values, sizeof(ws1.world.colonization.m_state.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.colonization.m_colonizer.values, &ws2.world.colonization.m_colonizer.values, sizeof(ws1.world.colonization.m_colonizer.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.colonization.m_last_investment.values, &ws2.world.colonization.m_last_investment.values, sizeof(ws1.world.colonization.m_last_investment.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.colonization.m_points_invested.values, &ws2.world.colonization.m_points_invested.values, sizeof(ws1.world.colonization.m_points_invested.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.colonization.m_level.values, &ws2.world.colonization.m_level.values, sizeof(ws1.world.colonization.m_level.values)) == 0);
	// obj state_ownership
	// state_ownership.state is unique
	REQUIRE(std::memcmp(&ws1.world.state_ownership.m_nation.values, &ws2.world.state_ownership.m_nation.values, sizeof(ws1.world.state_ownership.m_nation.values)) == 0);
	// obj flashpoint_focus
	// flashpoint_focus.state is unique
	// flashpoint_focus.nation is unique
	// obj abstract_state_membership
	// abstract_state_membership.province is unique
	REQUIRE(std::memcmp(&ws1.world.abstract_state_membership.m_state.values, &ws2.world.abstract_state_membership.m_state.values, sizeof(ws1.world.abstract_state_membership.m_state.values)) == 0);
	// obj core
	REQUIRE(std::memcmp(&ws1.world.core.m_province.values, &ws2.world.core.m_province.values, sizeof(ws1.world.core.m_province.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.core.m_identity.values, &ws2.world.core.m_identity.values, sizeof(ws1.world.core.m_identity.values)) == 0);
	// core.prov_tag_key is unique
	// obj identity_holder
	// identity_holder.nation is unique
	// identity_holder.identity is unique
	// obj technology
	REQUIRE(std::memcmp(&ws1.world.technology.m_name.values, &ws2.world.technology.m_name.values, sizeof(ws1.world.technology.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_modifier.values, &ws2.world.technology.m_modifier.values, sizeof(ws1.world.technology.m_modifier.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_image.values, &ws2.world.technology.m_image.values, sizeof(ws1.world.technology.m_image.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_folder_index.values, &ws2.world.technology.m_folder_index.values, sizeof(ws1.world.technology.m_folder_index.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_year.values, &ws2.world.technology.m_year.values, sizeof(ws1.world.technology.m_year.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_ai_chance.values, &ws2.world.technology.m_ai_chance.values, sizeof(ws1.world.technology.m_ai_chance.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_ai_weight.values, &ws2.world.technology.m_ai_weight.values, sizeof(ws1.world.technology.m_ai_weight.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_cost.values, &ws2.world.technology.m_cost.values, sizeof(ws1.world.technology.m_cost.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_colonial_points.values, &ws2.world.technology.m_colonial_points.values, sizeof(ws1.world.technology.m_colonial_points.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_increase_railroad.values, &ws2.world.technology.m_increase_railroad.values, sizeof(ws1.world.technology.m_increase_railroad.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_increase_fort.values, &ws2.world.technology.m_increase_fort.values, sizeof(ws1.world.technology.m_increase_fort.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_increase_naval_base.values, &ws2.world.technology.m_increase_naval_base.values, sizeof(ws1.world.technology.m_increase_naval_base.values)) == 0);
	REQUIRE(ws1.world.technology.m_activate_unit.size == ws2.world.technology.m_activate_unit.size);
	REQUIRE(std::memcmp(ws1.world.technology.m_activate_unit.values, ws2.world.technology.m_activate_unit.values, sizeof(decltype(ws1.world.technology.m_activate_unit.vptr(0))) * ws1.world.technology.m_activate_unit.size) == 0);
	REQUIRE(ws1.world.technology.m_activate_building.size == ws2.world.technology.m_activate_building.size);
	REQUIRE(std::memcmp(ws1.world.technology.m_activate_building.values, ws2.world.technology.m_activate_building.values, sizeof(decltype(ws1.world.technology.m_activate_building.vptr(0))) * ws1.world.technology.m_activate_building.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_rgo_goods_output.values, &ws2.world.technology.m_rgo_goods_output.values, sizeof(ws1.world.technology.m_rgo_goods_output.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_factory_goods_output.values, &ws2.world.technology.m_factory_goods_output.values, sizeof(ws1.world.technology.m_factory_goods_output.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_rgo_size.values, &ws2.world.technology.m_rgo_size.values, sizeof(ws1.world.technology.m_rgo_size.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.technology.m_modified_units.values, &ws2.world.technology.m_modified_units.values, sizeof(ws1.world.technology.m_modified_units.values)) == 0);
	// obj invention
	REQUIRE(std::memcmp(&ws1.world.invention.m_name.values, &ws2.world.invention.m_name.values, sizeof(ws1.world.invention.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_technology_type.values, &ws2.world.invention.m_technology_type.values, sizeof(ws1.world.invention.m_technology_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_modifier.values, &ws2.world.invention.m_modifier.values, sizeof(ws1.world.invention.m_modifier.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_chance.values, &ws2.world.invention.m_chance.values, sizeof(ws1.world.invention.m_chance.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_limit.values, &ws2.world.invention.m_limit.values, sizeof(ws1.world.invention.m_limit.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_enable_gas_attack.values, &ws2.world.invention.m_enable_gas_attack.values, sizeof(ws1.world.invention.m_enable_gas_attack.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_enable_gas_defense.values, &ws2.world.invention.m_enable_gas_defense.values, sizeof(ws1.world.invention.m_enable_gas_defense.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_shared_prestige.values, &ws2.world.invention.m_shared_prestige.values, sizeof(ws1.world.invention.m_shared_prestige.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_plurality.values, &ws2.world.invention.m_plurality.values, sizeof(ws1.world.invention.m_plurality.values)) == 0);
	REQUIRE(ws1.world.invention.m_activate_unit.size == ws2.world.invention.m_activate_unit.size);
	REQUIRE(std::memcmp(ws1.world.invention.m_activate_unit.values, ws2.world.invention.m_activate_unit.values, sizeof(decltype(ws1.world.invention.m_activate_unit.vptr(0))) * ws1.world.invention.m_activate_unit.size) == 0);
	REQUIRE(ws1.world.invention.m_activate_crime.size == ws2.world.invention.m_activate_crime.size);
	REQUIRE(std::memcmp(ws1.world.invention.m_activate_crime.values, ws2.world.invention.m_activate_crime.values, sizeof(decltype(ws1.world.invention.m_activate_crime.vptr(0))) * ws1.world.invention.m_activate_crime.size) == 0);
	REQUIRE(ws1.world.invention.m_activate_building.size == ws2.world.invention.m_activate_building.size);
	REQUIRE(std::memcmp(ws1.world.invention.m_activate_building.values, ws2.world.invention.m_activate_building.values, sizeof(decltype(ws1.world.invention.m_activate_building.vptr(0))) * ws1.world.invention.m_activate_building.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_rgo_goods_output.values, &ws2.world.invention.m_rgo_goods_output.values, sizeof(ws1.world.invention.m_rgo_goods_output.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_factory_goods_output.values, &ws2.world.invention.m_factory_goods_output.values, sizeof(ws1.world.invention.m_factory_goods_output.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_factory_goods_throughput.values, &ws2.world.invention.m_factory_goods_throughput.values, sizeof(ws1.world.invention.m_factory_goods_throughput.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_modified_units.values, &ws2.world.invention.m_modified_units.values, sizeof(ws1.world.invention.m_modified_units.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.invention.m_rebel_org.values, &ws2.world.invention.m_rebel_org.values, sizeof(ws1.world.invention.m_rebel_org.values)) == 0);
	// obj nation
	REQUIRE(std::memcmp(&ws1.world.nation.m_color.values, &ws2.world.nation.m_color.values, sizeof(ws1.world.nation.m_color.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_name.values, &ws2.world.nation.m_name.values, sizeof(ws1.world.nation.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_adjective.values, &ws2.world.nation.m_adjective.values, sizeof(ws1.world.nation.m_adjective.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_primary_culture.values, &ws2.world.nation.m_primary_culture.values, sizeof(ws1.world.nation.m_primary_culture.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_accepted_cultures.values, &ws2.world.nation.m_accepted_cultures.values, sizeof(ws1.world.nation.m_accepted_cultures.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_religion.values, &ws2.world.nation.m_religion.values, sizeof(ws1.world.nation.m_religion.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_capital.values, &ws2.world.nation.m_capital.values, sizeof(ws1.world.nation.m_capital.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_is_civilized.values, &ws2.world.nation.m_is_civilized.values, sizeof(ws1.world.nation.m_is_civilized.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_is_great_power.values, &ws2.world.nation.m_is_great_power.values, sizeof(ws1.world.nation.m_is_great_power.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_national_value.values, &ws2.world.nation.m_national_value.values, sizeof(ws1.world.nation.m_national_value.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_tech_school.values, &ws2.world.nation.m_tech_school.values, sizeof(ws1.world.nation.m_tech_school.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_government_type.values, &ws2.world.nation.m_government_type.values, sizeof(ws1.world.nation.m_government_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_plurality.values, &ws2.world.nation.m_plurality.values, sizeof(ws1.world.nation.m_plurality.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_prestige.values, &ws2.world.nation.m_prestige.values, sizeof(ws1.world.nation.m_prestige.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_infamy.values, &ws2.world.nation.m_infamy.values, sizeof(ws1.world.nation.m_infamy.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_revanchism.values, &ws2.world.nation.m_revanchism.values, sizeof(ws1.world.nation.m_revanchism.values)) == 0);
	REQUIRE(ws1.world.nation.m_active_technologies.size == ws2.world.nation.m_active_technologies.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_active_technologies.values, ws2.world.nation.m_active_technologies.values, sizeof(decltype(ws1.world.nation.m_active_technologies.vptr(0))) * ws1.world.nation.m_active_technologies.size) == 0);
	REQUIRE(ws1.world.nation.m_active_inventions.size == ws2.world.nation.m_active_inventions.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_active_inventions.values, ws2.world.nation.m_active_inventions.values, sizeof(decltype(ws1.world.nation.m_active_inventions.vptr(0))) * ws1.world.nation.m_active_inventions.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_ruling_party.values, &ws2.world.nation.m_ruling_party.values, sizeof(ws1.world.nation.m_ruling_party.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_ruling_party_last_appointed.values, &ws2.world.nation.m_ruling_party_last_appointed.values, sizeof(ws1.world.nation.m_ruling_party_last_appointed.values)) == 0);
	REQUIRE(ws1.world.nation.m_issues.size == ws2.world.nation.m_issues.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_issues.values, ws2.world.nation.m_issues.values, sizeof(decltype(ws1.world.nation.m_issues.vptr(0))) * ws1.world.nation.m_issues.size) == 0);
	REQUIRE(ws1.world.nation.m_reforms.size == ws2.world.nation.m_reforms.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_reforms.values, ws2.world.nation.m_reforms.values, sizeof(decltype(ws1.world.nation.m_reforms.vptr(0))) * ws1.world.nation.m_reforms.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_last_issue_or_reform_change.values, &ws2.world.nation.m_last_issue_or_reform_change.values, sizeof(ws1.world.nation.m_last_issue_or_reform_change.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_combined_issue_rules.values, &ws2.world.nation.m_combined_issue_rules.values, sizeof(ws1.world.nation.m_combined_issue_rules.values)) == 0);
	REQUIRE(ws1.world.nation.m_upper_house.size == ws2.world.nation.m_upper_house.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_upper_house.values, ws2.world.nation.m_upper_house.values, sizeof(decltype(ws1.world.nation.m_upper_house.vptr(0))) * ws1.world.nation.m_upper_house.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_is_substate.values, &ws2.world.nation.m_is_substate.values, sizeof(ws1.world.nation.m_is_substate.values)) == 0);
	REQUIRE(ws1.world.nation.m_flag_variables.size == ws2.world.nation.m_flag_variables.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_flag_variables.values, ws2.world.nation.m_flag_variables.values, sizeof(decltype(ws1.world.nation.m_flag_variables.vptr(0))) * ws1.world.nation.m_flag_variables.size) == 0);
	REQUIRE(ws1.world.nation.m_variables.size == ws2.world.nation.m_variables.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_variables.values, ws2.world.nation.m_variables.values, sizeof(decltype(ws1.world.nation.m_variables.vptr(0))) * ws1.world.nation.m_variables.size) == 0);
	REQUIRE(ws1.world.nation.m_modifier_values.size == ws2.world.nation.m_modifier_values.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_modifier_values.values, ws2.world.nation.m_modifier_values.values, sizeof(decltype(ws1.world.nation.m_modifier_values.vptr(0))) * ws1.world.nation.m_modifier_values.size) == 0);
	REQUIRE(ws1.world.nation.m_rgo_goods_output.size == ws2.world.nation.m_rgo_goods_output.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_rgo_goods_output.values, ws2.world.nation.m_rgo_goods_output.values, sizeof(decltype(ws1.world.nation.m_rgo_goods_output.vptr(0))) * ws1.world.nation.m_rgo_goods_output.size) == 0);
	REQUIRE(ws1.world.nation.m_factory_goods_output.size == ws2.world.nation.m_factory_goods_output.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_factory_goods_output.values, ws2.world.nation.m_factory_goods_output.values, sizeof(decltype(ws1.world.nation.m_factory_goods_output.vptr(0))) * ws1.world.nation.m_factory_goods_output.size) == 0);
	REQUIRE(ws1.world.nation.m_rgo_size.size == ws2.world.nation.m_rgo_size.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_rgo_size.values, ws2.world.nation.m_rgo_size.values, sizeof(decltype(ws1.world.nation.m_rgo_size.vptr(0))) * ws1.world.nation.m_rgo_size.size) == 0);
	REQUIRE(ws1.world.nation.m_factory_goods_throughput.size == ws2.world.nation.m_factory_goods_throughput.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_factory_goods_throughput.values, ws2.world.nation.m_factory_goods_throughput.values, sizeof(decltype(ws1.world.nation.m_factory_goods_throughput.vptr(0))) * ws1.world.nation.m_factory_goods_throughput.size) == 0);
	REQUIRE(ws1.world.nation.m_rebel_org_modifier.size == ws2.world.nation.m_rebel_org_modifier.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_rebel_org_modifier.values, ws2.world.nation.m_rebel_org_modifier.values, sizeof(decltype(ws1.world.nation.m_rebel_org_modifier.vptr(0))) * ws1.world.nation.m_rebel_org_modifier.size) == 0);
	REQUIRE(ws1.world.nation.m_unit_stats.size == ws2.world.nation.m_unit_stats.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_unit_stats.values, ws2.world.nation.m_unit_stats.values, sizeof(decltype(ws1.world.nation.m_unit_stats.vptr(0))) * ws1.world.nation.m_unit_stats.size) == 0);
	REQUIRE(ws1.world.nation.m_active_unit.size == ws2.world.nation.m_active_unit.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_active_unit.values, ws2.world.nation.m_active_unit.values, sizeof(decltype(ws1.world.nation.m_active_unit.vptr(0))) * ws1.world.nation.m_active_unit.size) == 0);
	REQUIRE(ws1.world.nation.m_active_crime.size == ws2.world.nation.m_active_crime.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_active_crime.values, ws2.world.nation.m_active_crime.values, sizeof(decltype(ws1.world.nation.m_active_crime.vptr(0))) * ws1.world.nation.m_active_crime.size) == 0);
	REQUIRE(ws1.world.nation.m_active_building.size == ws2.world.nation.m_active_building.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_active_building.values, ws2.world.nation.m_active_building.values, sizeof(decltype(ws1.world.nation.m_active_building.vptr(0))) * ws1.world.nation.m_active_building.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_has_gas_attack.values, &ws2.world.nation.m_has_gas_attack.values, sizeof(ws1.world.nation.m_has_gas_attack.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_has_gas_defense.values, &ws2.world.nation.m_has_gas_defense.values, sizeof(ws1.world.nation.m_has_gas_defense.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_max_railroad_level.values, &ws2.world.nation.m_max_railroad_level.values, sizeof(ws1.world.nation.m_max_railroad_level.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_max_fort_level.values, &ws2.world.nation.m_max_fort_level.values, sizeof(ws1.world.nation.m_max_fort_level.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_max_naval_base_level.values, &ws2.world.nation.m_max_naval_base_level.values, sizeof(ws1.world.nation.m_max_naval_base_level.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_current_modifiers.values, &ws2.world.nation.m_current_modifiers.values, sizeof(ws1.world.nation.m_current_modifiers.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_in_sphere_of.values, &ws2.world.nation.m_in_sphere_of.values, sizeof(ws1.world.nation.m_in_sphere_of.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_rank.values, &ws2.world.nation.m_rank.values, sizeof(ws1.world.nation.m_rank.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_industrial_rank.values, &ws2.world.nation.m_industrial_rank.values, sizeof(ws1.world.nation.m_industrial_rank.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_military_rank.values, &ws2.world.nation.m_military_rank.values, sizeof(ws1.world.nation.m_military_rank.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_prestige_rank.values, &ws2.world.nation.m_prestige_rank.values, sizeof(ws1.world.nation.m_prestige_rank.values)) == 0);
	REQUIRE(ws1.world.nation.m_demographics.size == ws2.world.nation.m_demographics.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_demographics.values, ws2.world.nation.m_demographics.values, sizeof(decltype(ws1.world.nation.m_demographics.vptr(0))) * ws1.world.nation.m_demographics.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_war_exhaustion.values, &ws2.world.nation.m_war_exhaustion.values, sizeof(ws1.world.nation.m_war_exhaustion.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_is_at_war.values, &ws2.world.nation.m_is_at_war.values, sizeof(ws1.world.nation.m_is_at_war.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_is_mobilized.values, &ws2.world.nation.m_is_mobilized.values, sizeof(ws1.world.nation.m_is_mobilized.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_mobilization_remaining.values, &ws2.world.nation.m_mobilization_remaining.values, sizeof(ws1.world.nation.m_mobilization_remaining.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_mobilization_schedule.values, &ws2.world.nation.m_mobilization_schedule.values, sizeof(ws1.world.nation.m_mobilization_schedule.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_is_bankrupt.values, &ws2.world.nation.m_is_bankrupt.values, sizeof(ws1.world.nation.m_is_bankrupt.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_last_war_loss.values, &ws2.world.nation.m_last_war_loss.values, sizeof(ws1.world.nation.m_last_war_loss.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_election_ends.values, &ws2.world.nation.m_election_ends.values, sizeof(ws1.world.nation.m_election_ends.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_disarmed_until.values, &ws2.world.nation.m_disarmed_until.values, sizeof(ws1.world.nation.m_disarmed_until.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_reparations_until.values, &ws2.world.nation.m_reparations_until.values, sizeof(ws1.world.nation.m_reparations_until.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_education_spending.values, &ws2.world.nation.m_education_spending.values, sizeof(ws1.world.nation.m_education_spending.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_military_spending.values, &ws2.world.nation.m_military_spending.values, sizeof(ws1.world.nation.m_military_spending.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_administrative_spending.values, &ws2.world.nation.m_administrative_spending.values, sizeof(ws1.world.nation.m_administrative_spending.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_social_spending.values, &ws2.world.nation.m_social_spending.values, sizeof(ws1.world.nation.m_social_spending.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_land_spending.values, &ws2.world.nation.m_land_spending.values, sizeof(ws1.world.nation.m_land_spending.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_naval_spending.values, &ws2.world.nation.m_naval_spending.values, sizeof(ws1.world.nation.m_naval_spending.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_construction_spending.values, &ws2.world.nation.m_construction_spending.values, sizeof(ws1.world.nation.m_construction_spending.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_effective_land_spending.values, &ws2.world.nation.m_effective_land_spending.values, sizeof(ws1.world.nation.m_effective_land_spending.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_effective_naval_spending.values, &ws2.world.nation.m_effective_naval_spending.values, sizeof(ws1.world.nation.m_effective_naval_spending.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_effective_construction_spending.values, &ws2.world.nation.m_effective_construction_spending.values, sizeof(ws1.world.nation.m_effective_construction_spending.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_maximum_military_costs.values, &ws2.world.nation.m_maximum_military_costs.values, sizeof(ws1.world.nation.m_maximum_military_costs.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_subsidies_spending.values, &ws2.world.nation.m_subsidies_spending.values, sizeof(ws1.world.nation.m_subsidies_spending.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_spending_level.values, &ws2.world.nation.m_spending_level.values, sizeof(ws1.world.nation.m_spending_level.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_private_investment.values, &ws2.world.nation.m_private_investment.values, sizeof(ws1.world.nation.m_private_investment.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_private_investment_effective_fraction.values, &ws2.world.nation.m_private_investment_effective_fraction.values, sizeof(ws1.world.nation.m_private_investment_effective_fraction.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_total_rich_income.values, &ws2.world.nation.m_total_rich_income.values, sizeof(ws1.world.nation.m_total_rich_income.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_total_middle_income.values, &ws2.world.nation.m_total_middle_income.values, sizeof(ws1.world.nation.m_total_middle_income.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_total_poor_income.values, &ws2.world.nation.m_total_poor_income.values, sizeof(ws1.world.nation.m_total_poor_income.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_poor_tax.values, &ws2.world.nation.m_poor_tax.values, sizeof(ws1.world.nation.m_poor_tax.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_middle_tax.values, &ws2.world.nation.m_middle_tax.values, sizeof(ws1.world.nation.m_middle_tax.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_rich_tax.values, &ws2.world.nation.m_rich_tax.values, sizeof(ws1.world.nation.m_rich_tax.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_tariffs.values, &ws2.world.nation.m_tariffs.values, sizeof(ws1.world.nation.m_tariffs.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_last_treasury.values, &ws2.world.nation.m_last_treasury.values, sizeof(ws1.world.nation.m_last_treasury.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_industrial_score.values, &ws2.world.nation.m_industrial_score.values, sizeof(ws1.world.nation.m_industrial_score.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_military_score.values, &ws2.world.nation.m_military_score.values, sizeof(ws1.world.nation.m_military_score.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_central_blockaded.values, &ws2.world.nation.m_central_blockaded.values, sizeof(ws1.world.nation.m_central_blockaded.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_central_rebel_controlled.values, &ws2.world.nation.m_central_rebel_controlled.values, sizeof(ws1.world.nation.m_central_rebel_controlled.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_owned_province_count.values, &ws2.world.nation.m_owned_province_count.values, sizeof(ws1.world.nation.m_owned_province_count.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_owned_state_count.values, &ws2.world.nation.m_owned_state_count.values, sizeof(ws1.world.nation.m_owned_state_count.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_central_province_count.values, &ws2.world.nation.m_central_province_count.values, sizeof(ws1.world.nation.m_central_province_count.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_rebel_controlled_count.values, &ws2.world.nation.m_rebel_controlled_count.values, sizeof(ws1.world.nation.m_rebel_controlled_count.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_occupied_count.values, &ws2.world.nation.m_occupied_count.values, sizeof(ws1.world.nation.m_occupied_count.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_central_ports.values, &ws2.world.nation.m_central_ports.values, sizeof(ws1.world.nation.m_central_ports.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_total_ports.values, &ws2.world.nation.m_total_ports.values, sizeof(ws1.world.nation.m_total_ports.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_central_crime_count.values, &ws2.world.nation.m_central_crime_count.values, sizeof(ws1.world.nation.m_central_crime_count.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_allies_count.values, &ws2.world.nation.m_allies_count.values, sizeof(ws1.world.nation.m_allies_count.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_vassals_count.values, &ws2.world.nation.m_vassals_count.values, sizeof(ws1.world.nation.m_vassals_count.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_substates_count.values, &ws2.world.nation.m_substates_count.values, sizeof(ws1.world.nation.m_substates_count.values)) == 0);
	REQUIRE(ws1.world.nation.m_stockpiles.size == ws2.world.nation.m_stockpiles.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_stockpiles.values, ws2.world.nation.m_stockpiles.values, sizeof(decltype(ws1.world.nation.m_stockpiles.vptr(0))) * ws1.world.nation.m_stockpiles.size) == 0);
	REQUIRE(ws1.world.nation.m_stockpile_targets.size == ws2.world.nation.m_stockpile_targets.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_stockpile_targets.values, ws2.world.nation.m_stockpile_targets.values, sizeof(decltype(ws1.world.nation.m_stockpile_targets.vptr(0))) * ws1.world.nation.m_stockpile_targets.size) == 0);
	REQUIRE(ws1.world.nation.m_drawing_on_stockpiles.size == ws2.world.nation.m_drawing_on_stockpiles.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_drawing_on_stockpiles.values, ws2.world.nation.m_drawing_on_stockpiles.values, sizeof(decltype(ws1.world.nation.m_drawing_on_stockpiles.vptr(0))) * ws1.world.nation.m_drawing_on_stockpiles.size) == 0);
	REQUIRE(ws1.world.nation.m_domestic_market_pool.size == ws2.world.nation.m_domestic_market_pool.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_domestic_market_pool.values, ws2.world.nation.m_domestic_market_pool.values, sizeof(decltype(ws1.world.nation.m_domestic_market_pool.vptr(0))) * ws1.world.nation.m_domestic_market_pool.size) == 0);
	REQUIRE(ws1.world.nation.m_real_demand.size == ws2.world.nation.m_real_demand.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_real_demand.values, ws2.world.nation.m_real_demand.values, sizeof(decltype(ws1.world.nation.m_real_demand.vptr(0))) * ws1.world.nation.m_real_demand.size) == 0);
	REQUIRE(ws1.world.nation.m_imports.size == ws2.world.nation.m_imports.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_imports.values, ws2.world.nation.m_imports.values, sizeof(decltype(ws1.world.nation.m_imports.vptr(0))) * ws1.world.nation.m_imports.size) == 0);
	REQUIRE(ws1.world.nation.m_army_demand.size == ws2.world.nation.m_army_demand.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_army_demand.values, ws2.world.nation.m_army_demand.values, sizeof(decltype(ws1.world.nation.m_army_demand.vptr(0))) * ws1.world.nation.m_army_demand.size) == 0);
	REQUIRE(ws1.world.nation.m_navy_demand.size == ws2.world.nation.m_navy_demand.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_navy_demand.values, ws2.world.nation.m_navy_demand.values, sizeof(decltype(ws1.world.nation.m_navy_demand.vptr(0))) * ws1.world.nation.m_navy_demand.size) == 0);
	REQUIRE(ws1.world.nation.m_construction_demand.size == ws2.world.nation.m_construction_demand.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_construction_demand.values, ws2.world.nation.m_construction_demand.values, sizeof(decltype(ws1.world.nation.m_construction_demand.vptr(0))) * ws1.world.nation.m_construction_demand.size) == 0);
	REQUIRE(ws1.world.nation.m_private_construction_demand.size == ws2.world.nation.m_private_construction_demand.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_private_construction_demand.values, ws2.world.nation.m_private_construction_demand.values, sizeof(decltype(ws1.world.nation.m_private_construction_demand.vptr(0))) * ws1.world.nation.m_private_construction_demand.size) == 0);
	REQUIRE(ws1.world.nation.m_demand_satisfaction.size == ws2.world.nation.m_demand_satisfaction.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_demand_satisfaction.values, ws2.world.nation.m_demand_satisfaction.values, sizeof(decltype(ws1.world.nation.m_demand_satisfaction.vptr(0))) * ws1.world.nation.m_demand_satisfaction.size) == 0);
	REQUIRE(ws1.world.nation.m_life_needs_costs.size == ws2.world.nation.m_life_needs_costs.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_life_needs_costs.values, ws2.world.nation.m_life_needs_costs.values, sizeof(decltype(ws1.world.nation.m_life_needs_costs.vptr(0))) * ws1.world.nation.m_life_needs_costs.size) == 0);
	REQUIRE(ws1.world.nation.m_everyday_needs_costs.size == ws2.world.nation.m_everyday_needs_costs.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_everyday_needs_costs.values, ws2.world.nation.m_everyday_needs_costs.values, sizeof(decltype(ws1.world.nation.m_everyday_needs_costs.vptr(0))) * ws1.world.nation.m_everyday_needs_costs.size) == 0);
	REQUIRE(ws1.world.nation.m_luxury_needs_costs.size == ws2.world.nation.m_luxury_needs_costs.size);
	REQUIRE(std::memcmp(ws1.world.nation.m_luxury_needs_costs.values, ws2.world.nation.m_luxury_needs_costs.values, sizeof(decltype(ws1.world.nation.m_luxury_needs_costs.vptr(0))) * ws1.world.nation.m_luxury_needs_costs.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_overseas_penalty.values, &ws2.world.nation.m_overseas_penalty.values, sizeof(ws1.world.nation.m_overseas_penalty.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_is_player_controlled.values, &ws2.world.nation.m_is_player_controlled.values, sizeof(ws1.world.nation.m_is_player_controlled.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_is_colonial_nation.values, &ws2.world.nation.m_is_colonial_nation.values, sizeof(ws1.world.nation.m_is_colonial_nation.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_dominant_culture.values, &ws2.world.nation.m_dominant_culture.values, sizeof(ws1.world.nation.m_dominant_culture.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_dominant_religion.values, &ws2.world.nation.m_dominant_religion.values, sizeof(ws1.world.nation.m_dominant_religion.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_dominant_ideology.values, &ws2.world.nation.m_dominant_ideology.values, sizeof(ws1.world.nation.m_dominant_ideology.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_dominant_issue_option.values, &ws2.world.nation.m_dominant_issue_option.values, sizeof(ws1.world.nation.m_dominant_issue_option.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_constructing_cb_target.values, &ws2.world.nation.m_constructing_cb_target.values, sizeof(ws1.world.nation.m_constructing_cb_target.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_constructing_cb_progress.values, &ws2.world.nation.m_constructing_cb_progress.values, sizeof(ws1.world.nation.m_constructing_cb_progress.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_constructing_cb_type.values, &ws2.world.nation.m_constructing_cb_type.values, sizeof(ws1.world.nation.m_constructing_cb_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_constructing_cb_is_discovered.values, &ws2.world.nation.m_constructing_cb_is_discovered.values, sizeof(ws1.world.nation.m_constructing_cb_is_discovered.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_available_cbs.values, &ws2.world.nation.m_available_cbs.values, sizeof(ws1.world.nation.m_available_cbs.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_non_colonial_population.values, &ws2.world.nation.m_non_colonial_population.values, sizeof(ws1.world.nation.m_non_colonial_population.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_non_colonial_bureaucrats.values, &ws2.world.nation.m_non_colonial_bureaucrats.values, sizeof(ws1.world.nation.m_non_colonial_bureaucrats.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_administrative_efficiency.values, &ws2.world.nation.m_administrative_efficiency.values, sizeof(ws1.world.nation.m_administrative_efficiency.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_research_points.values, &ws2.world.nation.m_research_points.values, sizeof(ws1.world.nation.m_research_points.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_current_research.values, &ws2.world.nation.m_current_research.values, sizeof(ws1.world.nation.m_current_research.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_active_regiments.values, &ws2.world.nation.m_active_regiments.values, sizeof(ws1.world.nation.m_active_regiments.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_recruitable_regiments.values, &ws2.world.nation.m_recruitable_regiments.values, sizeof(ws1.world.nation.m_recruitable_regiments.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_averge_land_unit_score.values, &ws2.world.nation.m_averge_land_unit_score.values, sizeof(ws1.world.nation.m_averge_land_unit_score.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_capital_ship_score.values, &ws2.world.nation.m_capital_ship_score.values, sizeof(ws1.world.nation.m_capital_ship_score.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_suppression_points.values, &ws2.world.nation.m_suppression_points.values, sizeof(ws1.world.nation.m_suppression_points.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_diplomatic_points.values, &ws2.world.nation.m_diplomatic_points.values, sizeof(ws1.world.nation.m_diplomatic_points.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_leadership_points.values, &ws2.world.nation.m_leadership_points.values, sizeof(ws1.world.nation.m_leadership_points.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_naval_supply_points.values, &ws2.world.nation.m_naval_supply_points.values, sizeof(ws1.world.nation.m_naval_supply_points.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_used_naval_supply_points.values, &ws2.world.nation.m_used_naval_supply_points.values, sizeof(ws1.world.nation.m_used_naval_supply_points.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_has_flash_point_state.values, &ws2.world.nation.m_has_flash_point_state.values, sizeof(ws1.world.nation.m_has_flash_point_state.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_is_target_of_some_cb.values, &ws2.world.nation.m_is_target_of_some_cb.values, sizeof(ws1.world.nation.m_is_target_of_some_cb.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_is_interesting.values, &ws2.world.nation.m_is_interesting.values, sizeof(ws1.world.nation.m_is_interesting.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_ai_is_threatened.values, &ws2.world.nation.m_ai_is_threatened.values, sizeof(ws1.world.nation.m_ai_is_threatened.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_ai_rival.values, &ws2.world.nation.m_ai_rival.values, sizeof(ws1.world.nation.m_ai_rival.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.nation.m_ai_home_port.values, &ws2.world.nation.m_ai_home_port.values, sizeof(ws1.world.nation.m_ai_home_port.values)) == 0);
	// obj diplomatic_relation
	REQUIRE(std::memcmp(&ws1.world.diplomatic_relation.m_related_nations.values, &ws2.world.diplomatic_relation.m_related_nations.values, sizeof(ws1.world.diplomatic_relation.m_related_nations.values)) == 0);
	// diplomatic_relation.diplomatic_pair is unique
	REQUIRE(std::memcmp(&ws1.world.diplomatic_relation.m_value.values, &ws2.world.diplomatic_relation.m_value.values, sizeof(ws1.world.diplomatic_relation.m_value.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.diplomatic_relation.m_are_allied.values, &ws2.world.diplomatic_relation.m_are_allied.values, sizeof(ws1.world.diplomatic_relation.m_are_allied.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.diplomatic_relation.m_truce_until.values, &ws2.world.diplomatic_relation.m_truce_until.values, sizeof(ws1.world.diplomatic_relation.m_truce_until.values)) == 0);
	// obj unilateral_relationship
	REQUIRE(std::memcmp(&ws1.world.unilateral_relationship.m_target.values, &ws2.world.unilateral_relationship.m_target.values, sizeof(ws1.world.unilateral_relationship.m_target.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.unilateral_relationship.m_source.values, &ws2.world.unilateral_relationship.m_source.values, sizeof(ws1.world.unilateral_relationship.m_source.values)) == 0);
	// unilateral_relationship.unilateral_pair is unique
	REQUIRE(std::memcmp(&ws1.world.unilateral_relationship.m_foreign_investment.values, &ws2.world.unilateral_relationship.m_foreign_investment.values, sizeof(ws1.world.unilateral_relationship.m_foreign_investment.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.unilateral_relationship.m_owns_debt_of.values, &ws2.world.unilateral_relationship.m_owns_debt_of.values, sizeof(ws1.world.unilateral_relationship.m_owns_debt_of.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.unilateral_relationship.m_military_access.values, &ws2.world.unilateral_relationship.m_military_access.values, sizeof(ws1.world.unilateral_relationship.m_military_access.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.unilateral_relationship.m_war_subsidies.values, &ws2.world.unilateral_relationship.m_war_subsidies.values, sizeof(ws1.world.unilateral_relationship.m_war_subsidies.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.unilateral_relationship.m_reparations.values, &ws2.world.unilateral_relationship.m_reparations.values, sizeof(ws1.world.unilateral_relationship.m_reparations.values)) == 0);
	// obj gp_relationship
	REQUIRE(std::memcmp(&ws1.world.gp_relationship.m_influence_target.values, &ws2.world.gp_relationship.m_influence_target.values, sizeof(ws1.world.gp_relationship.m_influence_target.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.gp_relationship.m_great_power.values, &ws2.world.gp_relationship.m_great_power.values, sizeof(ws1.world.gp_relationship.m_great_power.values)) == 0);
	// gp_relationship.gp_influence_pair is unique
	REQUIRE(std::memcmp(&ws1.world.gp_relationship.m_influence.values, &ws2.world.gp_relationship.m_influence.values, sizeof(ws1.world.gp_relationship.m_influence.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.gp_relationship.m_status.values, &ws2.world.gp_relationship.m_status.values, sizeof(ws1.world.gp_relationship.m_status.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.gp_relationship.m_penalty_expires_date.values, &ws2.world.gp_relationship.m_penalty_expires_date.values, sizeof(ws1.world.gp_relationship.m_penalty_expires_date.values)) == 0);
	// obj factory
	REQUIRE(std::memcmp(&ws1.world.factory.m_building_type.values, &ws2.world.factory.m_building_type.values, sizeof(ws1.world.factory.m_building_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory.m_level.values, &ws2.world.factory.m_level.values, sizeof(ws1.world.factory.m_level.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory.m_priority_low.values, &ws2.world.factory.m_priority_low.values, sizeof(ws1.world.factory.m_priority_low.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory.m_priority_high.values, &ws2.world.factory.m_priority_high.values, sizeof(ws1.world.factory.m_priority_high.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory.m_production_scale.values, &ws2.world.factory.m_production_scale.values, sizeof(ws1.world.factory.m_production_scale.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory.m_actual_production.values, &ws2.world.factory.m_actual_production.values, sizeof(ws1.world.factory.m_actual_production.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory.m_primary_employment.values, &ws2.world.factory.m_primary_employment.values, sizeof(ws1.world.factory.m_primary_employment.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory.m_secondary_employment.values, &ws2.world.factory.m_secondary_employment.values, sizeof(ws1.world.factory.m_secondary_employment.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory.m_triggered_modifiers.values, &ws2.world.factory.m_triggered_modifiers.values, sizeof(ws1.world.factory.m_triggered_modifiers.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory.m_full_profit.values, &ws2.world.factory.m_full_profit.values, sizeof(ws1.world.factory.m_full_profit.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory.m_subsidized.values, &ws2.world.factory.m_subsidized.values, sizeof(ws1.world.factory.m_subsidized.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.factory.m_unprofitable.values, &ws2.world.factory.m_unprofitable.values, sizeof(ws1.world.factory.m_unprofitable.values)) == 0);
	// obj factory_location
	// factory_location.factory is unique
	REQUIRE(std::memcmp(&ws1.world.factory_location.m_province.values, &ws2.world.factory_location.m_province.values, sizeof(ws1.world.factory_location.m_province.values)) == 0);
	// obj province_ownership
	// province_ownership.province is unique
	REQUIRE(std::memcmp(&ws1.world.province_ownership.m_nation.values, &ws2.world.province_ownership.m_nation.values, sizeof(ws1.world.province_ownership.m_nation.values)) == 0);
	// obj province_control
	// province_control.province is unique
	REQUIRE(std::memcmp(&ws1.world.province_control.m_nation.values, &ws2.world.province_control.m_nation.values, sizeof(ws1.world.province_control.m_nation.values)) == 0);
	// obj province_rebel_control
	// province_rebel_control.province is unique
	REQUIRE(std::memcmp(&ws1.world.province_rebel_control.m_rebel_faction.values, &ws2.world.province_rebel_control.m_rebel_faction.values, sizeof(ws1.world.province_rebel_control.m_rebel_faction.values)) == 0);
	// obj province_land_construction
	REQUIRE(std::memcmp(&ws1.world.province_land_construction.m_pop.values, &ws2.world.province_land_construction.m_pop.values, sizeof(ws1.world.province_land_construction.m_pop.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province_land_construction.m_nation.values, &ws2.world.province_land_construction.m_nation.values, sizeof(ws1.world.province_land_construction.m_nation.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province_land_construction.m_purchased_goods.values, &ws2.world.province_land_construction.m_purchased_goods.values, sizeof(ws1.world.province_land_construction.m_purchased_goods.values)) == 0);
	// province_land_construction.type is unique
	// obj province_naval_construction
	REQUIRE(std::memcmp(&ws1.world.province_naval_construction.m_province.values, &ws2.world.province_naval_construction.m_province.values, sizeof(ws1.world.province_naval_construction.m_province.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province_naval_construction.m_nation.values, &ws2.world.province_naval_construction.m_nation.values, sizeof(ws1.world.province_naval_construction.m_nation.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province_naval_construction.m_purchased_goods.values, &ws2.world.province_naval_construction.m_purchased_goods.values, sizeof(ws1.world.province_naval_construction.m_purchased_goods.values)) == 0);
	// province_naval_construction.type is unique
	// obj province_building_construction
	REQUIRE(std::memcmp(&ws1.world.province_building_construction.m_province.values, &ws2.world.province_building_construction.m_province.values, sizeof(ws1.world.province_building_construction.m_province.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province_building_construction.m_nation.values, &ws2.world.province_building_construction.m_nation.values, sizeof(ws1.world.province_building_construction.m_nation.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province_building_construction.m_purchased_goods.values, &ws2.world.province_building_construction.m_purchased_goods.values, sizeof(ws1.world.province_building_construction.m_purchased_goods.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province_building_construction.m_type.values, &ws2.world.province_building_construction.m_type.values, sizeof(ws1.world.province_building_construction.m_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.province_building_construction.m_is_pop_project.values, &ws2.world.province_building_construction.m_is_pop_project.values, sizeof(ws1.world.province_building_construction.m_is_pop_project.values)) == 0);
	// obj state_building_construction
	REQUIRE(std::memcmp(&ws1.world.state_building_construction.m_state.values, &ws2.world.state_building_construction.m_state.values, sizeof(ws1.world.state_building_construction.m_state.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_building_construction.m_nation.values, &ws2.world.state_building_construction.m_nation.values, sizeof(ws1.world.state_building_construction.m_nation.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_building_construction.m_purchased_goods.values, &ws2.world.state_building_construction.m_purchased_goods.values, sizeof(ws1.world.state_building_construction.m_purchased_goods.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_building_construction.m_type.values, &ws2.world.state_building_construction.m_type.values, sizeof(ws1.world.state_building_construction.m_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_building_construction.m_is_pop_project.values, &ws2.world.state_building_construction.m_is_pop_project.values, sizeof(ws1.world.state_building_construction.m_is_pop_project.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.state_building_construction.m_is_upgrade.values, &ws2.world.state_building_construction.m_is_upgrade.values, sizeof(ws1.world.state_building_construction.m_is_upgrade.values)) == 0);
	// obj overlord
	// overlord.subject is unique
	REQUIRE(std::memcmp(&ws1.world.overlord.m_ruler.values, &ws2.world.overlord.m_ruler.values, sizeof(ws1.world.overlord.m_ruler.values)) == 0);
	// obj rebel_faction
	REQUIRE(std::memcmp(&ws1.world.rebel_faction.m_type.values, &ws2.world.rebel_faction.m_type.values, sizeof(ws1.world.rebel_faction.m_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_faction.m_defection_target.values, &ws2.world.rebel_faction.m_defection_target.values, sizeof(ws1.world.rebel_faction.m_defection_target.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_faction.m_primary_culture.values, &ws2.world.rebel_faction.m_primary_culture.values, sizeof(ws1.world.rebel_faction.m_primary_culture.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_faction.m_primary_culture_group.values, &ws2.world.rebel_faction.m_primary_culture_group.values, sizeof(ws1.world.rebel_faction.m_primary_culture_group.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_faction.m_religion.values, &ws2.world.rebel_faction.m_religion.values, sizeof(ws1.world.rebel_faction.m_religion.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_faction.m_possible_regiments.values, &ws2.world.rebel_faction.m_possible_regiments.values, sizeof(ws1.world.rebel_faction.m_possible_regiments.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_faction.m_organization.values, &ws2.world.rebel_faction.m_organization.values, sizeof(ws1.world.rebel_faction.m_organization.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.rebel_faction.m_is_active.values, &ws2.world.rebel_faction.m_is_active.values, sizeof(ws1.world.rebel_faction.m_is_active.values)) == 0);
	// obj rebellion_within
	// rebellion_within.rebels is unique
	REQUIRE(std::memcmp(&ws1.world.rebellion_within.m_ruler.values, &ws2.world.rebellion_within.m_ruler.values, sizeof(ws1.world.rebellion_within.m_ruler.values)) == 0);
	// obj movement
	REQUIRE(std::memcmp(&ws1.world.movement.m_associated_issue_option.values, &ws2.world.movement.m_associated_issue_option.values, sizeof(ws1.world.movement.m_associated_issue_option.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.movement.m_associated_independence.values, &ws2.world.movement.m_associated_independence.values, sizeof(ws1.world.movement.m_associated_independence.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.movement.m_pop_support.values, &ws2.world.movement.m_pop_support.values, sizeof(ws1.world.movement.m_pop_support.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.movement.m_radicalism.values, &ws2.world.movement.m_radicalism.values, sizeof(ws1.world.movement.m_radicalism.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.movement.m_transient_radicalism.values, &ws2.world.movement.m_transient_radicalism.values, sizeof(ws1.world.movement.m_transient_radicalism.values)) == 0);
	// obj movement_within
	// movement_within.movement is unique
	REQUIRE(std::memcmp(&ws1.world.movement_within.m_nation.values, &ws2.world.movement_within.m_nation.values, sizeof(ws1.world.movement_within.m_nation.values)) == 0);
	// obj pop_movement_membership
	// pop_movement_membership.pop is unique
	REQUIRE(std::memcmp(&ws1.world.pop_movement_membership.m_movement.values, &ws2.world.pop_movement_membership.m_movement.values, sizeof(ws1.world.pop_movement_membership.m_movement.values)) == 0);
	// obj pop_rebellion_membership
	// pop_rebellion_membership.pop is unique
	REQUIRE(std::memcmp(&ws1.world.pop_rebellion_membership.m_rebel_faction.values, &ws2.world.pop_rebellion_membership.m_rebel_faction.values, sizeof(ws1.world.pop_rebellion_membership.m_rebel_faction.values)) == 0);
	// obj pop
	REQUIRE(std::memcmp(&ws1.world.pop.m_poptype.values, &ws2.world.pop.m_poptype.values, sizeof(ws1.world.pop.m_poptype.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_religion.values, &ws2.world.pop.m_religion.values, sizeof(ws1.world.pop.m_religion.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_culture.values, &ws2.world.pop.m_culture.values, sizeof(ws1.world.pop.m_culture.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_size.values, &ws2.world.pop.m_size.values, sizeof(ws1.world.pop.m_size.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_savings.values, &ws2.world.pop.m_savings.values, sizeof(ws1.world.pop.m_savings.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_consciousness.values, &ws2.world.pop.m_consciousness.values, sizeof(ws1.world.pop.m_consciousness.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_militancy.values, &ws2.world.pop.m_militancy.values, sizeof(ws1.world.pop.m_militancy.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_literacy.values, &ws2.world.pop.m_literacy.values, sizeof(ws1.world.pop.m_literacy.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_employment.values, &ws2.world.pop.m_employment.values, sizeof(ws1.world.pop.m_employment.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_life_needs_satisfaction.values, &ws2.world.pop.m_life_needs_satisfaction.values, sizeof(ws1.world.pop.m_life_needs_satisfaction.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_everyday_needs_satisfaction.values, &ws2.world.pop.m_everyday_needs_satisfaction.values, sizeof(ws1.world.pop.m_everyday_needs_satisfaction.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_luxury_needs_satisfaction.values, &ws2.world.pop.m_luxury_needs_satisfaction.values, sizeof(ws1.world.pop.m_luxury_needs_satisfaction.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_political_reform_desire.values, &ws2.world.pop.m_political_reform_desire.values, sizeof(ws1.world.pop.m_political_reform_desire.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_social_reform_desire.values, &ws2.world.pop.m_social_reform_desire.values, sizeof(ws1.world.pop.m_social_reform_desire.values)) == 0);
	REQUIRE(ws1.world.pop.m_demographics.size == ws2.world.pop.m_demographics.size);
	REQUIRE(std::memcmp(ws1.world.pop.m_demographics.values, ws2.world.pop.m_demographics.values, sizeof(decltype(ws1.world.pop.m_demographics.vptr(0))) * ws1.world.pop.m_demographics.size) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_dominant_ideology.values, &ws2.world.pop.m_dominant_ideology.values, sizeof(ws1.world.pop.m_dominant_ideology.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_dominant_issue_option.values, &ws2.world.pop.m_dominant_issue_option.values, sizeof(ws1.world.pop.m_dominant_issue_option.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.pop.m_is_primary_or_accepted_culture.values, &ws2.world.pop.m_is_primary_or_accepted_culture.values, sizeof(ws1.world.pop.m_is_primary_or_accepted_culture.values)) == 0);
	// obj pop_location
	// pop_location.pop is unique
	REQUIRE(std::memcmp(&ws1.world.pop_location.m_province.values, &ws2.world.pop_location.m_province.values, sizeof(ws1.world.pop_location.m_province.values)) == 0);
	// obj national_event
	REQUIRE(std::memcmp(&ws1.world.national_event.m_name.values, &ws2.world.national_event.m_name.values, sizeof(ws1.world.national_event.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_event.m_description.values, &ws2.world.national_event.m_description.values, sizeof(ws1.world.national_event.m_description.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_event.m_is_major.values, &ws2.world.national_event.m_is_major.values, sizeof(ws1.world.national_event.m_is_major.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_event.m_image.values, &ws2.world.national_event.m_image.values, sizeof(ws1.world.national_event.m_image.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_event.m_immediate_effect.values, &ws2.world.national_event.m_immediate_effect.values, sizeof(ws1.world.national_event.m_immediate_effect.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_event.m_options.values, &ws2.world.national_event.m_options.values, sizeof(ws1.world.national_event.m_options.values)) == 0);
	// obj provincial_event
	REQUIRE(std::memcmp(&ws1.world.provincial_event.m_name.values, &ws2.world.provincial_event.m_name.values, sizeof(ws1.world.provincial_event.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.provincial_event.m_description.values, &ws2.world.provincial_event.m_description.values, sizeof(ws1.world.provincial_event.m_description.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.provincial_event.m_options.values, &ws2.world.provincial_event.m_options.values, sizeof(ws1.world.provincial_event.m_options.values)) == 0);
	// obj free_national_event
	REQUIRE(std::memcmp(&ws1.world.free_national_event.m_name.values, &ws2.world.free_national_event.m_name.values, sizeof(ws1.world.free_national_event.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_national_event.m_description.values, &ws2.world.free_national_event.m_description.values, sizeof(ws1.world.free_national_event.m_description.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_national_event.m_only_once.values, &ws2.world.free_national_event.m_only_once.values, sizeof(ws1.world.free_national_event.m_only_once.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_national_event.m_has_been_triggered.values, &ws2.world.free_national_event.m_has_been_triggered.values, sizeof(ws1.world.free_national_event.m_has_been_triggered.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_national_event.m_is_major.values, &ws2.world.free_national_event.m_is_major.values, sizeof(ws1.world.free_national_event.m_is_major.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_national_event.m_image.values, &ws2.world.free_national_event.m_image.values, sizeof(ws1.world.free_national_event.m_image.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_national_event.m_trigger.values, &ws2.world.free_national_event.m_trigger.values, sizeof(ws1.world.free_national_event.m_trigger.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_national_event.m_mtth.values, &ws2.world.free_national_event.m_mtth.values, sizeof(ws1.world.free_national_event.m_mtth.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_national_event.m_immediate_effect.values, &ws2.world.free_national_event.m_immediate_effect.values, sizeof(ws1.world.free_national_event.m_immediate_effect.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_national_event.m_options.values, &ws2.world.free_national_event.m_options.values, sizeof(ws1.world.free_national_event.m_options.values)) == 0);
	// obj free_provincial_event
	REQUIRE(std::memcmp(&ws1.world.free_provincial_event.m_name.values, &ws2.world.free_provincial_event.m_name.values, sizeof(ws1.world.free_provincial_event.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_provincial_event.m_description.values, &ws2.world.free_provincial_event.m_description.values, sizeof(ws1.world.free_provincial_event.m_description.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_provincial_event.m_only_once.values, &ws2.world.free_provincial_event.m_only_once.values, sizeof(ws1.world.free_provincial_event.m_only_once.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_provincial_event.m_has_been_triggered.values, &ws2.world.free_provincial_event.m_has_been_triggered.values, sizeof(ws1.world.free_provincial_event.m_has_been_triggered.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_provincial_event.m_trigger.values, &ws2.world.free_provincial_event.m_trigger.values, sizeof(ws1.world.free_provincial_event.m_trigger.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_provincial_event.m_mtth.values, &ws2.world.free_provincial_event.m_mtth.values, sizeof(ws1.world.free_provincial_event.m_mtth.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.free_provincial_event.m_options.values, &ws2.world.free_provincial_event.m_options.values, sizeof(ws1.world.free_provincial_event.m_options.values)) == 0);
	// obj national_focus
	REQUIRE(std::memcmp(&ws1.world.national_focus.m_name.values, &ws2.world.national_focus.m_name.values, sizeof(ws1.world.national_focus.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_focus.m_icon.values, &ws2.world.national_focus.m_icon.values, sizeof(ws1.world.national_focus.m_icon.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_focus.m_type.values, &ws2.world.national_focus.m_type.values, sizeof(ws1.world.national_focus.m_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_focus.m_loyalty_value.values, &ws2.world.national_focus.m_loyalty_value.values, sizeof(ws1.world.national_focus.m_loyalty_value.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_focus.m_ideology.values, &ws2.world.national_focus.m_ideology.values, sizeof(ws1.world.national_focus.m_ideology.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_focus.m_limit.values, &ws2.world.national_focus.m_limit.values, sizeof(ws1.world.national_focus.m_limit.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_focus.m_promotion_type.values, &ws2.world.national_focus.m_promotion_type.values, sizeof(ws1.world.national_focus.m_promotion_type.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_focus.m_promotion_amount.values, &ws2.world.national_focus.m_promotion_amount.values, sizeof(ws1.world.national_focus.m_promotion_amount.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_focus.m_immigrant_attract.values, &ws2.world.national_focus.m_immigrant_attract.values, sizeof(ws1.world.national_focus.m_immigrant_attract.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.national_focus.m_railroads.values, &ws2.world.national_focus.m_railroads.values, sizeof(ws1.world.national_focus.m_railroads.values)) == 0);
	REQUIRE(ws1.world.national_focus.m_production_focus.size == ws2.world.national_focus.m_production_focus.size);
	REQUIRE(std::memcmp(ws1.world.national_focus.m_production_focus.values, ws2.world.national_focus.m_production_focus.values, sizeof(decltype(ws1.world.national_focus.m_production_focus.vptr(0))) * ws1.world.national_focus.m_production_focus.size) == 0);
	// obj decision
	REQUIRE(std::memcmp(&ws1.world.decision.m_name.values, &ws2.world.decision.m_name.values, sizeof(ws1.world.decision.m_name.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.decision.m_description.values, &ws2.world.decision.m_description.values, sizeof(ws1.world.decision.m_description.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.decision.m_image.values, &ws2.world.decision.m_image.values, sizeof(ws1.world.decision.m_image.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.decision.m_potential.values, &ws2.world.decision.m_potential.values, sizeof(ws1.world.decision.m_potential.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.decision.m_allow.values, &ws2.world.decision.m_allow.values, sizeof(ws1.world.decision.m_allow.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.decision.m_effect.values, &ws2.world.decision.m_effect.values, sizeof(ws1.world.decision.m_effect.values)) == 0);
	REQUIRE(std::memcmp(&ws1.world.decision.m_ai_will_do.values, &ws2.world.decision.m_ai_will_do.values, sizeof(ws1.world.decision.m_ai_will_do.values)) == 0);
}

TEST_CASE("sim_0", "[determinism]") {
	// Test that the game states are equal AFTER loading
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	game_state_1->game_seed = 808080;
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();
	game_state_2->game_seed = 808080;
	// should be equal in memory
	compare_game_states(*game_state_1, *game_state_2);
}

TEST_CASE("sim_1", "[determinism]") {
	// Test that the game states are equal after loading and performing 1 tick
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	game_state_1->game_seed = 808080;
	game_state_1->single_game_tick(); // 1 tick
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();
	game_state_2->game_seed = 808080;
	game_state_2->single_game_tick(); // 1 tick
	// should be equal in memory
	compare_game_states(*game_state_1, *game_state_2);
}

TEST_CASE("sim_2", "[determinism]") {
	// Test that the game states are equal after loading and performing 1 tick
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	game_state_1->game_seed = 808080;
	game_state_1->single_game_tick(); // 1 tick
	game_state_1->single_game_tick(); // 1 tick
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();
	game_state_2->game_seed = 808080;
	game_state_2->single_game_tick(); // 1 tick
	game_state_2->single_game_tick(); // 1 tick
	// should be equal in memory
	compare_game_states(*game_state_1, *game_state_2);
}
