#include "nations/diplomatic_crisis_dynamics.hpp"

#include "gamerule.hpp"
#include "system_state.hpp"

#include "catch2/catch.hpp"

#include <cmath>
#include <limits>
#include <memory>

namespace crisis_dynamics = nations::diplomatic_crisis_dynamics;

TEST_CASE("diplomatic crisis dynamics is an explicit opt-in no-op", "[diplomacy][crisis][transformation]") {
	crisis_dynamics::inputs inputs;
	inputs.active = true;
	inputs.temperature = 100.f;
	inputs.committed_attackers = 1;
	inputs.committed_defenders = 1;
	auto const result = crisis_dynamics::calculate(inputs);
	REQUIRE_FALSE(result.enabled);
	REQUIRE(result.stage == crisis_dynamics::escalation_stage::inactive);
	REQUIRE(result.escalation_pressure == 0.f);
	REQUIRE(result.settlement_pressure == 0.f);
	REQUIRE(result.war_risk == 0.f);
}

TEST_CASE("diplomatic crisis stages follow stable negotiation thresholds", "[diplomacy][crisis][transformation]") {
	using stage = crisis_dynamics::escalation_stage;
	REQUIRE(crisis_dynamics::stage_for(false, 100.f, false) == stage::inactive);
	REQUIRE(crisis_dynamics::stage_for(true, 99.f, true) == stage::diplomatic_opening);
	REQUIRE(crisis_dynamics::stage_for(true, 19.99f, false) == stage::diplomatic_opening);
	REQUIRE(crisis_dynamics::stage_for(true, 20.f, false) == stage::bargaining);
	REQUIRE(crisis_dynamics::stage_for(true, 50.f, false) == stage::confrontation);
	REQUIRE(crisis_dynamics::stage_for(true, 75.f, false) == stage::brinkmanship);
	REQUIRE(crisis_dynamics::stage_for(true, 90.f, false) == stage::war_imminent);
	REQUIRE(crisis_dynamics::stage_name(stage::brinkmanship) == "brinkmanship");
}

TEST_CASE("diplomatic crisis calculation is finite and bounded", "[diplomacy][crisis][transformation]") {
	crisis_dynamics::inputs inputs;
	inputs.enabled = true;
	inputs.active = true;
	inputs.temperature = std::numeric_limits<float>::infinity();
	inputs.total_participants = 1;
	inputs.committed_attackers = 2;
	inputs.committed_defenders = 3;
	inputs.attacker_power = std::numeric_limits<float>::quiet_NaN();
	inputs.defender_power = -100.f;
	inputs.attacker_war_exhaustion = std::numeric_limits<float>::infinity();
	inputs.defender_war_exhaustion = -1.f;
	inputs.attacker_wargoals = std::numeric_limits<uint64_t>::max();
	inputs.defender_wargoals = std::numeric_limits<uint64_t>::max();
	auto const result = crisis_dynamics::calculate(inputs);
	REQUIRE(result.enabled);
	REQUIRE(result.factors.temperature == 0.f);
	REQUIRE(result.factors.total_participants == 5);
	REQUIRE(result.factors.attacker_power == 0.f);
	REQUIRE(result.factors.defender_power == 0.f);
	for(auto const value : {
		result.normalized_temperature,
		result.commitment_ratio,
		result.bilateral_commitment,
		result.attacker_power_share,
		result.power_asymmetry,
		result.mean_war_exhaustion,
		result.claim_pressure,
		result.escalation_pressure,
		result.settlement_pressure,
		result.war_risk,
	}) {
		REQUIRE(std::isfinite(value));
		REQUIRE(value >= 0.f);
		REQUIRE(value <= 1.f);
	}
}

TEST_CASE("commitment claims and temperature produce explainable crisis pressure", "[diplomacy][crisis][transformation]") {
	crisis_dynamics::inputs low;
	low.enabled = true;
	low.active = true;
	low.temperature = 25.f;
	low.total_participants = 4;
	low.committed_attackers = 1;
	low.committed_defenders = 1;
	low.attacker_power = 100.f;
	low.defender_power = 100.f;
	auto high = low;
	high.temperature = 85.f;
	high.committed_attackers = 2;
	high.committed_defenders = 2;
	high.attacker_wargoals = 3;
	high.defender_wargoals = 2;
	auto const low_result = crisis_dynamics::calculate(low);
	auto const high_result = crisis_dynamics::calculate(high);
	REQUIRE(low_result.attacker_power_share == Approx(0.5f));
	REQUIRE(low_result.power_asymmetry == Approx(0.f));
	REQUIRE(high_result.commitment_ratio == Approx(1.f));
	REQUIRE(high_result.bilateral_commitment == Approx(1.f));
	REQUIRE(high_result.claim_pressure == Approx(5.f / 6.f));
	REQUIRE(high_result.escalation_pressure > low_result.escalation_pressure);
	REQUIRE(high_result.war_risk > low_result.war_risk);
	REQUIRE(high_result.stage == crisis_dynamics::escalation_stage::brinkmanship);

	auto asymmetric = high;
	asymmetric.attacker_power = 900.f;
	asymmetric.defender_power = 100.f;
	auto const asymmetric_result = crisis_dynamics::calculate(asymmetric);
	REQUIRE(asymmetric_result.attacker_power_share == Approx(0.9f));
	REQUIRE(asymmetric_result.power_asymmetry == Approx(0.8f));
	REQUIRE(asymmetric_result.settlement_pressure > high_result.settlement_pressure);

	auto missing_side = high;
	missing_side.committed_defenders = 0;
	auto const incomplete_result = crisis_dynamics::calculate(missing_side);
	REQUIRE(incomplete_result.missing_committed_side);
	REQUIRE(incomplete_result.war_risk == 0.f);
	REQUIRE(incomplete_result.stage == crisis_dynamics::escalation_stage::diplomatic_opening);

	auto war_threshold = high;
	war_threshold.temperature = 100.f;
	auto const threshold_result = crisis_dynamics::calculate(war_threshold);
	REQUIRE(threshold_result.war_risk == Approx(1.f));
	REQUIRE(threshold_result.stage == crisis_dynamics::escalation_stage::war_imminent);
}

TEST_CASE("state crisis adapter reads existing participants scores exhaustion and wargoals",
	"[diplomacy][crisis][transformation][integration]") {
	auto state = std::make_unique<sys::state>();
	state->crisis_participants.resize(4);
	state->crisis_attacker_wargoals.resize(2);
	state->crisis_defender_wargoals.resize(2);
	auto const rule = state->world.create_gamerule();
	state->hardcoded_gamerules.unused_gamerule = rule;
	state->world.gamerule_set_current_setting(
		rule, uint8_t(gamerule::age_of_transformation_settings::enabled));
	auto const attacker = state->world.create_nation();
	auto const defender = state->world.create_nation();
	auto const interested = state->world.create_nation();
	state->world.nation_set_military_score(attacker, 80);
	state->world.nation_set_industrial_score(attacker, 20);
	state->world.nation_set_military_score(defender, 25);
	state->world.nation_set_industrial_score(defender, 25);
	state->world.nation_set_war_exhaustion(attacker, 20.f);
	state->world.nation_set_war_exhaustion(defender, 60.f);
	state->primary_crisis_attacker = attacker;
	state->primary_crisis_defender = defender;
	state->current_crisis_state = sys::crisis_state::heating_up;
	state->crisis_temperature = 80.f;
	state->crisis_participants[0] = {attacker, true, false};
	state->crisis_participants[1] = {defender, false, false};
	state->crisis_participants[2] = {interested, false, true};
	auto const cb = state->world.create_cb_type();
	state->crisis_attacker_wargoals[0].cb = cb;
	state->crisis_defender_wargoals[0].cb = cb;

	auto const result = crisis_dynamics::evaluate_current_crisis(*state);
	REQUIRE(result.enabled);
	REQUIRE(result.factors.active);
	REQUIRE(result.factors.total_participants == 3);
	REQUIRE(result.factors.committed_attackers == 1);
	REQUIRE(result.factors.committed_defenders == 1);
	REQUIRE(result.factors.attacker_power == Approx(100.f));
	REQUIRE(result.factors.defender_power == Approx(50.f));
	REQUIRE(result.factors.attacker_war_exhaustion == Approx(0.2f));
	REQUIRE(result.factors.defender_war_exhaustion == Approx(0.6f));
	REQUIRE(result.factors.attacker_wargoals == 1);
	REQUIRE(result.factors.defender_wargoals == 1);
	REQUIRE(result.stage == crisis_dynamics::escalation_stage::brinkmanship);

	state->world.gamerule_set_current_setting(
		rule, uint8_t(gamerule::age_of_transformation_settings::disabled));
	auto const legacy = crisis_dynamics::evaluate_current_crisis(*state);
	REQUIRE_FALSE(legacy.enabled);
	REQUIRE(legacy.stage == crisis_dynamics::escalation_stage::inactive);
}
