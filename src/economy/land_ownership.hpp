#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "dcon_generated_ids.hpp"

namespace sys {
struct state;
}

namespace economy::land_ownership {

// Project Alice already persists the first two shares on every province. The
// remainder is land worked outside the capitalist/large-estate sector.
struct distribution {
	float landed_elites = 0.f;
	float capitalists = 0.f;
	float smallholders = 1.f;
	float state = 0.f;
	float foreign = 0.f;

	constexpr distribution() = default;
	constexpr distribution(float landed, float capitalist)
		: landed_elites(landed), capitalists(capitalist),
			smallholders(1.f - landed - capitalist) {
	}
	constexpr distribution(float landed, float capitalist,
			float smallholder, float state_owned, float foreign_owned)
		: landed_elites(landed), capitalists(capitalist),
			smallholders(smallholder), state(state_owned),
			foreign(foreign_owned) {
	}
};

inline constexpr float default_daily_adjustment = 1.f / 3650.f;

enum class owner_group : uint8_t {
	smallholders = 0,
	landed_elites = 1,
	capitalists = 2,
	state = 3,
	foreign = 4,
	count = 5,
};

constexpr inline std::size_t owner_group_count =
	std::size_t(owner_group::count);

struct market_config {
	bool enabled = false;
	// A monthly market may turn over at most this fraction of the province.
	float maximum_monthly_turnover = 0.005f;
	// Buyers may spend only cash above this many months of essential needs.
	float reserve_months = 6.f;
	// Voluntary sellers list only a small part of their holdings each month.
	float voluntary_ask_rate = 0.001f;
	// Tenant protection reduces hardship-driven listings.
	float tenant_protection = 0.f;
	float annual_land_tax_rate = 0.f;
	// No private estate may grow above this fraction when the cap is active.
	float large_estate_limit = 1.f;
	// Policy flows are expressed as a fraction of all provincial land per month.
	float agrarian_reform_rate = 0.f;
	float nationalization_rate = 0.f;
	float privatization_rate = 0.f;
	bool foreign_investment_allowed = false;
};

struct group_finance {
	float liquid_savings = 0.f;
	float monthly_essential_needs = 0.f;
	// Bounded 0..1 signal combining unemployment, unmet needs and debt stress.
	float hardship = 0.f;
};

enum class land_use_status : uint8_t {
	smallholder,
	tenant,
	landless_laborer,
};

struct land_use_distribution {
	float smallholders = 0.f;
	float tenants = 0.f;
	float landless_laborers = 0.f;
};

struct market_result {
	bool enabled = false;
	distribution before{};
	distribution target{};
	distribution after{};
	std::array<float, owner_group_count> cash_delta{};
	std::array<float, owner_group_count> bids{};
	std::array<float, owner_group_count> asks{};
	float land_value = 0.f;
	float turnover = 0.f;
	float distress_asks = 0.f;
	float land_tax = 0.f;
};

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

distribution normalize(distribution value);

// A 270-day default gives a nine-month net-rent signal. This pure helper is
// suitable for a persisted accumulator when the save schema receives migration.
float update_smoothed_rent(float previous_daily_rent, float current_daily_rent,
	float window_days = 270.f);

land_use_distribution classify_land_use(float rural_population,
	float privately_worked_land, float tenant_protection);

market_config configuration_for(sys::state const& state,
	dcon::province_id province);

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

market_result clear_market(distribution current,
	std::array<group_finance, owner_group_count> finances,
	float land_value, market_config const& config);

// Compatibility overload for callers that only have liquid savings.
market_result clear_market(distribution current,
	std::array<float, owner_group_count> savings,
	float land_value, market_config const& config);

// Monthly Project Alice adapter. It transfers POP savings between buyers and
// sellers and changes the already-serialized provincial ownership shares.
void update_markets(sys::state& state);

} // namespace economy::land_ownership
