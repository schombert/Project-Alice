#include "json.hpp"
#include "container_types.hpp"
#include "commands.hpp"
#include "system_state.hpp"
#include "parsers.hpp"
#include "demographics.hpp"
#include "province_templates.hpp"
#include "economy_stats.hpp"
#include "economy_production.hpp"

using json = nlohmann::json;

namespace webui {

json format_color(sys::state& state, uint32_t c) {
	json j = json::object();

	j["r"] = sys::int_red_from_int(c);
	j["g"] = sys::int_green_from_int(c);
	j["b"] = sys::int_blue_from_int(c);


	return j;
}

json format_commodity(sys::state& state, dcon::commodity_id c) {
	json j = format_commodity_link(state, c);

	{
		json jplist = json::array();
		for(auto n : state.world.in_nation)
			if(n.get_owned_province_count() != 0) {
				json jel = format_nation(state, n);
				jel["supply"] = economy::supply(state, n, c);

				if(jel["supply"] > 0.0f) {
					jplist.push_back(jel);
				}
			}
		j["producers"] = jplist;
	}
	{
		json jblist = json::array();
		for(auto n : state.world.in_nation)
			if(n.get_owned_province_count() != 0) {
				json jel = format_nation(state, n);
				jel["demand"] = economy::demand(state, n, c);

				if(jel["demand"] > 0.0f) {
					jblist.push_back(jel);
				}
			}

		j["consumers"] = jblist;
	}

	return j;
}

json format_commodity_link(sys::state& state, dcon::commodity_id c) {
	json j = json::object();

	j["id"] = c.index();;
	j["name"] = text::produce_simple_string(state, state.world.commodity_get_name(c));
	j["key"] = state.to_string_view(state.world.commodity_get_name(c));
	j["color"] = format_color(state, state.world.commodity_get_color(c));

	return j;
}

json format_nation(sys::state& state, dcon::nation_id n) {
	json j = json::object();

	j["id"] = n.index();
	j["name"] = text::produce_simple_string(state, text::get_name(state, n));

	auto identity = state.world.nation_get_identity_from_identity_holder(n);
	auto color = state.world.national_identity_get_color(identity);

	j["color"] = format_color(state, color);

	auto capital = state.world.nation_get_capital(n);
	j["capital"] = format_province_link(state, capital);

	json jlist = json::array();
	for(auto st : state.world.in_state_instance) {
		if(st.get_state_ownership().get_nation() == n) {
			jlist.push_back(format_state_link(state, st));
		}
	}
	j["states"] = jlist;

	auto nation_ppp_gdp_text = text::format_float(economy::gdp::value_nation(state, n));
	float population = state.world.nation_get_demographics(n, demographics::total);
	auto nation_ppp_gdp_per_capita_text = text::format_float(economy::gdp::value_nation(state, n) / population * 1000000.f);
	auto nation_sol_text = text::format_float(demographics::calculate_nation_sol(state, n));

	auto national_bank = state.world.nation_get_national_bank(n);
	auto state_debt = nations::get_debt(state, n);

	j["population"] = int(population);
	j["nation_ppp_gdp"] = nation_ppp_gdp_text;
	j["nation_ppp_gdp_per_capita"] = nation_ppp_gdp_per_capita_text;
	j["nation_sol"] = nation_sol_text;

	j["national_bank"] = national_bank;
	j["state_debt"] = state_debt;

	return j;
}

json format_nation_link(sys::state& state, dcon::nation_id n) {
	json j = json::object();

	j["id"] = n.index();
	j["name"] = text::produce_simple_string(state, text::get_name(state, n));

	auto identity = state.world.nation_get_identity_from_identity_holder(n);

	j["tag"] = nations::int_to_tag(state.world.national_identity_get_identifying_int(identity)); // Three letter content tag

	// auto color = state.world.national_identity_get_color(identity);
	// j["color"] = format_color(state, color);

	return j;
}

json format_nation(sys::state& state, dcon::national_identity_id n) {
	json j = json::object();

	auto fid = dcon::fatten(state.world, n);
	j["id"] = n.index();
	j["name"] = text::produce_simple_string(state, fid.get_name());
	j["color"] = format_color(state, fid.get_color());

	j["tag"] = nations::int_to_tag(state.world.national_identity_get_identifying_int(n)); // Three letter content tag

	return j;
}

json format_state(sys::state& state, dcon::state_instance_id stid) {
	auto state_name = text::get_dynamic_state_name(state, stid);
	// text::produce_simple_string(state, state.world.state_instance_get_name(stid));
	auto capital = state.world.state_instance_get_capital(stid);
	auto owner = state.world.province_get_nation_from_province_ownership(capital);

	auto def = state.world.state_instance_get_definition(stid);

	json j = json::object();

	j["id"] = stid.index();
	j["name"] = state_name;
	j["owner"] = format_nation_link(state, owner);

	j["capital"] = format_province_link(state, capital);

	{
		json jlist = json::array();
		for(auto p : state.world.in_province) {
			if(p.get_abstract_state_membership().get_state() == def) {
				jlist.push_back(format_province_link(state, p));
			}
		}

		j["provinces"] = jlist;
	}

	{
		json primary_worker = json::object();
		auto total = state.world.state_instance_get_demographics(stid, demographics::to_key(state, state.culture_definitions.primary_factory_worker));
		primary_worker["total"] = total;
		auto employed = state.world.state_instance_get_demographics(stid, demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
		primary_worker["employed"] = employed;
		primary_worker["unemployed"] = total - employed;
		j["primary_worker"] = primary_worker;
	}
	{
		json secondary_worker = json::object();
		auto total = state.world.state_instance_get_demographics(stid, demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
		secondary_worker["total"] = total;
		auto employed = state.world.state_instance_get_demographics(stid, demographics::to_employment_key(state, state.culture_definitions.secondary_factory_worker));
		secondary_worker["employed"] = employed;
		secondary_worker["unemployed"] = total - employed;
		j["secondary_worker"] = secondary_worker;
	}
	
	float total = 0.0f;
	float p_total = 0.0f;
	province::for_each_province_in_state_instance(state, stid, [&](dcon::province_id p) {
		total += state.economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].infrastructure * float(state.world.province_get_building_level(p, uint8_t(economy::province_building_type::railroad)));
		p_total += 1.0f;
	});
	j["infastructure"] = (p_total > 0) ? total / p_total : 0.0f;

	return j;
}

json format_state_link(sys::state& state, dcon::state_instance_id stid) {
	auto state_name = text::get_dynamic_state_name(state, stid);

	json j = json::object();

	j["id"] = stid.index();
	j["name"] = state_name;

	return j;
}

json format_province(sys::state& state, dcon::province_id pid) {
	auto prov = dcon::fatten(state.world, pid);

	auto province_name = text::produce_simple_string(state, state.world.province_get_name(prov));

	auto owner = state.world.province_get_nation_from_province_ownership(prov);

	auto rgo = state.world.province_get_rgo(prov);

	auto sid = state.world.province_get_state_membership(prov);

	json j = json::object();

	j["id"] = prov.id.index();
	j["name"] = province_name;
	j["provid"] = state.world.province_get_provid(prov);

	j["owner"] = format_nation_link(state, owner);
	j["state"] = format_state_link(state, sid);

	j["x"] = state.world.province_get_mid_point(prov).x;
	j["y"] = state.world.province_get_mid_point(prov).y;

	auto terrain = prov.get_terrain();
	if(auto name = terrain.get_name(); name) {
		json t = json::object();

		t["id"] = terrain.id.index();
		t["key"] = state.to_string_view(terrain.get_name());
		// j["terrain_name"] = text::produce_simple_string(state, state.world.province_get_terrain(prov).get_name());
		// j["terrain_icon"] = state.world.province_get_terrain(prov).get_icon();
		// j["terrain_desc"] = text::produce_simple_string(state, state.world.province_get_terrain(prov).get_desc());
		j["terrain"] = t;
	}

	if(auto name = prov.get_climate().get_name(); name) {
		j["climate"] = text::produce_simple_string(state, name);
	}
	if(auto name = prov.get_continent().get_name(); name) {
		j["continent"] = text::produce_simple_string(state, name);
	}

	// List key population types
	j["population"]["total"] = int(state.world.province_get_demographics(prov, demographics::total));
	j["population"]["capitalist"] = int(state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.capitalists)));
	j["population"]["aristocrat"] = int(state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.aristocrat)));
	j["population"]["bureaucrat"] = int(state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.bureaucrat)));
	j["population"]["clergy"] = int(state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.clergy)));
	j["population"]["officers"] = int(state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.officers)));
	j["population"]["soldiers"] = int(state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.soldiers)));
	j["population"]["craftsmen"] = int(state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.primary_factory_worker)));
	j["population"]["clerks"] = int(state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.secondary_factory_worker)));
	j["population"]["laborers"] = int(state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.laborers)));
	j["population"]["farmers"] = int(state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.farmers)));
	j["population"]["slaves"] = int(state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.slaves)));

	// List every RGO the province has
	{
		json jlist = json::array();

		for(auto c : state.world.in_commodity) {
			if(economy::rgo_max_employment(state, c, prov) > 100.f) {
				json t = json::object();

				t["commodity"] = format_commodity_link(state, c);

				t["max_employment"] = economy::rgo_max_employment(state, c, prov);

				t["income"] = economy::rgo_income(state, c, prov);
				t["output"] = economy::rgo_output(state, c, prov);
				t["employment"]["total"]["actual"] = economy::rgo_employment(state, c, prov);

				auto target_employment = state.world.province_get_rgo_target_employment(prov, c);
				auto satisfaction = state.world.province_get_labor_demand_satisfaction(prov, economy::labor::no_education);

				t["employed"] = target_employment * satisfaction;

				t["employment"]["no_education"]["target"] = int(target_employment);
				t["employment"]["no_education"]["satisfaction"] = satisfaction;
				t["employment"]["no_education"]["actual"] = int(target_employment * satisfaction);
				t["employment"]["no_education"]["wage"] = state.world.province_get_labor_price(prov, economy::labor::no_education);
				t["employment"]["no_education"]["max"] = int(economy::rgo_max_employment(state, c, prov));

				bool const is_mine = state.world.commodity_get_is_mine(c);
				auto const efficiency = 1.0f + state.world.province_get_modifier_values(prov, is_mine ? sys::provincial_mod_offsets::mine_rgo_eff : sys::provincial_mod_offsets::farm_rgo_eff) +
					state.world.nation_get_modifier_values(owner, is_mine ? sys::national_mod_offsets::mine_rgo_eff : sys::national_mod_offsets::farm_rgo_eff);
				t["efficiency"] = efficiency;

				auto const throughput = 1.0f + state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::local_rgo_throughput) +
					state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::rgo_throughput);
				t["throughput"] = throughput;

				// TODO: Efficiency inputs

				jlist.push_back(t);
			}
		}
		j["rgo"] = jlist;
	}
	 
	// Link factories
	{
		json jlist = json::array();
		for(auto floc : state.world.in_factory_location) {
			if(floc.get_province() == prov) {
				jlist.push_back(format_factory(state, floc.get_factory()));
			}
		}
		j["factories"] = jlist;
	}

	j["control_ratio"] = state.world.province_get_control_ratio(prov);

	/*json jlist = json::array();
	for(const auto adj : state.world.province_get_province_adjacency(prov)) {
		auto other = adj.get_connected_provinces(adj.get_connected_provinces(0) == prov ? 1 : 0);
		auto jobj = format_province_link(state, other);
		jobj["is_coastal"] = (adj.get_type() & province::border::coastal_bit) != 0;
		jobj["is_passable"] = (adj.get_type() & province::border::impassible_bit) == 0;
		jobj["is_strait"] = (adj.get_type() & province::border::non_adjacent_bit) != 0;
		jobj["cross_river"] = (adj.get_type() & province::border::river_crossing_bit) != 0;
		jobj["change_state"] = (adj.get_type() & province::border::state_bit) != 0;
		jobj["change_nation"] = (adj.get_type() & province::border::national_bit) != 0;

		jlist.push_back(jobj);
	}

	j["neighbours"] = jlist;*/

	return j;
}

json format_province_link(sys::state& state, dcon::province_id prov) {
	auto province_name = text::produce_simple_string(state, state.world.province_get_name(prov));

	json j = json::object();

	j["id"] = prov.index();
	j["name"] = province_name;

	return j;
}

json format_factory(sys::state& state, dcon::factory_id fid) {
	auto type = state.world.factory_get_building_type(fid);
	auto factory_name = text::produce_simple_string(state, state.world.factory_type_get_name(type));

	auto location = state.world.factory_get_province_from_factory_location(fid);
	auto owner = state.world.province_get_nation_from_province_ownership(location);
	auto sid = state.world.province_get_state_membership(location);
	auto market = state.world.state_instance_get_market_from_local_market(sid);

	json j = json::object();

	j["id"] = fid.index();
	j["name"] = factory_name;

	j["owner"] = format_nation_link(state, owner);
	j["state"] = format_state_link(state, sid);
	j["province"] = format_province_link(state, location);

	j["inputs"] = format_commodity_set(state, state.world.factory_type_get_inputs(type));
	j["output"] = format_commodity_link(state, state.world.factory_type_get_output(type));

	j["actual_production"] = state.world.factory_get_output(fid);
	j["level"] = economy::get_factory_level(state, fid);
	j["input_cost_per_worker"] = state.world.factory_get_input_cost_per_worker(fid);
	j["output_cost_per_worker"] = state.world.factory_get_output_per_worker(fid) * state.world.market_get_price(market, state.world.factory_type_get_output(type));

	j["unqualified_employment"] = state.world.factory_get_unqualified_employment(fid);
	j["primary_employment"] = state.world.factory_get_primary_employment(fid);
	j["secondary_employment"] = state.world.factory_get_secondary_employment(fid);

	j["employed"] = state.world.factory_get_unqualified_employment(fid) + state.world.factory_get_primary_employment(fid) + state.world.factory_get_secondary_employment(fid);

	return j;
}

json format_factory_link(sys::state& state, dcon::factory_id fid) {
	auto type = state.world.factory_get_building_type(fid);
	auto factory_name = text::produce_simple_string(state, state.world.factory_type_get_name(type));

	auto location = state.world.factory_get_province_from_factory_location(fid);
	auto sid = state.world.province_get_state_membership(location);

	json j = json::object();

	j["id"] = fid.index();
	j["name"] = factory_name;
	j["state"] = format_state_link(state, sid);

	return j;
}

json format_commodity_set(sys::state& state, economy::commodity_set set) {
	json jlist = json::array();
	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		if(set.commodity_type[i]) {
			json j = json::object();

			j["commodity"] = format_commodity_link(state, set.commodity_type[i]);
			j["amount"] = set.commodity_amounts[i];

			jlist.push_back(j);
		}
	}
	return jlist;
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

}
