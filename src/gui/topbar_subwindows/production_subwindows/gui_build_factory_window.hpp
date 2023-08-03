#pragma once

#include "gui_element_types.hpp"
#include "gui_production_enum.hpp"

namespace ui {

class factory_build_cancel_button : public generic_close_button {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override { text::add_line(state, contents, "production_close_tooltip"); }
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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override { text::add_line(state, contents, "shift_to_hold_open"); }

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
		auto total = 0.0f;
		for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
			auto cid = name.commodity_type[i];
			if(bool(cid)) {
				total += state.world.commodity_get_current_price(cid) * name.commodity_amounts[i];
			}
		} // Credit to leaf for this code :3
		return text::format_money(total);
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

class factory_build_time_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id ftid) noexcept {
		auto fat = dcon::fatten(state.world, ftid);
		auto name = fat.get_construction_time();
		return (text::prettify(name) + "d");
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

class factory_build_item_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any sidload = dcon::state_instance_id{};
			parent->impl_get(state, sidload);
			auto sid = any_cast<dcon::state_instance_id>(sidload);
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);

			disabled = !command::can_begin_factory_building_construction(state, state.local_player_nation, sid, content, false);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);

			Cyto::Any payload2 = element_selection_wrapper<dcon::factory_type_id>{content};
			parent->impl_get(state, payload2);
		}
	}
};

class factory_build_item : public listbox_row_element_base<dcon::factory_type_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
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

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(dcon::fatten(state.world, content).get_output().id);
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::factory_type_id>::get(state, payload);
	}
};

class factory_build_list : public listbox_element_base<factory_build_item, dcon::factory_type_id> {
protected:
	std::string_view get_row_element_name() override { return "new_factory_option"; }

public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any s_payload = dcon::state_instance_id{};
			parent->impl_get(state, s_payload);
			auto sid = any_cast<dcon::state_instance_id>(s_payload);

			row_contents.clear();
			// First the buildable factories
			state.world.for_each_factory_type([&](dcon::factory_type_id ftid) {
				if(command::can_begin_factory_building_construction(state, state.local_player_nation, sid, ftid, false))
					row_contents.push_back(ftid);
			});
			// Then the ones that can't be built
			state.world.for_each_factory_type([&](dcon::factory_type_id ftid) {
				if(!command::can_begin_factory_building_construction(state, state.local_player_nation, sid, ftid, false))
					row_contents.push_back(ftid);
			});
			update(state);
		}
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
	std::string get_text(sys::state& state) noexcept { return text::format_money(nations::get_treasury(state, state.local_player_nation)); }

	void on_update(sys::state& state) noexcept override { set_text(state, get_text(state)); }
};

class factory_build_description : public multiline_text_element_base {
public:
	void on_create(sys::state& state) noexcept override { multiline_text_element_base::on_create(state); }

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			auto fat = dcon::fatten(state.world, content);

			auto layout = text::create_endless_layout(internal_layout, text::layout_parameters{0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black, false});
			auto box = text::open_layout_box(layout, 0);
			text::add_to_layout_box(state, layout, box, fat.get_description());
			text::close_layout_box(layout, box);
		}
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

	/*
	 * There are only two hard things in Computer Science:
	 * Cache invalidation and naming thing -Phil Karlton
	 */
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<draggable_target>(state, id);

		} else if(name == "state_name") {
			return make_element_by_type<state_name_text>(state, id);

		} else if(name == "factory_type") {
			return make_element_by_type<factory_build_list>(state, id);

		} else if(name == "input_label") {
			return make_element_by_type<image_element_base>(state, id);

		} else
			// input_0_amount
			// input_1_amount
			// input_2_amount
			// input_3_amount
			if(name == "output") {
				return make_element_by_type<commodity_image>(state, id);

			} else if(name == "building_name") {
				return make_element_by_type<factory_title>(state, id);

			} else if(name == "output_label") {
				return make_element_by_type<simple_text_element_base>(state, id);

			} else if(name == "output_amount") {
				return make_element_by_type<output_amount_text>(state, id);

			} else
				// input_0
				// input_1
				// input_2
				// input_3
				if(name == "description_text") {
					return make_element_by_type<factory_build_description>(state, id);

				} else if(name == "needed_workers") {
					return make_element_by_type<simple_text_element_base>(state, id);

				} else if(name == "needed_workers_count") {
					return make_element_by_type<needed_workers_count_text>(state, id);

				} else if(name == "available_workers") {
					return make_element_by_type<simple_text_element_base>(state, id);

				} else if(name == "resource_cost_label") {
					return make_element_by_type<simple_text_element_base>(state, id);

				} else if(name == "in_stockpile_label") {
					return make_element_by_type<simple_text_element_base>(state, id);

				} else if(name == "base_price_label") {
					return make_element_by_type<simple_text_element_base>(state, id);

				} else if(name == "input_price_label") {
					return make_element_by_type<simple_text_element_base>(state, id);

				} else if(name == "total_label") {
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

				} else if(name == "prod_label") {
					return make_element_by_type<simple_text_element_base>(state, id);

				} else if(name == "prod_cost") {
					return make_element_by_type<simple_text_element_base>(state, id);

				} else if(name == "cancel") {
					return make_element_by_type<factory_build_cancel_button>(state, id);

				} else if(name == "build") {
					return make_element_by_type<factory_build_button>(state, id);

				} else {
					return nullptr;
				}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::factory_type_id>>()) {
			factory_to_build = any_cast<element_selection_wrapper<dcon::factory_type_id>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		}
		//======================================================================================================
		else if(payload.holds_type<dcon::factory_type_id>()) {
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
