#pragma once

#include "container_types.hpp"
#include "adaptive_ve.hpp"
#include "terms_pure.hpp"

namespace economy {

template<typename X, typename FROM, typename TO>
using convert_value_type = typename std::conditional_t<
	ve::is_vector_type_s<X>::value,
	typename ve::tagged_vector<TO>,
	TO
>;

template<typename TRADE_ROUTE>
struct tariff_data {
	using VALUE = typename std::conditional_t<ve::is_vector_type_s<TRADE_ROUTE>::value, ve::fp_vector, float>;
	using BOOL_VALUE = typename std::conditional_t<ve::is_vector_type_s<TRADE_ROUTE>::value, ve::mask_vector, bool>;
	using MARKET = convert_value_type<TRADE_ROUTE, dcon::trade_route_id, dcon::market_id>;

	BOOL_VALUE applies_export_tariff;
	BOOL_VALUE applies_import_tariff;
	VALUE export_tariff;
	VALUE import_tariff;

	VALUE distance;
	VALUE loss;
	VALUE transportation_cost;
};

template<typename TRADE_ROUTE>
struct trade_and_tariff {
	using VALUE = typename std::conditional_t<ve::is_vector_type_s<TRADE_ROUTE>::value, ve::fp_vector, float>;
	using BOOL_VALUE = typename std::conditional_t<ve::is_vector_type_s<TRADE_ROUTE>::value, ve::mask_vector, bool>;
	using MARKET = convert_value_type<TRADE_ROUTE, dcon::trade_route_id, dcon::market_id>;
	using NATION = convert_value_type<TRADE_ROUTE, dcon::trade_route_id, dcon::nation_id>;

	MARKET origin;
	MARKET target;

	NATION origin_nation;
	NATION target_nation;

	VALUE amount_origin;
	VALUE amount_target;

	VALUE tariff_origin;
	VALUE tariff_target;

	VALUE tariff_rate_origin;
	VALUE tariff_rate_target;

	VALUE price_origin;
	VALUE price_target;

	VALUE transport_cost;

	VALUE transportaion_loss;
	VALUE distance;

	VALUE origin_earn_per_unit;
	VALUE target_spend_per_unit;
	VALUE owner_earn_per_unit;
	VALUE owner_spend_per_unit;
};


namespace pops {
template<typename VALUE>
struct vectorized_budget_position {
	VALUE required{};
	VALUE satisfied_with_money_ratio{};
	VALUE satisfied_for_free_ratio{};
	VALUE spent{};
	VALUE demand_scale{};
};

template<typename VALUE>
struct vectorized_pops_budget {
	std::vector<vectorized_budget_position<VALUE>> per_consumption_category{ };
	vectorized_budget_position<VALUE> life_needs{};
	vectorized_budget_position<VALUE> housing{ };
	vectorized_budget_position<VALUE> everyday_needs{};
	vectorized_budget_position<VALUE> luxury_needs{};
	vectorized_budget_position<VALUE> investments{};
	vectorized_budget_position<VALUE> bank_savings{};
	vectorized_budget_position<VALUE> education{};

	VALUE can_use_free_services{ };
	VALUE remaining_savings{ };
	VALUE spent_total{ };
};
}

template<typename VALUE_IN, typename scalar>
auto need_weight_change(
	VALUE_IN cost_life,
	VALUE_IN price,
	VALUE_IN availability,
	VALUE_IN current_weight,
	VALUE_IN base_amount,
	VALUE_IN total_cost,
	VALUE_IN total_count
) {
	//return (adaptive_ve::max(adaptive_ve::constant_value<scalar>(0.f), availability * availability - 0.5f) - total_count * base_amount * current_weight * 10.f * price / total_cost) + (cost_life) / total_cost - price * base_amount / (cost_life * 1'000'000.f);
	return
		10.f * adaptive_ve::max(adaptive_ve::constant_value<scalar>(0.f), 2.f * availability * availability - 1.f)
		+ cost_life / total_cost
		- price * base_amount / (cost_life * 10'000.f)
		- total_count * base_amount * current_weight * price / total_cost;
}

}
