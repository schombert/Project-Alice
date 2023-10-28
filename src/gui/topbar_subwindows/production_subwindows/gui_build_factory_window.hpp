#pragma once

#include "gui_element_types.hpp"
#include "gui_production_enum.hpp"
#include "ai.hpp"

namespace ui {

class factory_build_cancel_button : public generic_close_button {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "production_close_tooltip");
	}
};

class factory_build_button : public shift_button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		disabled = !command::can_begin_factory_building_construction(state, state.local_player_nation, sid, content, false);
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			auto sid = retrieve<dcon::state_instance_id>(state, parent);
			auto content = retrieve<dcon::factory_type_id>(state, parent);

			command::begin_factory_building_construction(state, state.local_player_nation, sid, content, false);

			parent->set_visible(state, false);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "shift_to_hold_open");
	}

	void button_shift_action(sys::state& state) noexcept override {
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto content = retrieve<dcon::factory_type_id>(state, parent);

		command::begin_factory_building_construction(state, state.local_player_nation, sid, content, false);
	}


};

class factory_build_output_name_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id ftid) noexcept {
		auto fat = dcon::fatten(state.world, ftid);
		auto name = fat.get_name();
		return text::produce_simple_string(state, name);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		set_text(state, get_text(state, content));
	}
};

class factory_build_cost_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id ftid) noexcept {
		auto fat = dcon::fatten(state.world, ftid);
		auto& name = fat.get_construction_costs();

		float factory_mod = state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::factory_cost) + 1.0f;
		float admin_eff = state.world.nation_get_administrative_efficiency(state.local_player_nation);
		float admin_cost_factor =  (2.0f - admin_eff) * factory_mod;

		auto total = 0.0f;
		for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
			auto cid = name.commodity_type[i];
			if(bool(cid)) {
				total += state.world.commodity_get_current_price(cid) * name.commodity_amounts[i] * admin_cost_factor;
			}
		} // Credit to leaf for this code :3
		return text::format_money(total);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		set_text(state, get_text(state, content));
	}
};

class factory_build_time_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id ftid) noexcept {
		auto fat = dcon::fatten(state.world, ftid);
		auto name = fat.get_construction_time();
		return (text::prettify(name) + "d");
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		set_text(state, get_text(state, content));
	}
};

class factory_build_item_button : public button_element_base {
	bool visible = false;
public:
	void on_update(sys::state& state) noexcept override {
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		disabled = !command::can_begin_factory_building_construction(state, state.local_player_nation, sid, content, false);
		visible = !retrieve<bool>(state, parent);
	}

	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		send(state, parent, element_selection_wrapper<dcon::factory_type_id>{content});
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			button_element_base::render(state, x, y);
	}
};

class factory_build_item_highlight_image : public tinted_image_element_base {
	bool visible = false;
public:
	void on_create(sys::state& state) noexcept override {
		tinted_image_element_base::on_create(state);
		color = sys::pack_color(196, 255, 196);
	}

	void on_update(sys::state& state) noexcept override {
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		visible = retrieve<bool>(state, parent);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			tinted_image_element_base::render(state, x, y);
	}
};

class factory_build_item : public listbox_row_element_base<dcon::factory_type_id> {
	std::vector<dcon::factory_type_id> desired_types;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			auto hl_elm = make_element_by_type<factory_build_item_highlight_image>(state, id);
			hl_elm->base_data.size.x *= 2;	 // Nudge
			hl_elm->base_data.size.x += 42; // Nudge
			hl_elm->base_data.size.y += 5;	 // Nudge
			add_child_to_back(std::move(hl_elm));

			auto ptr = make_element_by_type<factory_build_item_button>(state, id);
			ptr->base_data.size.x *= 2;	 // Nudge
			ptr->base_data.size.x += 42; // Nudge
			ptr->base_data.size.y += 5;	 // Nudge
			return ptr;

		} else if(name == "output") {
			return make_element_by_type<commodity_image>(state, id);

		} else if(name == "name") {
			return make_element_by_type<factory_build_output_name_text>(state, id);

		} else if(name == "total_build_cost") {
			return make_element_by_type<factory_build_cost_text>(state, id);

		} else if(name == "buildtime") {
			return make_element_by_type<factory_build_time_text>(state, id);

		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		desired_types.clear();
		ai::get_desired_factory_types(state, state.local_player_nation, desired_types);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(dcon::fatten(state.world, content).get_output().id);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			auto sid = retrieve<dcon::state_instance_id>(state, parent);
			bool is_hl = std::find(desired_types.begin(), desired_types.end(), content) != desired_types.end();
			is_hl = is_hl && command::can_begin_factory_building_construction(state, state.local_player_nation, sid, content, false);
			payload.emplace<bool>(is_hl);
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::factory_type_id>::get(state, payload);
	}
};

class factory_build_list : public listbox_element_base<factory_build_item, dcon::factory_type_id> {
	std::vector<dcon::factory_type_id> desired_types;
	bool is_highlighted(sys::state& state, dcon::state_instance_id sid, dcon::factory_type_id ftid) {
		bool is_hl = std::find(desired_types.begin(), desired_types.end(), ftid) != desired_types.end();
		return is_hl && command::can_begin_factory_building_construction(state, state.local_player_nation, sid, ftid, false);
	}
protected:
	std::string_view get_row_element_name() override {
		return "new_factory_option";
	}

public:
	void on_update(sys::state& state) noexcept override {
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		row_contents.clear();
		desired_types.clear();
		ai::get_desired_factory_types(state, state.local_player_nation, desired_types);
		// First the desired factory types
		for(const auto ftid : desired_types)
			if(is_highlighted(state, sid, ftid))
				row_contents.push_back(ftid);
		// Then the buildable factories
		for(const auto ftid : state.world.in_factory_type) {
			if(command::can_begin_factory_building_construction(state, state.local_player_nation, sid, ftid, false) && std::find(desired_types.begin(), desired_types.end(), ftid) == desired_types.end()) {
				row_contents.push_back(ftid);
			}
		}
		// Then the ones that can't be built
		for(const auto ftid : state.world.in_factory_type) {
			if(!command::can_begin_factory_building_construction(state, state.local_player_nation, sid, ftid, false)) {
				row_contents.push_back(ftid);
			}
		}
		update(state);
	}
};

class factory_title : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id fid) noexcept {
		auto fat = dcon::fatten(state.world, fid);
		return text::produce_simple_string(state, fat.get_name());
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class needed_workers_count_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id fid) noexcept {
		auto fat = dcon::fatten(state.world, fid);
		return text::prettify(fat.get_base_workforce());
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class output_amount_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id fid) noexcept {
		auto fat = dcon::fatten(state.world, fid);
		return text::prettify(int64_t(fat.get_output_amount()));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class factory_current_funds_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state) noexcept {
		return text::format_money(nations::get_treasury(state, state.local_player_nation));
	}

	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}
};

class factory_build_description : public multiline_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		multiline_text_element_base::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			auto fat = dcon::fatten(state.world, content);

			auto layout = text::create_endless_layout(internal_layout,
					text::layout_parameters{0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black, false});
			auto box = text::open_layout_box(layout, 0);
			text::add_to_layout_box(state, layout, box, fat.get_description());
			text::close_layout_box(layout, box);
		}
	}
};

class input_n_amout : public simple_text_element_base {
public:
	int32_t n = 0;

	void on_update(sys::state& state) noexcept override {
		auto type = retrieve<dcon::factory_type_id>(state, parent);
		if(!type) {
			set_text(state, "");
			return;
		}
		auto& inputs = state.world.factory_type_get_inputs(type);
		if(n < int32_t(economy::commodity_set::set_size)) {
			auto amount = inputs.commodity_amounts[n];
			if(amount > 0) {
				set_text(state, text::format_float(amount, 2));
			} else {
				set_text(state, "");
			}
		}
	}
};

class input_n_image : public image_element_base {
public:
	int32_t n = 0;
	dcon::commodity_id c;

	void on_update(sys::state& state) noexcept override {
		auto type = retrieve<dcon::factory_type_id>(state, parent);
		if(!type) {
			c = dcon::commodity_id{};
			return;
		}
		auto& inputs = state.world.factory_type_get_inputs(type);
		if(n < int32_t(economy::commodity_set::set_size)) {
			c = inputs.commodity_type[n];
			frame = int32_t(state.world.commodity_get_icon(c));
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(c)	
			text::add_line(state, contents, state.world.commodity_get_name(c));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(c)
			image_element_base::render(state, x, y);
	}
};

class factory_build_window : public window_element_base {
private:
	dcon::factory_type_id factory_to_build{};

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "state_name") {
			return make_element_by_type<state_name_text>(state, id);
		} else if(name == "factory_type") {
			return make_element_by_type<factory_build_list>(state, id);
		} else if(name == "output") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "building_name") {
			return make_element_by_type<factory_title>(state, id);
		} else if(name == "output_amount") {
			return make_element_by_type<output_amount_text>(state, id);
		} else if(name == "description_text") {
			return make_element_by_type<factory_build_description>(state, id);
		} else if(name == "needed_workers") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "needed_workers_count") {
			return make_element_by_type<needed_workers_count_text>(state, id);
		} else if(name == "available_workers") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "current_funds_label") {
			return make_element_by_type<factory_current_funds_text>(state, id);
		} else if(name == "base_price") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "input_price") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "total_price") {
			return make_element_by_type<factory_build_cost_text>(state, id);
		} else if(name == "you_have") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "prod_cost") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "cancel") {
			return make_element_by_type<factory_build_cancel_button>(state, id);
		} else if(name == "build") {
			return make_element_by_type<factory_build_button>(state, id);
		} else if(name.substr(0, 6) == "input_" && parsers::is_integer(name.data() + 6, name.data() + name.length())) {
			auto ptr = make_element_by_type<input_n_image>(state, id);
			std::from_chars(name.data() + 6, name.data() + name.length(), ptr->n);
			return ptr;
			//input_0_amount
		} else if(name.substr(0, 6) == "input_" && name.substr(name.length() - 7) == "_amount" && parsers::is_integer(name.data() + 6, name.data() + name.length() - 7)) {
			auto ptr = make_element_by_type<input_n_amout>(state, id);
			std::from_chars(name.data() + 6, name.data() + name.length() - 7, ptr->n);
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::factory_type_id>>()) {
			factory_to_build = any_cast<element_selection_wrapper<dcon::factory_type_id>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::factory_type_id>()) {
			payload.emplace<dcon::factory_type_id>(factory_to_build);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(dcon::fatten(state.world, factory_to_build).get_output());
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
