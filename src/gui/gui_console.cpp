#include <string>
#include <string_view>
#include <variant>
#include <filesystem>
#include "gui_console.hpp"
#include "gui_fps_counter.hpp"
#include "nations.hpp"
#include "fif_dcon_generated.hpp"
#include "fif_common.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#include "stb_image_write.h"


void log_to_console(sys::state& state, ui::element_base* parent, std::string_view s) noexcept {
	Cyto::Any output = std::string(s);
	parent->impl_get(state, output);
}

void ui::console_edit::render(sys::state& state, int32_t x, int32_t y) noexcept {
	ui::edit_box_element_base::render(state, x, y);
}

void ui::console_edit::edit_box_update(sys::state& state, std::string_view s) noexcept {
}

void ui::console_edit::edit_box_tab(sys::state& state, std::string_view s) noexcept {
}

void ui::console_edit::edit_box_up(sys::state& state) noexcept {
	std::string up = up_history();
	if(!up.empty()) {
		this->set_text(state, up);
		auto index = int32_t(up.size());
		this->edit_index_position(state, index);
	}
}
void ui::console_edit::edit_box_down(sys::state& state) noexcept {
	std::string down = down_history();
	if(!down.empty()) {
		this->set_text(state, down);
		auto index = int32_t(down.size());
		this->edit_index_position(state, index);
	}
}

template<typename F>
void write_single_component(sys::state& state, native_string_view filename, F&& func) {
	auto sdir = simple_fs::get_or_create_oos_directory();
	auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[sys::sizeof_scenario_section(state).total_size]);
	auto buffer_position = func(buffer.get(), state);
	size_t total_size_used = reinterpret_cast<uint8_t*>(buffer_position) - buffer.get();
	simple_fs::write_file(sdir, filename, reinterpret_cast<char*>(buffer.get()), uint32_t(total_size_used));
}

int32_t* f_clear(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode))
		return p + 2;

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	if(state->ui_state.console_window) {
		static_cast<ui::console_window*>(state->ui_state.console_window)->clear_list(*state);
	}

	return p + 2;
}

int32_t* f_fps(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	if(!state->ui_state.fps_counter) {
		auto fps_counter = ui::make_element_by_type<ui::fps_counter_text_box>(*state, "fps_counter");
		state->ui_state.fps_counter = fps_counter.get();
		state->ui_state.root->add_child_to_front(std::move(fps_counter));
	}

	if(s.main_data_back(0) != 0) {
		state->ui_state.fps_counter->set_visible(*state, true);
		state->ui_state.root->move_child_to_front(state->ui_state.fps_counter);
	} else {
		state->ui_state.fps_counter->set_visible(*state, false);
	}

	s.pop_main();
	return p + 2;
}

int32_t* f_change_tag(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	if(to_nation && to_nation != state->local_player_nation && to_nation != state->world.national_identity_get_nation_from_identity_holder(state->national_definitions.rebel_id)) {
		if(state->local_player_nation)
			state->world.nation_set_is_player_controlled(state->local_player_nation, false);

		state->local_player_nation = to_nation;
		state->world.nation_set_is_player_controlled(to_nation, true);
		ai::remove_ai_data(*state, to_nation);
	}

	s.pop_main();
	return p + 2;
}

int32_t* f_spectate(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	dcon::nation_id to_nation = state->world.national_identity_get_nation_from_identity_holder(state->national_definitions.rebel_id);

	if(state->local_player_nation)
		state->world.nation_set_is_player_controlled(state->local_player_nation, false);

	state->local_player_nation = to_nation;

	return p + 2;
}

int32_t* f_set_westernized(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool westernize_status = s.main_data_back(0) != 0;
	s.pop_main();

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	if(westernize_status && state->world.nation_get_is_civilized(to_nation) == false)
		nations::make_civilized(*state, to_nation);
	else if(!westernize_status && state->world.nation_get_is_civilized(to_nation) == true)
		nations::make_uncivilized(*state, to_nation);

	return p + 2;
}

int32_t* f_make_crisis(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	if(state->current_crisis_state == sys::crisis_state::inactive) {
		state->last_crisis_end_date = sys::date{};
		nations::update_flashpoint_tags(*state);
		nations::monthly_flashpoint_update(*state);
		nations::daily_update_flashpoint_tension(*state);
		float max_tension = 0.0f;
		dcon::state_instance_id max_state;
		for(auto si : state->world.in_state_instance) {
			if(si.get_flashpoint_tension() > max_tension && si.get_nation_from_state_ownership().get_is_at_war() == false && si.get_flashpoint_tag().get_nation_from_identity_holder().get_is_at_war() == false) {
				max_tension = si.get_flashpoint_tension();
				max_state = si;
			}
		}
		if(!max_state) {
			for(auto si : state->world.in_state_instance) {
				if(si.get_flashpoint_tag() && !si.get_nation_from_state_ownership().get_is_great_power() && si.get_nation_from_state_ownership().get_is_at_war() == false && si.get_flashpoint_tag().get_nation_from_identity_holder().get_is_at_war() == false) {
					max_state = si;
					break;
				}
			}
		}
		assert(max_state);
		state->world.state_instance_set_flashpoint_tension(max_state, 10000.0f / state->defines.crisis_base_chance);
		nations::update_crisis(*state);
	}

	return p + 2;
}

int32_t* f_set_mil(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	float mvalue = 0.0f;
	auto ivalue = s.main_data_back(0);
	memcpy(&mvalue, &ivalue, 4);
	s.pop_main();

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	for(auto pr : state->world.nation_get_province_ownership(to_nation))
		for(auto pop : pr.get_province().get_pop_location())
			pop_demographics::set_militancy(*state, pop.get_pop().id, mvalue);

	return p + 2;
}
int32_t* f_set_con(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	float mvalue = 0.0f;
	auto ivalue = s.main_data_back(0);
	memcpy(&mvalue, &ivalue, 4);
	s.pop_main();

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	for(auto pr : state->world.nation_get_province_ownership(to_nation))
		for(auto pop : pr.get_province().get_pop_location())
			pop_demographics::set_consciousness(*state, pop.get_pop().id, mvalue);

	return p + 2;
}

int32_t* f_make_allied(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	dcon::nation_id to_nation_b;
	to_nation_b.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	nations::make_alliance(*state, to_nation, to_nation_b);

	return p + 2;
}

int32_t* f_end_game(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	game_scene::switch_scene(*state, game_scene::scene_id::end_screen);

	return p + 2;
}

int32_t* f_dump_oos(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* stateptr = (sys::state*)(state_global->data);
	sys::state& state = *stateptr;

	window::change_cursor(state, window::cursor_type::busy);
	state.debug_save_oos_dump();
	state.debug_scenario_oos_dump();
	// Extneded data NOT included in normal dumps
	write_single_component(state, NATIVE("map_data.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::memcpy_serialize(ptr_in, state.map_state.map_data.size_x);
		ptr_in = sys::memcpy_serialize(ptr_in, state.map_state.map_data.size_y);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.river_vertices);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.river_starts);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.river_counts);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.coastal_vertices);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.coastal_starts);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.coastal_counts);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.border_vertices);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.borders);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.terrain_id_map);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.province_id_map);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.province_area);
		ptr_in = sys::serialize(ptr_in, state.map_state.map_data.diagonal_borders);
		return ptr_in;
	});
	write_single_component(state, NATIVE("defines.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		memcpy(ptr_in, &(state.defines), sizeof(parsing::defines));
		ptr_in += sizeof(parsing::defines);
		return ptr_in;
	});
	write_single_component(state, NATIVE("economy_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		memcpy(ptr_in, &(state.economy_definitions), sizeof(economy::global_economy_state));
		ptr_in += sizeof(economy::global_economy_state);
		return ptr_in;
	});
	write_single_component(state, NATIVE("party_issues.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.culture_definitions.party_issues);
		return ptr_in;
	});
	write_single_component(state, NATIVE("political_issues.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.culture_definitions.political_issues);
		return ptr_in;
	});
	write_single_component(state, NATIVE("social_issues.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.culture_definitions.social_issues);
		return ptr_in;
	});
	write_single_component(state, NATIVE("military_issues.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.culture_definitions.military_issues);
		return ptr_in;
	});
	write_single_component(state, NATIVE("economic_issues.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.culture_definitions.economic_issues);
		return ptr_in;
	});
	write_single_component(state, NATIVE("tech_folders.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.culture_definitions.tech_folders);
		return ptr_in;
	});
	write_single_component(state, NATIVE("crimes.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.culture_definitions.crimes);
		return ptr_in;
	});
	write_single_component(state, NATIVE("culture_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.artisans);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.capitalists);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.farmers);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.laborers);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.clergy);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.soldiers);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.officers);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.slaves);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.bureaucrat);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.aristocrat);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.primary_factory_worker);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.secondary_factory_worker);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.officer_leadership_points);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.bureaucrat_tax_efficiency);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.conservative);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.jingoism);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.promotion_chance);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.demotion_chance);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.migration_chance);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.colonialmigration_chance);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.emigration_chance);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.assimilation_chance);
		ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.conversion_chance);
		return ptr_in;
	});
	write_single_component(state, NATIVE("unit_base_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.military_definitions.unit_base_definitions);
		return ptr_in;
	});
	write_single_component(state, NATIVE("military_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.first_background_trait);
		//ptr_in = sys::serialize(ptr_in, state.military_definitions.unit_base_definitions);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.base_army_unit);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.base_naval_unit);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.standard_civil_war);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.standard_great_war);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.standard_status_quo);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.liberate);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.uninstall_communist_gov);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.crisis_colony);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.crisis_liberate);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.irregular);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.infantry);
		ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.artillery);
		return ptr_in;
	});
	write_single_component(state, NATIVE("national_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.national_definitions.flag_variable_names);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.global_flag_variable_names);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.variable_names);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.triggered_modifiers);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.rebel_id);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.very_easy_player);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.easy_player);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.hard_player);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.very_hard_player);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.very_easy_ai);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.easy_ai);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.hard_ai);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.very_hard_ai);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.overseas);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.coastal);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.non_coastal);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.coastal_sea);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.sea_zone);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.land_province);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.blockaded);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.no_adjacent_controlled);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.core);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.has_siege);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.occupied);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.nationalism);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.infrastructure);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.base_values);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.war);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.peace);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.disarming);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.war_exhaustion);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.badboy);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.debt_default_to);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.bad_debter);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.great_power);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.second_power);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.civ_nation);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.unciv_nation);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.average_literacy);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.plurality);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.generalised_debt_default);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.total_occupation);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.total_blockaded);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.in_bankrupcy);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.num_allocated_national_variables);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.num_allocated_national_flags);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.num_allocated_global_flags);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.flashpoint_focus);
		ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.flashpoint_amount);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_yearly_pulse);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_quarterly_pulse);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_battle_won);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_battle_lost);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_surrender);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_new_great_nation);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_lost_great_nation);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_election_tick);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_colony_to_state);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_state_conquest);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_colony_to_state_free_slaves);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_debtor_default);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_debtor_default_small);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_debtor_default_second);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_civilize);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_my_factories_nationalized);
		ptr_in = sys::serialize(ptr_in, state.national_definitions.on_crisis_declare_interest);
		return ptr_in;
	});
	write_single_component(state, NATIVE("province_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.province_definitions.canals);
		ptr_in = sys::serialize(ptr_in, state.province_definitions.terrain_to_gfx_map);
		ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.first_sea_province);
		ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.europe);
		ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.asia);
		ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.africa);
		ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.north_america);
		ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.south_america);
		ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.oceania);
		return ptr_in;
	});
	write_single_component(state, NATIVE("dates.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::memcpy_serialize(ptr_in, state.start_date);
		ptr_in = sys::memcpy_serialize(ptr_in, state.end_date);
		return ptr_in;
	});
	write_single_component(state, NATIVE("trigger_data.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.trigger_data);
		return ptr_in;
	});
	write_single_component(state, NATIVE("trigger_data_indices.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.trigger_data_indices);
		return ptr_in;
	});
	write_single_component(state, NATIVE("effect_data.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.effect_data);
		return ptr_in;
	});
	write_single_component(state, NATIVE("effect_data_indices.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.effect_data_indices);
		return ptr_in;
	});
	write_single_component(state, NATIVE("value_modifier_segments.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.value_modifier_segments);
		return ptr_in;
	});
	write_single_component(state, NATIVE("value_modifiers.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.value_modifiers);
		return ptr_in;
	});
	write_single_component(state, NATIVE("ui_defs_gfx.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.ui_defs.gfx);
		return ptr_in;
	});
	write_single_component(state, NATIVE("ui_defs_textures.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.ui_defs.textures);
		return ptr_in;
	});
	write_single_component(state, NATIVE("ui_defs_textures.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.ui_defs.textures);
		return ptr_in;
	});
	write_single_component(state, NATIVE("ui_defs_gui.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.ui_defs.gui);
		return ptr_in;
	});
	write_single_component(state, NATIVE("font_collection_font_names.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
		ptr_in = sys::serialize(ptr_in, state.font_collection.font_names);
		return ptr_in;
	});
	log_to_console(state, state.ui_state.console_window, "Check \"My Documents\\Project Alice\\oos\" for the OOS dump");
	window::change_cursor(state, window::cursor_type::normal);

	return p + 2;
}

int32_t* f_cheat_wargoals(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	state->cheat_data.always_allow_wargoals = toggle_state;
	return p + 2;
}
int32_t* f_cheat_reforms(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	state->cheat_data.always_allow_reforms = toggle_state;
	return p + 2;
}
int32_t* f_cheat_deals(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	state->cheat_data.always_accept_deals = toggle_state;
	return p + 2;
}
int32_t* f_cheat_decisions(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	state->cheat_data.always_allow_decisions = toggle_state;
	return p + 2;
}
int32_t* f_daily_oos(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	state->cheat_data.daily_oos_check = toggle_state;
	return p + 2;
}
int32_t* f_cheat_decision_potential(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	state->cheat_data.always_potential_decisions = toggle_state;
	return p + 2;
}
int32_t* f_set_auto_choice(fif::state_stack& s, int32_t* p, fif::environment* env) {
	if(fif::typechecking_mode(env->mode)) {
		if(fif::typechecking_failed(env->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*env, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	if(toggle_state) {
		for(const auto e : state->world.in_national_event) {
			e.set_auto_choice(1);
		}
		for(const auto e : state->world.in_free_national_event) {
			e.set_auto_choice(1);
		}
		for(const auto e : state->world.in_provincial_event) {
			e.set_auto_choice(1);
		}
		for(const auto e : state->world.in_free_provincial_event) {
			e.set_auto_choice(1);
		}
	} else {
		for(const auto e : state->world.in_national_event) {
			e.set_auto_choice(0);
		}
		for(const auto e : state->world.in_free_national_event) {
			e.set_auto_choice(0);
		}
		for(const auto e : state->world.in_provincial_event) {
			e.set_auto_choice(0);
		}
		for(const auto e : state->world.in_free_provincial_event) {
			e.set_auto_choice(0);
		}
	}
	return p + 2;
}
int32_t* f_complete_construction(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	for(uint32_t i = state->world.province_building_construction_size(); i-- > 0;) {
		dcon::province_building_construction_id c{ dcon::province_building_construction_id::value_base_t(i) };

		if(state->world.province_building_construction_get_nation(c) != to_nation)
			continue;

		auto t = economy::province_building_type(state->world.province_building_construction_get_type(c));
		auto const& base_cost = state->economy_definitions.building_definitions[int32_t(t)].cost;
		auto& current_purchased = state->world.province_building_construction_get_purchased_goods(c);
		for(uint32_t j = 0; j < economy::commodity_set::set_size; ++j)
			current_purchased.commodity_amounts[j] = base_cost.commodity_amounts[j] * 2.f;
	}

	return p + 2;
}
int32_t* f_instant_research(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	auto pos = std::find(
		state->cheat_data.instant_research_nations.begin(),
		state->cheat_data.instant_research_nations.end(),
		to_nation
	);
	if(toggle_state && pos == state->cheat_data.instant_research_nations.end()) {
		state->cheat_data.instant_research_nations.push_back(to_nation);
	} else if(!toggle_state && pos != state->cheat_data.instant_research_nations.end()) {
		state->cheat_data.instant_research_nations.erase(pos);
	}

	return p + 2;
}

int32_t* f_conquer(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	dcon::nation_id to_nation_b;
	to_nation_b.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	auto target_owns = state->world.nation_get_province_ownership(to_nation);

	while(target_owns.begin() != target_owns.end()) {
		auto prov = (*target_owns.begin()).get_province();
		province::conquer_province(*state, prov, to_nation_b);
	}

	return p + 2;
}

int32_t* f_make_core(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	dcon::province_id prov;
	prov.value = dcon::province_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	province::add_core(*state, prov, state->world.nation_get_identity_from_identity_holder(to_nation));

	return p + 2;
}
int32_t* f_remove_core(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	dcon::province_id prov;
	prov.value = dcon::province_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	province::remove_core(*state, prov, state->world.nation_get_identity_from_identity_holder(to_nation));

	return p + 2;
}
int32_t* f_set_owner(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	dcon::province_id prov;
	prov.value = dcon::province_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	province::conquer_province(*state, prov, to_nation);

	return p + 2;
}
int32_t* f_set_controller(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	dcon::nation_id to_nation;
	to_nation.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	dcon::province_id prov;
	prov.value = dcon::province_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	province::set_province_controller(*state, prov, to_nation);

	return p + 2;
}
int32_t* f_cheat_army(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	state->cheat_data.instant_army = toggle_state;
	return p + 2;
}
int32_t* f_cheat_navy(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	state->cheat_data.instant_navy = toggle_state;
	return p + 2;
}
int32_t* f_cheat_factories(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	state->cheat_data.instant_industry = toggle_state;
	return p + 2;
}
int32_t* f_add_days(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	auto days = s.main_data_back(0);
	s.pop_main();

	state->current_date += int32_t(days);
	return p + 2;
}
int32_t* f_save_map(fif::state_stack& s, int32_t* ptr, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return ptr + 2;
		s.pop_main();
		return ptr + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	auto type = s.main_data_back(0);
	s.pop_main();

	bool opt_sea_lines = true;
	bool opt_province_lines = true;
	bool opt_blend = true;

	if(type == 0) {
		opt_sea_lines = false;
		opt_province_lines = false;
	} else if(type ==1) {
		opt_blend = false;
	} else if(type == 2) {
		opt_sea_lines = false;
	} else if(type == 3) {
		opt_sea_lines = false;
		opt_blend = false;
	} else if(type == 4) {
		opt_sea_lines = false;
		opt_province_lines = false;
		opt_blend = false;
	}

	auto total_px = state->map_state.map_data.size_x * state->map_state.map_data.size_y;
	auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[total_px * 3]);
	auto blend_fn = [&](uint32_t idx, bool sea_a, bool sea_b, dcon::province_id pa, dcon::province_id pb) {
		if(sea_a != sea_b) {
			buffer[idx * 3 + 0] = 0;
			buffer[idx * 3 + 1] = 0;
			buffer[idx * 3 + 2] = 0;
		}
		if(pa != pb) {
			if(((sea_a || sea_b) && opt_sea_lines)
			|| sea_a != sea_b
			|| (opt_province_lines && !sea_a && !sea_b)) {
				if(opt_blend) {
					buffer[idx * 3 + 0] &= 0x7f;
					buffer[idx * 3 + 1] &= 0x7f;
					buffer[idx * 3 + 2] &= 0x7f;
				} else {
					buffer[idx * 3 + 0] = 0;
					buffer[idx * 3 + 1] = 0;
					buffer[idx * 3 + 2] = 0;
				}
			}
		}
		};
	for(uint32_t y = 0; y < uint32_t(state->map_state.map_data.size_y); y++) {
		for(uint32_t x = 0; x < uint32_t(state->map_state.map_data.size_x); x++) {
			auto idx = y * uint32_t(state->map_state.map_data.size_x) + x;
			auto p = province::from_map_id(state->map_state.map_data.province_id_map[idx]);
			bool p_is_sea = state->map_state.map_data.province_id_map[idx] >= province::to_map_id(state->province_definitions.first_sea_province);
			if(p_is_sea) {
				buffer[idx * 3 + 0] = 128;
				buffer[idx * 3 + 1] = 128;
				buffer[idx * 3 + 2] = 255;
			} else {
				auto owner = state->world.province_get_nation_from_province_ownership(p);
				if(owner) {
					auto owner_color = state->world.nation_get_color(owner);
					buffer[idx * 3 + 0] = uint8_t(owner_color & 0xff);
					buffer[idx * 3 + 1] = uint8_t((owner_color >> 8) & 0xff) & 0xff;
					buffer[idx * 3 + 2] = uint8_t((owner_color >> 16) & 0xff) & 0xff;
				} else {
					buffer[idx * 3 + 0] = 170;
					buffer[idx * 3 + 1] = 170;
					buffer[idx * 3 + 2] = 170;
				}
			}
			if(x < uint32_t(state->map_state.map_data.size_x - 1)) {
				auto br_idx = idx + uint32_t(state->map_state.map_data.size_x);
				if(br_idx < total_px) {
					auto br_p = province::from_map_id(state->map_state.map_data.province_id_map[br_idx]);
					bool br_is_sea = state->map_state.map_data.province_id_map[br_idx] >= province::to_map_id(state->province_definitions.first_sea_province);
					blend_fn(idx, br_is_sea, p_is_sea, br_p, p);
				}
				auto rs_idx = idx + 1;
				if(rs_idx < total_px) {
					auto br_p = province::from_map_id(state->map_state.map_data.province_id_map[rs_idx]);
					bool br_is_sea = state->map_state.map_data.province_id_map[rs_idx] >= province::to_map_id(state->province_definitions.first_sea_province);
					blend_fn(idx, br_is_sea, p_is_sea, br_p, p);
				}
			}
		}
	}
	stbi_flip_vertically_on_write(true);
	auto func = [](void*, void* ptr_in, int size) -> void {
		auto sdir = simple_fs::get_or_create_oos_directory();
		simple_fs::write_file(sdir, NATIVE("map.png"), static_cast<const char*>(ptr_in), uint32_t(size));
		};
	stbi_write_png_to_func(func, nullptr, int(state->map_state.map_data.size_x), int(state->map_state.map_data.size_y), 3, buffer.get(), 0);

	return ptr + 2;
}
int32_t* f_dump_econ(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	if(state->cheat_data.ecodump) {
		state->cheat_data.ecodump = false;
	} else {
		state->cheat_data.ecodump = true;

		state->world.for_each_commodity([&](dcon::commodity_id c) {
			state->cheat_data.prices_dump_buffer += text::produce_simple_string(*state, state->world.commodity_get_name(c)) + ",";
			state->cheat_data.demand_dump_buffer += text::produce_simple_string(*state, state->world.commodity_get_name(c)) + ",";
			state->cheat_data.supply_dump_buffer += text::produce_simple_string(*state, state->world.commodity_get_name(c)) + ",";
		});

		state->cheat_data.prices_dump_buffer += "\n";
		state->cheat_data.demand_dump_buffer += "\n";
		state->cheat_data.supply_dump_buffer += "\n";

		state->world.for_each_pop_type([&](auto pop_type) {
			state->cheat_data.savings_buffer += text::produce_simple_string(
				*state,
				state->world.pop_type_get_name(pop_type)
			);
			state->cheat_data.savings_buffer += ";";
		});

		state->cheat_data.savings_buffer += "markets;nations;investments\n";
	}
	log_to_console(*state, state->ui_state.console_window, state->cheat_data.ecodump ? "✔" : "✘");

	return p + 2;
}
int32_t* f_provid(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	bool toggle_state = s.main_data_back(0) != 0;
	s.pop_main();

	state->cheat_data.show_province_id_tooltip = toggle_state;
	return p + 2;
}
int32_t* f_fire_event(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	auto id = int32_t(s.main_data_back(0));
	s.pop_main();

	dcon::nation_id to_nation_b;
	to_nation_b.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	dcon::free_national_event_id ev;
	for(auto v : state->world.in_free_national_event) {
		if(v.get_legacy_id() == uint32_t(id)) {
			ev = v;
			break;
		}
	}
	if(!ev) {
		e->report_error("no free national event found with that id");
		e->mode = fif::fif_mode::error;
	} else {
		event::trigger_national_event(*state, ev, to_nation_b, state->current_date.value, id ^ to_nation_b.index());
	}

	return p + 2;
}
int32_t* f_nation_name(fif::state_stack& s, int32_t* p, fif::environment* e) {
	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.push_back_main(state->type_text_key, 0, nullptr);
		return p + 2;
	}

	

	dcon::nation_id to_nation_b;
	to_nation_b.value = dcon::nation_id::value_base_t(s.main_data_back(0));
	s.pop_main();

	auto name = text::get_name(*state, to_nation_b);

	s.push_back_main(state->type_text_key, int64_t(name.value), nullptr);

	return p + 2;
}


inline int32_t* compile_modifier(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(!fif::typechecking_failed(e->mode)) {
			s.pop_main();
		}
		return p + 2;
	}
	if(e->mode != fif::fif_mode::interpreting) {
		e->report_error("attempted to compile a definition inside a definition");
		e->mode = fif::fif_mode::error;
		return p + 2;
	}

	auto state_global = fif::get_global_var(*e, "state-ptr");
	sys::state* state = (sys::state*)(state_global->data);

	auto index = s.main_data_back(0);
	s.pop_main();

	dcon::value_modifier_key mkey{ dcon::value_modifier_key::value_base_t(index) };
	std::string body = "" + fif_trigger::multiplicative_modifier(*state, mkey) + " drop drop r> ";
	std::string name_str = "ttest";

	e->dict.words.insert_or_assign(name_str, int32_t(e->dict.word_array.size()));
	e->dict.word_array.emplace_back();
	e->dict.word_array.back().source = body;

	return p + 2;
}

inline int32_t* load_file(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(!fif::typechecking_failed(e->mode)) {
			s.pop_main();
		}
		return p + 2;
	}
	if(e->mode != fif::fif_mode::interpreting) {
		e->report_error("attempted to load a file inside a definition");
		e->mode = fif::fif_mode::error;
		return p + 2;
	}

	auto name_token = fif::read_token(e->source_stack.back(), *e);

	auto dir = simple_fs::get_or_create_root_documents();
	auto file = simple_fs::open_file(dir, simple_fs::utf8_to_native(name_token.content));
	if(file) {
		auto content = simple_fs::view_contents(*file);
		fif::interpreter_stack values{ };
		fif::run_fif_interpreter(*e, std::string_view{content.data, content.file_size}, values);

	} else {
		e->mode = fif::fif_mode::error;
		e->report_error("could not open file");
		return p + 2;
	}
	return p + 2;
}


void ui::initialize_console_fif_environment(sys::state& state) {
	if(state.fif_environment)
		return;

	std::lock_guard lg{ state.lock_console_strings };

	state.fif_environment = std::make_unique<fif::environment>();

	int32_t error_count = 0;
	std::string error_list;
	state.fif_environment->report_error = [&](std::string_view s) {
		state.console_command_error += std::string("?R ERROR: ") + std::string(s) + "?W\\n";
	};

	fif::common_fif_environment(state, *state.fif_environment);

	fif::interpreter_stack values{ };

	fif::run_fif_interpreter(*state.fif_environment,
		" :struct localized i32 value ; "
		" :s localize text_key s: >index make localized .value! ; ",
		values);


	state.type_text_key = state.fif_environment->dict.types.find("text_key")->second;
	state.type_localized_key = state.fif_environment->dict.types.find("localized")->second;

	//
	// Add predefined names and tags
	//
	auto return_to_string = [&](dcon::text_key k) {
		std::string rvalue{state.to_string_view(k) };
		for(auto& c : rvalue) {
			if(uint8_t(c) == 127)
				c = '_';
		}
		return rvalue;
	};
	for(auto n : state.world.in_national_identity) {
		auto tag_str = nations::int_to_tag(n.get_identifying_int());
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + tag_str + " " + std::to_string(n.id.index()) + " >national_identity_id identity_holder-identity nation @ ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_religion) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >religion_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_culture) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >culture_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_commodity) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >commodity_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_ideology) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >ideology_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_issue) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >issue_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_issue_option) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >issue_option_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_reform) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >reform_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_reform_option) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >reform_option_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_cb_type) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >cb_type_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_pop_type) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >pop_type_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_rebel_type) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >rebel_type_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_government_type) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >government_type_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_province) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >province_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_state_definition) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >state_definition_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_technology) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >technology_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}
	for(auto r : state.world.in_invention) {
		fif::run_fif_interpreter(*state.fif_environment,
			std::string(" : ") + return_to_string(r.get_name()) + " " + std::to_string(r.id.index()) + " >invention_id ; ",
		values);
		state.fif_environment->mode = fif::fif_mode::interpreting;
	}

	//
	// Add console commands here
	//

	auto nation_id_type = state.fif_environment->dict.types.find("nation_id")->second;
	auto prov_id_type = state.fif_environment->dict.types.find("province_id")->second;

	fif::add_import("clear", nullptr, f_clear, {}, {}, * state.fif_environment);
	fif::add_import("fps", nullptr, f_fps, { fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("change-tag", nullptr, f_change_tag, { nation_id_type }, {}, *state.fif_environment);
	fif::add_import("set-westernized", nullptr, f_set_westernized, { nation_id_type, fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("make-crisis", nullptr, f_make_crisis, { }, {}, * state.fif_environment);
	fif::add_import("end-game", nullptr, f_end_game, { }, {}, * state.fif_environment);
	fif::add_import("set-mil", nullptr, f_set_mil, { nation_id_type, fif::fif_f32 }, {}, * state.fif_environment);
	fif::add_import("set-con", nullptr, f_set_con, { nation_id_type, fif::fif_f32 }, {}, * state.fif_environment);
	fif::add_import("make-allied", nullptr, f_make_allied, { nation_id_type, nation_id_type }, {}, * state.fif_environment);
	fif::add_import("dump-oos", nullptr, f_dump_oos, { }, {}, * state.fif_environment);
	fif::add_import("cheat-wargoals", nullptr, f_cheat_wargoals, { fif::fif_bool }, {}, *state.fif_environment);
	fif::add_import("cheat-reforms", nullptr, f_cheat_reforms, { fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("cheat-diplomacy", nullptr, f_cheat_deals, { fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("cheat-decisions", nullptr, f_cheat_decisions, { fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("cheat-decision-potential", nullptr, f_cheat_decision_potential, { fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("cheat-army", nullptr, f_cheat_army, { fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("cheat-navy", nullptr, f_cheat_navy, { fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("cheat-factories", nullptr, f_cheat_factories, { fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("daily-oos-check", nullptr, f_daily_oos, { fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("set-auto-choice", nullptr, f_set_auto_choice, { fif::fif_bool }, {}, *state.fif_environment);
	fif::add_import("complete-construction", nullptr, f_complete_construction, { nation_id_type }, {}, * state.fif_environment);
	fif::add_import("instant-research", nullptr, f_instant_research, { nation_id_type, fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("spectate", nullptr, f_spectate, { }, {}, *state.fif_environment);
	fif::add_import("conquer", nullptr, f_conquer, { nation_id_type , nation_id_type }, {}, * state.fif_environment);
	fif::add_import("make-core", nullptr, f_make_core, { prov_id_type , nation_id_type }, {}, * state.fif_environment);
	fif::add_import("remove-core", nullptr, f_remove_core, { prov_id_type , nation_id_type }, {}, * state.fif_environment);
	fif::add_import("set-owner", nullptr, f_set_owner, { prov_id_type , nation_id_type }, {}, * state.fif_environment);
	fif::add_import("set-controller", nullptr, f_set_controller, { prov_id_type , nation_id_type }, {}, * state.fif_environment);
	fif::add_import("add-days", nullptr, f_add_days, { fif::fif_i32 }, {}, * state.fif_environment);
	fif::add_import("save-map", nullptr, f_save_map, { fif::fif_i32 }, {}, * state.fif_environment);
	fif::add_import("dump-econ", nullptr, f_dump_econ, {  }, {}, * state.fif_environment);
	fif::add_import("provid", nullptr, f_provid, { fif::fif_bool }, {}, * state.fif_environment);
	fif::add_import("fire-event", nullptr, f_fire_event, { nation_id_type, fif::fif_i32 }, {}, * state.fif_environment);
	fif::add_import("nation-name", nullptr, f_nation_name, { nation_id_type }, { state.type_text_key }, *state.fif_environment);
	fif::add_import("load-file", nullptr, load_file, {}, {}, * state.fif_environment);

	fif::add_import("compile-mod", nullptr, compile_modifier, { fif::fif_i32 }, { }, * state.fif_environment);

	fif::run_fif_interpreter(*state.fif_environment,
		" : no-sea-line 0 ; : no-blend 1 ; : no-sea-line-2 2 ; : blend-no-sea 3 ; : vanilla 4 ; ",
		values);
	fif::run_fif_interpreter(*state.fif_environment,
		" :s name nation_id s: nation-name ; ",
		values);
	
	fif::run_fif_interpreter(*state.fif_environment,
		" : player " + std::to_string(offsetof(sys::state, local_player_nation)) + " state-ptr @ buf-add ptr-cast ptr(nation_id) ; ",
		values);

	fif::run_fif_interpreter(*state.fif_environment,
		" : selected-prov " + std::to_string(offsetof(sys::state, map_state) + offsetof(map::map_state, selected_province)) + " state-ptr @ buf-add ptr-cast ptr(province_id) ; ",
		values);

	assert(state.fif_environment->mode != fif::fif_mode::error);
}

std::string ui::format_fif_value(sys::state& state, int64_t data, int32_t type) {
	if(type == fif::fif_i8 || type == fif::fif_i16 || type == fif::fif_i32 || type == fif::fif_i64) {
		return std::to_string(data);
	} else if(type == fif::fif_u8 || type == fif::fif_u16 || type == fif::fif_u32 || type == fif::fif_u64) {
		return std::to_string(uint64_t(data));
	} else if(type == fif::fif_f32) {
		float v = 0;
		memcpy(&v, &data, 4);
		return std::to_string(v);
	} else if(type == fif::fif_f64) {
		double v = 0;
		memcpy(&v, &data, 8);
		return std::to_string(v);
	} else if(type == fif::fif_bool) {
		if(data != 0)
			return "true";
		return "false";
	} else if(type == fif::fif_opaque_ptr) {
		return "#ptr(nil)";
	} else if(type == state.type_text_key) {
		dcon::text_key k;
		k.value = dcon::text_key::value_base_t(data);
		return std::string("\"") + std::string(state.to_string_view(k)) + "\"";
	} else if(type == state.type_localized_key) {
		uint32_t localized_index = uint32_t(data);
		dcon::text_key k{ localized_index };
		if(!k)
			return "\"\"";

		std::string_view sv;
		if(auto it = state.locale_key_to_text_sequence.find(k); it != state.locale_key_to_text_sequence.end()) {
			return std::string("\"") + std::string{ state.locale_string_view(it->second) } + "\"";
		} else {
			return std::string("\"") + std::string{ state.to_string_view(k) } + "\"";
		}
	} else if(type == -1) {
		return "#nil";
	} else if(type == fif::fif_type) {
		return "#type";
	} else {
		if(state.fif_environment->dict.type_array[type].decomposed_types_count >= 2) {
			auto main_type = state.fif_environment->dict.all_stack_types[state.fif_environment->dict.type_array[type].decomposed_types_start];
			if(main_type == fif::fif_ptr) {
				return std::string("#ptr");
			} else {
				return std::string("#struct");
			}
		} else {
			return "#unknown";
		}
	}
}

void ui::console_edit::edit_box_enter(sys::state& state, std::string_view s) noexcept {
	if(s.empty()) {
		edit_box_update(state, s);
		return;
	}

	if(state.ui_state.shift_held_down) {
		log_to_console(state, parent, s);

		std::lock_guard lg{ state.lock_console_strings };
		state.console_command_pending += s;
		state.console_command_pending += " ";
	} else {
		log_to_console(state, parent, s);

		std::lock_guard lg{ state.lock_console_strings };
		state.console_command_pending += s;
		add_to_history(state, state.console_command_pending);
		command::notify_console_command(state);
	}
	edit_box_update(state, s);
	return;
}

void ui::console_edit::edit_box_esc(sys::state& state) noexcept {
	ui::console_window::show_toggle(state);
}
void ui::console_edit::edit_box_backtick(sys::state& state) noexcept {
	ui::console_window::show_toggle(state);
}
void ui::console_edit::edit_box_back_slash(sys::state& state) noexcept {
	ui::console_window::show_toggle(state);
}

void ui::console_window::show_toggle(sys::state& state) {
	assert(state.ui_state.console_window);
	if(state.ui_state.console_window->is_visible()) { //close
		sound::play_interface_sound(state, sound::get_console_close_sound(state), state.user_settings.master_volume * state.user_settings.interface_volume);
	} else { //open
		sound::play_interface_sound(state, sound::get_console_open_sound(state), state.user_settings.master_volume * state.user_settings.interface_volume);
	}

	state.ui_state.console_window->set_visible(state, !state.ui_state.console_window->is_visible());
	if(state.ui_state.console_window->is_visible())
		state.ui_state.root->move_child_to_front(state.ui_state.console_window);
}
