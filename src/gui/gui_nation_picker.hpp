#pragma once

#include "gui_element_types.hpp"
#include "gui_chat_window.hpp"
#include "serialization.hpp"

namespace ui {

class nation_picker_poptypes_chart : public piechart<dcon::pop_type_id> {
protected:
	void on_update(sys::state& state) noexcept override {
		distribution.clear();
		auto n = retrieve<dcon::nation_id>(state, parent);
		for(auto pt : state.world.in_pop_type) {
			auto amount = state.world.nation_get_demographics(n, demographics::to_key(state, pt));
			distribution.emplace_back(pt.id, amount);
		}
		update_chart(state);
	}
};

class nation_picker_cultures_chart : public piechart<dcon::culture_id> {
protected:
	void on_update(sys::state& state) noexcept override {
		distribution.clear();
		auto n = retrieve<dcon::nation_id>(state, parent);
		for(auto c : state.world.in_culture) {
			auto amount = state.world.nation_get_demographics(n, demographics::to_key(state, c));
			distribution.emplace_back(c.id, amount);
		}
		update_chart(state);
	}
};

class nation_picker_ideologies_chart : public piechart<dcon::ideology_id> {
protected:
	void on_update(sys::state& state) noexcept override {
		distribution.clear();
		auto n = retrieve<dcon::nation_id>(state, parent);
		for(auto c : state.world.in_ideology) {
			auto amount = state.world.nation_get_demographics(n, demographics::to_key(state, c));
			distribution.emplace_back(c.id, amount);
		}
		update_chart(state);
	}
};

class nation_details_window : public window_element_base {
	int16_t piechart_offset = 0;
	element_base* overlay1 = nullptr;
	element_base* overlay2 = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "player_shield") {
			return make_element_by_type<flag_button>(state, id);
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
			// at left
			auto aptr = make_element_by_type<nation_picker_cultures_chart>(state, id);
			if(piechart_offset == 0)
				piechart_offset = aptr->base_data.size.x + 4;
			aptr->base_data.position.x -= piechart_offset;
			add_child_to_front(std::move(aptr));
			// at middle
			auto bptr = make_element_by_type<nation_picker_ideologies_chart>(state, id);
			add_child_to_front(std::move(bptr));
			// at right
			auto cptr = make_element_by_type<nation_picker_poptypes_chart>(state, id);
			cptr->base_data.position.x += piechart_offset;
			// bring overlays on top
			if(overlay1)
				move_child_to_front(overlay1);
			if(overlay2)
				move_child_to_front(overlay2);
			return cptr;
		} else if(name == "selected_population_chart_overlay") {
			// at left
			auto aptr = make_element_by_type<image_element_base>(state, id);
			if(piechart_offset == 0)
				piechart_offset = aptr->base_data.size.x + 4;
			aptr->base_data.position.x -= piechart_offset;
			overlay1 = aptr.get();
			add_child_to_front(std::move(aptr));
			// at middle
			auto bptr = make_element_by_type<image_element_base>(state, id);
			overlay2 = bptr.get();
			add_child_to_front(std::move(bptr));
			// at right
			auto cptr = make_element_by_type<image_element_base>(state, id);
			cptr->base_data.position.x += piechart_offset;
			return cptr;
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
			ptr->base_data.position.x += 20;
			return ptr;
		} else if(name == "puppets_label") {
			auto ptr = make_element_by_type<nation_puppet_list_label>(state, id);
			return ptr;
		}

		return nullptr;
	}
};


struct save_item {
	native_string file_name;
	uint64_t timestamp = 0;
	sys::date save_date;
	dcon::national_identity_id save_flag;
	dcon::government_type_id as_gov;
	bool is_new_game = false;
	std::string name = "fe_new_game";

	bool is_bookmark() const {
		return file_name.starts_with(NATIVE("bookmark_"));
	}

	bool operator==(save_item const& o) const {
		return save_flag == o.save_flag && as_gov == o.as_gov && save_date == o.save_date && is_new_game == o.is_new_game && file_name == o.file_name && timestamp == o.timestamp;
	}
	bool operator!=(save_item const& o) const {
		return !(*this == o);
	}
};

class select_save_game : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		disabled = state.network_mode == sys::network_mode_type::client;
	}

	void button_action(sys::state& state) noexcept override {
		save_item* i = retrieve< save_item*>(state, parent);
		if(!i->is_new_game && i->file_name == state.loaded_save_file)
			return;

		window::change_cursor(state, window::cursor_type::busy); //show busy cursor so player doesn't question
		if(state.ui_state.request_window)
			static_cast<ui::diplomacy_request_window*>(state.ui_state.request_window)->messages.clear();
		if(state.ui_state.msg_window)
			static_cast<ui::message_window*>(state.ui_state.msg_window)->messages.clear();
		if(state.ui_state.request_topbar_listbox)
			static_cast<ui::diplomatic_message_topbar_listbox*>(state.ui_state.request_topbar_listbox)->messages.clear();
		if(state.ui_state.msg_log_window)
			static_cast<ui::message_log_window*>(state.ui_state.msg_log_window)->messages.clear();
		for(const auto& win : land_combat_end_popup::land_reports_pool)
			win->set_visible(state, false);
		for(const auto& win : naval_combat_end_popup::naval_reports_pool)
			win->set_visible(state, false);
		ui::clear_event_windows(state);

		state.network_state.save_slock.lock();
		std::vector<dcon::nation_id> no_ai_nations;
		for(const auto n : state.world.in_nation)
			if(state.world.nation_get_is_player_controlled(n))
				no_ai_nations.push_back(n);
		dcon::nation_id old_local_player_nation = state.local_player_nation;
		/*state.preload();*/
		/*state.render_semaphore.acquire();*/
		state.reset_state();
		bool loaded = false;
		if(i->is_new_game) {
			if(!sys::try_read_scenario_as_save_file(state, state.loaded_scenario_file)) {
				auto msg = std::string("Scenario file ") + simple_fs::native_to_utf8(state.loaded_scenario_file) + " could not be loaded.";
				ui::popup_error_window(state, "Scenario Error", msg);
			} else {
				loaded = true;
			}
		} else {
			if(!sys::try_read_save_file(state, i->file_name)) {
				auto msg = std::string("Save file ") + simple_fs::native_to_utf8(i->file_name) + " could not be loaded.";
				ui::popup_error_window(state, "Save Error", msg);
				state.save_list_updated.store(true, std::memory_order::release); //update savefile list
				//try loading save from scenario so we atleast have something to work on
				if(!sys::try_read_scenario_as_save_file(state, state.loaded_scenario_file)) {
					auto msg2 = std::string("Scenario file ") + simple_fs::native_to_utf8(state.loaded_scenario_file) + " could not be loaded.";
					ui::popup_error_window(state, "Scenario Error", msg2);
				} else {
					loaded = true;
				}
			} else {
				loaded = true;
			}
		}
		if(loaded) {
			/* Updating this flag lets the network state know that we NEED to send the
			savefile data, otherwise it is safe to assume the client has its own data
			friendly reminder that, scenario loading and reloading ends up with different outcomes */
			state.network_state.is_new_game = false;
			if(state.network_mode == sys::network_mode_type::host) {


				// notfy every client that every client is now loading (loading the save)
				for(auto& loading_client : state.network_state.clients) {
					if(loading_client.is_active()) {
						network::notify_player_is_loading(state, loading_client.hshake_buffer.nickname, loading_client.playing_as, true);
					}
				}

				/* Save the buffer before we fill the unsaved data */
				state.local_player_nation = dcon::nation_id{ };
				//network::place_host_player_after_saveload(state);

				network::write_network_save(state);
				network::set_no_ai_nations_after_reload(state, no_ai_nations, old_local_player_nation);
				state.fill_unsaved_data();
				state.network_state.current_mp_state_checksum = state.get_mp_state_checksum();

				assert(state.world.nation_get_is_player_controlled(state.local_player_nation));
				/* Now send the saved buffer before filling the unsaved data to the clients
				henceforth. */
				command::payload c;
				memset(&c, 0, sizeof(command::payload));
				c.type = command::command_type::notify_save_loaded;
				c.source = state.local_player_nation;
				c.data.notify_save_loaded.target = dcon::nation_id{};
				network::broadcast_save_to_clients(state, c, state.network_state.current_save_buffer.get(), state.network_state.current_save_length, state.network_state.current_mp_state_checksum);
			} else {
				state.fill_unsaved_data();
			}
		}
		/* Savefiles might load with new railroads, so for responsiveness we
		   update whenever one is loaded. */
		state.map_state.set_selected_province(dcon::province_id{});
		state.map_state.unhandled_province_selection = true;
		state.railroad_built.store(true, std::memory_order::release);
		state.sprawl_update_requested.store(true, std::memory_order::release);
		state.network_state.save_slock.unlock();
		state.game_state_updated.store(true, std::memory_order_release);
		/*state.render_semaphore.release();*/

		window::change_cursor(state, window::cursor_type::normal); //normal cursor now
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
	void button_action(sys::state& state) noexcept override { }

	void on_update(sys::state& state) noexcept  override {
		save_item* i = retrieve< save_item*>(state, parent);
		auto tag = i->save_flag;
		auto gov = i->as_gov;
		visible = !i->is_new_game && !i->is_bookmark();

		if(!visible)
			return;

		if(!bool(tag))
			tag = state.national_definitions.rebel_id;

		dcon::government_flag_id ft{};
		if(gov) {
			auto id = state.world.national_identity_get_government_flag_type(tag, gov);
			if(id)
				ft = id;
			else
				ft = state.world.government_type_get_flag(gov);
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
					flag_texture_handle, mask_handle, base_data.get_rotation(), gfx_def.is_vertically_flipped(),
					false);
			} else {
				ogl::render_textured_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
					float(x), float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, base_data.get_rotation(),
					gfx_def.is_vertically_flipped(),
					false);
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
		} else if(i->is_bookmark()) {
			set_text(state, text::produce_simple_string(state, i->name));
		} else {
			auto name = text::get_name(state, state.world.national_identity_get_nation_from_identity_holder(i->save_flag));
			if(auto gov_name = state.world.national_identity_get_government_name(i->save_flag, i->as_gov); state.key_is_localized(gov_name)) {
				name = gov_name;
			}
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

class save_game_item : public listbox_row_element_base<std::shared_ptr<save_item>> {
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<std::shared_ptr<save_item>>::on_create(state);
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
			payload.emplace<save_item*>(content.get());
			return message_result::consumed;
		}
		return listbox_row_element_base<std::shared_ptr<save_item>>::get(state, payload);
	}
};

class saves_listbox : public listbox_element_base<save_game_item, std::shared_ptr<save_item>> {
protected:
	std::string_view get_row_element_name() override {
		return "alice_savegameentry";
	}

	void update_save_list(sys::state& state) noexcept {
		row_contents.clear();
		row_contents.push_back(std::make_shared<save_item>(save_item{ NATIVE(""), 0, sys::date(0), dcon::national_identity_id{ }, dcon::government_type_id{ }, true, std::string("") }));

		auto sdir = simple_fs::get_or_create_save_game_directory();
		for(auto& f : simple_fs::list_files(sdir, NATIVE(".bin"))) {
			auto of = simple_fs::open_file(f);
			if(of) {
				auto content = simple_fs::view_contents(*of);
				sys::save_header h;
				if(content.file_size > sys::sizeof_save_header(h))
					sys::read_save_header(reinterpret_cast<uint8_t const*>(content.data), h);
				if(h.checksum.is_equal(state.scenario_checksum)) {
					row_contents.push_back(std::make_shared<save_item>(save_item{ simple_fs::get_file_name(f), h.timestamp, h.d, h.tag, h.cgov, false, std::string(h.save_name) }));
				}
			}
		}

		std::sort(row_contents.begin() + 1, row_contents.end(), [](std::shared_ptr<save_item> const& a, std::shared_ptr<save_item> const& b) {
			if(a->is_bookmark() != b->is_bookmark())
				return a->is_bookmark();
			return a->timestamp > b->timestamp;
		});

		update(state);
	}

public:
	void on_create(sys::state& state) noexcept override {
		base_data.size.x -= 20; //nudge
		base_data.size.y += base_data.position.y;
		base_data.position.y = 0;
		listbox_element_base<save_game_item, std::shared_ptr<save_item>>::on_create(state);
		update_save_list(state);
	}

	void on_update(sys::state& state) noexcept override {
		if(state.save_list_updated.load(std::memory_order::acquire) == true) {
			state.save_list_updated.store(false, std::memory_order::release); // acknowledge update
			update_save_list(state);
		}
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
		auto nation = retrieve<dcon::nation_id>(state, parent);
		if(state.network_mode == sys::network_mode_type::single_player) {
			disabled = nation == state.local_player_nation;
		} else {
			// Prevent (via UI) the player from selecting a nation already selected by someone
			disabled = !command::can_notify_player_picks_nation(state, state.local_player_nation, nation, state.network_state.nickname);
		}
	}

	void button_action(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		if(state.network_mode == sys::network_mode_type::single_player) {
			state.local_player_nation = n;
			state.world.nation_set_is_player_controlled(n, true);
			state.ui_state.nation_picker->impl_on_update(state);
		} else {
			command::notify_player_picks_nation(state, state.local_player_nation, n, state.network_state.nickname);
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
			auto ptr = make_element_by_type<flag_button>(state, id);
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
		default:
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
		if(state.network_mode == sys::network_mode_type::client) {
			//clients cant start the game, only tell that they're "ready"
		} else {
			if(command::can_notify_start_game(state, state.local_player_nation)) {
				if(auto cap = state.world.nation_get_capital(state.local_player_nation); cap) {
					if(state.map_state.get_zoom() < map::zoom_very_close)
						state.map_state.zoom = map::zoom_very_close;
					state.map_state.center_map_on_province(state, cap);
				}
				command::notify_start_game(state, state.local_player_nation);
			}
			
		}
	}

	void on_update(sys::state& state) noexcept override {
		disabled = !command::can_notify_start_game(state, state.local_player_nation);

		if(state.network_mode == sys::network_mode_type::client) {
			if(state.network_state.save_stream) { //in the middle of a save stream
				disabled = true;
			}
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.network_mode == sys::network_mode_type::host) {
			bool old_disabled = disabled;
			for(auto const& client : state.network_state.clients) {
				if(client.is_active()) {
					disabled = disabled || !client.send_buffer.empty();
				}
			}
			button_element_base::render(state, x, y);
			disabled = old_disabled;
		} else if(state.network_mode == sys::network_mode_type::client) {
			if(state.network_state.save_stream) {
				set_button_text(state, text::format_percentage(float(state.network_state.recv_count) / float(state.network_state.save_data.size())));
			} else {
				set_button_text(state, text::produce_simple_string(state, "ready"));
			}
			button_element_base::render(state, x, y);
		} else {
			button_element_base::render(state, x, y);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		if(state.network_mode == sys::network_mode_type::client) {
			if(state.network_state.save_stream) {
				text::localised_format_box(state, contents, box, std::string_view("alice_play_save_stream"));
			}
			for(auto const& client : state.network_state.clients) {
				if(client.is_active()) {
					if(!client.send_buffer.empty()) {
						text::substitution_map sub;
						text::add_to_substitution_map(sub, text::variable_type::playername, client.playing_as);
						text::localised_format_box(state, contents, box, std::string_view("alice_play_pending_client"), sub);
					}
				}
			}
		}
		else if(state.network_mode == sys::network_mode_type::host) {
			if(network::check_any_players_loading(state)) {
				text::localised_format_box(state, contents, box, std::string_view("alice_no_start_game_player_loading"));
			}
			if(network::any_player_on_invalid_nation(state)) {
				text::localised_format_box(state, contents, box, std::string_view("alice_no_start_game_invalid_nations"));
			}
		}
		text::close_layout_box(contents, box);
	}
};

class quit_game_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		window::close_window(state);
	}
};

class observer_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto observer_nation = state.world.national_identity_get_nation_from_identity_holder(state.national_definitions.rebel_id);
		if(state.network_mode == sys::network_mode_type::single_player) {
			auto previous_nation = state.local_player_nation;
			state.local_player_nation = observer_nation;
			state.world.nation_set_is_player_controlled(observer_nation, true);
			if(previous_nation) {
				state.world.nation_set_is_player_controlled(previous_nation, false);
			}
			state.ui_state.nation_picker->impl_on_update(state);
		} else {
			if(command::can_notify_player_picks_nation(state, state.local_player_nation, observer_nation, state.network_state.nickname)) {
				command::notify_player_picks_nation(state, state.local_player_nation, observer_nation, state.network_state.nickname);
			}
		}
	}
	void on_update(sys::state& state) noexcept override {
		auto observer_nation = state.world.national_identity_get_nation_from_identity_holder(state.national_definitions.rebel_id);
		if(state.network_mode == sys::network_mode_type::single_player) {
			disabled = state.local_player_nation == observer_nation;
		}
		else {
			disabled = !command::can_notify_player_picks_nation(state, state.local_player_nation, observer_nation, state.network_state.nickname);
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		auto observer_nation = state.world.national_identity_get_nation_from_identity_holder(state.national_definitions.rebel_id);
		if(state.local_player_nation == observer_nation) {
			text::localised_format_box(state, contents, box, std::string_view("alice_observer_already_observer_tooltip"));
		}
		text::close_layout_box(contents, box);
	}

};

class multiplayer_status_text : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto player = retrieve<dcon::mp_player_id>(state, parent);

		if(state.network_mode == sys::network_mode_type::single_player) {
			color = text::text_color::dark_green;
			set_text(state, text::produce_simple_string(state, "ready"));
		}
		else {;
			if(state.world.mp_player_get_fully_loaded(player)) {
				color = text::text_color::dark_green;
				set_text(state, text::produce_simple_string(state, "ready"));
			} else {
				color = text::text_color::yellow;
				set_text(state, text::produce_simple_string(state, "Loading"));
			}
		}


		//if(state.network_mode == sys::network_mode_type::host) {
		//	// on render
		//} else {
		//	set_text(state, text::produce_simple_string(state, "ready"));
		//}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		//auto n = retrieve<dcon::nation_id>(state, parent);
		//if(state.network_mode == sys::network_mode_type::host) {
		//	set_text(state, text::produce_simple_string(state, "ready")); // default
		//	if(state.network_state.is_new_game == false) {
		//		for(auto const& c : state.network_state.clients) {
		//			if(c.is_active() && c.playing_as == n) {
		//				auto completed = c.total_sent_bytes - c.save_stream_offset;
		//				auto total = c.save_stream_size;
		//				if(total > 0.f) {
		//					float progress = float(completed) / float(total);
		//					if(progress < 1.f) {
		//						text::substitution_map sub{};
		//						text::add_to_substitution_map(sub, text::variable_type::value, text::fp_percentage_one_place{ progress });
		//						set_text(state, text::produce_simple_string(state, text::resolve_string_substitution(state, "alice_status_stream", sub)));
		//					}
		//				}
		//				break;
		//			}
		//		}
		//	}
		//}
		color_text_element::render(state, x, y);
	}
};

class lobby_player_flag_button : public flag_button {
	public:
		dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
			if(state.network_mode == sys::network_mode_type::single_player) {
				return state.world.nation_get_identity_from_identity_holder(state.local_player_nation);
			}
			else {
				auto player = retrieve<dcon::mp_player_id>(state, parent);
				auto nation = state.world.mp_player_get_nation_from_player_nation(player);
				return state.world.nation_get_identity_from_identity_holder(nation);
			}
		}

};

class number_of_players_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		int32_t count = 0;
		if(state.network_mode == sys::network_mode_type::single_player) {
			count = 1;
		} else {
			state.world.for_each_mp_player([&](dcon::mp_player_id p) {
				count++;
			});
		}

		text::substitution_map sub{};
		text::add_to_substitution_map(sub, text::variable_type::num, count);
		set_text(state, text::resolve_string_substitution(state, "fe_num_players", sub));
	}
};

class nation_picker_multiplayer_entry : public listbox_row_element_base<dcon::mp_player_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "player_shield") {
			auto ptr = make_element_by_type<lobby_player_flag_button>(state, id);
			ptr->base_data.position.x += 10; // Nudge
			ptr->base_data.position.y += 7; // Nudge
			return ptr;
		} else if(name == "name") {
			auto ptr = make_element_by_type<player_name_text>(state, id);
			ptr->base_data.position.x += 10; // Nudge
			ptr->base_data.position.y += 7; // Nudge
			return ptr;
		} else if(name == "save_progress") {
			auto ptr = make_element_by_type<multiplayer_status_text>(state, id);
			ptr->base_data.position.x += 10; // Nudge
			ptr->base_data.position.y += 7; // Nudge
			return ptr;
		} else if(name == "button_kick") {
			auto ptr = make_element_by_type<player_kick_button>(state, id);
			ptr->base_data.position.x += 10; // Nudge
			ptr->base_data.position.y += 7; // Nudge
			return ptr;
		} else if(name == "button_ban") {
			auto ptr = make_element_by_type<player_ban_button>(state, id);
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

class nation_picker_multiplayer_listbox : public listbox_element_base<nation_picker_multiplayer_entry, dcon::mp_player_id> {
protected:
	std::string_view get_row_element_name() override {
		return "multiplayer_entry_server";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(state.network_mode == sys::network_mode_type::single_player) {
			row_contents.push_back(dcon::mp_player_id{ });
		} else {
			state.world.for_each_mp_player([&](dcon::mp_player_id p) {
				row_contents.push_back(p);
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

class nation_alice_readme_text : public scrollable_text {
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept {
		text::add_line(state, contents, "alice_readme");
		text::add_line_break_to_layout(state, contents);
		text::add_line(state, contents, "gc_desc");
	}
public:
	void on_reset_text(sys::state& state) noexcept override {
		auto container = text::create_endless_layout(state, delegate->internal_layout,
		text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y),
			base_data.data.text.font_handle, 0, text::alignment::left,
			text::is_black_from_font_id(base_data.data.text.font_handle) ? text::text_color::black : text::text_color::white,
			false });
		populate_layout(state, container);
		calibrate_scrollbar(state);
	}
	void on_create(sys::state& state) noexcept override {
		scrollable_text::on_create(state);
		on_reset_text(state);
	}
};

class nation_picker_container : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "frontend_chat_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "lobby_chat_edit") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "newgame_tab") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "savedgame_tab") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "year_label") {
			return make_element_by_type<date_label>(state, id);
		} else if(name == "multiplayer") {
			return make_element_by_type<nation_picker_multiplayer_window>(state, id);
		} else if(name == "singleplayer") {
			return make_element_by_type<nation_details_window>(state, id);
		} else if(name == "save_games") {
			return make_element_by_type<saves_window>(state, id);
		} else if(name == "bookmarks") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "playable_countries_window") {
			return make_element_by_type<playable_nations_window>(state, id);
		} else if(name == "back_button") {
			return make_element_by_type<quit_game_button>(state, id);
		} else if(name == "play_button") {
			return make_element_by_type<start_game_button>(state, id);
		} else if(name == "chatlog") {
			auto ptr = make_element_by_type<nation_alice_readme_text>(state, state.ui_state.defs_by_name.find(state.lookup_key("alice_readme_text"))->second.definition);
			add_child_to_front(std::move(ptr));
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "observer_button") {
			return make_element_by_type<observer_button>(state, id);

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
