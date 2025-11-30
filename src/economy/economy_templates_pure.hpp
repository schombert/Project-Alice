#pragma once

#include "container_types.hpp"

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

	std::array<BOOL_VALUE, 2> applies_tariff;
	std::array<VALUE, 2> export_tariff;
	std::array<VALUE, 2> import_tariff;
	std::array<MARKET, 2> markets;

	VALUE distance;
	VALUE loss;
	VALUE base_distance_cost;
	VALUE workers_satisfaction;
	VALUE effect_of_scale;
	VALUE distance_cost_scaled;
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

	VALUE payment_per_unit;
	VALUE payment_received_per_unit;
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
	vectorized_budget_position<VALUE> life_needs{};
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

}
