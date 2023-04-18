#pragma once

#include "gui_element_types.hpp"

namespace ui {

class goods_filter_window : public window_element_base {
public:
	int currgood = -3;
	dcon::gui_def_id filter_button_id;
	dcon::gui_def_id filter_enabled_id;
	dcon::gui_def_id goods_type_id;
	dcon::gfx_object_id goods_gfx_id;
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		for(uint8_t i = 0; i < state.ui_defs.gfx[goods_gfx_id].number_of_frames - 1; i++) {
			auto ptr = make_child(state, "goods_type", goods_type_id);
			this->add_child_to_back(std::move(ptr));
			auto ptr2 = make_child(state, "filter_enabled", filter_enabled_id);
			this->add_child_to_back(std::move(ptr2));
			auto ptr3 = make_child(state, "filter_button", filter_button_id);
			this->add_child_to_back(std::move(ptr3));
			currgood++;
		}
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "filter_button") {
			if(currgood == -1) {
				auto ptr = make_element_by_type<button_element_base>(state, id);
				ptr->set_visible(state, false);
				filter_button_id = id;
				currgood++;
				return ptr;
			}
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->base_data.position.y = int16_t(55 + (30 * (currgood/24)));
			int rowlimiter = currgood - (24 * (currgood / 24));
			if(rowlimiter == 0) {
				ptr->base_data.position.x = int16_t(33 * (1 + rowlimiter));
			} else {
				ptr->base_data.position.x = int16_t((33 * (1 + rowlimiter)) - (rowlimiter * 2));
			}
			return ptr;
		} else if(name == "filter_enabled") {
			if(currgood == -2) {
				auto ptr = make_element_by_type<image_element_base>(state, id);
				ptr->set_visible(state, false);
				filter_enabled_id = id;
				currgood++;
				return ptr;
			}
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = 1;
			ptr->base_data.position.y = int16_t(55 + (30 * (currgood / 24)));
			int rowlimiter = currgood - (24 * (currgood / 24));
			if(rowlimiter == 0) {
				ptr->base_data.position.x = int16_t(33 * (1 + rowlimiter));
			} else {
				ptr->base_data.position.x = int16_t((33 * (1 + rowlimiter)) - (rowlimiter * 2));
			}
			return ptr;
		} else if(name == "goods_type") {
			if(currgood == -3) {
				auto ptr = make_element_by_type<image_element_base>(state, id);
				ptr->set_visible(state, false);
				goods_gfx_id = ptr->base_data.data.image.gfx_object;
				goods_type_id = id;
				currgood++;
				return ptr;
			}
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = 1+currgood;
			ptr->base_data.position.y = int16_t(57 + (30 * (currgood / 24)));
			int rowlimiter = currgood - (24 * (currgood / 24));
			if(rowlimiter == 0) {
				ptr->base_data.position.x = int16_t((33 * (1 + rowlimiter)) + 2);
			} else {
				ptr->base_data.position.x = int16_t((33 * (1 + rowlimiter)) + (2 - (rowlimiter * 2)));
			}
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
