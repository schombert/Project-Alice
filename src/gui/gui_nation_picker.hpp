#pragma once

#include "gui_element_types.hpp"
#include "serialization.hpp"

namespace ui {

class picker_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, retrieve<dcon::nation_id>(state, parent));
		return fat_id.get_identity_from_identity_holder();
	}
	void button_action(sys::state& state) noexcept override {

	}
};

class nation_picker_poptypes_chart : public piechart<dcon::pop_type_id> {
protected:
	std::unordered_map<dcon::pop_type_id::value_base_t, float> get_distribution(sys::state& state) noexcept override {
		std::unordered_map<dcon::pop_type_id::value_base_t, float> distrib{};
		auto n = retrieve<dcon::nation_id>(state, parent);
		auto total = state.world.nation_get_demographics(n, demographics::total);
		if(total > 0) {
			for(auto pt : state.world.in_pop_type) {
				auto amount = state.world.nation_get_demographics(n, demographics::to_key(state, pt));
				distrib[dcon::pop_type_id::value_base_t(pt.id.index())] = amount / total;
			}
		}
		return distrib;
	}

};

class nation_details_window : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "player_shield") {
			return make_element_by_type<picker_flag>(state, id);
		} else if(name == "flag_overlay") {
			return make_element_by_type<nation_flag_frame>(state, id);
		} else if(name == "selected_nation_totalrank") {
			return make_element_by_type<nation_rank_text>(state, id);
		} else if(name == "selected_nation_label") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "selected_countrystatus") {
			return make_element_by_type<nation_status_text>(state, id);
		} else if(name == "selected_fog") {
			return make_element_by_type<nation_government_type_text>(state, id);
		} else if(name == "selected_prestige_rank") {
			return make_element_by_type<nation_prestige_rank_text>(state, id);
		} else if(name == "selected_industry_rank") {
			return make_element_by_type<nation_industry_rank_text>(state, id);
		} else if(name == "selected_military_rank") {
			return make_element_by_type<nation_military_rank_text>(state, id);
		} else if(name == "selected_population_amount") {
			return make_element_by_type<nation_population_text>(state, id);
		} else if(name == "selected_population_chart") {
			return make_element_by_type<nation_picker_poptypes_chart>(state, id);
		} else if(name == "wars_overlappingbox") {
			auto ptr = make_element_by_type<overlapping_enemy_flags>(state, id);
			ptr->base_data.position.y -= 8 - 1; // Nudge
			return ptr;
		} else if(name == "allies_overlappingbox") {
			auto ptr = make_element_by_type<overlapping_ally_flags>(state, id);
			ptr->base_data.position.y -= 8 - 1; // Nudge
			return ptr;
		} else if(name == "puppets_overlappingbox") {
			auto ptr = make_element_by_type<overlapping_puppet_flags>(state, id);
			ptr->base_data.position.y -= 8 - 1; // Nudge
			return ptr;
		}

		return nullptr;
	}
};


struct save_item {
	native_string file_name;
	dcon::national_identity_id save_flag;
	dcon::government_type_id as_gov;
	sys::date save_date;
	bool is_new_game = false;
};

class select_save_game : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		save_item* i = retrieve< save_item*>(state, parent);
		if(i->is_new_game) {
			if(!sys::try_read_scenario_as_save_file(state, state.loaded_scenario_file)) {
				auto msg = std::string("Scenario file ") + simple_fs::native_to_utf8(state.loaded_scenario_file) + " could not be loaded.";
				window::emit_error_message(msg, false);
			} else {
				state.fill_unsaved_data();
			}
		} else {
			if(!sys::try_read_save_file(state, i->file_name)) {
				auto msg = std::string("Save file ") + simple_fs::native_to_utf8(i->file_name) + " could not be loaded.";
				window::emit_error_message(msg, false);
			} else {
				state.fill_unsaved_data();
			}
		}
		state.game_state_updated.store(true, std::memory_order::memory_order_release);
	}
	void on_update(sys::state& state) noexcept override {
		save_item* i = retrieve< save_item*>(state, parent);
		frame = i->file_name == state.loaded_save_file ? 1 : 0;
	}
};

class save_flag : public button_element_base {
protected:
	GLuint flag_texture_handle = 0;
	bool visible = false;
public:
	void button_action(sys::state& state) noexcept override {
	}


	void on_update(sys::state& state) noexcept  override {
		save_item* i = retrieve< save_item*>(state, parent);
		auto tag = i->save_flag;
		auto gov = i->as_gov;
		visible = !i->is_new_game;

		if(!visible)
			return;

		if(!bool(tag))
			tag = state.world.nation_get_identity_from_identity_holder(state.national_definitions.rebel_id);

		culture::flag_type ft = culture::flag_type::default_flag;
		if(gov) {
			auto id = state.world.national_identity_get_government_flag_type(tag, gov);
			if(id != 0)
				ft = culture::flag_type(id - 1);
			else
				ft = state.culture_definitions.governments[gov].flag;
		}
		flag_texture_handle = ogl::get_flag_handle(state, tag, ft);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(!visible)
			return;

		dcon::gfx_object_id gid;
		if(base_data.get_element_type() == element_type::image) {
			gid = base_data.data.image.gfx_object;
		} else if(base_data.get_element_type() == element_type::button) {
			gid = base_data.data.button.button_image;
		}
		if(gid && flag_texture_handle > 0) {
			auto& gfx_def = state.ui_defs.gfx[gid];
			if(gfx_def.type_dependent) {
				auto mask_handle = ogl::get_texture_handle(state, dcon::texture_id(gfx_def.type_dependent - 1), true);
				auto& mask_tex = state.open_gl.asset_textures[dcon::texture_id(gfx_def.type_dependent - 1)];
				ogl::render_masked_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
					float(x) + float(base_data.size.x - mask_tex.size_x) * 0.5f,
					float(y) + float(base_data.size.y - mask_tex.size_y) * 0.5f,
					float(mask_tex.size_x),
					float(mask_tex.size_y),
					flag_texture_handle, mask_handle, base_data.get_rotation(), gfx_def.is_vertically_flipped());
			} else {
				ogl::render_textured_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
						float(x), float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, base_data.get_rotation(),
						gfx_def.is_vertically_flipped());
			}
		}
		image_element_base::render(state, x, y);
	}
};

class save_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		save_item* i = retrieve< save_item*>(state, parent);
		if(i->is_new_game) {
			set_text(state, text::produce_simple_string(state, "fe_new_game"));
		} else {
			auto name = i->as_gov ? state.world.national_identity_get_government_name(i->save_flag, i->as_gov) : state.world.national_identity_get_name(i->save_flag);
			set_text(state, text::produce_simple_string(state, name));
		}
	}
};

class save_date : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		save_item* i = retrieve< save_item*>(state, parent);
		set_text(state, text::date_to_string(state, i->save_date));
	}
};

class save_game_item : public listbox_row_element_base<save_item> {
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<save_item>::on_create(state);
		base_data.position.x += 9; // Nudge
		base_data.position.y += 7; // Nudge
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "save_game") {
			return make_element_by_type<select_save_game>(state, id);
		} else if(name == "shield") {
			return make_element_by_type<save_flag>(state, id);
		} else if(name == "title") {
			return make_element_by_type<save_name>(state, id);
		} else if(name == "date") {
			return make_element_by_type<save_date>(state, id);
		}
		return nullptr;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept  override {
		if(payload.holds_type<save_item*>()) {
			payload.emplace<save_item*>(&content);
			return message_result::consumed;
		}
		return listbox_row_element_base<save_item>::get(state, payload);
	}
};

class saves_listbox : public listbox_element_base<save_game_item, save_item> {
protected:
	std::string_view get_row_element_name() override {
		return "alice_savegameentry";
	}

public:
	void on_create(sys::state& state) noexcept override {
		listbox_element_base<save_game_item, save_item>::on_create(state);

		row_contents.push_back(save_item{ NATIVE(""), dcon::national_identity_id{ }, dcon::government_type_id{ }, sys::date(0), true });
		// TODO -- then we push in all the actual saves

		auto sdir = simple_fs::get_or_create_save_game_directory();
		for(auto& f : simple_fs::list_files(sdir, NATIVE(".bin"))) {
			auto of = simple_fs::open_file(f);
			if(of) {
				auto content = simple_fs::view_contents(*of);
				sys::save_header h;
				if(content.file_size > sys::sizeof_save_header(h))
					sys::read_save_header(reinterpret_cast<uint8_t const*>(content.data), h);
				if(h.count == state.scenario_counter && h.timestamp == state.scenario_time_stamp) {
					row_contents.push_back(save_item{ simple_fs::get_file_name(f), h.tag, h.cgov, h.d, false });
				}
			}
		}

		update(state);
	}
};

class saves_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "save_games_list") {
			return make_element_by_type<saves_listbox>(state, id);
		}
		return nullptr;
	}
};

class pick_nation_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		if(state.network_mode == sys::network_mode::single_player) {
			disabled = n == state.local_player_nation;
		} else {
			// Prevent (via UI) the player from selecting a nation already selected by someone
			disabled = !command::can_notify_player_picks_nation(state, state.local_player_nation, n);
		}
	}

	void button_action(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		if(state.network_mode == sys::network_mode::single_player) {
			state.local_player_nation = n;
			state.ui_state.nation_picker->impl_on_update(state);
		} else {
			command::notify_player_picks_nation(state, state.local_player_nation, n);
		}
	}
};

class playable_nations_item : public listbox_row_element_base<dcon::nation_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "playable_countries_button") {
			auto ptr = make_element_by_type<pick_nation_button>(state, id);
			ptr->base_data.position.x += 9; // Nudge
			ptr->base_data.position.y = 0; // Nudge
			return ptr;
		} else if(name == "playable_countries_flag") {
			auto ptr = make_element_by_type<picker_flag>(state, id);
			ptr->base_data.position.x += 9; // Nudge
			ptr->base_data.position.y = 1; // Nudge
			return ptr;
		} else if(name == "country_name") {
			auto ptr = make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
			ptr->base_data.position.x += 9; // Nudge
			ptr->base_data.position.y = 5; // Nudge
			return ptr;
		} else if(name == "prestige_rank") {
			auto ptr = make_element_by_type<nation_prestige_rank_text>(state, id);
			ptr->base_data.position.x += 9; // Nudge
			ptr->base_data.position.y = 5; // Nudge
			return ptr;
		} else if(name == "industry_rank") {
			auto ptr = make_element_by_type<nation_industry_rank_text>(state, id);
			ptr->base_data.position.x += 9; // Nudge
			ptr->base_data.position.y = 5; // Nudge
			return ptr;
		} else if(name == "military_rank") {
			auto ptr = make_element_by_type<nation_military_rank_text>(state, id);
			ptr->base_data.position.x += 9; // Nudge
			ptr->base_data.position.y = 5; // Nudge
			return ptr;
		}
		return nullptr;
	}
};

enum class picker_sort {
	name, mil_rank, indust_rank, p_rank
};

class playable_nations_lb : public listbox_element_base<playable_nations_item, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "playable_countries_list_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto n : state.world.in_nation) {
			if(n.get_owned_province_count() > 0)
				row_contents.push_back(n);
		}
		auto s = retrieve<picker_sort>(state, parent);
		switch(s) {
			case picker_sort::name:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					return text::get_name_as_string(state, fatten(state.world, a)) < text::get_name_as_string(state, fatten(state.world, b));
				});
				break;
			case picker_sort::mil_rank:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					return state.world.nation_get_military_rank(a) < state.world.nation_get_military_rank(b);
				});
				break;
			case picker_sort::indust_rank:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					return state.world.nation_get_industrial_rank(a) < state.world.nation_get_industrial_rank(b);
				});
				break;
			case picker_sort::p_rank:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					return state.world.nation_get_prestige_rank(a) < state.world.nation_get_prestige_rank(b);
				});
				break;
		}
		update(state);
	}
};

template< picker_sort stype>
class playable_nations_sort_button : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		send(state, parent, element_selection_wrapper<picker_sort>{stype});
	}
};

class playable_nations_window : public window_element_base {
	picker_sort sort_order = picker_sort::name;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "lobby_sort_countryname") {
			auto ptr = make_element_by_type<playable_nations_sort_button<picker_sort::name>>(state, id);
			ptr->base_data.position.y += 1; // Nudge
			return ptr;
		} else if(name == "lobby_sort_prestige") {
			return make_element_by_type<playable_nations_sort_button<picker_sort::p_rank>>(state, id);
		} else if(name == "lobby_sort_industry") {
			return make_element_by_type<playable_nations_sort_button<picker_sort::indust_rank>>(state, id);
		} else if(name == "lobby_sort_military") {
			return make_element_by_type<playable_nations_sort_button<picker_sort::mil_rank>>(state, id);
		} else if(name == "playable_countries_list") {
			return make_element_by_type<playable_nations_lb>(state, id);
		}
		return nullptr;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept  override {
		if(payload.holds_type<picker_sort>()) {
			payload.emplace<picker_sort>(sort_order);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<picker_sort>>()) {
			auto v = any_cast<element_selection_wrapper<picker_sort>>(payload);
			sort_order = v.data;
			impl_on_update(state);
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

class nation_picker_hidden : public element_base {
public:
	void on_create(sys::state& state) noexcept override {
		element_base::on_create(state);
		set_visible(state, false);
	}
};

class date_label : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		text::substitution_map m;
		auto myd = state.current_date.to_ymd(state.start_date);
		text::add_to_substitution_map(m, text::variable_type::year, int64_t(myd.year));
		set_text(state, text::resolve_string_substitution(state, "the_world_in", m));
	}
};

class start_game_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.world.nation_set_is_player_controlled(state.local_player_nation, true);
		state.mode = sys::game_mode::in_game;
		state.game_state_updated.store(true, std::memory_order::release);
	}
	void on_update(sys::state& state) noexcept override {
		disabled = !bool(state.local_player_nation);
	}
};

class quit_game_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "alice_exit"));
	}
	void button_action(sys::state& state) noexcept override {
		window::close_window(state);
	}
};

class multiplayer_ping_text : public simple_text_element_base {
	uint32_t last_ms = 0;
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		if(state.network_mode == sys::network_mode::single_player) {
			set_text(state, "");
		} else {
			set_text(state, std::to_string(last_ms) + " ms");
		}
	}
};

class number_of_players_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		int32_t count = 0;
		if(state.network_mode == sys::network_mode::single_player) {
			count = 1;
		} else {
			state.world.for_each_nation([&](dcon::nation_id n) {
				if(state.world.nation_get_is_player_controlled(n))
					count++;
			});
		}

		text::substitution_map sub{};
		text::add_to_substitution_map(sub, text::variable_type::num, count);
		set_text(state, text::resolve_string_substitution(state, "fe_num_players", sub));
	}
};

class nation_picker_kick_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		disabled = !command::can_notify_player_kick(state, state.local_player_nation, n);
	}

	void button_action(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		command::notify_player_kick(state, state.local_player_nation, n);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("tip_kick"));
		text::close_layout_box(contents, box);
	}
};

class nation_picker_ban_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		disabled = !command::can_notify_player_ban(state, state.local_player_nation, n);
	}

	void button_action(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		command::notify_player_ban(state, state.local_player_nation, n);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("tip_ban"));
		text::close_layout_box(contents, box);
	}
};

class nation_picker_multiplayer_entry : public listbox_row_element_base<dcon::nation_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "player_shield") {
			auto ptr = make_element_by_type<picker_flag>(state, id);
			ptr->base_data.position.x += 10; // Nudge
			ptr->base_data.position.y += 7; // Nudge
			return ptr;
		} else if(name == "name") {
			auto ptr = make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
			ptr->base_data.position.x += 10; // Nudge
			ptr->base_data.position.y += 7; // Nudge
			return ptr;
		} else if(name == "save_progress") {
			auto ptr = make_element_by_type<multiplayer_ping_text>(state, id);
			ptr->base_data.position.x += 10; // Nudge
			ptr->base_data.position.y += 7; // Nudge
			return ptr;
		} else if(name == "button_kick") {
			auto ptr = make_element_by_type<nation_picker_kick_button>(state, id);
			ptr->base_data.position.x += 10; // Nudge
			ptr->base_data.position.y += 7; // Nudge
			return ptr;
		} else if(name == "button_ban") {
			auto ptr = make_element_by_type<nation_picker_ban_button>(state, id);
			ptr->base_data.position.x += 10; // Nudge
			ptr->base_data.position.y += 7; // Nudge
			return ptr;
		} else if(name == "frontend_player_entry") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->base_data.position.x += 10; // Nudge
			ptr->base_data.position.y += 7; // Nudge
			return ptr;
		} else {
			return nullptr;
		}
	}
};

class nation_picker_multiplayer_listbox : public listbox_element_base<nation_picker_multiplayer_entry, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "multiplayer_entry_server";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(state.network_mode == sys::network_mode::single_player) {
			row_contents.push_back(state.local_player_nation);
		} else {
			state.world.for_each_nation([&](dcon::nation_id n) {
				if(state.world.nation_get_is_player_controlled(n))
					row_contents.push_back(n);
			});
		}
		update(state);
	}
};

class nation_picker_multiplayer_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "multiplayer_list") {
			return make_element_by_type<nation_picker_multiplayer_listbox>(state, id);
		} else if(name == "checksum") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "num_players") {
			return make_element_by_type<number_of_players_text>(state, id);
		}
		return nullptr;
	}
};

class nation_picker_container : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "frontend_chat_bg") {
			return make_element_by_type<nation_picker_hidden>(state, id);
		} else if(name == "lobby_chat_edit") {
			return make_element_by_type<nation_picker_hidden>(state, id);
		} else if(name == "newgame_tab") {
			return make_element_by_type<nation_picker_hidden>(state, id);
		} else if(name == "savedgame_tab") {
			return make_element_by_type<nation_picker_hidden>(state, id);
		} else if(name == "year_label") {
			return make_element_by_type<date_label>(state, id);
		} else if(name == "multiplayer") {
			return make_element_by_type<nation_picker_multiplayer_window>(state, id);
		} else if(name == "singleplayer") {
			return make_element_by_type<nation_details_window>(state, id);
		} else if(name == "save_games") {
			return make_element_by_type<saves_window>(state, id);
		} else if(name == "bookmarks") {
			return make_element_by_type<nation_picker_hidden>(state, id);
		} else if(name == "playable_countries_window") {
			return make_element_by_type<playable_nations_window>(state, id);
		} else if(name == "back_button") {
			return make_element_by_type<quit_game_button>(state, id);
		} else if(name == "play_button") {
			return make_element_by_type<start_game_button>(state, id);
		} else if(name == "chatlog") {
			return make_element_by_type<nation_picker_hidden>(state, id);
		}

		return nullptr;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept  override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

}
