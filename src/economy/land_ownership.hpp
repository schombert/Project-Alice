#pragma once

#include <algorithm>
#include <cmath>

namespace economy::land_ownership {

// Project Alice already persists the first two shares on every province. The
// remainder is land worked outside the capitalist/large-estate sector.
struct distribution {
	float landed_elites = 0.f;
	float capitalists = 0.f;
	float smallholders = 1.f;
};

inline constexpr float default_daily_adjustment = 1.f / 3650.f;

inline float finite_nonnegative(float value) {
	return std::isfinite(value) ? std::max(0.f, value) : 0.f;
}

inline distribution normalize(float landed_elites, float capitalists) {
	distribution result;
	result.landed_elites = finite_nonnegative(landed_elites);
	result.capitalists = finite_nonnegative(capitalists);
	auto const concentrated = result.landed_elites + result.capitalists;
	if(concentrated > 1.f) {
		result.landed_elites /= concentrated;
		result.capitalists /= concentrated;
	}
	result.smallholders = std::max(
		0.f, 1.f - result.landed_elites - result.capitalists);
	return result;
}

inline distribution target_from_claims(float smallholder_claim,
		float landed_claim, float capitalist_claim) {
	auto const smallholders = finite_nonnegative(smallholder_claim);
	auto const landed = finite_nonnegative(landed_claim);
	auto const capitalists = finite_nonnegative(capitalist_claim);
	auto const total = smallholders + landed + capitalists;
	if(total <= 0.f)
		return {};
	return normalize(landed / total, capitalists / total);
}

// Ownership is a stock, not a daily restatement of the local class structure.
// The first non-empty observation initializes old scenarios immediately; after
// that, acquisitions, class turnover and reforms can only move it gradually.
inline distribution advance(distribution current, distribution target,
		float daily_adjustment = default_daily_adjustment) {
	current = normalize(current.landed_elites, current.capitalists);
	target = normalize(target.landed_elites, target.capitalists);
	auto adjustment = std::isfinite(daily_adjustment)
		? std::clamp(daily_adjustment, 0.f, 1.f) : 0.f;
	auto const current_concentrated =
		current.landed_elites + current.capitalists;
	auto const target_concentrated =
		target.landed_elites + target.capitalists;
	if(current_concentrated <= 0.000001f && target_concentrated > 0.000001f)
		adjustment = 1.f;
	return normalize(
		current.landed_elites
			+ (target.landed_elites - current.landed_elites) * adjustment,
		current.capitalists
			+ (target.capitalists - current.capitalists) * adjustment);
}

} // namespace economy::land_ownership
