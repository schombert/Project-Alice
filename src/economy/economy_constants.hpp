#pragma once

namespace economy {

// descides the divisor for the army demand from reinforcements. It is set to 28 to spread out the reinforcement demand over 28 days, as reinforce ticks only happen once a month
constexpr inline float unit_reinforcement_demand_divisor = 28.0f;

//local merchants take a cut from most local monetary operations
inline constexpr float local_market_cut_baseline = 0.01f;

inline constexpr float merchant_cut_foreign = 0.05f;
inline constexpr float merchant_cut_domestic = 0.001f;
inline constexpr float effect_of_transportation_scale = 0.0005f;
inline constexpr float trade_distance_covered_by_pair_of_workers_per_unit_of_good = 1.f;
inline constexpr float invalid_trade_route_distance = 999999.f;
inline constexpr float trade_loss_per_distance_unit = 0.0001f;
inline constexpr float trade_effect_of_scale_lower_bound = 0.1f;
inline constexpr float trade_base_multiplicative_decay = 0.0001f;
inline constexpr float trade_base_additive_decay = 0.075f;
inline constexpr float min_trade_expansion_multiplier = 0.01f;
inline constexpr float trade_demand_satisfaction_cutoff = 0.7f;

float constexpr inline base_qol = 0.5f;
constexpr inline uint32_t build_factory = issue_rule::pop_build_factory;
constexpr inline uint32_t expand_factory = issue_rule::pop_expand_factory;
constexpr inline uint32_t can_invest = expand_factory | build_factory;

// stockpile related things:
inline constexpr float stockpile_to_supply = 0.1f;
inline constexpr float stockpile_spoilage = 0.02f;
inline constexpr float stockpile_expected_spending_per_commodity = 1'000.f;
inline constexpr float market_savings_target = 1'000'000.f;
inline constexpr float trade_transaction_soft_limit = 1'000.f;

// base subsistence
inline constexpr float subsistence_factor = 5.0f;
inline constexpr float subsistence_score_life = 30.0f;
inline constexpr float subsistence_score_total = subsistence_score_life;

// move to defines later
inline constexpr float payouts_spending_multiplier = 10.f;

// for the sake of machine tools
inline constexpr float production_throughput_multiplier = 1.15f;

// greed drives incomes of corresponding pops up
// while making life worse on average
// profit cuts change distribution of incomes
inline constexpr float aristocrats_greed = 0.2f;
inline constexpr float artisans_greed = 0.3f;
inline constexpr float labor_greed_life = 0.05f;
inline constexpr float labor_greed_everyday = 0.f;
inline constexpr float capitalists_greed = 0.3f;
}
