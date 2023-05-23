#pragma once

#include "gui_element_types.hpp"

namespace ui {


			//move_child_to_front(new_factory);
			//new_factory->set_visible(state, true);
			//new_factory->impl_set(state, payload);

class factory_build_cancel_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		parent->set_visible(state, false);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("production_close_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_build_build_button : public button_element_base {
public:
};

class factory_build_new_factory_option : public listbox_row_element_base<dcon::factory_type_id> {
private:
	image_element_base* output_icon = nullptr;
public:
	/*void update(sys::state& state) noexcept override {
		auto fat_btid = dcon::fatten(state.world, content);
		auto good = fat_btid.get_output();
		int32_t icon = good.get_icon();
		output_icon->frame = icon;
		//auto cid = fat_btid.get_output().id;
		//output_icon->frame = int32_t(state.world.commodity_get_icon(cid));
	}*/

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "output") {
			//auto ptr = make_element_by_type<image_element_base>(state, id);
			//output_icon = ptr.get();
			//return ptr;
			return make_element_by_type<commodity_factory_image>(state, id);

		} else if(name == "name") {
			//return make_element_by_type<factory_output_name_text>(state, id);
			return make_element_by_type<simple_text_element_base>(state, id);


		} else if(name == "total_build_cost") {
			//return make_element_by_type<factory_build_new_factory_option_time>(state, id);
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "buildtime") {
			//return make_element_by_type<factory_build_new_factory_option_time>(state, id);
			return make_element_by_type<simple_text_element_base>(state, id);

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

class factory_build_new_factory_list : public listbox_element_base<factory_build_new_factory_option, dcon::factory_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "new_factory_option";
	}
public:
	void on_update(sys::state& state) noexcept override{
		if(parent) {
			//listbox_element_base<factory_build_new_factory_option, dcon::factory_type_id>::on_create(state);
			row_contents.clear();
			state.world.for_each_factory_type([&](dcon::factory_type_id ident) {
				row_contents.push_back(ident);
			});
			update(state);
		}
	}
};


class factory_build_new_factory_window : public window_element_base {
private:
	dcon::state_instance_id state_id;
public:
	/*void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);

	}*/

	/*
	 * There are only two hard things in Computer Science:
	 * Cache invalidation and naming thing -Phil Karlton
	 */
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else
		if(name == "state_name") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "factory_type") {
			return make_element_by_type<factory_build_new_factory_list>(state, id);
		} else
		if(name == "input_label") {
			return make_element_by_type<image_element_base>(state, id);
		} else
			// input_0_amount
			// input_1_amount
			// input_2_amount
			// input_3_amount
		if(name == "output") {
			return make_element_by_type<image_element_base>(state, id);
		} else
		if(name == "building_name") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "output_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "output_ammount") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
			// input_0
			// input_1
			// input_2
			// input_3
		if(name == "description_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "needed_workers") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "needed_workers_count") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "available_workers") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "resource_cost_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "in_stockpile_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "base_price_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "input_price_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "total_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "current_funds_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "base_price") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "input_price") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "total_price") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "you_have") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "prod_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "prod_cost") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "cancel") {
			return make_element_by_type<factory_build_cancel_button>(state, id);
		} else
		if(name == "build") {
			return make_element_by_type<factory_build_build_button>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::state_instance_id>()) {
			state_id = any_cast<dcon::state_instance_id>(payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}
