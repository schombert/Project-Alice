#include <algorithm>

#include "commands.hpp"
#include "system_state.hpp"
#include "nations.hpp"
#include "ai.hpp"

namespace command {

void add_to_command_queue(sys::state& state, payload& p);

void c_switch_nation(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_switch_nation;
	p.source = source;
	p.data.tag_target.ident = t;
	add_to_command_queue(state, p);
}
bool can_c_switch_nation(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	dcon::nation_id n = state.world.national_identity_get_nation_from_identity_holder(t);
	return !(state.world.nation_get_is_player_controlled(n) || source == n);
}
void execute_c_switch_nation(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	if(!can_c_switch_nation(state, source, t))
		return;

	dcon::nation_id target = state.world.national_identity_get_nation_from_identity_holder(t);
	if(bool(source) && source != state.national_definitions.rebel_id) {
		state.world.nation_set_is_player_controlled(source, false);
	}
	if(source == state.local_player_nation) {
		state.local_player_nation = target;
	}
	if(bool(target) && target != state.national_definitions.rebel_id) {
		state.world.nation_set_is_player_controlled(target, true);
		ai::remove_ai_data(state, target);
	}
}

void c_change_diplo_points(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_diplo_points;
	p.source = source;
	p.data.cheat.value = value;
	add_to_command_queue(state, p);
}
void execute_c_change_diplo_points(sys::state& state, dcon::nation_id source, float value) {
	state.world.nation_get_diplomatic_points(source) += value;
}
void c_change_money(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_money;
	p.source = source;
	p.data.cheat.value = value;
	add_to_command_queue(state, p);
}
void execute_c_change_money(sys::state& state, dcon::nation_id source, float value) {
	state.world.nation_get_stockpiles(source, economy::money) += value;
}
void c_westernize(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_westernize;
	p.source = source;
	add_to_command_queue(state, p);
}
void execute_c_westernize(sys::state& state, dcon::nation_id source) {
	state.world.nation_set_is_civilized(source, true);
}
void c_unwesternize(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_unwesternize;
	p.source = source;
	add_to_command_queue(state, p);
}
void execute_c_unwesternize(sys::state& state, dcon::nation_id source) {
	state.world.nation_set_is_civilized(source, false);
}
void c_change_research_points(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_research_points;
	p.source = source;
	p.data.cheat.value = value;
	add_to_command_queue(state, p);
}
void execute_c_change_research_points(sys::state& state, dcon::nation_id source, float value) {
	state.world.nation_get_research_points(source) += value;
}
void c_change_cb_progress(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_cb_progress;
	p.source = source;
	p.data.cheat.value = value;
	add_to_command_queue(state, p);
}
void execute_c_change_cb_progress(sys::state& state, dcon::nation_id source, float value) {
	state.world.nation_get_constructing_cb_progress(source) += value;
}
void c_change_infamy(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_infamy;
	p.source = source;
	p.data.cheat.value = value;
	add_to_command_queue(state, p);
}
void execute_c_change_infamy(sys::state& state, dcon::nation_id source, float value) {
	state.world.nation_get_infamy(source) += value;
}
void c_end_game(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_end_game;
	p.source = source;
	add_to_command_queue(state, p);
}
void execute_c_end_game(sys::state& state, dcon::nation_id source) {
	state.mode = sys::game_mode_type::end_screen;
}

void c_complete_constructions(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_complete_constructions;
	p.source = source;
	add_to_command_queue(state, p);
}
void execute_c_complete_constructions(sys::state& state, dcon::nation_id source) {
	
	for(uint32_t i = state.world.province_building_construction_size(); i-- > 0;) {
		dcon::province_building_construction_id c{ dcon::province_building_construction_id::value_base_t(i) };

		if(state.world.province_building_construction_get_nation(c) != source)
			continue;

		auto t = economy::province_building_type(state.world.province_building_construction_get_type(c));
		auto const& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
		auto& current_purchased = state.world.province_building_construction_get_purchased_goods(c);
		for(uint32_t j = 0; j < economy::commodity_set::set_size; ++j)
			current_purchased.commodity_amounts[j] = base_cost.commodity_amounts[j] * 2.f;
	}
}

void c_event(sys::state& state, dcon::nation_id source, int32_t id) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_event;
	p.source = source;
	p.data.cheat_int.value = id;
	add_to_command_queue(state, p);
}
void execute_c_event(sys::state& state, dcon::nation_id source, int32_t id) {
	if(!source)
		return;
	dcon::free_national_event_id e;
	for(auto v : state.world.in_free_national_event) {
		if(v.get_legacy_id() == uint32_t(id)) {
			e = v;
			break;
		}
	}
	if(!e)
		return;

	event::trigger_national_event(state, e, source, state.current_date.value, id ^ source.index());
}
void c_event_as(sys::state& state, dcon::nation_id source, dcon::nation_id as, int32_t id) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_event_as;
	p.source = source;
	p.data.cheat_event.as = as;
	p.data.cheat_event.value = id;
	add_to_command_queue(state, p);
}
void execute_c_event_as(sys::state& state, dcon::nation_id source, dcon::nation_id as, int32_t id) {
	if(!as)
		return;
	dcon::free_national_event_id e;
	for(auto v : state.world.in_free_national_event) {
		if(v.get_legacy_id() == uint32_t(id)) {
			e = v;
			break;
		}
	}
	if(!e)
		return;

	event::trigger_national_event(state, e, as, state.current_date.value, id ^ as.index());
}
void c_force_crisis(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_force_crisis;
	p.source = source;
	add_to_command_queue(state, p);
}
void execute_c_force_crisis(sys::state& state, dcon::nation_id source) {
	if(state.current_crisis == sys::crisis_type::none) {
		state.last_crisis_end_date = sys::date{};
		nations::monthly_flashpoint_update(state);
		nations::daily_update_flashpoint_tension(state);
		float max_tension = 0.0f;
		dcon::state_instance_id max_state;
		for(auto si : state.world.in_state_instance) {
			if(si.get_flashpoint_tension() > max_tension && si.get_nation_from_state_ownership().get_is_at_war() == false && si.get_flashpoint_tag().get_nation_from_identity_holder().get_is_at_war() == false) {
				max_tension = si.get_flashpoint_tension();
				max_state = si;
			}
		}
		if(!max_state) {
			for(auto si : state.world.in_state_instance) {
				if(si.get_flashpoint_tag() && !si.get_nation_from_state_ownership().get_is_great_power() && si.get_nation_from_state_ownership().get_is_at_war() == false && si.get_flashpoint_tag().get_nation_from_identity_holder().get_is_at_war() == false) {
					max_state = si;
					break;
				}
			}
		}
		assert(max_state);
		state.world.state_instance_set_flashpoint_tension(max_state, 10000.0f / state.defines.crisis_base_chance);
		nations::update_crisis(state);
	}
}

void c_change_national_militancy(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_national_militancy;
	p.source = source;
	p.data.cheat.value = value;
	add_to_command_queue(state, p);
}
void execute_c_change_national_militancy(sys::state& state, dcon::nation_id source, float value) {
	for(auto pr : state.world.nation_get_province_ownership(source))
		for(auto pop : pr.get_province().get_pop_location())
			pop.get_pop().set_militancy(pop.get_pop().get_militancy() + value);
}

void c_force_ally(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_force_ally;
	p.source = source;
	p.data.nation_pick.target = target;
	add_to_command_queue(state, p);
}
void execute_c_force_ally(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	nations::make_alliance(state, source, target);
}

void c_change_prestige(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_prestige;
	p.source = source;
	p.data.cheat.value = value;
	add_to_command_queue(state, p);
}
void execute_c_change_prestige(sys::state& state, dcon::nation_id source, float value) {
	state.world.nation_get_prestige(source) += value;
}

void c_toggle_ai(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_toggle_ai;
	p.source = source;
	p.data.nation_pick.target = target;
	add_to_command_queue(state, p);
}
void execute_c_toggle_ai(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	state.world.nation_set_is_player_controlled(target, !state.world.nation_get_is_player_controlled(target));
}

void c_change_owner(sys::state& state, dcon::nation_id source, dcon::province_id pr, dcon::nation_id new_owner) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_owner;
	p.source = source;
	p.data.cheat_location.prov = pr;
	p.data.cheat_location.n = new_owner;
	add_to_command_queue(state, p);
}
void c_change_controller(sys::state& state, dcon::nation_id source, dcon::province_id pr, dcon::nation_id new_controller) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_controller;
	p.source = source;
	p.data.cheat_location.prov = pr;
	p.data.cheat_location.n = new_controller;
	add_to_command_queue(state, p);
}
void execute_c_change_owner(sys::state& state, dcon::nation_id source, dcon::province_id pr, dcon::nation_id new_owner) {
	province::change_province_owner(state, pr, new_owner);
}
void execute_c_change_controller(sys::state& state, dcon::nation_id source, dcon::province_id pr, dcon::nation_id new_controller) {
	province::set_province_controller(state, pr, new_controller);
	military::eject_ships(state, pr);
}
void c_instant_research(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_instant_research;
	p.source = source;
	add_to_command_queue(state, p);
}
void execute_c_instant_research(sys::state& state, dcon::nation_id source) {
	auto pos = std::find(
		state.cheat_data.instant_research_nations.begin(),
		state.cheat_data.instant_research_nations.end(),
		source
	);
	if(pos != state.cheat_data.instant_research_nations.end()) {
		state.cheat_data.instant_research_nations.erase(pos);
	} else {
		state.cheat_data.instant_research_nations.push_back(source);
	}
}

void c_add_population(sys::state& state, dcon::nation_id source, int32_t ammount) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_add_population;
	p.source = source;
	p.data.cheat_int.value = ammount;
	add_to_command_queue(state, p);
}

void execute_c_add_population(sys::state& state, dcon::nation_id source, int32_t ammount) {
	float total_population = state.world.nation_get_demographics(source, demographics::total);
	state.world.for_each_pop([&](dcon::pop_id p) {
		auto pop = dcon::fatten(state.world, p);
		if(source == pop.get_pop_location().get_province().get_nation_from_province_ownership()) {
			pop.set_size(pop.get_size() + std::ceil(ammount * pop.get_size() / total_population));
		}
	});
}

void c_instant_army(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_instant_army;
	p.source = source;
	add_to_command_queue(state, p);
}

void execute_c_instant_army(sys::state& state, dcon::nation_id source) {
	state.cheat_data.instant_army = !state.cheat_data.instant_army;
}

void c_instant_industry(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_instant_industry;
	p.source = source;
	add_to_command_queue(state, p);
}

void execute_c_instant_industry(sys::state& state, dcon::nation_id source) {
	state.cheat_data.instant_industry = !state.cheat_data.instant_industry;
}

void c_innovate(sys::state& state, dcon::nation_id source, dcon::invention_id invention) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_innovate;
	p.source = source;
	p.data.cheat_invention_data.invention = invention;
	add_to_command_queue(state, p);
}

void execute_c_innovate(sys::state& state, dcon::nation_id source, dcon::invention_id invention) {
	culture::apply_invention(state, source, invention);
}

}
