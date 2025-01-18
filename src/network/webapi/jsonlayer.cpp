#include "json.hpp"

using json = nlohmann::json;

namespace webui {

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

json format_state(sys::state& state, dcon::state_instance_id stid, int depth) {
	auto state_name = text::get_dynamic_state_name(state, stid);
	// text::produce_simple_string(state, state.world.state_instance_get_name(stid));
	auto capital = state.world.state_instance_get_capital(stid);
	auto owner = state.world.province_get_nation_from_province_ownership(capital);

	auto def = state.world.state_instance_get_definition(stid);

	json j = json::object();

	j["id"] = stid.index();
	j["name"] = state_name;
	j["owner"] = format_nation(state, owner);

	if(depth > 0) {
		j["capital"] = format_province(state, capital, depth - 1);

		json jlist = json::array();
		for(auto p : state.world.in_province) {
			if(p.get_abstract_state_membership().get_state() == def) {
				jlist.push_back(format_province(state, p, depth - 1));
			}
		}

		j["provinces"] = jlist;
	}

	return j;
}

json format_province(sys::state& state, dcon::province_id prov, int depth) {
	auto province_name = text::produce_simple_string(state, state.world.province_get_name(prov));

	auto owner = state.world.province_get_nation_from_province_ownership(prov);
	auto prov_population = state.world.province_get_demographics(prov, demographics::total);

	float num_capitalist = state.world.province_get_demographics(
			prov,
			demographics::to_key(state, state.culture_definitions.capitalists)
	);

	float num_aristocrat = state.world.province_get_demographics(
			prov,
			demographics::to_key(state, state.culture_definitions.aristocrat)
	);

	auto rgo = state.world.province_get_rgo(prov);

	auto sid = state.world.province_get_state_membership(prov);

	json j = json::object();

	j["id"] = prov.index();
	j["name"] = province_name;

	if(depth > 0) {
		j["owner"] = format_nation(state, owner);
		j["state"] = format_state(state, sid);
	}
	j["population"]["total"] = prov_population;
	j["population"]["capitalist"] = num_capitalist;
	j["population"]["aristocrat"] = num_aristocrat;

	j["rgo"] = text::produce_simple_string(state, state.world.commodity_get_name(rgo));

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

}
