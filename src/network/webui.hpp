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
static httplib::Server svr;

json format_color(sys::state& state, uint32_t c) {
	json j = json::object();

	j["r"] = sys::int_red_from_int(c);
	j["g"] = sys::int_green_from_int(c);
	j["b"] = sys::int_blue_from_int(c);


	return j;
}

json format_nation(sys::state& state, dcon::nation_id n) {
	json j = json::object();

	j["id"] = n.index();
	j["name"] = text::produce_simple_string(state, text::get_name(state, n));

	auto identity = state.world.nation_get_identity_from_identity_holder(n);
	auto color = state.world.national_identity_get_color(identity);

	j["color"] = format_color(state, color);

	return j;
}

json format_nation(sys::state& state, dcon::national_identity_id n) {
	json j = json::object();

	auto fid = dcon::fatten(state.world, n);

	j["name"] = text::produce_simple_string(state, fid.get_name());
	j["color"] = format_color(state, fid.get_color());

	return j;
}

json format_wargoal(sys::state& state, dcon::wargoal_id wid) {
	json j = json::object();

	auto fid = dcon::fatten(state.world, wid);

	j["added_by"] = format_nation(state, fid.get_added_by());
	j["state"] = text::produce_simple_string(state, fid.get_associated_state().get_name());
	j["target"] = format_nation(state, fid.get_target_nation());
	j["cb"] = text::produce_simple_string(state, fid.get_type().get_name());

	j["secondary_nation"] = format_nation(state, fid.get_secondary_nation());
	j["associated_tag"] = format_nation(state, fid.get_associated_tag());

	j["ticking_warscore"] = fid.get_ticking_war_score();

	return j;
}

json format_wargoal(sys::state& state, sys::full_wg wid) {
	json j = json::object();

	j["added_by"] = format_nation(state, wid.added_by);
	j["state"] = text::produce_simple_string(state, state.world.state_definition_get_name(wid.state));
	j["target"] = format_nation(state, wid.target_nation);
	j["cb"] = text::produce_simple_string(state, state.world.cb_type_get_name(wid.cb));

	j["secondary_nation"] = format_nation(state, wid.secondary_nation);
	j["associated_tag"] = format_nation(state, wid.wg_tag);

	return j;
}

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
				auto nation_ppp_gdp_text = text::format_float(economy::gdp_adjusted(state, nation.id));
				float population = state.world.nation_get_demographics(nation.id, demographics::total);
				auto nation_ppp_gdp_per_capita_text = text::format_float(economy::gdp_adjusted(state, nation.id) / population * 1000000.f);
				auto nation_sol_text = text::format_float(demographics::calculate_nation_sol(state, nation.id));

				auto national_bank = state.world.nation_get_national_bank(nation);
				auto state_debt = nations::get_debt(state, nation);

				json j = format_nation(state, nation);

				j["population"] = population;
				j["nation_ppp_gdp"] = nation_ppp_gdp_text;
				j["nation_ppp_gdp_per_capita"] = nation_ppp_gdp_per_capita_text;
				j["nation_sol"] = nation_sol_text;

				j["national_bank"] = national_bank;
				j["state_debt"] = state_debt;

				jlist.push_back(j);
			}

		res.set_content(jlist.dump(), "text/plain");

	});

	svr.Get("/commodities", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto commodity : state.world.in_commodity) {
				auto id = commodity.id.index();

				auto commodity_name = text::produce_simple_string(state, state.world.commodity_get_name(commodity));
				
				json j = json::object();

				j["id"] = id;
				j["name"] = commodity_name;

				{
					json jplist = json::array();
					for(auto n : state.world.in_nation)
						if(n.get_owned_province_count() != 0) {
							json jel = format_nation(state, n);
							jel["supply"] = economy::supply(state, n, commodity);
							jplist.push_back(jel);
						}
					j["producers"] = jplist;
				}
				{
					json jblist = json::array();
					for(auto n : state.world.in_nation)
						if(n.get_owned_province_count() != 0) {
							json jel = format_nation(state, n);
							jel["demand"] = economy::demand(state, n, commodity);
							jblist.push_back(jel);
						}

					j["consumers"] = jblist;
				}

				jlist.push_back(j);
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
			j["owner"] = format_nation(state, owner);
			j["population"]["total"] = prov_population;
			j["population"]["capitalist"] = num_capitalist;
			j["population"]["aristocrat"] = num_aristocrat;

			j["rgo"] = text::produce_simple_string(state, state.world.commodity_get_name(rgo));

			jlist.push_back(j);
		}

		res.set_content(jlist.dump(), "text/plain");
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

