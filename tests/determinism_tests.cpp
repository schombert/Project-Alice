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
	auto r1 = rng::get_random(*game_state, ~uint32_t(0x6a3f));
	auto r2 = rng::get_random(*game_state, ~uint32_t(0x6a3f));
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
	REQUIRE(f2 / f1 == 2.f);
}
TEST_CASE("fp_fmadd", "[determinism]") {
	UNOPTIMIZABLE_FLOAT(f1, 1.f);
	UNOPTIMIZABLE_FLOAT(f2, 2.f);
	UNOPTIMIZABLE_FLOAT(f3, 3.f);
	UNOPTIMIZABLE_FLOAT(f4, 4.f);

	float fmadd_1 = f1 * f2 + f3 * f4; //equation
	float fmadd_2 = f3 * f4 + f1 * f2; //reverse products
	float fmadd_3 = f1 * (f2 + (f3 * f4) / f1); //factorization by f1
	float fmadd_4 = f2 * (f1 + (f3 * f4) / f2); //factorization by f2
	float fmadd_5 = f3 * ((f1 * f2) / f3 + f4); //factorization by f3
	float fmadd_6 = f4 * ((f1 * f2) / f4 + f3); //factorization by f4

	REQUIRE(fmadd_1 == Approx(fmadd_2));
	REQUIRE(fmadd_1 == Approx(fmadd_3));
	REQUIRE(fmadd_1 == Approx(fmadd_4));
	REQUIRE(fmadd_1 == Approx(fmadd_5));
	REQUIRE(fmadd_1 == Approx(fmadd_6));
}

TEST_CASE("fp_fprec", "[determinism]") {
	UNOPTIMIZABLE_FLOAT(f1, 2.f);
	UNOPTIMIZABLE_FLOAT(f2, 4.f);
	UNOPTIMIZABLE_FLOAT(f3, 3.f);

	float fprec_1 = f1 * f2 / f3; //equation
	float fprec_2 = (f1 * f2) / f3; //same
	float fprec_3 = f1 * (f2 / f3); //same
	float fprec_4 = (f1 / f3) * f2; //same

	REQUIRE(fprec_1 == Approx(fprec_2));
	REQUIRE(fprec_1 == Approx(fprec_3));
	REQUIRE(fprec_1 == Approx(fprec_4));
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
		REQUIRE(math::sin(f1) == Approx(std::sin(f1)).margin(0.01f));
		REQUIRE(math::cos(f1) == Approx(std::cos(f1)).margin(0.01f));
	}
}

#undef UNOPTIMIZABLE_FLOAT

void compare_game_states(sys::state& ws1, sys::state& ws2) {
	auto ymd = ws1.current_date.to_ymd(ws1.start_date);
	INFO(ymd.year << "." << ymd.month << "." << ymd.day);

	// REQUIRE(std::memcmp(tmp1.get(), tmp2.get(), sizeof_save_section(ws1)) == 0);
	REQUIRE(ws1.get_mp_state_checksum().to_string() == ws2.get_mp_state_checksum().to_string());
}

void checked_pop_update(sys::state& ws) {
	auto ymd_date = ws.current_date.to_ymd(ws.start_date);
	auto month_start = sys::year_month_day{ymd_date.year, ymd_date.month, uint16_t(1)};
	auto next_month_start = sys::year_month_day{ymd_date.year, uint16_t(ymd_date.month + 1), uint16_t(1)};
	auto const days_in_month = uint32_t(sys::days_difference(month_start, next_month_start));

	// pop update:
	static demographics::ideology_buffer idbuf(ws);
	static demographics::issues_buffer isbuf(ws);
	static demographics::promotion_buffer pbuf;
	static demographics::assimilation_buffer abuf;
	static demographics::migration_buffer mbuf;
	static demographics::migration_buffer cmbuf;
	static demographics::migration_buffer imbuf;

	idbuf = demographics::ideology_buffer(ws);
	isbuf = demographics::issues_buffer(ws);

	// calculate complex changes in parallel where we can, but don't actually apply the results
	// instead, the changes are saved to be applied only after all triggers have been evaluated
	concurrency::parallel_for(0, 7, [&](int32_t index) {
		switch(index) {
		case 0:
		{
			auto o = uint32_t(ymd_date.day);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_ideologies(ws, o, days_in_month, idbuf);
			break;
		}
		case 1:
		{
			auto o = uint32_t(ymd_date.day + 1);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_issues(ws, o, days_in_month, isbuf);
			break;
		}
		case 2:
		{
			auto o = uint32_t(ymd_date.day + 6);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_type_changes(ws, o, days_in_month, pbuf);
			break;
		}
		case 3:
		{
			auto o = uint32_t(ymd_date.day + 7);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_assimilation(ws, o, days_in_month, abuf);
			break;
		}
		case 4:
		{
			auto o = uint32_t(ymd_date.day + 8);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_internal_migration(ws, o, days_in_month, mbuf);
			break;
		}
		case 5:
		{
			auto o = uint32_t(ymd_date.day + 9);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_colonial_migration(ws, o, days_in_month, cmbuf);
			break;
		}
		case 6:
		{
			auto o = uint32_t(ymd_date.day + 10);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_immigration(ws, o, days_in_month, imbuf);
			break;
		}
		}
	});

	// apply in parallel where we can
	concurrency::parallel_for(0, 7, [&](int32_t index) {
		switch(index) {
		case 0:
		{
			auto o = uint32_t(ymd_date.day + 0);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::apply_ideologies(ws, o, days_in_month, idbuf);
			break;
		}
		case 1:
		{
			auto o = uint32_t(ymd_date.day + 1);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::apply_issues(ws, o, days_in_month, isbuf);
			break;
		}
		case 2:
		{
			auto o = uint32_t(ymd_date.day + 2);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_militancy(ws, o, days_in_month);
			break;
		}
		case 3:
		{
			auto o = uint32_t(ymd_date.day + 3);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_consciousness(ws, o, days_in_month);
			break;
		}
		case 4:
		{
			auto o = uint32_t(ymd_date.day + 4);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_literacy(ws, o, days_in_month);
			break;
		}
		case 5:
		{
			auto o = uint32_t(ymd_date.day + 5);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_growth(ws, o, days_in_month);
			break;
		}
		case 6:
			province::ve_for_each_land_province(ws,
					[&](auto ids) { ws.world.province_set_daily_net_migration(ids, ve::fp_vector{}); });
			break;
		case 7:
			province::ve_for_each_land_province(ws,
					[&](auto ids) { ws.world.province_set_daily_net_immigration(ids, ve::fp_vector{}); });
			break;
		}
	});

	// because they may add pops, these changes must be applied sequentially
	{
		auto o = uint32_t(ymd_date.day + 6);
		if(o >= days_in_month)
			o -= days_in_month;
		demographics::apply_type_changes(ws, o, days_in_month, pbuf);
	}
	{
		auto o = uint32_t(ymd_date.day + 7);
		if(o >= days_in_month)
			o -= days_in_month;
		demographics::apply_assimilation(ws, o, days_in_month, abuf);
	}
	{
		auto o = uint32_t(ymd_date.day + 8);
		if(o >= days_in_month)
			o -= days_in_month;
		demographics::apply_internal_migration(ws, o, days_in_month, mbuf);
	}
	{
		auto o = uint32_t(ymd_date.day + 9);
		if(o >= days_in_month)
			o -= days_in_month;
		demographics::apply_colonial_migration(ws, o, days_in_month, cmbuf);
	}
	{
		auto o = uint32_t(ymd_date.day + 10);
		if(o >= days_in_month)
			o -= days_in_month;
		demographics::apply_immigration(ws, o, days_in_month, imbuf);
	}
}

void checked_single_tick(sys::state& ws1, sys::state& ws2) {
	// do update logic
	province::update_connected_regions(ws1);
	province::update_connected_regions(ws2);
	compare_game_states(ws1, ws2);
	province::update_cached_values(ws1);
	province::update_cached_values(ws2);
	compare_game_states(ws1, ws2);
	nations::update_cached_values(ws1);
	nations::update_cached_values(ws1);
	compare_game_states(ws1, ws2);

	ws1.current_date += 1;
	ws2.current_date += 1;

	auto ymd_date = ws1.current_date.to_ymd(ws1.start_date);

	diplomatic_message::update_pending(ws1);
	diplomatic_message::update_pending(ws2);
	compare_game_states(ws1, ws2);

	auto month_start = sys::year_month_day{ ymd_date.year, ymd_date.month, uint16_t(1) };
	auto next_month_start = sys::year_month_day{ ymd_date.year, uint16_t(ymd_date.month + 1), uint16_t(1) };
	auto const days_in_month = uint32_t(sys::days_difference(month_start, next_month_start));

	demographics::remove_size_zero_pops(ws1);
	demographics::remove_size_zero_pops(ws2);
	compare_game_states(ws1, ws2);

	// basic repopulation of demographics derived values
	demographics::regenerate_from_pop_data<true>(ws1);
	demographics::regenerate_from_pop_data<true>(ws2);
	compare_game_states(ws1, ws2);

	// values updates pass 1 (mostly trivial things, can be done in parallel)
	concurrency::parallel_for(0, 16, [&](int32_t index) {
		switch(index) {
		case 0:
			ai::refresh_home_ports(ws1);
			ai::refresh_home_ports(ws2);
			break;
		case 1:
			nations::update_research_points(ws1);
			nations::update_research_points(ws2);
			break;
		case 2:
			military::regenerate_land_unit_average(ws1);
			military::regenerate_land_unit_average(ws2);
			break;
		case 3:
			military::regenerate_ship_scores(ws1);
			military::regenerate_ship_scores(ws2);
			break;
		case 4:
			nations::update_industrial_scores(ws1);
			nations::update_industrial_scores(ws2);
			break;
		case 5:
			military::update_naval_supply_points(ws1);
			military::update_naval_supply_points(ws2);
			break;
		case 6:
			military::update_all_recruitable_regiments(ws1);
			military::update_all_recruitable_regiments(ws2);
			break;
		case 7:
			military::regenerate_total_regiment_counts(ws1);
			military::regenerate_total_regiment_counts(ws2);
			break;
		case 8:
			break;
		case 9:
			economy::update_employment(ws1);
			economy::update_employment(ws2);
			break;
		case 10:
			nations::update_national_administrative_efficiency(ws1);
			nations::update_administrative_efficiency(ws1);
			rebel::daily_update_rebel_organization(ws1);

			nations::update_national_administrative_efficiency(ws2);
			nations::update_administrative_efficiency(ws2);
			rebel::daily_update_rebel_organization(ws2);
			break;
		case 11:
			military::daily_leaders_update(ws1);
			military::daily_leaders_update(ws2);
			break;
		case 12:
			politics::daily_party_loyalty_update(ws1);
			politics::daily_party_loyalty_update(ws2);
			break;
		case 13:
			nations::daily_update_flashpoint_tension(ws1);
			nations::daily_update_flashpoint_tension(ws2);
			break;
		case 14:
			military::update_ticking_war_score(ws1);
			military::update_ticking_war_score(ws2);
			break;
		case 15:
			military::increase_dig_in(ws1);
			military::increase_dig_in(ws2);
			break;
		case 16:
			military::recover_org(ws1);
			military::recover_org(ws2);
			break;
		}
	});
	compare_game_states(ws1, ws2);

	economy::daily_update(ws1, false, 1.f);
	economy::daily_update(ws2, false, 1.f);
	compare_game_states(ws1, ws2);

	military::update_siege_progress(ws1);
	military::update_siege_progress(ws2);
	compare_game_states(ws1, ws2);
	military::update_movement(ws1);
	military::update_movement(ws2);
	compare_game_states(ws1, ws2);
	military::update_naval_battles(ws1);
	military::update_naval_battles(ws2);
	compare_game_states(ws1, ws2);
	military::update_land_battles(ws1);
	military::update_land_battles(ws2);
	compare_game_states(ws1, ws2);
	military::advance_mobilizations(ws1);
	military::advance_mobilizations(ws2);
	compare_game_states(ws1, ws2);

	event::update_events(ws1);
	event::update_events(ws2);
	compare_game_states(ws1, ws2);

	culture::update_research(ws1, uint32_t(ymd_date.year));
	culture::update_research(ws2, uint32_t(ymd_date.year));
	compare_game_states(ws1, ws2);

	nations::update_military_scores(ws1); // depends on ship score, land unit average
	nations::update_military_scores(ws2); // depends on ship score, land unit average
	compare_game_states(ws1, ws2);
	nations::update_rankings(ws1);				// depends on industrial score, military scores
	nations::update_rankings(ws2);				// depends on industrial score, military scores
	compare_game_states(ws1, ws2);
	nations::update_great_powers(ws1);		// depends on rankings
	nations::update_great_powers(ws2);		// depends on rankings
	compare_game_states(ws1, ws2);
	nations::update_influence(ws1);				// depends on rankings, great powers
	nations::update_influence(ws2);				// depends on rankings, great powers
	compare_game_states(ws1, ws2);

	province::update_colonization(ws1);
	province::update_colonization(ws2);
	compare_game_states(ws1, ws2);
	military::update_cbs(ws1); // may add/remove cbs to a nation
	military::update_cbs(ws2); // may add/remove cbs to a nation
	compare_game_states(ws1, ws2);

	nations::update_crisis(ws1);
	nations::update_crisis(ws2);
	compare_game_states(ws1, ws2);
	politics::update_elections(ws1);
	politics::update_elections(ws2);
	compare_game_states(ws1, ws2);

	//
	//if(ws1.current_date.value % 4 == 0) {
		ai::update_ai_colonial_investment(ws1);
		ai::update_ai_colonial_investment(ws2);
		compare_game_states(ws1, ws2);
	//}

	// Once per month updates, spread out over the month
	//switch(ymd_date.day) {
	for(int32_t index = 0; index <= 31; index++) {
		switch(index) {
		case 1:
			nations::update_monthly_points(ws1);
			nations::update_monthly_points(ws2);
			break;
		case 2:
			sys::update_modifier_effects(ws1);
			sys::update_modifier_effects(ws2);
			break;
		case 3:
			military::monthly_leaders_update(ws1);
			military::monthly_leaders_update(ws2);
			compare_game_states(ws1, ws2);
			ai::add_gw_goals(ws1);
			ai::add_gw_goals(ws2);
			break;
		case 4:
			military::reinforce_regiments(ws1);
			military::reinforce_regiments(ws2);
			compare_game_states(ws1, ws2);
			ai::make_defense(ws1);
			ai::make_defense(ws2);
			break;
		case 5:
			rebel::update_movements(ws1);
			rebel::update_movements(ws2);
			compare_game_states(ws1, ws2);
			rebel::update_factions(ws1);
			rebel::update_factions(ws2);
			break;
		case 6:
			ai::form_alliances(ws1);
			ai::form_alliances(ws2);
			compare_game_states(ws1, ws2);
			ai::make_attacks(ws1);
			ai::make_attacks(ws2);
			break;
		case 7:
			ai::update_ai_general_status(ws1);
			ai::update_ai_general_status(ws2);
			break;
		case 8:
			military::apply_attrition(ws1);
			military::apply_attrition(ws2);
			break;
		case 9:
			military::repair_ships(ws1);
			military::repair_ships(ws2);
			break;
		case 10:
			province::update_crimes(ws1);
			province::update_crimes(ws2);
			break;
		case 11:
			province::update_nationalism(ws1);
			province::update_nationalism(ws2);
			break;
		case 12:
			ai::update_ai_research(ws1);
			ai::update_ai_research(ws2);
			compare_game_states(ws1, ws2);
			rebel::update_armies(ws1);
			rebel::update_armies(ws2);
			compare_game_states(ws1, ws2);
			rebel::rebel_hunting_check(ws1);
			rebel::rebel_hunting_check(ws2);
			break;
		case 13:
			ai::perform_influence_actions(ws1);
			ai::perform_influence_actions(ws2);
			break;
		case 14:
			ai::update_focuses(ws1);
			ai::update_focuses(ws2);
			break;
		case 15:
			culture::discover_inventions(ws1);
			culture::discover_inventions(ws2);
			break;
		case 16:
			ai::take_ai_decisions(ws1);
			ai::take_ai_decisions(ws2);
			break;
		case 17:
			ai::build_ships(ws1);
			ai::build_ships(ws2);
			compare_game_states(ws1, ws2);
			ai::update_land_constructions(ws1);
			ai::update_land_constructions(ws2);
			break;
		case 18:
			ai::update_ai_econ_construction(ws1);
			ai::update_ai_econ_construction(ws2);
			break;
		case 19:
			ai::update_budget(ws1);
			ai::update_budget(ws2);
		case 20:
			nations::update_flashpoint_tags(ws1);		
			nations::monthly_flashpoint_update(ws1);
			nations::update_flashpoint_tags(ws2);
			nations::monthly_flashpoint_update(ws2);
			compare_game_states(ws1, ws2);
			ai::make_defense(ws1);
			ai::make_defense(ws2);
			break;
		case 21:
			ai::update_ai_colony_starting(ws1);
			ai::update_ai_colony_starting(ws2);
			break;
		case 22:
			ai::take_reforms(ws1);
			ai::take_reforms(ws2);
			break;
		case 23:
			ai::civilize(ws1);
			ai::civilize(ws2);
			compare_game_states(ws1, ws2);
			ai::make_war_decs(ws1);
			ai::make_war_decs(ws2);
			break;
		case 24:
			rebel::execute_rebel_victories(ws1);
			rebel::execute_rebel_victories(ws2);
			compare_game_states(ws1, ws2);
			ai::make_attacks(ws1);
			ai::make_attacks(ws2);
			compare_game_states(ws1, ws2);
			rebel::update_armies(ws1);
			rebel::update_armies(ws2);
			compare_game_states(ws1, ws2);
			rebel::rebel_hunting_check(ws1);
			rebel::rebel_hunting_check(ws2);
			break;
		case 25:
			rebel::execute_province_defections(ws1);
			rebel::execute_province_defections(ws2);
			break;
		case 26:
			ai::make_peace_offers(ws1);
			ai::make_peace_offers(ws2);
			break;
		case 27:
			ai::update_crisis_leaders(ws1);
			ai::update_crisis_leaders(ws2);
			break;
		case 28:
			rebel::rebel_risings_check(ws1);
			rebel::rebel_risings_check(ws2);
			break;
		case 29:
			ai::update_war_intervention(ws1);
			ai::update_war_intervention(ws2);
			break;
		case 30:
			ai::update_ships(ws1);
			ai::update_ships(ws2);
			compare_game_states(ws1, ws2);
			rebel::update_armies(ws1);
			rebel::update_armies(ws2);
			compare_game_states(ws1, ws2);
			rebel::rebel_hunting_check(ws1);
			rebel::rebel_hunting_check(ws2);
			break;
		case 31:
			ai::update_cb_fabrication(ws1);
			ai::update_cb_fabrication(ws2);
			break;
		default:
			break;
		}
		compare_game_states(ws1, ws2);
	}

	military::apply_regiment_damage(ws1);
	military::apply_regiment_damage(ws2);
	compare_game_states(ws1, ws2);

	//if(ymd_date.day == 1) {
	//	if(ymd_date.month == 1) {
			// yearly update : redo the upper house
			for(auto n : ws1.world.in_nation) {
				politics::recalculate_upper_house(ws1, n);
			}
			for(auto n : ws2.world.in_nation) {
				politics::recalculate_upper_house(ws2, n);
			}
			compare_game_states(ws1, ws2);

			ai::update_influence_priorities(ws1);
			ai::update_influence_priorities(ws2);
			compare_game_states(ws1, ws2);
	//	}
	//	if(ymd_date.month == 6) {
			ai::update_influence_priorities(ws1);
			ai::update_influence_priorities(ws2);
			compare_game_states(ws1, ws2);
	//	}
		//if(ymd_date.month == 2) {
			ai::upgrade_colonies(ws1);
			ai::upgrade_colonies(ws2);
			compare_game_states(ws1, ws2);
		//}
	//}

	ai::general_ai_unit_tick(ws1);
	ai::general_ai_unit_tick(ws2);
	compare_game_states(ws1, ws2);

	ai::daily_cleanup(ws1);
	ai::daily_cleanup(ws2);
	compare_game_states(ws1, ws2);
}
// supposed to be a as-close-as-possible clone of the real deal, with compare_game_state calls inbetween
void checked_single_tick_advanced(sys::state& state1, sys::state& state2) {
	// do update logic

	state1.current_date += 1;
	state2.current_date += 1;

	auto ymd_date = state1.current_date.to_ymd(state1.start_date);

	diplomatic_message::update_pending(state1);
	diplomatic_message::update_pending(state2);

	auto month_start = sys::year_month_day{ ymd_date.year, ymd_date.month, uint16_t(1) };
	auto next_month_start = ymd_date.month != 12 ? sys::year_month_day{ ymd_date.year, uint16_t(ymd_date.month + 1), uint16_t(1) } : sys::year_month_day{ ymd_date.year + 1, uint16_t(1), uint16_t(1) };
	auto const days_in_month = uint32_t(sys::days_difference(month_start, next_month_start));

	compare_game_states(state1, state2);

	// pop update:
	static demographics::ideology_buffer idbuf1(state1);
	static demographics::issues_buffer isbuf1(state1);
	static demographics::promotion_buffer pbuf1;
	static demographics::assimilation_buffer abuf1;
	static demographics::migration_buffer mbuf1;
	static demographics::migration_buffer cmbuf1;
	static demographics::migration_buffer imbuf1;


	static demographics::ideology_buffer idbuf2(state2);
	static demographics::issues_buffer isbuf2(state2);
	static demographics::promotion_buffer pbuf2;
	static demographics::assimilation_buffer abuf2;
	static demographics::migration_buffer mbuf2;
	static demographics::migration_buffer cmbuf2;
	static demographics::migration_buffer imbuf2;

	// calculate complex changes in parallel where we can, but don't actually apply the results
	// instead, the changes are saved to be applied only after all triggers have been evaluated

	concurrency::parallel_for(0, 7, [&](int32_t index) {
		switch(index) {
		case 0:
		{
			auto o = uint32_t(ymd_date.day);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_ideologies(state1, o, days_in_month, idbuf1);
			demographics::update_ideologies(state2, o, days_in_month, idbuf2);
			break;
		}
		case 1:
		{
			auto o = uint32_t(ymd_date.day + 1);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_issues(state1, o, days_in_month, isbuf1);
			demographics::update_issues(state2, o, days_in_month, isbuf2);
			break;
		}
		case 2:
		{
			auto o = uint32_t(ymd_date.day + 6);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_type_changes(state1, o, days_in_month, pbuf1);
			demographics::update_type_changes(state2, o, days_in_month, pbuf2);
			break;
		}
		case 3:
		{
			auto o = uint32_t(ymd_date.day + 7);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_assimilation(state1, o, days_in_month, abuf1);
			demographics::update_assimilation(state2, o, days_in_month, abuf2);
			break;
		}
		case 4:
		{
			auto o = uint32_t(ymd_date.day + 8);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_internal_migration(state1, o, days_in_month, mbuf1);
			demographics::update_internal_migration(state2, o, days_in_month, mbuf2);
			break;
		}
		case 5:
		{
			auto o = uint32_t(ymd_date.day + 9);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_colonial_migration(state1, o, days_in_month, cmbuf1);
			demographics::update_colonial_migration(state2, o, days_in_month, cmbuf2);
			break;
		}
		case 6:
		{
			auto o = uint32_t(ymd_date.day + 10);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_immigration(state1, o, days_in_month, imbuf1);
			demographics::update_immigration(state2, o, days_in_month, imbuf2);
			break;
		}
		default:
			break;
		}
	});

	// apply in parallel where we can
	concurrency::parallel_for(0, 8, [&](int32_t index) {
		switch(index) {
		case 0:
		{
			auto o = uint32_t(ymd_date.day + 0);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::apply_ideologies(state1, o, days_in_month, idbuf1);
			demographics::apply_ideologies(state2, o, days_in_month, idbuf2);
			break;
		}
		case 1:
		{
			auto o = uint32_t(ymd_date.day + 1);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::apply_issues(state1, o, days_in_month, isbuf1);
			demographics::apply_issues(state2, o, days_in_month, isbuf2);
			break;
		}
		case 2:
		{
			auto o = uint32_t(ymd_date.day + 2);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_militancy(state1, o, days_in_month);
			demographics::update_militancy(state2, o, days_in_month);
			break;
		}
		case 3:
		{
			auto o = uint32_t(ymd_date.day + 3);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_consciousness(state1, o, days_in_month);
			demographics::update_consciousness(state2, o, days_in_month);
			break;
		}
		case 4:
		{
			auto o = uint32_t(ymd_date.day + 4);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_literacy(state1, o, days_in_month);
			demographics::update_literacy(state2, o, days_in_month);
			break;
		}
		case 5:
		{
			auto o = uint32_t(ymd_date.day + 5);
			if(o >= days_in_month)
				o -= days_in_month;
			demographics::update_growth(state1, o, days_in_month);
			demographics::update_growth(state2, o, days_in_month);
			break;
		}
		case 6:
			province::ve_for_each_land_province(state1,
					[&](auto ids) { state1.world.province_set_daily_net_migration(ids, ve::fp_vector{}); });
			province::ve_for_each_land_province(state2,
					[&](auto ids) { state2.world.province_set_daily_net_migration(ids, ve::fp_vector{}); });
			break;
		case 7:
			province::ve_for_each_land_province(state1,
					[&](auto ids) { state1.world.province_set_daily_net_immigration(ids, ve::fp_vector{}); });
			province::ve_for_each_land_province(state2,
					[&](auto ids) { state2.world.province_set_daily_net_immigration(ids, ve::fp_vector{}); });
			break;
		default:
			break;
		}
	});

	compare_game_states(state1, state2);





	// because they may add pops, these changes must be applied sequentially
	{
		auto o = uint32_t(ymd_date.day + 6);
		if(o >= days_in_month)
			o -= days_in_month;
		demographics::apply_type_changes(state1, o, days_in_month, pbuf1);
		demographics::apply_type_changes(state2, o, days_in_month, pbuf2);
	}
	{
		auto o = uint32_t(ymd_date.day + 7);
		if(o >= days_in_month)
			o -= days_in_month;
		demographics::apply_assimilation(state1, o, days_in_month, abuf1);
		demographics::apply_assimilation(state2, o, days_in_month, abuf2);
	}
	{
		auto o = uint32_t(ymd_date.day + 8);
		if(o >= days_in_month)
			o -= days_in_month;
		demographics::apply_internal_migration(state1, o, days_in_month, mbuf1);
		demographics::apply_internal_migration(state2, o, days_in_month, mbuf2);
	}
	{
		auto o = uint32_t(ymd_date.day + 9);
		if(o >= days_in_month)
			o -= days_in_month;
		demographics::apply_colonial_migration(state1, o, days_in_month, cmbuf1);
		demographics::apply_colonial_migration(state2, o, days_in_month, cmbuf2);
	}
	{
		auto o = uint32_t(ymd_date.day + 10);
		if(o >= days_in_month)
			o -= days_in_month;
		demographics::apply_immigration(state1, o, days_in_month, imbuf1);
		demographics::apply_immigration(state2, o, days_in_month, imbuf2);
	}

	demographics::remove_size_zero_pops(state1);
	demographics::remove_size_zero_pops(state2);
	compare_game_states(state1, state2);

	// basic repopulation of demographics derived values

	int64_t pc_difference1 = 0;
	int64_t pc_difference2 = 0;

	if(state1.network_mode != sys::network_mode_type::single_player)
		demographics::regenerate_from_pop_data_daily(state1);
	if(state2.network_mode != sys::network_mode_type::single_player)
		demographics::regenerate_from_pop_data_daily(state2);
	compare_game_states(state1, state2);

	//
	// ALTERNATE PAR DEMO START POINT A
	//

	concurrency::parallel_invoke([&]() {
		// values updates pass 1 (mostly trivial things, can be done in parallel)
		concurrency::parallel_for(0, 15, [&](int32_t index) {
			switch(index) {
			case 0:
				ai::refresh_home_ports(state1);
				ai::refresh_home_ports(state2);
				break;
			case 1:
				// Instant research cheat
				//state 1
				for(auto n : state1.cheat_data.instant_research_nations) {
					auto tech = state1.world.nation_get_current_research(n);
					if(tech.is_valid()) {
						float points = culture::effective_technology_rp_cost(state1, state1.current_date.to_ymd(state1.start_date).year, n, tech);
						state1.world.nation_set_research_points(n, points);
					}
				}
				nations::update_research_points(state1);
				// state 2
				for(auto n : state2.cheat_data.instant_research_nations) {
					auto tech = state2.world.nation_get_current_research(n);
					if(tech.is_valid()) {
						float points = culture::effective_technology_rp_cost(state2, state2.current_date.to_ymd(state2.start_date).year, n, tech);
						state2.world.nation_set_research_points(n, points);
					}
				}
				nations::update_research_points(state2);

				break;
			case 2:
				military::regenerate_land_unit_average(state1);
				military::regenerate_land_unit_average(state2);
				break;
			case 3:
				military::regenerate_ship_scores(state1);
				military::regenerate_ship_scores(state2);
				break;
			case 4:
				military::update_naval_supply_points(state1);
				military::update_naval_supply_points(state2);
				break;
			case 5:
				military::update_all_recruitable_regiments(state1);
				military::update_all_recruitable_regiments(state2);
				break;
			case 6:
				military::regenerate_total_regiment_counts(state1);
				military::regenerate_total_regiment_counts(state2);
				break;
			case 7:
				economy::update_employment(state1);
				economy::update_employment(state2);
				break;
			case 8:
				nations::update_national_administrative_efficiency(state1);
				nations::update_administrative_efficiency(state1);

				nations::update_national_administrative_efficiency(state2);
				nations::update_administrative_efficiency(state2);
				rebel::daily_update_rebel_organization(state1);
				rebel::daily_update_rebel_organization(state2);
				break;
			case 9:
				military::daily_leaders_update(state1);
				military::daily_leaders_update(state2);
				break;
			case 10:
				politics::daily_party_loyalty_update(state1);
				politics::daily_party_loyalty_update(state2);
				break;
			case 11:
				nations::daily_update_flashpoint_tension(state1);
				nations::daily_update_flashpoint_tension(state2);
				break;
			case 12:
				military::update_ticking_war_score(state1);
				military::update_ticking_war_score(state2);
				break;
			case 13:
				military::increase_dig_in(state1);
				military::increase_dig_in(state2);
				break;
			case 14:
				military::update_blockade_status(state1);
				military::update_blockade_status(state2);
				break;
			}
		});
		compare_game_states(state1, state2);

		economy::daily_update(state1, false, 1.f);
		economy::daily_update(state2, false, 1.f);
		compare_game_states(state1, state2);

		//
		// ALTERNATE PAR DEMO START POINT B
		//

		military::recover_org(state1);
		military::recover_org(state2);
		military::update_siege_progress(state1);
		military::update_siege_progress(state2);
		military::update_movement(state1);
		military::update_movement(state2);
		military::update_naval_battles(state1);
		military::update_naval_battles(state2);
		military::update_land_battles(state1);
		military::update_land_battles(state2);
		compare_game_states(state1, state2);

		military::advance_mobilizations(state1);
		military::advance_mobilizations(state2);

		province::update_colonization(state1);
		province::update_colonization(state2);
		military::update_cbs(state1); // may add/remove cbs to a nation
		military::update_cbs(state2);

		event::update_events(state1);
		event::update_events(state2);

		culture::update_research(state1, uint32_t(ymd_date.year));
		culture::update_research(state2, uint32_t(ymd_date.year));
		compare_game_states(state1, state2);

		nations::update_industrial_scores(state1);
		nations::update_industrial_scores(state2);
		nations::update_military_scores(state1); // depends on ship score, land unit average
		nations::update_military_scores(state2);
		nations::update_rankings(state1);				// depends on industrial score, military scores
		nations::update_rankings(state2);
		nations::update_great_powers(state1);		// depends on rankings
		nations::update_great_powers(state2);
		nations::update_influence(state1);				// depends on rankings, great powers
		nations::update_influence(state2);
		compare_game_states(state1, state2);


		nations::update_crisis(state1);
		nations::update_crisis(state2);
		politics::update_elections(state1);
		politics::update_elections(state2);

		if(state1.current_date.value % 4 == 0) {
			ai::update_ai_colonial_investment(state1);
		}
		if(state2.current_date.value % 4 == 0) {
			ai::update_ai_colonial_investment(state2);
		}
		if(state1.defines.alice_eval_ai_mil_everyday != 0.0f) {
			ai::make_defense(state1);
			ai::make_attacks(state1);
			ai::update_ships(state1);
		}
		if(state2.defines.alice_eval_ai_mil_everyday != 0.0f) {
			ai::make_defense(state2);
			ai::make_attacks(state2);
			ai::update_ships(state2);
		}
		ai::take_ai_decisions(state1);
		ai::take_ai_decisions(state2);
		compare_game_states(state1, state2);

		// Once per month updates, spread out over the month
		switch(ymd_date.day) {
		case 1:
			nations::update_monthly_points(state1);
			nations::update_monthly_points(state2);
			economy::prune_factories(state1);
			economy::prune_factories(state2);
			compare_game_states(state1, state2);
			break;
		case 2:
			province::update_blockaded_cache(state1);
			province::update_blockaded_cache(state2);
			sys::update_modifier_effects(state1);
			sys::update_modifier_effects(state2);
			compare_game_states(state1, state2);
			break;
		case 3:
			military::monthly_leaders_update(state1);
			military::monthly_leaders_update(state2);
			ai::add_gw_goals(state1);
			ai::add_gw_goals(state2);
			compare_game_states(state1, state2);
			break;
		case 4:
			military::reinforce_regiments(state1);
			military::reinforce_regiments(state2);
			if(!bool(state1.defines.alice_eval_ai_mil_everyday)) {
				ai::make_defense(state1);
			}
			if(!bool(state2.defines.alice_eval_ai_mil_everyday)) {
				ai::make_defense(state2);
			}
			compare_game_states(state1, state2);
			break;
		case 5:
			rebel::update_movements(state1);
			rebel::update_movements(state2);
			rebel::update_factions(state1);
			rebel::update_factions(state2);
			compare_game_states(state1, state2);
			break;
		case 6:
			ai::form_alliances(state1);
			ai::form_alliances(state2);
			if(!bool(state1.defines.alice_eval_ai_mil_everyday)) {
				ai::make_attacks(state1);
			}
			if(!bool(state2.defines.alice_eval_ai_mil_everyday)) {
				ai::make_attacks(state2);
			}
			compare_game_states(state1, state2);
			break;
		case 7:
			ai::update_ai_general_status(state1);
			ai::update_ai_general_status(state2);
			compare_game_states(state1, state2);
			break;
		case 8:
			military::apply_attrition(state1);
			military::apply_attrition(state2);
			compare_game_states(state1, state2);
			break;
		case 9:
			military::repair_ships(state1);
			military::repair_ships(state2);
			compare_game_states(state1, state2);
			break;
		case 10:
			province::update_crimes(state1);
			province::update_crimes(state2);
			compare_game_states(state1, state2);
			break;
		case 11:
			province::update_nationalism(state1);
			province::update_nationalism(state2);
			compare_game_states(state1, state2);
			break;
		case 12:
			ai::update_ai_research(state1);
			ai::update_ai_research(state2);
			rebel::update_armies(state1);
			rebel::update_armies(state2);
			rebel::rebel_hunting_check(state1);
			rebel::rebel_hunting_check(state2);
			compare_game_states(state1, state2);
			break;
		case 13:
			ai::perform_influence_actions(state1);
			ai::perform_influence_actions(state2);
			compare_game_states(state1, state2);
			break;
		case 14:
			ai::update_focuses(state1);
			ai::update_focuses(state2);
			compare_game_states(state1, state2);
			break;
		case 15:
			culture::discover_inventions(state1);
			culture::discover_inventions(state2);
			compare_game_states(state1, state2);
			break;
		case 16:
			ai::build_ships(state1);
			ai::build_ships(state2);
			compare_game_states(state1, state2);
			break;
		case 17:
			ai::update_land_constructions(state1);
			ai::update_land_constructions(state2);
			compare_game_states(state1, state2);
			break;
		case 18:
			ai::update_ai_econ_construction(state1);
			ai::update_ai_econ_construction(state2);
			compare_game_states(state1, state2);
			break;
		case 19:
			ai::update_budget(state1);
			ai::update_budget(state2);
			compare_game_states(state1, state2);
			break;
		case 20:
			nations::update_flashpoint_tags(state1);
			nations::monthly_flashpoint_update(state1);
			nations::update_flashpoint_tags(state2);
			nations::monthly_flashpoint_update(state2);
			if(!bool(state1.defines.alice_eval_ai_mil_everyday)) {
				ai::make_defense(state1);
			}
			if(!bool(state2.defines.alice_eval_ai_mil_everyday)) {
				ai::make_defense(state2);
			}
			compare_game_states(state1, state2);
			break;
		case 21:
			ai::update_ai_colony_starting(state1);
			ai::update_ai_colony_starting(state2);
			compare_game_states(state1, state2);
			break;
		case 22:
			ai::take_reforms(state1);
			ai::take_reforms(state2);
			compare_game_states(state1, state2);
			break;
		case 23:
			ai::civilize(state1);
			ai::civilize(state2);
			ai::make_war_decs(state1);
			ai::make_war_decs(state2);
			compare_game_states(state1, state2);
			break;
		case 24:
			rebel::execute_rebel_victories(state1);
			rebel::execute_rebel_victories(state2);
			if(!bool(state1.defines.alice_eval_ai_mil_everyday)) {
				ai::make_attacks(state1);
			}
			if(!bool(state2.defines.alice_eval_ai_mil_everyday)) {
				ai::make_attacks(state2);
			}
			rebel::update_armies(state1);
			rebel::update_armies(state2);
			rebel::rebel_hunting_check(state1);
			rebel::rebel_hunting_check(state2);
			compare_game_states(state1, state2);
			break;
		case 25:
			rebel::execute_province_defections(state1);
			rebel::execute_province_defections(state2);
			compare_game_states(state1, state2);
			break;
		case 26:
			ai::make_peace_offers(state1);
			ai::make_peace_offers(state2);
			compare_game_states(state1, state2);
			break;
		case 27:
			ai::update_crisis_leaders(state1);
			ai::update_crisis_leaders(state2);
			compare_game_states(state1, state2);
			break;
		case 28:
			rebel::rebel_risings_check(state1);
			rebel::rebel_risings_check(state2);
			compare_game_states(state1, state2);
			break;
		case 29:
			ai::update_war_intervention(state1);
			ai::update_war_intervention(state2);
			compare_game_states(state1, state2);
			break;
		case 30:
			if(!bool(state1.defines.alice_eval_ai_mil_everyday)) {
				ai::update_ships(state1);
			}
			if(!bool(state2.defines.alice_eval_ai_mil_everyday)) {
				ai::update_ships(state2);
			}
			rebel::update_armies(state1);
			rebel::update_armies(state2);
			rebel::rebel_hunting_check(state1);
			rebel::rebel_hunting_check(state2);
			compare_game_states(state1, state2);
			break;
		case 31:
			ai::update_cb_fabrication(state1);
			ai::update_cb_fabrication(state2);
			ai::update_ai_ruling_party(state1);
			ai::update_ai_ruling_party(state2);
			compare_game_states(state1, state2);
			break;
		default:
			break;
		}

		military::apply_regiment_damage(state1);
		military::apply_regiment_damage(state2);
		compare_game_states(state1, state2);

		if(ymd_date.day == 1) {
			if(ymd_date.month == 1) {
				state1.sprawl_update_requested.store(true);
				state2.sprawl_update_requested.store(true);

				// yearly update : redo the upper house
				for(auto n : state1.world.in_nation) {
					if(n.get_owned_province_count() != 0)
						politics::recalculate_upper_house(state1, n);
				}
				for(auto n : state2.world.in_nation) {
					if(n.get_owned_province_count() != 0)
						politics::recalculate_upper_house(state2, n);
				}

				ai::update_influence_priorities(state1);
				ai::update_influence_priorities(state2);
				nations::generate_sea_trade_routes(state1);
				nations::generate_sea_trade_routes(state2);
				nations::recalculate_markets_distance(state1);
				nations::recalculate_markets_distance(state2);
				compare_game_states(state1, state2);
			}
			if(ymd_date.month == 2) {
				ai::upgrade_colonies(state1);
				ai::upgrade_colonies(state2);
				compare_game_states(state1, state2);
			}
			if(ymd_date.month == 3) {
				if(!state1.national_definitions.on_quarterly_pulse.empty()) {
					for(auto n : state1.world.in_nation) {
						if(n.get_owned_province_count() > 0) {
							event::fire_fixed_event(state1, state1.national_definitions.on_quarterly_pulse, trigger::to_generic(n.id), event::slot_type::nation, n.id, -1, event::slot_type::none);
						}
					}
				}
				if(!state2.national_definitions.on_quarterly_pulse.empty()) {
					for(auto n : state2.world.in_nation) {
						if(n.get_owned_province_count() > 0) {
							event::fire_fixed_event(state2, state2.national_definitions.on_quarterly_pulse, trigger::to_generic(n.id), event::slot_type::nation, n.id, -1, event::slot_type::none);
						}
					}
				}
				compare_game_states(state1, state2);
			}

			if(ymd_date.month == 4 && ymd_date.year % 2 == 0) { // the purge
				demographics::remove_small_pops(state1);
				demographics::remove_small_pops(state2);
				compare_game_states(state1, state2);
			}
			if(ymd_date.month == 5) {
				ai::prune_alliances(state1);
				ai::prune_alliances(state2);
				ai::update_factory_types_priority(state1);
				ai::update_factory_types_priority(state2);
				compare_game_states(state1, state2);
			}
			if(ymd_date.month == 6) {
				if(!state1.national_definitions.on_quarterly_pulse.empty()) {
					for(auto n : state1.world.in_nation) {
						if(n.get_owned_province_count() > 0) {
							event::fire_fixed_event(state1, state1.national_definitions.on_quarterly_pulse, trigger::to_generic(n.id), event::slot_type::nation, n.id, -1, event::slot_type::none);
						}
					}
				}
				if(!state2.national_definitions.on_quarterly_pulse.empty()) {
					for(auto n : state2.world.in_nation) {
						if(n.get_owned_province_count() > 0) {
							event::fire_fixed_event(state2, state2.national_definitions.on_quarterly_pulse, trigger::to_generic(n.id), event::slot_type::nation, n.id, -1, event::slot_type::none);
						}
					}
				}
				compare_game_states(state1, state2);

			}

			if(ymd_date.month == 7) {
				ai::update_influence_priorities(state1);
				ai::update_influence_priorities(state2);
				nations::recalculate_markets_distance(state1);
				nations::recalculate_markets_distance(state2);
				compare_game_states(state1, state2);
			}
			if(ymd_date.month == 9) {
				if(!state1.national_definitions.on_quarterly_pulse.empty()) {
					for(auto n : state1.world.in_nation) {
						if(n.get_owned_province_count() > 0) {
							event::fire_fixed_event(state1, state1.national_definitions.on_quarterly_pulse, trigger::to_generic(n.id), event::slot_type::nation, n.id, -1, event::slot_type::none);
						}
					}
				}
				if(!state2.national_definitions.on_quarterly_pulse.empty()) {
					for(auto n : state2.world.in_nation) {
						if(n.get_owned_province_count() > 0) {
							event::fire_fixed_event(state2, state2.national_definitions.on_quarterly_pulse, trigger::to_generic(n.id), event::slot_type::nation, n.id, -1, event::slot_type::none);
						}
					}
				}
				compare_game_states(state1, state2);
			}
			if(ymd_date.month == 10) {
				if(!state1.national_definitions.on_yearly_pulse.empty()) {
					for(auto n : state1.world.in_nation) {
						if(n.get_owned_province_count() > 0) {
							event::fire_fixed_event(state1, state1.national_definitions.on_yearly_pulse, trigger::to_generic(n.id), event::slot_type::nation, n.id, -1, event::slot_type::none);
						}
					}
				}
				if(!state2.national_definitions.on_yearly_pulse.empty()) {
					for(auto n : state2.world.in_nation) {
						if(n.get_owned_province_count() > 0) {
							event::fire_fixed_event(state2, state2.national_definitions.on_yearly_pulse, trigger::to_generic(n.id), event::slot_type::nation, n.id, -1, event::slot_type::none);
						}
					}
				}
				compare_game_states(state1, state2);
			}
			if(ymd_date.month == 11) {
				ai::prune_alliances(state1);
				ai::prune_alliances(state2);
				compare_game_states(state1, state2);
			}
			if(ymd_date.month == 12) {
				if(!state1.national_definitions.on_quarterly_pulse.empty()) {
					for(auto n : state1.world.in_nation) {
						if(n.get_owned_province_count() > 0) {
							event::fire_fixed_event(state1, state1.national_definitions.on_quarterly_pulse, trigger::to_generic(n.id), event::slot_type::nation, n.id, -1, event::slot_type::none);
						}
					}
				}
				if(!state2.national_definitions.on_quarterly_pulse.empty()) {
					for(auto n : state2.world.in_nation) {
						if(n.get_owned_province_count() > 0) {
							event::fire_fixed_event(state2, state2.national_definitions.on_quarterly_pulse, trigger::to_generic(n.id), event::slot_type::nation, n.id, -1, event::slot_type::none);
						}
					}
				}
				compare_game_states(state1, state2);

			}
		}
		compare_game_states(state1, state2);

		ai::general_ai_unit_tick(state1);
		ai::general_ai_unit_tick(state2);

		compare_game_states(state1, state2);
		military::run_gc(state1);
		military::run_gc(state2);
		compare_game_states(state1, state2);

		nations::run_gc(state1);
		nations::run_gc(state2);
		military::update_blackflag_status(state1);
		military::update_blackflag_status(state2);
		ai::daily_cleanup(state1);
		ai::daily_cleanup(state2);
		compare_game_states(state1, state2);

		province::update_connected_regions(state1);
		province::update_connected_regions(state2);
		province::update_cached_values(state1);
		province::update_cached_values(state2);
		nations::update_cached_values(state1);
		nations::update_cached_values(state2);
		compare_game_states(state1, state2);

	},
	[&]() {
		if(state1.network_mode == sys::network_mode_type::single_player)
			demographics::alt_regenerate_from_pop_data_daily(state1);
		if(state2.network_mode == sys::network_mode_type::single_player)
			demographics::alt_regenerate_from_pop_data_daily(state2);

	}
	);
	compare_game_states(state1, state2);

	if(state1.network_mode == sys::network_mode_type::single_player) {
		state1.world.nation_swap_demographics_demographics_alt();
		state1.world.state_instance_swap_demographics_demographics_alt();
		state1.world.province_swap_demographics_demographics_alt();

		demographics::alt_demographics_update_extras(state1);
	}
	if(state2.network_mode == sys::network_mode_type::single_player) {
		state2.world.nation_swap_demographics_demographics_alt();
		state2.world.state_instance_swap_demographics_demographics_alt();
		state2.world.province_swap_demographics_demographics_alt();

		demographics::alt_demographics_update_extras(state2);
	}
	compare_game_states(state1, state2);

	/*
	 * END OF DAY: update cached data
	 */

	state1.player_data_cache.treasury_record[state1.current_date.value % 32] = nations::get_treasury(state1, state1.local_player_nation);
	state2.player_data_cache.treasury_record[state2.current_date.value % 32] = nations::get_treasury(state2, state2.local_player_nation);


	state1.player_data_cache.population_record[state1.current_date.value % 32] = state1.world.nation_get_demographics(state1.local_player_nation, demographics::total);
	state2.player_data_cache.population_record[state2.current_date.value % 32] = state2.world.nation_get_demographics(state2.local_player_nation, demographics::total);

	if((state1.current_date.value % 16) == 0) {
		auto index = economy::most_recent_price_record_index(state1);
		for(auto c : state1.world.in_commodity) {
			c.set_price_record(index, economy::median_price(state1, c));
		}
		auto index2 = economy::most_recent_price_record_index(state2);
		for(auto c : state2.world.in_commodity) {
			c.set_price_record(index2, economy::median_price(state2, c));
		}
	}
	compare_game_states(state1, state2);

	if(((ymd_date.month % 3) == 0) && (ymd_date.day == 1)) {
		auto index = economy::most_recent_gdp_record_index(state1);
		for(auto n : state1.world.in_nation) {
			n.set_gdp_record(index, economy::gdp(state1, n));
		}
		auto index2 = economy::most_recent_gdp_record_index(state2);
		for(auto n : state2.world.in_nation) {
			n.set_gdp_record(index2, economy::gdp(state2, n));
		}
	}

	state1.ui_date = state1.current_date;
	state2.ui_date = state2.current_date;

	state1.game_state_updated.store(true, std::memory_order::release);
	state2.game_state_updated.store(true, std::memory_order::release);
	compare_game_states(state1, state2);

	switch(state1.user_settings.autosaves) {
	case sys::autosave_frequency::none:
		break;
	case sys::autosave_frequency::daily:
		write_save_file(state1, sys::save_type::autosave);
		break;
	case sys::autosave_frequency::monthly:
		if(ymd_date.day == 1)
			write_save_file(state1, sys::save_type::autosave);
		break;
	case sys::autosave_frequency::yearly:
		if(ymd_date.month == 1 && ymd_date.day == 1)
			write_save_file(state1, sys::save_type::autosave);
		break;
	default:
		break;
	}
	switch(state2.user_settings.autosaves) {
	case sys::autosave_frequency::none:
		break;
	case sys::autosave_frequency::daily:
		write_save_file(state2, sys::save_type::autosave);
		break;
	case sys::autosave_frequency::monthly:
		if(ymd_date.day == 1)
			write_save_file(state2, sys::save_type::autosave);
		break;
	case sys::autosave_frequency::yearly:
		if(ymd_date.month == 1 && ymd_date.day == 1)
			write_save_file(state2, sys::save_type::autosave);
		break;
	default:
		break;
	}
}


TEST_CASE("sim_none", "[determinism]") {
	// Test that the game states are equal AFTER loading
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();
	game_state_2->game_seed = game_state_1->game_seed = 808080;
	compare_game_states(*game_state_1, *game_state_2);
}

TEST_CASE("sim_day", "[determinism]") {
	// Test that the game states are equal after loading and performing 1 tick
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();
	game_state_2->game_seed = game_state_1->game_seed = 808080;
	checked_single_tick(*game_state_1, *game_state_2);
}

TEST_CASE("sim_week", "[determinism]") {
	// Test that the game states are equal after loading and performing 7 tick
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();
	game_state_2->game_seed = game_state_1->game_seed = 808080;
	for(int i = 0; i < 7; i++) {
		checked_single_tick(*game_state_1, *game_state_2);
	}
}

TEST_CASE("sim_month", "[determinism]") {
	// Test that the game states are equal after loading and performing 31 tick
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();
	game_state_2->game_seed = game_state_1->game_seed = 808080;
	for(int i = 0; i < 31; i++) {
		checked_single_tick(*game_state_1, *game_state_2);
	}
}

TEST_CASE("sim_year", "[determinism]") {
	// Test that the game states are equal after loading and performing 1 tick
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();
	game_state_2->game_seed = game_state_1->game_seed = 808080;
	for(int i = 0; i <= 400; i++) {
		checked_single_tick(*game_state_1, *game_state_2);
	}
}

TEST_CASE("sim_game", "[determinism]") {
	// Test that the game states are equal after playing
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();

	game_state_2->game_seed = game_state_1->game_seed = 808080;

	compare_game_states(*game_state_1, *game_state_2);

	for(int i = 0; i <= 1000; i++) {
		game_state_1->single_game_tick();
		game_state_2->single_game_tick();
		compare_game_states(*game_state_1, *game_state_2);
	}
}
// this one uses the slower, but more accurate checked tick
TEST_CASE("sim_game_advanced", "[determinism]") {
	// Test that the game states are equal after playing
	static std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	static std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();

	game_state_2->game_seed = game_state_1->game_seed = 808080;
	game_state_1->network_mode = sys::network_mode_type::host;
	game_state_2->network_mode = sys::network_mode_type::client;

	compare_game_states(*game_state_1, *game_state_2);

	for(int i = 0; i <= 400; i++) {
		checked_single_tick_advanced(*game_state_1, *game_state_2);
		compare_game_states(*game_state_1, *game_state_2);
	}
}
TEST_CASE("fill_unsaved_values_determinism", "[determinism]") {
	// allocated statically untill crash fix is found
	static std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();

	game_state_1->game_seed = 808080;


	for(int i = 0; i <= 600; i++) {



		game_state_1->single_game_tick();

		auto before_key1 = game_state_1->get_save_checksum();

		game_state_1->fill_unsaved_data();


		// make sure that the fill_unsaved_data does not change saved data at all
		REQUIRE(before_key1.to_string() == game_state_1->get_save_checksum().to_string());

	}
}
