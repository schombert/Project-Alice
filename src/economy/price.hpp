#pragma once
#include "adaptive_ve.hpp"

namespace economy {
namespace price_properties {

namespace common {
template<typename VALUE, float min_price, float speed_multiplier, float additive_smoothing>
VALUE change(VALUE current_price, VALUE supply, VALUE demand) {
	// avoid singularity
	supply = supply + additive_smoothing * 3.f;
	demand = demand + additive_smoothing;

	auto probability_to_sell = adaptive_ve::min<VALUE>(demand / supply, 1.f); 
	auto probability_to_buy = adaptive_ve::min<VALUE>(supply / demand, 1.f);

	
	auto probability_to_keep_price_when_failed_to_buy = probability_to_buy;
	auto probability_to_keep_price_when_failed_to_sell = probability_to_sell;

	auto speed = speed_multiplier + (0.001f * (1.f - speed_multiplier) / (1.f + current_price));

	auto relative_price_change =
		(
			(1.f - probability_to_sell)
			* (1.f - speed)
			* (1.f - probability_to_keep_price_when_failed_to_sell)
			+ (probability_to_sell + (1.f - probability_to_sell) * probability_to_keep_price_when_failed_to_sell - 1.f)
			+
			(1.f - probability_to_buy)
			* (1.f + speed)
			* (1.f - probability_to_keep_price_when_failed_to_buy)
			+ (probability_to_buy + (1.f - probability_to_buy) * probability_to_keep_price_when_failed_to_buy - 1.f)
		);

	//auto relative_price_change_clamped = adaptive_ve::min<VALUE>(adaptive_ve::max<VALUE>(relative_price_change, -relative_speed_limit), relative_speed_limit);
	return relative_price_change * (current_price + min_price * 10.f);
}
}

//inline constexpr float relative_speed_limit = 0.05f;

// the only purpose of upper price bound is to prevent float overflow
// min price prevents singularity at zero

namespace commodity {
inline constexpr float min = 0.0001f;
// A price must be able to signal scarcity, but an unbounded nominal price is
// not a market-clearing mechanism.  Once it reaches a multiple of the base
// value, budget-constrained consumers can no longer provide useful feedback
// and construction can be locked out forever.
inline constexpr float maximum_base_price_multiplier = 100.f;
inline constexpr float epsilon = min * 0.1f;
inline constexpr float speed_multiplier = 0.01f;
inline constexpr float additive_smoothing = 0.0075f;
inline float maximum(float base_price) {
	return adaptive_ve::max(min, base_price * maximum_base_price_multiplier);
}
template<typename VALUE>
VALUE change(VALUE current_price, VALUE supply, VALUE demand) {
	return common::change<VALUE, min, speed_multiplier, additive_smoothing>(current_price, supply, demand);
}
}
namespace labor {
inline constexpr float min = 0.0000001f;
inline constexpr float max = 1'000'000'000'000.f;
inline constexpr float epsilon = min * 0.1f;
inline constexpr float speed_multiplier = 0.0015f;
inline constexpr float additive_smoothing = 1.f;
template<typename VALUE>
VALUE change(VALUE current_price, VALUE supply, VALUE demand) {
	return common::change<VALUE, min, speed_multiplier, additive_smoothing>(current_price, supply, demand);
}
}
namespace service {
inline constexpr float min = 0.0000001f;
inline constexpr float max = 1'000'000'000'000.f;
inline constexpr float epsilon = min * 0.1f;
inline constexpr float speed_multiplier = 0.0015f;
inline constexpr float additive_smoothing = 1.f;
template<typename VALUE>
VALUE change(VALUE current_price, VALUE supply, VALUE demand) {
	return common::change<VALUE, min, speed_multiplier, additive_smoothing>(current_price, supply, demand);
}
}


}
}
