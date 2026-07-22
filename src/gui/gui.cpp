#include "gui_map_icons.hpp"
#include "gui_element_base.hpp"
#include "gui_templates.hpp"

namespace ui {

void tl_edge::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(visible)
		image_element_base::render(state, x, y);
}


// GUI component for map_general element of top_unit_icon, use together with alice_render_on_map_generals define
class tl_map_general_icon : public button_element_base {
public:
	bool visible = true;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			button_element_base::render(state, x, y);
	}

	void on_update(sys::state& state) noexcept override {
		auto top_icon = (top_unit_icon*) parent;
		auto counter = (unit_counter_window*) top_icon->parent;
		auto& params = counter->display_cache[counter->current_province.index()];
		// frame = int32_t(params->colors[0]);
		auto army = params.top_army_id;
		if(army) {
			if(!state.world.army_get_general_from_army_leadership(army)) {
				visible = false;
				return;
			}
			auto loc = state.world.army_get_location_from_army_location(army);
			auto map_pos = state.world.province_get_mid_point(loc);

			if(state.world.army_get_arrival_time(army) && state.world.army_get_path(army).size() > 0) {
				auto dest = state.world.army_get_path(army)[0];

				if(dest == loc) {
					return;
				}
				auto dest_pos = state.world.province_get_mid_point(dest);

				float dx = dest_pos.x - map_pos.x;
				float dy = dest_pos.y - map_pos.y;
				float angle = std::atan2(dy, dx) * (180.0f / std::numbers::pi_v<float>);
				// 8 directional segments starting from right and up
				frame = static_cast<int>(std::fmod(angle + 360.0f + 22.5f, 360.0f) / 45.0f);
			}
		}
	}
	void button_action(sys::state& state) noexcept override {
		send(state, parent, toggle_unit_grid{ false });
	}
	void button_shift_action(sys::state& state) noexcept override {
		send(state, parent, toggle_unit_grid{ true });
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		populate_unit_tooltip(state, contents, retrieve<dcon::province_id>(state, parent));
	}
};

message_result prov_map_battle_bar::test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept {
	return opaque_element_base::test_mouse(state, x, y, type);
}
message_result prov_map_battle_bar::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);

	auto prov = retrieve<dcon::province_id>(state, parent);
	if(state.world.province_get_land_battle_location(prov).begin() != state.world.province_get_land_battle_location(prov).end()) {
		auto b = *state.world.province_get_land_battle_location(prov).begin();

		auto w = b.get_battle().get_war_from_land_battle_in_war();
		land_combat_window* win = static_cast<land_combat_window*>(state.ui_state.army_combat_window);
		win->battle = b.get_battle();
		if(state.ui_state.army_combat_window->is_visible()) {
			state.ui_state.army_combat_window->impl_on_update(state);
		} else {
			state.ui_state.army_combat_window->set_visible(state, true);
			if(state.ui_state.province_window) {
				state.ui_state.province_window->set_visible(state, false);
				state.set_selected_province(dcon::province_id{});
			}
			if(state.ui_state.naval_combat_window) {
				state.ui_state.naval_combat_window->set_visible(state, false);
			}
		}
		return message_result::consumed;
	}
		
	if(state.world.province_get_naval_battle_location(prov).begin() != state.world.province_get_naval_battle_location(prov).end()) {
		auto b = *state.world.province_get_naval_battle_location(prov).begin();
		auto w = b.get_battle().get_war_from_naval_battle_in_war();

		naval_combat_window* win = static_cast<naval_combat_window*>(state.ui_state.naval_combat_window);
		win->battle = b.get_battle();
		if(state.ui_state.naval_combat_window->is_visible()) {
			state.ui_state.naval_combat_window->impl_on_update(state);
		} else {
			state.ui_state.naval_combat_window->set_visible(state, true);
			if(state.ui_state.province_window) {
				state.ui_state.province_window->set_visible(state, false);
				state.set_selected_province(dcon::province_id{});
			}
			if(state.ui_state.army_combat_window) {
				state.ui_state.army_combat_window->set_visible(state, false);
			}
		}
		return message_result::consumed;
	}
	return message_result::consumed;
}

void tl_unit_1::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(visible)
		image_element_base::render(state, x, y);
}
void tl_unit_2::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(visible)
		image_element_base::render(state, x, y);
}
void top_unit_icon::on_create(sys::state& state) noexcept {
	auto first_child = base_data.data.window.first_child;
	auto num_children = base_data.data.window.num_children;
	for(uint32_t i = num_children; i-- > 0;) {
		auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(i + first_child.index()));
		make_child(state, state.to_string_view(state.ui_defs.gui[child_tag].name), child_tag);
	}
}
std::unique_ptr<element_base> top_unit_icon::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "controller_flag") {
		auto& item = flag;
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "strength") {
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(strength_label.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, strength_label.base_data);
		strength_label.parent = this;
	} else if(name == "unit_1") {
		auto& item = unit1;
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "unit_2") {
		auto& item = unit2;
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "dig_in") {
		auto& item = dig_in;
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "status") {
		auto& item = status;
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "org_bar") {
		auto& item = org_bar;
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "edge1") {
		auto& item = edge[0];
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "edge2") {
		auto& item = edge[1];
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "edge3") {
		auto& item = edge[2];
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "edge4") {
		auto& item = edge[3];
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "frame_bg") {
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(frame.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, frame.base_data);
		frame.on_create(state);
		frame.parent = this;
	} else if(name == "map_general") {
		return make_element_by_type<tl_map_general_icon>(state, id);
	}

	return nullptr;
}

mouse_probe top_unit_icon::impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept {
	if(state.map_state.get_zoom() >= big_counter_cutoff) {
		auto probe = window_element_base::impl_probe_mouse(state, x, y, type);
		{
			auto& item = frame;
			probe = item.impl_probe_mouse(state, x, y, type);
			if(probe.under_mouse) return probe;
		}
		for (int i = 0; i < 4; i++) {
			auto& item = edge[i];
			if(item.visible) {
				probe = item.impl_probe_mouse(state, x, y, type);
				if(probe.under_mouse) return probe;
			}
		}
		{
			auto& item = unit1;
			if(item.visible) {
				probe = item.impl_probe_mouse(state, x, y, type);
				if(probe.under_mouse) return probe;
			}
		}
		{
			auto& item = unit2;
			if(item.visible) {
				probe = item.impl_probe_mouse(state, x, y, type);
				if(probe.under_mouse) return probe;
			}
		}
		if (show_flag) {
			auto& item = flag;
			probe = item.impl_probe_mouse(state, x, y, type);
			if(probe.under_mouse) return probe;
		}
		{
			auto& item = dig_in;
			probe = item.impl_probe_mouse(state, x, y, type);
			if(probe.under_mouse) return probe;
		}
		{
			auto& item = org_bar;
			probe = item.impl_probe_mouse(state, x, y, type);
			if(probe.under_mouse) return probe;
		}
		{
			auto& item = status;
			probe = item.impl_probe_mouse(state, x, y, type);
			if(probe.under_mouse) return probe;
		}
		{
			auto& item = strength_label;
			probe = item.impl_probe_mouse(state, x, y, type);
			if(probe.under_mouse) return probe;
		}

		return mouse_probe{ nullptr, ui::xy_pair{0,0} };
	} else
		return mouse_probe{ nullptr, ui::xy_pair{0,0} };
}
void top_unit_icon::impl_render(sys::state& state, int32_t x, int32_t y) noexcept {
	{
		auto& item = frame;
		auto shift = child_relative_location(state, *this, item);
		item.impl_render(state, x + shift.x, y + shift.y);
	}
	for (int i = 0; i < 4; i++) {
		auto& item = edge[i];
		if(item.visible) {
			auto shift = child_relative_location(state, *this, item);
			item.impl_render(state, x + shift.x, y + shift.y);
		}
	}
	{
		auto& item = unit1;
		if(item.visible) {
			auto shift = child_relative_location(state, *this, item);
			item.impl_render(state, x + shift.x, y + shift.y);
		}
	}
	{
		auto& item = unit2;
		if(item.visible) {
			auto shift = child_relative_location(state, *this, item);
			item.impl_render(state, x + shift.x, y + shift.y);
		}
	}
	if (show_flag) {
		auto& item = flag;
		auto shift = child_relative_location(state, *this, item);
		item.impl_render(state, x + shift.x, y + shift.y);
	}
	{
		auto& item = dig_in;
		auto shift = child_relative_location(state, *this, item);
		item.impl_render(state, x + shift.x, y + shift.y);
	}
	{
		auto& item = org_bar;
		auto shift = child_relative_location(state, *this, item);
		item.impl_render(state, x + shift.x, y + shift.y);
	}
	{
		auto& item = status;
		auto shift = child_relative_location(state, *this, item);
		item.impl_render(state, x + shift.x, y + shift.y);
	}
	{
		auto shift = child_relative_location(state, *this, strength_label);
		strength_label.impl_render(state, x + shift.x, y + shift.y);
	}
}


std::unique_ptr<element_base> small_top_unit_icon::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "controller_flag") {
		auto& item = flag;
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "strength") {
		auto& item = strength_label;
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.parent = this;
	} else if(name == "frame_bg") {
		auto& item = frame;
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	} else if(name == "small_attrition_icon") {
		auto& item = attrition_icon;
		auto& def = ui::get_element_data(state, id);
		std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, item.base_data);
		item.on_create(state);
		item.parent = this;
	}
	return nullptr;
}

void small_top_unit_icon::impl_render(sys::state& state, int32_t x, int32_t y) noexcept {
	{
		auto& item = frame;
		auto shift = child_relative_location(state, *this, item);
		item.impl_render(state, x + shift.x, y + shift.y);
	}
	{
		auto& item = flag;
		auto shift = child_relative_location(state, *this, item);
		item.impl_render(state, x + shift.x, y + shift.y);
	}
	{
		auto& item = strength_label;
		auto shift = child_relative_location(state, *this, item);
		item.impl_render(state, x + shift.x, y + shift.y);
	}
	if (has_attrition) {
		auto& item = attrition_icon;
		auto shift = child_relative_location(state, *this, item);
		item.impl_render(state, x + shift.x, y + shift.y);
	}
}

void render_button(
	sys::state& state, element_data& base_data, int frame,
	bool black_text,
	ogl::color_modification cmod,
	bool h_flip,
	int32_t x, int32_t y
) noexcept {
	dcon::gfx_object_id gid;
	if(base_data.get_element_type() == element_type::image) {
		gid = base_data.data.image.gfx_object;
	} else if(base_data.get_element_type() == element_type::button) {
		gid = base_data.data.button.button_image;
	}

	if(!gid) {
		ogl::render_tinted_rect(
			state,
			(float)x, (float)y, float(base_data.size.x), float(base_data.size.y),
			1.f, 0.f, 0.f,
			base_data.get_rotation(), false, h_flip
		);
	}
	auto const& gfx_def = state.ui_defs.gfx[gid];
	if(!gfx_def.primary_texture_handle) {
		ogl::render_tinted_rect(
			state,
			(float)x, (float)y, float(base_data.size.x), float(base_data.size.y),
			1.f, 0.f, 0.f,
			base_data.get_rotation(), gfx_def.is_vertically_flipped(), h_flip
		);
	}
	auto texture_handle = ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent());

	auto tcolor = sys::pack_color(1.f, 1.f, 1.f);
	float r = 1.f;
	float g = 1.f;
	float b = 1.f;

	bool tint = false;
	if(state.user_settings.color_blind_mode != sys::color_blind_mode::none) {
		tint = true;
		if(cmod == ogl::color_modification::interactable) {
			if(state.user_settings.color_blind_mode == sys::color_blind_mode::tritan) {
				tcolor = sys::pack_color(0.75f, 1.f, 0.75f);
			} else {
				tcolor = sys::pack_color(0.75f, 0.75f, 1.f);
			}
		} else if(cmod == ogl::color_modification::interactable_disabled) {
			tcolor = sys::pack_color(0.66f, 0.66f, 0.66f);
		} else if(cmod == ogl::color_modification::disabled) {
			tcolor = sys::pack_color(0.44f, 0.44f, 0.44f);
		}
	}

	if (tint) {
		if(gfx_def.number_of_frames > 1) {
			ogl::render_tinted_subsprite(
				state,
				frame, gfx_def.number_of_frames,
				float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				sys::red_from_int(tcolor), sys::green_from_int(tcolor), sys::blue_from_int(tcolor),
				texture_handle,
				base_data.get_rotation(), gfx_def.is_vertically_flipped(), h_flip
			);
		} else {
			ogl::render_tinted_textured_rect(
				state,
				float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				sys::red_from_int(tcolor), sys::green_from_int(tcolor), sys::blue_from_int(tcolor),
				texture_handle,
				base_data.get_rotation(), gfx_def.is_vertically_flipped(), h_flip
			);
		}
	} else {
		if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
			ogl::render_bordered_rect(
				state, cmod, gfx_def.type_dependent,
				float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				texture_handle,
				base_data.get_rotation(), gfx_def.is_vertically_flipped(), h_flip
			);
		} else if(gfx_def.number_of_frames > 1) {
			ogl::render_subsprite(
				state, cmod,
				frame, gfx_def.number_of_frames,
				float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				texture_handle,
				base_data.get_rotation(), gfx_def.is_vertically_flipped(), h_flip
			);
		} else {
			ogl::render_textured_rect(
				state, cmod,
				float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				texture_handle,
				base_data.get_rotation(), gfx_def.is_vertically_flipped(), h_flip
			);
		}			
	}
}

void map_button_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	auto cmod = get_color_modification(hover, disabled, interactable);

	render_button(
		state, base_data, frame, black_text,
		cmod,
		state.world.locale_get_native_rtl(state.font_collection.get_current_locale()), x, y
	);

	if(internal_layout.contents.empty())
		return;

	auto text_color = black_text ? ogl::color3f{ 0.0f, 0.0f, 0.0f } : ogl::color3f{ 1.0f, 1.0f, 1.0f };
	if(state.user_settings.color_blind_mode != sys::color_blind_mode::none) {
		if(cmod == ogl::color_modification::interactable_disabled) {
			text_color = black_text ? ogl::color3f{ 1.f, 1.f, 1.f } : ogl::color3f{ 0.f, 0.f, 0.f };
		} else if(cmod == ogl::color_modification::disabled) {
			text_color = black_text ? ogl::color3f{ 1.f, 1.f, 1.f } : ogl::color3f{ 0.f, 0.f, 0.f };
		}
	}

	auto linesz = state.font_collection.line_height(state, base_data.data.button.font_handle);
	if(linesz == 0.f)
		return;
	auto ycentered = (base_data.size.y - linesz) / 2;

	for(auto& t : internal_layout.contents) {
		render_text_chunk(
			state,
			t,
			float(x) + t.x,
			float(y + int32_t(ycentered)),
			base_data.data.button.font_handle,
			text_color,
			cmod
		);
	}
}


void unit_counter_window::on_update(sys::state& state) noexcept {
	state.world.for_each_province([&](auto prov){
		auto& display = display_cache[prov.index()];

		if(prov.index() < state.province_definitions.first_sea_province.index()) {
			auto armies = state.world.province_get_army_location(prov);
			populated_counters[prov.index()] = (armies.begin() == armies.end()) ? 0 : 1;
		} else {
			auto navies = state.world.province_get_navy_location(prov);
			populated_counters[prov.index()] = (navies.begin() == navies.end()) ? 0 : 1;
		}

		bool player_involved_battle = false;
		dcon::land_battle_id lbattle;
		dcon::naval_battle_id nbattle;
		if(state.world.province_get_land_battle_location(prov).begin() != state.world.province_get_land_battle_location(prov).end()) {
			auto b = *state.world.province_get_land_battle_location(prov).begin();
			auto w = b.get_battle().get_war_from_land_battle_in_war();
			if(!w) { //rebels
				player_involved_battle = true;
				lbattle = b.get_battle();
				display.player_involved_battle = true;
			} else if(military::get_role(state, w, state.local_player_nation) != military::war_role::none) { //in a war
				player_involved_battle = true;
				display.player_involved_battle = true;
				lbattle = b.get_battle();
			} else { // player is not involved
				player_involved_battle = false;
				display.player_involved_battle = false;
				lbattle = b.get_battle();
			}
		}

		// if a land battle was not found, try to find a naval battle
		if(!lbattle) {
			if(state.world.province_get_naval_battle_location(prov).begin() != state.world.province_get_naval_battle_location(prov).end()) {
				auto b = *state.world.province_get_naval_battle_location(prov).begin();
				auto w = b.get_battle().get_war_from_naval_battle_in_war();
				if(military::get_role(state, w, state.local_player_nation) != military::war_role::none) { // in a war
					player_involved_battle = true;
					nbattle = b.get_battle();
				} else { // player not involved
					player_involved_battle = false;
					nbattle = b.get_battle();
				}
			}
		}

		static std::vector<int32_t> by_icon_count;
		for(auto& i : by_icon_count)
			i = 0;

		display.top_left_status = -1;
		display.top_dig_in = -1;
		display.top_right_dig_in = -1;
		display.right_frames = 0;
		display.common_unit_1 = -1;
		display.common_unit_2 = -1;
		display.top_left_value = 0.0f;
		display.top_right_value = 0.0f;
		display.top_left_org_value = 0.0f;
		display.top_right_org_value = 0.0f;
		display.top_right_org_value = 0.0f;
		display.is_army = false;

		display.attacker_casualties = 0.0f;
		display.defender_casualties = 0.0f;
		display.player_is_attacker = false;

		bool top_right_found_enemy = false;
		bool top_right_found_ally = false;
		bool top_right_found_other = false;

		bool found_selected = false;
		bool all_selected = true;
		bool found_ally = false;
		bool found_enemy = false;
		bool found_other = false;

		auto set_flags = [&]<class T, bool Is_Top_Right>(T n) mutable {

			if constexpr(std::is_same<T, dcon::navy_fat_id>()) {
				if constexpr(Is_Top_Right) {
					auto controller = military::get_effective_unit_commander(state, n);
					if(!controller || military::are_at_war(state, controller, state.local_player_nation))
						top_right_found_enemy = true;
					else if(military::are_allied_in_war(state, controller, state.local_player_nation))
						top_right_found_ally = true;
					else
						top_right_found_other = true;
				} else {
					auto controller = military::get_effective_unit_commander(state, n);

					if(state.is_selected(n) && controller == state.local_player_nation)
						found_selected = true;
					else if(controller == state.local_player_nation)
						all_selected = false;
					else if(!controller || military::are_at_war(state, controller, state.local_player_nation))
						found_enemy = true;
					else if(military::are_allied_in_war(state, controller, state.local_player_nation))
						found_ally = true;
					else
						found_other = true;
				}
			} else {
				if constexpr(Is_Top_Right) {
					auto controller = military::get_effective_unit_commander(state, n);
					if(!controller || military::are_at_war(state, controller, state.local_player_nation))
						top_right_found_enemy = true;
					else if(military::are_allied_in_war(state, controller, state.local_player_nation))
						top_right_found_ally = true;
					else
						top_right_found_other = true;
				} else {
					auto controller = military::get_effective_unit_commander(state, n);

					if(state.is_selected(n) && controller == state.local_player_nation)
						found_selected = true;
					else if(controller == state.local_player_nation)
						all_selected = false;
					else if(!controller || military::are_at_war(state, controller, state.local_player_nation))
						found_enemy = true;
					else if(military::are_allied_in_war(state, controller, state.local_player_nation))
						found_ally = true;
					else
						found_other = true;
				};
			}
		};

		if(lbattle) {
			display.is_army = true;
			float max_str = 0.0f;
			float max_opp_str = 0.0f;
			int32_t total_count = 0;
			int32_t total_opp_count = 0;

			auto w = state.world.land_battle_get_war_from_land_battle_in_war(lbattle);
			bool player_is_attacker = w ? military::is_attacker(state, w, state.local_player_nation) : false;

			bool land_battle_attacker_is_player = false;
			if(player_is_attacker && state.world.land_battle_get_war_attacker_is_attacker(lbattle))
				land_battle_attacker_is_player = true;
			else if(!player_is_attacker && !state.world.land_battle_get_war_attacker_is_attacker(lbattle))
				land_battle_attacker_is_player = true;

			display.player_is_attacker = land_battle_attacker_is_player;

			display.top_left_status = 6;

			display.attacker_casualties = state.world.land_battle_get_attacker_casualties(lbattle);
			display.defender_casualties = state.world.land_battle_get_defender_casualties(lbattle);

			for(auto ar : state.world.land_battle_get_army_battle_participation(lbattle)) {
				auto controller = ar.get_army().get_controller_from_army_control();
				// either armies opposed to the player while the player is involved, or armies which are part of the attacking side of the war while the player is not involved
				if((!controller || (player_involved_battle && military::is_attacker(state, w, controller) != player_is_attacker)) || (!player_involved_battle && military::is_attacker(state, w, controller))) {
					++display.right_frames;
					set_flags.template operator() < dcon::army_fat_id, true > (ar.get_army());

					float str = 0.0f;
					for(auto m : state.world.army_get_army_membership(ar.get_army())) {
						++total_opp_count;

						str += m.get_regiment().get_strength();
						display.top_right_value += m.get_regiment().get_strength();
						display.top_right_org_value += m.get_regiment().get_org();
					}

					auto dig_in = ar.get_army().get_dig_in();
					if(display.top_right_dig_in == -1) {
						display.top_right_dig_in = int8_t(dig_in);
					} else {
						display.top_right_dig_in = std::min(display.top_right_dig_in, int8_t(dig_in));
					}

					if(str > max_opp_str) {
						max_opp_str = str;
						display.top_right_nation = controller;
						display.top_right_rebel = ar.get_army().get_controller_from_army_rebel_control();
					}
				} else { // same side
					float str = 0.0f;
					for(auto m : state.world.army_get_army_membership(ar.get_army())) {
						auto icon = state.military_definitions.unit_base_definitions[m.get_regiment().get_type()].icon - 1;
						set_flags.template operator() < dcon::army_fat_id, false > (ar.get_army());
						if(uint32_t(icon) >= by_icon_count.size()) {
							by_icon_count.resize(icon + 1, 0);
						}
						++(by_icon_count[icon]);
						++total_count;

						str += m.get_regiment().get_strength();
						display.top_left_value += m.get_regiment().get_strength();
						display.top_left_org_value += m.get_regiment().get_org();
					}

					auto dig_in = ar.get_army().get_dig_in();
					if(display.top_dig_in == -1) {
						display.top_dig_in = int8_t(dig_in);
					} else {
						display.top_dig_in = std::min(display.top_dig_in, int8_t(dig_in));
					}

					if(str > max_str) {
						max_str = str;
						display.top_left_nation = controller;
						display.top_left_rebel = dcon::rebel_faction_id{};
					}
				}
			}

			display.top_left_org_value /= float(total_count);
			display.top_right_org_value /= float(total_opp_count);

			float a_str = 0;
			float d_str = 0;

			for(auto reg : state.world.land_battle_get_reserves(lbattle)) {
				auto reg_str = state.world.regiment_get_strength(reg.regiment);
				if(reg_str > state.defines.alice_reg_deploy_from_reserve_str && state.world.regiment_get_org(reg.regiment) >= state.defines.alice_reg_deploy_from_reserve_org) {
					if((reg.flags & military::battle_regiment::is_attacking) != 0)
						a_str += reg_str;
					else
						d_str += reg_str;
				}
			}

			auto& att_back = state.world.land_battle_get_attacker_back_line(lbattle);
			auto& def_back = state.world.land_battle_get_defender_back_line(lbattle);
			auto& att_front = state.world.land_battle_get_attacker_front_line(lbattle);
			auto& def_front = state.world.land_battle_get_defender_front_line(lbattle);
			for(auto r : att_back) {
				if(r.regiment)
					a_str += state.world.regiment_get_strength(r.regiment);
			}
			for(auto r : att_front) {
				if(r.regiment)
					a_str += state.world.regiment_get_strength(r.regiment);
			}
			for(auto r : def_back) {
				if(r.regiment)
					d_str += state.world.regiment_get_strength(r.regiment);
			}
			for(auto r : def_front) {
				if(r.regiment)
					d_str += state.world.regiment_get_strength(r.regiment);
			}
			if(state.world.land_battle_get_war_attacker_is_attacker(lbattle) == player_is_attacker) {
				display.battle_progress = a_str / (a_str + d_str);
			} else {
				display.battle_progress = d_str / (a_str + d_str);
			}

			active_battles[prov.index()] = 1;
		} else if(nbattle) {
			float max_str = 0.0f;
			float max_opp_str = 0.0f;
			int32_t total_count = 0;
			int32_t total_opp_count = 0;

			auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(nbattle);
			bool player_is_attacker = w ? military::is_attacker(state, w, state.local_player_nation) : false;

			display.top_left_status = 6;

			for(auto ar : state.world.naval_battle_get_navy_battle_participation(nbattle)) {
				auto controller = ar.get_navy().get_controller_from_navy_control();
				// either navies opposed to the player while the player is involved, or navies which are part of the attacking side of the war while the player is not involved
				if((!controller || (player_involved_battle && military::is_attacker(state, w, controller) != player_is_attacker)) || (!player_involved_battle && military::is_attacker(state, w, controller))) {
					set_flags.template operator() < dcon::navy_fat_id, true > (ar.get_navy());


					++display.right_frames;

					float str = 0.0f;
					for(auto m : state.world.navy_get_navy_membership(ar.get_navy())) {
						++total_opp_count;

						str += m.get_ship().get_strength();
						display.top_right_value += 1.0f;
						display.top_right_org_value += m.get_ship().get_org();
					}

					if(str > max_opp_str) {
						max_opp_str = str;
						display.top_right_nation = controller;
						display.top_right_rebel = dcon::rebel_faction_id{};
					}
				} else { // same side
					float str = 0.0f;
					for(auto m : state.world.navy_get_navy_membership(ar.get_navy())) {
						set_flags.template operator() < dcon::navy_fat_id, false > (ar.get_navy());
						auto icon = state.military_definitions.unit_base_definitions[m.get_ship().get_type()].icon - 1;
						if(uint32_t(icon) >= by_icon_count.size()) {
							by_icon_count.resize(icon + 1, 0);
						}
						++(by_icon_count[icon]);
						++total_count;

						str += m.get_ship().get_strength();
						display.top_left_value += 1.0f;
						display.top_left_org_value += m.get_ship().get_org();
					}

					if(str > max_str) {
						max_str = str;
						display.top_left_nation = controller;
						display.top_left_rebel = dcon::rebel_faction_id{};
					}
				}
			}

			display.top_left_org_value /= float(total_count);
			display.top_right_org_value /= float(total_opp_count);

			float attacker_combat_score = 0;
			float defender_combat_score = 0;

			auto slots = state.world.naval_battle_get_slots(nbattle);

			for(uint32_t j = slots.size(); j-- > 0;) {
				switch(slots[j].flags & military::ship_in_battle::mode_mask) {
				case military::ship_in_battle::mode_seeking:
				case military::ship_in_battle::mode_approaching:
				case military::ship_in_battle::mode_retreating:
				case military::ship_in_battle::mode_engaged:
					if((slots[j].flags & military::ship_in_battle::is_attacking) != 0)
						attacker_combat_score += military::get_ship_combat_score(state, slots[j].ship);
					else
						defender_combat_score += military::get_ship_combat_score(state, slots[j].ship);
					break;
				default:
					break;
				}
			}

			if(state.world.naval_battle_get_war_attacker_is_attacker(nbattle) == player_is_attacker) {
				display.battle_progress = attacker_combat_score / (attacker_combat_score + defender_combat_score);
			} else {
				display.battle_progress = defender_combat_score / (attacker_combat_score + defender_combat_score);
			}

			active_battles[prov.index()] = 1;
		}
		// no battle
		else {
			// if sea province
			if(prov.index() < state.province_definitions.first_sea_province.index()) {
				for(auto a : state.world.province_get_army_location(prov)) {
					if(!(a.get_army().get_navy_from_army_transport())) {

						set_flags.template operator() < dcon::army_fat_id, false > (a.get_army());
						display.top_army_id = a.get_army();
					}
				}
			} else { // if land province
				auto navies = state.world.province_get_navy_location(prov);
				if(navies.begin() == navies.end()) {
					return;
				} else {
					for(auto n : navies) {
						set_flags.template operator() < dcon::navy_fat_id, false > (n.get_navy());
					}
				}
			}

			active_battles[prov.index()] = 0;
		}


		display.colors_used = 0;

		if(found_selected) {
			display.colors[display.colors_used] = outline_color::gold;
			++display.colors_used;
		}
		if(!all_selected) {
			display.colors[display.colors_used] = outline_color::blue;
			++display.colors_used;
		}
		if(!player_involved_battle) {
			if(found_enemy) {
				display.colors[display.colors_used] = outline_color::red;
				++display.colors_used;
			}
		}
		if(found_ally) {
			display.colors[display.colors_used] = outline_color::cyan;
			++display.colors_used;
		}
		if(found_other) {
			display.colors[display.colors_used] = outline_color::gray;
			++display.colors_used;
		}



		if(top_right_found_enemy) {
			display.top_right_color = outline_color::red;
		}

		if(top_right_found_ally) {
			display.top_right_color = outline_color::cyan;
		}
		if(top_right_found_other) {
			display.top_right_color = outline_color::gray;
		}



		if(!nbattle && !lbattle) {
			if(prov.index() < state.province_definitions.first_sea_province.index()) {
				std::function<bool(dcon::army_id)> filter;

				if(display.colors[0] == outline_color::gold) {
					//display.top_left_nation = state.local_player_nation;
					display.top_left_rebel = dcon::rebel_faction_id{};
					filter = [&](dcon::army_id a) { return military::get_effective_unit_commander(state, a) == state.local_player_nation && state.is_selected(a); };
				} else if(display.colors[0] == outline_color::blue) {
					//display.top_left_nation = state.local_player_nation;
					display.top_left_rebel = dcon::rebel_faction_id{};
					filter = [&](dcon::army_id a) { return military::get_effective_unit_commander(state, a) == state.local_player_nation && !state.is_selected(a); };
				} else if(display.colors[0] == outline_color::cyan) {
					filter = [&](dcon::army_id a) {
						auto n = state.world.army_get_controller_from_army_control(a);
						return n != state.local_player_nation && military::are_allied_in_war(state, n, state.local_player_nation);
						};
				} else if(display.colors[0] == outline_color::red) {
					filter = [&](dcon::army_id a) {
						auto n = state.world.army_get_controller_from_army_control(a);
						return !n || military::are_at_war(state, n, state.local_player_nation);
						};
				} else if(display.colors[0] == outline_color::gray) {
					filter = [&](dcon::army_id a) {
						auto n = state.world.army_get_controller_from_army_control(a);
						return n != state.local_player_nation && !military::are_allied_in_war(state, n, state.local_player_nation) && !military::are_at_war(state, n, state.local_player_nation);
						};
				}

				float max_str = 0.0f;

				int32_t total_count = 0;
				int32_t ucount = 0;

				for(auto a : state.world.province_get_army_location(prov)) {
					if(!(a.get_army().get_navy_from_army_transport()) && filter(a.get_army())) {
						++ucount;
						float str = 0.0f;
						for(auto m : state.world.army_get_army_membership(a.get_army())) {
							auto icon = state.military_definitions.unit_base_definitions[m.get_regiment().get_type()].icon - 1;
							icon = std::max(icon, 0);
							if(uint32_t(icon) >= by_icon_count.size()) {
								by_icon_count.resize(icon + 1, 0);
							}
							++(by_icon_count[icon]);
							++total_count;

							str += m.get_regiment().get_strength();
							display.is_army = true;
							display.top_left_value += m.get_regiment().get_strength();
							display.top_left_org_value += m.get_regiment().get_org();
						}

						auto frame = status_frame(state, a.get_army());
						if(display.top_left_status == -1) {
							display.top_left_status = int8_t(frame);
						} else if(display.top_left_status == int8_t(frame)) {

						} else {
							display.top_left_status = 0;
						}

						auto dig_in = a.get_army().get_dig_in();
						if(display.top_dig_in == -1) {
							display.top_dig_in = int8_t(dig_in);
						} else {
							display.top_dig_in = std::min(display.top_dig_in, int8_t(dig_in));
						}

						if(str > max_str) {
							max_str = str;
							display.top_left_nation = state.world.army_get_controller_from_army_control(a.get_army());
							display.top_left_rebel = state.world.army_get_controller_from_army_rebel_control(a.get_army());
						}
					}
				}

				if(ucount > 1 && display.colors_used == 1) {
					display.colors_used = 2;
					display.colors[1] = display.colors[0];
				}

				display.top_left_org_value /= float(total_count);

				battle.set_visible(state, false);
				active_siege[prov.index()] = state.world.province_get_siege_progress(prov) > 0.f;
				top_icon.base_data.position.x = -30;
				top_right_icon.set_visible(state, false);
				small_top_icon.base_data.position.x = -30;
				small_top_right_icon.set_visible(state, false);
			} else {
				std::function<bool(dcon::navy_id)> filter;
				display.top_left_rebel = dcon::rebel_faction_id{};

				if(display.colors[0] == outline_color::gold) {
					//display.top_left_nation = state.local_player_nation;
					filter = [&](dcon::navy_id a) { return military::get_effective_unit_commander(state, a) == state.local_player_nation && state.is_selected(a); };
				} else if(display.colors[0] == outline_color::blue) {
					//display.top_left_nation = state.local_player_nation;
					filter = [&](dcon::navy_id a) { return military::get_effective_unit_commander(state, a) == state.local_player_nation && !state.is_selected(a); };
				} else if(display.colors[0] == outline_color::cyan) {
					filter = [&](dcon::navy_id a) {
						auto n = state.world.navy_get_controller_from_navy_control(a);
						return n != state.local_player_nation && military::are_allied_in_war(state, n, state.local_player_nation);
						};
				} else if(display.colors[0] == outline_color::red) {
					filter = [&](dcon::navy_id a) {
						auto n = state.world.navy_get_controller_from_navy_control(a);
						return military::are_at_war(state, n, state.local_player_nation);
						};
				} else if(display.colors[0] == outline_color::gray) {
					filter = [&](dcon::navy_id a) {
						auto n = state.world.navy_get_controller_from_navy_control(a);
						return n != state.local_player_nation && !military::are_allied_in_war(state, n, state.local_player_nation) && !military::are_at_war(state, n, state.local_player_nation);
						};
				}

				float max_str = 0.0f;
				int32_t total_count = 0;
				int32_t ucount = 0;

				for(auto a : state.world.province_get_navy_location(prov)) {
					if(filter(a.get_navy())) {
						++ucount;
						float str = 0.0f;
						for(auto m : state.world.navy_get_navy_membership(a.get_navy())) {
							auto icon = state.military_definitions.unit_base_definitions[m.get_ship().get_type()].icon - 1;
							if(uint32_t(icon) >= by_icon_count.size()) {
								by_icon_count.resize(icon + 1, 0);
							}
							++(by_icon_count[icon]);
							++total_count;

							str += m.get_ship().get_strength();
							display.top_left_value += 1.0f;
							display.top_left_org_value += m.get_ship().get_org();
						}

						auto frame = status_frame(state, a.get_navy());
						if(display.top_left_status == -1) {
							display.top_left_status = int8_t(frame);
						} else if(display.top_left_status == int8_t(frame)) {

						} else {
							display.top_left_status = 0;
						}

						if(str > max_str) {
							max_str = str;
							display.top_left_nation = state.world.navy_get_controller_from_navy_control(a.get_navy());
						}
					}
				}

				if(ucount > 1 && display.colors_used == 1) {
					display.colors_used = 2;
					display.colors[1] = display.colors[0];
				}

				display.top_left_org_value /= float(total_count);

				battle.set_visible(state, false);
				active_siege[prov.index()] = state.world.province_get_siege_progress(prov) > 0.f;
				top_icon.base_data.position.x = -30;
				top_right_icon.set_visible(state, false);
				small_top_icon.base_data.position.x = -30;
				small_top_right_icon.set_visible(state, false);
			}
		}

		if(display.top_left_nation == state.local_player_nation) {
			int32_t max_index = 0;
			for(uint32_t i = 1; i < by_icon_count.size(); ++i) {
				if(by_icon_count[i] > by_icon_count[max_index])
					max_index = int32_t(i);
			}
			display.common_unit_1 = int8_t(max_index);

			if(by_icon_count.size() > 0) {
				by_icon_count[max_index] = 0;
				max_index = 0;
				for(uint32_t i = 1; i < by_icon_count.size(); ++i) {
					if(by_icon_count[i] > by_icon_count[max_index])
						max_index = int32_t(i);
				}
				if(by_icon_count[max_index] > 0)
					display.common_unit_2 = int8_t(max_index);
			}
		}

		has_attrition[prov.index()] = false;
		if(prov.index() >= state.province_definitions.first_sea_province.index()) {
			for(auto navy : state.world.province_get_navy_location(prov)) {
				if(military::will_recieve_attrition(state, navy.get_navy())) {
					has_attrition[prov.index()] = true;
					break;
				}
			}
		}
		else {
			for(auto army : state.world.province_get_army_location(prov)) {
				if(military::will_recieve_attrition(state, army.get_army())) {
					has_attrition[prov.index()] = true;
					break;
				}
			}
		}

	});

	auto rtl = state.world.locale_get_native_rtl(state.font_collection.get_current_locale())
		? text::layout_base::rtl_status::rtl
		: text::layout_base::rtl_status::ltr;

	state.world.for_each_province([&](auto prov) {
		auto& params =  display_cache[prov.index()];
		auto& layout = strength_layout_cache[prov.index()];
		auto& layout_small = strength_layout_small_cache[prov.index()];
		auto strength = params.top_left_value;
		if(params.is_army) {
			strength *= state.defines.pop_size_per_regiment;
			strength = floor(strength);
		}

		auto& label = top_icon.strength_label;
		auto& label_small = small_top_icon.strength_label;

		if(strength != strength_cache[prov.index()]) {
			strength_cache[prov.index()] = strength;
			auto pretty = text::prettify(int32_t(strength));

			layout.contents.clear();
			layout_small.contents.clear();
			layout.number_of_lines = 0;
			layout_small.number_of_lines = 0;

			{
				auto& bd = label.base_data;
				auto al = text::to_text_alignment(bd.data.text.get_alignment());
				text::single_line_layout sl{
					layout, text::layout_parameters{
						0, 0,
						static_cast<int16_t>(bd.size.x - bd.data.text.border_size.x * 2),
						static_cast<int16_t>(bd.size.y),
						bd.data.text.font_handle, 0, al, text::text_color::white, true, true
					},
					rtl
				};
				sl.add_text(state, pretty);					
			}

			{
				auto& bd = label_small.base_data;
				auto al = text::to_text_alignment(bd.data.text.get_alignment());
				text::single_line_layout sl{
					layout_small, text::layout_parameters{
						0, 0,
						static_cast<int16_t>(bd.size.x - bd.data.text.border_size.x * 2),
						static_cast<int16_t>(bd.size.y),
						bd.data.text.font_handle, 0, al, text::text_color::white, true, true
					},
					rtl
				};
				sl.add_text(state, pretty);					
			}
		}
	});

	state.world.for_each_province([&](auto prov) {
		auto& params =  display_cache[prov.index()];
		auto& layout = strength_right_layout_cache[prov.index()];
		auto& layout_small = strength_right_layout_small_cache[prov.index()];
		auto strength = params.top_right_value;
		if(params.is_army) {
			strength *= state.defines.pop_size_per_regiment;
			strength = floor(strength);
		}

		auto& label = top_right_icon.strength_label;
		auto& label_small = small_top_right_icon.strength_label;

		if(strength != strength_right_cache[prov.index()]) {
			strength_right_cache[prov.index()] = strength;
			auto pretty = text::prettify(int32_t(strength));

			layout.contents.clear();
			layout_small.contents.clear();
			layout.number_of_lines = 0;
			layout_small.number_of_lines = 0;

			{
				auto& bd = label.base_data;
				auto al = text::to_text_alignment(bd.data.text.get_alignment());
				text::single_line_layout sl{
					layout, text::layout_parameters{
						0, 0,
						static_cast<int16_t>(bd.size.x - bd.data.text.border_size.x * 2),
						static_cast<int16_t>(bd.size.y),
						bd.data.text.font_handle, 0, al, text::text_color::white, true, true
					},
					rtl
				};
				sl.add_text(state, pretty);					
			}

			{
				auto& bd = label_small.base_data;
				auto al = text::to_text_alignment(bd.data.text.get_alignment());
				text::single_line_layout sl{
					layout_small, text::layout_parameters{
						0, 0,
						static_cast<int16_t>(bd.size.x - bd.data.text.border_size.x * 2),
						static_cast<int16_t>(bd.size.y),
						bd.data.text.font_handle, 0, al, text::text_color::white, true, true
					},
					rtl
				};
				sl.add_text(state, pretty);					
			}
		}
	});

	// casualties
	state.world.for_each_province([&](auto prov) {
		auto& params =  display_cache[prov.index()];
		{			
			auto& label = battle.attacker_casualties;
			auto& layout = casualties_layout_cache[prov.index()];
			auto loss = params.attacker_casualties * state.defines.pop_size_per_regiment;
			auto& cached = casualties_cache[prov.index()];

			if(loss != cached) {
				cached = loss;
				auto pretty = text::prettify(int32_t(loss));

				layout.contents.clear();
				layout.number_of_lines = 0;

				auto& bd = label.base_data;
				auto al = text::to_text_alignment(bd.data.text.get_alignment());
				text::single_line_layout sl{
					layout, text::layout_parameters{
						0, 0,
						static_cast<int16_t>(bd.size.x - bd.data.text.border_size.x * 2),
						static_cast<int16_t>(bd.size.y),
						bd.data.text.font_handle, 0, al, text::text_color::red, true, true
					},
					rtl
				};
				sl.add_text(state, pretty);
			}
		}
		{
			auto& label = battle.defender_casualties;
			auto& layout = casualties_right_layout_cache[prov.index()];
			auto loss = params.defender_casualties * state.defines.pop_size_per_regiment;
			auto& cached = casualties_right_cache[prov.index()];

			if(loss != cached) {
				cached = loss;
				auto pretty = text::prettify(int32_t(loss));

				layout.contents.clear();
				layout.number_of_lines = 0;

				auto& bd = label.base_data;
				auto al = text::to_text_alignment(bd.data.text.get_alignment());
				text::single_line_layout sl{
					layout, text::layout_parameters{
						0, 0,
						static_cast<int16_t>(bd.size.x - bd.data.text.border_size.x * 2),
						static_cast<int16_t>(bd.size.y),
						bd.data.text.font_handle, 0, al, text::text_color::red, true, true
					},
					rtl
				};
				sl.add_text(state, pretty);
			}
		}
	});

	// update flags here for efficiency: no need to send payloads
	state.world.for_each_province([&](auto prov) {
		auto& params = display_cache[prov.index()];
		auto rebel_faction = params.top_left_rebel;
		auto nation = params.top_left_nation;
		if(nation) {
			flag_texture_handles[prov.index()] = ogl::get_flag_handle(state, state.world.nation_get_identity_from_identity_holder(nation), culture::get_current_flag_type(state, nation));
			return;
		}
		if(rebel_faction) {
			flag_texture_handles[prov.index()] = ogl::get_rebel_flag_handle(state, rebel_faction);
			return;
		}
		auto reb_tag = state.national_definitions.rebel_id;
		flag_texture_handles[prov.index()] = ogl::get_flag_handle(state, reb_tag, { });
	});

	state.world.for_each_province([&](auto prov) {
		auto& params = display_cache[prov.index()];
		auto rebel_faction = params.top_right_rebel;
		auto nation = params.top_right_nation;
		if(nation) {
			flag_right_texture_handles[prov.index()] = ogl::get_flag_handle(state, state.world.nation_get_identity_from_identity_holder(nation), culture::get_current_flag_type(state, nation));
			return;
		}
		if(rebel_faction) {
			flag_right_texture_handles[prov.index()] = ogl::get_rebel_flag_handle(state, rebel_faction);
			return;
		}
		auto reb_tag = state.national_definitions.rebel_id;
		flag_right_texture_handles[prov.index()] = ogl::get_flag_handle(state, reb_tag, { });
	});
}

void unit_counter_window::toogle_grid (sys::state& state, dcon::province_id target, bool with_shift) {
	if(state.ui_state.unit_details_box->for_province == target && state.ui_state.unit_details_box->is_visible() == true) {
		state.ui_state.unit_details_box->set_visible(state, false);
		if(!with_shift) {
			state.selected_armies.clear();
			state.selected_navies.clear();
		} else {
			if(target.index() >= state.province_definitions.first_sea_province.index()) {
				for(auto n : state.world.province_get_navy_location(target)) {
					state.deselect(n.get_navy().id);
				}
			} else {
				for(auto n : state.world.province_get_army_location(target)) {
					state.deselect(n.get_army().id);
				}
			}
		}
	} else {
		if(!with_shift) {
			state.selected_armies.clear();
			state.selected_navies.clear();
		}

		if(target.index() >= state.province_definitions.first_sea_province.index()) {
			for(auto n : state.world.province_get_navy_location(target)) {
				// if(state.world.navy_get_controller_from_navy_control(n.get_navy()) == state.local_player_nation) {
					state.select(n.get_navy().id);
				//  }
			}
		} else {
			for(auto n : state.world.province_get_army_location(target)) {
				if(!(n.get_army().get_navy_from_army_transport()) /* && n.get_army().get_controller_from_army_control() == state.local_player_nation */) {
					state.select(n.get_army().id);
				}
			}
		}

		auto pos = last_screen_coordinates[target.index()];
		auto location = xy_pair { int16_t(pos.x), int16_t(pos.y) };
		if(state.map_state.get_zoom() >= big_counter_cutoff) {
			int32_t height = 60;
			int32_t left = -30;
			int32_t right = 31;
			if(active_siege[target.index()]) {
				height = 80;
				right = 48;
				left = -49;
			} else if(active_battles[target.index()]) {
				height = 80;
				right = 62;
				left = -62;
			}
			location.x += int16_t(left);
			location.y -= 30;
			state.ui_state.unit_details_box->open(state, location, ui::xy_pair{int16_t(right - left), int16_t(height)}, target, false);
		} else {
			if(active_siege[target.index()]) {
				location.x -= 49;
				location.y -= 10;
				state.ui_state.unit_details_box->open(state, location, ui::xy_pair{int16_t(97), int16_t(39)}, target, false);
			} else if(active_battles[target.index()]) {
				location.x -= 62;
				location.y -= 10;
				state.ui_state.unit_details_box->open(state, location, ui::xy_pair{int16_t(124), int16_t(39)}, target, false);
			} else {
				location.x -= 30;
				location.y -= 10;
				state.ui_state.unit_details_box->open(state, location, ui::xy_pair{int16_t(60), int16_t(19)}, target, false);
			}
		}
	}
}

void unit_counter_window::button_action(sys::state& state) noexcept {
	if (hovered_part == unit_counter_part::frame || hovered_part == unit_counter_part::flag) {
		toogle_grid(state, hovered_icon, false);
	} else if (hovered_part == unit_counter_part::battle) {
		auto prov = hovered_icon;
		if(state.world.province_get_land_battle_location(prov).begin() != state.world.province_get_land_battle_location(prov).end()) {
			auto b = *state.world.province_get_land_battle_location(prov).begin();

			auto w = b.get_battle().get_war_from_land_battle_in_war();
			land_combat_window* win = static_cast<land_combat_window*>(state.ui_state.army_combat_window);
			win->battle = b.get_battle();
			if(state.ui_state.army_combat_window->is_visible()) {
				state.ui_state.army_combat_window->impl_on_update(state);
			} else {
				state.ui_state.army_combat_window->set_visible(state, true);
				if(state.ui_state.province_window) {
					state.ui_state.province_window->set_visible(state, false);
					state.set_selected_province(dcon::province_id{});
				}
				if(state.ui_state.naval_combat_window) {
					state.ui_state.naval_combat_window->set_visible(state, false);
				}
			}
		}		
		if(state.world.province_get_naval_battle_location(prov).begin() != state.world.province_get_naval_battle_location(prov).end()) {
			auto b = *state.world.province_get_naval_battle_location(prov).begin();
			auto w = b.get_battle().get_war_from_naval_battle_in_war();

			naval_combat_window* win = static_cast<naval_combat_window*>(state.ui_state.naval_combat_window);
			win->battle = b.get_battle();
			if(state.ui_state.naval_combat_window->is_visible()) {
				state.ui_state.naval_combat_window->impl_on_update(state);
			} else {
				state.ui_state.naval_combat_window->set_visible(state, true);
				if(state.ui_state.province_window) {
					state.ui_state.province_window->set_visible(state, false);
					state.set_selected_province(dcon::province_id{});
				}
				if(state.ui_state.army_combat_window) {
					state.ui_state.army_combat_window->set_visible(state, false);
				}
			}
		}
	}
}
void unit_counter_window::button_shift_action(sys::state& state) noexcept {
	if (hovered_part == unit_counter_part::frame || hovered_part == unit_counter_part::flag) {
		toogle_grid(state, hovered_icon, true);
	}
}
void unit_counter_window::button_right_action(sys::state& state) noexcept {
	if (hovered_part == unit_counter_part::flag) {
		
	}
}
tooltip_behavior unit_counter_window::has_tooltip(sys::state& state) noexcept {
	return tooltip_behavior::variable_tooltip;
}
void unit_counter_window::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	populate_unit_tooltip(state, contents, hovered_icon);
}


message_result unit_counter_window::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(!state.user_settings.left_mouse_click_hold_and_release) {
		sound::play_interface_sound(state, get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		if(mods == sys::key_modifiers::modifiers_shift)
			button_shift_action(state);
		else if(mods == sys::key_modifiers::modifiers_ctrl)
			button_ctrl_action(state);
		else
			button_action(state);
	}
	return message_result::consumed;
}
message_result unit_counter_window::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	sound::play_interface_sound(state, get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
	if(mods == sys::key_modifiers::modifiers_shift)
		button_shift_right_action(state);
	else if(mods == sys::key_modifiers::modifiers_ctrl)
		button_ctrl_right_action(state);
	else
		button_right_action(state);
	return message_result::consumed;
}
message_result unit_counter_window::on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods, bool under_mouse) noexcept {
	if(state.user_settings.left_mouse_click_hold_and_release && under_mouse) {
		sound::play_interface_sound(state, get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		if(mods == sys::key_modifiers::modifiers_shift)
			button_shift_action(state);
		else if(mods == sys::key_modifiers::modifiers_ctrl)
			button_ctrl_action(state);
		else
			button_action(state);
	}
	return message_result::consumed;
}
message_result unit_counter_window::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	if(base_data.get_element_type() == element_type::button && base_data.data.button.shortcut == key) {
		sound::play_interface_sound(state, get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		if(mods == sys::key_modifiers::modifiers_shift)
			button_shift_action(state);
		else if(mods == sys::key_modifiers::modifiers_ctrl)
			button_ctrl_action(state);
		else
			button_action(state);
		return message_result::consumed;
	} else {
		return message_result::unseen;
	}
}

}
