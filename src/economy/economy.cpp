#include "economy.hpp"
#include "economy_templates.hpp"
#include "demographics.hpp"
#include "dcon_generated.hpp"
#include "ai.hpp"
#include "system_state.hpp"
#include "prng.hpp"
#include "math_fns.hpp"
#include "nations_templates.hpp"
#include "province_templates.hpp"
#include "triggers.hpp"

namespace economy {


float pop_min_wage_factor(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_modifier_values(n, sys::national_mod_offsets::minimum_wage);
}
template<typename T>
ve::fp_vector ve_pop_min_wage_factor(sys::state& state, T n) {
	return state.world.nation_get_modifier_values(n, sys::national_mod_offsets::minimum_wage);
}
template<typename T>
ve::fp_vector ve_artisan_min_wage(sys::state& state, T markets) {
	auto life = state.world.market_get_life_needs_costs(markets, state.culture_definitions.artisans);
	auto everyday = state.world.market_get_everyday_needs_costs(markets, state.culture_definitions.artisans);
	return (life + everyday) * artisans_greed;
}
template<typename T>
ve::fp_vector ve_farmer_min_wage(sys::state& state, T markets, ve::fp_vector min_wage_factor) {
	auto life = state.world.market_get_life_needs_costs(markets, state.culture_definitions.farmers);
	auto everyday = state.world.market_get_everyday_needs_costs(markets, state.culture_definitions.farmers);
	return min_wage_factor * (life + everyday) * 1.1f;
}
template<typename T>
ve::fp_vector ve_laborer_min_wage(sys::state& state, T markets, ve::fp_vector min_wage_factor) {
	auto life = state.world.market_get_life_needs_costs(markets, state.culture_definitions.laborers);
	auto everyday = state.world.market_get_everyday_needs_costs(markets, state.culture_definitions.laborers);
	return min_wage_factor * (life + everyday) * 1.1f;
}
float farmer_min_wage(sys::state& state, dcon::market_id m, float min_wage_factor) {
	auto life = state.world.market_get_life_needs_costs(m, state.culture_definitions.farmers);
	auto everyday = state.world.market_get_everyday_needs_costs(m, state.culture_definitions.farmers);
	return min_wage_factor * (life + everyday) * 1.1f;
}
float laborer_min_wage(sys::state& state, dcon::market_id m, float min_wage_factor) {
	auto life = state.world.market_get_life_needs_costs(m, state.culture_definitions.laborers);
	auto everyday = state.world.market_get_everyday_needs_costs(m, state.culture_definitions.laborers);
	return min_wage_factor * (life + everyday) * 1.1f;
}
float factory_min_wage(sys::state& state, dcon::market_id m, dcon::state_instance_id s, float min_wage_factor) {
	auto employed = state.world.state_instance_get_demographics(s, demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
	auto total = state.world.state_instance_get_demographics(s, demographics::to_key(state, state.culture_definitions.primary_factory_worker));
	auto unemployement_crisis_measures = ve::select(total > 0.f, employed / total, 1.f);
	auto life = state.world.market_get_life_needs_costs(m, state.culture_definitions.primary_factory_worker);
	auto everyday = state.world.market_get_everyday_needs_costs(m, state.culture_definitions.primary_factory_worker);

	return min_wage_factor * (life + everyday) * 1.1f * unemployement_crisis_measures * unemployement_crisis_measures * unemployement_crisis_measures;
}
template<typename T, typename S>
ve::fp_vector ve_factory_min_wage(
	sys::state& state,
	T markets,
	S zones,
	ve::fp_vector min_wage_factor
) {
	auto employed = state.world.state_instance_get_demographics(zones, demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
	auto total = state.world.state_instance_get_demographics(zones, demographics::to_key(state, state.culture_definitions.primary_factory_worker));
	auto unemployement_crisis_measures = ve::select(total > 0.f, employed / total, 1.f);
	auto life = state.world.market_get_life_needs_costs(markets, state.culture_definitions.primary_factory_worker);
	auto everyday = state.world.market_get_everyday_needs_costs(markets, state.culture_definitions.primary_factory_worker);

	return min_wage_factor * (life + everyday) * 1.1f * unemployement_crisis_measures * unemployement_crisis_measures * unemployement_crisis_measures;
}


constexpr inline auto utility_decay_p = 0.998f;
constexpr inline auto current_profits_weight_p =
utility_decay_p
/ (1 - utility_decay_p);
constexpr inline auto short_term_profits_weight_p =
current_profits_weight_p
/ (1 - utility_decay_p);
constexpr inline auto mid_term_profits_weight_p =
(2 * short_term_profits_weight_p - current_profits_weight_p)
/ (1.f - utility_decay_p);
constexpr inline auto long_term_profits_weight_p =
(3 * mid_term_profits_weight_p - 3 * short_term_profits_weight_p + current_profits_weight_p)
/ (1.f - utility_decay_p);

//constexpr inline auto utility_decay_n = 0.6f;
constexpr inline auto utility_decay_n = 0.998f;
constexpr inline auto current_profits_weight_n =
utility_decay_n
/ (1 - utility_decay_n);
constexpr inline auto short_term_profits_weight_n =
current_profits_weight_n
/ (1 - utility_decay_n);
constexpr inline auto mid_term_profits_weight_n =
(2 * short_term_profits_weight_n - current_profits_weight_n)
/ (1.f - utility_decay_n);
constexpr inline auto long_term_profits_weight_n =
(3 * mid_term_profits_weight_n - 3 * short_term_profits_weight_n + current_profits_weight_n)
/ (1.f - utility_decay_n);

constexpr inline auto utility_decay_trade = 0.1f;
constexpr inline auto current_profits_weight_trade =
utility_decay_trade
/ (1 - utility_decay_trade);
constexpr inline auto short_term_profits_weight_trade =
current_profits_weight_trade
/ (1 - utility_decay_trade);
constexpr inline auto mid_term_profits_weight_trade =
(2 * short_term_profits_weight_trade - current_profits_weight_trade)
/ (1.f - utility_decay_trade);
constexpr inline auto long_term_profits_weight_trade =
(3 * mid_term_profits_weight_trade - 3 * short_term_profits_weight_trade + current_profits_weight_trade)
/ (1.f - utility_decay_trade);

enum class economy_reason {
	pop, factory, rgo, artisan, construction, nation, stockpile, overseas_penalty, trade
};

float expected_savings_per_capita(sys::state& state) {
	float primary_commodity_basket = 0.0f;
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			primary_commodity_basket += 2.0f * state.world.commodity_get_cost(c) * state.world.pop_type_get_life_needs(pt, c);
			primary_commodity_basket += 2.0f * state.world.commodity_get_cost(c) * state.world.pop_type_get_everyday_needs(pt, c);
		});
	});
	primary_commodity_basket /= float(state.world.pop_type_size());

	return 10.f * primary_commodity_basket / state.defines.alice_needs_scaling_factor;
}

void sanity_check(sys::state& state) {
#ifdef NDEBUG
#else
	/*
	ve::fp_vector total{};
	state.world.execute_serial_over_pop([&](auto ids) {
		total = total + state.world.pop_get_savings(ids);
	});
	assert(total.reduce() > 0.f);
	*/
	//assert(state.inflation > 0.1f && state.inflation < 10.f);
#endif
}

float price(sys::state& state, dcon::state_instance_id s, dcon::commodity_id c) {
	auto market = state.world.state_instance_get_market_from_local_market(s);
	return state.world.market_get_price(market, c);
}

float price(sys::state& state, dcon::nation_id s, dcon::commodity_id c) {
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

template<typename T>
ve::fp_vector ve_price(sys::state& state, T s, dcon::commodity_id c) {
	return state.world.market_get_price(s, c);
}

float price(sys::state& state, dcon::market_id s, dcon::commodity_id c) {
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

	return total_supply - domestic_trade_volume(state, s, c);
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

			auto s_origin = state.world.market_get_zone_from_local_market(origin);
			auto s_target = state.world.market_get_zone_from_local_market(target);
			auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
			auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

			if(n_origin != n_target) return;

			auto sat = state.world.market_get_direct_demand_satisfaction(origin, c);
			auto absolute_volume = std::abs(current_volume);

			total_volume += sat * absolute_volume;
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
	return total_demand;
}

float consumption(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
) {
	return state.world.market_get_demand(s, c) * state.world.market_get_demand_satisfaction(s, c);
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
		total += local * state.world.market_get_demand_satisfaction(market, c);
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
		total += local * state.world.market_get_demand_satisfaction(m, c);
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
	float total = 0.f;
	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		total += state.world.market_get_export(market, c);
	});
	return total;
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

			auto sat = state.world.market_get_direct_demand_satisfaction(origin, c);
			auto absolute_volume = std::abs(current_volume);
			auto s_origin = state.world.market_get_zone_from_local_market(origin);
			auto s_target = state.world.market_get_zone_from_local_market(target);
			auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
			auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

			auto distance = state.world.trade_route_get_distance(trade_route);

			auto trade_good_loss_mult = std::max(0.f, 1.f - 0.0001f * distance);

			if(n_origin != n_target) {
				per_nation_data.get(n_target) += sat * absolute_volume;
			}
		});

		total += state.world.market_get_export(market, c);
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
	float total = 0.f;
	state.world.nation_for_each_state_ownership(s, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		total += state.world.market_get_import(market, c);
	});
	return total;
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

			auto sat = state.world.market_get_direct_demand_satisfaction(origin, c);
			auto absolute_volume = std::abs(current_volume);
			auto s_origin = state.world.market_get_zone_from_local_market(origin);
			auto s_target = state.world.market_get_zone_from_local_market(target);
			auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
			auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

			auto distance = state.world.trade_route_get_distance(trade_route);

			auto trade_good_loss_mult = std::max(0.f, 1.f - 0.0001f * distance);

			if(n_origin != n_target) {
				per_nation_data.get(n_origin) += sat * absolute_volume * trade_good_loss_mult;
			}
		});

		total += state.world.market_get_import(market, c);
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

// register demand functions

void register_demand(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id commodity_type,
	float amount,
	economy_reason reason
) {
	assert(amount >= 0.f);
	state.world.market_get_demand(s, commodity_type) += amount;
	assert(std::isfinite(state.world.market_get_demand(s, commodity_type)));
}

template<typename T>
void register_demand(
	sys::state& state,
	T s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
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

template<typename T>
void register_intermediate_demand(
	sys::state& state,
	T s,
	dcon::commodity_id c,
	ve::fp_vector amount,
	economy_reason reason
) {
	register_demand(state, s, c, amount, reason);
	state.world.market_set_intermediate_demand(
		s,
		c,
		state.world.market_get_intermediate_demand(s, c) + amount
	);
	auto local_price = ve_price(state, s, c);
	auto sat = state.world.market_get_demand_satisfaction(s, c);
	state.world.market_set_gdp(s, state.world.market_get_gdp(s) - amount * local_price * sat);
}


// it's registered as demand separately
void register_construction_demand(sys::state& state, dcon::market_id s, dcon::commodity_id commodity_type, float amount) {
	state.world.market_get_construction_demand(s, commodity_type) += amount;
	assert(state.world.market_get_construction_demand(s, commodity_type) >= 0.f);
}

void register_domestic_supply(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id commodity_type,
	float amount,
	economy_reason reason
) {
	state.world.market_get_supply(s, commodity_type) += amount;
	state.world.market_get_gdp(s) += amount * price(state, s, commodity_type);
}

void register_foreign_supply(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id commodity_type,
	float amount,
	economy_reason reason
) {
	state.world.market_get_supply(s, commodity_type) += amount;
}

template<typename T>
void register_domestic_supply(
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
	state.world.market_set_gdp(
		s,
		state.world.market_get_gdp(s)
		+ amount * ve_price(state, s, commodity_type)
	);
}

template void for_each_new_factory<std::function<void(new_factory)>>(sys::state&, dcon::state_instance_id, std::function<void(new_factory)>&&);
template void for_each_upgraded_factory<std::function<void(upgraded_factory)>>(sys::state&, dcon::state_instance_id, std::function<void(upgraded_factory)>&&);

bool can_take_loans(sys::state& state, dcon::nation_id n) {
	if(!state.world.nation_get_is_player_controlled(n) || !state.world.nation_get_is_debt_spending(n))
		return false;

	/*
	A country cannot borrow if it is less than define:BANKRUPTCY_EXTERNAL_LOAN_YEARS since their last bankruptcy.
	*/
	auto last_br = state.world.nation_get_bankrupt_until(n);
	if(last_br && state.current_date < last_br)
		return false;

	return true;
}

float interest_payment(sys::state& state, dcon::nation_id n) {
	/*
	Every day, a nation must pay its creditors. It must pay national-modifier-to-loan-interest x debt-amount x interest-to-debt-holder-rate / 30
	When a nation takes a loan, the interest-to-debt-holder-rate is set at nation-taking-the-loan-technology-loan-interest-modifier + define:LOAN_BASE_INTEREST, with a minimum of 0.01.
	*/
	auto debt = state.world.nation_get_local_loan(n);
	return debt * std::max(0.01f, (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::loan_interest) + 1.0f) * state.defines.loan_base_interest) / 30.0f;
}
float max_loan(sys::state& state, dcon::nation_id n) {
	/*
	There is an income cap to how much may be borrowed, namely: define:MAX_LOAN_CAP_FROM_BANKS x (national-modifier-to-max-loan-amount + 1) x national-tax-base.
	*/
	auto mod = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_loan_modifier) + 1.0f);
	auto total_tax_base = state.world.nation_get_total_rich_income(n) + state.world.nation_get_total_middle_income(n) + state.world.nation_get_total_poor_income(n);
	return std::max(0.0f, (total_tax_base + state.world.nation_get_national_bank(n)) * mod);
}

int32_t most_recent_price_record_index(sys::state& state) {
	return (state.current_date.value >> 4) % price_history_length;
}
int32_t previous_price_record_index(sys::state& state) {
	return ((state.current_date.value >> 4) + price_history_length - 1) % price_history_length;
}

int32_t most_recent_gdp_record_index(sys::state& state) {
	auto date = state.current_date.to_ymd(state.start_date);
	return (date.year * 4 + date.month / 3) % gdp_history_length;
}
int32_t previous_gdp_record_index(sys::state& state) {
	auto date = state.current_date.to_ymd(state.start_date);
	return ((date.year * 4 + date.month / 3) + gdp_history_length - 1) % gdp_history_length;
}

float ideal_pound_conversion_rate(sys::state& state, dcon::market_id n) {
	return state.world.market_get_life_needs_costs(n, state.culture_definitions.primary_factory_worker)
		+ 0.1f * state.world.market_get_everyday_needs_costs(n, state.culture_definitions.primary_factory_worker);
}

float gdp_adjusted(sys::state& state, dcon::market_id n) {
	float raw = state.world.market_get_gdp(n);
	float ideal_pound = ideal_pound_conversion_rate(state, n);
	return raw / ideal_pound;
}

float gdp_adjusted(sys::state& state, dcon::nation_id n) {
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		total = total + economy::gdp_adjusted(state, market);
	});
	return total;
}

float full_spending_cost(sys::state& state, dcon::nation_id n);
void populate_army_consumption(sys::state& state);
void populate_navy_consumption(sys::state& state);
void populate_construction_consumption(sys::state& state);

float commodity_daily_production_amount(sys::state& state, dcon::commodity_id c) {
	// TODO
	return 0.f;
}

float stockpile_commodity_daily_increase(sys::state& state, dcon::commodity_id c, dcon::nation_id n) {
	// TODO
	return 0.f;
}

float global_market_commodity_daily_increase(sys::state& state, dcon::commodity_id c) {
	// TODO
	return 0.f;
}

bool has_factory(sys::state const& state, dcon::state_instance_id si) {
	auto sdef = state.world.state_instance_get_definition(si);
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);
	auto crng = state.world.state_instance_get_state_building_construction(si);
	if(crng.begin() != crng.end())
		return true;

	for(auto p : state.world.state_definition_get_abstract_state_membership(sdef)) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			auto rng = p.get_province().get_factory_location();
			if(rng.begin() != rng.end())
				return true;
		}
	}
	return false;
}


void initialize_artisan_distribution(sys::state& state) {
	state.world.market_resize_artisan_score(state.world.commodity_size());
	state.world.market_resize_artisan_actual_production(state.world.commodity_size());

	auto const csize = state.world.commodity_size();

	for(auto m : state.world.in_market) {
		auto zone = state.world.market_get_zone_from_local_market(m);
		auto nation = state.world.state_instance_get_nation_from_state_ownership(zone);

		for(uint32_t i = 1; i < csize; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

			if(
				state.world.commodity_get_artisan_output_amount(cid) > 0.0f
				&& (
					state.world.commodity_get_is_available_from_start(cid)
					|| state.world.nation_get_unlocked_commodities(nation, cid)
					)
			) {
				m.set_artisan_score(cid, 0.f);
			}
		}
	}
}
bool valid_need(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	return state.world.commodity_get_is_available_from_start(c)
		|| state.world.nation_get_unlocked_commodities(n, c);
}

template<typename T>
ve::mask_vector valid_need(sys::state& state, T n, dcon::commodity_id c) {
	ve::mask_vector available_at_start = state.world.commodity_get_is_available_from_start(c);
	ve::mask_vector active_mask = state.world.nation_get_unlocked_commodities(n, c);
	return available_at_start || active_mask;
}

bool valid_life_need(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	return state.world.commodity_get_is_life_need(c)
		&& (
			state.world.commodity_get_is_available_from_start(c)
			|| state.world.nation_get_unlocked_commodities(n, c)
		);
}
bool valid_everyday_need(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	return state.world.commodity_get_is_everyday_need(c)
		&& (
			state.world.commodity_get_is_available_from_start(c)
			|| state.world.nation_get_unlocked_commodities(n, c)
		);
}
bool valid_luxury_need(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	return state.world.commodity_get_is_luxury_need(c)
		&& (
			state.world.commodity_get_is_available_from_start(c)
			|| state.world.nation_get_unlocked_commodities(n, c)
		);
}

void initialize_needs_weights(sys::state& state, dcon::market_id n) {
	auto zone = state.world.market_get_zone_from_local_market(n);
	auto nation = state.world.state_instance_get_nation_from_state_ownership(zone);
	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_life_need(state, nation, c)) {
				auto& w = state.world.market_get_life_needs_weights(n, c);
				w = 0.001f;
			}
		});
	}
	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_everyday_need(state, nation, c)) {
				auto& w = state.world.market_get_everyday_needs_weights(n, c);
				w = 0.001f;
			}
		});
	}
	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_luxury_need(state, nation, c)) {
				auto& w = state.world.market_get_luxury_needs_weights(n, c);
				w = 0.001f;
			}
		});
	}
}

float need_weight(sys::state& state, dcon::market_id n, dcon::commodity_id c) {
	return 1.0f / std::max(price(state, n, c), 0.0001f);
}

void rebalance_needs_weights(sys::state& state, dcon::market_id n) {
	auto zone = state.world.market_get_zone_from_local_market(n);
	auto nation = state.world.state_instance_get_nation_from_state_ownership(zone);

	/*
	{
		float total_weights = 0.f;
		uint32_t count = 0;

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_life_need(state, nation, c)) {
				auto weight = need_weight(state, n, c);
				total_weights += weight;
				count++;
			}
		});

		// remove some of needs completely depending on price
		float total_weights_adjusted = 0.f;
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_life_need(state, nation, c)) {
				auto weight = need_weight(state, n, c);
				if(weight > total_weights * 0.25f / count) {
					total_weights_adjusted += weight;
				}
			}
		});

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(valid_life_need(state, nation, c)) {
				auto weight = need_weight(state, n, c);
				auto ideal_weighting = weight / total_weights_adjusted * count;
				if(weight < total_weights * 0.25f / count) {
					ideal_weighting = 0;
				}
				auto& w = state.world.market_get_life_needs_weights(n, c);
				w = ideal_weighting * state.defines.alice_need_drift_speed + w * (1.0f - state.defines.alice_need_drift_speed);

				assert(std::isfinite(w));
				assert(w <= count + 0.01f);
			}
		});
	}

	{
		float total_weights = 0.f;
		uint32_t count = 0;

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(valid_everyday_need(state, nation, c)) {
				auto weight = need_weight(state, n, c);
				total_weights += weight;
				count++;
			}
		});

		// remove some of needs completely depending on price
		float total_weights_adjusted = 0.f;
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_everyday_need(state, nation, c)) {
				auto weight = need_weight(state, n, c);
				if(weight > total_weights * 0.25f / count) {
					total_weights_adjusted += weight;
				}
			}
		});

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(valid_everyday_need(state, nation, c)) {
				auto weight = need_weight(state, n, c);
				auto ideal_weighting = weight / total_weights_adjusted * count;
				if(weight < total_weights * 0.25f / count) {
					ideal_weighting = 0;
				}
				auto& w = state.world.market_get_everyday_needs_weights(n, c);
				w = ideal_weighting * state.defines.alice_need_drift_speed + w * (1.0f - state.defines.alice_need_drift_speed);

				assert(std::isfinite(w));
				assert(w <= count + 0.01f);
			}
		});
	}

	{
		float total_weights = 0.f;
		uint32_t count = 0;

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(valid_luxury_need(state, nation, c)) {
				auto weight = need_weight(state, n, c);
				total_weights += weight;
				count++;
			}
		});

		// remove some of needs completely depending on price
		float total_weights_adjusted = 0.f;
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_luxury_need(state, nation, c)) {
				auto weight = need_weight(state, n, c);
				if(weight > total_weights * 0.25f / count) {
					total_weights_adjusted += weight;
				}
			}
		});

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(valid_luxury_need(state, nation, c)) {
				auto weight = need_weight(state, n, c);
				auto ideal_weighting = weight / total_weights_adjusted * count;
				if(weight < total_weights * 0.25f / count) {
					ideal_weighting = 0;
				}
				auto& w = state.world.market_get_luxury_needs_weights(n, c);
				w = ideal_weighting * state.defines.alice_need_drift_speed + w * (1.0f - state.defines.alice_need_drift_speed);

				assert(std::isfinite(w));
				assert(w <= count + 0.01f);
			}
		});
	}

	*/

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		if(valid_luxury_need(state, nation, c)) {
			state.world.market_set_luxury_needs_weights(n, c, 1.f);
		}
		if(valid_everyday_need(state, nation, c)) {
			state.world.market_set_everyday_needs_weights(n, c, 1.f);
		}
		if(valid_life_need(state, nation, c)) {
			state.world.market_set_life_needs_weights(n, c, 1.f);
		}
	});
}


void convert_commodities_into_ingredients(
	sys::state& state,
	std::vector<float>& buffer_commodities,
	std::vector<float>& buffer_ingredients,
	std::vector<float>& buffer_weights
) {
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		float amount = buffer_commodities[c.index()];

		if(state.world.commodity_get_rgo_amount(c) > 0.f) {
			buffer_ingredients[c.index()] += amount;
		} else {
			//calculate input vectors weights:
			std::vector<float> weights;
			float total_weight = 0.f;
			float non_zero_count = 0.f;

			state.world.for_each_factory_type([&](dcon::factory_type_id t) {
				auto o = state.world.factory_type_get_output(t);
				if(o == c) {
					auto& inputs = state.world.factory_type_get_inputs(t);

					float weight_current = 0;

					for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
						if(inputs.commodity_type[i]) {
							float weight_input = buffer_weights[inputs.commodity_type[i].index()];
							total_weight += weight_input;
							weight_current += weight_input;
						} else {
							break;
						}
					}

					if(weight_current > 0.f)
						non_zero_count++;

					weights.push_back(weight_current);
				}
			});

			if(total_weight == 0) {
				for(size_t i = 0; i < weights.size(); i++) {
					weights[i] = 1.f;
					total_weight++;
				}
			} else {
				float average_weight = total_weight / non_zero_count;
				for(size_t i = 0; i < weights.size(); i++) {
					if(weights[i] == 0.f) {
						weights[i] = average_weight;
						total_weight += average_weight;
					}
				}
			}

			//now we have weights and can use them for transformation of output into ingredients:
			size_t index = 0;

			state.world.for_each_factory_type([&](dcon::factory_type_id t) {
				auto o = state.world.factory_type_get_output(t);
				if(o == c) {
					auto& inputs = state.world.factory_type_get_inputs(t);
					float output_power = state.world.factory_type_get_output_amount(t);

					float weight_current = weights[index] / total_weight;
					index++;

					for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
						if(inputs.commodity_type[i]) {

							buffer_ingredients[inputs.commodity_type[i].index()] += inputs.commodity_amounts[i] * amount / output_power * weight_current;

							float weight_input = buffer_weights[inputs.commodity_type[i].index()];
							total_weight += weight_input;
							weight_current += weight_input;
						} else {
							break;
						}
					}
				}
			});
		}
	});
}

void presimulate(sys::state& state) {
	// economic updates without construction
#ifdef NDEBUG
	uint32_t steps = 365;
#else
	uint32_t steps = 20;
#endif
	for(uint32_t i = 0; i < steps; i++) {
		update_rgo_employment(state);
		update_factory_employment(state);
		daily_update(state, true, (float)i / (float)steps);
		ai::update_budget(state);
	}
}

bool has_building(sys::state const& state, dcon::state_instance_id si, dcon::factory_type_id fac) {
	auto sdef = state.world.state_instance_get_definition(si);
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);
	for(auto p : state.world.state_definition_get_abstract_state_membership(sdef)) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			for(auto b : p.get_province().get_factory_location()) {
				if(b.get_factory().get_building_type() == fac)
					return true;
			}
		}
	}
	return false;
}

bool is_bankrupt_debtor_to(sys::state& state, dcon::nation_id debt_holder, dcon::nation_id debtor) {
	return state.world.nation_get_is_bankrupt(debt_holder) &&
		state.world.unilateral_relationship_get_owns_debt_of(
				state.world.get_unilateral_relationship_by_unilateral_pair(debtor, debt_holder)) > 0.1f;
}

bool nation_is_constructing_factories(sys::state& state, dcon::nation_id n) {
	auto rng = state.world.nation_get_state_building_construction(n);
	return rng.begin() != rng.end();
}
bool nation_has_closed_factories(sys::state& state, dcon::nation_id n) { // TODO - should be "good" now
	auto nation_fat = dcon::fatten(state.world, n);
	for(auto prov_owner : nation_fat.get_province_ownership()) {
		auto prov = prov_owner.get_province();
		for(auto factloc : prov.get_factory_location()) {
			auto scale = factloc.get_factory().get_production_scale();
			if(scale < factory_closed_threshold) {
				return true;
			}
		}
	}
	return false;
}

template<typename T>
auto artisan_input_multiplier(
	sys::state& state,
	T nations
) {
	auto alice_input_base = state.defines.alice_inputs_base_factor_artisans;
	auto nation_input_mod = 1.f + state.world.nation_get_modifier_values(
		nations, sys::national_mod_offsets::artisan_input);

	if constexpr(std::is_same_v<T, dcon::nation_id>) {
		return std::max(alice_input_base * nation_input_mod, 0.01f);
	} else {
		return ve::max(alice_input_base * nation_input_mod, 0.01f);
	}
}

template<typename T>
auto artisan_output_multiplier(
	sys::state& state,
	T nations
) {
	auto alice_output_base = state.defines.alice_output_base_factor_artisans;
	auto nation_output_mod = 1.f + state.world.nation_get_modifier_values(
		nations, sys::national_mod_offsets::artisan_input);

	if constexpr(std::is_same_v<T, dcon::nation_id>) {
		return std::max(alice_output_base * nation_output_mod, 0.01f);
	} else {
		return ve::max(alice_output_base * nation_output_mod, 0.01f);
	}
}

template<typename T>
auto artisan_throughput_multiplier(
	sys::state& state,
	T nations
) {
	return production_throughput_multiplier * ve::max(
		0.01f,
		1.0f
		+ state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::artisan_throughput)
	);
}

template<typename T, typename S>
ve::fp_vector base_artisan_profit(
	sys::state& state,
	T markets,
	S nations,
	dcon::commodity_id c
) {
	auto const& inputs = state.world.commodity_get_artisan_inputs(c);
	ve::fp_vector input_total = 0.0f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_total = input_total + inputs.commodity_amounts[i] * ve_price(state, markets, inputs.commodity_type[i]);
		} else {
			break;
		}
	}

	auto output_total = state.world.commodity_get_artisan_output_amount(c) * ve_price(state, markets, c);

	auto input_multiplier = artisan_input_multiplier(state, nations);
	auto output_multiplier = artisan_output_multiplier(state, nations);

	return output_total * output_multiplier - input_multiplier * input_total;
}

float base_artisan_profit(
	sys::state& state,
	dcon::market_id market,
	dcon::commodity_id c
) {
	auto sid = state.world.market_get_zone_from_local_market(market);
	auto nid = state.world.state_instance_get_nation_from_state_ownership(sid);

	auto const& inputs = state.world.commodity_get_artisan_inputs(c);
	auto input_total = 0.0f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_total = input_total + inputs.commodity_amounts[i] * price(state, market, inputs.commodity_type[i]);
		} else {
			break;
		}
	}

	auto output_total = state.world.commodity_get_artisan_output_amount(c) * price(state, market, c);

	auto input_multiplier = artisan_input_multiplier<dcon::nation_id>(state, nid);
	auto output_multiplier = artisan_output_multiplier<dcon::nation_id>(state, nid);

	return output_total * output_multiplier - input_multiplier * input_total;
}

template<typename T>
ve::mask_vector ve_valid_artisan_good(
	sys::state& state,
	T nations,
	dcon::commodity_id cid
) {
	ve::mask_vector from_the_start = state.world.commodity_get_is_available_from_start(cid);
	ve::mask_vector active = state.world.nation_get_unlocked_commodities(nations, cid);
	auto can_produce = state.world.commodity_get_artisan_output_amount(cid) > 0.0f;

	return can_produce && (from_the_start || active);
}

bool valid_artisan_good(
	sys::state& state,
	dcon::nation_id nations,
	dcon::commodity_id cid
) {
	auto from_the_start = state.world.commodity_get_is_available_from_start(cid);
	auto active = state.world.nation_get_unlocked_commodities(nations, cid);
	auto can_produce = state.world.commodity_get_artisan_output_amount(cid) > 0.0f;

	return can_produce && (from_the_start || active);
}

inline constexpr float ln_2 = 0.30103f;

//crude approximation of exp
float pseudo_exp_for_negative(float f) {
	if(f < -128.f) {
		return 0.f;
	}

	f = f / 128.f;
	f = 1 + f + f * f / 2 + f * f * f / 6;

	f = f * f; // 2
	f = f * f; // 4
	f = f * f; // 8
	f = f * f; // 16
	f = f * f; // 32
	f = f * f; // 64
	f = f * f; // 128

	return f;
}
//crude vectorised approximation of exp
ve::fp_vector ve_pseudo_exp_for_negative(ve::fp_vector f) {
	ve::fp_vector temp = f;

	f = f / 128.f;
	f = 1 + f + f * f / 2 + f * f * f / 6;

	f = f * f; // 2
	f = f * f; // 4
	f = f * f; // 8
	f = f * f; // 16
	f = f * f; // 32
	f = f * f; // 64
	f = f * f; // 128

	ve::fp_vector result = ve::select(temp < -128.f, 0.f, f);

	ve::apply(
		[](float value) {
			assert(std::isfinite(value));
		}, result
	);

	return result;
}

template<typename T, typename S>
void adjust_artisan_balance(
	sys::state& state,
	T markets,
	S nations
) {
	auto const csize = state.world.commodity_size();
	float distribution_drift_speed = 0.001f;

	auto sids = state.world.market_get_zone_from_local_market(markets);
	auto num_artisans = state.world.state_instance_get_demographics(
		sids,
		demographics::to_key(state, state.culture_definitions.artisans)
	);
	ve::fp_vector total{  };
	auto min_wage = ve_artisan_min_wage(state, markets);

	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto mask = ve_valid_artisan_good(state, nations, cid);

		auto base_profit = base_artisan_profit(state, markets, nations, cid);
		auto const& inputs = state.world.commodity_get_artisan_inputs(cid);
		ve::fp_vector min_available = 1.0f;
		ve::fp_vector input_total = 1.0f;

		for(uint32_t j = 0; j < commodity_set::set_size; ++j) {
			if(inputs.commodity_type[j]) {
				min_available = ve::min(
					min_available,
					state.world.market_get_demand_satisfaction(markets, inputs.commodity_type[j]));
				input_total =
					input_total
					+ inputs.commodity_amounts[j]
					* ve_price(state, markets, inputs.commodity_type[j]);
			} else {
				break;
			}
		}

		//auto min_input_importance = ve::min(1.f, 1000.f / (input_total + 0.00001f));
		//auto min_input_coefficient = min_available + (min_input_importance * (1.f - min_available));

		auto profit = ve::select(
			mask,
			base_profit / 10'000.0f - min_wage / state.defines.alice_needs_scaling_factor,
			std::numeric_limits<float>::lowest()
		);

		auto w = state.world.market_get_artisan_score(markets, cid);
		auto current_employment = w * num_artisans;

		auto supply = state.world.market_get_supply(markets, cid)
			+ state.world.market_get_stockpile(markets, cid) * stockpile_to_supply + price_rigging;
		auto demand = state.world.market_get_demand(markets, cid) + price_rigging;

		auto price_acceleration_from_additional_supply =
			price_speed_mod * (demand / supply / supply + 1 / demand);
		auto price_speed_change = price_speed_mod * (demand / supply - supply / demand);

		auto weight_short_term = ve::select(profit < 0, ve::fp_vector{ short_term_profits_weight_n }, ve::fp_vector { short_term_profits_weight_p });
		auto weight_mid_term = ve::select(profit < 0, ve::fp_vector{ mid_term_profits_weight_n }, ve::fp_vector{ mid_term_profits_weight_p });
		auto weight_long_term = ve::select(profit < 0, ve::fp_vector{ long_term_profits_weight_n }, ve::fp_vector{ long_term_profits_weight_p });

		auto hire_rate_from_income =
			profit
			* weight_short_term;

		auto hire_rate_from_predicted_acceleration_of_price =
			-price_acceleration_from_additional_supply * supply * weight_mid_term;

		auto hire_rate_from_predicted_change_of_price =
			price_speed_change * weight_mid_term;

		auto money_to_workers_divisor =
			2
			* weight_long_term
			* price_acceleration_from_additional_supply;

		auto optimal_hire_change =
			(
				hire_rate_from_income
				+ hire_rate_from_predicted_acceleration_of_price //overestimate to account for stockpiles
				+ hire_rate_from_predicted_change_of_price
			) / money_to_workers_divisor;

		auto change = optimal_hire_change * 1000.f;
		auto new_employment = ve::max(current_employment + change, 0.0f);

		auto new_weight = ve::select(
			mask,
			new_employment / (num_artisans + 1.f),
			0.f
		);

		state.world.market_set_artisan_score(markets, cid, new_weight);
		total = total + new_weight;

		ve::apply(
			[](float x) {
				assert(std::isfinite(x));
			}, new_weight
		);
	}

	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto w = state.world.market_get_artisan_score(markets, cid);
		w = ve::select(total >= 1.f, w / total, w);
		state.world.market_set_artisan_score(markets, cid, w);
	}
}

void initialize(sys::state& state) {
	initialize_artisan_distribution(state);

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		state.world.execute_serial_over_market([&](auto markets) {
			state.world.market_set_price(markets, c, state.world.commodity_get_cost(c));
			state.world.market_set_demand(markets, c, ve::fp_vector{});
			state.world.market_set_supply(markets, c, ve::fp_vector{});
			state.world.market_set_consumption(markets, c, ve::fp_vector{});
		});

		auto fc = fatten(state.world, c);

		for(uint32_t i = 0; i < price_history_length; ++i) {
			fc.set_price_record(i, fc.get_cost());
		}
		// fc.set_global_market_pool();
	});

	/*
	auto savings_buffer = state.world.pop_type_make_vectorizable_float_buffer();
	state.world.for_each_pop_type([&](dcon::pop_type_id t) {
		auto ft = fatten(state.world, t);
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			savings_buffer.get(t) +=
				state.world.commodity_get_is_available_from_start(c)
				?
				state.world.commodity_get_cost(c) * ft.get_life_needs(c)
				+ 0.5f * state.world.commodity_get_cost(c) * ft.get_everyday_needs(c)
				: 0.0f;
		});
		auto strata = (ft.get_strata() * 2) + 1;
		savings_buffer.get(t) *= strata;
	});
	*/


	auto expected_savings = expected_savings_per_capita(state);

	state.world.for_each_pop([&](dcon::pop_id p) {
		auto fp = fatten(state.world, p);
		pop_demographics::set_life_needs(state, p, 1.0f);
		pop_demographics::set_everyday_needs(state, p, 0.1f);
		pop_demographics::set_luxury_needs(state, p, 0.0f);
		fp.set_savings(fp.get_size() * expected_savings * 10.f);
	});

	sanity_check(state);

	state.world.execute_serial_over_market([&](auto mid) {
		state.world.market_set_income_scale(mid, 1.f);
	});

	state.world.for_each_factory([&](dcon::factory_id f) {
		auto ff = fatten(state.world, f);
		ff.set_production_scale(1.0f);
	});

	// learn some weights for rgo from initial territories:
	auto csize = state.world.commodity_size();
	std::vector<std::vector<float>> per_climate_distribution_buffer(state.world.modifier_size() + 1, std::vector<float>(csize + 1, 0.f));
	std::vector<std::vector<float>> per_terrain_distribution_buffer(state.world.modifier_size() + 1, std::vector<float>(csize + 1, 0.f));
	std::vector<std::vector<float>> per_continent_distribution_buffer(state.world.modifier_size() + 1, std::vector<float>(csize + 1, 0.f));

	// init the map for climates
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto fp = fatten(state.world, p);
		dcon::commodity_id main_trade_good = state.world.province_get_rgo(p);
		if(state.world.commodity_get_money_rgo(main_trade_good)) {
			return;
		}
		dcon::modifier_id climate = fp.get_climate();
		dcon::modifier_id terrain = fp.get_terrain();
		dcon::modifier_id continent = fp.get_continent();
		per_climate_distribution_buffer[climate.value][main_trade_good.value] += 1.f;
		per_terrain_distribution_buffer[terrain.value][main_trade_good.value] += 1.f;
		per_continent_distribution_buffer[continent.value][main_trade_good.value] += 1.f;
	});

	// normalisation
	for(uint32_t i = 0; i < uint32_t(state.world.modifier_size()); i++) {
		float climate_sum = 0.f;
		float terrain_sum = 0.f;
		float continent_sum = 0.f;
		for(uint32_t j = 0; j < csize; j++) {
			climate_sum += per_climate_distribution_buffer[i][j];
			terrain_sum += per_terrain_distribution_buffer[i][j];
			continent_sum += per_continent_distribution_buffer[i][j];
		}
		for(uint32_t j = 0; j < csize; j++) {
			per_climate_distribution_buffer[i][j] *= climate_sum == 0.f ? 1.f : 1.f / climate_sum;
			per_terrain_distribution_buffer[i][j] *= terrain_sum == 0.f ? 1.f : 1.f / terrain_sum;
			per_continent_distribution_buffer[i][j] *= continent_sum == 0.f ? 1.f : 1.f / continent_sum;
		}
	}

	province::for_each_land_province(state, [&](dcon::province_id p) {
		if(state.world.province_get_rgo_was_set_during_scenario_creation(p)) {
			return;
		}

		auto fp = fatten(state.world, p);
		dcon::modifier_id climate = fp.get_climate();
		dcon::modifier_id terrain = fp.get_terrain();
		dcon::modifier_id continent = fp.get_continent();

		dcon::commodity_id main_trade_good = state.world.province_get_rgo(p);
		bool is_mine = state.world.commodity_get_is_mine(main_trade_good);

		//max size of exploitable land:
		auto max_rgo_size = std::ceil(2000.f / state.defines.alice_rgo_per_size_employment
			* state.map_state.map_data.province_area[province::to_map_id(p)]);

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			fp.set_rgo_employment_per_good(c, 0.f);
			fp.set_rgo_target_employment_per_good(c, 0.f);
		});

		// currently exploited land
		float pop_amount = 0.0f;
		for(auto pt : state.world.in_pop_type) {
			if(pt == state.culture_definitions.slaves) {
				pop_amount += state.world.province_get_demographics(p, demographics::to_key(state, state.culture_definitions.slaves));
			} else if(pt.get_is_paid_rgo_worker()) {
				pop_amount += state.world.province_get_demographics(p, demographics::to_key(state, pt));
			}
		}

		auto size_at_the_start_of_the_game = std::ceil(pop_amount / state.defines.alice_rgo_per_size_employment);
		auto real_size = std::min(size_at_the_start_of_the_game * 1.5f, max_rgo_size);

		assert(std::isfinite(real_size));
		fp.set_rgo_size(real_size);

		static std::vector<float> true_distribution;
		true_distribution.resize(state.world.commodity_size());

		float total = 0.f;
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			float climate_d = per_climate_distribution_buffer[climate.value][c.value];
			float terrain_d = per_terrain_distribution_buffer[terrain.value][c.value];
			float continent_d = per_continent_distribution_buffer[continent.value][c.value];
			float current = (climate_d + terrain_d) * (climate_d + terrain_d) * continent_d;
			true_distribution[c.index()] = current;
			total += current;
		});

		// remove continental restriction if failed:
		if(total == 0.f) {
			state.world.for_each_commodity([&](dcon::commodity_id c) {
				float climate_d = per_climate_distribution_buffer[climate.value][c.value];
				float terrain_d = per_terrain_distribution_buffer[terrain.value][c.value];
				float current = (climate_d + terrain_d) * (climate_d + terrain_d);
				true_distribution[c.index()] = current;
				total += current;
			});
		}

		// make it into uniform distrubution on available goods then...
		if(total == 0.f) {
			state.world.for_each_commodity([&](dcon::commodity_id c) {
				if(state.world.commodity_get_money_rgo(c)) {
					return;
				}
				if(!state.world.commodity_get_is_available_from_start(c)) {
					return;
				}
				float current = 1.f;
				true_distribution[c.index()] = current;
				total += current;
			});
		}

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			assert(std::isfinite(total));
			// if everything had failed for some reason, then assume 0 distribution: main rgo is still active
			if(total == 0.f) {
				true_distribution[c.index()] = 0.f;
			} else {
				true_distribution[c.index()] /= total;
			}
		});

		// distribution of rgo land per good		
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto fc = fatten(state.world, c);
			assert(std::isfinite(true_distribution[c.index()]));
			state.world.province_get_rgo_max_size_per_good(fp, c) += real_size * true_distribution[c.index()];
		});
	});

	state.world.for_each_nation([&](dcon::nation_id n) {
		auto fn = fatten(state.world, n);
		fn.set_administrative_spending(int8_t(80));
		fn.set_military_spending(int8_t(60));
		fn.set_education_spending(int8_t(100));
		fn.set_social_spending(int8_t(100));
		fn.set_land_spending(int8_t(100));
		fn.set_naval_spending(int8_t(100));
		fn.set_construction_spending(int8_t(100));
		fn.set_overseas_spending(int8_t(100));

		fn.set_poor_tax(int8_t(75));
		fn.set_middle_tax(int8_t(75));
		fn.set_rich_tax(int8_t(75));

		fn.set_spending_level(1.0f);
	});

	state.world.for_each_market([&](dcon::market_id n) {
		initialize_needs_weights(state, n);
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			state.world.market_set_demand_satisfaction(n, c, 0.0f);
			state.world.market_set_direct_demand_satisfaction(n, c, 0.0f);
			// set domestic market pool
		});
	});

	update_rgo_employment(state);
	update_factory_employment(state);

	populate_army_consumption(state);
	populate_navy_consumption(state);
	populate_construction_consumption(state);

	state.world.for_each_nation([&](dcon::nation_id n) {
		state.world.nation_set_stockpiles(n, money, 1000.f);
	});
}

float sphere_leader_share_factor(sys::state& state, dcon::nation_id sphere_leader, dcon::nation_id sphere_member) {
	/*
	Share factor : If the nation is a civ and is a secondary power start with define : SECOND_RANK_BASE_SHARE_FACTOR, and
	otherwise start with define : CIV_BASE_SHARE_FACTOR.Also calculate the sphere owner's foreign investment in the nation as a
	fraction of the total foreign investment in the nation (I believe that this is treated as zero if there is no foreign
	investment at all). The share factor is (1 - base share factor) x sphere owner investment fraction + base share factor. For
	uncivs, the share factor is simply equal to define:UNCIV_BASE_SHARE_FACTOR (so 1, by default). If a nation isn't in a sphere,
	we let the share factor be 0 if it needs to be used in any other calculation.
	*/
	if(state.world.nation_get_is_civilized(sphere_member)) {
		float base = state.world.nation_get_rank(sphere_member) <= state.defines.colonial_rank
			? state.defines.second_rank_base_share_factor
			: state.defines.civ_base_share_factor;
		auto const ul = state.world.get_unilateral_relationship_by_unilateral_pair(sphere_member, sphere_leader);
		float sl_investment = state.world.unilateral_relationship_get_foreign_investment(ul);
		float total_investment = nations::get_foreign_investment(state, sphere_member);
		float investment_fraction = total_investment > 0.0001f ? sl_investment / total_investment : 0.0f;
		return base + (1.0f - base) * investment_fraction;
	} else {
		return state.defines.unciv_base_share_factor;
	}
}

float effective_tariff_rate(sys::state& state, dcon::nation_id n) {
	auto tariff_efficiency = std::max(0.0f, nations::tariff_efficiency(state, n));
	auto r = tariff_efficiency * float(state.world.nation_get_tariffs(n)) / 100.0f;
	return std::max(r, 0.0f);
}

void update_factory_triggered_modifiers(sys::state& state) {
	state.world.for_each_factory([&](dcon::factory_id f) {
		auto fac_type = fatten(state.world, state.world.factory_get_building_type(f));
		float sum = 1.0f;
		auto prov = state.world.factory_get_province_from_factory_location(f);
		auto pstate = state.world.province_get_state_membership(prov);
		auto powner = state.world.province_get_nation_from_province_ownership(prov);

		if(powner && pstate) {
			if(auto mod_a = fac_type.get_bonus_1_trigger();
					mod_a && trigger::evaluate(state, mod_a, trigger::to_generic(pstate), trigger::to_generic(powner), 0)) {
				sum -= fac_type.get_bonus_1_amount();
			}
			if(auto mod_b = fac_type.get_bonus_2_trigger();
					mod_b && trigger::evaluate(state, mod_b, trigger::to_generic(pstate), trigger::to_generic(powner), 0)) {
				sum -= fac_type.get_bonus_2_amount();
			}
			if(auto mod_c = fac_type.get_bonus_3_trigger();
					mod_c && trigger::evaluate(state, mod_c, trigger::to_generic(pstate), trigger::to_generic(powner), 0)) {
				sum -= fac_type.get_bonus_3_amount();
			}
		}

		state.world.factory_set_triggered_modifiers(f, sum);
	});
}

float subsistence_size(sys::state const& state, dcon::province_id p) {
	auto rgo_ownership = state.world.province_get_landowners_share(p) + state.world.province_get_capitalists_share(p);
	return state.world.province_get_rgo_size(p) * (1.f - rgo_ownership);
}

float rgo_effective_size(sys::state const& state, dcon::nation_id n, dcon::province_id p, dcon::commodity_id c) {
	bool is_mine = state.world.commodity_get_is_mine(c);

	float base = 0.f;
	auto rgo = state.world.province_get_rgo(p);
	if(rgo == c) {
		// set main rgo size to a fixed number for now: allow modders to replace it later per province basis...
		base = state.defines.alice_base_rgo_employment_bonus / state.defines.alice_rgo_per_size_employment;
	}

	// - We calculate its effective size which is its base size x (technology-bonus-to-specific-rgo-good-size +
	// technology-general-farm-or-mine-size-bonus + provincial-mine-or-farm-size-modifier + 1)
	auto rgo_ownership = state.world.province_get_landowners_share(p) + state.world.province_get_capitalists_share(p);
	auto sz = state.world.province_get_rgo_max_size_per_good(p, c) * rgo_ownership + base;
	auto pmod = state.world.province_get_modifier_values(p, is_mine ? sys::provincial_mod_offsets::mine_rgo_size : sys::provincial_mod_offsets::farm_rgo_size);
	auto nmod = state.world.nation_get_modifier_values(n, is_mine ? sys::national_mod_offsets::mine_rgo_size : sys::national_mod_offsets::farm_rgo_size);
	auto specific_pmod = state.world.nation_get_rgo_size(n, c);
	auto bonus = pmod + nmod + specific_pmod + 1.0f;

	return std::max(sz * bonus, 0.00f);
}

float rgo_total_effective_size(sys::state& state, dcon::nation_id n, dcon::province_id p) {
	float total = 0.f;
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		total += rgo_effective_size(state, n, p, c);
	});
	return total;
}

float subsistence_max_pseudoemployment(sys::state& state, dcon::nation_id n, dcon::province_id p) {
	return state.defines.alice_rgo_per_size_employment * subsistence_size(state, p) * 1.1f;
}

float rgo_total_employment(sys::state& state, dcon::nation_id n, dcon::province_id p) {
	float total = 0.f;
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		total += state.world.province_get_rgo_employment_per_good(p, c);
	});
	return total;
}

float rgo_max_employment(sys::state& state, dcon::nation_id n, dcon::province_id p, dcon::commodity_id c) {
	return state.defines.alice_rgo_per_size_employment * rgo_effective_size(state, n, p, c);
}

float rgo_total_max_employment(sys::state& state, dcon::nation_id n, dcon::province_id p) {
	float total = 0.f;
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		total += rgo_max_employment(state, n, p, c);
	});
	return total;
}

void update_local_subsistence_factor(sys::state& state) {
	state.world.execute_parallel_over_province([&](auto ids) {
		auto max_subsistence = ve::apply([&](dcon::province_id p) {
			return subsistence_max_pseudoemployment(state, state.world.province_get_nation_from_province_ownership(p), p);
		}, ids);

		auto employment = state.world.province_get_subsistence_employment(ids);
		auto saturation = employment / (4.f + max_subsistence);
		auto saturation_score = 1.f / (saturation + 1.f);

		auto quality = (ve::to_float(state.world.province_get_life_rating(ids)) - 10.f) / 10.f;
		quality = ve::max(quality, 0.f) + 0.01f;
		auto score = (subsistence_factor * quality) + subsistence_score_life;
		score = (score * saturation_score);
		state.world.province_set_subsistence_score(ids, score);
	});
}

float adjusted_subsistence_score(sys::state& state, dcon::province_id p) {
	return state.world.province_get_subsistence_score(p)
		* state.world.province_get_subsistence_employment(p)
		/ (state.world.province_get_demographics(p, demographics::total) + 1.f);
}

template<typename T>
ve::fp_vector ve_adjusted_subsistence_score(
	sys::state& state,
	T p
) {
	return state.world.province_get_subsistence_score(p)
		* state.world.province_get_subsistence_employment(p)
		/ (state.world.province_get_demographics(p, demographics::total) + 1.f);
}

void update_land_ownership(sys::state& state) {
	state.world.execute_parallel_over_province([&](auto ids) {
		auto local_states = state.world.province_get_state_membership(ids);
		auto weight_population =
			state.world.state_instance_get_demographics(local_states, demographics::to_key(state, state.culture_definitions.farmers))
			+ state.world.state_instance_get_demographics(local_states, demographics::to_key(state, state.culture_definitions.laborers));
		auto weight_capitalists = weight_population / 50.f
			+ state.world.state_instance_get_demographics(local_states, demographics::to_key(state, state.culture_definitions.capitalists)) * 200.f;
		auto weight_aristocracy = weight_population / 50.f
			+ state.world.state_instance_get_demographics(local_states, demographics::to_key(state, state.culture_definitions.aristocrat)) * 200.f
			+ state.world.state_instance_get_demographics(local_states, demographics::to_key(state, state.culture_definitions.slaves));
		auto total = weight_aristocracy + weight_capitalists + weight_population + 1.0f;
		state.world.province_set_landowners_share(ids, weight_aristocracy / total);
		state.world.province_set_capitalists_share(ids, weight_capitalists / total);
	});
}

int32_t factory_priority(sys::state const& state, dcon::factory_id f) {
	return (state.world.factory_get_priority_low(f) ? 1 : 0) + (state.world.factory_get_priority_high(f) ? 2 : 0);
}
void set_factory_priority(sys::state& state, dcon::factory_id f, int32_t priority) {
	state.world.factory_set_priority_high(f, priority >= 2);
	state.world.factory_set_priority_low(f, (priority & 1) != 0);
}
bool factory_is_profitable(sys::state const& state, dcon::factory_id f) {
	return state.world.factory_get_unprofitable(f) == false || state.world.factory_get_subsidized(f);
}

struct commodity_profit_holder {
	float profit = 0.0f;
	dcon::commodity_id c;
};

void update_rgo_employment(sys::state& state) {
	int32_t last = state.province_definitions.first_sea_province.index();

	concurrency::parallel_for(0, last, [&](int32_t for_index) {
		//province::for_each_land_province(state, [&](dcon::province_id p) {
		dcon::province_id p{ dcon::province_id::value_base_t(for_index) };

		auto owner = state.world.province_get_nation_from_province_ownership(p);
		auto s = state.world.province_get_state_membership(p);
		auto m = state.world.state_instance_get_market_from_local_market(s);
		auto current_employment = 0.f;
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			current_employment += state.world.province_get_rgo_employment_per_good(p, c);
		});
		current_employment += state.world.province_get_subsistence_employment(p);

		bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));
		float worker_pool = 0.0f;
		for(auto wt : state.culture_definitions.rgo_workers) {
			worker_pool += state.world.province_get_demographics(p, demographics::to_key(state, wt));
		}
		float slave_pool = state.world.province_get_demographics(p, demographics::to_key(state, state.culture_definitions.slaves));
		float labor_pool = worker_pool + slave_pool;

		float total_population = state.world.province_get_demographics(p, demographics::total);

		labor_pool = std::min(labor_pool, total_population);

		// update rgo employment per good:

		//sorting goods by profitability
		//static std::vector<dcon::commodity_id> ordered_rgo_goods;

		commodity_profit_holder ordered_list[126];
		assert(state.world.commodity_size() <= 126);

		//ordered_rgo_goods.clear();

		uint32_t used_indices = 0;
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(rgo_max_employment(state, owner, p, c) > 0.f) {
				ordered_list[used_indices].c = c;
				ordered_list[used_indices].profit = rgo_expected_worker_norm_profit(state, p, m, owner, c);
				++used_indices;
			} else {
				state.world.province_set_rgo_employment_per_good(p, c, 0.f);
			}
		});

		std::sort(ordered_list, ordered_list + used_indices, [&](commodity_profit_holder const& a, commodity_profit_holder const& b) {
			return (a.profit > b.profit);
		});

		// distributing workers in almost the same way as factories:
		float speed = 1.f;

		float total_workforce = labor_pool;
		float max_employment_total = 0.f;
		float total_employed = 0.f;

		for(uint32_t i = 0; i < used_indices; ++i) {
			auto c = ordered_list[i].c;
			float max_employment = rgo_max_employment(state, owner, p, c);
			max_employment_total += max_employment;
			float target_workforce = std::min(state.world.province_get_rgo_target_employment_per_good(p, c), total_workforce);

			float current_workforce = state.world.province_get_rgo_employment_per_good(p, c);
			float new_employment = std::min(current_workforce * (1 - speed) + target_workforce * speed, total_workforce);
			total_workforce -= new_employment;

			new_employment = std::clamp(new_employment, 0.f, max_employment);
			total_employed += new_employment;

			state.world.province_set_rgo_employment_per_good(p, c, new_employment);
		}

		float subsistence = std::min(subsistence_max_pseudoemployment(state, owner, p), total_workforce);
		total_workforce -= subsistence;
		total_employed += subsistence;

		state.world.province_set_subsistence_employment(p, subsistence);

		assert(total_employed <= total_population + 1.f);

		float employment_ratio = 0.f;
		if(max_employment_total > 1.f) {
			employment_ratio = total_employed / (max_employment_total + 1.f);
		} else {
			employment_ratio = 1.f;
		}
		state.world.province_set_rgo_employment(p, employment_ratio);

		auto slave_fraction = (slave_pool > current_employment) ? current_employment / slave_pool : 1.0f;
		auto free_fraction = std::max(0.0f, (worker_pool > current_employment - slave_pool) ? (current_employment - slave_pool) / std::max(worker_pool, 0.01f) : 1.0f);

		for(auto pop : state.world.province_get_pop_location(p)) {
			auto pt = pop.get_pop().get_poptype();
			if(pt == state.culture_definitions.slaves) {
				pop_demographics::set_raw_employment(state, pop.get_pop(), slave_fraction);
			} else if(pt.get_is_paid_rgo_worker()) {
				pop_demographics::set_raw_employment(state, pop.get_pop(), free_fraction);
			}
		}
	});
}

float factory_max_employment(sys::state const& state, dcon::factory_id f) {
	return state.defines.alice_factory_per_level_employment * state.world.factory_get_level(f);
}

float factory_primary_employment(sys::state const& state, dcon::factory_id f) {
	auto primary_employment = state.world.factory_get_primary_employment(f);
	return factory_max_employment(state, f) * (state.economy_definitions.craftsmen_fraction * primary_employment);
}
float factory_secondary_employment(sys::state const& state, dcon::factory_id f) {
	auto secondary_employment = state.world.factory_get_secondary_employment(f);
	return factory_max_employment(state, f) * ((1 - state.economy_definitions.craftsmen_fraction) * secondary_employment);
}
float factory_total_employment(sys::state const& state, dcon::factory_id f) {
	// TODO: Document this, also is this a stub?
	auto primary_employment = state.world.factory_get_primary_employment(f);
	auto secondary_employment = state.world.factory_get_secondary_employment(f);
	return factory_max_employment(state, f) * (state.economy_definitions.craftsmen_fraction * primary_employment + (1 - state.economy_definitions.craftsmen_fraction) * secondary_employment);
}

void update_factory_employment(sys::state& state) {
	state.world.for_each_state_instance([&](dcon::state_instance_id si) {
		float primary_pool = state.world.state_instance_get_demographics(si,
				demographics::to_key(state, state.culture_definitions.primary_factory_worker));
		float secondary_pool = state.world.state_instance_get_demographics(si,
				demographics::to_key(state, state.culture_definitions.secondary_factory_worker));

		static std::vector<dcon::factory_id> ordered_factories;
		ordered_factories.clear();

		province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
			for(auto fac : state.world.province_get_factory_location(p)) {
				ordered_factories.push_back(fac.get_factory());
			}
		});

		std::sort(ordered_factories.begin(), ordered_factories.end(), [&](dcon::factory_id a, dcon::factory_id b) {
			if(factory_is_profitable(state, a) != factory_is_profitable(state, b)) {
				return factory_is_profitable(state, a);
			}
			if(factory_priority(state, a) != factory_priority(state, b)) {
				return factory_priority(state, a) > factory_priority(state, b);
			}
			return a.index() < b.index();
		});

		float employment_shift_speed = 0.001f;

		float primary_pool_copy = primary_pool;
		float secondary_pool_copy = secondary_pool;
		for(uint32_t index = 0; index < ordered_factories.size();) {
			uint32_t next_index = index;

			float total_workforce = 0.0f;
			for(; next_index < ordered_factories.size(); ++next_index) {
				if(
					factory_is_profitable(state, ordered_factories[index])
					!=
					factory_is_profitable(state, ordered_factories[next_index])
					||
					factory_priority(state, ordered_factories[index])
					!=
					factory_priority(state, ordered_factories[next_index])
				) {
					break;
				}
				total_workforce += factory_max_employment(state, ordered_factories[next_index]) *
					state.world.factory_get_production_scale(ordered_factories[next_index]);
			}

			{
				float type_share = state.economy_definitions.craftsmen_fraction * total_workforce;
				float scale = primary_pool_copy >= type_share ? 1.0f : primary_pool_copy / type_share;
				primary_pool_copy = std::max(0.0f, primary_pool_copy - type_share);


				for(uint32_t i = index; i < next_index; ++i) {
					float old_employment = state.world.factory_get_primary_employment(ordered_factories[i]);
					float new_employment =
						old_employment * (1.f - employment_shift_speed)
						+ scale * state.world.factory_get_production_scale(ordered_factories[i]) * employment_shift_speed;

					state.world.factory_set_primary_employment(
						ordered_factories[i],
						new_employment
					);
				}
			}
			{
				float type_share = (1.0f - state.economy_definitions.craftsmen_fraction) * total_workforce;
				float scale = secondary_pool_copy >= type_share ? 1.0f : secondary_pool_copy / type_share;
				secondary_pool_copy = std::max(0.0f, secondary_pool_copy - type_share);

				for(uint32_t i = index; i < next_index; ++i) {

					float old_employment = state.world.factory_get_secondary_employment(ordered_factories[i]);
					float new_employment =
						old_employment * (1.f - employment_shift_speed)
						+ scale * state.world.factory_get_production_scale(ordered_factories[i]) * employment_shift_speed;

					state.world.factory_set_secondary_employment(
						ordered_factories[i],
						new_employment
					);
				}
			}

			index = next_index;
		}

		float prim_employment = 1.0f - (primary_pool > 0 ? primary_pool_copy / primary_pool : 0.0f);
		float sec_employment = 1.0f - (secondary_pool > 0 ? secondary_pool_copy / secondary_pool : 0.0f);

		province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
			for(auto pop : state.world.province_get_pop_location(p)) {
				if(pop.get_pop().get_poptype() == state.culture_definitions.primary_factory_worker) {
					pop_demographics::set_raw_employment(state, pop.get_pop(), prim_employment);
				} else if(pop.get_pop().get_poptype() == state.culture_definitions.secondary_factory_worker) {
					pop_demographics::set_raw_employment(state, pop.get_pop(), sec_employment);
				}
			}
		});
	});
}

/*
*
- Each factory has an input, output, and throughput multipliers.
- These are computed from the employees present. Input and output are 1 + employee effects, throughput starts at 0
- The input multiplier is also multiplied by (1 + sum-of-any-triggered-modifiers-for-the-factory) x
0v(national-mobilization-impact)
- Note: overseas is repurposed to administration of colonies
- Owner fraction is calculated from the fraction of owners in the state to total state population in the state (with some cap --
5%?)
- For each pop type employed, we calculate the ratio of number-of-pop-employed-of-a-type / (base-workforce x level) to the optimal
fraction defined for the production type (capping it at 1). That ratio x the-employee-effect-amount is then added into the
input/output/throughput modifier for the factory.
- Then, for input/output/throughput we sum up national and provincial modifiers to general factory input/output/throughput are
added, plus technology modifiers to its specific output commodity, add one to the sum, and then multiply the
input/output/throughput modifier from the workforce by it.

- The target input consumption scale is: input-multiplier x throughput-multiplier x factory level
- The actual consumption scale is limited by the input commodities sitting in the stockpile (i.e. input-consumption-scale x
input-quantity must be less than the amount in the stockpile)
- A similar process is done for efficiency inputs, except the consumption of efficiency inputs is
(national-factory-maintenance-modifier + 1) x input-multiplier x throughput-multiplier x factory level
- Finally, we get the efficiency-adjusted consumption scale by multiplying the base consumption scale by (0.75 + 0.25 x the
efficiency consumption scale)

*/

float factory_full_production_quantity(sys::state const& state, dcon::factory_id f, dcon::nation_id n,
		float mobilization_impact) {
	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();

	/*
	float total_state_pop = std::max(0.01f, state.world.state_instance_get_demographics(s, demographics::total));
	float owner_fraction = std::min(0.05f, state.world.state_instance_get_demographics(s, demographics::to_key(state,
	state.culture_definitions.capitalists)) / total_state_pop); float input_multiplier = fac.get_triggered_modifiers() * (1.0f +
	owner_fraction * -2.5f);
	*/

	float throughput_multiplier = (state.world.nation_get_factory_goods_throughput(n, fac_type.get_output()) + 1.0f);
	float output_multiplier = state.world.nation_get_factory_goods_output(n, fac_type.get_output())
		+ 1.0f
		+ fac.get_secondary_employment()
		* (1.0f - state.economy_definitions.craftsmen_fraction)
		* 1.5f
		* 2.f; // additional multiplier to give advantage to "old industrial giants" which have a bunch of clerks already

	float max_production_scale = fac.get_primary_employment()
		* fac.get_level()
		* std::max(0.0f, mobilization_impact);

	return throughput_multiplier * output_multiplier * max_production_scale;
}

float rgo_efficiency(sys::state& state, dcon::nation_id n, dcon::province_id p, dcon::commodity_id c) {
	bool is_mine = state.world.commodity_get_is_mine(c);

	float main_rgo = 1.f;
	auto rgo = state.world.province_get_rgo(p);
	if(rgo == c) {
		main_rgo = state.defines.alice_base_rgo_efficiency_bonus;
	}

	float base_amount = state.world.commodity_get_rgo_amount(c);
	float throughput =
		1.0f
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_throughput)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_throughput)
		+ state.world.province_get_modifier_values(p,
			is_mine ?
			sys::provincial_mod_offsets::mine_rgo_eff
			:
			sys::provincial_mod_offsets::farm_rgo_eff)
		+ state.world.nation_get_modifier_values(n,
			is_mine ?
			sys::national_mod_offsets::mine_rgo_eff
			:
			sys::national_mod_offsets::farm_rgo_eff);

	float saturation = std::clamp(state.world.province_get_rgo_employment_per_good(p, c)
		/ (rgo_max_employment(state, n, p, c) + 1.f), 0.0f, 1.0f);

	float result = base_amount
		* main_rgo
		* (1.f + 1.0f * (1.f - saturation))
		* std::max(0.5f, throughput)
		* state.defines.alice_rgo_boost
		* std::max(0.5f, (1.0f + state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_output) +
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_output) +
			state.world.nation_get_rgo_goods_output(n, c)));

	assert(result >= 0.0f && std::isfinite(result));
	return result;
}

float rgo_full_production_quantity(sys::state& state, dcon::nation_id n, dcon::province_id p, dcon::commodity_id c) {
	/*
	- We calculate its effective size which is its base size x (technology-bonus-to-specific-rgo-good-size +
	technology-general-farm-or-mine-size-bonus + provincial-mine-or-farm-size-modifier + 1)
	- We add its production to domestic supply, calculating that amount basically in the same way we do for factories, by
	computing RGO-throughput x RGO-output x RGO-size x base-commodity-production-quantity, except that it is affected by different
	modifiers.
	*/
	auto eff_size = rgo_effective_size(state, n, p, c);
	auto val = eff_size * rgo_efficiency(state, n, p, c);
	assert(val >= 0.0f && std::isfinite(val));
	return val;
}

float factory_min_input_available(
	sys::state& state,
	dcon::market_id m,
	dcon::factory_type_fat_id fac_type
) {
	float min_input_available = 1.0f;
	auto& inputs = fac_type.get_inputs();
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			min_input_available =
				std::min(
					min_input_available,
					state.world.market_get_demand_satisfaction(m, inputs.commodity_type[i])
				);
		} else {
			break;
		}
	}
	return min_input_available;
}

float factory_input_total_cost(sys::state& state, dcon::market_id m, dcon::factory_type_fat_id fac_type) {
	float input_total = 0.0f;
	auto& inputs = fac_type.get_inputs();
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_total += inputs.commodity_amounts[i] * price(state, m, inputs.commodity_type[i]);
		} else {
			break;
		}
	}
	return input_total;
}

float factory_min_e_input_available(sys::state& state, dcon::market_id m, dcon::factory_type_fat_id fac_type) {
	float min_e_input_available = 1.0f;
	auto& e_inputs = fac_type.get_efficiency_inputs();
	for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
		if(e_inputs.commodity_type[i]) {
			min_e_input_available =
				std::min(
					min_e_input_available,
					state.world.market_get_demand_satisfaction(m, e_inputs.commodity_type[i])
				);
		} else {
			break;
		}
	}

	return min_e_input_available;
}

float factory_e_input_total_cost(sys::state& state, dcon::market_id m, dcon::factory_type_fat_id fac_type) {
	float e_input_total = 0.0f;
	auto& e_inputs = fac_type.get_efficiency_inputs();
	for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
		if(e_inputs.commodity_type[i]) {
			e_input_total += e_inputs.commodity_amounts[i] * price(state, m, e_inputs.commodity_type[i]);
		} else {
			break;
		}
	}

	return e_input_total;
}

float priority_multiplier(sys::state& state, dcon::factory_type_id fac_type, dcon::nation_id n) {
	auto exp = state.world.nation_get_factory_type_experience(n, fac_type);
	return 100000.f / (100000.f + exp);
}

float nation_factory_input_multiplier(sys::state& state, dcon::factory_type_id fac_type, dcon::nation_id n) {
	auto mult = priority_multiplier(state, fac_type, n);

	return mult * std::max(
		0.1f,
		state.defines.alice_inputs_base_factor
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_input)
	);
}
float nation_factory_output_multiplier(sys::state& state, dcon::factory_type_id fac_type, dcon::nation_id n) {
	auto output = state.world.factory_type_get_output(fac_type);
	return state.world.nation_get_factory_goods_output(n, output)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_output)
		+ 1.0f;
}

float factory_input_multiplier(sys::state& state, dcon::factory_fat_id fac, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s) {
	float total_workers = factory_max_employment(state, fac);
	float small_size_effect = 1.f;
	float small_bound = state.defines.alice_factory_per_level_employment * 5.f;
	if(total_workers < small_bound) {
		small_size_effect = 0.5f + total_workers / small_bound * 0.5f;
	}

	float total_state_pop = std::max(0.01f, state.world.state_instance_get_demographics(s, demographics::total));
	float capitalists = state.world.state_instance_get_demographics(s, demographics::to_key(state, state.culture_definitions.capitalists));
	float owner_fraction = total_state_pop > 0
		? std::min(
			0.05f,
			capitalists / total_state_pop)
		: 0.0f;

	auto mult = priority_multiplier(state, fac.get_building_type(), n);

	return small_size_effect *
		fac.get_triggered_modifiers() *
		std::max(
			0.1f,
			mult
			* (
				state.defines.alice_inputs_base_factor
				+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_input)
				+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_input)
				+ owner_fraction * -2.5f
			)
		);
}

float factory_throughput_multiplier(sys::state& state, dcon::factory_type_fat_id fac_type, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s) {
	auto national_t = state.world.nation_get_factory_goods_throughput(n, fac_type.get_output());
	auto provincial_fac_t = state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_throughput);
	auto nationnal_fac_t = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_throughput);

	auto result = 1.f
		* std::max(0.f, 1.f + national_t)
		* std::max(0.f, 1.f + provincial_fac_t)
		* std::max(0.f, 1.f + nationnal_fac_t);

	return production_throughput_multiplier * result;
}

float factory_output_multiplier(sys::state& state, dcon::factory_fat_id fac, dcon::nation_id n, dcon::province_id p) {
	auto fac_type = fac.get_building_type();

	return std::max(0.f, state.world.nation_get_factory_goods_output(n, fac_type.get_output())
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_output)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_output)
		+ fac.get_secondary_employment()
			* (1.0f - state.economy_definitions.craftsmen_fraction)
			* 1.5f
		+ 1.0f);
}

float factory_max_production_scale_non_modified(sys::state& state, dcon::factory_fat_id fac) {
	return fac.get_primary_employment()
		* fac.get_level();
}

float factory_max_production_scale(sys::state& state, dcon::factory_fat_id fac, float mobilization_impact, bool occupied) {
	return fac.get_primary_employment()
		* fac.get_level()
		* (occupied ? 0.1f : 1.0f)
		* std::max(0.0f, mobilization_impact);
}

float update_factory_scale(
	sys::state& state,
	dcon::factory_fat_id fac,
	dcon::market_id m,
	float max_production_scale,
	float raw_profit,
	float desired_raw_profit
) {
	float total_workers = factory_max_employment(state, fac);
	float several_workers_scale = 10.f / total_workers;

	// we don't want for factories to change "world balance" too much individually
	// when relative production is high, we want to reduce our speed
	// for example, if relative production is 1.0, then we want to clamp our speed with ~0.01 or something small like this;
	// and if relative production is ~0, then clamps are not needed
	float relative_production_amount
		=
		state.world.factory_type_get_output_amount(fac.get_building_type())
		/ (
			state.world.market_get_supply(m, fac.get_building_type().get_output())
			+ state.world.market_get_demand(m, fac.get_building_type().get_output())
			+ 10.f
		);

	float relative_modifier = (1 / (relative_production_amount + 0.01f)) / 1000.f;

	float effective_production_scale = 0.0f;
	if(state.world.factory_get_subsidized(fac)) {
		auto new_production_scale = std::min(1.0f, fac.get_production_scale() + several_workers_scale * fac.get_level() * 10.f);
		fac.set_production_scale(new_production_scale);
		return std::min(new_production_scale * fac.get_level(), max_production_scale);
	}

	float over_profit_ratio = (raw_profit) / (desired_raw_profit + 0.0001f) - 1.f;
	float under_profit_ratio = (desired_raw_profit) / (raw_profit + 0.0001f) - 1.f;

	float speed_modifier = (over_profit_ratio - under_profit_ratio);
	float speed = production_scale_delta * speed_modifier + several_workers_scale * ((raw_profit - desired_raw_profit > 0.f) ? 1.f : -1.f);

	speed = std::clamp(speed, -relative_modifier, relative_modifier);

	auto new_production_scale = std::clamp(fac.get_production_scale() + speed, 0.f, 1.f);
	fac.set_production_scale(new_production_scale);
	return std::min(new_production_scale * fac.get_level(), max_production_scale);
}

float factory_desired_raw_profit(dcon::factory_fat_id fac, float spendings) {
	return spendings * 1.01f;
}

void update_single_factory_consumption(
	sys::state& state,
	dcon::factory_id f,
	dcon::province_id p,
	dcon::state_instance_id s,
	dcon::market_id m,
	dcon::nation_id n,
	float mobilization_impact,
	float expected_min_wage,
	bool occupied
) {
	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();

	assert(fac_type);
	assert(fac_type.get_output());
	assert(n);
	assert(p);
	assert(s);

	float total_workers = factory_max_employment(state, f);
	float several_workers_scale = 10.f / total_workers;
	float max_production_scale = factory_max_production_scale(
		state,
		fac,
		mobilization_impact,
		occupied
	);

	float current_workers = total_workers * max_production_scale;

	//inputs

	float input_total = factory_input_total_cost(state, m, fac_type);
	float min_input_available = factory_min_input_available(state, m, fac_type);
	float e_input_total = factory_e_input_total_cost(state, m, fac_type);
	float min_e_input_available = factory_min_e_input_available(state, m, fac_type);

	//modifiers

	float input_multiplier = factory_input_multiplier(state, fac, n, p, s);
	auto const mfactor = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
	float throughput_multiplier = factory_throughput_multiplier(state, fac_type, n, p, s);
	float output_multiplier = factory_output_multiplier(state, fac, n, p);

	float bonus_profit_thanks_to_max_e_input = fac_type.get_output_amount()
		* 0.25f
		* throughput_multiplier
		* output_multiplier
		* min_input_available
		* price(state, m, fac_type.get_output());

	// if efficiency inputs are not worth it, then do not buy them
	if(bonus_profit_thanks_to_max_e_input < e_input_total * mfactor * input_multiplier)
		min_e_input_available = 0.f;

	//this value represents total production if 1 lvl of this factory is filled with workers
	float total_production = fac_type.get_output_amount()
		* (0.75f + 0.25f * min_e_input_available)
		* throughput_multiplier
		* output_multiplier
		* min_input_available;

	assert(min_input_available >= 0.f);
	assert(output_multiplier >= 0.f);
	assert(throughput_multiplier >= 0.f);
	assert(min_e_input_available >= 0.f);

	//this value represents raw profit if 1 lvl of this factory is filled with workers
	float profit =
		total_production
		* price(state, m, fac_type.get_output());

	float base_profit =
		profit;

	float base_spendings =
		input_multiplier
		* input_total
		* min_input_available
		+
		input_multiplier
		* mfactor
		* e_input_total
		* min_e_input_available;

	assert(base_profit >= 0.f);
	assert(base_spendings >= 0.f);

	float base_scale = std::min(1.f, (base_profit + 1.f) / (base_spendings + 1.f));

	//this value represents spendings if 1 lvl of this factory is filled with workers
	float spendings =
		expected_min_wage
		* (state.defines.alice_factory_per_level_employment / state.defines.alice_needs_scaling_factor)
		+
		input_multiplier
		* throughput_multiplier
		* input_total
		* min_input_available
		+
		input_multiplier * mfactor
		* e_input_total
		* min_e_input_available
		* min_input_available;

	float desired_profit = factory_desired_raw_profit(fac, spendings);
	float max_pure_profit = profit - spendings;
	state.world.factory_set_unprofitable(f, !(max_pure_profit > 0.0f));

	float effective_production_scale = update_factory_scale(
		state,
		fac,
		m,
		max_production_scale,
		profit,
		desired_profit
	);

	auto& inputs = fac_type.get_inputs();
	auto& e_inputs = fac_type.get_efficiency_inputs();

	// register real demand :
	// input_multiplier * throughput_multiplier * level * primary_employment
	// also multiply by target production scale...
	// otherwise too much excess demand is generated
	// also multiply by something related to minimal satisfied input
	// to prevent generation of too much demand on rgos already
	// influenced by a shortage
	// to make this modifier even more sane
	// we check our potential profit
	// and check how much of this input
	// we could potentially buy with our income

	auto min_input_importance = std::min(1.f, 1000.f / (input_multiplier * input_total + 0.00001f));

	auto min_input_coefficient = (min_input_importance + (1.f - min_input_importance) * min_input_available);

	auto min_e_input_importance = std::min(
		1.f, 1.f / (input_multiplier * mfactor * e_input_total + 0.00001f)
	);
	auto min_e_input_coefficient = (min_e_input_importance + (1.f - min_e_input_importance) * min_e_input_available);

	assert(input_multiplier >= 0.f);
	assert(throughput_multiplier >= 0.f);
	assert(effective_production_scale >= 0.f);
	assert(min_input_coefficient >= 0.f);
	assert(base_scale >= 0.f);

	float input_scale =
		input_multiplier
		* throughput_multiplier
		* effective_production_scale
		* min_input_coefficient
		* base_scale;

	assert(input_scale >= 0.f);

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			register_intermediate_demand(state,
				m,
				inputs.commodity_type[i],
				input_scale
				* inputs.commodity_amounts[i]
				, economy_reason::factory
			);
		} else {
			break;
		}
	}

	// and for efficiency inputs
	//  the consumption of efficiency inputs is (national-factory-maintenance-modifier + 1) x input-multiplier x
	//  throughput-multiplier x factory level
	for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
		if(e_inputs.commodity_type[i]) {
			register_intermediate_demand(
				state,
				m,
				e_inputs.commodity_type[i],
				mfactor
				* input_scale
				* e_inputs.commodity_amounts[i]
				* min_e_input_coefficient
				, economy_reason::factory
			);
		} else {
			break;
		}
	}

	float actual_production = total_production * effective_production_scale;
	float pure_profit = max_pure_profit * effective_production_scale;

	state.world.factory_set_actual_production(f, actual_production);
	state.world.factory_set_full_profit(f, pure_profit);
}

void update_single_factory_production(
	sys::state& state,
	dcon::factory_id f,
	dcon::market_id m,
	dcon::nation_id n,
	float expected_min_wage
) {
	auto production = state.world.factory_get_actual_production(f);
	if(production > 0) {
		auto fac = fatten(state.world, f);
		auto fac_type = fac.get_building_type();
		auto amount = production;
		auto money_made = state.world.factory_get_full_profit(f);

		state.world.factory_set_actual_production(f, amount);
		register_domestic_supply(state, m, fac_type.get_output(), amount, economy_reason::factory);

		if(!fac.get_subsidized()) {
			state.world.factory_set_full_profit(f, money_made);
		} else {
			float min_wages =
				expected_min_wage
				* fac.get_level()
				* fac.get_primary_employment()
				* (state.defines.alice_factory_per_level_employment / state.defines.alice_needs_scaling_factor);
			if(money_made < min_wages) {
				auto diff = min_wages - money_made;
				if(state.world.nation_get_stockpiles(n, money) > diff || can_take_loans(state, n)) {
					state.world.factory_set_full_profit(f, min_wages);
					state.world.nation_get_stockpiles(n, money) -= diff;
					state.world.nation_get_subsidies_spending(n) += diff;
				} else {
					state.world.factory_set_full_profit(f, std::max(money_made, 0.0f));
					fac.set_subsidized(false);
				}
			} else {
				state.world.factory_set_full_profit(f, money_made);
			}
		}
	} else {
	}
}

rgo_workers_breakdown rgo_relevant_population(sys::state& state, dcon::province_id p, dcon::nation_id n) {
	auto relevant_paid_population = 0.f;
	for(auto wt : state.culture_definitions.rgo_workers) {
		relevant_paid_population += state.world.province_get_demographics(p, demographics::to_key(state, wt));
	}
	auto slaves = state.world.province_get_demographics(p, demographics::to_employment_key(state, state.culture_definitions.slaves));

	rgo_workers_breakdown result = {
		.paid_workers = relevant_paid_population,
		.slaves = slaves,
		.total = relevant_paid_population + slaves
	};

	return result;
}

float rgo_desired_worker_norm_profit(
	sys::state& state,
	dcon::province_id p,
	dcon::market_id m,
	dcon::nation_id n,
	float min_wage, float total_relevant_population
) {
	auto current_employment = rgo_total_employment(state, n, p); // maximal amount of workers which rgo could potentially employ

	//we assume a "perfect ratio" of 1 aristo per N pops
	float perfect_aristos_amount = total_relevant_population / 1000.f;
	float perfect_aristos_amount_adjusted = perfect_aristos_amount / state.defines.alice_needs_scaling_factor;
	float aristos_desired_cut = aristocrats_greed * perfect_aristos_amount_adjusted * (
		state.world.market_get_everyday_needs_costs(m, state.culture_definitions.aristocrat)
		+ state.world.market_get_life_needs_costs(m, state.culture_definitions.aristocrat)
		//+ state.world.market_get_luxury_needs_costs(m, state.culture_definitions.aristocrat)
	);
	float aristo_burden_per_worker = aristos_desired_cut / (total_relevant_population + 1);

	float subsistence = adjusted_subsistence_score(state, p);
	if(subsistence == 0) subsistence = state.world.province_get_subsistence_score(p);
	float subsistence_life = std::clamp(subsistence, 0.f, subsistence_score_life);
	subsistence -= subsistence_life;
	float subsistence_everyday = std::clamp(subsistence, 0.f, subsistence_score_everyday);
	subsistence -= subsistence_everyday;

	subsistence_life = subsistence_life / subsistence_score_life;
	subsistence_everyday = subsistence_everyday / subsistence_score_everyday;

	bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));

	dcon::pop_type_id pop_type = is_mine ? state.culture_definitions.laborers : state.culture_definitions.farmers;

	auto ln_costs = state.world.market_get_life_needs_costs(m, pop_type);
	auto en_costs = state.world.market_get_everyday_needs_costs(m, pop_type);
	auto lx_costs = state.world.market_get_luxury_needs_costs(m, pop_type);

	float subsistence_based_min_wage =
		subsistence_life * ln_costs
		+ subsistence_everyday * en_costs;

	// 1000.f to prevent deflation spirals due to
	// high lower bound on prices
	// example:
	// timber reaches price of MIN_PRICE
	// but it produces 100000000000 units per worker
	// in result expected profit is
	// higher than desired profit even if price is already at the bottom

	float min_wage_burden_per_worker =
		(1000.f + min_wage + subsistence_based_min_wage) / state.defines.alice_needs_scaling_factor;

	float desired_profit_by_worker = aristo_burden_per_worker + min_wage_burden_per_worker;

	// we want to employ at least someone, so we decrease our desired profits when employment is low
	// otherwise everyone works in subsistence and landowners get no money
	// not exactly an ideal solution but it works and doesn't create goods or wealth out of thin air
	//float employment_ratio = current_employment / (total_relevant_population + 1.f);
	// desired_profit_by_worker = desired_profit_by_worker; // * employment_ratio; //* employment_ratio;

	assert(std::isfinite(desired_profit_by_worker));


	return desired_profit_by_worker;
}

float rgo_expected_worker_norm_profit(
	sys::state& state,
	dcon::province_id p,
	dcon::market_id m,
	dcon::nation_id n,
	dcon::commodity_id c
) {
	auto efficiency = rgo_efficiency(state, n, p, c);
	auto current_price = price(state, m, c);

	return
		efficiency
		* current_price
		/ state.defines.alice_rgo_per_size_employment;
}

float convex_function(float x) {
	return 1.f - (1.f - x) * (1.f - x);
}


void update_province_rgo_employment(
	sys::state& state,
	dcon::province_id p,
	dcon::market_id m,
	dcon::nation_id n,
	float mobilization_impact,
	float expected_min_wage
) {
	auto rgo_pops = rgo_relevant_population(state, p, n);
	float desired_profit = rgo_desired_worker_norm_profit(state, p, m, n, expected_min_wage, rgo_pops.total);

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto max_production = rgo_full_production_quantity(state, n, p, c);
		if(max_production < 0.001f) {
			return;
		}

		// maximal amount of workers which rgo could potentially employ
		auto pops_max = rgo_max_employment(state, n, p, c);
		auto current_employment = state.world.province_get_rgo_employment_per_good(p, c);
		float expected_profit = rgo_expected_worker_norm_profit(state, p, m, n, c);

		auto efficiency_per_hire = rgo_efficiency(state, n, p, c) / state.defines.alice_rgo_per_size_employment;

		auto desired_price = desired_profit / efficiency_per_hire;

		auto current_price = price(state, m, c) - desired_price;

		auto supply = state.world.market_get_supply(m, c)
			+ state.world.market_get_stockpile(m, c) * stockpile_to_supply + price_rigging;
		auto demand = state.world.market_get_demand(m, c) + price_rigging;

		auto price_acceleration_from_additional_supply =
			price_speed_mod * (demand / supply / supply + 1 / demand);
		auto price_speed_change = price_speed_mod * (demand / supply - supply / demand);

		auto weight_short_term = short_term_profits_weight_n;
		auto weight_mid_term = mid_term_profits_weight_n;
		auto weight_long_term = long_term_profits_weight_n;
		if(current_price >= 0.f) {
			weight_short_term = short_term_profits_weight_p;
			weight_mid_term = mid_term_profits_weight_p;
			weight_long_term = long_term_profits_weight_p;
		}

		auto hire_rate_from_income =
			current_price
			* weight_short_term;

		auto hire_rate_from_predicted_acceleration_of_price =
			-price_acceleration_from_additional_supply * supply * weight_mid_term;

		auto hire_rate_from_predicted_change_of_price =
			price_speed_change * weight_mid_term;

		auto money_to_workers_divisor =
			2
			* weight_long_term
			* efficiency_per_hire
			* price_acceleration_from_additional_supply;

		auto optimal_hire_change =
			(
				hire_rate_from_income
				+ hire_rate_from_predicted_acceleration_of_price //overestimate to account for stockpiles
				+ hire_rate_from_predicted_change_of_price
			) / money_to_workers_divisor;

		auto change = optimal_hire_change;

		//change = std::clamp(change, -relative_modifier, relative_modifier);
		assert(std::isfinite(current_employment + change));
		auto new_employment = std::clamp(current_employment + change, 0.0f, pops_max);
		state.world.province_set_rgo_target_employment_per_good(p, c, new_employment);

		// rgos produce all the way down
		float employment_ratio = current_employment / pops_max;
		assert(max_production * employment_ratio >= 0);
		state.world.province_set_rgo_actual_production_per_good(p, c, max_production * employment_ratio);
	});
}

void update_province_rgo_production(
	sys::state& state,
	dcon::province_id p,
	dcon::market_id m,
	dcon::nation_id n
) {
	state.world.province_set_rgo_full_profit(p, 0.f);
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto amount = state.world.province_get_rgo_actual_production_per_good(p, c);

		register_domestic_supply(state, m, c, amount, economy_reason::rgo);

		float profit = amount * state.world.market_get_price(m, c);

		assert(profit >= 0);

		state.world.province_set_rgo_profit_per_good(p, c, profit);
		state.world.province_get_rgo_full_profit(p) += profit;

		if(state.world.commodity_get_money_rgo(c)) {
			assert(
				std::isfinite(
					amount
					* state.defines.gold_to_cash_rate
					* state.world.commodity_get_cost(c)
				)
				&& amount * state.defines.gold_to_cash_rate >= 0.0f
			);
			state.world.nation_get_stockpiles(n, money) +=
				amount
				* state.defines.gold_to_cash_rate
				* state.world.commodity_get_cost(c);
		}
	});
}

template<typename T, typename S, typename U>
void update_artisan_consumption(
	sys::state& state,
	T markets,
	S nations,
	U states,
	ve::fp_vector expected_min_wage,
	ve::fp_vector mobilization_impact
) {
	auto const csize = state.world.commodity_size();
	auto num_artisans = state.world.state_instance_get_demographics(
		states, demographics::to_key(state, state.culture_definitions.artisans));
	ve::fp_vector total_profit = 0.0f;

	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.market_set_artisan_actual_production(markets, cid, 0.0f);

		auto valid_mask = ve_valid_artisan_good(state, nations, cid);

		ve::fp_vector input_total = 0.0f;
		auto const& inputs = state.world.commodity_get_artisan_inputs(cid);
		ve::fp_vector min_available = 1.0f;
		for(uint32_t j = 0; j < commodity_set::set_size; ++j) {
			if(inputs.commodity_type[j]) {
				input_total =
					input_total
					+ inputs.commodity_amounts[j]
					* ve_price(state, markets, inputs.commodity_type[j]);
				min_available = ve::min(
					min_available,
					state.world.market_get_demand_satisfaction(markets, inputs.commodity_type[j]));
			} else {
				break;
			}
		}

		ve::fp_vector output_total =
			state.world.commodity_get_artisan_output_amount(cid)
			* ve_price(state, markets, cid);

		ve::fp_vector input_multiplier = artisan_input_multiplier(state, nations);
		ve::fp_vector throughput_multiplier = artisan_throughput_multiplier(state, nations);
		ve::fp_vector output_multiplier = artisan_output_multiplier(state, nations);

		ve::fp_vector distribution = state.world.market_get_artisan_score(markets, cid);

		ve::fp_vector max_production_scale =
			num_artisans
			* distribution / 10'000.0f
			* mobilization_impact;

		auto min_input_importance = ve::min(1.f, 1000.f / (input_total + 0.00001f));
		auto min_input_coefficient = min_available + (min_input_importance * (1.f - min_available));

		auto demand_mod = ve::select(
			valid_mask,
			input_multiplier
			* throughput_multiplier
			* max_production_scale
			* min_input_coefficient,
			0.f
		);

		auto supply_amount = ve::select(
			valid_mask,
			state.world.commodity_get_artisan_output_amount(cid)
			* throughput_multiplier
			* output_multiplier
			* max_production_scale
			* min_available,
			0.f
		);

		auto profitability_factor =
			(
				output_total * output_multiplier * throughput_multiplier * min_available
				- input_multiplier * input_total * throughput_multiplier * min_available
			) / (0.5f * expected_min_wage * (10'000.0f / state.defines.alice_needs_scaling_factor));

		for(uint32_t j = 0; j < commodity_set::set_size; ++j) {
			if(inputs.commodity_type[j]) {
				register_intermediate_demand(
					state,
					markets,
					inputs.commodity_type[j],
					demand_mod * inputs.commodity_amounts[j],
					economy_reason::artisan
				);
			} else {
				break;
			}
		}

		state.world.market_set_artisan_actual_production(
			markets,
			cid,
			supply_amount
		);

		auto base_profit =
			output_total * output_multiplier
			- input_multiplier * input_total;

		total_profit = total_profit
			+ ve::max(0.0f,
				base_profit
				* throughput_multiplier
				* max_production_scale
				* min_available
			);
	}

	state.world.market_set_artisan_profit(markets, total_profit);
}

template<typename T>
void update_market_artisan_production(sys::state& state, T markets) {
	auto const csize = state.world.commodity_size();
	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto production = state.world.market_get_artisan_actual_production(markets, cid);
		register_domestic_supply(state, markets, cid, production, economy_reason::artisan);
	}
}

void populate_army_consumption(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_army_demand(ids, cid, 0.0f);
		});
	}

	state.world.for_each_regiment([&](dcon::regiment_id r) {
		auto reg = fatten(state.world, r);
		auto type = state.world.regiment_get_type(r);
		auto owner = reg.get_army_from_army_membership().get_controller_from_army_control();
		auto pop = reg.get_pop_from_regiment_source();
		auto location = pop.get_pop_location().get_province().get_state_membership();
		auto market = location.get_market_from_local_market();
		auto scale = owner.get_spending_level();
		auto strength = reg.get_strength();

		if(owner && type) {
			auto o_sc_mod = std::max(
				0.01f,
				state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption)
				+ 1.0f
			);
			auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(supply_cost.commodity_type[i]) {
					// Day-to-day consumption
					// Strength under 100% reduces unit supply consumption
					state.world.market_get_army_demand(market, supply_cost.commodity_type[i]) +=
						supply_cost.commodity_amounts[i]
						* state.world.nation_get_unit_stats(owner, type).supply_consumption
						* o_sc_mod * strength;
				} else {
					break;
				}
			}
			auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto reinforcement = military::unit_calculate_reinforcement(state, reg);
					if(reinforcement > 0) {
						// Regiment needs reinforcement - add extra consumption. Every 1% of reinforcement demands 1% of unit cost
						state.world.market_get_army_demand(market, build_cost.commodity_type[i]) +=
							build_cost.commodity_amounts[i]
							* reinforcement;
					}
				} else {
					break;
				}
		}
		}
	});
}

void populate_navy_consumption(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_navy_demand(ids, cid, 0.0f);
		});
	}

	state.world.for_each_ship([&](dcon::ship_id r) {
		auto shp = fatten(state.world, r);
		auto type = state.world.ship_get_type(r);
		auto owner = shp.get_navy_from_navy_membership().get_controller_from_navy_control();
		auto market = owner.get_capital().get_state_membership().get_market_from_local_market();

		if(owner && type) {
			auto o_sc_mod = std::max(
				0.01f,
				state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption)
				+ 1.0f
			);

			auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(supply_cost.commodity_type[i]) {
					state.world.market_get_navy_demand(market, supply_cost.commodity_type[i]) +=
						supply_cost.commodity_amounts[i]
						* state.world.nation_get_unit_stats(owner, type).supply_consumption
						* o_sc_mod;
				} else {
					break;
				}
			}

			auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto reinforcement = military::unit_calculate_reinforcement(state, shp);
					if(reinforcement > 0) {
						// Ship needs repair - add extra consumption. Every 1% of reinforcement demands 1% of unit cost
						state.world.market_get_army_demand(market, build_cost.commodity_type[i]) +=
							build_cost.commodity_amounts[i]
							* reinforcement;
					}
				} else {
					break;
				}
			}
		}
	});
}


// we want "cheaper per day"(= slower) construction at the start to avoid initial demand bomb
// and "more expensive"(=faster) construction at late game
inline constexpr float day_1_build_time_modifier_non_factory = 2.f;
inline constexpr float day_inf_build_time_modifier_non_factory = 0.5f;
inline constexpr float day_1_derivative_non_factory = -0.2f;

inline constexpr float diff_non_factory = day_1_build_time_modifier_non_factory - day_inf_build_time_modifier_non_factory;
inline constexpr float shift_non_factory = -diff_non_factory / day_1_derivative_non_factory;
inline constexpr float slope_non_factory = diff_non_factory * shift_non_factory;


float global_non_factory_construction_time_modifier(sys::state& state) {
	float t = math::sqrt((static_cast<float>(state.current_date.value) * 0.01f + 2.f));
	return day_inf_build_time_modifier_non_factory + slope_non_factory / (t + shift_non_factory);
}

inline constexpr float day_1_build_time_modifier_factory = 0.9f;
inline constexpr float day_inf_build_time_modifier_factory = 0.75f;
inline constexpr float day_1_derivative_factory = -0.01f;

inline constexpr float diff_factory = day_1_build_time_modifier_factory - day_inf_build_time_modifier_factory;
inline constexpr float shift_factory = -diff_factory / day_1_derivative_factory;
inline constexpr float slope_factory = diff_factory * shift_factory;

// also we want to speed up factories construction right at the start
// as it's the most vulnerable time for them
// and we need to establish *some* industrial base for world to develop
// their build time should also become faster with time to delay growth bottleneck
float global_factory_construction_time_modifier(sys::state& state) {
	float t = math::sqrt((static_cast<float>(state.current_date.value) * 0.01f + 2.f));
	return day_inf_build_time_modifier_factory + slope_factory / (t + shift_factory);
}

void populate_construction_consumption(sys::state& state) {

	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_construction_demand(ids, cid, 0.0f);
		});
	}

	static auto current_budget = state.world.nation_make_vectorizable_float_buffer();
	state.world.execute_serial_over_nation([&](auto ids) {
		auto base_budget = state.world.nation_get_stockpiles(ids, economy::money);
		auto construction_priority = ve::to_float(state.world.nation_get_construction_spending(ids)) / 100.f;
		current_budget.set(ids, ve::max(0.f, base_budget * construction_priority));
	});

	for(auto lc : state.world.in_province_land_construction) {
		auto province = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(lc));
		auto owner = state.world.province_get_nation_from_province_ownership(province);
		float& base_budget = current_budget.get(owner);

		auto local_zone = state.world.province_get_state_membership(province);
		auto market = state.world.state_instance_get_market_from_local_market(local_zone);

		float admin_eff = state.world.nation_get_administrative_efficiency(owner);
		float admin_cost_factor = 2.0f - admin_eff;

		if(owner && state.world.province_get_nation_from_province_control(province) == owner) {
			auto& base_cost =
				state.military_definitions.unit_base_definitions[
					state.world.province_land_construction_get_type(lc)
				].build_cost;
			auto& current_purchased
				= state.world.province_land_construction_get_purchased_goods(lc);
			float construction_time =
				global_non_factory_construction_time_modifier(state)
				* float(state.military_definitions.unit_base_definitions[
					state.world.province_land_construction_get_type(lc)
				].build_time);


			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				auto cid = base_cost.commodity_type[i];
				if(!cid)
					break;
				if(
					current_purchased.commodity_amounts[i]
			>
					base_cost.commodity_amounts[i] * admin_cost_factor
				) continue;

				auto can_purchase_budget = base_budget / (price(state, market, cid) + 0.001f);
				auto can_purchase_construction = base_cost.commodity_amounts[i]
					* admin_cost_factor
					/ construction_time;

				auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
				base_budget = std::max(0.f, base_budget - can_purchase * price(state, market, cid));
				register_construction_demand(
					state,
					market,
					cid,
					can_purchase
				);
			}
		}
	}

	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto owner = state.world.province_get_nation_from_province_ownership(p);

		auto local_zone = state.world.province_get_state_membership(p);
		auto market = state.world.state_instance_get_market_from_local_market(local_zone);

		if(!owner || state.world.province_get_nation_from_province_control(p) != owner)
			return;
		auto rng = state.world.province_get_province_naval_construction(p);
		if(rng.begin() == rng.end())
			return;

		auto c = *(rng.begin());

		float admin_eff = state.world.nation_get_administrative_efficiency(owner);
		float admin_cost_factor = 2.0f - admin_eff;

		float& base_budget = current_budget.get(owner);
		auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
		auto& current_purchased = c.get_purchased_goods();
		float construction_time = global_non_factory_construction_time_modifier(state) *
			float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			auto cid = base_cost.commodity_type[i];
			if(!cid)
				break;
			if(
				current_purchased.commodity_amounts[i]
		>
				base_cost.commodity_amounts[i] * admin_cost_factor
			) continue;

			auto can_purchase_budget = base_budget / (price(state, market, cid) + 0.001f);
			auto can_purchase_construction = base_cost.commodity_amounts[i]
				* admin_cost_factor
				/ construction_time;

			auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
			base_budget = std::max(0.f, base_budget - can_purchase * price(state, market, cid));
			register_construction_demand(
				state,
				market,
				cid,
				can_purchase
			);
		}
	});

	for(auto c : state.world.in_province_building_construction) {
		auto owner = c.get_nation().id;
		auto spending_scale = state.world.nation_get_spending_level(owner);
		auto local_zone = c.get_province().get_state_membership();
		auto market = state.world.state_instance_get_market_from_local_market(local_zone);
		if(owner && c.get_province().get_nation_from_province_ownership() == c.get_province().get_nation_from_province_control() && !c.get_is_pop_project()) {
			auto t = economy::province_building_type(c.get_type());
			float& base_budget = current_budget.get(owner);
			auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_non_factory_construction_time_modifier(state) *
				float(state.economy_definitions.building_definitions[int32_t(t)].time);

			float admin_eff = state.world.nation_get_administrative_efficiency(owner);
			float admin_cost_factor = 2.0f - admin_eff;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				auto cid = base_cost.commodity_type[i];
				if(!cid) break;
				if(current_purchased.commodity_amounts[i] >
					base_cost.commodity_amounts[i] * admin_cost_factor) continue;

				auto can_purchase_budget = base_budget / (price(state, market, cid) + 0.001f);
				auto can_purchase_construction = base_cost.commodity_amounts[i]
					* admin_cost_factor
					/ construction_time;

				auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
				base_budget = std::max(0.f, base_budget - can_purchase * price(state, market, cid));
				register_construction_demand(
					state,
					market,
					cid,
					can_purchase
				);
			}
		}
	}

	for(auto c : state.world.in_state_building_construction) {
		auto owner = c.get_nation().id;
		auto spending_scale = state.world.nation_get_spending_level(owner);
		auto market = state.world.state_instance_get_market_from_local_market(c.get_state());
		if(owner && !c.get_is_pop_project()) {
			float& base_budget = current_budget.get(owner);
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();

			float construction_time =
				global_factory_construction_time_modifier(state)
				* float(c.get_type().get_construction_time())
				* (c.get_is_upgrade() ? 0.5f : 1.0f);

			float factory_mod =
				state.world.nation_get_modifier_values(
					owner,
					sys::national_mod_offsets::factory_cost
				) + 1.0f;

			float admin_eff = state.world.nation_get_administrative_efficiency(owner);
			float admin_cost_factor = 2.0f - admin_eff;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				auto cid = base_cost.commodity_type[i];
				if(!cid) break;
				if(current_purchased.commodity_amounts[i] >
					base_cost.commodity_amounts[i] * admin_cost_factor * factory_mod) continue;

				auto can_purchase_budget = base_budget / (price(state, market, cid) + 0.001f);
				auto can_purchase_construction = base_cost.commodity_amounts[i]
					* admin_cost_factor
					* factory_mod
					/ construction_time;

				auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
				base_budget = std::max(0.f, base_budget - can_purchase * price(state, market, cid));
				register_construction_demand(
					state,
					market,
					cid,
					can_purchase
				);
			}
		}
	}
}

float estimate_construction_spending(sys::state& state, dcon::nation_id n) {
	uint32_t total_commodities = state.world.commodity_size();
	auto current_budget =
		state.world.nation_get_stockpiles(n, economy::money)
		* float(state.world.nation_get_construction_spending(n))
		/ 100.f;

	auto total_cost = 0.f;

	/*
	state.world.nation_for_each_province_ownership(n, [&](auto ownership) {
		auto province = state.world.province_ownership_get_province(ownership);
		state.world.province_for_each_province_
	});
	*/

	for(auto lc : state.world.in_province_land_construction) {
		auto province = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(lc));
		auto owner = state.world.province_get_nation_from_province_ownership(province);

		if(owner != n) {
			continue;
		}

		float& base_budget = current_budget;

		auto local_zone = state.world.province_get_state_membership(province);
		auto market = state.world.state_instance_get_market_from_local_market(local_zone);

		float admin_eff = state.world.nation_get_administrative_efficiency(owner);
		float admin_cost_factor = 2.0f - admin_eff;

		if(owner && state.world.province_get_nation_from_province_control(province) == owner) {
			auto& base_cost =
				state.military_definitions.unit_base_definitions[
					state.world.province_land_construction_get_type(lc)
				].build_cost;
			auto& current_purchased
				= state.world.province_land_construction_get_purchased_goods(lc);
			float construction_time =
				global_non_factory_construction_time_modifier(state)
				* float(state.military_definitions.unit_base_definitions[
					state.world.province_land_construction_get_type(lc)
				].build_time);


			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				auto cid = base_cost.commodity_type[i];
				if(!cid)
					break;
				if(
					current_purchased.commodity_amounts[i]
			>
					base_cost.commodity_amounts[i] * admin_cost_factor
				) continue;

				auto can_purchase_budget = base_budget / (price(state, market, cid) + 0.001f);
				auto can_purchase_construction = base_cost.commodity_amounts[i]
					* admin_cost_factor
					/ construction_time;

				auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
				auto cost = std::min(base_budget, can_purchase * price(state, market, cid));
				base_budget -= cost;
				total_cost += cost * state.world.market_get_demand_satisfaction(market, cid);
			}
		}
	}

	state.world.nation_for_each_province_ownership(n, [&](auto ownership) {
		auto owner = n;
		auto p = state.world.province_ownership_get_province(ownership);

		auto local_zone = state.world.province_get_state_membership(p);
		auto market = state.world.state_instance_get_market_from_local_market(local_zone);

		if(!owner || state.world.province_get_nation_from_province_control(p) != owner)
			return;
		auto rng = state.world.province_get_province_naval_construction(p);
		if(rng.begin() == rng.end())
			return;

		auto c = *(rng.begin());

		float admin_eff = state.world.nation_get_administrative_efficiency(owner);
		float admin_cost_factor = 2.0f - admin_eff;

		float& base_budget = current_budget;
		auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
		auto& current_purchased = c.get_purchased_goods();
		float construction_time = global_non_factory_construction_time_modifier(state) *
			float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			auto cid = base_cost.commodity_type[i];
			if(!cid)
				break;
			if(
				current_purchased.commodity_amounts[i]
		>
				base_cost.commodity_amounts[i] * admin_cost_factor
			) continue;

			auto can_purchase_budget = base_budget / (price(state, market, cid) + 0.001f);
			auto can_purchase_construction = base_cost.commodity_amounts[i]
				* admin_cost_factor
				/ construction_time;

			auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
			auto cost = std::min(base_budget, can_purchase * price(state, market, cid));
			base_budget -= cost;
			total_cost += cost * state.world.market_get_demand_satisfaction(market, cid);
		}
	});

	for(auto c : state.world.in_province_building_construction) {
		auto owner = c.get_nation().id;
		if(owner != n) {
			continue;
		}
		auto spending_scale = state.world.nation_get_spending_level(owner);
		auto local_zone = c.get_province().get_state_membership();
		auto market = state.world.state_instance_get_market_from_local_market(local_zone);
		if(owner && c.get_province().get_nation_from_province_ownership() == c.get_province().get_nation_from_province_control() && !c.get_is_pop_project()) {
			auto t = economy::province_building_type(c.get_type());
			float& base_budget = current_budget;
			auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_non_factory_construction_time_modifier(state) *
				float(state.economy_definitions.building_definitions[int32_t(t)].time);

			float admin_eff = state.world.nation_get_administrative_efficiency(owner);
			float admin_cost_factor = 2.0f - admin_eff;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				auto cid = base_cost.commodity_type[i];
				if(!cid) break;
				if(current_purchased.commodity_amounts[i] >
					base_cost.commodity_amounts[i] * admin_cost_factor) continue;

				auto can_purchase_budget = base_budget / (price(state, market, cid) + 0.001f);
				auto can_purchase_construction = base_cost.commodity_amounts[i]
					* admin_cost_factor
					/ construction_time;

				auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);

				auto cost = std::min(base_budget, can_purchase * price(state, market, cid));
				base_budget -= cost;
				total_cost += cost * state.world.market_get_demand_satisfaction(market, cid);
			}
		}
	}

	for(auto c : state.world.in_state_building_construction) {
		auto owner = c.get_nation().id;
		if(owner != n) {
			continue;
		}
		auto spending_scale = state.world.nation_get_spending_level(owner);
		auto market = state.world.state_instance_get_market_from_local_market(c.get_state());
		if(owner && !c.get_is_pop_project()) {
			float& base_budget = current_budget;
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();

			float construction_time =
				global_factory_construction_time_modifier(state)
				* float(c.get_type().get_construction_time())
				* (c.get_is_upgrade() ? 0.5f : 1.0f);

			float factory_mod =
				state.world.nation_get_modifier_values(
					owner,
					sys::national_mod_offsets::factory_cost
				) + 1.0f;

			float admin_eff = state.world.nation_get_administrative_efficiency(owner);
			float admin_cost_factor = 2.0f - admin_eff;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				auto cid = base_cost.commodity_type[i];
				if(!cid) break;
				if(current_purchased.commodity_amounts[i] >
					base_cost.commodity_amounts[i] * admin_cost_factor * factory_mod) continue;

				auto can_purchase_budget = base_budget / (price(state, market, cid) + 0.001f);
				auto can_purchase_construction = base_cost.commodity_amounts[i]
					* admin_cost_factor
					* factory_mod
					/ construction_time;

				auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
				auto cost = std::min(base_budget, can_purchase * price(state, market, cid));
				base_budget -= cost;
				total_cost += cost * state.world.market_get_demand_satisfaction(market, cid);
			}
		}
	}

	return total_cost;
}

float estimate_private_construction_spendings(sys::state& state, dcon::nation_id nid) {
	float total = 0.f;

	for(auto c : state.world.in_province_building_construction) {
		auto owner = c.get_nation().id;
		if(owner != nid) {
			continue;
		}

		auto market = state.world.state_instance_get_market_from_local_market(
			c.get_province().get_state_membership()
		);

		// Rationale for not checking building type: Its an invalid state; should not occur under normal circumstances
		if(owner && owner == c.get_province().get_nation_from_province_control() && c.get_is_pop_project()) {
			auto t = economy::province_building_type(c.get_type());
			auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_non_factory_construction_time_modifier(state) *
				float(state.economy_definitions.building_definitions[int32_t(t)].time);
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i])
						total +=
						base_cost.commodity_amounts[i]
						* price(state, market, base_cost.commodity_type[i])
						/ construction_time;
				} else {
					break;
				}
			}
		}
	}

	for(auto c : state.world.in_state_building_construction) {
		auto owner = c.get_nation().id;
		if(owner != nid) {
			continue;
		}

		auto market = state.world.state_instance_get_market_from_local_market(c.get_state());
		if(owner && c.get_is_pop_project()) {
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_factory_construction_time_modifier(state) *
				float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::factory_cost) + 1.0f) * std::max(0.1f, state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::factory_owner_cost));

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod)
						total +=
						base_cost.commodity_amounts[i]
						* price(state, market, base_cost.commodity_type[i])
						* factory_mod
						/ construction_time;
				} else {
					break;
				}
			}
		}
	}

	return total;
}

void populate_private_construction_consumption(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_private_construction_demand(ids, cid, 0.0f);
		});
	}

	for(auto c : state.world.in_province_building_construction) {
		auto owner = c.get_nation().id;
		auto market = state.world.state_instance_get_market_from_local_market(
			c.get_province().get_state_membership()
		);

		// Rationale for not checking building type: Its an invalid state; should not occur under normal circumstances
		if(owner && owner == c.get_province().get_nation_from_province_control() && c.get_is_pop_project()) {
			auto t = economy::province_building_type(c.get_type());
			auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_non_factory_construction_time_modifier(state) *
				float(state.economy_definitions.building_definitions[int32_t(t)].time);
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i])
						state.world.market_get_private_construction_demand(
							market,
							base_cost.commodity_type[i]
						) +=
						base_cost.commodity_amounts[i]
						/ construction_time;
				} else {
					break;
				}
			}
		}
	}

	for(auto c : state.world.in_state_building_construction) {
		auto owner = c.get_nation().id;
		auto market = state.world.state_instance_get_market_from_local_market(c.get_state());
		if(owner && c.get_is_pop_project()) {
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_factory_construction_time_modifier(state) *
				float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::factory_cost) + 1.0f) * std::max(0.1f, state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::factory_owner_cost));

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod)
						state.world.market_get_private_construction_demand(
							market, base_cost.commodity_type[i]
						) +=
						base_cost.commodity_amounts[i]
						* factory_mod
						/ construction_time;
				} else {
					break;
				}
			}
		}
	}
}

float full_spending_cost(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	float military_total = 0.f;
	uint32_t total_commodities = state.world.commodity_size();

	float base_budget = state.world.nation_get_stockpiles(n, economy::money);
	float construction_budget =
		std::max(
			0.f,
			float(state.world.nation_get_construction_spending(n))
			/ 100.0f
			* base_budget
		);

	float l_spending = float(state.world.nation_get_land_spending(n)) / 100.0f;
	float n_spending = float(state.world.nation_get_naval_spending(n)) / 100.0f;
	float o_spending = float(state.world.nation_get_overseas_spending(n)) / 100.f;

	float total_construction_costs = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto local_market = state.world.state_instance_get_market_from_local_market(local_state);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto v = state.world.market_get_army_demand(local_market, cid)
				* l_spending
				* price(state, local_market, cid);
			assert(std::isfinite(v) && v >= 0.0f);
			total += v;
			military_total += v;
		}
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto v = state.world.market_get_navy_demand(local_market, cid)
				* n_spending * price(state, local_market, cid);
			assert(std::isfinite(v) && v >= 0.0f);
			total += v;
			military_total += v;
		}
		assert(std::isfinite(total) && total >= 0.0f);
		state.world.nation_set_maximum_military_costs(n, military_total);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto demand_const = state.world.market_get_construction_demand(local_market, cid);
			auto c_price = price(state, local_market, cid);

			total_construction_costs += demand_const * c_price;
		}
	});

	total += std::min(construction_budget, total_construction_costs);


	auto capital_state = state.world.province_get_state_membership(state.world.nation_get_capital(n));
	auto capital_market = state.world.state_instance_get_market_from_local_market(capital_state);

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			total += difference * price(state, capital_market, cid);
		}
	}
	assert(std::isfinite(total) && total >= 0.0f);

	auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));
	if(overseas_factor > 0) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || state.world.nation_get_unlocked_commodities(n, cid))) {
				total += overseas_factor * price(state, capital_market, cid) * o_spending;
			}
		}
	}

	assert(std::isfinite(total) && total >= 0.0f);
	// direct payments to pops

	auto const a_spending = float(state.world.nation_get_administrative_spending(n)) / 100.0f * float(state.world.nation_get_administrative_spending(n)) / 100.0f;
	auto const s_spending = state.world.nation_get_administrative_efficiency(n) * float(state.world.nation_get_social_spending(n)) / 100.0f;
	auto const e_spending = float(state.world.nation_get_education_spending(n)) * float(state.world.nation_get_education_spending(n)) / 100.0f / 100.0f;
	auto const m_spending = float(state.world.nation_get_military_spending(n)) * float(state.world.nation_get_military_spending(n)) / 100.0f / 100.f;
	auto const p_level = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::pension_level);
	auto const unemp_level = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::unemployment_benefit);
	auto const di_spending =
		float(state.world.nation_get_domestic_investment_spending(n))
		* float(state.world.nation_get_domestic_investment_spending(n))
		/ 100.0f
		/ 100.0f;

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		auto capitalists_def = state.culture_definitions.capitalists;
		auto capitalists_key = demographics::to_key(state, capitalists_def);
		auto capitalists = state.world.state_instance_get_demographics(local_state, capitalists_key);
		auto capitalists_base =
			state.world.market_get_life_needs_costs(market, capitalists_def)
			+ state.world.market_get_everyday_needs_costs(market, capitalists_def)
			+ state.world.market_get_luxury_needs_costs(market, capitalists_def);

		auto aristocrats_def = state.culture_definitions.aristocrat;
		auto aristoctats_key = demographics::to_key(state, aristocrats_def);
		auto aristocrats = state.world.state_instance_get_demographics(local_state, aristoctats_key);
		auto aristocrats_base =
			state.world.market_get_life_needs_costs(market, aristocrats_def)
			+ state.world.market_get_everyday_needs_costs(market, aristocrats_def)
			+ state.world.market_get_luxury_needs_costs(market, aristocrats_def);

		total +=
			state.defines.alice_domestic_investment_multiplier
			* di_spending
			* (
				capitalists * capitalists_base
				+ aristocrats * aristocrats_base
			)
			/ state.defines.alice_needs_scaling_factor;

		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			auto key = demographics::to_key(state, pt);
			auto employment_key = demographics::to_employment_key(state, pt);

			auto adj_pop_of_type =
				state.world.state_instance_get_demographics(local_state, key)
				/ state.defines.alice_needs_scaling_factor;

			if(adj_pop_of_type <= 0)
				return;

			auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
			if(ln_type == culture::income_type::administration) {
				total += a_spending * adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt);
			} else if(ln_type == culture::income_type::education) {
				total += e_spending * adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt);
			} else if(ln_type == culture::income_type::military) {
				total += m_spending * adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt);
			} else { // unemployment, pensions
				total += s_spending
					* adj_pop_of_type
					* p_level
					* state.world.market_get_life_needs_costs(market, pt);

				if(state.world.pop_type_get_has_unemployment(pt)) {
					auto emp =
						state.world.state_instance_get_demographics(local_state, employment_key)
						/ state.defines.alice_needs_scaling_factor;
					total +=
						s_spending
						* (adj_pop_of_type - emp)
						* unemp_level
						* state.world.market_get_life_needs_costs(market, pt);
				}
			}

			auto en_type = culture::income_type(state.world.pop_type_get_everyday_needs_income_type(pt));
			if(en_type == culture::income_type::administration) {
				total += a_spending * adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt);
			} else if(en_type == culture::income_type::education) {
				total += e_spending * adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt);
			} else if(en_type == culture::income_type::military) {
				total += m_spending * adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt);
			}

			auto lx_type = culture::income_type(state.world.pop_type_get_luxury_needs_income_type(pt));
			if(lx_type == culture::income_type::administration) {
				total += a_spending * adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt);
			} else if(lx_type == culture::income_type::education) {
				total += e_spending * adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt);
			} else if(lx_type == culture::income_type::military) {
				total += m_spending * adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt);
			}

			assert(std::isfinite(total) && total >= 0.0f);
		});
	});

	assert(std::isfinite(total) && total >= 0.0f);

	return total;
}

float estimate_stockpile_filling_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();

	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto market = state.world.state_instance_get_market_from_local_market(capital_state);

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto difference =
			state.world.nation_get_stockpile_targets(n, cid)
			- state.world.nation_get_stockpiles(n, cid);

		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			total +=
				difference
				* price(state, market, cid)
				* state.world.market_get_demand_satisfaction(market, cid);
		}
	}

	return total;
}

float estimate_overseas_penalty_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto market = state.world.state_instance_get_market_from_local_market(capital_state);

	auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));
	uint32_t total_commodities = state.world.commodity_size();

	if(overseas_factor > 0) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || state.world.nation_get_unlocked_commodities(n, cid))) {
				total +=
					overseas_factor
					* price(state, market, cid)
					* state.world.market_get_demand_satisfaction(market, cid);
			}
		}
	}

	return total;
}

float full_private_investment_cost(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		uint32_t total_commodities = state.world.commodity_size();
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			total +=
				state.world.market_get_private_construction_demand(market, cid)
				* price(state, market, cid);
		}
	});
	return total;
}

void update_national_consumption(sys::state& state, dcon::nation_id n, float spending_scale, float private_investment_scale) {
	uint32_t total_commodities = state.world.commodity_size();
	float l_spending = float(state.world.nation_get_land_spending(n)) / 100.0f;
	float n_spending = float(state.world.nation_get_naval_spending(n)) / 100.0f;
	float o_spending = float(state.world.nation_get_overseas_spending(n)) / 100.0f;

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto sat = state.world.market_get_demand_satisfaction(market, cid);
			auto sat_importance = std::min(1.f, 1.f / (price(state, market, cid) + 0.001f));
			auto sat_coefficient = (sat_importance + (1.f - sat_importance) * sat);

			register_demand(
				state,
				market,
				cid,
				state.world.market_get_army_demand(market, cid)
				* l_spending
				* spending_scale
				* sat_coefficient
				, economy_reason::nation
			);
			register_demand(
				state,
				market,
				cid,
				state.world.market_get_navy_demand(market, cid)
				* n_spending
				* spending_scale
				* sat_coefficient
				, economy_reason::nation
			);
		}
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			register_demand(
				state,
				market,
				cid,
				state.world.market_get_construction_demand(market, cid)
				* spending_scale,
				economy_reason::construction
			);
		}
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			register_demand(
				state,
				market,
				cid,
				state.world.market_get_private_construction_demand(market, cid)
				* private_investment_scale, economy_reason::construction);
		}
	});

	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto market = state.world.state_instance_get_market_from_local_market(capital_state);

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			auto sat = state.world.market_get_demand_satisfaction(market, cid);
			auto sat_importance = std::min(1.f, 1.f / (price(state, market, cid) + 0.001f));
			auto sat_coefficient = (sat_importance + (1.f - sat_importance) * sat);
			register_demand(
				state,
				market,
				cid,
				difference
				* spending_scale
				* sat_coefficient,
				economy_reason::stockpile
			);
		}
	}
	auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));
	if(overseas_factor > 0.f) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || state.world.nation_get_unlocked_commodities(n, cid))) {
				auto sat = state.world.market_get_demand_satisfaction(market, cid);
				auto sat_importance = std::min(1.f, 1.f / (price(state, market, cid) + 0.001f));
				auto sat_coefficient = (sat_importance + (1.f - sat_importance) * sat);

				register_demand(
					state,
					market,
					cid,
					overseas_factor
					* spending_scale
					* o_spending
					* sat_coefficient,
					economy_reason::overseas_penalty
				);
			}
		}
	}
}

void update_pop_consumption(
	sys::state& state,
	ve::vectorizable_buffer<float, dcon::nation_id>& invention_count
) {
	uint32_t total_commodities = state.world.commodity_size();

	state.ui_state.last_tick_investment_pool_change = 0;

	static const ve::fp_vector zero = ve::fp_vector{ 0.f };
	static const ve::fp_vector one = ve::fp_vector{ 1.f };

	// satisfaction buffers
	// they store how well pops satisfy their needs
	// we store them per pop now
	// because iteration per state
	// and per pop of each state is way too slow
	// we start with filling them with according subsistence values
	// and then attempt to buy the rest

	ve::int_vector build_factory = issue_rule::pop_build_factory;
	ve::int_vector expand_factory = issue_rule::pop_expand_factory;
	ve::int_vector can_invest = expand_factory | build_factory;
	
	state.world.execute_serial_over_pop([&](auto ids) {
		// get all data into vectors
		auto provs = state.world.pop_get_province_from_pop_location(ids);
		auto states = state.world.province_get_state_membership(provs);
		auto markets = state.world.state_instance_get_market_from_local_market(states);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
		auto pop_type = state.world.pop_get_poptype(ids);
		auto strata = state.world.pop_type_get_strata(pop_type);
		auto life_costs = ve::apply(
			[&](dcon::market_id m, dcon::pop_type_id pt) {
				return state.world.market_get_life_needs_costs(m, pt);
			}, markets, pop_type
		);
		auto everyday_costs = ve::apply(
			[&](dcon::market_id m, dcon::pop_type_id pt) {
				return state.world.market_get_everyday_needs_costs(m, pt);
			}, markets, pop_type
		);
		auto luxury_costs = ve::apply(
			[&](dcon::market_id m, dcon::pop_type_id pt) {
				return state.world.market_get_luxury_needs_costs(m, pt);
			}, markets, pop_type
		);
		auto pop_size = state.world.pop_get_size(ids);
		auto savings = state.world.pop_get_savings(ids);
		auto subsistence = ve_adjusted_subsistence_score(state, provs);

		auto available_subsistence = ve::min(subsistence_score_life, subsistence);
		subsistence = subsistence - available_subsistence;
		auto life_needs_satisfaction = available_subsistence / subsistence_score_life;

		available_subsistence = ve::min(subsistence_score_everyday, subsistence);
		subsistence = subsistence - available_subsistence;
		auto everyday_needs_satisfaction = available_subsistence / subsistence_score_everyday;

		auto luxury_needs_satisfaction = ve::fp_vector{ 0.f };

		// calculate market expenses

		auto life_to_satisfy = ve::max(0.f, 1.f - life_needs_satisfaction);
		auto everyday_to_satisfy = ve::max(0.f, 1.f - everyday_needs_satisfaction);
		auto luxury_to_satisfy = ve::max(0.f, 1.f - luxury_needs_satisfaction);

		auto required_spendings_for_life_needs =
			life_to_satisfy
			* life_costs
			* pop_size / state.defines.alice_needs_scaling_factor;

		auto required_spendings_for_everyday_needs =
			everyday_to_satisfy
			* everyday_costs
			* pop_size / state.defines.alice_needs_scaling_factor;

		auto required_spendings_for_luxury_needs =
			luxury_to_satisfy
			* luxury_costs
			* pop_size / state.defines.alice_needs_scaling_factor;

		// if goods are way too expensive:
		// reduce spendings, possibly to zero
		// even if it will lead to loss of money:
		// because at some point optimal way to satisfy your needs
		// is to not satisfy them immediately
		// we don't have base production for every type of goods,
		// so it's needed to avoid inflation spiral in early game
		// 
		// PS it's a way to embed "soft" price limits without breaking the ingame economy

		auto life_spending_mod = ve::max(0.f, ve::min(1.f, 1000.f / (1.f + life_costs)) - 0.1f);
		auto everyday_spending_mod = ve::max(0.f, ve::min(1.f, 10000.f / (1.f + everyday_costs)) - 0.1f);
		auto luxury_spending_mod = ve::max(0.f, ve::min(1.f, 100000.f / (1.f + luxury_costs)) - 0.1f);

		auto zero_life_costs = required_spendings_for_life_needs == 0;
		auto zero_everyday_costs = required_spendings_for_everyday_needs == 0;
		auto zero_luxury_costs = required_spendings_for_luxury_needs == 0;

		// buy life needs

		auto spend_on_life_needs = life_spending_mod * ve::min(savings, required_spendings_for_life_needs);
		auto satisfaction_life_money = ve::select(
			zero_life_costs,
			life_to_satisfy,
			spend_on_life_needs / required_spendings_for_life_needs);

		savings = savings - spend_on_life_needs;

		// handle investments now:
		auto nation_rules = state.world.nation_get_combined_issue_rules(nations);

		auto is_civilised = state.world.nation_get_is_civilized(nations);
		auto allows_investment_mask = (nation_rules & can_invest) != 0;
		ve::mask_vector nation_allows_investment = is_civilised && allows_investment_mask;

		auto capitalists_mask = pop_type == state.culture_definitions.capitalists;
		auto artisans_mask = pop_type == state.culture_definitions.artisans;
		auto landowners_mask = pop_type == state.culture_definitions.aristocrat;

		auto investment_ratio =
			ve::select(capitalists_mask, state.defines.alice_invest_capitalist, zero)
			+ ve::select(landowners_mask, state.defines.alice_invest_aristocrat, zero)
			+ ve::select(artisans_mask, state.defines.alice_invest_aristocrat, zero);

		auto investment = savings * investment_ratio;

		savings = savings - investment;

		// buy everday needs

		auto spend_on_everyday_needs = everyday_spending_mod * ve::min(savings, required_spendings_for_everyday_needs);
		auto satisfaction_everyday_money = ve::select(
			zero_everyday_costs,
			everyday_to_satisfy,
			spend_on_everyday_needs / required_spendings_for_everyday_needs);

		savings = savings - spend_on_everyday_needs;

		//handle savings before luxury goods spending
		ve::fp_vector bank_to_pop_money_transfer { 0.f };
		auto enough_savings = savings > required_spendings_for_luxury_needs;
		auto savings_for_transfer = required_spendings_for_luxury_needs - savings;

		auto enough_in_bank = state.world.nation_get_national_bank(nations) > ve::max(required_spendings_for_luxury_needs + 10000.f, savings_for_transfer);
		bank_to_pop_money_transfer = ve::select(
			enough_savings && nation_allows_investment && capitalists_mask,
			bank_to_pop_money_transfer - savings_for_transfer * state.defines.alice_save_aristocrat,
			bank_to_pop_money_transfer
		);
		bank_to_pop_money_transfer = ve::select(
			enough_savings && nation_allows_investment && landowners_mask,
			bank_to_pop_money_transfer - savings_for_transfer * state.defines.alice_save_capitalist,
			bank_to_pop_money_transfer
		);
		bank_to_pop_money_transfer = ve::select(
			!enough_savings && nation_allows_investment && enough_in_bank,
			bank_to_pop_money_transfer + savings_for_transfer,
			bank_to_pop_money_transfer
		);

		ve::apply(
			[&](float transfer, dcon::nation_id n) {
				state.world.nation_get_national_bank(n) -= transfer;
				return 0;
			}, bank_to_pop_money_transfer, nations
		);

		savings = savings + bank_to_pop_money_transfer;

		// buy luxury needs

		auto spend_on_luxury_needs = luxury_spending_mod * ve::min(savings, required_spendings_for_luxury_needs);
		auto satisfaction_luxury_money = ve::select(
			zero_luxury_costs,
			luxury_to_satisfy,
			spend_on_luxury_needs / required_spendings_for_luxury_needs);

		savings = savings - spend_on_luxury_needs;

		// induce demand

		auto willing_to_spend_on_life_needs = life_spending_mod * savings * state.defines.alice_needs_lf_spend;
		auto willing_to_spend_on_everyday_needs = everyday_spending_mod * savings * state.defines.alice_needs_ev_spend;
		auto willing_to_spend_on_luxury_needs = luxury_spending_mod * savings * state.defines.alice_needs_lx_spend;

		auto induced_life_demand = ve::select(
			zero_life_costs,
			0.f,
			willing_to_spend_on_life_needs / life_costs
		);
		auto induced_everyday_demand = ve::select(
			zero_everyday_costs,
			0.f,
			willing_to_spend_on_everyday_needs / everyday_costs
		);
		auto induced_luxury_demand = ve::select(
			zero_luxury_costs,
			0.f,
			willing_to_spend_on_luxury_needs / luxury_costs
		);
		ve::fp_vector old_life = pop_demographics::get_life_needs(state, ids);
		ve::fp_vector old_everyday = pop_demographics::get_everyday_needs(state, ids);
		ve::fp_vector old_luxury = pop_demographics::get_luxury_needs(state, ids);

		savings = savings * (
			1.f
			- state.defines.alice_needs_lf_spend
			- state.defines.alice_needs_ev_spend
			- state.defines.alice_needs_lx_spend
		);

		// suppose that old satisfaction was calculated
		// for the same local subsistence conditions and find "raw" satisfaction
		// old = raw + sub ## first summand is "raw satisfaction"
		// we assume that currently calculated satisfaction is caused only by subsistence

		auto old_life_money =
			ve::max(0.f, old_life - life_needs_satisfaction);
		auto old_everyday_money =
			ve::max(0.f, old_everyday - everyday_needs_satisfaction);
		auto old_luxury_money =
			ve::max(0.f, old_luxury - luxury_needs_satisfaction);

		auto delayed_life_from_money =
			(old_life_money * 0.5f) + (satisfaction_life_money * 0.5f);
		auto delayed_everyday_from_money =
			(old_everyday_money * 0.5f) + (satisfaction_everyday_money * 0.5f);
		auto delayed_luxury_from_money =
			(old_luxury_money * 0.5f) + (satisfaction_luxury_money * 0.5f);

		auto result_life = ve::min(1.f, delayed_life_from_money + life_needs_satisfaction);
		auto result_everyday = ve::min(1.f, delayed_everyday_from_money + everyday_needs_satisfaction);
		auto result_luxury = ve::min(1.f, delayed_luxury_from_money + luxury_needs_satisfaction);

		ve::apply([&](float life, float everyday, float luxury) {
			assert(life >= 0.f && life <= 1.f);
			assert(everyday >= 0.f && everyday <= 1.f);
			assert(luxury >= 0.f && luxury <= 1.f);
		}, result_life, result_everyday, result_luxury);


		pop_demographics::set_life_needs(state, ids, result_life);
		pop_demographics::set_everyday_needs(state, ids, result_everyday);
		pop_demographics::set_luxury_needs(state, ids, result_luxury);

		auto final_demand_scale_life =
			pop_size / state.defines.alice_needs_scaling_factor
			* delayed_life_from_money;

		auto final_demand_scale_everyday =
			pop_size / state.defines.alice_needs_scaling_factor
			* delayed_everyday_from_money;

		auto final_demand_scale_luxury =
			pop_size / state.defines.alice_needs_scaling_factor
			* delayed_luxury_from_money;

		ve::apply([&](
			dcon::market_id m,
			float scale_life,
			float scale_everyday,
			float scale_luxury,
			float investment,
			auto pop_type
			) {
				state.world.market_get_life_needs_scale(m, pop_type) += scale_life;
				state.world.market_get_everyday_needs_scale(m, pop_type) += scale_everyday;
				state.world.market_get_luxury_needs_scale(m, pop_type) += scale_luxury;
				auto zone = state.world.market_get_zone_from_local_market(m);
				auto nation = state.world.state_instance_get_nation_from_state_ownership(zone);
				state.world.nation_get_private_investment(nation) += investment;
				if(nation == state.local_player_nation) {
					state.ui_state.last_tick_investment_pool_change += investment;
				}
		},
			markets,
			final_demand_scale_life,
			final_demand_scale_everyday,
			final_demand_scale_luxury,
			investment,
			pop_type
		);

		//state.world.pop_set_savings(ids, savings);
	});

	// iterate over all (market,pop type,trade good) pairs to finalise this calculation
	state.world.execute_serial_over_market([&](auto ids) {
		auto states = state.world.market_get_zone_from_local_market(ids);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
		auto invention_factor = state.defines.invention_impact_on_demand * invention_count.get(nations) + 1.f;

		ve::fp_vector life_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_life_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_life_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_life_needs) + 1.0f
		};
		ve::fp_vector everyday_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
		};
		ve::fp_vector luxury_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_luxury_needs) + 1.0f,
		};

		for(const auto t : state.world.in_pop_type) {
			auto scale_life = state.world.market_get_life_needs_scale(ids, t);
			auto scale_everyday = state.world.market_get_everyday_needs_scale(ids, t);
			auto scale_luxury = state.world.market_get_luxury_needs_scale(ids, t);

			auto strata = t.get_strata();

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

				auto life_weight =
					state.world.market_get_life_needs_weights(ids, cid);
				auto everyday_weight =
					state.world.market_get_everyday_needs_weights(ids, cid);
				auto luxury_weight =
					state.world.market_get_luxury_needs_weights(ids, cid);
				auto base_life =
					state.world.pop_type_get_life_needs(t, cid);
				auto base_everyday =
					state.world.pop_type_get_everyday_needs(t, cid);
				auto base_luxury =
					state.world.pop_type_get_luxury_needs(t, cid);

				auto valid_good_mask = valid_need(state, nations, cid);

				auto demand_life =
					base_life
					* scale_life
					* life_mul[strata]
					* life_weight
					* state.defines.alice_lf_needs_scale;
				auto demand_everyday =
					base_everyday
					* scale_everyday
					* everyday_mul[strata]
					* everyday_weight
					* state.defines.alice_ev_needs_scale
					* invention_factor;
				auto demand_luxury =
					base_luxury
					* scale_luxury
					* luxury_mul[strata]
					* luxury_weight
					* state.defines.alice_lx_needs_scale
					* invention_factor;

				demand_life = ve::select(valid_good_mask, demand_life, 0.f);
				demand_everyday = ve::select(valid_good_mask, demand_everyday, 0.f);
				demand_luxury = ve::select(valid_good_mask, demand_luxury, 0.f);

				register_demand(state, ids, cid, demand_life, economy_reason::pop);
				register_demand(state, ids, cid, demand_everyday, economy_reason::pop);
				register_demand(state, ids, cid, demand_luxury, economy_reason::pop);
			}
		}
	});
}

void advance_construction(sys::state& state, dcon::nation_id n) {
	uint32_t total_commodities = state.world.commodity_size();
	float p_spending = state.world.nation_get_private_investment_effective_fraction(n);
	float refund_amount = 0.0f;

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
			auto& nat_demand = state.world.market_get_construction_demand(market, c);
			auto com_price = price(state, market, c);
			auto d_sat = state.world.market_get_demand_satisfaction(market, c);
			refund_amount +=
				nat_demand
				* (1.0f - d_sat)
				* com_price;
			assert(refund_amount >= 0.0f);

			nat_demand *= d_sat;
			state.world.market_get_private_construction_demand(market, c) *= p_spending * d_sat;
		}
	});

	assert(refund_amount >= 0.0f);
	state.world.nation_get_stockpiles(n, economy::money) += refund_amount;

	float admin_eff = state.world.nation_get_administrative_efficiency(n);
	float admin_cost_factor = 2.0f - admin_eff;

	for(auto p : state.world.nation_get_province_ownership(n)) {
		if(p.get_province().get_nation_from_province_control() != n)
			continue;

		auto market = state.world.state_instance_get_market_from_local_market(p.get_province().get_state_membership());

		for(auto pops : p.get_province().get_pop_location()) {
			auto rng = pops.get_pop().get_province_land_construction();
			if(rng.begin() != rng.end()) {
				auto c = *(rng.begin());
				auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time =
					global_non_factory_construction_time_modifier(state)
					* float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(
							current_purchased.commodity_amounts[i]
							< base_cost.commodity_amounts[i]
							* admin_cost_factor
						) {
							auto amount = base_cost.commodity_amounts[i] / construction_time;
							auto& source
								= state.world.market_get_construction_demand(
									market,
									base_cost.commodity_type[i]
								);
							auto delta =
								std::max(0.0f,
								std::min(source,
									base_cost.commodity_amounts[i] / construction_time
								));
							current_purchased.commodity_amounts[i] += delta;
							source -= delta;
						}
					} else {
						break;
					}
				}
				break; // only advance one construction per province
			}
		}
		{
			auto rng = p.get_province().get_province_naval_construction();
			if(rng.begin() != rng.end()) {
				auto c = *(rng.begin());
				auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = global_non_factory_construction_time_modifier(state) * float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor) {
							auto amount = base_cost.commodity_amounts[i] / construction_time;
							auto& source = state.world.market_get_construction_demand(market, base_cost.commodity_type[i]);
							auto delta = std::max(0.0f, std::min(source, base_cost.commodity_amounts[i] / construction_time));

							current_purchased.commodity_amounts[i] += delta;
							source -= delta;
						}
					} else {
						break;
					}
				}
			}
		}
	}

	for(auto c : state.world.nation_get_province_building_construction(n)) {
		if(c.get_province().get_nation_from_province_ownership() == c.get_province().get_nation_from_province_control()) {
			auto t = economy::province_building_type(c.get_type());
			auto market = c.get_province().get_state_membership().get_market_from_local_market();
			// Rationale for not checking the building type:
			// Pop projects created for forts and naval bases should NOT happen in the first place, so checking against them
			// is a waste of resources
			if(!c.get_is_pop_project()) {
				auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = global_non_factory_construction_time_modifier(state) * float(state.economy_definitions.building_definitions[int32_t(t)].time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor) {
							auto amount = base_cost.commodity_amounts[i] / construction_time;
							auto& source = state.world.market_get_construction_demand(market, base_cost.commodity_type[i]);
							auto delta = std::max(0.0f, std::min(source, base_cost.commodity_amounts[i] / construction_time));

							current_purchased.commodity_amounts[i] += delta;
							source -= delta;
						}
					} else {
						break;
					}
				}
			} else if(c.get_is_pop_project()) {
				auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = global_non_factory_construction_time_modifier(state) * float(state.economy_definitions.building_definitions[int32_t(t)].time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i]) {
							auto amount = base_cost.commodity_amounts[i] / construction_time;
							auto& source = state.world.market_get_private_construction_demand(market, base_cost.commodity_type[i]);
							auto delta = std::max(0.0f, std::min(source, base_cost.commodity_amounts[i] / construction_time));

							current_purchased.commodity_amounts[i] += delta;
							source -= delta;
						}
					} else {
						break;
					}
				}
			}
		}
	}

	for(auto c : state.world.nation_get_state_building_construction(n)) {
		auto market = c.get_state().get_market_from_local_market();
		if(!c.get_is_pop_project()) {
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_factory_construction_time_modifier(state) * float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod * admin_cost_factor) {
						auto amount = base_cost.commodity_amounts[i] / construction_time;
						auto& source = state.world.market_get_construction_demand(market, base_cost.commodity_type[i]);
						auto delta = std::max(0.0f, std::min(source, base_cost.commodity_amounts[i] * factory_mod / construction_time));

						current_purchased.commodity_amounts[i] += delta;
						source -= delta;
					}
				} else {
					break;
				}
			}
		} else {
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_factory_construction_time_modifier(state) * float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f) *
				std::max(0.1f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_owner_cost));

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod) {
						auto amount = base_cost.commodity_amounts[i] / construction_time;
						auto& source = state.world.market_get_private_construction_demand(market, base_cost.commodity_type[i]);
						auto delta = std::max(0.0f, std::min(source, base_cost.commodity_amounts[i] * factory_mod / construction_time));

						current_purchased.commodity_amounts[i] += delta;
						source -= delta;
					}
				} else {
					break;
				}
			}
		}
	}
}

struct profit_distribution {
	float per_primary_worker;
	float per_secondary_worker;
	float per_owner;
};

profit_distribution distribute_factory_profit(
	sys::state const& state,
	dcon::state_instance_const_fat_id s,
	float min_wage,
	float total_profit
) {
	float total_min_to_pworkers =
		min_wage * state.world.state_instance_get_demographics(s,
			demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
	float total_min_to_sworkers =
		min_wage * state.world.state_instance_get_demographics(s,
			demographics::to_employment_key(state, state.culture_definitions.secondary_factory_worker));

	float num_pworkers = state.world.state_instance_get_demographics(s,
			demographics::to_key(state, state.culture_definitions.primary_factory_worker));
	float num_sworkers = state.world.state_instance_get_demographics(s,
			demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
	float num_owners = state.world.state_instance_get_demographics(s,
			demographics::to_key(state, state.culture_definitions.capitalists));

	auto per_pworker_profit = 0.0f;
	auto per_sworker_profit = 0.0f;
	auto per_owner_profit = 0.0f;

	if(total_min_to_pworkers + total_min_to_sworkers <= total_profit && num_owners > 0) {
		// profit higher than miin wage
		// owners present
		auto surplus = total_profit - (total_min_to_pworkers + total_min_to_sworkers);
		per_pworker_profit = num_pworkers > 0 ? (total_min_to_pworkers + surplus * factory_pworkers_cut) / num_pworkers : 0.0f;
		per_sworker_profit = num_sworkers > 0 ? (total_min_to_sworkers + surplus * factory_sworkers_cut) / num_sworkers : 0.0f;
		per_owner_profit = (surplus * factory_owners_cut) / num_owners;
	} else if(total_min_to_pworkers + total_min_to_sworkers <= total_profit && num_sworkers > 0) {
		// profit higher than miin wage
		// no owners
		// sworkers present
		auto surplus = total_profit - (total_min_to_pworkers + total_min_to_sworkers);
		per_pworker_profit = num_pworkers > 0 ? (total_min_to_pworkers + surplus * factory_pworkers_cut / factory_workers_cut) / num_pworkers : 0.0f;
		per_sworker_profit = num_sworkers > 0 ? (total_min_to_sworkers + surplus * factory_sworkers_cut / factory_workers_cut) / num_sworkers : 0.0f;
	} else if(total_min_to_pworkers + total_min_to_sworkers <= total_profit) {
		// profit higher than min wage
		// no owners
		// no sworkers
		per_pworker_profit = num_pworkers > 0 ? total_profit / num_pworkers : 0.0f;
	} else if(num_pworkers + num_sworkers > 0) {
		// profit lower than min wage
		per_pworker_profit = total_profit * factory_pworkers_cut / factory_workers_cut / (num_pworkers + num_sworkers);
		per_sworker_profit = total_profit * factory_sworkers_cut / factory_workers_cut / (num_pworkers + num_sworkers);
	}

	return {
		.per_primary_worker = per_pworker_profit,
		.per_secondary_worker = per_sworker_profit,
		.per_owner = per_owner_profit
	};
}

// this function partly emulates demand generated by nations
void emulate_construction_demand(sys::state& state, dcon::nation_id n) {
	// phase 1:
	// simulate spending on construction of units
	// useful to help the game start with some production of artillery and small arms

	float income_to_build_units = 100'000.f;

	if(state.world.nation_get_owned_province_count(n) == 0) {
		return;
	}


	// we build infantry and artillery:
	auto infantry = state.military_definitions.infantry;
	auto artillery = state.military_definitions.artillery;

	auto& infantry_def = state.military_definitions.unit_base_definitions[infantry];
	auto& artillery_def = state.military_definitions.unit_base_definitions[artillery];

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		float daily_cost = 0.f;

		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(infantry_def.build_cost.commodity_type[i]) {
				auto p = price(state, market, infantry_def.build_cost.commodity_type[i]);
				daily_cost += infantry_def.build_cost.commodity_amounts[i] / infantry_def.build_time * p;
			} else {
				break;
			}
		}
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(artillery_def.build_cost.commodity_type[i]) {
				auto p = price(state, market, artillery_def.build_cost.commodity_type[i]);
				daily_cost += artillery_def.build_cost.commodity_amounts[i] / artillery_def.build_time * p;
			} else {
				break;
			}
		}

		auto pairs_to_build = std::max(0.f, income_to_build_units / (daily_cost + 1.f) - 0.1f);

		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(infantry_def.build_cost.commodity_type[i]) {
				auto daily_amount = infantry_def.build_cost.commodity_amounts[i] / infantry_def.build_time;
				register_demand(state, market, infantry_def.build_cost.commodity_type[i], daily_amount * pairs_to_build, economy_reason::construction);
			} else {
				break;
			}
		}
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(artillery_def.build_cost.commodity_type[i]) {
				auto daily_amount = artillery_def.build_cost.commodity_amounts[i] / artillery_def.build_time;
				register_demand(state, market, artillery_def.build_cost.commodity_type[i], daily_amount * pairs_to_build, economy_reason::construction);
			} else {
				break;
			}
		}
	});

	// phase 2:
	// simulate spending on construction of factories
	// helps with machine tools and cement

	float income_to_build_factories = 1'000'000.f;

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		// iterate over all factory types available from the start and find "average" daily construction cost:
		float sum_of_build_times = 0.f;
		float cost_factory_set = 0.f;
		float count = 0.f;

		state.world.for_each_factory_type([&](dcon::factory_type_id factory_type) {
			if(!state.world.factory_type_get_is_available_from_start(factory_type)) {
				return;
			}

			auto build_time = state.world.factory_type_get_construction_time(factory_type);
			auto& build_cost = state.world.factory_type_get_construction_costs(factory_type);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto pr = price(state, market, build_cost.commodity_type[i]);
					cost_factory_set += pr * build_cost.commodity_amounts[i] / build_time;
				} else {
					break;
				}
			}
			count++;
		});


		// calculate amount of factory sets we are building:
		auto num_of_factory_sets = std::max(0.f, income_to_build_factories / (cost_factory_set + 1.f) - 0.1f);

		// emulate construction demand
		state.world.for_each_factory_type([&](dcon::factory_type_id factory_type) {
			if(!state.world.factory_type_get_is_available_from_start(factory_type)) {
				return;
			}

			auto build_time = state.world.factory_type_get_construction_time(factory_type);
			auto& build_cost = state.world.factory_type_get_construction_costs(factory_type);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto amount = build_cost.commodity_amounts[i];
					register_demand(
						state,
						market,
						build_cost.commodity_type[i], amount / build_time * num_of_factory_sets,
						economy_reason::construction
					);
				} else {
					break;
				}
			}
			count++;
		});
	});
}

// ### Private Investment ###

const inline float courage = 1.0f;
const inline float days_prepaid = 5.f;

/* Returns number of initiated projects */
std::vector<full_construction_state> estimate_private_investment_upgrade(sys::state& state, dcon::nation_id nid) {
	auto n = dcon::fatten(state.world, nid);
	auto nation_rules = n.get_combined_issue_rules();

	// check if current projects are already too expensive for capitalists to manage
	float total_cost = estimate_private_construction_spendings(state, n) * days_prepaid * 40.f;
	float total_cost_added = 0.f;
	float current_inv = n.get_private_investment();

	std::vector<full_construction_state> res;

	if(current_inv <= total_cost) {
		return res;
	}

	if(!n.get_is_civilized()) {
		return res;
	}

	if((nation_rules & (issue_rule::pop_build_factory | issue_rule::pop_expand_factory)) == 0) {
		return res;
	}

	static std::vector<dcon::factory_type_id> desired_types;
	desired_types.clear();

	static std::vector<dcon::state_instance_id> states_in_order;
	states_in_order.clear();
	for(auto si : n.get_state_ownership()) {
		if(si.get_state().get_capital().get_is_colonial() == false) {
			states_in_order.push_back(si.get_state().id);
		}
	}

	std::sort(
		states_in_order.begin(),
		states_in_order.end(),
		[&](dcon::state_instance_id a, dcon::state_instance_id b
			) {
				auto a_pop = state.world.state_instance_get_demographics(a, demographics::total);
				auto b_pop = state.world.state_instance_get_demographics(b, demographics::total);
				if(a_pop != b_pop)
					return a_pop > b_pop;
				return a.index() < b.index(); // force total ordering
	});

	//upgrade all good targets!!!
	//upgrading only one per run is too slow and leads to massive unemployment!!!
	bool found_investment = false;

	for(auto s : states_in_order) {
		auto market = state.world.state_instance_get_market_from_local_market(s);
		auto pw_num = state.world.state_instance_get_demographics(s,
				demographics::to_key(state, state.culture_definitions.primary_factory_worker));
		auto pw_employed = state.world.state_instance_get_demographics(s,
				demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));

		if(pw_employed >= pw_num && pw_num > 0.0f)
			continue; // no spare workers

		int32_t num_factories = 0;
		float profit = 0.0f;
		dcon::factory_id selected_factory;

		// is there an upgrade target ?
		auto d = state.world.state_instance_get_definition(s);
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == n) {
				for(auto f : p.get_province().get_factory_location()) {
					++num_factories;

					if(
						(nation_rules & issue_rule::pop_expand_factory) != 0
						&& f.get_factory().get_production_scale() >= 0.9f
						&& f.get_factory().get_primary_employment() >= 0.9f
						&& f.get_factory().get_level() < uint8_t(255)) {

						auto type = f.get_factory().get_building_type();
						auto ug_in_progress = false;
						for(auto c : state.world.state_instance_get_state_building_construction(s)) {
							if(c.get_type() == type) {
								ug_in_progress = true;
								break;
							}
						}

						if(ug_in_progress) {
							continue;
						}

						if(auto new_p =
								f.get_factory().get_full_profit()
								/ f.get_factory().get_level();
							new_p > profit
						) {
							profit = new_p;
							selected_factory = f.get_factory();
						}
					}
				}
			}
		}
		if(selected_factory && profit > 0.f) {
			res.push_back({	n, s, true, true, state.world.factory_get_building_type(selected_factory) });
			
		}
	}

	return res;
}

/* Returns number of initiated projects */
std::vector<full_construction_state> estimate_private_investment_construct(sys::state& state, dcon::nation_id nid) {
	auto n = dcon::fatten(state.world, nid);
	auto nation_rules = n.get_combined_issue_rules();

	// check if current projects are already too expensive for capitalists to manage
	float total_cost = estimate_private_construction_spendings(state, n) * days_prepaid * 40.f;
	float total_cost_added = 0.f;
	float current_inv = n.get_private_investment();

	std::vector<full_construction_state> res;

	if(current_inv <= total_cost) {
		return res;
	}

	if(!n.get_is_civilized()) {
		return res;
	}

	if((nation_rules & (issue_rule::pop_build_factory | issue_rule::pop_expand_factory)) == 0) {
		return res;
	}

	static std::vector<dcon::factory_type_id> desired_types;
	desired_types.clear();

	static std::vector<dcon::state_instance_id> states_in_order;
	states_in_order.clear();

	for(auto si : n.get_state_ownership()) {
		if(si.get_state().get_capital().get_is_colonial() == false) {
			states_in_order.push_back(si.get_state().id);
		}
	}

	float profit = 0.0f;
	dcon::factory_id selected_factory;

	std::sort(
		states_in_order.begin(),
		states_in_order.end(),
		[&](dcon::state_instance_id a, dcon::state_instance_id b
			) {
				auto a_pop = state.world.state_instance_get_demographics(a, demographics::total);
				auto b_pop = state.world.state_instance_get_demographics(b, demographics::total);
				if(a_pop != b_pop)
					return a_pop > b_pop;
				return a.index() < b.index(); // force total ordering
	});

	for(auto s : states_in_order) {
		auto existing_constructions = state.world.state_instance_get_state_building_construction(s);
		if(existing_constructions.begin() != existing_constructions.end())
			continue; // already building

		if(current_inv * courage < total_cost + total_cost_added) {
			continue;
		}
		
		if((nation_rules & issue_rule::pop_build_factory) == 0) {
			continue;
		}

		int32_t num_factories = 0;
		auto d = state.world.state_instance_get_definition(s);
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == n) {
				for(auto f : p.get_province().get_factory_location()) {
					++num_factories;
				}
			}
		}

		if(num_factories >= int32_t(state.defines.factories_per_state)) {
			continue;
		}

		auto market = state.world.state_instance_get_market_from_local_market(s);
			
		// randomly try a valid (check coastal, unlocked, non existing) factory
		desired_types.clear();
		ai::get_desired_factory_types(state, n, market, desired_types);

		if(desired_types.empty()) {
			continue;
		}

		auto selected = desired_types[
			rng::get_random(state, uint32_t((n.id.index() << 6) ^ s.index()))
				% desired_types.size()
		];

		if(
			state.world.factory_type_get_is_coastal(selected)
			&& !province::state_is_coastal(state, s)
		)
			continue;

		bool already_in_progress = [&]() {
			for(auto p : state.world.state_instance_get_state_building_construction(s)) {
				if(p.get_type() == selected)
					return true;
			}
			return false;
			}();

		if(already_in_progress)
			continue;


		bool present_in_location = false;
		province::for_each_province_in_state_instance(state, s, [&](dcon::province_id p) {
			for(auto fac : state.world.province_get_factory_location(p)) {
				auto type = fac.get_factory().get_building_type();
				if(selected == type) {
					present_in_location = true;
					return;
				}
			}
		});

		if(present_in_location) {
			continue;
		}

		auto costs = state.world.factory_type_get_construction_costs(selected);
		auto time = state.world.factory_type_get_construction_time(selected);
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(costs.commodity_type[i]) {
				total_cost_added +=
					costs.commodity_amounts[i]
					* price(state, market, costs.commodity_type[i])
					/ float(time)
					* days_prepaid;
			} else {
				break;
			}
		}

		if(current_inv * courage < total_cost + total_cost_added) {
			continue;
		}

		res.push_back({
			n, s, true, false, selected });
	}

	return res;
}

std::vector<full_construction_province> estimate_private_investment_province(sys::state& state, dcon::nation_id nid) {
	auto n = dcon::fatten(state.world, nid);
	auto nation_rules = n.get_combined_issue_rules();

	// check if current projects are already too expensive for capitalists to manage
	float total_cost = estimate_private_construction_spendings(state, n) * days_prepaid * 40.f;
	float total_cost_added = 0.f;
	float current_inv = n.get_private_investment();

	std::vector<full_construction_province> res;

	if(current_inv <= total_cost) {
		return res;
	}

	if(!n.get_is_civilized()) {
		return res;
	}

	if((nation_rules & issue_rule::pop_build_factory) == 0) {
		return res;
	}

	static std::vector<std::pair<dcon::province_id, int32_t>> provinces_in_order;
	provinces_in_order.clear();
	for(auto si : n.get_state_ownership()) {
		if(si.get_state().get_capital().get_is_colonial() == false) {
			auto s = si.get_state().id;
			auto d = state.world.state_instance_get_definition(s);
			int32_t num_factories = 0;
			for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
				if(province::generic_can_build_railroads(state, p.get_province(), n) &&
						p.get_province().get_nation_from_province_ownership() == n) {
					for(auto f : p.get_province().get_factory_location())
						num_factories += int32_t(f.get_factory().get_level());
					provinces_in_order.emplace_back(p.get_province().id, num_factories);
				}
			}
			// The state's number of factories is intentionally given
			// to all the provinces within the state so the
			// railroads aren't just built on a single province within a state
			for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
				if(province::generic_can_build_railroads(state, p.get_province(), n) &&
						p.get_province().get_nation_from_province_ownership() == n)
					provinces_in_order.emplace_back(p.get_province().id, num_factories);
			}
		}
	}
	if(!provinces_in_order.empty()) {
		std::pair<dcon::province_id, int32_t> best_p = provinces_in_order[0];
		for(auto e : provinces_in_order)
			if(e.second > best_p.second)
				best_p = e;

		auto sid = state.world.province_get_state_membership(best_p.first);
		auto market = state.world.state_instance_get_market_from_local_market(sid);

		auto costs = state.economy_definitions.building_definitions[int32_t(province_building_type::railroad)].cost;
		auto time = state.economy_definitions.building_definitions[int32_t(province_building_type::railroad)].time;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(costs.commodity_type[i]) {
				total_cost_added +=
					costs.commodity_amounts[i]
					* price(state, market, costs.commodity_type[i])
					/ float(time)
					* days_prepaid;
			} else {
				break;
			}
		}

		if(n.get_private_investment() * courage < total_cost + total_cost_added) {
			return res;
		}

		res.push_back({ n, best_p.first , true, province_building_type::railroad });
	}

	return res;
}

void run_private_investment(sys::state& state) {
	// make new investments
	for(auto n : state.world.in_nation) {
		auto upgrades = estimate_private_investment_upgrade(state, n);

		for(auto r : upgrades) {
			auto new_up = fatten(
			state.world,
			state.world.force_create_state_building_construction(r.state, r.nation)
			);

			new_up.set_is_pop_project(r.is_pop_project);
			new_up.set_is_upgrade(r.is_upgrade);
			new_up.set_type(r.type);
		}

		auto constructions = estimate_private_investment_construct(state, n);

		for(auto r : constructions) {
			auto new_up = fatten(
			state.world,
			state.world.force_create_state_building_construction(r.state, r.nation)
			);

			new_up.set_is_pop_project(r.is_pop_project);
			new_up.set_is_upgrade(r.is_upgrade);
			new_up.set_type(r.type);
		}

		auto province_constr = estimate_private_investment_province(state, n);

		for(auto r : province_constr) {
			auto new_rr = fatten(
				state.world,
				state.world.force_create_province_building_construction(r.province, r.nation)
			);
			new_rr.set_is_pop_project(r.is_pop_project);
			new_rr.set_type(uint8_t(r.type));
		}

		// If nowhere to invest
		if (estimate_private_construction_spendings(state, n) < 1.f && upgrades.size() == 0 && constructions.size() == 0 && province_constr.size() == 0) {
			// If it's a subject - transfer private investment to overlord
			// If it's an overlord - distribute some private invesmtent to subjects
			auto rel = state.world.nation_get_overlord_as_subject(n);
			auto overlord = state.world.overlord_get_ruler(rel);

			auto amt = state.world.nation_get_private_investment(n) * state.defines.alice_privateinvestment_subject_transfer / 100.f;
			state.world.nation_get_private_investment(n) -= amt;

			if(overlord) {
				state.world.nation_get_private_investment(overlord) += amt;
			} else {
				auto subjects = nations::nation_get_subjects(state, n);
				if(subjects.size() > 0) {
					auto part = amt / subjects.size();
					for(auto s : subjects) {
						state.world.nation_get_private_investment(s) += part;
					}
				}
			}
		}
	}
}

void daily_update(sys::state& state, bool presimulation, float presimulation_stage) {

	static const ve::fp_vector zero = ve::fp_vector{ 0.f };
	static const ve::fp_vector one = ve::fp_vector{ 1.f };

	float average_expected_savings = expected_savings_per_capita(state);

	sanity_check(state);

	/* initialization parallel block */

	concurrency::parallel_for(0, 10, [&](int32_t index) {
		switch(index) {
		case 0:
			populate_army_consumption(state);
			break;
		case 1:
			populate_navy_consumption(state);
			break;
		case 2:
			populate_private_construction_consumption(state);
			break;
		case 3:
			update_factory_triggered_modifiers(state);
			break;
		case 4:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_market([&](auto nids) {
					state.world.market_set_everyday_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 5:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_market([&](auto nids) {
					state.world.market_set_luxury_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 6:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_market([&](auto nids) {
					state.world.market_set_life_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 7:
			state.world.execute_serial_over_nation([&](auto ids) {
				state.world.nation_set_subsidies_spending(ids, 0.0f);
			});
			break;
		case 8:
			state.world.execute_serial_over_nation([&](auto ids) {
				auto treasury = state.world.nation_get_stockpiles(ids, economy::money);
				state.world.nation_set_last_treasury(ids, treasury);
			});
			break;
		}
	});

	populate_construction_consumption(state);

	sanity_check(state);

	/* end initialization parallel block */

	auto const num_nation = state.world.nation_size();
	uint32_t total_commodities = state.world.commodity_size();

	/*
		update scoring for provinces
	*/

	update_land_ownership(state);
	update_local_subsistence_factor(state);

	sanity_check(state);

	// update rgo employment before zeroing demand/supply

	// note: markets are independent, so nations are independent:
	// so we can execute in parallel over nations but not over provinces

	state.world.execute_parallel_over_nation([&](auto nations) {
		ve::apply([&](dcon::nation_id n) {
			// STEP 3 update local rgo employment:
			state.world.nation_for_each_province_ownership_as_nation(n, [&](dcon::province_ownership_id poid) {
				auto p = state.world.province_ownership_get_province(poid);
				bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));
				auto state_instance = state.world.province_get_state_membership(p);
				auto market = state_instance.get_market_from_local_market();
				auto n = state_instance.get_nation_from_state_ownership();
				auto min_wage_factor = pop_min_wage_factor(state, n);
				auto pop_farmer_min_wage = farmer_min_wage(state, market, min_wage_factor);
				auto pop_laborer_min_wage = laborer_min_wage(state, market, min_wage_factor);

				update_province_rgo_employment(
					state,
					p,
					market,
					n,
					military::mobilization_impact(state, n),
					is_mine ? pop_laborer_min_wage : pop_farmer_min_wage
				);
			});
		}, nations);
	});

	sanity_check(state);

	auto coastal_capital_buffer = ve::vectorizable_buffer<dcon::province_id, dcon::state_instance_id>(state.world.state_instance_size());

	state.world.execute_parallel_over_state_instance([&](auto ids) {
		ve::apply([&](auto sid) {
			coastal_capital_buffer.set(sid, province::state_get_coastal_capital(state, sid));
		}, ids);
	});

	//static auto export_tariff = state.world.nation_make_vectorizable_float_buffer();
	//static auto import_tariff = state.world.nation_make_vectorizable_float_buffer();

	static auto tariff_buffer = state.world.nation_make_vectorizable_float_buffer();

	state.world.execute_parallel_over_nation([&](auto ids) {
		ve::apply([&](auto nid) {
			tariff_buffer.set(nid, effective_tariff_rate(state, nid));
		}, ids);
	});

	// update trade volume based on potential profits right at the start
	// we can't put it between demand and supply generation!
	concurrency::parallel_for(uint32_t(0), total_commodities, [&](uint32_t k) {
		dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };

		if(state.world.commodity_get_money_rgo(c)) {
			return;
		}

		state.world.execute_serial_over_trade_route([&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, c);
			auto A = ve::apply([&](auto route) {
				return state.world.trade_route_get_connected_markets(route, 0);
			}, trade_route);

			auto B = ve::apply([&](auto route) {
				return state.world.trade_route_get_connected_markets(route, 1);
			}, trade_route);

			auto absolute_volume = ve::abs(current_volume);
			//auto sat = state.world.market_get_direct_demand_satisfaction(origin, c);

			auto s_A = state.world.market_get_zone_from_local_market(A);
			auto s_B = state.world.market_get_zone_from_local_market(B);

			auto n_A = state.world.state_instance_get_nation_from_state_ownership(s_A);
			auto n_B = state.world.state_instance_get_nation_from_state_ownership(s_B);

			auto at_war = ve::apply([&](auto n_a, auto n_b) {
				return military::are_at_war(state, n_a, n_b);
			}, n_A, n_B);

			auto sphere_A = state.world.nation_get_in_sphere_of(n_A);
			auto sphere_B = state.world.nation_get_in_sphere_of(n_B);
			
			auto is_A_civ = state.world.nation_get_is_civilized(n_A);
			auto is_B_civ = state.world.nation_get_is_civilized(n_B);

			auto is_sea_route = state.world.trade_route_get_is_sea_route(trade_route);
			auto is_land_route = state.world.trade_route_get_is_land_route(trade_route);

			auto port_A = coastal_capital_buffer.get(s_A);
			auto port_B = coastal_capital_buffer.get(s_B);

			auto is_A_blockaded = state.world.province_get_is_blockaded(port_A);
			auto is_B_blockaded = state.world.province_get_is_blockaded(port_B);

			is_sea_route = is_sea_route & !is_A_blockaded & !is_B_blockaded;

			auto same_nation = n_A == n_B;
			auto same_sphere = (n_A == sphere_B) || (n_B == sphere_A) || (sphere_A == sphere_B);

			auto merchant_cut = ve::select(same_nation, ve::fp_vector{ 1.001f }, ve::fp_vector{ 1.05f });

			auto import_tariff_A = ve::select(same_nation || same_sphere, ve::fp_vector{ 0.f }, tariff_buffer.get(n_A));
			auto export_tariff_A = ve::select(same_nation || same_sphere, ve::fp_vector{ 0.f }, tariff_buffer.get(n_A));
			auto import_tariff_B = ve::select(same_nation || same_sphere, ve::fp_vector{ 0.f }, tariff_buffer.get(n_B));
			auto export_tariff_B = ve::select(same_nation || same_sphere, ve::fp_vector{ 0.f }, tariff_buffer.get(n_B));

			ve::fp_vector distance = 999999.f;
			auto land_distance = state.world.trade_route_get_land_distance(trade_route);
			auto sea_distance = state.world.trade_route_get_sea_distance(trade_route);

			distance = ve::select(is_land_route, ve::min(distance, land_distance), distance);
			distance = ve::select(is_sea_route, ve::min(distance, sea_distance), distance);

			state.world.trade_route_set_distance(trade_route, distance);

			auto trade_good_loss_mult = ve::max(0.f, 1.f - 0.0001f * distance);

			// todo: transport cost should be variable?
			auto transport_cost = distance * 0.05f;

			// effect of scale
			// volume reduces transport costs
			auto effect_of_scale = ve::max(0.1f, 1.f - absolute_volume * 0.0005f);

			auto price_A_export = ve_price(state, A, c) * (1.f + export_tariff_A);
			auto price_B_export = ve_price(state, B, c) * (1.f + export_tariff_B);

			auto price_A_import = ve_price(state, A, c) * (1.f - import_tariff_A) * trade_good_loss_mult;
			auto price_B_import = ve_price(state, B, c) * (1.f - import_tariff_B) * trade_good_loss_mult;

			auto current_profit_A_to_B = price_B_import - price_A_export * merchant_cut - transport_cost * effect_of_scale;
			auto current_profit_B_to_A = price_A_import - price_B_export * merchant_cut - transport_cost * effect_of_scale;

			auto none_is_profiable = (current_profit_A_to_B <= 0.f) && (current_profit_B_to_A <= 0.f);

			auto max_change = 0.1f + absolute_volume * 0.1f;
			auto change = ve::select(current_profit_A_to_B > 0.f, current_profit_A_to_B / price_A_export, 0.f);
			change = ve::select(current_profit_B_to_A > 0.f, -current_profit_B_to_A / price_B_export, change);
			change = ve::min(ve::max(change, -max_change), max_change);
			change = ve::select(none_is_profiable, -current_volume, change);
			change = ve::select(at_war, -current_volume, change);

			// trade slowly decays to create soft limit on transportation
			// essentially, regularisation of trade weights
			ve::fp_vector decay = 0.99f;

			// dirty, embarassing and disgusting hack
			// to avoid trade generating too much demand
			// on already expensive goods
			// but it works well
			decay = ve::select(current_volume > 0.f, decay * ve::min(1.f, 10000.f / price_A_export), decay * ve::min(1.f, 10000.f / price_B_export));
			state.world.trade_route_set_volume(trade_route, c, (current_volume + change) * decay);

			ve::apply([&](auto route) {
				assert(std::isfinite(state.world.trade_route_get_volume(route, c)));
			}, trade_route);
		});
	});

	sanity_check(state);

	// normalize with max_throughput

	state.world.for_each_market([&](auto market) {
		auto total = 0.f;
		auto max = state.world.market_get_max_throughput(market);

		state.world.for_each_commodity([&](auto commodity) {
			state.world.market_for_each_trade_route(market, [&](auto trade_route) {
				total += std::abs(state.world.trade_route_get_volume(trade_route, commodity));
			});
		});

		if(total > max) {
			state.world.for_each_commodity([&](auto commodity) {
				state.world.market_for_each_trade_route(market, [&](auto trade_route) {
					auto now = state.world.trade_route_get_volume(trade_route, commodity);
					state.world.trade_route_set_volume(trade_route, commodity, now * max / total);
				});
			});
		}
	});

	sanity_check(state);

	static ve::vectorizable_buffer<float, dcon::nation_id> invention_count = state.world.nation_make_vectorizable_float_buffer();
	state.world.execute_serial_over_nation([&](auto nations) {
		invention_count.set(nations, 0.f);
	});

	sanity_check(state);

	state.world.for_each_invention([&](auto iid) {
		state.world.execute_serial_over_nation([&](auto nations) {
			auto count =
				invention_count.get(nations)
				+ ve::select(state.world.nation_get_active_inventions(nations, iid), one, zero);
			invention_count.set(nations, count);
		});
	});

	state.world.execute_parallel_over_market([&](auto markets) {
		// reset gdp
		state.world.market_set_gdp(markets, 0.f);

		auto states = state.world.market_get_zone_from_local_market(markets);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);

		auto invention_factor = invention_count.get(nations) * state.defines.invention_impact_on_demand + 1.0f;

		// STEP 0: gather total demand costs:
		/*
		- Each pop strata and needs type has its own demand modifier, calculated as follows:
		- (national-modifier-to-goods-demand + define:BASE_GOODS_DEMAND) x (national-modifier-to-specific-strata-and-needs-type + 1) x
		(define:INVENTION_IMPACT_ON_DEMAND x number-of-unlocked-inventions + 1, but for non-life-needs only)
		- Each needs demand is also multiplied by  2 - the nation's administrative efficiency if the pop has education / admin /
		military income for that need category
		- We calculate an adjusted pop-size as (0.5 + pop-consciousness / define:PDEF_BASE_CON) x (for non-colonial pops: 1 +
		national-plurality (as a fraction of 100)) x pop-size
		*/

		uint32_t total_commodities = state.world.commodity_size();

		// poor strata update

		{
			uint8_t strata_filter = 0;
			auto offset_life = sys::national_mod_offsets::poor_life_needs;
			auto offset_everyday = sys::national_mod_offsets::poor_everyday_needs;
			auto offset_luxury = sys::national_mod_offsets::poor_luxury_needs;

			auto life_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_life) + 1.0f)
				* state.defines.alice_lf_needs_scale;
			auto everyday_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_everyday) + 1.0f)
				* state.defines.alice_ev_needs_scale;
			auto luxury_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_luxury) + 1.0f)
				* state.defines.alice_lx_needs_scale;

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };

				auto prices = state.world.market_get_price(markets, c);

				auto available = state.world.commodity_get_is_available_from_start(c);
				auto is_active = state.world.nation_get_unlocked_commodities(nations, c);
				auto life_weights = state.world.market_get_life_needs_weights(markets, c);
				auto everyday_weights = state.world.market_get_everyday_needs_weights(markets, c);
				auto luxury_weights = state.world.market_get_luxury_needs_weights(markets, c);

				state.world.for_each_pop_type([&](dcon::pop_type_id pop_type) {
					if(state.world.pop_type_get_strata(pop_type) != strata_filter) {
						return;
					}

					auto life_base = state.world.pop_type_get_life_needs(pop_type, c);
					auto everyday_base = state.world.pop_type_get_everyday_needs(pop_type, c);
					auto luxury_base = state.world.pop_type_get_luxury_needs(pop_type, c);

					auto life_costs = prices * life_base * life_needs_mult * life_weights;
					auto everyday_costs = prices * everyday_base * everyday_needs_mult * everyday_weights * invention_factor;
					auto luxury_costs = prices * luxury_base * luxury_needs_mult * luxury_weights * invention_factor;

					state.world.market_set_life_needs_costs(
						markets,
						pop_type,
						state.world.market_get_life_needs_costs(markets, pop_type) + life_costs
					);
					state.world.market_set_everyday_needs_costs(
						markets,
						pop_type,
						state.world.market_get_everyday_needs_costs(markets, pop_type) + everyday_costs
					);
					state.world.market_set_luxury_needs_costs(
						markets,
						pop_type,
						state.world.market_get_luxury_needs_costs(markets, pop_type) + luxury_costs
					);

					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_life_needs_costs(markets, pop_type)
							);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_everyday_needs_costs(markets, pop_type)
							);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_luxury_needs_costs(markets, pop_type)
							);
				});
			}
		}

		// middle strata update

		{
			uint8_t strata_filter = 1;
			auto offset_life = sys::national_mod_offsets::middle_life_needs;
			auto offset_everyday = sys::national_mod_offsets::middle_everyday_needs;
			auto offset_luxury = sys::national_mod_offsets::middle_luxury_needs;

			auto life_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_life) + 1.0f)
				* state.defines.alice_lf_needs_scale;
			auto everyday_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_everyday) + 1.0f)
				* state.defines.alice_ev_needs_scale;
			auto luxury_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_luxury) + 1.0f)
				* state.defines.alice_lx_needs_scale;

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };

				auto prices = state.world.market_get_price(markets, c);

				auto available = state.world.commodity_get_is_available_from_start(c);
				auto is_active = state.world.nation_get_unlocked_commodities(nations, c);
				auto life_weights = state.world.market_get_life_needs_weights(markets, c);
				auto everyday_weights = state.world.market_get_everyday_needs_weights(markets, c);
				auto luxury_weights = state.world.market_get_luxury_needs_weights(markets, c);

				state.world.for_each_pop_type([&](dcon::pop_type_id pop_type) {
					if(state.world.pop_type_get_strata(pop_type) != strata_filter) {
						return;
					}

					auto life_base = state.world.pop_type_get_life_needs(pop_type, c);
					auto everyday_base = state.world.pop_type_get_everyday_needs(pop_type, c);
					auto luxury_base = state.world.pop_type_get_luxury_needs(pop_type, c);

					auto life_costs = prices * life_base * life_needs_mult * life_weights;
					auto everyday_costs = prices * everyday_base * everyday_needs_mult * everyday_weights * invention_factor;
					auto luxury_costs = prices * luxury_base * luxury_needs_mult * luxury_weights * invention_factor;

					state.world.market_set_life_needs_costs(
						markets,
						pop_type,
						state.world.market_get_life_needs_costs(markets, pop_type) + life_costs
					);
					state.world.market_set_everyday_needs_costs(
						markets,
						pop_type,
						state.world.market_get_everyday_needs_costs(markets, pop_type) + everyday_costs
					);
					state.world.market_set_luxury_needs_costs(
						markets,
						pop_type,
						state.world.market_get_luxury_needs_costs(markets, pop_type) + luxury_costs
					);

					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_life_needs_costs(markets, pop_type)
							);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_everyday_needs_costs(markets, pop_type)
							);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_luxury_needs_costs(markets, pop_type)
							);
				});
			}
		}

		// rich strata update

		{
			uint8_t strata_filter = 2;
			auto offset_life = sys::national_mod_offsets::rich_life_needs;
			auto offset_everyday = sys::national_mod_offsets::rich_everyday_needs;
			auto offset_luxury = sys::national_mod_offsets::rich_luxury_needs;

			auto life_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_life) + 1.0f)
				* state.defines.alice_lf_needs_scale;
			auto everyday_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_everyday) + 1.0f)
				* state.defines.alice_ev_needs_scale;
			auto luxury_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_luxury) + 1.0f)
				* state.defines.alice_lx_needs_scale;

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };

				auto prices = state.world.market_get_price(markets, c);

				auto available = state.world.commodity_get_is_available_from_start(c);
				auto is_active = state.world.nation_get_unlocked_commodities(nations, c);
				auto life_weights = state.world.market_get_life_needs_weights(markets, c);
				auto everyday_weights = state.world.market_get_everyday_needs_weights(markets, c);
				auto luxury_weights = state.world.market_get_luxury_needs_weights(markets, c);

				state.world.for_each_pop_type([&](dcon::pop_type_id pop_type) {
					if(state.world.pop_type_get_strata(pop_type) != strata_filter) {
						return;
					}

					auto life_base = state.world.pop_type_get_life_needs(pop_type, c);
					auto everyday_base = state.world.pop_type_get_everyday_needs(pop_type, c);
					auto luxury_base = state.world.pop_type_get_luxury_needs(pop_type, c);

					auto life_costs = prices * life_base * life_needs_mult * life_weights;
					auto everyday_costs = prices * everyday_base * everyday_needs_mult * everyday_weights * invention_factor;
					auto luxury_costs = prices * luxury_base * luxury_needs_mult * luxury_weights * invention_factor;

					state.world.market_set_life_needs_costs(
						markets,
						pop_type,
						state.world.market_get_life_needs_costs(markets, pop_type) + life_costs
					);
					state.world.market_set_everyday_needs_costs(
						markets,
						pop_type,
						state.world.market_get_everyday_needs_costs(markets, pop_type) + everyday_costs
					);
					state.world.market_set_luxury_needs_costs(
						markets,
						pop_type,
						state.world.market_get_luxury_needs_costs(markets, pop_type) + luxury_costs
					);


					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_life_needs_costs(markets, pop_type)
							);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_everyday_needs_costs(markets, pop_type)
							);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_luxury_needs_costs(markets, pop_type)
							);
				});
			}
		}

		auto mobilization_impact =
			ve::select(
				state.world.nation_get_is_mobilized(nations),
				military::ve_mobilization_impact(state, nations), 1.f
			);

		auto min_wage_factor = ve_pop_min_wage_factor(state, nations);
		auto artisan_min_wage = ve_artisan_min_wage(state, markets);
		auto factory_min_wage = ve_factory_min_wage(state, markets, states, min_wage_factor);

		// clear real demand
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			state.world.market_set_demand(markets, c, ve::fp_vector{});
			state.world.market_set_intermediate_demand(markets, c, ve::fp_vector{});
		});

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto demand = state.world.market_get_demand(markets, c);
		});

		// STEP 1: artisans consumption update:
		update_artisan_consumption(state, markets, nations, states, artisan_min_wage, mobilization_impact);

		// STEP 2: update local factories consumption:
		ve::apply(
			[&](
				dcon::state_instance_id s,
				dcon::market_id m,
				dcon::nation_id n,
				float mob_impact,
				float min_wage
				) {
					auto capital = state.world.state_instance_get_capital(s);
					province::for_each_province_in_state_instance(state, s, [&](auto p) {
						for(auto f : state.world.province_get_factory_location(p)) {
							update_single_factory_consumption(
								state,
								f.get_factory(),
								p,
								s,
								m,
								n,
								mob_impact,
								min_wage,
								state.world.province_get_nation_from_province_control(p) != n // is occupied
							);
						}
					});
				}, states, markets, nations, mobilization_impact, factory_min_wage
		);
	});

	sanity_check(state);

	// RGO do not consume anything... yet

	// STEP 3 update pops consumption:

	// reset data first:
	state.world.for_each_pop_type([&](auto pt) {
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_life_needs_scale(ids, pt, 0.f);
			state.world.market_set_everyday_needs_scale(ids, pt, 0.f);
			state.world.market_set_luxury_needs_scale(ids, pt, 0.f);
		});
	});

	sanity_check(state);

	update_pop_consumption(state, invention_count);

	sanity_check(state);

	// STEP 4 national budget updates
	for(auto n : state.nations_by_rank) {
		auto cap_prov = state.world.nation_get_capital(n);
		auto cap_continent = state.world.province_get_continent(cap_prov);
		auto cap_region = state.world.province_get_connected_region_id(cap_prov);
		{
			// update national spending
			//
			// step 1: figure out total
			float total = full_spending_cost(state, n);

			// step 2: limit to actual budget
			float budget = 0.0f;
			float spending_scale = 0.0f;
			if(state.world.nation_get_is_player_controlled(n)) {
				auto& sp = state.world.nation_get_stockpiles(n, economy::money);

				/*
				BANKRUPTCY
				*/
				auto ip = interest_payment(state, n);
				// To become bankrupt nation should be unable to cover its interest payments with its actual money or more loans
				if(sp < ip && state.world.nation_get_local_loan(n) >= max_loan(state, n)) {
					go_bankrupt(state, n);
				}
				if(ip > 0) {
					sp -= ip;
					state.world.nation_get_national_bank(n) += ip;
				}

				// If available loans don't allow run 100% of spending, adjust spending scale
				if(can_take_loans(state, n) && total - state.world.nation_get_stockpiles(n, economy::money) <= max_loan(state, n) - state.world.nation_get_local_loan(n)) {
					budget = total;
					spending_scale = 1.0f;
				} else {
					budget = std::max(0.0f, state.world.nation_get_stockpiles(n, economy::money));
					spending_scale = (total < 0.001f || total <= budget) ? 1.0f : budget / total;
				}
			} else {
				budget = std::max(0.0f, state.world.nation_get_stockpiles(n, economy::money));
				spending_scale = (total < 0.001f || total <= budget) ? 1.0f : budget / total;

				auto& sp = state.world.nation_get_stockpiles(n, economy::money);

				if(sp < 0 && sp < -max_loan(state, n)) {
					go_bankrupt(state, n);
				}
			}

			assert(spending_scale >= 0);
			assert(std::isfinite(spending_scale));
			assert(std::isfinite(budget));

			state.world.nation_get_stockpiles(n, economy::money) -= std::min(budget, total * spending_scale);
			state.world.nation_set_spending_level(n, spending_scale);

			auto s = state.world.nation_get_stockpiles(n, economy::money);
			auto l = state.world.nation_get_local_loan(n);
			if(s < 0 && l < max_loan(state, n) &&
				std::abs(s) <= max_loan(state, n) - l) {
				state.world.nation_get_local_loan(n) += std::abs(s);
				state.world.nation_set_stockpiles(n, economy::money, 0);
			}
			else if (s < 0) {
				// Nation somehow got into negative bigger than its loans allow
				go_bankrupt(state, n);
			}
			else if(s > 0 && l > 0) {
				auto change = std::min(s, l);
				state.world.nation_get_local_loan(n) -= change;
				state.world.nation_get_stockpiles(n, economy::money) -= change;
			}

			float pi_total = full_private_investment_cost(state, n);
			float perceived_spending = pi_total;
			float pi_budget = state.world.nation_get_private_investment(n);
			auto pi_scale = perceived_spending <= pi_budget ? 1.0f : pi_budget / perceived_spending;
			//cut away low values:
			pi_scale = std::max(0.f, pi_scale - 0.1f);
			state.world.nation_set_private_investment_effective_fraction(n, pi_scale);
			state.world.nation_set_private_investment(n, std::max(0.0f, pi_budget - pi_total * pi_scale));

			update_national_consumption(state, n, spending_scale, pi_scale);
		}
	}

	sanity_check(state);

	// register trade demand
	concurrency::parallel_for(uint32_t(0), total_commodities, [&](uint32_t k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };

		if(state.world.commodity_get_money_rgo(cid)) {
			return;
		}

		state.world.for_each_trade_route([&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
			auto origin =
				current_volume > 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);
			auto target =
				current_volume <= 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);

			auto trade_loss = 0.8f;

			auto sat = state.world.market_get_direct_demand_satisfaction(origin, cid);

			//reduce volume in case of low supply
			current_volume = current_volume * std::max(0.99f, sat);
			state.world.trade_route_set_volume(trade_route, cid, current_volume);

			auto absolute_volume = std::abs(current_volume);

			auto s_origin = state.world.market_get_zone_from_local_market(origin);
			auto s_target = state.world.market_get_zone_from_local_market(target);

			auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
			auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

			register_demand(state, origin, cid, absolute_volume, economy_reason::trade);
		});
	});

	sanity_check(state);

	/*
	perform actual consumption / purchasing subject to availability at markets:
	*/

	state.world.execute_parallel_over_market([&](auto ids) {
		auto zones = state.world.market_get_zone_from_local_market(ids);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(zones);
		auto capital = state.world.nation_get_capital(nations);
		auto capital_states = state.world.province_get_state_membership(capital);
		auto capital_mask = capital_states == zones;
		auto income_scale = state.world.market_get_income_scale(ids);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };

			// we do not actually consume/purchase money rgos
			if(state.world.commodity_get_money_rgo(c)) {
				continue;
			}

			auto draw_from_stockpile = state.world.nation_get_drawing_on_stockpiles(nations, c) == true;

			// thankfully with local economies
			// there is no multiple layers of pools nonsense and
			// we can update markets in a really simple way:

			auto production = state.world.market_get_supply(ids, c);
			auto stockpiles = state.world.market_get_stockpile(ids, c);
			// we draw from stockpile in capital
			auto national_stockpile = ve::select(
				capital_mask && draw_from_stockpile,
				state.world.nation_get_stockpiles(nations, c),
				0.f
			);
			auto total_supply = production + national_stockpile;
			auto demand = state.world.market_get_demand(ids, c);
			auto old_saturation = state.world.market_get_demand_satisfaction(ids, c);
			auto new_saturation = ve::select(demand == 0.f, 0.f, total_supply / demand);
			new_saturation = ve::min(new_saturation, 1.f);

			auto delayed_saturation =
				old_saturation * state.defines.alice_sat_delay_factor
				+ new_saturation * (1.f - state.defines.alice_sat_delay_factor);

			state.world.market_set_demand_satisfaction(ids, c, delayed_saturation);
			state.world.market_set_consumption(ids, c, delayed_saturation * demand);
			state.world.market_set_direct_demand_satisfaction(ids, c, new_saturation);

			// we bought something locally

			// one of them is 0

			demand = demand - new_saturation * demand;
			total_supply = total_supply - new_saturation * demand;

			auto wealth = state.world.market_get_stockpile(ids, economy::money);

			// now we buy/sell from/to local stockpile first:
			// limit change to avoid negative amount of goods
			auto stockpile_change = ve::max(-stockpiles, total_supply - demand);

			// might be negative
			auto change_cost = stockpile_change * ve_price(state, ids, c) * state.inflation * income_scale;

			// transaction
			// local money stockpile might go negative:
			// traders can take loans after all
			// our balance goal is to make sure that money trajectory is sane
			// there should be no +- inf in money stockpiles

			demand = ve::max(
				0.f,
				demand + ve::select(stockpile_change < 0.f, stockpile_change, 0.f)
			);

			//decay stockpiles
			state.world.market_set_stockpile(ids, c, (stockpiles + stockpile_change) * 0.99f);
			// during presimulation
			// we try to figure out
			// how much money we need to have in our markets
			if(presimulation)
				state.world.market_set_stockpile(ids, economy::money, wealth * 0.999f + ve::abs(change_cost));
			else
				state.world.market_set_stockpile(ids, economy::money, wealth - change_cost);

			// then we siphon from national stockpile:
			// there is only one capital in a country!,
			// which means that we can safely pay back for siphoned stockpile
			// and change national stockpile at the same time
			auto buy_from_nation = ve::min(national_stockpile, demand);
			auto bought_from_nation_cost =
				buy_from_nation * ve_price(state, ids, c) * state.inflation * income_scale;
			state.world.nation_set_stockpiles(nations, c, national_stockpile - buy_from_nation);
			auto treasury = state.world.nation_get_stockpiles(nations, economy::money);
			state.world.nation_set_stockpiles(
				nations, economy::money, treasury + bought_from_nation_cost);
		}
	});

	sanity_check(state);

	/*
	pay non "employed" pops (also zeros money for "employed" pops)
	*/

	state.world.execute_parallel_over_pop([&](auto ids) {
		auto owners = nations::owner_of_pop(state, ids);
		auto provs = state.world.pop_get_province_from_pop_location(ids);
		auto states = state.world.province_get_state_membership(provs);
		auto markets = state.world.state_instance_get_market_from_local_market(states);
		auto income_scale = state.world.market_get_income_scale(markets);
		auto owner_spending = state.world.nation_get_spending_level(owners);

		auto pop_savings = state.world.pop_get_savings(ids);

		auto pop_of_type = state.world.pop_get_size(ids);
		auto adj_pop_of_type = pop_of_type / state.defines.alice_needs_scaling_factor;

		auto const a_spending = owner_spending * ve::to_float(state.world.nation_get_administrative_spending(owners)) * ve::to_float(state.world.nation_get_administrative_spending(owners)) / 100.0f / 100.f;
		auto const s_spending = owner_spending * state.world.nation_get_administrative_efficiency(owners) *
			ve::to_float(state.world.nation_get_social_spending(owners)) / 100.0f;
		auto const e_spending = owner_spending * ve::to_float(state.world.nation_get_education_spending(owners)) * ve::to_float(state.world.nation_get_education_spending(owners)) / 100.0f / 100.f;
		auto const m_spending = owner_spending * ve::to_float(state.world.nation_get_military_spending(owners)) * ve::to_float(state.world.nation_get_military_spending(owners)) / 100.0f / 100.0f;
		auto const p_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::pension_level);
		auto const unemp_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::unemployment_benefit);
		auto const di_level = owner_spending * ve::to_float(state.world.nation_get_domestic_investment_spending(owners)) * ve::to_float(state.world.nation_get_domestic_investment_spending(owners)) / 100.0f / 100.f;

		auto types = state.world.pop_get_poptype(ids);

		auto ln_types = state.world.pop_type_get_life_needs_income_type(types);
		auto en_types = state.world.pop_type_get_everyday_needs_income_type(types);
		auto lx_types = state.world.pop_type_get_luxury_needs_income_type(types);

		auto ln_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_life_needs_costs(n, pt) : 0.0f; },
				types, markets);
		auto en_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_everyday_needs_costs(n, pt) : 0.0f; },
				types, markets);
		auto lx_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_luxury_needs_costs(n, pt) : 0.0f; },
				types, markets);

		auto total_costs = ln_costs + en_costs + lx_costs;

		auto acc_a =
			ve::select(ln_types == int32_t(culture::income_type::administration), a_spending * adj_pop_of_type * ln_costs, 0.0f);
		auto acc_e = ve::select(ln_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * ln_costs, 0.0f);
		auto acc_m = ve::select(ln_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * ln_costs, 0.0f);

		auto none_of_above = ln_types != int32_t(culture::income_type::military) &&
			ln_types != int32_t(culture::income_type::education) &&
			ln_types != int32_t(culture::income_type::administration);

		auto acc_u = ve::select(none_of_above, s_spending * adj_pop_of_type * p_level * ln_costs, 0.0f);

		acc_a = acc_a + ve::select(en_types == int32_t(culture::income_type::administration), a_spending * adj_pop_of_type * en_costs, 0.0f);
		acc_e = acc_e + ve::select(en_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * en_costs, 0.0f);
		acc_m = acc_m + ve::select(en_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * en_costs, 0.0f);

		acc_u = acc_u + ve::select(types == state.culture_definitions.capitalists, di_level * adj_pop_of_type * state.defines.alice_domestic_investment_multiplier * total_costs, 0.0f);
		acc_u = acc_u + ve::select(types == state.culture_definitions.aristocrat, di_level * adj_pop_of_type * state.defines.alice_domestic_investment_multiplier * total_costs, 0.0f);

		acc_a = acc_a + ve::select(lx_types == int32_t(culture::income_type::administration), a_spending * adj_pop_of_type * lx_costs, 0.0f);
		acc_e = acc_e + ve::select(lx_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * lx_costs, 0.0f);
		acc_m = acc_m + ve::select(lx_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * lx_costs, 0.0f);

		auto employment = pop_demographics::get_employment(state, ids);

		acc_u = acc_u + ve::select(none_of_above && state.world.pop_type_get_has_unemployment(types),
												s_spending * (pop_of_type - employment) / state.defines.alice_needs_scaling_factor * unemp_level * ln_costs, 0.0f);

		ve::fp_vector base_income{};
		if(presimulation) {
			base_income = pop_of_type * average_expected_savings * (1.f - presimulation_stage);
		}

		state.world.pop_set_savings(ids, (income_scale * state.inflation) * (base_income + (acc_e + acc_m) + (acc_u + acc_a)));
		state.world.pop_set_savings(ids, state.inflation * ((acc_e + acc_m) + (acc_u + acc_a)));
#ifndef NDEBUG
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_e);
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_m);
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_u);
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_a);
#endif
	});

	sanity_check(state);

	// updates of national purchases:
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		auto n = dcon::nation_id{ dcon::nation_id::value_base_t(i) };
		if(state.world.nation_get_owned_province_count(n) == 0)
			return;

		auto capital = state.world.nation_get_capital(n);
		auto capital_market = state.world.state_instance_get_market_from_local_market(
			state.world.province_get_state_membership(capital)
		);

		float base_budget = state.world.nation_get_stockpiles(n, economy::money);

		/*
		determine effective spending levels
		we iterate over all markets to gather contruction data
		*/
		auto nations_commodity_spending = state.world.nation_get_spending_level(n);
		float refund = 0.0f;
		{
			float max_sp = 0.0f;
			float total = 0.0f;
			float spending_level = float(state.world.nation_get_naval_spending(n)) / 100.0f;

			state.world.nation_for_each_state_ownership_as_nation(n, [&](dcon::state_ownership_id soid) {
				auto local_state = state.world.state_ownership_get_state(soid);
				auto local_market = state.world.state_instance_get_market_from_local_market(local_state);
				for(uint32_t k = 1; k < total_commodities; ++k) {
					dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };

					auto sat = state.world.market_get_demand_satisfaction(local_market, c);
					auto val = state.world.market_get_navy_demand(local_market, c);
					auto delta =
						val
						* (1.0f - sat)
						* nations_commodity_spending
						* spending_level
						* price(state, local_market, c);
					assert(delta >= 0.f);
					refund += delta;
					total += val;
					max_sp += val * sat;
				}
			});

			if(total > 0.f)
				max_sp /= total;
			state.world.nation_set_effective_naval_spending(
				n, nations_commodity_spending * max_sp * spending_level);
		}
		{
			float max_sp = 0.0f;
			float total = 0.0f;
			float spending_level = float(state.world.nation_get_land_spending(n)) / 100.0f;

			state.world.nation_for_each_state_ownership_as_nation(n, [&](dcon::state_ownership_id soid) {
				auto local_state = state.world.state_ownership_get_state(soid);
				auto local_market = state.world.state_instance_get_market_from_local_market(local_state);
				for(uint32_t k = 1; k < total_commodities; ++k) {
					dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };

					auto sat = state.world.market_get_demand_satisfaction(local_market, c);
					auto val = state.world.market_get_army_demand(local_market, c);
					auto delta =
						val
						* (1.0f - sat)
						* nations_commodity_spending
						* spending_level
						* price(state, local_market, c);
					assert(delta >= 0.f);
					refund += delta;
					total += val;
					max_sp += val * sat;
				}
			});
			if(total > 0.f)
				max_sp /= total;
			state.world.nation_set_effective_land_spending(
				n, nations_commodity_spending * max_sp * spending_level);
		}
		{
			state.world.nation_set_effective_construction_spending(
				n,
				nations_commodity_spending
			);
		}
		/*
		fill stockpiles from the capital market
		*/

		for(uint32_t k = 1; k < total_commodities; ++k) {
			dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };
			auto difference = state.world.nation_get_stockpile_targets(n, c) - state.world.nation_get_stockpiles(n, c);
			if(difference > 0.f && state.world.nation_get_drawing_on_stockpiles(n, c) == false) {
				auto sat = state.world.market_get_direct_demand_satisfaction(capital_market, c);
				state.world.nation_get_stockpiles(n, c) +=
					difference * nations_commodity_spending * sat;
				auto delta =
					difference
					* (1.0f - sat)
					* nations_commodity_spending
					* price(state, capital_market, c);
				assert(delta >= 0.f);
				refund += delta;
			}
		}

		/*
		calculate overseas penalty:
		ideally these goods would be bought in colonies
		but limit to capital for now
		*/

		{
			auto overseas_factor = state.defines.province_overseas_penalty
				* float(
					state.world.nation_get_owned_province_count(n)
					- state.world.nation_get_central_province_count(n)
				);
			auto overseas_budget = float(state.world.nation_get_overseas_spending(n)) / 100.f;
			auto overseas_budget_satisfaction = 1.f;

			if(overseas_factor > 0) {
				for(uint32_t k = 1; k < total_commodities; ++k) {
					dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };
					if(state.world.commodity_get_overseas_penalty(c) && valid_need(state, n, c)) {
						auto sat = state.world.market_get_demand_satisfaction(capital_market, c);
						overseas_budget_satisfaction = std::min(sat, overseas_budget_satisfaction);
						auto price_of = price(state, capital_market, c);
						auto delta = overseas_factor
							* (1.0f - sat) * nations_commodity_spending * price_of;
						assert(delta >= 0.f);
						refund += delta;
					}
				}

				state.world.nation_set_overseas_penalty(n, overseas_budget
					* overseas_budget_satisfaction);
			} else {
				state.world.nation_set_overseas_penalty(n, 1.0f);
			}
		}

		// finally, pay back refund:
		assert(std::isfinite(refund) && refund >= 0.0f);
		state.world.nation_get_stockpiles(n, money) += refund;
	});

	sanity_check(state);

	/* now we know demand satisfaction and can set actual satifaction of pops */

	/* prepare needs satisfaction caps */
	state.world.execute_parallel_over_market([&](auto ids) {
		auto states = state.world.market_get_zone_from_local_market(ids);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);

		uint32_t total_commodities = state.world.commodity_size();
		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			ve::fp_vector ln_total = 0.0f;
			ve::fp_vector en_total = 0.0f;
			ve::fp_vector lx_total = 0.0f;

			ve::fp_vector ln_max = 0.0f;
			ve::fp_vector en_max = 0.0f;
			ve::fp_vector lx_max = 0.0f;

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
				auto sat = state.world.market_get_demand_satisfaction(ids, c);
				auto valid_mask = valid_need(state, nations, c);

				auto ln_val = ve::select(valid_mask,
					state.world.pop_type_get_life_needs(pt, c)
					* state.world.market_get_life_needs_weights(ids, c), 0.f);

				ln_total = ln_total + ln_val;
				ln_max = ln_max + ln_val * sat;

				auto en_val = ve::select(valid_mask,
					state.world.pop_type_get_everyday_needs(pt, c)
					* state.world.market_get_everyday_needs_weights(ids, c), 0.f);

				en_total = en_total + en_val;
				en_max = en_max + en_val * sat;

				auto lx_val = ve::select(valid_mask,
					state.world.pop_type_get_luxury_needs(pt, c)
					* state.world.market_get_luxury_needs_weights(ids, c), 0.f);

				lx_total = lx_total + lx_val;
				lx_max = lx_max + lx_val * sat;
			}

			ln_max = ve::select(ln_total > 0.f, ln_max / ln_total, 1.f);
			en_max = ve::select(en_total > 0.f, en_max / en_total, 1.f);
			lx_max = ve::select(lx_total > 0.f, lx_max / lx_total, 1.f);

			ve::apply([](float life, float everyday, float luxury) {
				assert(life >= 0.f && life <= 1.f);
				assert(everyday >= 0.f && everyday <= 1.f);
				assert(luxury >= 0.f && luxury <= 1.f);
			}, ln_max, en_max, lx_max);

			state.world.market_set_max_life_needs_satisfaction(ids, pt, ln_max);
			state.world.market_set_max_everyday_needs_satisfaction(ids, pt, en_max);
			state.world.market_set_max_luxury_needs_satisfaction(ids, pt, lx_max);
		});
	});

	sanity_check(state);

	// finally we can move to production:
	// reset supply:

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		state.world.execute_serial_over_market([&](auto markets) {
			state.world.market_set_supply(markets, c, ve::fp_vector{});
			state.world.market_set_import(markets, c, ve::fp_vector{});
			state.world.market_set_export(markets, c, ve::fp_vector{});
		});
	});

	// we can handle each trade good separately: they do not influence each other
	// 
	// register trade supply
	concurrency::parallel_for(uint32_t(1), total_commodities, [&](uint32_t k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };

		if(state.world.commodity_get_money_rgo(cid)) {
			return;
		}

		state.world.for_each_trade_route([&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
			auto origin =
				current_volume > 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);
			auto target =
				current_volume <= 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);

			auto sat = state.world.market_get_direct_demand_satisfaction(origin, cid);
			auto absolute_volume = std::abs(current_volume);
			auto s_origin = state.world.market_get_zone_from_local_market(origin);
			auto s_target = state.world.market_get_zone_from_local_market(target);
			auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
			auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

			auto distance = state.world.trade_route_get_distance(trade_route);

			auto trade_good_loss_mult = std::max(0.f, 1.f - 0.0001f * distance);

			register_foreign_supply(state, target, cid, sat * absolute_volume * trade_good_loss_mult, economy_reason::trade);

			if(n_origin != n_target) {
				state.world.market_get_export(origin, cid) += sat * absolute_volume;
				state.world.market_get_import(target, cid) += sat * absolute_volume * trade_good_loss_mult;

				assert(std::isfinite(state.world.market_get_export(origin, cid)));
				assert(std::isfinite(state.world.market_get_import(origin, cid)));
			}
		});
	});

	sanity_check(state);

	// artisans production
	state.world.execute_serial_over_market([&](auto ids) {
		update_market_artisan_production(state, ids);
	});

	sanity_check(state);

	auto amount_of_nations = state.world.nation_size();

	for (auto n : state.world.in_nation) {
		auto const min_wage_factor = pop_min_wage_factor(state, n);

		for(auto p : state.world.nation_get_province_ownership(n)) {
			auto province = p.get_province();
			auto local_state = province.get_state_membership();
			auto market = local_state.get_market_from_local_market();

			float local_factory_min_wage = factory_min_wage(
				state, market, local_state, min_wage_factor);

			// factories production
			for(auto f : state.world.province_get_factory_location(p.get_province())) {
				update_single_factory_production(
					state,
					f.get_factory(),
					market,
					n,
					local_factory_min_wage
				);
			}

			// rgo production
			update_province_rgo_production(state, p.get_province(), market, n);

			/* adjust pop satisfaction based on consumption and subsistence */

			float subsistence = adjusted_subsistence_score(state, p.get_province());
			float subsistence_life = std::clamp(subsistence, 0.f, subsistence_score_life);
			subsistence -= subsistence_life;
			float subsistence_everyday = std::clamp(subsistence, 0.f, subsistence_score_everyday);
			subsistence -= subsistence_everyday;

			subsistence_life /= subsistence_score_life;
			subsistence_everyday /= subsistence_score_everyday;

			for(auto pl : p.get_province().get_pop_location()) {
				auto t = pl.get_pop().get_poptype();

				auto ln = pop_demographics::get_life_needs(state, pl.get_pop());
				auto en = pop_demographics::get_everyday_needs(state, pl.get_pop());
				auto lx = pop_demographics::get_luxury_needs(state, pl.get_pop());


				// sat = raw + sub ## first summand is "raw satisfaction"
				ln -= subsistence_life;
				en -= subsistence_everyday;

				// as it a very rough estimation based on very rough values,
				// we have to sanitise values:
				ln = std::max(0.f, ln);
				en = std::max(0.f, en);
				lx = std::max(0.f, lx);

				assert(std::isfinite(ln));
				assert(std::isfinite(en));
				assert(std::isfinite(lx));

				ln = std::min(ln, state.world.market_get_max_life_needs_satisfaction(market, t));
				en = std::min(en, state.world.market_get_max_everyday_needs_satisfaction(market, t));
				lx = std::min(lx, state.world.market_get_max_luxury_needs_satisfaction(market, t));

				ln += subsistence_life;
				en += subsistence_everyday;

				pop_demographics::set_life_needs(state, pl.get_pop(), ln);
				pop_demographics::set_everyday_needs(state, pl.get_pop(), en);
				pop_demographics::set_luxury_needs(state, pl.get_pop(), lx);
			}
		}

		/*
		pay "employed" pops
		*/

		{
			// ARTISAN
			auto const artisan_type = state.culture_definitions.artisans;
			auto key = demographics::to_key(state, artisan_type);

			float num_artisans = state.world.nation_get_demographics(n, key);
			if(num_artisans > 0) {
				for(auto p : state.world.nation_get_province_ownership(n)) {
					auto province = p.get_province();
					auto local_state = province.get_state_membership();
					auto market = local_state.get_market_from_local_market();
					auto income_scale = state.world.market_get_income_scale(market);
					float artisan_profit = state.world.market_get_artisan_profit(market);
					auto local_artisans = state.world.state_instance_get_demographics(local_state, key);
					if(local_artisans == 0.f) {
						continue;
					}
					auto per_profit = artisan_profit / num_artisans;
					for(auto pl : p.get_province().get_pop_location()) {
						if(artisan_type == pl.get_pop().get_poptype()) {
							pl.get_pop().set_savings(
								pl.get_pop().get_savings()
								+ income_scale * state.inflation * pl.get_pop().get_size() * per_profit);
							assert(
								std::isfinite(pl.get_pop().get_savings())
								&& pl.get_pop().get_savings() >= 0
							);
						}
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					}
				}
			}
		}

		/*
		pay factory workers / capitalists
		*/

		for(auto si : state.world.nation_get_state_ownership(n)) {
			float total_profit = 0.f;
			float rgo_owner_profit = 0.f;

			auto market = si.get_state().get_market_from_local_market();
			auto income_scale = state.world.market_get_income_scale(market);

			float local_factory_min_wage = factory_min_wage(
				state, market, si.get_state(), min_wage_factor);
			float local_farmer_min_wage = farmer_min_wage(state, market, min_wage_factor);
			float local_laborer_min_wage = laborer_min_wage(state, market, min_wage_factor);

			float num_capitalist = state.world.state_instance_get_demographics(
					si.get_state(),
					demographics::to_key(state, state.culture_definitions.capitalists)
			);

			float num_aristocrat = state.world.state_instance_get_demographics(
					si.get_state(),
					demographics::to_key(state, state.culture_definitions.aristocrat)
			);

			float num_rgo_owners = num_capitalist + num_aristocrat;

			auto capitalists_ratio = num_capitalist / (num_rgo_owners + 1.f);
			auto aristocrats_ratio = num_aristocrat / (num_rgo_owners + 1.f);

			province::for_each_province_in_state_instance(
				state, si.get_state(), [&](dcon::province_id p) {
					for(auto f : state.world.province_get_factory_location(p)) {
						total_profit += std::max(0.f, f.get_factory().get_full_profit());
					}

					{
						// FARMER / LABORER
						bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));

						auto const min_wage =
							(is_mine ? local_laborer_min_wage : local_farmer_min_wage)
							/ state.defines.alice_needs_scaling_factor;

						float total_min_to_workers = 0.0f;
						float num_workers = 0.0f;
						for(auto wt : state.culture_definitions.rgo_workers) {
							total_min_to_workers +=
								min_wage
								* state.world.province_get_demographics(
									p,
									demographics::to_employment_key(state, wt)
								);
							num_workers +=
								state.world.province_get_demographics(p, demographics::to_key(state, wt));
						}
						float total_rgo_profit = state.world.province_get_rgo_full_profit(p);
						float total_worker_wage = 0.0f;

						if(num_rgo_owners > 0) {
							// owners ALWAYS get "some" chunk of income
							rgo_owner_profit += rgo_owners_cut * total_rgo_profit;
							total_rgo_profit = (1.f - rgo_owners_cut) * total_rgo_profit;
						}

						total_worker_wage = total_rgo_profit;

						auto per_worker_profit = num_workers > 0 ? total_worker_wage / num_workers : 0.0f;

						for(auto pl : state.world.province_get_pop_location(p)) {
							if(pl.get_pop().get_poptype().get_is_paid_rgo_worker()) {
								pl.get_pop().set_savings(
									pl.get_pop().get_savings()
									+ income_scale * state.inflation * pl.get_pop().get_size() * per_worker_profit);
								assert(
									std::isfinite(pl.get_pop().get_savings())
									&& pl.get_pop().get_savings() >= 0
								);
							}
							assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
						}
					}
			});

			auto const per_rgo_owner_profit = num_rgo_owners > 0 ? rgo_owner_profit / num_rgo_owners : 0.0f;

			auto const min_wage = local_factory_min_wage / state.defines.alice_needs_scaling_factor;

			auto profit = distribute_factory_profit(state, si.get_state(), min_wage, total_profit);

			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				for(auto pl : state.world.province_get_pop_location(p)) {
					if(state.culture_definitions.primary_factory_worker == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(pl.get_pop().get_savings() + income_scale * state.inflation * pl.get_pop().get_size() * profit.per_primary_worker);
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					} else if(state.culture_definitions.secondary_factory_worker == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(pl.get_pop().get_savings() + income_scale * state.inflation * pl.get_pop().get_size() * profit.per_secondary_worker);
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					} else if(state.culture_definitions.capitalists == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(pl.get_pop().get_savings() + income_scale * state.inflation * pl.get_pop().get_size() * (profit.per_owner + per_rgo_owner_profit));
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					} else if(state.culture_definitions.aristocrat == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(pl.get_pop().get_savings() + income_scale * state.inflation * pl.get_pop().get_size() * per_rgo_owner_profit);
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					}
					assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
				}
			});
		}

		/* advance construction */
		advance_construction(state, n);

		if(presimulation) {
			emulate_construction_demand(state, n);
		}

		/* collect and distribute money for private education and other abstracted spendings */
		auto edu_money = 0.f;
		auto adm_money = 0.f;
		auto const edu_adm_spending = 0.05f;
		auto const edu_adm_effect = 1.f - edu_adm_spending;
		auto const education_ratio = 0.8f;
		auto from_investment_pool = state.world.nation_get_private_investment(n);
		state.world.nation_set_private_investment(n, from_investment_pool * 0.999f);
		from_investment_pool *= 0.001f;
		auto payment_per_pop = from_investment_pool / (1.f + state.world.nation_get_demographics(n, demographics::total));

		for(auto p : state.world.nation_get_province_ownership(n)) {
			auto province = p.get_province();
			if(state.world.province_get_nation_from_province_ownership(province) == state.world.province_get_nation_from_province_control(province)) {
				float current = 0.f;
				float local_teachers = 0.f;
				float local_managers = 0.f;
				for(auto pl : province.get_pop_location()) {

					auto pop = pl.get_pop();
					auto pt = pop.get_poptype();
					auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
					if(ln_type == culture::income_type::administration) {
						local_managers += pop.get_size();
					} else if(ln_type == culture::income_type::education) {
						local_teachers += pop.get_size();
					}

					// services/bribes/charity and other nonsense
					auto const pop_money = pop.get_savings();
					pop.set_savings(pop_money + payment_per_pop);
				}
				if(local_teachers + local_managers > 0.f) {
					for(auto pl : province.get_pop_location()) {
						auto const pop_money = pl.get_pop().get_savings();
						current += pop_money * edu_adm_spending;
						pl.get_pop().set_savings(pop_money * edu_adm_effect);
						assert(std::isfinite(pl.get_pop().get_savings()));
					}
				}
				float local_education_ratio = education_ratio;
				if(local_managers == 0.f) {
					local_education_ratio = 1.f;
				}
				for(auto pl : province.get_pop_location()) {
					auto pop = pl.get_pop();
					auto pt = pop.get_poptype();
					auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
					if(ln_type == culture::income_type::administration) {
						float ratio = (local_managers > 0.f) ? pop.get_size() / local_managers : 0.f;
						pop.set_savings(pop.get_savings() + current * (1.f - local_education_ratio) * ratio);
						assert(std::isfinite(pop.get_savings()));
						adm_money += current * (1.f - local_education_ratio) * ratio;
					} else if(ln_type == culture::income_type::education) {
						float ratio = (local_teachers > 0.f) ? pop.get_size() / local_teachers : 0.f;
						pop.set_savings(pop.get_savings() + current * local_education_ratio * ratio);
						assert(std::isfinite(pop.get_savings()));
						edu_money += current * local_education_ratio * ratio;
					}
				}
			}
		}
		state.world.nation_set_private_investment_education(n, edu_money);
		state.world.nation_set_private_investment_administration(n, adm_money);

		/*
		collect taxes
		*/

		if(false) {
			// we do not want to accumulate tons of money during presim
			state.world.for_each_nation([&](dcon::nation_id n) {
				state.world.nation_set_stockpiles(n, money, 0.f);
			});
		}

		auto const tax_eff = nations::tax_efficiency(state, n);

		float total_poor_tax_base = 0.0f;
		float total_mid_tax_base = 0.0f;
		float total_rich_tax_base = 0.0f;

		auto const poor_effect = (1.0f - tax_eff * float(state.world.nation_get_poor_tax(n)) / 100.0f);
		auto const middle_effect = (1.0f - tax_eff * float(state.world.nation_get_middle_tax(n)) / 100.0f);
		auto const rich_effect = (1.0f - tax_eff * float(state.world.nation_get_rich_tax(n)) / 100.0f);

		assert(poor_effect >= 0 && middle_effect >= 0 && rich_effect >= 0);

		for(auto p : state.world.nation_get_province_ownership(n)) {
			auto province = p.get_province();
			if(state.world.province_get_nation_from_province_ownership(province) == state.world.province_get_nation_from_province_control(province)) {
				for(auto pl : province.get_pop_location()) {
					auto& pop_money = pl.get_pop().get_savings();
					auto strata = culture::pop_strata(pl.get_pop().get_poptype().get_strata());
					if(strata == culture::pop_strata::poor) {
						total_poor_tax_base += pop_money;
						pop_money *= poor_effect;
					} else if(strata == culture::pop_strata::middle) {
						total_mid_tax_base += pop_money;
						pop_money *= middle_effect;
					} else if(strata == culture::pop_strata::rich) {
						total_rich_tax_base += pop_money;
						pop_money *= rich_effect;
					}
					assert(std::isfinite(pl.get_pop().get_savings()));
				}
			}
		}

		state.world.nation_set_total_rich_income(n, total_rich_tax_base);
		state.world.nation_set_total_middle_income(n, total_mid_tax_base);
		state.world.nation_set_total_poor_income(n, total_poor_tax_base);
		auto collected_tax = total_poor_tax_base * tax_eff * float(state.world.nation_get_poor_tax(n)) / 100.0f +
			total_mid_tax_base * tax_eff * float(state.world.nation_get_middle_tax(n)) / 100.0f +
			total_rich_tax_base * tax_eff * float(state.world.nation_get_rich_tax(n)) / 100.0f;
		assert(std::isfinite(collected_tax));
		assert(collected_tax >= 0);
		state.world.nation_get_stockpiles(n, money) += collected_tax;

		{
			/*
			collect tariffs
			*/

			auto tariff_rate = effective_tariff_rate(state, n);
			float t_total = 0.0f;

			for(auto si : state.world.nation_get_state_ownership(n)) {
				float total_profit = 0.f;
				float rgo_owner_profit = 0.f;

				auto market = si.get_state().get_market_from_local_market();
				for(uint32_t k = 1; k < total_commodities; ++k) {
					dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
					t_total +=
						state.world.market_get_price(market, cid)
						* tariff_rate
						* state.world.market_get_import(market, cid);
					t_total +=
						state.world.market_get_price(market, cid)
						* tariff_rate
						* state.world.market_get_export(market, cid);
				}
			}

			assert(std::isfinite(t_total));
			assert(t_total >= 0);
			state.world.nation_get_stockpiles(n, money) += t_total;
		}

		// shift needs weights
		for(auto si : state.world.nation_get_state_ownership(n)) {
			float total_profit = 0.f;
			float rgo_owner_profit = 0.f;

			auto market = si.get_state().get_market_from_local_market();
			rebalance_needs_weights(state, market);
		}
	};
	state.world.execute_serial_over_market([&](auto ids) {
		auto local_states = state.world.market_get_zone_from_local_market(ids);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(local_states);

		adjust_artisan_balance(state, ids, nations);
	});

	sanity_check(state);

	/*
	adjust prices based on global production & consumption
	*/
	state.world.execute_serial_over_market([&](auto ids) {
		auto income_scale = state.world.market_get_income_scale(ids);

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(!state.world.commodity_get_money_rgo(c))
				return;

			ve::fp_vector life_cost = 0.f;
			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id _cid{ dcon::commodity_id::value_base_t(i) };

				if(state.world.commodity_get_is_available_from_start(_cid)) {
					ve::fp_vector price = ve_price(state, ids, _cid);
					auto t = state.culture_definitions.laborers;
					float base_life = state.world.pop_type_get_life_needs(t, _cid);

					life_cost =
						life_cost
						+ base_life * state.defines.alice_lf_needs_scale * price;
				}
			}

			state.world.market_set_price(ids, c, life_cost * state.world.commodity_get_cost(c) / 50.f);
		});
	});

	state.world.execute_serial_over_market([&](auto ids) {
		concurrency::parallel_for(uint32_t(1), total_commodities, [&](uint32_t k) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };

			//handling gold cost separetely
			if(state.world.commodity_get_money_rgo(cid)) {
				return;
			}

			// dirty hack ...
			// for now
			// ideally simulation should stop demanding goods
			// when they are way too expensive

			ve::fp_vector supply =
				state.world.market_get_supply(ids, cid)
				+ state.world.market_get_stockpile(ids, cid) * stockpile_to_supply
				+ price_rigging + 0.0001f;
			ve::fp_vector demand = state.world.market_get_demand(ids, cid)
				+ price_rigging;

			auto base_price = state.world.commodity_get_cost(cid);
			auto current_price = ve_price(state, ids, cid);
			auto market_balance = demand - supply;
			auto oversupply_factor = ve::max(supply / demand - 1.f, 0.f);
			auto overdemand_factor = ve::max(demand / supply - 1.f, 0.f);
			auto speed_modifer = (overdemand_factor - oversupply_factor);
			auto price_speed = 0.0001f * speed_modifer;
			price_speed = price_speed * current_price;
			current_price = current_price + price_speed;

#ifndef NDEBUG
			ve::apply([&](auto value) { assert(std::isfinite(value)); }, current_price);
#endif

			//the only purpose of upper price bound is to prevent float overflow
			state.world.market_set_price(ids, cid, ve::min(ve::max(current_price, 0.001f), 1'000'000'000'000.f));
		});
	});

	sanity_check(state);

	if(state.cheat_data.ecodump) {
		float accumulator[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			float states_count = 0.f;
			float total_price = 0.f;
			float total_production = 0.f;
			float total_demand = 0.f;

			state.world.for_each_market([&](auto id) {
				states_count++;
				total_price += state.world.market_get_price(id, c);
				total_production += state.world.market_get_supply(id, c);
				total_demand += state.world.market_get_demand(id, c);
			});

			state.cheat_data.prices_dump_buffer += std::to_string(total_price / states_count) + ",";
			state.cheat_data.supply_dump_buffer += std::to_string(total_production) + ",";
			state.cheat_data.demand_dump_buffer += std::to_string(total_demand) + ",";
		});

		state.cheat_data.prices_dump_buffer += "\n";
		state.cheat_data.supply_dump_buffer += "\n";
		state.cheat_data.demand_dump_buffer += "\n";
	}


	/*
	DIPLOMATIC EXPENSES
	*/

	for(auto n : state.world.in_nation) {
		// Subject money transfers
		auto rel = state.world.nation_get_overlord_as_subject(n);
		auto overlord = state.world.overlord_get_ruler(rel);

		if(overlord) {
			auto transferamt = estimate_subject_payments_paid(state, n);
			state.world.nation_get_stockpiles(n, money) -= transferamt;
			state.world.nation_get_stockpiles(overlord, money) += transferamt;
		}

		for(auto uni : n.get_unilateral_relationship_as_source()) {
			if(uni.get_war_subsidies()) {
				auto sub_size = estimate_war_subsidies(state, uni.get_target(), uni.get_source());

				if(sub_size <= n.get_stockpiles(money)) {
					n.get_stockpiles(money) -= sub_size;
					uni.get_target().get_stockpiles(money) += sub_size;
				} else {
					uni.set_war_subsidies(false);

					notification::post(state, notification::message{
						[source = n.id, target = uni.get_target().id](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_wsub_end_1", text::variable_type::x, source, text::variable_type::y, target);
						},
						"msg_wsub_end_title",
						n.id, uni.get_target().id, dcon::nation_id{},
						sys::message_base_type::war_subsidies_end
					});
				}
			}
			if(uni.get_reparations() && state.current_date < n.get_reparations_until()) {
				auto const tax_eff = nations::tax_efficiency(state, n);
				auto total_tax_base = n.get_total_rich_income() + n.get_total_middle_income() + n.get_total_poor_income();

				auto payout = total_tax_base * tax_eff * state.defines.reparations_tax_hit;
				auto capped_payout = std::min(n.get_stockpiles(money), payout);
				assert(capped_payout >= 0.0f);

				n.get_stockpiles(money) -= capped_payout;
				uni.get_target().get_stockpiles(money) += capped_payout;
			}
		}
	}

	sanity_check(state);

	/*
	update inflation
	*/

	// control over money mass is two-step
	// firstly we control global amount of money with "inflation": global income scaler
	// secondly we control local amount of money with local income scalers
	// we want to smooth out absurd "spikes"
	// in per capita money distributions of markets

	float total_pop = 0.0f;
	float total_pop_money = 0.0f;
	state.world.for_each_nation([&](dcon::nation_id n) {
		total_pop += state.world.nation_get_demographics(n, demographics::total);
		total_pop_money +=
			state.world.nation_get_total_rich_income(n)
			+ state.world.nation_get_total_middle_income(n)
			+ state.world.nation_get_total_poor_income(n);
	});
	float target_money = total_pop * average_expected_savings;
	state.inflation = 1.01f;
	if(target_money < total_pop_money)
		state.inflation = 0.99f;

	state.world.for_each_market([&](auto ids) {
		auto sid = state.world.market_get_zone_from_local_market(ids);
		auto local_pop = state.world.state_instance_get_demographics(sid, demographics::total);
		float target_money = local_pop * average_expected_savings;

		float local_money = 0.f;
		province::for_each_province_in_state_instance(state, sid, [&](auto pid) {
			state.world.province_for_each_pop_location(pid, [&](auto pop_location) {
				auto pop = state.world.pop_location_get_pop(pop_location);
				local_money += state.world.pop_get_savings(pop);
			});
		});

		float target_scale = target_money / (local_money + 0.0001f);
		float prev_scale = state.world.market_get_income_scale(ids);
		state.world.market_set_income_scale(ids, (target_scale + prev_scale) / 2.f);
	});

	sanity_check(state);

	// make constructions:
	resolve_constructions(state);

	if(!presimulation) {
		run_private_investment(state);
	}

	sanity_check(state);

	//write gdp to file
	if(state.cheat_data.ecodump) {
		for(auto si : state.world.in_state_instance) {
			auto market = si.get_market_from_local_market();
			auto nation = si.get_nation_from_state_ownership();

			auto life_costs =
				state.world.market_get_life_needs_costs(
					market, state.culture_definitions.primary_factory_worker)
				+ state.world.market_get_everyday_needs_costs(
					market, state.culture_definitions.primary_factory_worker)
				+ state.world.market_get_luxury_needs_costs(
					market, state.culture_definitions.primary_factory_worker);

			auto tag = nations::int_to_tag(
				state.world.national_identity_get_identifying_int(
					state.world.nation_get_identity_from_identity_holder(nation)
				)
			);
			auto state_name = text::produce_simple_string(
				state,
				state.world.state_definition_get_name(
					si.get_definition()
				)
			);
			auto name = text::produce_simple_string(state, text::get_name(state, nation));
			state.cheat_data.national_economy_dump_buffer +=
				tag + ","
				+ name + ","
				+ state_name + ","
				+ std::to_string(state.world.market_get_gdp(market)) + ","
				+ std::to_string(life_costs) + ","
				+ std::to_string(state.world.state_instance_get_demographics(si, demographics::total)) + ","
				+ std::to_string(state.current_date.value) + "\n";
		}
	}

	sanity_check(state);
}

void regenerate_unsaved_values(sys::state& state) {
	state.culture_definitions.rgo_workers.clear();
	for(auto pt : state.world.in_pop_type) {
		if(pt.get_is_paid_rgo_worker())
			state.culture_definitions.rgo_workers.push_back(pt);
	}

	auto const total_commodities = state.world.commodity_size();
	for(uint32_t k = 1; k < total_commodities; ++k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
		for(auto pt : state.world.in_pop_type) {
			if(pt != state.culture_definitions.slaves) {
				if(pt.get_life_needs(cid) > 0.0f)
					state.world.commodity_set_is_life_need(cid, true);
				if(pt.get_everyday_needs(cid) > 0.0f)
					state.world.commodity_set_is_everyday_need(cid, true);
				if(pt.get_luxury_needs(cid) > 0.0f)
					state.world.commodity_set_is_luxury_need(cid, true);
			}
		}
	}

	state.world.market_resize_intermediate_demand(state.world.commodity_size());

	state.world.market_resize_life_needs_costs(state.world.pop_type_size());
	state.world.market_resize_everyday_needs_costs(state.world.pop_type_size());
	state.world.market_resize_luxury_needs_costs(state.world.pop_type_size());

	state.world.market_resize_life_needs_scale(state.world.pop_type_size());
	state.world.market_resize_everyday_needs_scale(state.world.pop_type_size());
	state.world.market_resize_luxury_needs_scale(state.world.pop_type_size());

	state.world.market_resize_max_life_needs_satisfaction(state.world.pop_type_size());
	state.world.market_resize_max_everyday_needs_satisfaction(state.world.pop_type_size());
	state.world.market_resize_max_luxury_needs_satisfaction(state.world.pop_type_size());

	state.world.province_resize_rgo_actual_production_per_good(state.world.commodity_size());
}

float government_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	auto overseas_factor =
		state.defines.province_overseas_penalty *
		float(
			state.world.nation_get_owned_province_count(n)
			- state.world.nation_get_central_province_count(n)
		);
	auto o_adjust = 0.0f;
	if(overseas_factor > 0) {
		if(
			state.world.commodity_get_overseas_penalty(c)
			&& (
				state.world.commodity_get_is_available_from_start(c)
				|| state.world.nation_get_unlocked_commodities(n, c)
				)
		) {
			o_adjust = overseas_factor;
		}
	}

	auto total = 0.f;

	state.world.nation_for_each_state_ownership_as_nation(n, [&](auto soid) {
		auto market =
			state.world.state_instance_get_market_from_local_market(
				state.world.state_ownership_get_state(soid)
			);
		total = total + state.world.market_get_army_demand(market, c);
		total = total + state.world.market_get_navy_demand(market, c);
		total = total + state.world.market_get_construction_demand(market, c);
	});

	return total + o_adjust;
}

float factory_type_build_cost(sys::state& state, dcon::nation_id n, dcon::market_id m, dcon::factory_type_id factory_type) {
	auto fat = dcon::fatten(state.world, factory_type);
	auto& costs = fat.get_construction_costs();

	float factory_mod = state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::factory_cost) + 1.0f;
	float admin_eff = state.world.nation_get_administrative_efficiency(state.local_player_nation);
	float admin_cost_factor = (2.0f - admin_eff) * factory_mod;

	auto total = 0.0f;
	for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
		auto cid = costs.commodity_type[i];
		if(bool(cid)) {
			total += price(state, m, cid) * costs.commodity_amounts[i] * admin_cost_factor;
		}
	}

	return total;
}

float factory_type_output_cost(
	sys::state& state,
	dcon::nation_id n,
	dcon::market_id m,
	dcon::factory_type_id factory_type
) {
	auto fac_type = dcon::fatten(state.world, factory_type);
	float output_multiplier = nation_factory_output_multiplier(state, factory_type, n);
	float total_production = fac_type.get_output_amount() * output_multiplier;

	return total_production * price(state, m, fac_type.get_output());
}

float factory_type_input_cost(
	sys::state& state,
	dcon::nation_id n,
	dcon::market_id m,
	dcon::factory_type_id factory_type
) {
	auto fac_type = dcon::fatten(state.world, factory_type);
	float input_total = factory_input_total_cost(state, m, fac_type);
	float e_input_total = factory_e_input_total_cost(state, m, fac_type);

	//modifiers
	auto const maint_multiplier = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
	float input_multiplier = nation_factory_input_multiplier(state, fac_type, n);

	return input_total * input_multiplier + e_input_total * input_multiplier * maint_multiplier;
}

float nation_factory_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	auto amount = 0.f;
	for(auto ownership : state.world.nation_get_province_ownership(n)) {
		for(auto location : state.world.province_get_factory_location(ownership.get_province())) {
			// factory
			auto f = state.world.factory_location_get_factory(location);
			auto p = ownership.get_province();
			auto s = p.get_state_membership();
			auto fac = fatten(state.world, f);
			auto fac_type = fac.get_building_type();

			// assume that all inputs are available
			float min_input_available = 1.f;
			float min_e_input_available = 1.f;

			//modifiers

			float input_multiplier = factory_input_multiplier(state, fac, n, p, s);
			float throughput_multiplier = factory_throughput_multiplier(state, fac_type, n, p, s);
			float output_multiplier = factory_output_multiplier(state, fac, n, p);

			//this value represents total production if 1 lvl of this factory is filled with workers
			float total_production = fac_type.get_output_amount()
				* (0.75f + 0.25f * min_e_input_available)
				* throughput_multiplier
				* output_multiplier
				* min_input_available;

			float effective_production_scale = fac.get_production_scale();

			auto& inputs = fac_type.get_inputs();
			auto& e_inputs = fac_type.get_efficiency_inputs();

			// register real demand : input_multiplier * throughput_multiplier * level * primary_employment
			// also multiply by target production scale... otherwise too much excess demand is generated
			// also multiply by something related to minimal satisfied input
			// to prevent generation of too much demand on rgos already influenced by a shortage

			float input_scale =
				input_multiplier
				* throughput_multiplier
				* effective_production_scale
				* (0.1f + min_input_available * 0.9f);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(inputs.commodity_type[i]) {
					if(inputs.commodity_type[i] == c) {
						amount +=
							+input_scale * inputs.commodity_amounts[i];
						break;
					}
				} else {
					break;
				}
			}

			// and for efficiency inputs
			//  the consumption of efficiency inputs is (national-factory-maintenance-modifier + 1) x input-multiplier x
			//  throughput-multiplier x factory level
			auto const mfactor = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
			for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
				if(e_inputs.commodity_type[i]) {
					if(e_inputs.commodity_type[i] == c) {
						amount +=
							mfactor
							* input_scale
							* e_inputs.commodity_amounts[i]
							* (0.1f + min_e_input_available * 0.9f);
						break;
					}
				} else {
					break;
				}
			}
		}
	}
	return amount;
}

float nation_pop_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	auto amount = 0.f;
	if(state.world.commodity_get_is_available_from_start(c) || state.world.nation_get_unlocked_commodities(n, c)) {
		state.world.nation_for_each_state_ownership(n, [&](auto soid) {
			auto si = state.world.state_ownership_get_state(soid);
			auto m = state.world.state_instance_get_market_from_local_market(si);
			state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
				amount += (
						state.world.pop_type_get_life_needs(pt, c)
							* state.world.market_get_life_needs_weights(m, c)
						+ state.world.pop_type_get_everyday_needs(pt, c)
							* state.world.market_get_everyday_needs_weights(m, c)
						+ state.world.pop_type_get_luxury_needs(pt, c)
							* state.world.market_get_luxury_needs_weights(m, c)
					)
					* state.world.nation_get_demographics(n, demographics::to_key(state, pt))

					/ state.defines.alice_needs_scaling_factor;
			});
		});
	}
	return amount;
}

float nation_total_imports(sys::state& state, dcon::nation_id n) {
	float t_total = 0.0f;

	auto const total_commodities = state.world.commodity_size();

	for(uint32_t k = 1; k < total_commodities; ++k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
		state.world.nation_for_each_state_ownership(n, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			t_total += price(state, market, cid) * state.world.market_get_import(market, cid);
		});
	}

	return t_total;
}

float nation_total_exports(sys::state& state, dcon::nation_id n) {
	float t_total = 0.0f;

	auto const total_commodities = state.world.commodity_size();

	for(uint32_t k = 1; k < total_commodities; ++k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
		state.world.nation_for_each_state_ownership(n, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			t_total += price(state, market, cid) * state.world.market_get_export(market, cid);
		});
	}

	return t_total;
}

float pop_income(sys::state& state, dcon::pop_id p) {
	auto saved = state.world.pop_get_savings(p);
	if(saved <= 0.0f)
		return 0.0f;

	auto owner = nations::owner_of_pop(state, p);
	auto const tax_eff = nations::tax_efficiency(state, owner);
	auto strata = culture::pop_strata(state.world.pop_type_get_strata(state.world.pop_get_poptype(p)));
	switch(strata) {
	default:
	case culture::pop_strata::poor:
		return saved / std::max(0.0001f, (1.0f - tax_eff * float(state.world.nation_get_poor_tax(owner)) / 100.0f));
	case culture::pop_strata::middle:
		return saved / std::max(0.0001f, (1.0f - tax_eff * float(state.world.nation_get_middle_tax(owner)) / 100.0f));
	case culture::pop_strata::rich:
		return saved / std::max(0.0001f, (1.0f - tax_eff * float(state.world.nation_get_rich_tax(owner)) / 100.0f));
	}
}

float estimate_gold_income(sys::state& state, dcon::nation_id n) {
	auto amount = 0.f;
	for(auto poid : state.world.nation_get_province_ownership_as_nation(n)) {
		auto prov = poid.get_province();

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(state.world.commodity_get_money_rgo(c)) {
				amount +=
					province::rgo_production_quantity(state, prov.id, c)
					* state.world.commodity_get_cost(c);
			}
		});
	}
	return amount * state.defines.gold_to_cash_rate;
}

float estimate_tariff_income(sys::state& state, dcon::nation_id n) {
	return
		nations::tariff_efficiency(state, n) *
		(
			economy::nation_total_imports(state, n)
			+ economy::nation_total_exports(state, n)
		);
}

float estimate_social_spending(sys::state& state, dcon::nation_id n) {
	auto total = 0.f;
	auto const p_level = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::pension_level);
	auto const unemp_level = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::unemployment_benefit);

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			auto adj_pop_of_type =
				state.world.state_instance_get_demographics(local_state, demographics::to_key(state, pt))
				/ state.defines.alice_needs_scaling_factor;
			if(adj_pop_of_type <= 0)
				return;

			auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
			if(ln_type == culture::income_type::administration || ln_type == culture::income_type::education || ln_type == culture::income_type::military) {
				//nothing
			} else { // unemployment, pensions
				total += adj_pop_of_type * p_level * state.world.market_get_life_needs_costs(market, pt);
				if(state.world.pop_type_get_has_unemployment(pt)) {
					auto emp = state.world.state_instance_get_demographics(local_state, demographics::to_employment_key(state, pt)) / state.defines.alice_needs_scaling_factor;
					total +=
						(adj_pop_of_type - emp)
						* unemp_level
						* state.world.market_get_life_needs_costs(market, pt);
				}
			}
		});
	});
	return total;
}

float estimate_pop_payouts_by_income_type(sys::state& state, dcon::nation_id n, culture::income_type in) {
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			auto adj_pop_of_type =
				state.world.state_instance_get_demographics(local_state, demographics::to_key(state, pt)) / state.defines.alice_needs_scaling_factor;

			if(adj_pop_of_type <= 0)
				return;

			auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
			if(ln_type == in) {
				total += adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt);
			}

			auto en_type = culture::income_type(state.world.pop_type_get_everyday_needs_income_type(pt));
			if(en_type == in) {
				total += adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt);
			}

			auto lx_type = culture::income_type(state.world.pop_type_get_luxury_needs_income_type(pt));
			if(lx_type == in) {
				total += adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt);
			}
		});
	});
	return total;
}

float estimate_tax_income_by_strata(sys::state& state, dcon::nation_id n, culture::pop_strata ps) {
	switch(ps) {
	default:
	case culture::pop_strata::poor:
		return state.world.nation_get_total_poor_income(n) * nations::tax_efficiency(state, n);
	case culture::pop_strata::middle:
		return state.world.nation_get_total_middle_income(n) * nations::tax_efficiency(state, n);
	case culture::pop_strata::rich:
		return state.world.nation_get_total_rich_income(n) * nations::tax_efficiency(state, n);
	}
}

float estimate_subsidy_spending(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_subsidies_spending(n);
}

float estimate_war_subsidies_income(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	for(auto uni : state.world.nation_get_unilateral_relationship_as_target(n)) {
		if(uni.get_war_subsidies()) {
			total += estimate_war_subsidies(state, uni.get_target(), uni.get_source());
		}
	}
	return total;
}
float estimate_reparations_income(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	for(auto uni : state.world.nation_get_unilateral_relationship_as_target(n)) {
		if(uni.get_reparations() && state.current_date < uni.get_source().get_reparations_until()) {
			auto source = uni.get_source();
			auto const tax_eff = nations::tax_efficiency(state, n);
			auto total_tax_base = state.world.nation_get_total_rich_income(source) +
				state.world.nation_get_total_middle_income(source) +
				state.world.nation_get_total_poor_income(source);
			auto payout = total_tax_base * tax_eff * state.defines.reparations_tax_hit;
			total += payout;
		}
	}
	return total;
}

float estimate_war_subsidies_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	for(auto uni : state.world.nation_get_unilateral_relationship_as_source(n)) {
		if(uni.get_war_subsidies()) {
			total += estimate_war_subsidies(state, uni.get_target(), uni.get_source());
		}
	}

	return total;
}

float estimate_reparations_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	if(state.current_date < state.world.nation_get_reparations_until(n)) {
		for(auto uni : state.world.nation_get_unilateral_relationship_as_source(n)) {
			if(uni.get_reparations()) {
				auto const tax_eff = nations::tax_efficiency(state, n);
				auto total_tax_base = state.world.nation_get_total_rich_income(n) +
					state.world.nation_get_total_middle_income(n) +
					state.world.nation_get_total_poor_income(n);
				auto payout = total_tax_base * tax_eff * state.defines.reparations_tax_hit;
				total += payout;
			}
		}
	}
	return total;
}

float estimate_diplomatic_balance(sys::state& state, dcon::nation_id n) {
	float w_sub = estimate_war_subsidies_income(state, n) - estimate_war_subsidies_spending(state, n);
	float w_reps = estimate_reparations_income(state, n) - estimate_reparations_spending(state, n);

	float subject_payments = estimate_subject_payments_paid(state, n) + estimate_subject_payments_received(state, n);

	return w_sub + w_reps + subject_payments;
}

float estimate_domestic_investment(sys::state& state, dcon::nation_id n) {
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		auto adj_pop_of_type_capis = (state.world.state_instance_get_demographics(
			local_state, demographics::to_key(state, state.culture_definitions.capitalists))
			) / state.defines.alice_needs_scaling_factor;
		auto adj_pop_of_type_arist = (state.world.state_instance_get_demographics(
			local_state, demographics::to_key(state, state.culture_definitions.aristocrat))
			) / state.defines.alice_needs_scaling_factor;
		float arist_costs =
			state.world.market_get_life_needs_costs(market, state.culture_definitions.aristocrat)
			+ state.world.market_get_everyday_needs_costs(market, state.culture_definitions.aristocrat)
			+ state.world.market_get_luxury_needs_costs(market, state.culture_definitions.aristocrat);
		float capis_costs =
			state.world.market_get_life_needs_costs(market, state.culture_definitions.capitalists)
			+ state.world.market_get_everyday_needs_costs(market, state.culture_definitions.capitalists)
			+ state.world.market_get_luxury_needs_costs(market, state.culture_definitions.capitalists);

		total = total
			+ state.defines.alice_domestic_investment_multiplier
			* (
				adj_pop_of_type_capis * capis_costs
				+ adj_pop_of_type_arist * arist_costs
			);
	});
	return total;
}

float estimate_land_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			total +=
				state.world.market_get_army_demand(market, cid)
				* price(state, market, cid)
				* state.world.market_get_demand_satisfaction(market, cid);
		}
	});
	return total;
}

float estimate_naval_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			total += state.world.market_get_navy_demand(market, cid)
				* price(state, market, cid)
				* state.world.market_get_demand_satisfaction(market, cid);
		}
	});
	return total;
}

float estimate_war_subsidies(sys::state& state, dcon::nation_fat_id target, dcon::nation_fat_id source) {
	/* total-nation-tax-base x defines:WARSUBSIDIES_PERCENT */

	auto target_m_costs = (target.get_total_rich_income() + target.get_total_middle_income() + target.get_total_poor_income()) * state.defines.warsubsidies_percent;
	auto source_m_costs = (source.get_total_rich_income() + source.get_total_middle_income() + source.get_total_poor_income()) * state.defines.warsubsidies_percent;
	return std::min(target_m_costs, source_m_costs);
}

float estimate_subject_payments_paid(sys::state& state, dcon::nation_id n) {
	auto const tax_eff = nations::tax_efficiency(state, n);
	auto collected_tax = state.world.nation_get_total_poor_income(n) * tax_eff * float(state.world.nation_get_poor_tax(n)) / 100.0f +
		state.world.nation_get_total_middle_income(n) * tax_eff * float(state.world.nation_get_middle_tax(n)) / 100.0f +
		state.world.nation_get_total_rich_income(n) * tax_eff * float(state.world.nation_get_rich_tax(n)) / 100.0f;

	auto rel = state.world.nation_get_overlord_as_subject(n);
	auto overlord = state.world.overlord_get_ruler(rel);

	if(overlord) {
		auto transferamt = collected_tax;

		if(state.world.nation_get_is_substate(n)) {
			transferamt *= state.defines.alice_substate_subject_money_transfer / 100.f;
		} else {
			transferamt *= state.defines.alice_puppet_subject_money_transfer / 100.f;
		}

		return transferamt;
	}

	return 0;
}

float estimate_subject_payments_received(sys::state& state, dcon::nation_id o) {
	auto res = 0.0f;
	for(auto n : state.world.in_nation) {
		auto rel = state.world.nation_get_overlord_as_subject(n);
		auto overlord = state.world.overlord_get_ruler(rel);

		if(overlord == o) {
			auto const tax_eff = nations::tax_efficiency(state, n);
			auto const collected_tax = state.world.nation_get_total_poor_income(n) * tax_eff * float(state.world.nation_get_poor_tax(n)) / 100.0f +
				state.world.nation_get_total_middle_income(n) * tax_eff * float(state.world.nation_get_middle_tax(n)) / 100.0f +
				state.world.nation_get_total_rich_income(n) * tax_eff * float(state.world.nation_get_rich_tax(n)) / 100.0f;
			auto transferamt = collected_tax;

			if(state.world.nation_get_is_substate(n)) {
				transferamt *= state.defines.alice_substate_subject_money_transfer / 100.f;
			} else {
				transferamt *= state.defines.alice_puppet_subject_money_transfer / 100.f;
			}

			res += transferamt;
		}
	}

	return res;
}

construction_status province_building_construction(sys::state& state, dcon::province_id p, province_building_type t) {
	for(auto pb_con : state.world.province_get_province_building_construction(p)) {
		if(pb_con.get_type() == uint8_t(t)) {
			float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_get_nation_from_province_ownership(p));
			float admin_cost_factor = pb_con.get_is_pop_project() ? 1.0f : 2.0f - admin_eff;

			float total = 0.0f;
			float purchased = 0.0f;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += state.economy_definitions.building_definitions[int32_t(t)].cost.commodity_amounts[i] * admin_cost_factor;
				purchased += pb_con.get_purchased_goods().commodity_amounts[i];
			}
			return construction_status{ total > 0.0f ? purchased / total : 0.0f, true };
		}
	}
	return construction_status{ 0.0f, false };
}

construction_status factory_upgrade(sys::state& state, dcon::factory_id f) {
	auto in_prov = state.world.factory_get_province_from_factory_location(f);
	auto in_state = state.world.province_get_state_membership(in_prov);
	auto fac_type = state.world.factory_get_building_type(f);

	for(auto st_con : state.world.state_instance_get_state_building_construction(in_state)) {
		if(st_con.get_type() == fac_type) {
			float admin_eff = state.world.nation_get_administrative_efficiency(st_con.get_nation());
			float factory_mod = state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_cost) + 1.0f;
			float pop_factory_mod = std::max(0.1f, state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_owner_cost));
			float admin_cost_factor = (st_con.get_is_pop_project() ? pop_factory_mod : (2.0f - admin_eff)) * factory_mod;


			float total = 0.0f;
			float purchased = 0.0f;
			auto& goods = state.world.factory_type_get_construction_costs(fac_type);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += goods.commodity_amounts[i] * admin_cost_factor;
				purchased += st_con.get_purchased_goods().commodity_amounts[i];
			}

			return construction_status{ total > 0.0f ? purchased / total : 0.0f, true };
		}
	}

	return construction_status{ 0.0f, false };
}

bool state_contains_constructed_factory(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id ft) {
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

bool state_contains_factory(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id ft) {
	auto d = state.world.state_instance_get_definition(s);

	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_state_membership() == s) {
			for(auto f : p.get_province().get_factory_location()) {
				if(f.get_factory().get_building_type() == ft)
					return true;
			}
		}
	}
	for(auto sc : state.world.state_instance_get_state_building_construction(s)) {
		if(sc.get_type() == ft)
			return true;
	}

	return false;
}

int32_t state_factory_count(sys::state& state, dcon::state_instance_id sid, dcon::nation_id n) {
	int32_t num_factories = 0;
	auto d = state.world.state_instance_get_definition(sid);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d))
		if(p.get_province().get_nation_from_province_ownership() == n)
			num_factories += int32_t(state.world.province_get_factory_location(p.get_province()).end() - state.world.province_get_factory_location(p.get_province()).begin());
	for(auto p : state.world.state_instance_get_state_building_construction(sid))
		if(p.get_is_upgrade() == false)
			++num_factories;

	// For new factories: no more than defines:FACTORIES_PER_STATE existing + under construction new factories must be
	assert(num_factories <= int32_t(state.defines.factories_per_state));
	return num_factories;
}

float unit_construction_progress(sys::state& state, dcon::province_land_construction_id c) {

	float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_land_construction_get_nation(c));
	float admin_cost_factor = 2.0f - admin_eff;

	auto& goods = state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(c)].build_cost;
	auto& cgoods = state.world.province_land_construction_get_purchased_goods(c);

	float total = 0.0f;
	float purchased = 0.0f;

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		total += goods.commodity_amounts[i] * admin_cost_factor;
		purchased += cgoods.commodity_amounts[i];
	}

	return total > 0.0f ? purchased / total : 0.0f;
}

float unit_construction_progress(sys::state& state, dcon::province_naval_construction_id c) {
	float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_naval_construction_get_nation(c));
	float admin_cost_factor = 2.0f - admin_eff;

	auto& goods = state.military_definitions.unit_base_definitions[state.world.province_naval_construction_get_type(c)].build_cost;
	auto& cgoods = state.world.province_naval_construction_get_purchased_goods(c);

	float total = 0.0f;
	float purchased = 0.0f;

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		total += goods.commodity_amounts[i] * admin_cost_factor;
		purchased += cgoods.commodity_amounts[i];
	}

	return total > 0.0f ? purchased / total : 0.0f;
}

void add_factory_level_to_state(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id t, bool is_upgrade) {

	if(is_upgrade) {
		auto d = state.world.state_instance_get_definition(s);
		auto o = state.world.state_instance_get_nation_from_state_ownership(s);
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == o) {
				for(auto f : p.get_province().get_factory_location()) {
					if(f.get_factory().get_building_type() == t) {
						auto factory_level = f.get_factory().get_level();
						auto new_factory_level = std::min(float(std::numeric_limits<uint8_t>::max()), float(factory_level) + 1.f + math::sqrt(f.get_factory().get_level()) / 2.f);
						f.get_factory().get_level() = uint8_t(new_factory_level);
						return;
					}
				}
			}
		}
	}
	auto state_cap = state.world.state_instance_get_capital(s);
	auto new_fac = fatten(state.world, state.world.create_factory());
	new_fac.set_building_type(t);
	new_fac.set_level(uint8_t(1));
	new_fac.set_production_scale(1.0f);

	state.world.try_create_factory_location(new_fac, state_cap);
}

void resolve_constructions(sys::state& state) {

	for(auto c : state.world.in_province_land_construction) {
		float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_land_construction_get_nation(c));
		float admin_cost_factor = 2.0f - admin_eff;

		auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
		auto& current_purchased = c.get_purchased_goods();
		float construction_time = float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

		bool all_finished = true;
		if(!(c.get_nation().get_is_player_controlled() && state.cheat_data.instant_army)) {
			for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
				if(base_cost.commodity_type[j]) {
					if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * admin_cost_factor) {
						all_finished = false;
					}
				} else {
					break;
				}
			}
		}

		if(all_finished) {
			auto pop_location = c.get_pop().get_province_from_pop_location();

			auto new_reg = military::create_new_regiment(state, c.get_nation(), c.get_type());
			auto a = fatten(state.world, state.world.create_army());

			a.set_controller_from_army_control(c.get_nation());
			state.world.try_create_army_membership(new_reg, a);
			state.world.try_create_regiment_source(new_reg, c.get_pop());
			military::army_arrives_in_province(state, a, pop_location, military::crossing_type::none);
			military::move_land_to_merge(state, c.get_nation(), a, pop_location, c.get_template_province());

			if(c.get_nation() == state.local_player_nation) {
				notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "amsg_army_built");
					},
					"amsg_army_built",
					state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
					sys::message_base_type::army_built
				});
			}

			state.world.delete_province_land_construction(c);
		}
	}

	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto rng = state.world.province_get_province_naval_construction(p);
		if(rng.begin() != rng.end()) {
			auto c = *(rng.begin());

			float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_naval_construction_get_nation(c));
			float admin_cost_factor = 2.0f - admin_eff;

			auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

			bool all_finished = true;
			if(!(c.get_nation().get_is_player_controlled() && state.cheat_data.instant_navy)) {
				for(uint32_t i = 0; i < commodity_set::set_size && all_finished; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor) {
							all_finished = false;
						}
					} else {
						break;
					}
				}
			}

			if(all_finished) {
				auto new_ship = military::create_new_ship(state, c.get_nation(), c.get_type());
				auto a = fatten(state.world, state.world.create_navy());
				a.set_controller_from_navy_control(c.get_nation());
				a.set_location_from_navy_location(p);
				state.world.try_create_navy_membership(new_ship, a);
				military::move_navy_to_merge(state, c.get_nation(), a, c.get_province(), c.get_template_province());

				if(c.get_nation() == state.local_player_nation) {
					notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "amsg_navy_built");
						},
						"amsg_navy_built",
						state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::navy_built
					});
				}

				state.world.delete_province_naval_construction(c);
			}
		}
	});

	for(auto c : state.world.in_province_building_construction) {
		auto for_province = state.world.province_building_construction_get_province(c);

		float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_building_construction_get_nation(c));
		float admin_cost_factor = state.world.province_building_construction_get_is_pop_project(c) ? 1.0f : 2.0f - admin_eff;

		auto t = province_building_type(state.world.province_building_construction_get_type(c));
		auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
		auto& current_purchased = state.world.province_building_construction_get_purchased_goods(c);
		bool all_finished = true;

		for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
			if(base_cost.commodity_type[j]) {
				if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * admin_cost_factor) {
					all_finished = false;
				}
			} else {
				break;
			}
		}

		if(all_finished) {
			if(state.world.province_get_building_level(for_province, uint8_t(t)) < state.world.nation_get_max_building_level(state.world.province_get_nation_from_province_ownership(for_province), uint8_t(t))) {
				state.world.province_get_building_level(for_province, uint8_t(t)) += 1;

				if(t == province_building_type::railroad) {
					/* Notify the railroad mesh builder to update the railroads! */
					state.railroad_built.store(true, std::memory_order::release);
				}

				if(state.world.province_building_construction_get_nation(c) == state.local_player_nation) {
					switch(t) {
					case province_building_type::naval_base:
						notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
								text::add_line(state, contents, "amsg_naval_base_complete");
							},
							"amsg_naval_base_complete",
							state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
							sys::message_base_type::naval_base_complete
						});
						break;
					case province_building_type::fort:
						notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
								text::add_line(state, contents, "amsg_fort_complete");
							},
							"amsg_fort_complete",
							state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
							sys::message_base_type::fort_complete
						});
						break;
					case province_building_type::railroad:
						notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
								text::add_line(state, contents, "amsg_rr_complete");
							},
							"amsg_rr_complete",
							state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
							sys::message_base_type::rr_complete
						});
						break;
					default:
						break;
					}
				}
			}
			state.world.delete_province_building_construction(c);
		}
	}

	for(auto c : state.world.in_state_building_construction) {
		auto n = state.world.state_building_construction_get_nation(c);
		auto type = state.world.state_building_construction_get_type(c);
		auto& base_cost = state.world.factory_type_get_construction_costs(type);
		auto& current_purchased = state.world.state_building_construction_get_purchased_goods(c);

		if(!state.world.state_building_construction_get_is_pop_project(c)) {
			float admin_eff = state.world.nation_get_administrative_efficiency(n);
			float admin_cost_factor = 2.0f - admin_eff;

			float factory_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f;

			bool all_finished = true;
			if(!(n == state.local_player_nation && state.cheat_data.instant_industry)) {
				for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
					if(base_cost.commodity_type[j]) {
						if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * factory_mod * admin_cost_factor) {
							all_finished = false;
						}
					} else {
						break;
					}
				}
			}
			if(all_finished) {
				add_factory_level_to_state(state, state.world.state_building_construction_get_state(c), type,
						state.world.state_building_construction_get_is_upgrade(c));
				state.world.delete_state_building_construction(c);
			}
		} else {
			float factory_mod = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f) *
				std::max(0.1f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_owner_cost));

			bool all_finished = true;
			if(!(n == state.local_player_nation && state.cheat_data.instant_industry)) {
				for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
					if(base_cost.commodity_type[j]) {
						if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * factory_mod) {
							all_finished = false;
						}
					} else {
						break;
					}
				}
			}
			if(all_finished) {
				add_factory_level_to_state(state, state.world.state_building_construction_get_state(c), type,
						state.world.state_building_construction_get_is_upgrade(c));

				if(state.world.state_building_construction_get_nation(c) == state.local_player_nation) {
					notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "amsg_factory_complete");
						},
						"amsg_factory_complete",
						state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::factory_complete
					});
				}

				state.world.delete_state_building_construction(c);
			}
		}
	}
}

/* TODO -
 * This should return what we think the income will be next day, and as a result wont account for any unprecedented actions
 * return value is passed directly into text::fp_currency{} without adulteration.
 */
float estimate_daily_income(sys::state& state, dcon::nation_id n) {
	auto const tax_eff = nations::tax_efficiency(state, n);
	return (
		state.world.nation_get_total_poor_income(n) * state.world.nation_get_poor_tax(n) / 100.f
		+ state.world.nation_get_total_middle_income(n) * state.world.nation_get_middle_tax(n) / 100.f
		+ state.world.nation_get_total_rich_income(n) * state.world.nation_get_rich_tax(n) / 100.f
	) * tax_eff;
}

void try_add_factory_to_state(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id t) {
	auto n = state.world.state_instance_get_nation_from_state_ownership(s);

	if(state.world.factory_type_get_is_coastal(t)) {
		if(!province::state_is_coastal(state, s))
			return; // requires coast to build coastal factory
	}

	auto existing_constructions = state.world.state_instance_get_state_building_construction(s);
	int32_t num_factories = 0;
	for(auto prj : existing_constructions) {
		if(!prj.get_is_upgrade())
			++num_factories;
		if(prj.get_type() == t)
			return; // can't duplicate type
	}

	// is there an upgrade target ?
	auto d = state.world.state_instance_get_definition(s);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == n) {
			for(auto f : p.get_province().get_factory_location()) {
				++num_factories;
				if(f.get_factory().get_building_type() == t)
					return; // can't build another of this type
			}
		}
	}

	if(num_factories < int32_t(state.defines.factories_per_state)) {
		add_factory_level_to_state(state, s, t, false);
	}
}

void bound_budget_settings(sys::state& state, dcon::nation_id n) {
	{
		auto min_tariff = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_tariff));
		auto max_tariff = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_tariff));
		max_tariff = std::max(min_tariff, max_tariff);

		auto& tariff = state.world.nation_get_tariffs(n);
		tariff = int8_t(std::clamp(std::clamp(int32_t(tariff), min_tariff, max_tariff), 0, 100));
	}
	{
		auto min_tax = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_tax));
		auto max_tax = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_tax));
		if(max_tax <= 0)
			max_tax = 100;
		max_tax = std::max(min_tax, max_tax);

		auto& ptax = state.world.nation_get_poor_tax(n);
		ptax = int8_t(std::clamp(std::clamp(int32_t(ptax), min_tax, max_tax), 0, 100));
		auto& mtax = state.world.nation_get_middle_tax(n);
		mtax = int8_t(std::clamp(std::clamp(int32_t(mtax), min_tax, max_tax), 0, 100));
		auto& rtax = state.world.nation_get_rich_tax(n);
		rtax = int8_t(std::clamp(std::clamp(int32_t(rtax), min_tax, max_tax), 0, 100));
	}
	{
		auto min_spend =
			int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_military_spending));
		auto max_spend =
			int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_military_spending));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		auto& v = state.world.nation_get_military_spending(n);
		v = int8_t(std::clamp(std::clamp(int32_t(v), min_spend, max_spend), 0, 100));
	}
	{
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_social_spending));
		auto max_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_social_spending));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		auto& v = state.world.nation_get_social_spending(n);
		v = int8_t(std::clamp(std::clamp(int32_t(v), min_spend, max_spend), 0, 100));
	}
	{
		auto min_spend =
			int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_military_spending));
		auto max_spend =
			int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_military_spending));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		auto& v = state.world.nation_get_military_spending(n);
		v = int8_t(std::clamp(std::clamp(int32_t(v), min_spend, max_spend), 0, 100));
	}
	{
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_domestic_investment));
		auto max_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_domestic_investment));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		auto& v = state.world.nation_get_domestic_investment_spending(n);
		v = int8_t(std::clamp(std::clamp(int32_t(v), min_spend, max_spend), 0, 100));
	}
}

void prune_factories(sys::state& state) {
	for(auto si : state.world.in_state_instance) {
		auto owner = si.get_nation_from_state_ownership();
		auto rules = owner.get_combined_issue_rules();

		if(owner.get_is_player_controlled() && (rules & issue_rule::destroy_factory) != 0) // not for players who can manually destroy
			continue;

		dcon::factory_id deletion_choice;
		int32_t factory_count = 0;

		province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
			for(auto f : state.world.province_get_factory_location(p)) {
				++factory_count;
				auto scale = f.get_factory().get_production_scale();
				float ten_workers = 10.f / factory_max_employment(state, f.get_factory());
				bool unprofitable = f.get_factory().get_unprofitable();
				if(((scale < ten_workers) && unprofitable) && (!deletion_choice || state.world.factory_get_level(deletion_choice) > f.get_factory().get_level())) {
					deletion_choice = f.get_factory();
				}
			}
		});

		// aggressive pruning
		// to help building more healthy economy instead of 1 profitable giant factory with 6 small 0 scale factories
		if(deletion_choice && (4 + factory_count) >= int32_t(state.defines.factories_per_state)) {
			auto production_type = state.world.factory_get_building_type(deletion_choice);
			state.world.delete_factory(deletion_choice);

			for(auto proj : si.get_state_building_construction()) {
				if(proj.get_type() == production_type) {
					state.world.delete_state_building_construction(proj);
					break;
				}
			}
		}
	}
}

dcon::modifier_id get_province_selector_modifier(sys::state& state) {
	return state.economy_definitions.selector_modifier;
}

dcon::modifier_id get_province_immigrator_modifier(sys::state& state) {
	return state.economy_definitions.immigrator_modifier;
}

void go_bankrupt(sys::state& state, dcon::nation_id n) {
	auto& debt = state.world.nation_get_local_loan(n);

	/*
	 If a nation cannot pay and the amount it owes is less than define:SMALL_DEBT_LIMIT, the nation it owes money to gets an on_debtor_default_small event (with the nation defaulting in the from slot). Otherwise, the event is pulled from on_debtor_default. The nation then goes bankrupt. It receives the bad_debter modifier for define:BANKRUPCY_EXTERNAL_LOAN_YEARS years (if it goes bankrupt again within this period, creditors receive an on_debtor_default_second event). It receives the in_bankrupcy modifier for define:BANKRUPCY_DURATION days. Its prestige is reduced by a factor of define:BANKRUPCY_FACTOR, and each of its pops has their militancy increase by 2.
	*/
	auto existing_br = state.world.nation_get_bankrupt_until(n);
	if(existing_br && state.current_date < existing_br) {
		for(auto gn : state.great_nations) {
			if(gn.nation && gn.nation != n) {
				event::fire_fixed_event(state, state.national_definitions.on_debtor_default_second, trigger::to_generic(gn.nation), event::slot_type::nation, gn.nation, trigger::to_generic(n), event::slot_type::nation);
			}
		}
	} else if(debt >= -state.defines.small_debt_limit) {
		for(auto gn : state.great_nations) {
			if(gn.nation && gn.nation != n) {
				event::fire_fixed_event(state, state.national_definitions.on_debtor_default_small, trigger::to_generic(gn.nation), event::slot_type::nation, gn.nation, trigger::to_generic(n), event::slot_type::nation);
			}
		}
	} else {
		for(auto gn : state.great_nations) {
			if(gn.nation && gn.nation != n) {
				event::fire_fixed_event(state, state.national_definitions.on_debtor_default, trigger::to_generic(gn.nation), event::slot_type::nation, gn.nation, trigger::to_generic(n), event::slot_type::nation);
			}
		}
	}

	// RESET MONEY: POTENTIAL MERGE CONFLICT WITH SNEAKBUG'S FUTURE CHANGES
	state.world.nation_set_stockpiles(n, economy::money, 0.f);

	sys::add_modifier_to_nation(state, n, state.national_definitions.in_bankrupcy, state.current_date + int32_t(state.defines.bankrupcy_duration * 365));
	sys::add_modifier_to_nation(state, n, state.national_definitions.bad_debter, state.current_date + int32_t(state.defines.bankruptcy_external_loan_years * 365));

	debt = 0.0f;
	state.world.nation_set_is_debt_spending(n, false);
	state.world.nation_set_bankrupt_until(n, state.current_date + int32_t(state.defines.bankrupcy_duration * 365));

	notification::post(state, notification::message{
		[n](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_bankruptcy_1", text::variable_type::x, n);
		},
		"msg_bankruptcy_title",
		n, dcon::nation_id{}, dcon::nation_id{},
		sys::message_base_type::bankruptcy
	});
}

float estimate_investment_pool_daily_loss(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_private_investment(n) * 0.001f;
}

} // namespace economy
