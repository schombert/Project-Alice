#pragma once

#include "gui_element_types.hpp"

namespace ui {

struct commodity_filter_query_data {
	dcon::commodity_id cid;
	bool filter;
};
struct commodity_filter_toggle_data : public element_selection_wrapper<dcon::commodity_id> {};

class commodity_filter_button : public button_element_base {
  public:
	void button_action(sys::state &state) noexcept override {
		if (parent) {
			Cyto::Any payload = dcon::commodity_id();
			parent->impl_get(state, payload);
			auto cid = any_cast<dcon::commodity_id>(payload);
			Cyto::Any f_payload = commodity_filter_toggle_data{cid};
			parent->impl_get(state, f_payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state &state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state &state, int32_t x, int32_t y, text::columnar_layout &contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		Cyto::Any payload = dcon::commodity_id{};
		parent->impl_get(state, payload);
		auto content = any_cast<dcon::commodity_id>(payload); // Runtime Error >w<
		text::localised_single_sub_box(state, contents, box, std::string_view("production_toggle_filter_tooltip"), text::variable_type::goods, dcon::fatten(state.world, content).get_name());
		text::close_layout_box(contents, box);
	}
};

class commodity_filter_enabled_image : public image_element_base {
  public:
	void on_update(sys::state &state) noexcept override {
		if (parent) {
			Cyto::Any payload = dcon::commodity_id();
			parent->impl_get(state, payload);
			auto cid = any_cast<dcon::commodity_id>(payload);
			Cyto::Any f_payload = commodity_filter_query_data{cid, false};
			parent->impl_get(state, f_payload);
			auto content = any_cast<commodity_filter_query_data>(f_payload);
			frame = content.filter ? 1 : 0; // Enabled=1, Disabled=0
		}
	}
};

class commodity_filter_item : public window_element_base {
  public:
	dcon::commodity_id content{};

	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "filter_button") {
			return make_element_by_type<commodity_filter_button>(state, id);
		} else if (name == "filter_enabled") {
			return make_element_by_type<commodity_filter_enabled_image>(state, id);
		} else if (name == "goods_type") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state &state, Cyto::Any &payload) noexcept override {
		if (payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(content);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class commodity_filters_window : public window_element_base {
  public:
	void on_create(sys::state &state) noexcept override {
		window_element_base::on_create(state);

		int16_t index = 0;
		state.world.for_each_commodity([&](dcon::commodity_id cid) {
			bool can_be_produced = false;
			state.world.for_each_factory_type([&](dcon::factory_type_id ftid) {
				can_be_produced = can_be_produced || state.world.factory_type_get_output(ftid) == cid;
			});
			if (!can_be_produced)
				return;

			auto ptr = make_element_by_type<commodity_filter_item>(state, "goods_filter_template");
			static_cast<commodity_filter_item *>(ptr.get())->content = cid;

			int16_t rowlimiter = index - (24 * (index / 24));
			if (rowlimiter == 0) {
				ptr->base_data.position.x = int16_t(33 * rowlimiter);
			} else {
				ptr->base_data.position.x = int16_t((33 * rowlimiter) - (rowlimiter * 2));
			}
			ptr->base_data.position.y = int16_t(30 * (index / 24));
			add_child_to_back(std::move(ptr));
			index++;
		});
	}
};

} // namespace ui
