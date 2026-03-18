#include "system_state.hpp"
#include "demographics.hpp"
#include "economy_stats.hpp"
#include "economy_trade_routes.hpp"
#include "economy_constants.hpp"
#include "money.hpp"
#include "economy_templates_pure.hpp"
#include "economy_pops_constants.hpp"
#include "economy_pops.hpp"
#include "province_templates.hpp"
#include "economy_production.hpp"

namespace economy {

ve::fp_vector ve_price(
	sys::state const& state,
	ve::contiguous_tags<dcon::market_id> s,
	dcon::commodity_id c
) {
	return state.world.market_get_price(s, c);
}
ve::fp_vector ve_price(
	sys::state const& state,
	ve::partial_contiguous_tags<dcon::market_id> s,
	dcon::commodity_id c
) {
	return state.world.market_get_price(s, c);
}
ve::fp_vector ve_price(
	sys::state const& state,
	ve::tagged_vector<dcon::market_id> s,
	dcon::commodity_id c
) {
	return state.world.market_get_price(s, c);
}

void register_demand(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id commodity_type,
	float amount
	//economy_reason reason
) {
	assert(amount >= 0.f);
	auto current = state.world.market_get_demand(s, commodity_type);
	state.world.market_set_demand(s, commodity_type, current + amount);
	assert(std::isfinite(state.world.market_get_demand(s, commodity_type)));
}

template<typename MARKETS>
void t_register_demand(
	sys::state& state,
	MARKETS s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount
	//economy_reason reason
) {
	ve::apply(
		[](float amount) {
			assert(std::isfinite(amount) && amount >= 0.f);
		}, amount
	);
	state.world.market_set_demand(
		s,
		commodity_type,
		state.world.market_get_demand(s, commodity_type) + amount
	);
	ve::apply(
		[](float demand) {
			assert(std::isfinite(demand) && demand >= 0.f);
		}, state.world.market_get_demand(s, commodity_type)
	);
}

void register_demand(
	sys::state& state,
	ve::contiguous_tags<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount
	//economy_reason reason
) {
	t_register_demand(state, s, commodity_type, amount);
}
void register_demand(
	sys::state& state,
	ve::partial_contiguous_tags<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount
	//economy_reason reason
) {
	t_register_demand(state, s, commodity_type, amount);
}
void register_demand(
	sys::state& state,
	ve::tagged_vector<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount
	//economy_reason reason
) {
	t_register_demand(state, s, commodity_type, amount);
}

void register_intermediate_demand(
	sys::state& state,
	ve::contiguous_tags<dcon::market_id> s,
	dcon::commodity_id c,
	ve::fp_vector amount
	//economy_reason reason
) {
	register_demand(state, s, c, amount);
	state.world.market_set_intermediate_demand(
		s,
		c,
		state.world.market_get_intermediate_demand(s, c) + amount
	);
}
void register_intermediate_demand(
	sys::state& state,
	ve::partial_contiguous_tags<dcon::market_id> s,
	dcon::commodity_id c,
	ve::fp_vector amount
	//economy_reason reason
) {
	register_demand(state, s, c, amount);
	state.world.market_set_intermediate_demand(
		s,
		c,
		state.world.market_get_intermediate_demand(s, c) + amount
	);
}
void register_intermediate_demand(
	sys::state& state,
	ve::tagged_vector<dcon::market_id> s,
	dcon::commodity_id c,
	ve::fp_vector amount
	//economy_reason reason
) {
	register_demand(state, s, c, amount);
	state.world.market_set_intermediate_demand(
		s,
		c,
		state.world.market_get_intermediate_demand(s, c) + amount
	);
}

void register_intermediate_demand(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c,
	float amount
	//economy_reason reason
) {
	// check for market validity before writing data to it
	if(s) {
			register_demand(state, s, c, amount);
		state.world.market_set_intermediate_demand(
			s,
			c,
			state.world.market_get_intermediate_demand(s, c) + amount
		);
	}

}

void register_domestic_supply(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id commodity_type,
	float amount,
	economy_reason reason
) {
	state.world.market_set_supply(s, commodity_type, state.world.market_get_supply(s, commodity_type) + amount);
	auto median_price = state.world.commodity_get_median_price(commodity_type);
	state.world.market_set_gdp(s, state.world.market_get_gdp(s) + amount * median_price);
}

void register_foreign_supply(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id commodity_type,
	float amount,
	economy_reason reason
) {
	state.world.market_set_supply(s, commodity_type, state.world.market_get_supply(s, commodity_type) + amount);
}

template<typename T>
void ve_register_domestic_supply(
	sys::state& state,
	T s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
) {
	state.world.market_set_supply(
		s,
		commodity_type,
		state.world.market_get_supply(s, commodity_type) + amount
	);
	auto median_price = state.world.commodity_get_median_price(commodity_type);
	state.world.market_set_gdp(
		s,
		state.world.market_get_gdp(s)
		+ amount * median_price
	);
}

void register_domestic_supply(
	sys::state& state,
	ve::contiguous_tags<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
) {
	ve_register_domestic_supply(state, s, commodity_type, amount, reason);
}
void register_domestic_supply(
	sys::state& state,
	ve::partial_contiguous_tags<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
) {
	ve_register_domestic_supply(state, s, commodity_type, amount, reason);
}
void register_domestic_supply(
	sys::state& state,
	ve::tagged_vector<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
) {
	ve_register_domestic_supply(state, s, commodity_type, amount, reason);
}

float price(sys::state const& state, dcon::state_instance_id s, dcon::commodity_id c) {
	auto market = state.world.state_instance_get_market_from_local_market(s);
	return state.world.market_get_price(market, c);
}

float price(sys::state const& state, dcon::nation_id s, dcon::commodity_id c) {
	auto total_cost = 0.f;
	auto total_supply = 0.f;
	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		dcon::market_id market = state.world.state_instance_get_market_from_local_market(sid);

		auto local_price = price(state, market, c);
		auto local_supply = state.world.market_get_supply(market, c);

		total_cost += local_price * local_supply;
		total_supply += local_supply;
	});

	if(total_supply == 0.f) {
		auto capital = state.world.nation_get_capital(s);
		auto sid = state.world.province_get_state_membership(capital);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		return state.world.market_get_price(market, c);
	}

	return total_cost / total_supply;
}

float price(sys::state& state, dcon::commodity_id c) {
	auto total_cost = 0.f;
	auto total_supply = 0.f;
	state.world.for_each_market([&](auto m) {
		auto local_price = price(state, m, c);
		auto local_supply = state.world.market_get_supply(m, c) + 0.0001f;

		total_cost += local_price * local_supply;
		total_supply += local_supply;
	});

	if(total_supply == 0.f) {
		return 0.f;
	}

	return total_cost / total_supply;
}

float median_price(sys::state& state, dcon::commodity_id c) {
	std::vector<float> prices{};
	state.world.for_each_market([&](auto m) {
		auto local_price = price(state, m, c);
		prices.push_back(local_price);
	});
	std::sort(prices.begin(), prices.end());
	if(prices.size() == 0) {
		return 0;
	}
	if(prices.size() % 2 == 0) {
		return (prices[prices.size() / 2] + prices[prices.size() / 2 - 1]) / 2.f;
	}
	return (prices[prices.size() / 2]);
}
float median_price(sys::state& state, dcon::nation_id s, dcon::commodity_id c) {
	std::vector<float> prices{};
	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		dcon::market_id m = state.world.state_instance_get_market_from_local_market(sid);
		auto local_price = price(state, m, c);
		prices.push_back(local_price);
	});
	if(prices.size() == 0) {
		return 0;
	}
	std::sort(prices.begin(), prices.end());
	if(prices.size() % 2 == 0) {
		return (prices[prices.size() / 2] + prices[prices.size() / 2 - 1]) / 2.f;
	}
	return (prices[prices.size() / 2]);
}

float stockpile(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	float total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		dcon::market_id m = state.world.state_instance_get_market_from_local_market(sid);
		total += state.world.market_get_stockpile(m, c);
	});
	return total;
}


float price(sys::state const& state, dcon::market_id s, dcon::commodity_id c) {
	return state.world.market_get_price(s, c);
}

float supply(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
) {
	return state.world.market_get_supply(s, c);
}
float supply(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
) {
	auto total_supply = 0.f;
	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);

		auto local_supply = state.world.market_get_supply(market, c);
		total_supply += local_supply;
	});

	return total_supply; //- domestic_trade_volume(state, s, c);
}

float domestic_trade_volume(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
) {
	auto total_volume = 0.f;

	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);

		state.world.market_for_each_trade_route(market, [&](auto trade_route) {
			trade_and_tariff<dcon::trade_route_id> explanation = explain_trade_route_commodity(state, trade_route, c);
			if(explanation.origin_nation == s && explanation.target_nation == s)
				total_volume += explanation.amount_origin;
		});
	});

	return total_volume;
}

float supply(
	sys::state& state,
	dcon::commodity_id c
) {
	auto total_supply = 0.f;
	state.world.for_each_market([&](auto m) {
		auto local_supply = state.world.market_get_supply(m, c);
		total_supply += local_supply;
	});
	return total_supply;
}

float demand(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
) {
	return state.world.market_get_demand(s, c);
}
float demand(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
) {
	auto total_demand = 0.f;
	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		auto local_demand = state.world.market_get_demand(market, c);
		total_demand += local_demand;
	});

	return total_demand;
}
float demand(
	sys::state& state,
	dcon::commodity_id c
) {
	auto total_demand = 0.f;
	state.world.for_each_market([&](auto m) {
		auto local_demand = state.world.market_get_demand(m, c);
		total_demand += local_demand;
	});

	/*
	state.world.for_each_trade_route([&](dcon::trade_route_id route) {
		auto volume = state.world.trade_route_get_volume(route, c);
		total_demand -= volume;
	});
	*/

	return total_demand;
}

float consumption(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
) {
	return std::max(0.f, (state.world.market_get_demand(s, c) - trade_demand(state, s, c))) * state.world.market_get_actual_probability_to_buy(s, c);
}
float consumption(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
) {
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);

		auto local = state.world.market_get_demand(market, c);
		total += local * state.world.market_get_actual_probability_to_buy(market, c);
	});

	return total;
}
float consumption(
	sys::state& state,
	dcon::commodity_id c
) {
	auto total = 0.f;
	state.world.for_each_market([&](auto m) {
		auto local = state.world.market_get_demand(m, c);
		total += local * state.world.market_get_actual_probability_to_buy(m, c);
	});
	return total;
}

float market_pool(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
) {
	return state.world.market_get_stockpile(s, c);
}
float market_pool(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
) {
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);

		auto local = state.world.market_get_stockpile(market, c);
		total += local;
	});

	return total;
}
float market_pool(
	sys::state& state,
	dcon::commodity_id c
) {
	auto total = 0.f;
	state.world.for_each_market([&](auto m) {
		auto local = state.world.market_get_stockpile(m, c);
		total += local;
	});
	return total;
}

float demand_satisfaction(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
) {
	auto d = demand(state, s, c);
	auto con = consumption(state, s, c);
	if(d == 0.f) {
		return 0.f;
	}
	return con / d;
}
float demand_satisfaction(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
) {
	auto d = demand(state, s, c);
	auto con = consumption(state, s, c);
	if(d == 0.f) {
		return 0.f;
	}
	return con / d;
}
float demand_satisfaction(
	sys::state& state,
	dcon::commodity_id c
) {
	auto d = demand(state, c);
	auto con = consumption(state, c);
	if(d == 0.f) {
		return 0.f;
	}
	return con / d;
}


float average_capitalists_luxury_cost(
	sys::state& state,
	dcon::nation_id s
) {
	auto total = 0.f;
	auto count = 0.f;
	auto def = state.culture_definitions.capitalists;
	auto key = demographics::to_key(state, def);

	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);

		auto local_count = state.world.state_instance_get_demographics(
			sid, key
		);

		auto luxury = state.world.market_get_luxury_needs_costs(
			market, def
		);
		auto everyday = state.world.market_get_everyday_needs_costs(
			market, def
		);
		auto life = state.world.market_get_life_needs_costs(
			market, def
		);

		total += (luxury + everyday + life) * local_count / state.defines.alice_needs_scaling_factor;
		count += local_count;
	});

	if(count == 0.f) {
		return 0.f;
	}

	return total / count;
}

float trade_supply(sys::state const& state,
	dcon::market_id m,
	dcon::commodity_id c
) {
	auto stockpiles = state.world.market_get_stockpile(m, c);
	auto sid = state.world.market_get_zone_from_local_market(m);
	auto capital = state.world.state_instance_get_capital(sid);
	auto result = std::max(0.f, stockpiles) * stockpile_to_supply;
	return result;
}

float trade_supply(sys::state& state,
	dcon::nation_id n,
	dcon::commodity_id c
) {
	float total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		total += trade_supply(state, market, c);
	});
	return total;
}

float trade_demand(sys::state& state,
	dcon::market_id m,
	dcon::commodity_id c
) {
	auto stockpiles = state.world.market_get_stockpile(m, c);
	auto sid = state.world.market_get_zone_from_local_market(m);
	auto capital = state.world.state_instance_get_capital(sid);
	auto wage = state.world.province_get_labor_price(capital, labor::no_education);
	auto result = 0.f;

	state.world.market_for_each_trade_route(m, [&](auto trade_route) {
		auto current_volume = state.world.trade_route_get_volume(trade_route, c);
		auto origin =
			current_volume > 0.f
			? state.world.trade_route_get_connected_markets(trade_route, 0)
			: state.world.trade_route_get_connected_markets(trade_route, 1);
		auto target =
			current_volume <= 0.f
			? state.world.trade_route_get_connected_markets(trade_route, 0)
			: state.world.trade_route_get_connected_markets(trade_route, 1);
		if(origin != m) return;
		auto absolute_volume = std::abs(current_volume);
		result += absolute_volume;
	});
	return result;
}

float trade_demand(sys::state& state,
	dcon::nation_id n,
	dcon::commodity_id c
) {
	float total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		total += trade_demand(state, market, c);
	});
	return total;
}

float trade_influx(sys::state& state,
	dcon::market_id m,
	dcon::commodity_id c
) {
	float result = 0.f;
	state.world.market_for_each_trade_route(m, [&](auto trade_route) {
		auto current_volume = state.world.trade_route_get_volume(trade_route, c);
		auto origin =
			current_volume > 0.f
			? state.world.trade_route_get_connected_markets(trade_route, 0)
			: state.world.trade_route_get_connected_markets(trade_route, 1);
		auto target =
			current_volume <= 0.f
			? state.world.trade_route_get_connected_markets(trade_route, 0)
			: state.world.trade_route_get_connected_markets(trade_route, 1);
		if(target != m) return;
		auto sat = state.world.market_get_actual_probability_to_buy(origin, c);
		auto absolute_volume = std::abs(current_volume);
		auto distance = state.world.trade_route_get_distance(trade_route);
		auto trade_good_loss_mult = std::max(0.f, 1.f - 0.0001f * distance);
		result += sat * absolute_volume * trade_good_loss_mult;
	});
	return result;
}

float trade_outflux(sys::state& state,
	dcon::market_id m,
	dcon::commodity_id c
) {
	float result = 0.f;
	state.world.market_for_each_trade_route(m, [&](auto trade_route) {
		auto current_volume = state.world.trade_route_get_volume(trade_route, c);
		auto origin =
			current_volume > 0.f
			? state.world.trade_route_get_connected_markets(trade_route, 0)
			: state.world.trade_route_get_connected_markets(trade_route, 1);
		auto target =
			current_volume <= 0.f
			? state.world.trade_route_get_connected_markets(trade_route, 0)
			: state.world.trade_route_get_connected_markets(trade_route, 1);
		if(origin != m) return;
		auto sat = state.world.market_get_actual_probability_to_buy(origin, c);
		auto absolute_volume = std::abs(current_volume);
		result += sat * absolute_volume;
	});
	return result;
}

float trade_value_flow(
	sys::state& state,
	dcon::market_id origin,
	dcon::market_id target
) {
	if(origin == target) {
		return 0.f;
	}
	float result = 0.f;
	auto trade_route = state.world.get_trade_route_by_province_pair(origin, target);
	state.world.for_each_commodity([&](auto c) {
		auto median_price = state.world.commodity_get_median_price(c);
		auto current_volume = state.world.trade_route_get_volume(trade_route, c);
		auto m_origin =
			current_volume > 0.f
			? state.world.trade_route_get_connected_markets(trade_route, 0)
			: state.world.trade_route_get_connected_markets(trade_route, 1);
		auto m_target =
			current_volume <= 0.f
			? state.world.trade_route_get_connected_markets(trade_route, 0)
			: state.world.trade_route_get_connected_markets(trade_route, 1);
		auto sat = state.world.market_get_actual_probability_to_buy(m_origin, c);
		auto absolute_volume = std::abs(current_volume);
		auto s_origin = state.world.market_get_zone_from_local_market(m_origin);
		auto s_target = state.world.market_get_zone_from_local_market(m_target);
		auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
		auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

		if(m_origin == origin && m_target == target) {
			result += sat * absolute_volume * median_price;
		}
	});
	return result;
}

std::vector<float> trade_value_flow_nation_to_all(
	sys::state& state,
	dcon::nation_id origin
) {
	std::vector<float> result;
	result.resize(state.world.nation_size());

	state.world.nation_for_each_state_ownership(origin, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		state.world.market_for_each_trade_route(market, [&](auto trade_route) {
			state.world.for_each_commodity([&](auto c) {
				auto median_price = state.world.commodity_get_median_price(c);
				auto current_volume = state.world.trade_route_get_volume(trade_route, c);
				auto m_origin =
					current_volume > 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);
				auto m_target =
					current_volume <= 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);
				auto sat = state.world.market_get_actual_probability_to_buy(m_origin, c);
				auto absolute_volume = std::abs(current_volume);
				auto s_origin = state.world.market_get_zone_from_local_market(m_origin);
				auto s_target = state.world.market_get_zone_from_local_market(m_target);
				auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
				auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

				if(n_origin == origin) {
					result[n_target.index()] += sat * absolute_volume * median_price;
				}
			});
		});
	});

	return result;
}

std::vector<float> trade_value_flow_all_to_nation(
	sys::state& state,
	dcon::nation_id target
) {
	std::vector<float> result;
	result.resize(state.world.nation_size());

	state.world.nation_for_each_state_ownership(target, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		state.world.market_for_each_trade_route(market, [&](auto trade_route) {
			state.world.for_each_commodity([&](auto c) {
				auto median_price = state.world.commodity_get_median_price(c);
				auto current_volume = state.world.trade_route_get_volume(trade_route, c);
				auto m_origin =
					current_volume > 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);
				auto m_target =
					current_volume <= 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);
				auto sat = state.world.market_get_actual_probability_to_buy(m_origin, c);
				auto absolute_volume = std::abs(current_volume);
				auto s_origin = state.world.market_get_zone_from_local_market(m_origin);
				auto s_target = state.world.market_get_zone_from_local_market(m_target);
				auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
				auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

				if(n_target == target) {
					result[n_origin.index()] += sat * absolute_volume * median_price;
				}
			});
		});
	});

	return result;
}



float trade_value_flow(
	sys::state& state,
	dcon::nation_id origin,
	dcon::nation_id target
) {
	if(origin == target) {
		return 0.f;
	}
	float result = 0.f;
	state.world.for_each_commodity([&](auto c) {
		auto median_price = state.world.commodity_get_median_price(c);
		auto total_volume = 0.f;
		state.world.nation_for_each_state_ownership(origin, [&](auto soid) {
			auto sid = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(sid);
			state.world.market_for_each_trade_route(market, [&](auto trade_route) {
				auto current_volume = state.world.trade_route_get_volume(trade_route, c);
				auto m_origin =
					current_volume > 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);
				auto m_target =
					current_volume <= 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);
				auto sat = state.world.market_get_actual_probability_to_buy(m_origin, c);
				auto absolute_volume = std::abs(current_volume);
				auto s_origin = state.world.market_get_zone_from_local_market(m_origin);
				auto s_target = state.world.market_get_zone_from_local_market(m_target);
				auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
				auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

				if(n_origin == origin && n_target == target) {
					total_volume += sat * absolute_volume;
				}
			});
		});

		result += total_volume * median_price;
	});
	return result;
}


float export_value(
	sys::state& state,
	dcon::market_id s
) {
	float result = 0.f;
	state.world.for_each_commodity([&](auto c) {
		auto median_price = state.world.commodity_get_median_price(c);
		auto total_volume = 0.f;

		state.world.market_for_each_trade_route(s, [&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, c);
			auto origin =
				current_volume > 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);
			auto target =
				current_volume <= 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);

			if(origin != s) return;
			auto sat = state.world.market_get_actual_probability_to_buy(origin, c);
			auto absolute_volume = std::abs(current_volume);
			auto s_origin = state.world.market_get_zone_from_local_market(origin);
			auto s_target = state.world.market_get_zone_from_local_market(target);
			auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
			auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);
			total_volume += sat * absolute_volume;
		});

		result += total_volume * median_price;
	});
	return result;
}
float export_value(
	sys::state& state,
	dcon::nation_id s
) {
	float result = 0.f;
	state.world.for_each_commodity([&](auto c) {
		auto median_price = state.world.commodity_get_median_price(c);
		result += export_volume(state, s, c) * median_price;
	});
	return result;
}
float import_value(
	sys::state& state,
	dcon::market_id s
) {
	float result = 0.f;
	state.world.for_each_commodity([&](auto c) {
		auto median_price = state.world.commodity_get_median_price(c);
		auto total_volume = 0.f;

		state.world.market_for_each_trade_route(s, [&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, c);
			auto origin =
				current_volume > 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);
			auto target =
				current_volume <= 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);

			if(target != s) return;

			auto sat = state.world.market_get_actual_probability_to_buy(origin, c);
			auto absolute_volume = std::abs(current_volume);
			auto s_origin = state.world.market_get_zone_from_local_market(origin);
			auto s_target = state.world.market_get_zone_from_local_market(target);
			auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
			auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);
			total_volume += sat * absolute_volume;
		});

		result += total_volume * median_price;
	});
	return result;
}
float import_value(
	sys::state& state,
	dcon::nation_id s
) {
	float result = 0.f;
	state.world.for_each_commodity([&](auto c) {
		auto median_price = state.world.commodity_get_median_price(c);
		result += import_volume(state, s, c) * median_price;
	});
	return result;
}

float export_volume(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
) {
	return state.world.market_get_export(s, c);
}
float export_volume(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
) {
	// todo: specialise
	return export_volume_detailed(state, s, c).volume;
}

trade_volume_data_detailed export_volume_detailed(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
) {
	static ve::vectorizable_buffer<float, dcon::nation_id> per_nation_data(uint32_t(1));
	static uint32_t old_count = 1;

	auto new_count = state.world.nation_size();
	if(new_count > old_count) {
		per_nation_data = state.world.nation_make_vectorizable_float_buffer();
		old_count = new_count;
	}

	state.world.execute_serial_over_nation([&](auto nids) {
		per_nation_data.set(nids, 0.f);
	});

	float total = 0.f;

	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		state.world.market_for_each_trade_route(market, [&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, c);
			auto origin =
				current_volume > 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);
			auto target =
				current_volume <= 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);

			if(origin != market) return;

			auto sat = state.world.market_get_actual_probability_to_buy(origin, c);
			auto absolute_volume = std::abs(current_volume);
			auto s_origin = state.world.market_get_zone_from_local_market(origin);
			auto s_target = state.world.market_get_zone_from_local_market(target);
			auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
			auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

			auto distance = state.world.trade_route_get_distance(trade_route);

			auto trade_good_loss_mult = std::max(0.f, 1.f - 0.0001f * distance);

			if(n_origin != n_target) {
				per_nation_data.get(n_target) += sat * absolute_volume;
				total += sat * absolute_volume;
			}
		});
	});

	nation_enriched_float top[5] = { };

	state.world.for_each_nation([&](dcon::nation_id nid) {
		auto value = per_nation_data.get(nid);
		for(size_t i = 0; i < 5; i++) {
			if(top[i].nation) {
				if(value > top[i].value) {
					for(size_t j = 4; j > i; j--) {
						top[j].value = top[j - 1].value;
						top[j].nation = top[j - 1].nation;
					}
					top[i].nation = nid;
					top[i].value = value;
					return;
				}
			} else {
				top[i].nation = nid;
				top[i].value = value;

				return;
			}
		}
	});

	trade_volume_data_detailed result = {
		total, c, { top[0], top[1], top[2], top[3], top[4] }
	};

	return result;
}

float import_volume(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
) {
	return state.world.market_get_import(s, c);
}
float import_volume(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
) {
	// todo: make it specialised
	return import_volume_detailed(state, s, c).volume;
}

trade_volume_data_detailed import_volume_detailed(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
) {
	static ve::vectorizable_buffer<float, dcon::nation_id> per_nation_data(uint32_t(1));
	static uint32_t old_count = 1;

	auto new_count = state.world.nation_size();
	if(new_count > old_count) {
		per_nation_data = state.world.nation_make_vectorizable_float_buffer();
		old_count = new_count;
	}

	state.world.execute_serial_over_nation([&](auto nids) {
		per_nation_data.set(nids, 0.f);
	});

	float total = 0.f;

	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		state.world.market_for_each_trade_route(market, [&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, c);
			auto origin =
				current_volume > 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);
			auto target =
				current_volume <= 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);

			if(target != market) return;

			auto sat = state.world.market_get_actual_probability_to_buy(origin, c);
			auto absolute_volume = std::abs(current_volume);
			auto s_origin = state.world.market_get_zone_from_local_market(origin);
			auto s_target = state.world.market_get_zone_from_local_market(target);
			auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
			auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

			auto distance = state.world.trade_route_get_distance(trade_route);

			auto trade_good_loss_mult = std::max(0.f, 1.f - 0.0001f * distance);

			if(n_origin != n_target) {
				per_nation_data.get(n_origin) += sat * absolute_volume * trade_good_loss_mult;
				total += sat * absolute_volume * trade_good_loss_mult;
			}
		});

	});

	nation_enriched_float top[5] = { };

	state.world.for_each_nation([&](dcon::nation_id nid) {
		auto value = per_nation_data.get(nid);
		for(size_t i = 0; i < 5; i++) {
			if(top[i].nation) {
				if(value > top[i].value) {
					for(size_t j = 4; j > i; j--) {
						top[j].value = top[j - 1].value;
						top[j].nation = top[j - 1].nation;
					}
					top[i].nation = nid;
					top[i].value = value;
					return;
				}
			} else {
				top[i].nation = nid;
				top[i].value = value;

				return;
			}
		}
	});

	trade_volume_data_detailed result = {
		total, c, { top[0], top[1], top[2], top[3], top[4] }
	};

	return result;
}

float get_factory_level(sys::state& state, dcon::factory_id f) {
	auto ftid = state.world.factory_get_building_type(f);
	return state.world.factory_get_size(f) / state.world.factory_type_get_base_workforce(ftid);
}

int32_t province_factory_count(sys::state& state, dcon::province_id pid) {
	int32_t num_factories = 0;
	num_factories += int32_t(state.world.province_get_factory_location(pid).end() - state.world.province_get_factory_location(pid).begin());
	for(auto p : state.world.province_get_factory_construction(pid))
		if(p.get_is_upgrade() == false)
			++num_factories;

	// For new factories: no more than defines:FACTORIES_PER_STATE existing + under construction new factories must be
	assert(num_factories <= int32_t(state.defines.factories_per_state));
	return num_factories;
}
// Returns sum of all factory levels in a province
float province_factory_level(sys::state& state, dcon::province_id pid) {
	float factory_size = 0;
	for(auto fl : state.world.province_get_factory_location(pid)) {
		factory_size += get_factory_level(state, fl.get_factory());
	}
	return factory_size;
}

int32_t state_factory_count(sys::state& state, dcon::state_instance_id sid, dcon::nation_id n) {
	int32_t num_factories = 0;
	auto d = state.world.state_instance_get_definition(sid);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d))
		if(p.get_province().get_nation_from_province_ownership() == n)
			num_factories += province_factory_count(state, p.get_province());
	return num_factories;
}
// Returns sum of all factory levels in a state
float state_factory_level(sys::state& state, dcon::state_instance_id sid, dcon::nation_id n) {
	float factory_size = 0;
	auto d = state.world.state_instance_get_definition(sid);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d))
		if(p.get_province().get_nation_from_province_ownership() == n)
			factory_size += province_factory_level(state, p.get_province());
	return factory_size;
}

bool has_factory(sys::state const& state, dcon::province_id si) {
	return true;
	//auto crng = state.world.province_get_factory_construction(si);
	//auto rng = state.world.province_get_factory_location(si);
	//if((crng.begin() != crng.end()) || (rng.begin() != rng.end()))
	//	return true;
	//return false;
}

bool has_factory(sys::state const& state, dcon::state_instance_id sid) {
	return true;
	//auto d = state.world.state_instance_get_definition(sid);
	//for(auto p : state.world.state_definition_get_abstract_state_membership(d))
	//	if(p.get_province().get_state_membership() == sid) {
	//		if(has_factory(state, p.get_province()))
	//			return true;
	//	}
	//return false;
}


bool has_constructed_factory(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id ft) {
	auto d = state.world.state_instance_get_definition(s);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_state_membership() == s) {
			for(auto f : p.get_province().get_factory_location()) {
				if(f.get_factory().get_building_type() == ft)
					return true;
			}
		}
	}
	return false;
}

bool has_factory(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id ft) {
	auto d = state.world.state_instance_get_definition(s);

	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_state_membership() == s) {
			for(auto f : p.get_province().get_factory_location()) {
				if(f.get_factory().get_building_type() == ft)
					return true;
			}
		}
	}
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_state_membership() == s) {
			for(auto f : p.get_province().get_factory_construction()) {
				if(f.get_type() == ft)
					return true;
			}
		}
	}
	return false;
}

float effective_tariff_import_rate(sys::state& state, dcon::nation_id n, dcon::market_id m) {
	auto tariff_efficiency = std::max(0.0f, nations::tariff_efficiency(state, n, m));
	auto r = tariff_efficiency * float(state.world.nation_get_tariffs_import(n)) / 100.0f;
	return std::max(r, 0.0f);
}
float effective_tariff_export_rate(sys::state& state, dcon::nation_id n, dcon::market_id m) {
	auto tariff_efficiency = std::max(0.0f, nations::tariff_efficiency(state, n, m));
	auto r = tariff_efficiency * float(state.world.nation_get_tariffs_export(n)) / 100.0f;
	return std::max(r, 0.0f);
}


float estimate_probability_to_buy_after_demand_increase(sys::state& state, dcon::market_id m, dcon::commodity_id c, float additional_demand) {
	auto historical_demand = state.world.market_get_aggregated_demand_history(m, c);
	auto historical_supply = state.world.market_get_aggregated_supply_history(m, c);
	auto target_demand = historical_demand + additional_demand;
	return target_demand == 0.f ? 0.f : std::min(1.f, historical_supply / target_demand);
}

float estimate_probability_to_buy_after_supply_increase(sys::state& state, dcon::market_id m, dcon::commodity_id c, float additional_supply) {
	auto historical_demand = state.world.market_get_aggregated_demand_history(m, c);
	auto historical_supply = state.world.market_get_aggregated_supply_history(m, c);
	auto target_supply = historical_supply + additional_supply;
	return historical_demand == 0.f ? 0.f : std::min(1.f, target_supply / historical_demand);
}

float estimate_probability_to_sell_after_supply_increase(sys::state& state, dcon::market_id m, dcon::commodity_id c, float additional_supply) {
	auto historical_demand = state.world.market_get_aggregated_demand_history(m, c);
	auto historical_supply = state.world.market_get_aggregated_supply_history(m, c);
	auto target_supply = historical_supply + additional_supply;
	return target_supply == 0.f ? 0.f : std::min(1.f, historical_demand / target_supply);
}


float estimate_next_budget(sys::state& state, dcon::nation_id n) {
	// treasury is remainder after spending + income
	// so there is no need to account for income as it's already there
	auto treasury = state.world.nation_get_stockpiles(n, economy::money);
	return treasury;
}


/*

Market:

Buys out supply of locals as long as they are in demand.
Sells to locals demanded items as long as they are supplied

Exports are accounted as part of demand.
Imports are going directly into the stockpiles, so they are not a part of local supply.

*/
market_budget breakdown_market_budget(sys::state& unsafe_state, dcon::market_id m) {
	const sys::state& state = unsafe_state;

	market_budget result {};

	unsafe_state.world.for_each_commodity([&](auto cid) {
		auto p = price(state, m, cid);
		result.bought += (supply(unsafe_state, m, cid) - trade_supply(state, m, cid)) * p * state.world.market_get_actual_probability_to_sell(m, cid);
		result.sold += demand(unsafe_state, m, cid) * p * state.world.market_get_actual_probability_to_buy(m, cid);
	});


	auto treasury = state.world.market_get_stockpile(m, economy::money);

	result.dividents = treasury > 0 ? treasury * economy::pops::trade_dividents_rate : 0.f;

	auto sid = state.world.market_get_zone_from_local_market(m);
	province::for_each_province_in_state_instance(unsafe_state, sid, [&](auto pid) {
		state.world.province_for_each_pop_location(pid, [&](auto poploc){
			auto pop = state.world.pop_location_get_pop(poploc);
			result.investments += economy::pops::estimate_trade_spending(state, pop);
		});

		float rgo_workers_wage =
			state.world.province_get_pop_labor_distribution(pid, pop_labor::rgo_worker_no_education)
			* state.world.province_get_labor_price(pid, labor::no_education)
			* state.world.province_get_labor_supply_sold(pid, labor::no_education);

		float aristocrats_share = state.world.province_get_landowners_share(pid);
		float others_share = (1.f - aristocrats_share);

		// FACTORIES
		// all profits go to market stockpiles and then they are distributed to capitalists
		for(auto f : state.world.province_get_factory_location(pid)) {
			auto fac = f.get_factory();
			auto profit = explain_last_factory_profit(state, fac);
			result.factories += profit.profit;
		}

		auto total_rgo_profit = 0.f;
		total_rgo_profit += state.world.province_get_rgo_profit(pid);
		for(auto pl : state.world.province_get_pop_location(pid)) {
			if(pl.get_pop().get_poptype() == state.culture_definitions.slaves) {
				total_rgo_profit += pl.get_pop().get_size() * rgo_workers_wage;
			}
		}

		auto local_market_cut = economy::pops::market_cut(state, m, state.world.province_get_labor_price(pid, labor::no_education));

		result.rgo = total_rgo_profit * local_market_cut;
		total_rgo_profit -= result.rgo;

		float num_aristocrat = state.world.province_get_demographics(
			pid,
			demographics::to_key(state, state.culture_definitions.aristocrat)
		);

		if(total_rgo_profit >= 0.f && num_aristocrat > 0.f) {
			result.rgo += total_rgo_profit * others_share;
		} else {
			result.rgo += total_rgo_profit;
		}

		for(int32_t i = 0; i < advanced_province_buildings::list::total; i++) {
			auto& def = advanced_province_buildings::definitions[i];
			auto private_size = state.world.province_get_advanced_province_building_private_size(pid, i);
			auto cost_of_input = state.world.province_get_labor_price(pid, def.throughput_labour_type);
			auto cost_of_output = state.world.province_get_service_price(pid, def.output);
			auto actually_bought = state.world.province_get_labor_demand_satisfaction(pid, def.throughput_labour_type);

			auto output = state.world.province_get_advanced_province_building_private_output(pid, i);
			auto actually_sold = state.world.province_get_service_sold(pid, def.output);

			auto profit = output * actually_sold * cost_of_output - private_size * cost_of_input * actually_bought;

			result.services += profit;
		}
	});

	unsafe_state.world.for_each_commodity([&](auto cid) {
		state.world.market_for_each_trade_route(m, [&](auto route){
			trade_and_tariff<dcon::trade_route_id> details = explain_trade_route_commodity(unsafe_state, route, cid);
			if(m == details.origin) {
				result.exports += details.payment_received_per_unit * details.amount_origin;
			} else {
				result.imports += details.payment_per_unit * details.amount_origin;
			}
		});
	});

	result.estimated_change = -result.bought + result.sold + result.investments - result.dividents +result.rgo + result.factories - result.imports + result.exports;

	return result;
}

void make_trade_center_tooltip(sys::state& state, text::columnar_layout& contents, dcon::market_id market) {
	auto budget = breakdown_market_budget(state, market);
	auto current = state.world.market_get_stockpile(market, money);
	text::add_line(state, contents, "trade_centre_money", text::variable_type::val, text::fp_currency{ current });
	text::add_line(state, contents, "trade_centre_estimated_money", text::variable_type::val, text::fp_currency{ current + budget.estimated_change });
	text::add_line(state, contents, "trade_centre_estimated_change", text::variable_type::val, text::fp_currency{ budget.estimated_change });
	text::add_line(state, contents, "trade_centre_bought", text::variable_type::val, text::fp_currency{ -budget.bought }, 15);
	text::add_line(state, contents, "trade_centre_import", text::variable_type::val, text::fp_currency{ -budget.imports }, 15);
	text::add_line(state, contents, "trade_centre_export", text::variable_type::val, text::fp_currency{ budget.exports }, 15);
	text::add_line(state, contents, "trade_centre_sold", text::variable_type::val, text::fp_currency{ budget.sold }, 15);
	text::add_line(state, contents, "trade_centre_investments", text::variable_type::val, text::fp_currency{ budget.investments }, 15);
	text::add_line(state, contents, "trade_centre_dividents", text::variable_type::val, text::fp_currency{ -budget.dividents }, 15);
	text::add_line(state, contents, "trade_centre_factories", text::variable_type::val, text::fp_currency{ budget.factories }, 15);
	text::add_line(state, contents, "trade_centre_rgo", text::variable_type::val, text::fp_currency{ budget.rgo }, 15);
	text::add_line(state, contents, "trade_centre_services", text::variable_type::val, text::fp_currency{ budget.services }, 15);
}

}
