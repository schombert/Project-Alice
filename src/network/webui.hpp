#pragma once

#include <array>
#include <string>
#include "container_types.hpp"
#include "commands.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include "system_state.hpp"
#include "network.hpp"
#include "parsers.hpp"
#include "simple_fs.hpp"
#include "network.hpp"
#include "demographics.hpp"

#include <text.hpp>
#include <json.hpp>

#define CPPHTTPLIB_NO_EXCEPTIONS
#include <httplib.h>

using json = nlohmann::json;

namespace webui {

// HTTP

inline void init(sys::state& state) noexcept {

	httplib::Server svr;

	if(state.defines.alice_expose_webui != 1 || state.network_mode == sys::network_mode_type::client) {
		return;
	}

	svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
		res.set_content("Homepage", "text/plain");
	});

	svr.Get("/date", [&](const httplib::Request& req, httplib::Response& res) {
		auto dt = state.current_date.to_ymd(state.start_date);
		json j = json::object();
		j["year"] = dt.year;
		j["month"] = dt.month;
		j["day"] = dt.day;
		j["date"] = std::to_string(dt.day) + "." + std::to_string(dt.month) + "." + std::to_string(dt.year);

		res.set_content(j.dump(), "text/plain");
	});


	svr.Get("/nations", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto nation : state.world.in_nation) {
				auto nation_name = text::produce_simple_string(state, text::get_name(state, nation.id));

				auto nation_ppp_gdp_text = text::format_float(economy::gdp_adjusted(state, nation.id));
				float population = state.world.nation_get_demographics(nation.id, demographics::total);
				auto nation_ppp_gdp_per_capita_text = text::format_float(economy::gdp_adjusted(state, nation.id) / population * 1000000.f);
				auto nation_sol_text = text::format_float(demographics::calculate_nation_sol(state, nation.id));

				json j = json::object();

				j["id"] = nation.id.index();
				j["name"] = nation_name;
				j["population"] = population;
				j["nation_ppp_gdp"] = nation_ppp_gdp_text;
				j["nation_ppp_gdp_per_capita"] = nation_ppp_gdp_per_capita_text;
				j["nation_sol"] = nation_sol_text;

				jlist.push_back(j);
			}

		res.set_content(jlist.dump(), "text/plain");

	});

	svr.Get("/commodities", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto commodity : state.world.in_commodity) {
				auto id = commodity.id.index();

				auto commodity_name = text::produce_simple_string(state, state.world.commodity_get_name(commodity));

				auto sat = state.world.nation_get_demand_satisfaction(state.local_player_nation, commodity);
				auto cprice = text::format_money(state.world.commodity_get_current_price(commodity));
				auto ccost = text::format_money(state.world.commodity_get_cost(commodity));
				auto eprice = text::format_money(state.world.nation_get_effective_prices(state.local_player_nation, commodity));
				auto realdemand = text::format_float(state.world.commodity_get_total_real_demand(commodity));
				auto totalproduction = text::format_float(state.world.commodity_get_total_production(commodity));
				auto market_pool = text::format_float(state.world.commodity_get_global_market_pool(commodity));

				json j = json::object();

				j["id"] = id;
				j["name"] = commodity_name;
				if(sat < 0.5f) {
					j["satisfaction"] = "commodity_shortage";
				} else if(sat >= 1.f) {
					j["satisfaction"] = "commodity_surplus";
				}
				j["current_price"] = cprice;
				j["current_cost"] = ccost;
				j["effective_price"] = eprice;
				j["real_demand"] = realdemand;
				j["total_production"] = totalproduction;
				j["market_pool"] = market_pool;

				json jp = json::array();

				struct tagged_value {
					float v = 0.0f;
					dcon::nation_id n;
				};
				static std::vector<tagged_value> producers;

				producers.clear();
				for(auto n : state.world.in_nation) {
					if(n.get_domestic_market_pool(commodity) >= 0.05f) {
						producers.push_back(tagged_value{ n.get_domestic_market_pool(commodity), n.id });
					}
				}
				if(producers.size() > 0) {
					std::sort(producers.begin(), producers.end(), [](auto const& a, auto const& b) { return a.v > b.v; });
					for(uint32_t i = 0; i < producers.size() && i < state.defines.great_nations_count; ++i) {
						json jpo;

						jpo["id"] = producers[i].n.index();
						jpo["name"] = text::produce_simple_string(state, text::get_name(state, producers[i].n));
						auto color = state.world.nation_get_color(producers[i].n);
						jpo["color"]["r"] = sys::int_red_from_int(color);
						jpo["color"]["g"] = sys::int_green_from_int(color);
						jpo["color"]["b"] = sys::int_blue_from_int(color);

						jpo["value"] = producers[i].v;

						jp.push_back(jpo);
					}
				}

				j["producers"] = jp;

				jlist.push_back(j);
		}

		res.set_content(jlist.dump(), "text/plain");
	});

	svr.Get("/provinces", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto prov : state.world.in_province) {
			auto id = prov.id.index();

			auto province_name = text::produce_simple_string(state, state.world.province_get_name(prov));

			auto owner = state.world.province_get_nation_from_province_ownership(prov.id);
			auto prov_population = state.world.province_get_demographics(prov.id, demographics::total);

			float num_capitalist = state.world.province_get_demographics(
					prov,
					demographics::to_key(state, state.culture_definitions.capitalists)
			);

			float num_aristocrat = state.world.province_get_demographics(
					prov,
					demographics::to_key(state, state.culture_definitions.aristocrat)
			);

			auto rgo = state.world.province_get_rgo(prov);

			json j = json::object();

			j["id"] = id;
			j["name"] = province_name;
			j["owner"] = text::produce_simple_string(state, text::get_name(state, owner));
			j["population"]["total"] = prov_population;
			j["population"]["capitalist"] = num_capitalist;
			j["population"]["aristocrat"] = num_aristocrat;

			j["rgo"] = text::produce_simple_string(state, state.world.commodity_get_name(rgo));

			json jp = json::array();

			jlist.push_back(j);
		}

		res.set_content(jlist.dump(), "text/plain");
	});

	svr.listen("0.0.0.0", 1234);;
}

}

