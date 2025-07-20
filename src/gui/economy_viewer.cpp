#include <numbers>

#include "economy_viewer.hpp"
#include "economy_stats.hpp"
#include "economy_production.hpp"
#include "economy_trade_routes.hpp"
#include "color.hpp"
#include "labour_details.hpp"


namespace economy_viewer {

enum class static_elements : int32_t {
	markets = 0,
	factory_types = 10000,
	factory_type_page = 11000,
	factory_type_page_number = 11100,
	factory_type_priority_button = 11200,
	factory_type_selector = 12000,
	commodities = 13000,
	commodities_page = 13800,
	commodities_page_number = 13820,
	commodities_mode_selector_button = 13830,
	commodities_mode_selector_button_label = 13850,
	commodities_inputs = 14000,
	commodities_inputs_amount = 15000,
	commodities_inputs_cost = 16000,
	commodities_e_inputs = 17000,
	commodities_e_inputs_amount = 18000,
	commodities_e_inputs_cost = 19000,


	commodities_output = 20000,
	commodities_output_amount = 21000,
	commodities_output_cost = 22000,

	wages_tab_labor_selector_button = 23000,
	wages_tab_labor_selector_button_label = 23050,
	wages_tab_stats_selector_button = 23100,
	wages_tab_stats_selector_button_label = 23150,

	commodities_button = 10000000,
	factory_types_button = 10000001,
	close_factory_type_description = 10000002,
	province_name = 10000003,
	state_def_name = 10000004,
	factory_type_name = 10000005,
	factory_type_total = 10000006,


	priority_text = 10000007,
	priority_text_national = 10000008,
	priority_text_private = 10000009,
	priority_text_bonus = 10000010,
	priority_value_national = 10000011,
	priority_value_private = 10000012,
	priority_value_bonus = 10000013,

	nation_vs_market_toggle = 10000014,
	nation_vs_market_toggle_label = 10000015,

	wages_tab = 10000016,
	wages_tab_label = 10000017,

	trade_volume_tab = 10000018,
	trade_volume_tab_label = 10000019,

	tab_name_commodity = 10005000,
	tab_name_factory_type = 10005001,
	tab_name_production_chain = 10005002,


	input_efficiency_leaders = 10006000,

	bin_center = 20000000,
	bin_range = 20000001
};

enum class scaling_mode {
	linear, log
};

void update(sys::state& state) {

	auto key = state.lookup_key("alice_button_factory_type_priority");
	auto def = state.ui_state.defs_by_name.find(key)->second.definition;
	auto& data = state.ui_defs.gui[def];

	if(state.iui_state.per_market_data.size() != state.world.market_size() + 1) {
		state.iui_state.per_market_data.resize(state.world.market_size() + 1);
	}

	if(state.iui_state.per_nation_data.size() != state.world.nation_size() + 1) {
		state.iui_state.per_nation_data.resize(state.world.nation_size() + 1);
	}

	if(state.iui_state.per_province_data.size() != state.world.province_size() + 1) {
		state.iui_state.per_province_data.resize(state.world.province_size() + 1);
	}

	state.iui_state.input_efficiency_leaders.clear();
	state.iui_state.input_efficiency_leaders_string.clear();

	bool cut_away_negative = false;
	bool start_min_at_zero = false;
	bool balance_color = false;
	bool do_not_cut_away_values = false;

	auto scaling = scaling_mode::linear;
	int bins = 30;

	if(state.selected_factory_type && state.iui_state.tab == iui::iui_tab::factory_types) {
		auto& inputs = state.world.factory_type_get_inputs(state.selected_factory_type);
		auto& e_inputs = state.world.factory_type_get_efficiency_inputs(state.selected_factory_type);
		auto ftid = state.selected_factory_type;

		state.world.for_each_market([&](dcon::market_id market) {
			auto sid = state.world.market_get_zone_from_local_market(market);
			auto sdif = state.world.state_instance_get_definition(sid);
			auto nid = state.world.state_instance_get_nation_from_state_ownership(sid);
			auto mid = market;

			auto e_input_mult = state.world.nation_get_modifier_values(nid, sys::national_mod_offsets::factory_maintenance) + 1.0f;
			auto input_mult = economy::nation_factory_input_multiplier(state, ftid, nid);
			auto output_mult = economy::nation_factory_output_multiplier(state, ftid, nid);

			float total = 0.f;
			for(unsigned i = 0; i < inputs.set_size; i++) {
				if(inputs.commodity_type[i]) {
					auto cid = inputs.commodity_type[i];
					total -= inputs.commodity_amounts[i] * input_mult * state.world.market_get_price(mid, cid);
				}
			}
			for(unsigned i = 0; i < e_inputs.set_size; i++) {
				if(e_inputs.commodity_type[i]) {
					auto cid = e_inputs.commodity_type[i];
					total -= e_inputs.commodity_amounts[i] * input_mult * e_input_mult * state.world.market_get_price(mid, cid);
				}
			}

			auto cid = state.world.factory_type_get_output(ftid).id;
			total += state.world.factory_type_get_output_amount(ftid) * output_mult * state.world.market_get_price(mid, cid);
			state.iui_state.per_market_data[market.index()] = total;

			cut_away_negative = true;
			start_min_at_zero = true;
			scaling = scaling_mode::log;
		});

		state.world.for_each_nation([&](dcon::nation_id nid) {
			if(economy::priority_multiplier(state, ftid, nid) < 0.85f) {
				state.iui_state.input_efficiency_leaders.push_back(nid);
			}
		});
	} else if(state.iui_state.tab == iui::iui_tab::wages) {
		if(state.iui_state.national_data) {
			state.world.for_each_nation([&](dcon::nation_id n) {
				auto exists = (state.world.nation_get_owned_province_count(n) != 0);
				if(!exists) {
					return;
				}

				float total_price = 0.f;
				float total_demand = 0.f;
				float total_supply = 0.f;
				float count = 0.f;
				state.world.nation_for_each_province_ownership(n, [&](auto poid) {
					auto pid = state.world.province_ownership_get_province(poid);
					total_price += state.world.province_get_labor_price(pid, state.iui_state.selected_labor_type)
						* state.world.province_get_labor_demand(pid, state.iui_state.selected_labor_type);
					total_demand += state.world.province_get_labor_demand(pid, state.iui_state.selected_labor_type);
					total_supply += state.world.province_get_labor_supply(pid, state.iui_state.selected_labor_type);
					count += 1.f;
				});

				switch(state.iui_state.selected_labor_info) {
				case iui::labor_info_mode::price:
					state.iui_state.per_nation_data[n.index()] =
						total_price * 10'000.f / (total_demand + 0.0001f);
					break;
				case iui::labor_info_mode::demand:
					state.iui_state.per_nation_data[n.index()] =
						total_demand;
					break;
				case iui::labor_info_mode::supply:
					state.iui_state.per_nation_data[n.index()] =
						total_supply;
					break;
				case iui::labor_info_mode::supply_demand_ratio:
					balance_color = true;
					state.iui_state.per_nation_data[n.index()] =
						(total_supply + 0.0001f)
						/ (total_demand + 0.0001f);
					break;
				default:
					break;
				}
			});
		} else {
			state.world.for_each_province([&](dcon::province_id pid) {
				switch(state.iui_state.selected_labor_info) {
				case iui::labor_info_mode::price:
					state.iui_state.per_province_data[pid.index()] =
						state.world.province_get_labor_price(pid, state.iui_state.selected_labor_type) * 10'000.f;
					break;
				case iui::labor_info_mode::demand:
					state.iui_state.per_province_data[pid.index()] =
						state.world.province_get_labor_demand(pid, state.iui_state.selected_labor_type);
					break;
				case iui::labor_info_mode::supply:
					state.iui_state.per_province_data[pid.index()] =
						state.world.province_get_labor_supply(pid, state.iui_state.selected_labor_type);
					break;
				case iui::labor_info_mode::supply_demand_ratio:
					balance_color = true;
					state.iui_state.per_province_data[pid.index()] =
						(state.world.province_get_labor_supply(pid, state.iui_state.selected_labor_type) + 0.0001f)
						/ (state.world.province_get_labor_demand(pid, state.iui_state.selected_labor_type) + 0.0001f);
					break;
				default:
					break;
				}
			});
		}
	} else if(state.iui_state.tab == iui::iui_tab::trade_volume) {
		
		if(state.iui_state.national_data) {
			dcon::nation_id origin{ };
			if(state.map_state.selected_province) {
				origin = state.world.province_get_nation_from_province_ownership(state.map_state.selected_province);
			}

			state.world.for_each_nation([&](dcon::nation_id n) {
				auto exists = (state.world.nation_get_owned_province_count(n) != 0);
				if(!exists) {
					return;
				}
				switch(state.iui_state.selected_trade_info) {
				case iui::trade_volume_info_mode::exported_volume:
					do_not_cut_away_values = true;
					if(origin) {
						state.iui_state.per_nation_data[n.index()] = economy::trade_value_flow(state, origin, n);
					} else {
						state.iui_state.per_nation_data[n.index()] = economy::export_value(state, n);
					}
					break;

				case iui::trade_volume_info_mode::imported_volume:
					do_not_cut_away_values = true;
					if(origin) {
						state.iui_state.per_nation_data[n.index()] = economy::trade_value_flow(state, n, origin);
					} else {
						state.iui_state.per_nation_data[n.index()] = economy::import_value(state, n);
					}
					break;

				case iui::trade_volume_info_mode::total_volume:
					do_not_cut_away_values = true;
					if(origin) {
						state.iui_state.per_nation_data[n.index()] =
							economy::trade_value_flow(state, n, origin) + economy::trade_value_flow(state, origin, n);
					} else {
						state.iui_state.per_nation_data[n.index()] =
							economy::import_value(state, n) + economy::export_value(state, n);
					}
					break;

				case iui::trade_volume_info_mode::trade_balance:
					do_not_cut_away_values = true;
					balance_color = true;
					if(origin) {
						state.iui_state.per_nation_data[n.index()] =
							-economy::trade_value_flow(state, n, origin) + economy::trade_value_flow(state, origin, n);
					} else {
						state.iui_state.per_nation_data[n.index()] =
							-economy::import_value(state, n) + economy::export_value(state, n);
					}
					break;

				case iui::trade_volume_info_mode::embargo:
					do_not_cut_away_values = true;
					balance_color = true;
					if(origin) {
						if(origin == n) {
							state.iui_state.per_nation_data[n.index()] = -1.f;
						} else if(economy::embargo_exists(state, origin, n).combined) {
							state.iui_state.per_nation_data[n.index()] = 1.f;
						} else {
							state.iui_state.per_nation_data[n.index()] = 0.f;
						}
					} else {
						state.iui_state.per_nation_data[n.index()] = 0.f;
					}
					break;

				default:
					break;
				}
			});
		} else {
			dcon::market_id origin{ };
			dcon::nation_id origin_n{ };
			if(state.map_state.selected_province) {
				auto sid = state.world.province_get_state_membership(state.map_state.selected_province);
				origin = state.world.state_instance_get_market_from_local_market(sid);
				origin_n = state.world.province_get_nation_from_province_ownership(state.map_state.selected_province);
			}

			state.world.for_each_market([&](dcon::market_id target) {
				auto target_s = state.world.market_get_zone_from_local_market(target);
				auto target_n = state.world.state_instance_get_nation_from_state_ownership(target_s);
				switch(state.iui_state.selected_trade_info) {
				case iui::trade_volume_info_mode::exported_volume:
					do_not_cut_away_values = true;
					if(origin) {
						state.iui_state.per_market_data[target.index()] = economy::trade_value_flow(state, origin, target);
					} else {
						state.iui_state.per_market_data[target.index()] = economy::export_value(state, target);
					}
					break;

				case iui::trade_volume_info_mode::imported_volume:
					do_not_cut_away_values = true;
					if(origin) {
						state.iui_state.per_market_data[target.index()] = economy::trade_value_flow(state, target, origin);
					} else {
						state.iui_state.per_market_data[target.index()] = economy::import_value(state, target);
					}
					break;

				case iui::trade_volume_info_mode::total_volume:
					do_not_cut_away_values = true;
					if(origin) {
						state.iui_state.per_market_data[target.index()] =
							economy::trade_value_flow(state, target, origin) + economy::trade_value_flow(state, origin, target);
					} else {
						state.iui_state.per_market_data[target.index()] =
							economy::import_value(state, target) + economy::export_value(state, target);
					}
					break;

				case iui::trade_volume_info_mode::trade_balance:
					do_not_cut_away_values = true;
					balance_color = true;
					if(origin) {
						state.iui_state.per_market_data[target.index()] =
							-economy::trade_value_flow(state, target, origin) + economy::trade_value_flow(state, origin, target);
					} else {
						state.iui_state.per_market_data[target.index()] =
							-economy::import_value(state, target) + economy::export_value(state, target);
					}
					break;

				case iui::trade_volume_info_mode::embargo:
					do_not_cut_away_values = true;
					balance_color = true;
					if(origin) {
						if(origin_n == target_n) {
							state.iui_state.per_market_data[target.index()] = -1.f;
						} else if(economy::embargo_exists(state, origin_n, target_n).combined) {
							state.iui_state.per_market_data[target.index()] = 1.f;
						} else {
							state.iui_state.per_market_data[target.index()] = 0.f;
						}
					} else {
						state.iui_state.per_market_data[target.index()] = 0.f;
					}
					break;
				default:
					break;
				}
			});
		}
	} else if(state.selected_trade_good && state.iui_state.tab == iui::iui_tab::commodities_markets) {
		if(state.iui_state.national_data) {
			state.world.for_each_nation([&](dcon::nation_id n) {
				auto exists = (state.world.nation_get_owned_province_count(n) != 0);
				if(!exists) {
					return;
				}
				switch(state.iui_state.selected_commodity_info) {
				case iui::commodity_info_mode::price:
					state.iui_state.per_nation_data[n.index()] = economy::median_price(state, n, state.selected_trade_good);
					break;

				case iui::commodity_info_mode::supply:
					state.iui_state.per_nation_data[n.index()] = economy::supply(state, n, state.selected_trade_good);
					break;

				case iui::commodity_info_mode::demand:
					state.iui_state.per_nation_data[n.index()] = economy::demand(state, n, state.selected_trade_good);
					break;

				case iui::commodity_info_mode::production:
					state.iui_state.per_nation_data[n.index()] =
						std::max(
							0.f,
							economy::supply(state, n, state.selected_trade_good)
							- economy::trade_supply(state, n, state.selected_trade_good)
						);
					break;

				case iui::commodity_info_mode::consumption:
					state.iui_state.per_nation_data[n.index()] =
						std::max(
							0.f,
							economy::demand(state, n, state.selected_trade_good)
							- economy::trade_demand(state, n, state.selected_trade_good)
						);
					break;

				case iui::commodity_info_mode::stockpiles:
					state.iui_state.per_nation_data[n.index()] = economy::stockpile(state, n, state.selected_trade_good);
					break;

				case iui::commodity_info_mode::potentials:
				{
					state.iui_state.per_nation_data[n.index()] = (float) economy::calculate_nation_factory_limit(state, n, state.selected_trade_good);
					break;
				}

				case iui::commodity_info_mode::balance:
				{
					auto supply = economy::supply(state, n, state.selected_trade_good);
					auto demand = economy::demand(state, n, state.selected_trade_good);
					auto shift = 0.001f;
					auto balance = (supply + shift) / (demand + shift) - (demand + shift) / (supply + shift);
					state.iui_state.per_nation_data[n.index()] = balance;
					balance_color = true;
					scaling = scaling_mode::log;
					break;
				}

				case iui::commodity_info_mode::trade_in:
					state.iui_state.per_nation_data[n.index()] = economy::import_volume(state, n, state.selected_trade_good);
					break;

				case iui::commodity_info_mode::trade_out:
					state.iui_state.per_nation_data[n.index()] = economy::export_volume(state, n, state.selected_trade_good);
					break;

				case iui::commodity_info_mode::trade_balance:
				{
					auto supply = economy::import_volume(state, n, state.selected_trade_good);
					auto demand = economy::export_volume(state, n, state.selected_trade_good);
					auto shift = 0.001f;
					auto balance = supply - demand;
					state.iui_state.per_nation_data[n.index()] = balance;
					balance_color = true;
					break;
				}
				default:
					break;
				}
			});
		} else {
			state.world.for_each_market([&](dcon::market_id market) {
				switch(state.iui_state.selected_commodity_info) {
					case iui::commodity_info_mode::price:
						state.iui_state.per_market_data[market.index()] = state.world.market_get_price(market, state.selected_trade_good);
						break;

					case iui::commodity_info_mode::supply:
						state.iui_state.per_market_data[market.index()] = state.world.market_get_supply(market, state.selected_trade_good);
						break;

					case iui::commodity_info_mode::demand:
						state.iui_state.per_market_data[market.index()] = state.world.market_get_demand(market, state.selected_trade_good);
						break;

					case iui::commodity_info_mode::production:
						state.iui_state.per_market_data[market.index()] =
							std::max(
								0.f,
								state.world.market_get_supply(market, state.selected_trade_good)
								- economy::trade_supply(state, market, state.selected_trade_good)
							);
						break;

					case iui::commodity_info_mode::consumption:
						state.iui_state.per_market_data[market.index()] =
							std::max(
								0.f,
								state.world.market_get_demand(market, state.selected_trade_good)
								- economy::trade_demand(state, market, state.selected_trade_good)
							);
						break;

					case iui::commodity_info_mode::stockpiles:
						state.iui_state.per_market_data[market.index()] = state.world.market_get_stockpile(market, state.selected_trade_good);
						break;

					case iui::commodity_info_mode::potentials:
					{
						auto loc = state.world.market_get_zone_from_local_market(market);
						state.iui_state.per_market_data[market.index()] = (float) economy::calculate_state_factory_limit(state, loc, state.selected_trade_good);
						cut_away_negative = true;
						start_min_at_zero = true;
						break;
					}

					case iui::commodity_info_mode::balance:
					{
						auto supply = state.world.market_get_supply(market, state.selected_trade_good);
						auto demand = state.world.market_get_demand(market, state.selected_trade_good);
						auto shift = 0.001f;
						auto balance = (supply + shift) / (demand + shift) - (demand + shift) / (supply + shift);
						state.iui_state.per_market_data[market.index()] = balance;
						balance_color = true;
						scaling = scaling_mode::log;
						break;
					}

					case iui::commodity_info_mode::trade_in:
						state.iui_state.per_market_data[market.index()] = economy::trade_influx(state, market, state.selected_trade_good);
						break;

					case iui::commodity_info_mode::trade_out:
						state.iui_state.per_market_data[market.index()] = economy::trade_outflux(state, market, state.selected_trade_good);
						break;

					case iui::commodity_info_mode::trade_balance:
					{
						auto supply = economy::trade_influx(state, market, state.selected_trade_good);
						auto demand = economy::trade_outflux(state, market, state.selected_trade_good);
						auto shift = 0.001f;
						auto balance = supply - demand;
						state.iui_state.per_market_data[market.index()] = balance;
						balance_color = true;
						break;
					}
					default:
						break;
				}
			});
		}
	} else {
		state.world.for_each_market([&](dcon::market_id market) {
			state.iui_state.per_market_data[market.index()] = state.world.market_get_gdp(market);
		});
	}

	// calculate statistics to improve map mode readability:
	std::vector<float> sample;
	if(state.iui_state.national_data) {
		state.world.for_each_nation([&](dcon::nation_id n) {
			auto exists = (state.world.nation_get_owned_province_count(n) != 0);
			if(exists) {
				sample.push_back(state.iui_state.per_nation_data[n.index()]);
			}
		});
	} else {
		if(state.iui_state.tab == iui::iui_tab::wages) {
			state.world.for_each_province([&](dcon::province_id pid) {
				sample.push_back(state.iui_state.per_province_data[pid.index()]);
			});
		} else {
			state.world.for_each_market([&](dcon::market_id market) {
				sample.push_back(state.iui_state.per_market_data[market.index()]);
			});
		}
	}

	std::sort(sample.begin(), sample.end());
	auto N = sample.size();
	auto upper_decile = sample[9 * N / 10];
	auto bottom_decile = sample[N / 10];
	if(do_not_cut_away_values) {
		upper_decile = sample[N - 1];
		bottom_decile = sample[0];
	}
	auto main_range = upper_decile - bottom_decile;

	state.iui_state.bins_start = bottom_decile - main_range * 2.f;
	state.iui_state.bins_end = upper_decile + main_range * 2.f;

	if(do_not_cut_away_values) {
		state.iui_state.bins_start = bottom_decile;
		state.iui_state.bins_end = upper_decile;
	}

	auto bin_range = (state.iui_state.bins_end - state.iui_state.bins_start) / float(bins);

	// fill histogram
	state.iui_state.bins.clear();
	state.iui_state.bins.resize(bins);
	for(auto data_point : sample) {
		auto clamped = std::clamp(data_point, state.iui_state.bins_start, state.iui_state.bins_end);
		int bin = std::clamp(int((data_point - state.iui_state.bins_start) / bin_range), 0, bins - 1);
		state.iui_state.bins[bin] += 1.f / float(N);
	}

	auto market_data_max = upper_decile * 2.f;
	auto market_data_min = bottom_decile;

	if(balance_color) {
		uint32_t split = 0;
		while(split < N && sample[split] < 0.f) split++;

		auto max_index = std::min((split + 9 * N) / 10, sample.size() - 1);
		if(max_index < 0) {
			max_index = 0;
		}

		market_data_max = sample[max_index];
		market_data_min = sample[split / 10];
	}

	if(start_min_at_zero) {
		market_data_min = 0.f;
	}

	if(cut_away_negative && !do_not_cut_away_values) {
		market_data_max = std::max(0.f, market_data_max);
		market_data_min = std::max(0.f, market_data_min);
	}

	if(do_not_cut_away_values) {
		market_data_min = bottom_decile;
		market_data_max = upper_decile;
	}

	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);

	if(balance_color) {
		state.world.for_each_province([&](dcon::province_id pid) {
			auto sid = state.world.province_get_state_membership(pid);
			auto market = state.world.state_instance_get_market_from_local_market(sid);
			auto owner = state.world.state_instance_get_nation_from_state_ownership(sid);

			if(market) {
				// scale negative and positive values separately
				float value = state.iui_state.per_market_data[market.index()];
				if(state.iui_state.national_data) {
					value = state.iui_state.per_nation_data[owner.index()];
				} else {
					if(state.iui_state.tab == iui::iui_tab::wages) {
						value = state.iui_state.per_province_data[pid.index()];
					}
				}
				if(scaling == scaling_mode::log) {
					if(value > 0 && market_data_max > 0) {
						value = std::min(std::log(1.f + value / market_data_max * (float(std::numbers::e) - 1.f)), 1.f);
					} else if(value < 0 && market_data_min < 0) {
						value = std::max(std::log(1.f - value / std::abs(market_data_min) * (float(std::numbers::e) - 1.f)) , -1.f);
					} else {
						value = 0.f;
					}
				} else {
					if(value > 0 && market_data_max > 0) {
						value = std::min(value / market_data_max, 1.f);
					} else if(value < 0 && market_data_min < 0) {
						value = std::max(value / std::abs(market_data_min), -1.f);
					} else {
						value = 0.f;
					}
				}
				uint32_t color = ogl::color_gradient_diverging_berlin(value);
				auto i = province::to_map_id(pid);
				prov_color[i] = color;
				prov_color[i + texture_size] = color;
			}
		});
	} else if(market_data_max > market_data_min) {
		state.world.for_each_province([&](dcon::province_id pid) {
			auto sid = state.world.province_get_state_membership(pid);
			auto market = state.world.state_instance_get_market_from_local_market(sid);
			auto owner = state.world.state_instance_get_nation_from_state_ownership(sid);

			if(market) {
				float original_value = state.iui_state.per_market_data[market.index()];
				if(state.iui_state.national_data) {
					original_value = state.iui_state.per_nation_data[owner.index()];
				} else {
					if(state.iui_state.tab == iui::iui_tab::wages) {
						original_value = state.iui_state.per_province_data[pid.index()];
					}
				}

				double value = double(std::clamp(original_value, market_data_min, market_data_max));
				double rescaled_value = (value - market_data_min) / (market_data_max - market_data_min);
				// from 0 to 1

				if(scaling == scaling_mode::log) {
					rescaled_value = std::log((rescaled_value * (std::numbers::e - 1) + 1));
				}

				uint32_t color = ogl::color_gradient_magma(float(rescaled_value));
				auto i = province::to_map_id(pid);
				prov_color[i] = color;
				prov_color[i + texture_size] = color;
			}
		});
	}

	state.map_state.set_province_color(prov_color, map_mode::mode::handled_from_outside);
}

void commodity_panel(sys::state& state, int32_t identifier_amount, int32_t identifier_cost, dcon::commodity_id cid, float amount, float price, iui::rect& r) {
	auto icon_size = 20.f;
	ogl::render_commodity_icon(
		state, cid,
		r.x + 5.f, r.y + r.h / 2.f - icon_size / 2.f, icon_size, icon_size
	);
	iui::rect data{ r.x + 15.f, r.y, r.w - 18.f, r.h / 2.f };
	state.iui_state.float_2(
		state, identifier_amount,
		data, amount
	);
	data.y += r.h / 2.f;
	state.iui_state.price(
		state, identifier_cost,
		data, amount * price
	);
}

void render(sys::state& state) {
	if(state.iui_state.per_market_data.size() != state.world.market_size() + 1) {
		state.iui_state.per_market_data.resize(state.world.market_size() + 1);
	}
	if(state.iui_state.per_nation_data.size() != state.world.nation_size() + 1) {
		state.iui_state.per_nation_data.resize(state.world.nation_size() + 1);
	}
	if(state.iui_state.per_province_data.size() != state.world.province_size() + 1) {
		state.iui_state.per_province_data.resize(state.world.province_size() + 1);
	}

	state.iui_state.frame_start(state);
	

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(state.open_gl.ui_shader_program);
	glUniform1i(state.open_gl.ui_shader_texture_sampler_uniform, 0);
	glUniform1i(state.open_gl.ui_shader_secondary_texture_sampler_uniform, 1);
	glUniform1f(state.open_gl.ui_shader_screen_width_uniform, float(state.x_size) / state.user_settings.ui_scale);
	glUniform1f(state.open_gl.ui_shader_screen_height_uniform, float(state.y_size) / state.user_settings.ui_scale);
	glUniform1f(state.open_gl.ui_shader_gamma_uniform, state.user_settings.gamma);
	glViewport(0, 0, state.x_size, state.y_size);
	glDepthRange(-1.0f, 1.0f);

	auto zoom = state.map_state.zoom;
	auto screen_size = glm::vec2(state.x_size, state.y_size) / state.user_settings.ui_scale;

	// render market data
	if(zoom > map::zoom_close || (zoom > map::zoom_close / 2.f && state.iui_state.national_data && state.iui_state.tab == iui::iui_tab::commodities_markets)) {
		iui::rect market_label_rect{ 0.f, 0.f, state.iui_state.map_label.w, state.iui_state.map_label.h };
		if(state.iui_state.tab == iui::iui_tab::trade_volume) {
			if(state.iui_state.selected_trade_info == iui::trade_volume_info_mode::embargo) {
				market_label_rect = { 0.f, 0.f, state.iui_state.map_label.w * 2, state.iui_state.map_label.h * 2 };
			}
		}
		iui::rect market_label_rect_text = market_label_rect;
		iui::shrink(market_label_rect_text, 2.f);
		market_label_rect_text.w -= 5.f;


		state.world.for_each_province([&](dcon::province_id pid) {
			auto sid = state.world.province_get_state_membership(pid);
			auto mid = state.world.state_instance_get_market_from_local_market(sid);

			auto& midpoint = state.world.province_get_mid_point(pid);
			auto map_pos = state.map_state.normalize_map_coord(midpoint);
			auto owner = state.world.state_instance_get_nation_from_state_ownership(sid);

			auto capital = state.world.state_instance_get_capital(sid);

			if(state.iui_state.national_data) {
				auto nation_capital = state.world.nation_get_capital(owner);
				if(nation_capital != pid) {
					return;
				}
			} else {
				if(state.iui_state.tab != iui::iui_tab::wages) {
					if(pid != capital) {
						return;
					}
				}
			}

			glm::vec2 screen_pos{};
			if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos, { 200.f, 200.f })) {
				return;
			}

			iui::move_to(
				market_label_rect,
				screen_pos.x - market_label_rect.w / 2.f, screen_pos.y - market_label_rect.h / 2.f
			);

			iui::move_to(
				market_label_rect_text,
				screen_pos.x - market_label_rect.w / 2.f + 5.f, screen_pos.y - market_label_rect.h / 2.f + 2.f
			);

			

			float value = 0.f;
			if(mid) {
				value = state.iui_state.per_market_data[mid.index()];
			}
			if(state.iui_state.national_data) {
				if (owner)
					value = state.iui_state.per_nation_data[owner.index()];
			} else {
				if(state.iui_state.tab == iui::iui_tab::wages) {
					value = state.iui_state.per_province_data[pid.index()];
				}
			}

			bool draw_panel = true;

			if(state.iui_state.tab == iui::iui_tab::trade_volume) {
				if(state.iui_state.selected_trade_info == iui::trade_volume_info_mode::embargo) {
					if(value < 1.f) {
						draw_panel = false;
					}
				}
			}

			if (draw_panel) {
				state.iui_state.panel_textured(state, market_label_rect, state.iui_state.map_label.texture_handle);
			}

			if(state.iui_state.tab == iui::iui_tab::factory_types) {
				if(mid.index() < (int32_t)(state.iui_state.per_market_data.size())) {
					state.iui_state.price(
						state, mid.index(),
						market_label_rect_text,
						value
					);
				}
			} else if(state.iui_state.tab == iui::iui_tab::commodities_markets) {
				if(state.iui_state.selected_commodity_info == iui::commodity_info_mode::price) {
					state.iui_state.price(
						state, mid.index(),
						market_label_rect_text,
						value
					);
				} else {
					state.iui_state.float_2(
						state, mid.index(),
						market_label_rect_text,
						value
					);
				}
			} else if(state.iui_state.tab == iui::iui_tab::wages) {
				if(state.iui_state.selected_labor_info == iui::labor_info_mode::price) {
					state.iui_state.price(
						state, pid.index(),
						market_label_rect_text,
						value
					);
				} else {
					state.iui_state.float_2(
						state, pid.index(),
						market_label_rect_text,
						value
					);
				}
			} else if(state.iui_state.tab == iui::iui_tab::trade_volume) {
				if(state.iui_state.selected_trade_info != iui::trade_volume_info_mode::embargo) {
					state.iui_state.price(
						state, pid.index(),
						market_label_rect_text,
						value
					);
				} else {
					if(value == 1.f) {
						state.iui_state.localized_string(
							state, pid.index(), market_label_rect_text, "economy_scene_embargo"
						);

						dcon::nation_id origin{ };
						if(state.map_state.selected_province) {
							origin = state.world.province_get_nation_from_province_ownership(state.map_state.selected_province);
						}

						auto embargo_details = economy::embargo_exists(state, origin, owner);

						// add explanation:
						if (embargo_details.war) {
							market_label_rect.y += market_label_rect.h;
							market_label_rect_text.y += market_label_rect.h;
							state.iui_state.panel_textured(state, market_label_rect, state.iui_state.map_label.texture_handle);
							state.iui_state.localized_string(
								state, pid.index(), market_label_rect_text, "embargo_reason_war"
							);
						}
						if(embargo_details.origin_embargo) {
							market_label_rect.y += market_label_rect.h;
							market_label_rect_text.y += market_label_rect.h;
							state.iui_state.panel_textured(state, market_label_rect, state.iui_state.map_label.texture_handle);
							state.iui_state.localized_string(
								state, pid.index(), market_label_rect_text, "embargo_reason_origin_embargo"
							);
						}
						if(embargo_details.origin_join_embargo) {
							market_label_rect.y += market_label_rect.h;
							market_label_rect_text.y += market_label_rect.h;
							state.iui_state.panel_textured(state, market_label_rect, state.iui_state.map_label.texture_handle);
							state.iui_state.localized_string(
								state, pid.index(), market_label_rect_text, "embargo_reason_origin_join_embargo"
							);
						}
						if(embargo_details.target_embargo) {
							market_label_rect.y += market_label_rect.h;
							market_label_rect_text.y += market_label_rect.h;
							state.iui_state.panel_textured(state, market_label_rect, state.iui_state.map_label.texture_handle);
							state.iui_state.localized_string(
								state, pid.index(), market_label_rect_text, "embargo_reason_target_embargo"
							);
						}
						if(embargo_details.target_join_embargo) {
							market_label_rect.y += market_label_rect.h;
							market_label_rect_text.y += market_label_rect.h;
							state.iui_state.panel_textured(state, market_label_rect, state.iui_state.map_label.texture_handle);
							state.iui_state.localized_string(
								state, pid.index(), market_label_rect_text, "embargo_reason_target_join_embargo"
							);
						}
					}
				}
			}
		});
	}

	{
		float tabs_layout_start = 0.f;
		float tabs_layout_margin = 0.f;
		float tab_height = state.iui_state.top_bar_button.h;
		float tab_width = state.iui_state.top_bar_button.w;

		iui::rect tab_rect{ tabs_layout_start, tabs_layout_margin, tab_width, tab_height };

		iui::rect tab_name_rect = tab_rect;
		iui::shrink(tab_name_rect, 10.f);

		if(state.iui_state.button_textured(
			state, (int32_t)(static_elements::commodities_button),
			tab_rect, 3, state.iui_state.top_bar_button.texture_handle,
			state.iui_state.tab == iui::iui_tab::commodities_markets
		)) {
			state.iui_state.tab = iui::iui_tab::commodities_markets;
			update(state);
		}

		state.iui_state.localized_string(
			state, (int32_t)static_elements::tab_name_commodity, tab_name_rect, "alice_commodity_tab",
			ui::get_text_color(state, text::text_color::gold)
		);

		iui::rect toggle = tab_rect;

		toggle.y += toggle.h;
		toggle.h = 20.f;

		if(state.iui_state.button_textured(
			state, (int32_t)(static_elements::nation_vs_market_toggle),
			toggle, 3, state.iui_state.top_bar_button.texture_handle,
			state.iui_state.national_data
		)) {
			state.iui_state.national_data = !state.iui_state.national_data;
			update(state);
		}

		state.iui_state.localized_string(
			state, (int32_t)static_elements::nation_vs_market_toggle_label, toggle, "per_nation",
			ui::get_text_color(state, text::text_color::gold)
		);

		tab_rect.x += tab_width + tabs_layout_margin;
		tab_name_rect.x += tab_width + tabs_layout_margin;

		if(state.iui_state.button_textured(
			state, (int32_t)(static_elements::factory_types_button),
			tab_rect, 3, state.iui_state.top_bar_button.texture_handle,
			state.iui_state.tab == iui::iui_tab::factory_types
		)) {
			state.iui_state.tab = iui::iui_tab::factory_types;
			update(state);
		}

		state.iui_state.localized_string(
			state, (int32_t)static_elements::tab_name_commodity, tab_name_rect, "alice_factory_type_tab",
			ui::get_text_color(state, text::text_color::gold)
		);

		tab_rect.x += tab_width + tabs_layout_margin;
		tab_name_rect.x += tab_width + tabs_layout_margin;

		if(state.iui_state.button_textured(
			state, (int32_t)(static_elements::wages_tab),
			tab_rect, 3, state.iui_state.top_bar_button.texture_handle,
			state.iui_state.tab == iui::iui_tab::wages
		)) {
			state.iui_state.tab = iui::iui_tab::wages;
			update(state);
		}
		state.iui_state.localized_string(
			state, (int32_t)static_elements::wages_tab_label, tab_name_rect, "alice_wages_tab",
			ui::get_text_color(state, text::text_color::gold)
		);

		tab_rect.x += tab_width + tabs_layout_margin;
		tab_name_rect.x += tab_width + tabs_layout_margin;

		if(state.iui_state.button_textured(
			state, (int32_t)(static_elements::trade_volume_tab),
			tab_rect, 3, state.iui_state.top_bar_button.texture_handle,
			state.iui_state.tab == iui::iui_tab::trade_volume
		)) {
			state.iui_state.tab = iui::iui_tab::trade_volume;
			update(state);
		}
		state.iui_state.localized_string(
			state, (int32_t)static_elements::trade_volume_tab_label, tab_name_rect, "alice_trade_volume_tab",
			ui::get_text_color(state, text::text_color::gold)
		);
	}

	if(state.iui_state.tab == iui::iui_tab::commodities_markets) {

		// absolute
		float size_panel_w = 400.f;
		float size_panel_h = 500.f;
		float margin = 5.f;
		float size_selector_w = 150.f;
		float size_page_selector_h = 50.f;
		float top = screen_size.y - size_panel_h;
		float priority_settings_height = 80.f;

		// calculated
		float size_view_w = size_panel_w - size_selector_w;
		iui::rect selector_panel = { margin, top, size_selector_w, size_panel_h - size_page_selector_h };
		iui::rect page_selector_rect = { margin, top + size_panel_h - size_page_selector_h, size_selector_w, size_page_selector_h };
		iui::rect view_panel = { margin + size_selector_w, top, size_view_w, size_panel_h };

		iui::shrink(selector_panel, margin);
		iui::shrink(page_selector_rect, margin);
		iui::shrink(view_panel, margin);

		uint32_t items_on_page = 10;
		iui::rect selector_button = selector_panel;
		selector_button.h = selector_panel.h / float(items_on_page);
		auto page = state.iui_state.page_commodities;

		{
			auto icon_size = 20.f;
			state.iui_state.panel(state, selector_panel);
			float y = 10.f;
			for(
				uint32_t i = 0;
				i < items_on_page;
				i++
			) {
				auto item_index = i + items_on_page * page;
				if(item_index >= state.world.commodity_size()) {
					break;
				}
				dcon::commodity_id cid{ (dcon::commodity_id::value_base_t)item_index };
				selector_button.y = selector_panel.y + i * selector_button.h;
				if(state.iui_state.button_textured(
					state, (int32_t)static_elements::commodities + cid.index(),
					selector_button,
					3, state.iui_state.item_selector_bg.texture_handle,
					state.selected_trade_good == cid
				)) {
					if(state.selected_trade_good == cid) {
						state.selected_trade_good = { };
					} else {
						state.selected_trade_good = cid;
					}
					update(state);
					state.update_trade_flow.store(true, std::memory_order::release);
				}
				state.iui_state.localized_string_r(
					state, (int32_t)static_elements::commodities + cid.index(),
					{ selector_button.x + 30.f, selector_button.y, selector_button.w - 30.f, selector_button.h },
					text::produce_simple_string(state, state.world.commodity_get_name(cid))
				);
				ogl::render_commodity_icon(
					state, cid,
					selector_button.x + 5.f,
					selector_button.y + selector_button.h / 2.f - icon_size / 2.f, icon_size, icon_size
				);
			}

			state.iui_state.panel_textured(
				state, page_selector_rect, state.iui_state.page_selector_bg.texture_handle
			);

			int pages = (state.world.commodity_size() - 1) / items_on_page + 1;

			iui::rect page_button{
				0.f,
				page_selector_rect.y - state.iui_state.page_button.h / 2.f + page_selector_rect.h / 2.f,
				state.iui_state.page_button.w,
				state.iui_state.page_button.h
			};

			if(pages * state.iui_state.page_button.w <= page_selector_rect.w) {
				float margin_pages = (page_selector_rect.w - pages * page_button.w) / (pages + 1);

				for(int i = 0; i < pages; i++) {
					page_button.x = page_selector_rect.x + (page_button.w + margin_pages) * i + margin_pages;

					if(state.iui_state.button_textured(
						state,
						(int32_t)static_elements::commodities_page + i,
						page_button,
						3, state.iui_state.page_button.texture_handle,
						state.iui_state.page_commodities == i
					)) {
						state.iui_state.page_commodities = i;
					}

					state.iui_state.int_whole(
						state,
						(int32_t)static_elements::commodities_page_number + i,
						page_button, i
					);
				}
			} else {
				page_button.x = page_selector_rect.x + margin;
				if(state.iui_state.button_textured(
					state, (int32_t)static_elements::commodities_page,
					page_button, 3, state.iui_state.page_button.texture_handle, false
				)) {
					state.iui_state.page_commodities = std::max(0, state.iui_state.page_commodities - 1);
				}

				page_button.x = page_selector_rect.x + page_selector_rect.w - page_button.w - margin;
				if(state.iui_state.button_textured(
					state, (int32_t)static_elements::commodities_page + 2,
					page_button, 3, state.iui_state.page_button.texture_handle, false
				)) {
					state.iui_state.page_commodities = std::min(
						pages - 1, state.iui_state.page_commodities + 1
					);
				}

				iui::rect page_selector_page_current = page_selector_rect;
				page_selector_page_current.x = page_selector_rect.x + page_selector_rect.w / 4.f;
				page_selector_page_current.w = page_selector_rect.w / 2.f;
				page_selector_page_current.y = page_selector_rect.y + margin;
				page_selector_page_current.h = page_selector_rect.h - margin * 2.f;

				page_selector_page_current.w /= 2.f;
				state.iui_state.int_whole(
					state,
					(int32_t)static_elements::commodities_page_number + 1,
					page_selector_page_current, state.iui_state.page_commodities
				);

				page_selector_page_current.x += page_selector_page_current.w;
				state.iui_state.int_whole(
					state,
					(int32_t)static_elements::commodities_page_number + 2,
					page_selector_page_current, pages
				);
			}
		}

		// now we will render control buttons:

		float view_mode_height = 25.f;
		float view_mode_width = 120.f;
		for(uint8_t i = 0; i < uint8_t(iui::commodity_info_mode::total); i++) {
			iui::rect button_rect = { size_selector_w + 10.f, screen_size.y - 350.f + i * view_mode_height, view_mode_width, view_mode_height };

			// Don't show Potentials buttons if there are no potentials for this good
			if(!economy::get_commodity_uses_potentials(state, state.selected_trade_good) && (uint8_t)iui::commodity_info_mode::potentials == i) {
				continue;
			}
			if(state.iui_state.button_textured(
				state, (int32_t)(static_elements::commodities_mode_selector_button) + i,
				button_rect, 3, state.iui_state.top_bar_button.texture_handle,
				(uint8_t)state.iui_state.selected_commodity_info == i
			)) {
				state.iui_state.selected_commodity_info = (iui::commodity_info_mode)i;
				update(state);
			}

			state.iui_state.localized_string(
				state,
				(int32_t)(static_elements::commodities_mode_selector_button_label) + i,
				button_rect,
				iui::localize_commodity_info_mode((iui::commodity_info_mode)i),
				ui::get_text_color(state, text::text_color::gold)
			);
		}

	} else if(state.iui_state.tab == iui::iui_tab::factory_types) {

		// absolute
		float size_panel_w = 400.f;
		float size_panel_h = 500.f;
		float margin = 5.f;
		float size_selector_w = 150.f;
		float size_page_selector_h = 50.f;
		float top = screen_size.y - size_panel_h;
		float priority_settings_height = 80.f;

		// calculated
		float size_view_w = size_panel_w - size_selector_w;
		iui::rect selector_panel = { margin, top, size_selector_w, size_panel_h - size_page_selector_h };
		iui::rect page_selector_rect = { margin, top + size_panel_h - size_page_selector_h, size_selector_w, size_page_selector_h };
		iui::rect view_panel = { margin + size_selector_w, top, size_view_w, size_panel_h };

		iui::shrink(selector_panel, margin);
		iui::shrink(page_selector_rect, margin);
		iui::shrink(view_panel, margin);

		uint32_t factories_on_a_page = 10;
		iui::rect selector_button = selector_panel;
		selector_button.h = selector_panel.h / float(factories_on_a_page);
		auto page = state.iui_state.page_production_methods;


		{
			auto icon_size = 20.f;
			state.iui_state.panel(state, selector_panel);
			float y = 10.f;
			for(
				uint32_t i = 0;
				i < factories_on_a_page;
				i++
			) {
				auto factory_index = i + factories_on_a_page * page;
				if(factory_index >= state.world.factory_type_size()) {
					break;
				}
				dcon::factory_type_id ftid{ (dcon::factory_type_id::value_base_t)factory_index };
				selector_button.y = selector_panel.y + i * selector_button.h;
				if(state.iui_state.button_textured(
					state, (int32_t)static_elements::factory_type_selector + ftid.index(),
					selector_button,
					3, state.iui_state.item_selector_bg.texture_handle,
					state.selected_factory_type == ftid
				)) {
					if(state.selected_factory_type == ftid) {
						state.selected_factory_type = { };
					} else {
						state.selected_factory_type = ftid;
						update(state);
					}
				}
				state.iui_state.localized_string_r(
					state, (int32_t)static_elements::factory_types + ftid.index(),
					{ selector_button.x + 30.f, selector_button.y, selector_button.w - 30.f, selector_button.h },
					text::produce_simple_string(state, state.world.factory_type_get_name(ftid))
				);
				ogl::render_commodity_icon(
					state, state.world.factory_type_get_output(ftid),
					selector_button.x + 5.f,
					selector_button.y + selector_button.h / 2.f - icon_size / 2.f, icon_size, icon_size
				);
			}

			state.iui_state.panel_textured(
				state, page_selector_rect, state.iui_state.page_selector_bg.texture_handle
			);

			int pages = (state.world.factory_type_size() - 1) / factories_on_a_page + 1;

			iui::rect page_button{
				0.f,
				page_selector_rect.y - state.iui_state.page_button.h / 2.f + page_selector_rect.h / 2.f,
				state.iui_state.page_button.w,
				state.iui_state.page_button.h
			};

			if(pages * state.iui_state.page_button.w <= page_selector_rect.w) {
				float margin_pages = (page_selector_rect.w - pages * page_button.w) / (pages + 1);

				for(int i = 0; i < pages; i++) {
					page_button.x = page_selector_rect.x + (page_button.w + margin_pages) * i + margin_pages;

					if(state.iui_state.button_textured(
						state,
						(int32_t)static_elements::factory_type_page + i,
						page_button,
						3, state.iui_state.page_button.texture_handle,
						state.iui_state.page_production_methods == i
					)) {
						state.iui_state.page_production_methods = i;
					}

					state.iui_state.int_whole(
						state,
						(int32_t)static_elements::factory_type_page_number + i,
						page_button, i
					);
				}
			} else {
				page_button.x = page_selector_rect.x + margin;
				if(state.iui_state.button_textured(
					state, (int32_t)static_elements::factory_type_page,
					page_button, 3, state.iui_state.page_button.texture_handle, false
				)) {
					state.iui_state.page_production_methods = std::max(0, state.iui_state.page_production_methods - 1);
				}

				page_button.x = page_selector_rect.x + page_selector_rect.w - page_button.w - margin;
				if(state.iui_state.button_textured(
					state, (int32_t)static_elements::factory_type_page + 2,
					page_button, 3, state.iui_state.page_button.texture_handle, false
				)) {
					state.iui_state.page_production_methods = std::min(
						pages - 1, state.iui_state.page_production_methods + 1
					);
				}

				iui::rect page_selector_page_current = page_selector_rect;
				page_selector_page_current.x = page_selector_rect.x + page_selector_rect.w / 4.f;
				page_selector_page_current.w = page_selector_rect.w / 2.f;
				page_selector_page_current.y = page_selector_rect.y + margin;
				page_selector_page_current.h = page_selector_rect.h - margin * 2.f;

				page_selector_page_current.w /= 2.f;
				state.iui_state.int_whole(
					state,
					(int32_t)static_elements::factory_type_page_number + 1,
					page_selector_page_current, state.iui_state.page_production_methods
				);

				page_selector_page_current.x += page_selector_page_current.w;
				state.iui_state.int_whole(
					state,
					(int32_t)static_elements::factory_type_page_number + 2,
					page_selector_page_current, pages
				);
			}
		}

		if(state.selected_factory_type && state.map_state.selected_province) {
			auto ftid = state.selected_factory_type;
			auto sid = state.world.province_get_state_membership(state.map_state.selected_province);
			auto sdif = state.world.state_instance_get_definition(sid);
			auto nid = state.world.province_get_nation_from_province_ownership(state.map_state.selected_province);
			auto mid = state.world.state_instance_get_market_from_local_market(sid);

			auto e_input_mult = state.world.nation_get_modifier_values(nid, sys::national_mod_offsets::factory_maintenance) + 1.0f;
			auto input_mult = economy::nation_factory_input_multiplier(state, ftid, nid);
			auto output_mult = economy::nation_factory_output_multiplier(state, ftid, nid);

			state.iui_state.panel_textured(state, view_panel, state.iui_state.factory_type_bg.texture_handle);

			iui::rect name_labels{
				view_panel.x + 5.f, view_panel.y + 5.f, 200.f, 40.f
			};

			state.iui_state.panel_textured(state, name_labels, state.iui_state.factory_type_name_bg.texture_handle);

			state.iui_state.localized_string_r(
				state,
				(int32_t)static_elements::state_def_name,
				{ view_panel.x + 10.f, view_panel.y + 7.f, view_panel.w, 18.f },
				text::produce_simple_string(state, state.world.state_definition_get_name(sdif))
			);

			state.iui_state.localized_string_r(
				state,
				(int32_t)static_elements::factory_type_name,
				{ view_panel.x + 10.f, view_panel.y + 25.f, view_panel.w, 18.f },
				text::produce_simple_string(state, state.world.factory_type_get_name(ftid))
			);

			iui::rect close{
				view_panel.x + view_panel.w - state.iui_state.close_button.w,
				view_panel.y,
				state.iui_state.close_button.w,
				state.iui_state.close_button.h
			};
			if(state.iui_state.button_textured(
				state, (int32_t)static_elements::close_factory_type_description, close,
				3, state.iui_state.close_button.texture_handle, false
			)) {
				state.selected_factory_type = { };
			}

			//retrieve data
			auto& inputs = state.world.factory_type_get_inputs(ftid);
			int inputs_size = 0;
			for(unsigned i = 0; i < inputs.set_size; i++) {
				if(inputs.commodity_type[i]) {
					inputs_size++;
				}
			}
			auto& e_inputs = state.world.factory_type_get_efficiency_inputs(ftid);
			int e_inputs_size = 0;
			for(unsigned i = 0; i < e_inputs.set_size; i++) {
				if(e_inputs.commodity_type[i]) {
					e_inputs_size++;
				}
			}

			float grid_margin = 5.f;
			int items_per_row = 3;

			iui::rect commodity_rect{ 0.f, 0.f, (view_panel.w - grid_margin) / float(items_per_row) - grid_margin, 40.f };
			iui::rect commodity_labels{ 0.f, 0.f, ((view_panel.w - grid_margin) / float(items_per_row) - grid_margin) - 5.f, 40.f };

			int rows_input = ((inputs_size - 1) / items_per_row) + 1;
			int rows_e_input = ((e_inputs_size - 1) / items_per_row) + 1;

			// data about production method
			{
				// inputs
				float total = 0.f;

				float current_y = view_panel.y + 50.f;

				for(unsigned i = 0; i < inputs.set_size; i++) {
					if(inputs.commodity_type[i]) {
						auto cid = inputs.commodity_type[i];

						int row = i / items_per_row;
						int column = i - row * items_per_row;
						commodity_rect.x = view_panel.x + grid_margin + column * (grid_margin + commodity_rect.w);
						commodity_rect.y = current_y + grid_margin + row * (grid_margin + commodity_rect.h);

						commodity_labels.x = commodity_rect.x;
						commodity_labels.y = commodity_rect.y;

						state.iui_state.panel_textured(state, commodity_rect, state.iui_state.commodity_bg.texture_handle);
						commodity_panel(
							state,
							(int32_t)static_elements::commodities_inputs_amount + cid.index(),
							(int32_t)static_elements::commodities_inputs_cost + cid.index(),
							cid,
							inputs.commodity_amounts[i] * input_mult,
							state.world.market_get_price(mid, cid),
							commodity_labels
						);

						total -= inputs.commodity_amounts[i] * input_mult * state.world.market_get_price(mid, cid);
					}
				}

				current_y += grid_margin + rows_input * (grid_margin + commodity_rect.h) + 10.f;

				// maintenance

				for(unsigned i = 0; i < e_inputs.set_size; i++) {
					if(e_inputs.commodity_type[i]) {
						auto cid = e_inputs.commodity_type[i];

						int row = i / items_per_row;
						int column = i - row * items_per_row;
						commodity_rect.x = view_panel.x + grid_margin + column * (grid_margin + commodity_rect.w);
						commodity_rect.y = current_y + grid_margin + row * (grid_margin + commodity_rect.h);

						commodity_labels.x = commodity_rect.x;
						commodity_labels.y = commodity_rect.y;

						state.iui_state.panel_textured(state, commodity_rect, state.iui_state.commodity_bg.texture_handle);
						commodity_panel(
							state,
							(int32_t)static_elements::commodities_inputs_amount + cid.index(),
							(int32_t)static_elements::commodities_inputs_cost + cid.index(),
							cid,
							e_inputs.commodity_amounts[i] * input_mult * e_input_mult,
							state.world.market_get_price(mid, cid),
							commodity_labels
						);

						total -= e_inputs.commodity_amounts[i] * input_mult * e_input_mult * state.world.market_get_price(mid, cid);
					}
				}

				current_y += grid_margin + rows_e_input * (grid_margin + commodity_rect.h) + 10.f;

				commodity_rect.x = view_panel.x + grid_margin;
				commodity_rect.y = current_y;

				// output
				auto cid = state.world.factory_type_get_output(ftid).id;

				commodity_labels.x = commodity_rect.x;
				commodity_labels.y = commodity_rect.y;

				state.iui_state.panel_textured(state, commodity_rect, state.iui_state.commodity_bg.texture_handle);
				commodity_panel(
					state,
					(int32_t)static_elements::commodities_inputs_amount + cid.index(),
					(int32_t)static_elements::commodities_inputs_cost + cid.index(),
					cid,
					state.world.factory_type_get_output_amount(ftid) * output_mult,
					state.world.market_get_price(mid, cid),
					commodity_labels
				);

				commodity_rect.x += commodity_rect.w + grid_margin;
				commodity_labels.x = commodity_rect.x;
				commodity_labels.y = commodity_rect.y;

				total += state.world.factory_type_get_output_amount(ftid) * output_mult * state.world.market_get_price(mid, cid);
				state.iui_state.panel_textured(state, commodity_rect, state.iui_state.commodity_bg.texture_handle);
				state.iui_state.price(
					state, (int32_t)static_elements::factory_type_total,
					commodity_labels, total
				);

				iui::rect leader_rect{ view_panel.x + margin, commodity_labels.y + commodity_labels.h + 5.f, 20.f, 20.f };

				auto font_size = float(text::size_from_font_id(state.iui_state.current_font));
				auto font_index = text::font_index_from_font_id(state, state.iui_state.current_font);
				auto& current_font = state.font_collection.get_font(state, font_index);

				for(unsigned i = 0; i < state.iui_state.input_efficiency_leaders.size(); i++) {
					auto leader = state.iui_state.input_efficiency_leaders[i];
					ogl::render_text_flag(
						state,
						{ state.world.nation_get_identity_from_identity_holder(leader) },
						leader_rect.x,
						leader_rect.y,
						font_size,
						current_font
					);

					leader_rect.x += leader_rect.w;

					if(leader_rect.x + leader_rect.w > view_panel.x + view_panel.w - margin) {
						leader_rect.y += leader_rect.h;
						leader_rect.x = view_panel.x + margin;
					}
				}


				iui::rect priority_settings{ view_panel.x, view_panel.y + view_panel.h - priority_settings_height - 30.f, view_panel.w, priority_settings_height };
				iui::shrink(priority_settings, margin);
				state.iui_state.panel_textured(state, priority_settings, state.iui_state.factory_type_priority_bg.texture_handle);
				iui::shrink(priority_settings, margin);

				iui::rect priority_description{ priority_settings.x, priority_settings.y, priority_settings.w - 10.f, 20.f };

				state.iui_state.localized_string(
					state,
					(int32_t)static_elements::priority_text,
					priority_description,
					text::produce_simple_string(state, "alice_factory_type_viewer_priority")
				);

				priority_description.y += 15.f;
				state.iui_state.localized_string(
					state,
					(int32_t)static_elements::priority_text_national,
					priority_description,
					text::produce_simple_string(state, "alice_factory_type_viewer_priority_national")
				);
				state.iui_state.float_2(
					state,
					(int32_t)static_elements::priority_value_national,
					priority_description, nations::priority_national(state, nid, ftid)
				);

				priority_description.y += 15.f;
				state.iui_state.localized_string(
					state,
					(int32_t)static_elements::priority_text_private,
					priority_description,
					text::produce_simple_string(state, "alice_factory_type_viewer_priority_private")
				);
				state.iui_state.float_2(
					state,
					(int32_t)static_elements::priority_value_private,
					priority_description, nations::priority_private(state, nid, ftid)
				);

				priority_description.y += 15.f;
				state.iui_state.localized_string(
					state,
					(int32_t)static_elements::priority_text_bonus,
					priority_description,
					text::produce_simple_string(state, "alice_factory_type_viewer_priority_bonus")
				);
				state.iui_state.float_2(
					state,
					(int32_t)static_elements::priority_value_bonus,
					priority_description,
					economy::priority_multiplier(state, ftid, nid)
				);

				iui::rect button_priority{
					view_panel.x + 7.f, view_panel.y + view_panel.h - 30.f,
					state.iui_state.priority_button.w, state.iui_state.priority_button.h
				};

				auto current_priority = state.world.nation_get_factory_type_experience_priority_national(
					state.local_player_nation, ftid
				);

				for(int i = 0; i < 9; i++) {
					if(state.iui_state.button_textured(
						state, (int32_t)static_elements::factory_type_priority_button + i,
						button_priority,
						3, state.iui_state.priority_button.texture_handle,
						current_priority >= (float)(i) * 100.f
					)) {
						command::set_factory_type_priority(state, state.local_player_nation, ftid, (float)(i) * 100.f);
					}
					button_priority.x += button_priority.w;
				}
			}
		}
	} else if(state.iui_state.tab == iui::iui_tab::wages) {
		float view_mode_height = 25.f;
		float view_mode_width = 150.f;
		float shift_y = 0.f;
		for(int32_t i = 0; i < economy::labor::total; i++) {
			if(i == economy::labor::guild_education) {
				shift_y -= view_mode_height;
				continue;
			}

			iui::rect button_rect = { 10.f, screen_size.y - 350.f + i * view_mode_height + shift_y, view_mode_width, view_mode_height };
			if(state.iui_state.button_textured(
				state, (int32_t)(static_elements::wages_tab_labor_selector_button) + i,
				button_rect, 3, state.iui_state.top_bar_button.texture_handle,
				state.iui_state.selected_labor_type == i
			)) {
				state.iui_state.selected_labor_type = i;
				update(state);
			}

			state.iui_state.localized_string(
				state,
				(int32_t)static_elements::wages_tab_labor_selector_button_label + i,
				button_rect,
				ui::labour_type_to_employment_name_text_key(i),
				ui::get_text_color(state, text::text_color::gold)
			);
		}

		for(int32_t i = 0; i < (int32_t)iui::labor_info_mode::total; i++) {
			iui::rect button_rect = { view_mode_width + 10.f, screen_size.y - 350.f + i * view_mode_height, view_mode_width, view_mode_height };
			if(state.iui_state.button_textured(
				state, (int32_t)(static_elements::wages_tab_stats_selector_button)+i,
				button_rect, 3, state.iui_state.top_bar_button.texture_handle,
				state.iui_state.selected_labor_info == (iui::labor_info_mode)i
			)) {
				state.iui_state.selected_labor_info = (iui::labor_info_mode)i;
				update(state);
			}

			state.iui_state.localized_string(
				state,
				(int32_t)static_elements::wages_tab_stats_selector_button_label + i,
				button_rect,
				iui::localize_labor_info_mode((iui::labor_info_mode)i),
				ui::get_text_color(state, text::text_color::gold)
			);
		}
	} else if(state.iui_state.tab == iui::iui_tab::trade_volume) {
		float view_mode_height = 25.f;
		float view_mode_width = 150.f;
		float shift_y = 0.f;

		for(int32_t i = 0; i < (int32_t)iui::trade_volume_info_mode::total; i++) {
			iui::rect button_rect = { 10.f, screen_size.y - 350.f + i * view_mode_height, view_mode_width, view_mode_height };
			if(state.iui_state.button_textured(
				state, (int32_t)(static_elements::wages_tab_stats_selector_button)+i,
				button_rect, 3, state.iui_state.top_bar_button.texture_handle,
				state.iui_state.selected_trade_info == (iui::trade_volume_info_mode)i
			)) {
				state.iui_state.selected_trade_info = (iui::trade_volume_info_mode)i;
				update(state);
			}

			state.iui_state.localized_string(
				state,
				(int32_t)static_elements::wages_tab_stats_selector_button_label + i,
				button_rect,
				iui::localize_trade_volume_info_mode((iui::trade_volume_info_mode)i),
				ui::get_text_color(state, text::text_color::gold)
			);
		}
	}

	// render histogram:
	bool hovered_bin = false;
	float bin_center = 0.f;

	auto bins_count = state.iui_state.bins.size();
	auto bin_width = 150.f / bins_count;
	auto bin_range = (state.iui_state.bins_end - state.iui_state.bins_start) / float(bins_count);

	for(uint32_t i = 0; i < bins_count; i++) {
		auto value = state.iui_state.bins[i];
		auto height = value * 600.f;
		iui::rect bin = { screen_size.x - 150.f + bin_width * i, screen_size.y - height, bin_width, height };

		if(state.iui_state.check_hover(state, bin)) {
			hovered_bin = true;
			bin_center = state.iui_state.bins_start + bin_range * (float(i) + 0.5f);
			state.iui_state.panel_colored(state, bin, 1.0f, 1.0f, 0.2f);
		} else {
			state.iui_state.panel_colored(state, bin, 0.9f, 0.5f, 0.1f);
		}
	}

	iui::rect center_rect{
		screen_size.x - 160.f - state.iui_state.map_label.w,
		screen_size.y - 40.f,
		state.iui_state.map_label.w,
		state.iui_state.map_label.h
	};

	iui::rect range_rect{
		screen_size.x - 160.f - state.iui_state.map_label.w,
		screen_size.y - 20.f,
		state.iui_state.map_label.w,
		state.iui_state.map_label.h
	};

	if(hovered_bin) {
		state.iui_state.panel_textured(state, center_rect, state.iui_state.map_label.texture_handle);
		center_rect.w -= 5.f;
		state.iui_state.float_2(
			state,
			(int32_t)static_elements::bin_center,
			center_rect,
			bin_center
		);

		state.iui_state.panel_textured(state, range_rect, state.iui_state.map_label.texture_handle);
		range_rect.w -= 5.f;
		state.iui_state.float_2(
			state,
			(int32_t)static_elements::bin_range,
			range_rect,
			bin_range
		);
	}

	state.iui_state.frame_end();
}
}
