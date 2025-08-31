#pragma once
#include "adaptive_ve.hpp"

namespace economy {
namespace price_properties {
inline constexpr float speed_multiplier = 0.001f;
inline constexpr float additive_smoothing = 0.001f;
inline constexpr float relative_speed_limit = 0.05f;

// the only purpose of upper price bound is to prevent float overflow
// min price prevents singularity at zero

namespace commodity {
inline constexpr float min = 0.0001f;
inline constexpr float max = 1'000'000'000'000.f;
}
namespace labor {
inline constexpr float min = 0.00001f;
inline constexpr float max = 1'000'000'000'000.f;
}
namespace service {
inline constexpr float min = 0.0000001f;
inline constexpr float max = 1'000'000'000'000.f;
}

template<typename VALUE>
VALUE change(VALUE current_price, VALUE supply, VALUE demand) {
	// avoid singularity
	supply = supply + additive_smoothing * 3.f;
	demand = demand + additive_smoothing;
	auto oversupply_factor = adaptive_ve::max<VALUE>(supply / demand - 1.f, 0.f);
	auto overdemand_factor = adaptive_ve::max<VALUE>(demand / supply - 1.f, 0.f);
	auto speed_modifer = (overdemand_factor - oversupply_factor);
	auto price_speed = adaptive_ve::min<VALUE>(adaptive_ve::max<VALUE>(speed_multiplier * speed_modifer, -relative_speed_limit), relative_speed_limit);
	return price_speed * current_price;
}
}
}

