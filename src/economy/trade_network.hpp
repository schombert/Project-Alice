#pragma once

namespace trade_network {

inline float naval_base_level_to_market_attractiveness = 0.25f;

void generate_initial_trade_routes(sys::state& state);
void generate_sea_trade_routes(sys::state& state);
void recalculate_markets_distance(sys::state& state);

enum class candidate_failure_reason {
	none, low_score, low_approximate_score, already_exists, same_state, not_coastal
};

struct trade_candidate_information {
	float score = 0.f;
	float big_score = 0.f;
	float approx_score = 0.f;
	float approx_big_score = 0.f;
	candidate_failure_reason reason = candidate_failure_reason::none;
	bool potentially_valid = false;
};

struct sea_trade_candidate_data {
	float naval_base_level;
	dcon::market_id market;
	dcon::nation_id owner;
	dcon::province_id coast;
	dcon::state_instance_id state_instance;
	uint16_t connected_coastal_region;
	float population;
};

trade_candidate_information process_trade_partner_candidate(
	sys::state const& state, sea_trade_candidate_data origin, sea_trade_candidate_data target,
	float local_base_distance, float local_base_population, float global_mult
);

float local_base_trade_distance(sys::state const& state, dcon::state_instance_id origin, dcon::province_id sampled_province, float baseline_population);
float get_local_average_population(sys::state const& state, dcon::state_instance_id origin, dcon::province_id sampled_province, float baseline_population, float base_distance);
float sea_trade_route_distance(sys::state const& state, dcon::province_id a, dcon::province_id b);

}
