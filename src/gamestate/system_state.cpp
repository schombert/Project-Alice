#include "system_state.hpp"
#include "opengl_wrapper.hpp"
#include "window.hpp"
#include "gui_element_base.hpp"
#include <algorithm>
#include <functional>
#include "parsers_declarations.hpp"
#include "gui_minimap.hpp"

namespace sys {
	//
	// window event functions
	//

	void state::on_rbutton_down(int32_t x, int32_t y, key_modifiers mod) {
		// TODO: look at return value
		ui_state.root->impl_on_rbutton_down(*this, int32_t(x / user_settings.ui_scale), int32_t(y / user_settings.ui_scale), mod);
	}
	void state::on_mbutton_down(int32_t x, int32_t y, key_modifiers mod) {
		map_display.on_mbuttom_down(x, y, x_size, y_size, mod);
	}
	void state::on_lbutton_down(int32_t x, int32_t y, key_modifiers mod) {
		// TODO: look at return value
		ui_state.root->impl_on_lbutton_down(*this, int32_t(x / user_settings.ui_scale), int32_t(y / user_settings.ui_scale), mod);
	}
	void state::on_rbutton_up(int32_t x, int32_t y, key_modifiers mod) {

	}
	void state::on_mbutton_up(int32_t x, int32_t y, key_modifiers mod) {
		map_display.on_mbuttom_up(x, y, mod);
	}
	void state::on_lbutton_up(int32_t x, int32_t y, key_modifiers mod) {
		if(is_dragging) {
			is_dragging = false;
			on_drag_finished(x, y, mod);
		}
	}
	void state::on_mouse_move(int32_t x, int32_t y, key_modifiers mod) {
		// TODO figure out tooltips
		auto r = ui_state.root->impl_on_mouse_move(*this, int32_t(x / user_settings.ui_scale), int32_t(y / user_settings.ui_scale), mod);
		if(r != ui::message_result::consumed) {
			map_display.on_mouse_move(x, y, x_size, y_size, mod);
		}
	}
	void state::on_mouse_drag(int32_t x, int32_t y, key_modifiers mod) { // called when the left button is held down
		is_dragging = true;

		if(ui_state.drag_target)
			ui_state.drag_target->on_drag(*this,
				int32_t(mouse_x_position / user_settings.ui_scale), int32_t(mouse_y_position / user_settings.ui_scale),
				int32_t(x / user_settings.ui_scale), int32_t(y / user_settings.ui_scale),
				mod);
	}
	void state::on_drag_finished(int32_t x, int32_t y, key_modifiers mod) { // called when the left button is released after one or more drag events
		if(ui_state.drag_target) {
			ui_state.drag_target->on_drag_finish(*this);
			ui_state.drag_target = nullptr;
		}
	}
	void state::on_resize(int32_t x, int32_t y, window_state win_state) {
		if(win_state != window_state::minimized) {
			ui_state.root->base_data.size.x = int16_t(x / user_settings.ui_scale);
			ui_state.root->base_data.size.y = int16_t(y / user_settings.ui_scale);
		}
	}
	void state::on_mouse_wheel(int32_t x, int32_t y, key_modifiers mod, float amount) { // an amount of 1.0 is one "click" of the wheel
		// TODO: look at return value
		auto r = ui_state.root->impl_on_scroll(*this, int32_t(x / user_settings.ui_scale), int32_t(y / user_settings.ui_scale), amount, mod);
		if(r != ui::message_result::consumed) {
			// TODO Settings for making zooming the map faster
			map_display.on_mouse_wheel(x, y, mod, amount);
		}
	}
	void state::on_key_down(virtual_key keycode, key_modifiers mod) {
		if(!ui_state.edit_target) {
			if(ui_state.root->impl_on_key_down(*this, keycode, mod) != ui::message_result::consumed) {
				if(keycode == virtual_key::ESCAPE) {
					ui::show_main_menu(*this);
				}
				map_display.on_key_down(keycode, mod);
			}
		}
	}
	void state::on_key_up(virtual_key keycode, key_modifiers mod) {
		map_display.on_key_up(keycode, mod);
	}
	void state::on_text(char c) { // c is win1250 codepage value
		if(ui_state.edit_target)
			ui_state.edit_target->on_text(*this, c);
	}
	void state::render() { // called to render the frame may (and should) delay returning until the frame is rendered, including waiting for vsync
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		map_display.render(x_size, y_size);
		map_display.update();

		// UI rendering
		glUseProgram(open_gl.ui_shader_program);
		glUniform1f(ogl::parameters::screen_width, float(x_size) / user_settings.ui_scale);
		glUniform1f(ogl::parameters::screen_height, float(y_size) / user_settings.ui_scale);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glViewport(0, 0, x_size, y_size);
		glDepthRange(-1.0, 1.0);

		ui_state.under_mouse = ui_state.root->impl_probe_mouse(*this, int32_t(mouse_x_position / user_settings.ui_scale), int32_t(mouse_y_position / user_settings.ui_scale));
		ui_state.root->impl_render(*this, 0, 0);
	}
	void state::on_create() {
		{
			auto new_elm = ui::make_element(*this, "topbar");
			ui_state.root->add_child_to_front(std::move(new_elm));
		}
		{
			auto new_elm = ui::make_element_by_type<ui::minimap_container_window>(*this, "menubar");
			ui_state.root->add_child_to_front(std::move(new_elm));
		}
		{
			auto new_elm = ui::make_element_by_type<ui::minimap_picture_window>(*this, "minimap_pic");
			ui_state.root->add_child_to_front(std::move(new_elm));
		}
	}
	//
	// string pool functions
	//

	std::string_view state::to_string_view(dcon::text_key tag) const {
		if(!tag)
			return std::string_view();
		auto start_position = text_data.data() + tag.index();
		auto data_size = text_data.size();
		auto end_position = start_position;
		for(; end_position < text_data.data() + data_size; ++end_position) {
			if(*end_position == 0)
				break;
		}
		return std::string_view(text_data.data() + tag.index(), size_t(end_position - start_position));
	}

	dcon::text_key state::add_to_pool_lowercase(std::string const& text) {
		auto res = add_to_pool(text);
		for(auto i = 0; i < int32_t(text.length()); ++i) {
			text_data[res.index() + i] = char(tolower(text_data[res.index() + i]));
		}
		return res;
	}
	dcon::text_key state::add_to_pool_lowercase(std::string_view text) {
		auto res = add_to_pool(text);
		for(auto i = 0; i < int32_t(text.length()); ++i) {
			text_data[res.index() + i] = char(tolower(text_data[res.index() + i]));
		}
		return res;
	}
	dcon::text_key state::add_to_pool(std::string const& text) {
		auto start = text_data.size();
		auto size = text.size();
		if(size == 0)
			return dcon::text_key();
		text_data.resize(start + size + 1, char(0));
		std::memcpy(text_data.data() + start, text.c_str(), size + 1);
		return dcon::text_key(uint32_t(start));
	}
	dcon::text_key state::add_to_pool(std::string_view text) {
		auto start = text_data.size();
		auto length = text.length();
		text_data.resize(start + length + 1, char(0));
		std::memcpy(text_data.data() + start, text.data(), length);
		text_data.back() = 0;
		return dcon::text_key(uint32_t(start));
	}

	dcon::text_key state::add_unique_to_pool(std::string const& text) {
		auto search_result = std::search(text_data.data(), text_data.data() + text_data.size(), std::boyer_moore_horspool_searcher(text.c_str(), text.c_str() + text.length() + 1));
		if(search_result != text_data.data() + text_data.size()) {
			return dcon::text_key(uint32_t(search_result - text_data.data()));
		} else {
			return add_to_pool(text);
		}
	}


	void state::save_user_settings() const {
		auto settings_location = simple_fs::get_or_create_settings_directory();
		simple_fs::write_file(settings_location, NATIVE("user_settings.dat"), reinterpret_cast<char const*>(&user_settings), uint32_t(sizeof(user_settings_s)));
	}
	void state::load_user_settings() {
		auto settings_location = simple_fs::get_or_create_settings_directory();
		auto settings_file = open_file(settings_location, NATIVE("user_settings.dat"));
		if(settings_file) {
			auto content = view_contents(*settings_file);
			std::memcpy(&user_settings, content.data, std::min(uint32_t(sizeof(user_settings_s)), content.file_size));

			user_settings.interface_volume = std::clamp(user_settings.interface_volume, 0.0f, 1.0f);
			user_settings.music_volume = std::clamp(user_settings.music_volume, 0.0f, 1.0f);
			user_settings.effects_volume = std::clamp(user_settings.effects_volume, 0.0f, 1.0f);
			user_settings.master_volume = std::clamp(user_settings.master_volume, 0.0f, 1.0f);
		}
	}


	void state::update_ui_scale(float new_scale) {
		user_settings.ui_scale = new_scale;
		ui_state.root->base_data.size.x = int16_t(x_size / user_settings.ui_scale);
		ui_state.root->base_data.size.y = int16_t(y_size / user_settings.ui_scale);
		// TODO move windows
	}

	void list_pop_types(sys::state& state, parsers::scenario_building_context& context) {
		auto root = get_root(state.common_fs);
		auto poptypes = open_directory(root, NATIVE("poptypes"));

		for(auto& file : simple_fs::list_files(poptypes, NATIVE("txt"))) {
			auto full_name = get_full_name(file);
			auto last = full_name.c_str() + full_name.length();
			auto first = full_name.c_str();
			for(; last > first; --last) {
				if(*last == NATIVE('.'))
					break;
			}
			auto start_of_name = last;
			for(; start_of_name >= first; --start_of_name) {
				if(*start_of_name == NATIVE('\\') || *start_of_name == NATIVE('/')) {
					++start_of_name;
					break;
				}
			}
			auto utf8typename = simple_fs::native_to_utf8(native_string_view(start_of_name, last - start_of_name));

			auto name_id = text::find_or_add_key(context.state, utf8typename);
			auto type_id = state.world.create_pop_type();
			state.world.pop_type_set_name(type_id, name_id);
			context.map_of_poptypes.insert_or_assign(std::string(utf8typename), type_id);
		}
	}

	void state::load_scenario_data() {
		parsers::error_handler err("");

		text::load_text_data(*this, 2); // 2 = English
		ui::load_text_gui_definitions(*this, err);

		auto root = get_root(common_fs);
		auto common = open_directory(root, NATIVE("common"));

		parsers::scenario_building_context context(*this);
		// Read national tags from countries.txt
		{
			auto countries = open_file(common, NATIVE("countries.txt"));
			if(countries) {
				auto content = view_contents(*countries);
				err.file_name = "countries.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_national_identity_file(gen, err, context);
			}
		}
		// read religions from religion.txt
		{
			auto religion = open_file(common, NATIVE("religion.txt"));
			if(religion) {
				auto content = view_contents(*religion);
				err.file_name = "religion.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_religion_file(gen, err, context);
			}
		}
		// read cultures from cultures.txt
		{
			auto cultures = open_file(common, NATIVE("cultures.txt"));
			if(cultures) {
				auto content = view_contents(*cultures);
				err.file_name = "cultures.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_culture_file(gen, err, context);
			}
		}
		// read commodities from goods.txt
		{
			// FIRST: make sure that we have a money good
			if(world.commodity_size() == 0) {
				// create money
				auto money_id = world.create_commodity();
				assert(money_id.index() == 0);
			}
			auto goods = open_file(common, NATIVE("goods.txt"));
			if(goods) {
				auto content = view_contents(*goods);
				err.file_name = "goods.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_goods_file(gen, err, context);
			}
		}
		// read buildings.text
		world.factory_type_resize_construction_costs(world.commodity_size());
		{
			auto buildings = open_file(common, NATIVE("buildings.txt"));
			if(buildings) {
				auto content = view_contents(*buildings);
				err.file_name = "buildings.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_building_file(gen, err, context);
			}
		}
		// pre parse ideologies.txt
		{
			context.ideologies_file = open_file(common, NATIVE("ideologies.txt"));
			if(context.ideologies_file) {
				auto content = view_contents(*context.ideologies_file);
				err.file_name = "ideologies.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_ideology_file(gen, err, context);
			}
		}
		// pre parse issues.txt
		{
			context.issues_file = open_file(common, NATIVE("issues.txt"));
			if(context.issues_file) {
				auto content = view_contents(*context.issues_file);
				err.file_name = "issues.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_issues_file(gen, err, context);
			}
		}
		// parse governments.txt
		{
			auto governments = open_file(common, NATIVE("governments.txt"));
			if(governments) {
				auto content = view_contents(*governments);
				err.file_name = "governments.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_governments_file(gen, err, context);
			}
		}
		// pre parse cb_types.txt
		{
			context.cb_types_file = open_file(common, NATIVE("cb_types.txt"));
			if(context.cb_types_file) {
				auto content = view_contents(*context.cb_types_file);
				err.file_name = "cb_types.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_cb_types_file(gen, err, context);
			}
		}
		// parse traits.txt
		{
			auto traits = open_file(common, NATIVE("traits.txt"));
			if(traits) {
				auto content = view_contents(*traits);
				err.file_name = "traits.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_traits_file(gen, err, context);
			}
		}
		// pre parse crimes.txt
		{
			context.crimes_file = open_file(common, NATIVE("crime.txt"));
			if(context.crimes_file) {
				auto content = view_contents(*context.crimes_file);
				err.file_name = "crime.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_crimes_file(gen, err, context);
			}
		}
		// pre parse triggered_modifiers.txt
		{
			context.triggered_modifiers_file = open_file(common, NATIVE("triggered_modifiers.txt"));
			if(context.triggered_modifiers_file) {
				auto content = view_contents(*context.triggered_modifiers_file);
				err.file_name = "triggered_modifiers.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_triggered_modifiers_file(gen, err, context);
			}
		}
		// parse nationalvalues.txt
		{
			auto nv_file = open_file(common, NATIVE("nationalvalues.txt"));
			if(nv_file) {
				auto content = view_contents(*nv_file);
				err.file_name = "nationalvalues.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_national_values_file(gen, err, context);
			}
		}
		// parse static_modifiers.txt
		{
			auto sm_file = open_file(common, NATIVE("static_modifiers.txt"));
			if(sm_file) {
				auto content = view_contents(*sm_file);
				err.file_name = "static_modifiers.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_static_modifiers_file(gen, err, context);
			}
		}
		// parse event_modifiers.txt
		{
			auto em_file = open_file(common, NATIVE("event_modifiers.txt"));
			if(em_file) {
				auto content = view_contents(*em_file);
				err.file_name = "event_modifiers.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_event_modifiers_file(gen, err, context);
			}
		}
		// read defines.lua
		{
			auto defines_file = open_file(common, NATIVE("defines.lua"));
			if(defines_file) {
				auto content = view_contents(*defines_file);
				err.file_name = "defines.lua";
				defines.parse_file(*this, std::string_view(content.data, content.data + content.file_size), err);
			}
		}
		// gather names of poptypes
		list_pop_types(*this, context);
		// pre parse rebel_types.txt
		{
			context.rebel_types_file = open_file(common, NATIVE("rebel_types.txt"));
			if(context.rebel_types_file) {
				auto content = view_contents(*context.rebel_types_file);
				err.file_name = "rebel_types.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_rebel_types_file(gen, err, context);
			}
		}

		auto map = open_directory(root, NATIVE("map"));
		// parse default.map
		{
			auto def_map_file = open_file(map, NATIVE("default.map"));
			if(def_map_file) {
				auto content = view_contents(*def_map_file);
				err.file_name = "default.map";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_default_map_file(gen, err, context);
			}
		}
		// parse definition.csv
		{
			auto def_csv_file = open_file(map, NATIVE("definition.csv"));
			if(def_csv_file) {
				auto content = view_contents(*def_csv_file);
				err.file_name = "definition.csv";
				parsers::read_map_colors(content.data, content.data + content.file_size, err, context);
			}
		}
		// parse terrain.txt
		{
			auto terrain_file = open_file(map, NATIVE("terrain.txt"));
			if(terrain_file) {
				auto content = view_contents(*terrain_file);
				err.file_name = "terrain.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_terrain_file(gen, err, context);
			}
		}
		// parse region.txt
		{
			auto region_file = open_file(map, NATIVE("region.txt"));
			if(region_file) {
				auto content = view_contents(*region_file);
				err.file_name = "region.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_region_file(gen, err, context);
			}
		}
		// parse continent.txt
		{
			auto continent_file = open_file(map, NATIVE("continent.txt"));
			if(continent_file) {
				auto content = view_contents(*continent_file);
				err.file_name = "continent.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_continent_file(gen, err, context);
			}
		}
		// parse climate.txt
		{
			auto climate_file = open_file(map, NATIVE("climate.txt"));
			if(climate_file) {
				auto content = view_contents(*climate_file);
				err.file_name = "climate.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_climate_file(gen, err, context);
			}
		}
		// TODO do something with err
	}
}
