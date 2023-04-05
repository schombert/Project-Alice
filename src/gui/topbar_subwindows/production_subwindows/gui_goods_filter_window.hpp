#pragma once

#include "gui_element_types.hpp"

namespace ui {

class goods_filter_window : public window_element_base {
public:
	int currgood = -3;
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		for(int i = 0; i < 48; i++) {
			auto ptr = make_child(state, "goods_type", dcon::gui_def_id(1379));
			this->add_child_to_back(std::move(ptr));
			auto ptr2 = make_child(state, "filter_enabled", dcon::gui_def_id(1378));
			this->add_child_to_back(std::move(ptr2));
			auto ptr3 = make_child(state, "filter_button", dcon::gui_def_id(1377));
			this->add_child_to_back(std::move(ptr3));
			currgood++;
		}
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "filter_button") {
			if(currgood == -1) {
				currgood++;
				return nullptr;
			}
			auto ptr = make_element_by_type<button_element_base>(state, id);
			if(currgood < 24) {
				ptr->base_data.position.y = 55;
				if(currgood == 0) {
					ptr->base_data.position.x = int16_t(33 * (1 + currgood));
				} else {
					ptr->base_data.position.x = int16_t((33 * (1 + currgood)) - (currgood * 2));
				}
			} else {
				ptr->base_data.position.y = 85;
				if(currgood-24 == 0) {
					ptr->base_data.position.x = int16_t(33 * (1 + (currgood-24)));
				} else {
					ptr->base_data.position.x = int16_t((33 * (1 + (currgood-24))) - ((currgood-24) * 2));
				}
			}
			return ptr;
		} else if(name == "filter_enabled") {
			if(currgood == -2) {
				currgood++;
				return nullptr;
			}
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = 1;
			if(currgood < 24) {
				ptr->base_data.position.y = 55;
				if(currgood == 0) {
					ptr->base_data.position.x = int16_t(33 * (1 + currgood));
				} else {
					ptr->base_data.position.x = int16_t((33 * (1 + currgood)) - (currgood * 2));
				}
			} else {
				ptr->base_data.position.y = 85;
				if(currgood-24 == 0) {
					ptr->base_data.position.x = int16_t(33 * (1 + (currgood-24)));
				} else {
					ptr->base_data.position.x = int16_t((33 * (1 + (currgood-24))) - ((currgood-24) * 2));
				}
			}
			return ptr;
		} else if(name == "goods_type") {
			if(currgood == -3) {
				currgood++;
				return nullptr;
			}
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = 1+currgood;
			if(currgood < 24) {
				ptr->base_data.position.y = 57;
				if(currgood == 0) {
					ptr->base_data.position.x = int16_t((33 * (1 + currgood)) + 2);
				} else {
					ptr->base_data.position.x = int16_t((33 * (1 + currgood)) + (2 - (currgood * 2)));
				}
			} else {
				ptr->base_data.position.y = 87;
				if(currgood-24 == 0) {
					ptr->base_data.position.x = int16_t((33 * (1 + (currgood-24))) + 2);
				} else {
					ptr->base_data.position.x = int16_t((33 * (1 + (currgood-24))) + (2 - ((currgood-24) * 2)));
				}
			}
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
