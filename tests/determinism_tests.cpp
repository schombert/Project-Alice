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

	auto tmp1 = std::unique_ptr<uint8_t[]>(new uint8_t[sizeof_save_section(ws1)]);
	write_save_section(tmp1.get(), ws1);
	auto tmp2 = std::unique_ptr<uint8_t[]>(new uint8_t[sizeof_save_section(ws2)]);
	write_save_section(tmp2.get(), ws1);
	REQUIRE(sizeof_save_section(ws1) == sizeof_save_section(ws2));
	REQUIRE(std::memcmp(tmp1.get(), tmp2.get(), sizeof_save_section(ws1)) == 0);
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
	demographics::regenerate_from_pop_data(ws1);
	demographics::regenerate_from_pop_data(ws2);
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
			economy::update_rgo_employment(ws1);
			economy::update_rgo_employment(ws2);
			break;
		case 9:
			economy::update_factory_employment(ws1);
			economy::update_factory_employment(ws2);
			break;
		case 10:
			nations::update_administrative_efficiency(ws1);
			rebel::daily_update_rebel_organization(ws1);

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

	economy::daily_update(ws1);
	economy::daily_update(ws2);
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
			ai::perform_battle_cycling(ws1);
			ai::perform_battle_cycling(ws2);
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
			nations::monthly_flashpoint_update(ws1);
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
			ai::perform_battle_cycling(ws1);
			ai::perform_battle_cycling(ws2);
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
	for(int i = 0; i <= 365; i++) {
		checked_single_tick(*game_state_1, *game_state_2);
	}
}
