#include "gui_element_types.hpp"
#include "gui_context_window.hpp"
#include "gui_common_elements.hpp"

namespace ui {

class context_menu_entry_logic {
public:
	virtual dcon::text_key get_name(sys::state& state, context_menu_context context) {
		return state.lookup_key("null");
	}

	virtual bool is_available(sys::state& state, context_menu_context context) {
		return false;
	}

	virtual void button_action(sys::state& state, context_menu_context context, ui::element_base* parent) {

	}

	virtual void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, context_menu_context context) {

	}
	virtual ~context_menu_entry_logic() { };
};

class context_menu_build_factory : public context_menu_entry_logic {
public:
	dcon::text_key get_name(sys::state& state, context_menu_context context) noexcept override {
		return state.lookup_key("build_factory");
	}

	bool is_available(sys::state& state, context_menu_context context) noexcept override  {
		auto pid = context.province;
		auto sid = state.world.province_get_state_membership(pid);
		auto n = state.world.province_get_nation_from_province_ownership(pid);

		bool can_build = false;
		state.world.for_each_factory_type([&](dcon::factory_type_id ftid) {
			can_build =
				can_build || command::can_begin_factory_building_construction(state, state.local_player_nation, pid, ftid, false);
		});
		return can_build;
	}

	void button_action(sys::state& state, context_menu_context context, ui::element_base* parent) noexcept override {
		open_build_factory(state, context.province);
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, context_menu_context context) noexcept override  {
		text::add_line(state, contents, "build_factory");

		auto pid = context.province;
		auto sid = state.world.province_get_state_membership(pid);
		auto n = state.world.province_get_nation_from_province_ownership(pid);

		int32_t num_factories = economy::state_factory_count(state, sid, n);

		text::add_line(state, contents, "production_build_new_factory_tooltip");
		text::add_line_break_to_layout(state, contents);
		text::add_line_with_condition(state, contents, "factory_condition_1", state.world.nation_get_is_civilized(n));
		// Disallow building in colonies unless define flag is set
		if(state.defines.alice_allow_factories_in_colonies == 0.f) {
			text::add_line_with_condition(state, contents, "factory_condition_2", economy::can_build_factory_in_colony(state, sid));
		}

		if(n == state.local_player_nation) {
			auto rules = state.world.nation_get_combined_issue_rules(n);
			text::add_line_with_condition(state, contents, "factory_condition_3", (rules & issue_rule::build_factory) != 0);
		} else {
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_4", state.world.nation_get_is_great_power(state.local_player_nation) && !state.world.nation_get_is_great_power(n));

			text::add_line_with_condition(state, contents, "factory_upgrade_condition_5", state.world.nation_get_is_civilized(n));

			auto target = state.world.nation_get_combined_issue_rules(n);
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_6",
					(target & issue_rule::allow_foreign_investment) != 0);

			text::add_line_with_condition(state, contents, "factory_upgrade_condition_7", !military::are_at_war(state, state.local_player_nation, n));
		}

		{
			auto box = text::open_layout_box(contents);
			auto r = num_factories < int32_t(state.defines.factories_per_state);
			if(r) {
				text::add_to_layout_box(state, contents, box, text::embedded_icon::check);
			} else {
				text::add_to_layout_box(state, contents, box, text::embedded_icon::xmark);
			}
			text::add_space_to_layout_box(state, contents, box);
			text::localised_single_sub_box(state, contents, box, "factory_condition_4", text::variable_type::val, int64_t(state.defines.factories_per_state));
			text::close_layout_box(contents, box);
		}
	}
};

class context_menu_upgrade_factory : public context_menu_entry_logic {
public:
	dcon::text_key get_name(sys::state& state, context_menu_context context) noexcept override {
		return state.lookup_key("upgrade_factory");
	}

	bool is_available(sys::state& state, context_menu_context context) noexcept override {
		auto fid = context.factory;
		auto fat = dcon::fatten(state.world, fid);
		auto pid = fat.get_province_from_factory_location();
		auto sid = pid.get_state_membership();
		auto type = fat.get_building_type();

		return command::can_begin_factory_building_construction(state, state.local_player_nation, pid, fat.get_building_type().id, true);
	}

	void button_action(sys::state& state, context_menu_context context, ui::element_base* parent) noexcept override {
		auto fid = context.factory;
		auto fat = dcon::fatten(state.world, fid);
		auto pid = fat.get_province_from_factory_location();
		auto sid = pid.get_state_membership();
		command::begin_factory_building_construction(state, state.local_player_nation, pid, fat.get_building_type().id, true);
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, context_menu_context context) noexcept override {
		auto fid = context.factory;
		auto fat = dcon::fatten(state.world, fid);
		auto pid = fat.get_province_from_factory_location();
		auto sid = pid.get_state_membership();
		const dcon::nation_id n = fat.get_province_from_factory_location().get_nation_from_province_ownership();
		auto type = state.world.factory_get_building_type(fid);

		// no double upgrade
		bool is_not_upgrading = true;
		for(auto p : state.world.province_get_factory_construction(pid)) {
			if(p.get_type() == type)
				is_not_upgrading = false;
		}
		if(!is_not_upgrading) {
			return;
		}

		text::add_line(state, contents, "production_expand_factory_tooltip");

		text::add_line_break_to_layout(state, contents);

		bool is_civ = state.world.nation_get_is_civilized(state.local_player_nation);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_1", is_civ);

		bool state_is_not_colonial = !state.world.province_get_is_colonial(state.world.state_instance_get_capital(sid));
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_2", state_is_not_colonial);

		bool is_activated = state.world.nation_get_active_building(n, type) == true || state.world.factory_type_get_is_available_from_start(type);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_3", is_activated);
		if(n != state.local_player_nation) {
			bool gp_condition = (state.world.nation_get_is_great_power(state.local_player_nation) == true &&
					state.world.nation_get_is_great_power(n) == false);
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_4", gp_condition);

			text::add_line_with_condition(state, contents, "factory_upgrade_condition_5", state.world.nation_get_is_civilized(n));

			auto rules = state.world.nation_get_combined_issue_rules(n);
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_6",
				(rules & issue_rule::allow_foreign_investment) != 0);

			text::add_line_with_condition(state, contents, "factory_upgrade_condition_7",
				!military::are_at_war(state, state.local_player_nation, n));
		} else {
			auto rules = state.world.nation_get_combined_issue_rules(state.local_player_nation);
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_8", (rules & issue_rule::expand_factory) != 0);
		}
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_9", is_not_upgrading);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_10", fat.get_size() < 255);

		auto output = state.world.factory_type_get_output(type);
		if(state.world.commodity_get_uses_potentials(output)) {
			auto limit = economy::calculate_state_factory_limit(state, fat.get_factory_location().get_province().get_state_membership(), output);

			// Will upgrade put us over the limit?
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_11", fat.get_size() + 1 <= limit);
		}

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "factory_upgrade_shortcuts");
	}
};

class context_menu_delete_factory : public context_menu_entry_logic {
public:
	dcon::text_key get_name(sys::state& state, context_menu_context context) noexcept override {
		if (economy::factory_total_employment_score(state, context.factory) >= economy::factory_closed_threshold)
			return state.lookup_key("close_and_del");
		return state.lookup_key("delete_factory");
	}

	bool is_available(sys::state& state, context_menu_context context) noexcept override {
		auto fid = context.factory;
		return command::can_delete_factory(state, state.local_player_nation, fid);
	}

	void button_action(sys::state& state, context_menu_context context, ui::element_base* parent) noexcept override {
		auto fid = context.factory;
		command::delete_factory(state, state.local_player_nation, fid);
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, context_menu_context context) noexcept override {
		auto owner = state.world.province_get_nation_from_province_ownership(context.province);
		if(owner == state.local_player_nation) {
			text::add_line(state, contents, "factory_delete_header");
			if(!command::can_delete_factory(state, state.local_player_nation, context.factory)) {
				text::add_line_break_to_layout(state, contents);
				text::add_line(state, contents, "factory_delete_not_allowed");
			}
		}
	}
};

class context_menu_cancel_factory_construction : public context_menu_entry_logic {
public:
	dcon::text_key get_name(sys::state& state, context_menu_context context) noexcept override {
		return state.lookup_key("cancel_fac_construction");
	}

	bool is_available(sys::state& state, context_menu_context context) noexcept override {
		auto fid = context.fconstruction;
		auto type = state.world.factory_construction_get_type(fid);
		auto prov = state.world.factory_construction_get_province(fid);
		return command::can_cancel_factory_building_construction(state, state.local_player_nation, prov, type);
	}

	void button_action(sys::state& state, context_menu_context context, ui::element_base* parent) noexcept override {
		auto fid = context.fconstruction;
		auto type = state.world.factory_construction_get_type(fid);
		auto prov = state.world.factory_construction_get_province(fid);
		command::cancel_factory_building_construction(state, state.local_player_nation, prov, type);
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, context_menu_context context) noexcept override {
		text::add_line(state, contents, "cancel_fac_construction");
	}
};

inline static context_menu_delete_factory context_menu_delete_factory_logic;
inline static context_menu_upgrade_factory context_menu_upgrade_factory_logic;
inline static context_menu_build_factory context_menu_build_factory_logic;
inline static context_menu_cancel_factory_construction context_menu_cancel_factory_construction_logic;

class context_window_entry : public button_element_base {
public:
	int ind = 0;
	context_menu_entry_logic* entry_logic;
	context_menu_context context;

	void on_update(sys::state& state) noexcept override {
		if(entry_logic) {
			this->set_button_text(state, text::produce_simple_string(state, entry_logic->get_name(state, context)));
			disabled = !entry_logic->is_available(state, context);
		}
	}
	void button_action(sys::state& state) noexcept override {
		if (entry_logic)
			entry_logic->button_action(state, context, parent);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(entry_logic)
			entry_logic->update_tooltip(state, x, y, contents, context);
	}
};

class context_menu_window : public window_element_base {
public:
	std::vector<context_window_entry*> entries = std::vector<context_window_entry*>(16);
	std::vector<context_menu_entry_logic*> logics = std::vector<context_menu_entry_logic*>(16);
	context_menu_context context;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		for(size_t i = 0; i < entries.size(); i++) {
			auto ptr = make_element_by_type<context_window_entry>(state, "context_menu_item");
			ptr->base_data.position.y += int16_t((ptr->base_data.size.y) * i);
			ptr->set_visible(state, false);

			entries[i] = ptr.get();
			entries[i]->ind = (int)i;

			add_child_to_front(std::move(ptr));
		}
	}

	void on_update(sys::state& state) noexcept override {
		window_element_base::on_update(state);

		for(size_t i = 0; i < entries.size(); i++) {
			if(logics[i]) {
				entries[i]->set_visible(state, true);
				entries[i]->entry_logic = logics[i];
				entries[i]->context = context;
			} else {
				entries[i]->set_visible(state, false);
			}
		}
	}
};

void hide_context_menu(sys::state& state) {
	
	if(!state.ui_state.context_menu) {
		return;
	}
		
	state.ui_state.context_menu->set_visible(state, false);
}

void show_context_menu(sys::state& state, context_menu_context context) {
	std::vector<context_menu_entry_logic*> logics = std::vector<context_menu_entry_logic*>(16);

	if(context.province) {
		logics[0] = &context_menu_build_factory_logic;
	}
	else if(context.factory) {
		logics[0] = &context_menu_upgrade_factory_logic;
		logics[1] = &context_menu_delete_factory_logic;
	}
	else if(context.fconstruction) {
		logics[0] = &context_menu_cancel_factory_construction_logic;
	}

	if(!state.ui_state.context_menu) {
		auto new_mm = make_element_by_type<context_menu_window>(state, "context_menu_window");
		state.ui_state.context_menu = new_mm.get();
		state.ui_state.context_menu->logics = logics;
		state.ui_state.context_menu->context = context;
		state.ui_state.context_menu->base_data.position.x = (int16_t) (state.mouse_x_position / state.user_settings.ui_scale);
		state.ui_state.context_menu->base_data.position.y = (int16_t) (state.mouse_y_position / state.user_settings.ui_scale);
		state.ui_state.context_menu->set_visible(state, true);
		state.ui_state.context_menu->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_mm));
	} else {
		state.ui_state.context_menu->logics = logics;
		state.ui_state.context_menu->context = context;
		state.ui_state.context_menu->base_data.position.x = (int16_t)(state.mouse_x_position / state.user_settings.ui_scale);
		state.ui_state.context_menu->base_data.position.y = (int16_t)(state.mouse_y_position / state.user_settings.ui_scale);
		state.ui_state.context_menu->set_visible(state, true);
		state.ui_state.context_menu->impl_on_update(state);
		state.ui_state.root->move_child_to_front(state.ui_state.context_menu);
	}
}

}
