#pragma once


namespace economy {

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

}
