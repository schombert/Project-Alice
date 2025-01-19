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
#include "json.hpp"

#define CPPHTTPLIB_NO_EXCEPTIONS
#include "httplib.h"

#include "jsonlayer.hpp"
#include "jsonlayer.cpp"

using json = nlohmann::json;

namespace webui {

// HTTP
static httplib::Server svr;

inline void init(sys::state& state) noexcept {

	if(state.host_settings.alice_expose_webui != 1 || state.network_mode == sys::network_mode_type::client) {
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
				json j = format_nation(state, nation);

				jlist.push_back(j);
			}

		res.set_content(jlist.dump(), "text/plain");

	});

	svr.Get(R"(/nation/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
		auto match = req.matches[1];
		auto nationnum = std::atoi(match.str().c_str());

		dcon::nation_id n{ dcon::nation_id::value_base_t(nationnum) };

		auto j = format_nation(state, n);

		res.set_content(j.dump(), "text/plain");
	});

	svr.Get(R"(/factory/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
		auto match = req.matches[1];
		auto facnum = std::atoi(match.str().c_str());
		dcon::factory_id f{ dcon::factory_id::value_base_t(facnum) };

		auto j = format_factory(state, f);

		res.set_content(j.dump(), "text/plain");
	});

	svr.Get("/commodities", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto commodity : state.world.in_commodity) {
			jlist.push_back(format_commodity(state, commodity));
		}

		res.set_content(jlist.dump(), "text/plain");
	});

	svr.Get("/routes", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto cid : state.world.in_commodity) {
			state.world.for_each_trade_route([&](dcon::trade_route_id trade_route) {
				auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
				auto origin =
					current_volume > 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);
				auto target =
					current_volume <= 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);

				auto s_origin = state.world.market_get_zone_from_local_market(origin);
				auto s_target = state.world.market_get_zone_from_local_market(target);

				auto p_origin = state.world.state_instance_get_capital(s_origin);
				auto p_target = state.world.state_instance_get_capital(s_target);

				auto sat = state.world.market_get_direct_demand_satisfaction(origin, cid);

				auto absolute_volume = std::abs(current_volume);
				auto factual_volume = sat * absolute_volume;

				if(absolute_volume <= 0) {
					return;
				}

				bool is_sea = state.world.trade_route_get_distance(trade_route) == state.world.trade_route_get_sea_distance(trade_route);

				auto commodity_name = text::produce_simple_string(state, state.world.commodity_get_name(cid));

				json j = json::object();

				j["commodity_id"] = cid.id.value;
				j["commodity"] = commodity_name;

				j["origin_market_id"] = origin.value;
				j["target_market_id"] = target.value;

				j["origin_state_id"] = s_origin.value;
				j["target_state_id"] = s_target.value;

				j["origin_province_id"] = p_origin.id.value;
				j["target_province_id"] = p_target.id.value;

				j["origin_province_name"] = text::produce_simple_string(state, state.world.province_get_name(p_origin));
				j["target_province_name"] = text::produce_simple_string(state, state.world.province_get_name(p_target));

				auto origin_country = state.world.province_get_nation_from_province_ownership(p_origin);
				auto target_country = state.world.province_get_nation_from_province_ownership(p_target);

				j["origin_country_id"] = origin_country.value;
				j["target_country_id"] = target_country.value;

				j["origin_country_name"] = text::produce_simple_string(state, text::get_name(state, origin_country));
				j["target_country_name"] = text::produce_simple_string(state, text::get_name(state, target_country));

				j["volume"] = text::format_float(factual_volume);
				j["desired_volume"] = text::format_float(absolute_volume);

				j["is_sea"] = is_sea;
				jlist.push_back(j);
			});
		}

		res.set_content(jlist.dump(), "text/plain");
	});

	svr.Get(R"(/province/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
		auto match = req.matches[1];
		auto provnum = std::atoi(match.str().c_str());

		dcon::province_id p{ dcon::province_id::value_base_t(provnum) };

		auto j = format_province(state, p);

		res.set_content(j.dump(), "text/plain");
	});

	svr.Get("/provinces", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto prov : state.world.in_province) {
			auto j = format_province(state, prov);
			jlist.push_back(j);
		}

		res.set_content(jlist.dump(), "text/plain");
	});

	svr.Get(R"(/state/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
		auto match = req.matches[1];
		auto statenum = std::atoi(match.str().c_str());

		dcon::state_instance_id s{ dcon::state_instance_id::value_base_t(statenum) };

		auto j = format_state(state, s);

		res.set_content(j.dump(), "text/plain");
	});

	svr.Get("/wars", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto war : state.world.in_war) {
			auto id = war.id.index();

			json j = json::object();

			j["id"] = id;
			j["name"] = text::produce_simple_string(state, war.get_name());
			j["is_great"] = war.get_is_great();
			j["is_crisis"] = war.get_is_crisis_war();
			j["attacker_battle_score"] = war.get_attacker_battle_score();
			j["defender_battle_score"] = war.get_defender_battle_score();
			j["primary_attacker"] = format_nation(state, war.get_primary_attacker());
			j["primary_defender"] = format_nation(state, war.get_primary_defender());

			j["over_state"] = text::produce_simple_string(state, war.get_over_state().get_name());

			json jalist = json::array();
			json jdlist = json::array();
			std::vector<dcon::nation_id> attackers;

			for(auto wp : state.world.war_get_war_participant(war)) {
				if(wp.get_is_attacker()) { 
					jalist.push_back(format_nation(state, wp.get_nation()));
					attackers.push_back(wp.get_nation());
				} else {
					jdlist.push_back(format_nation(state, wp.get_nation()));
				}
			}

			j["attackers"] = jalist;
			j["defenders"] = jdlist;

			json jawgslist = json::array();
			json jdwgslist = json::array();

			for(auto el : war.get_wargoals_attached()) {
				auto wg = el.get_wargoal();
				if(std::find(attackers.begin(), attackers.end(), wg.get_added_by()) != attackers.end()) {
					jawgslist.push_back(format_wargoal(state, wg));
				}
				else {
					jdwgslist.push_back(format_wargoal(state, wg));
				}
			}

			j["attacker_wargoals"] = jawgslist;
			j["defender_wargoals"] = jdwgslist;

			jlist.push_back(j);
		}

		res.set_content(jlist.dump(), "text/plain");
	});

	svr.Get("/crisis", [&](const httplib::Request& req, httplib::Response& res) {
		json j = json::object();

		j["attacker"] = format_nation(state, state.crisis_attacker);
		j["defender"] = format_nation(state, state.crisis_defender);

		j["primary_attacker"] = format_nation(state, state.primary_crisis_attacker);
		j["primary_defender"] = format_nation(state, state.primary_crisis_defender);

		if(state.crisis_state_instance) {
			auto fid = dcon::fatten(state.world, state.crisis_state_instance);
			auto defid = fid.get_definition();
			j["over_state"] = text::produce_simple_string(state, defid.get_name());
		}

		j["temperature"] = state.crisis_temperature;

		json jalist = json::array();
		json jdlist = json::array();

		for(auto cp : state.crisis_participants) {
			if(cp.supports_attacker) {
				jalist.push_back(format_nation(state, cp.id));
			} else if (!cp.merely_interested) {
				jdlist.push_back(format_nation(state, cp.id));
			}
		}

		j["attackers"] = jalist;
		j["defenders"] = jdlist;

		json jawgslist = json::array();
		json jdwgslist = json::array();

		for(auto awg : state.crisis_attacker_wargoals) {
			jawgslist.push_back(format_wargoal(state, awg));
		}
		for(auto dwg : state.crisis_attacker_wargoals) {
			jdwgslist.push_back(format_wargoal(state, dwg));
		}

		j["attacker_wargoals"] = jawgslist;
		j["defender_wargoals"] = jdwgslist;

		res.set_content(j.dump(), "text/plain");
	});

	svr.listen("0.0.0.0", 1234);
}

}

