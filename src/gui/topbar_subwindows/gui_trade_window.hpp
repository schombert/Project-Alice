#pragma once

#include "gui_element_types.hpp"

namespace ui {
class trade_market_activity_entry : public listbox_row_element_base<dcon::commodity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if(name == "cost") {
			return make_element_by_type<commodity_price_text>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			impl_set(state, payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};
class trade_market_activity_listbox : public listbox_element_base<trade_market_activity_entry, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
        return "market_activity_entry";
    }
public:
	void on_create(sys::state& state) noexcept override {
		listbox_element_base::on_create(state);

		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			row_contents.push_back(id);
		});
		update(state);
	}
};

class trade_stockpile_entry : public listbox_row_element_base<dcon::commodity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if(name == "value") {
			return make_element_by_type<commodity_national_player_stockpile_text>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			impl_set(state, payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};
class trade_stockpile_listbox : public listbox_element_base<trade_stockpile_entry, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
        return "stockpile_entry";
    }
public:
	void on_create(sys::state& state) noexcept override {
		listbox_element_base::on_create(state);

		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			row_contents.push_back(id);
		});
		update(state);
	}
};

class trade_common_market_entry : public listbox_row_element_base<dcon::commodity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			impl_set(state, payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};
class trade_common_market_listbox : public listbox_element_base<trade_common_market_entry, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
        return "common_market_entry";
    }
public:
	void on_create(sys::state& state) noexcept override {
		listbox_element_base::on_create(state);

		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			row_contents.push_back(id);
		});
		update(state);
	}
};

class trade_commodity_entry_button : public button_element_base {
	dcon::commodity_id commodity_id{};
public:
	void button_action(sys::state& state) noexcept override {
		Cyto::Any payload = commodity_id;
		parent->impl_get(state, payload);
	}
};
class trade_commodity_entry : public window_element_base {
	dcon::commodity_id commodity_id{};
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "entry_button") {
			return make_element_by_type<trade_commodity_entry_button>(state, id);
		} else if(name == "goods_type") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if(name == "price") {
			return make_element_by_type<commodity_price_text>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			commodity_id = any_cast<dcon::commodity_id>(payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};
class trade_flow_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "material_name") {
			return make_element_by_type<generic_name_text<dcon::commodity_id>>(state, id);
		} else if(name == "material_icon_big") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if(name == "header_produced_by") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_produced");
			return ptr;
		} else if(name == "header_used_by") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_used");
			return ptr;
		} else if(name == "header_may_be_used_by") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_may_be_used");
			return ptr;
		} else if(name == "total_produced_text") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_total_produced");
			return ptr;
		} else if(name == "total_used_text") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_total_used");
			return ptr;
		} else {
			return nullptr;
		}
	}
};
template<sys::commodity_group Group>
class trade_commodity_group_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		// TODO: use goods_entry_offset instead of hardcoding
		xy_pair cell_size{ 99, 33 };

		xy_pair offset{ 0, 0 };
		xy_pair base_offset = offset;
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if(sys::commodity_group(state.world.commodity_get_commodity_group(id)) != Group)
				return;

			auto ptr = make_element_by_type<trade_commodity_entry>(state, state.ui_state.defs_by_name.find("goods_entry")->second.definition);
			ptr->base_data.position = offset;
			offset.x += cell_size.x;
			if(offset.x + cell_size.x >= base_data.size.x) {
				offset.x = base_offset.x;
				offset.y += cell_size.y;
			}

			Cyto::Any payload = id;
			ptr->impl_set(state, payload);
			add_child_to_front(std::move(ptr));
		});
	}
};

class trade_details_open_window {
public:
	dcon::commodity_id commodity_id{};
};
class trade_details_button : public button_element_base {
	dcon::commodity_id commodity_id{};
public:
	void button_action(sys::state& state) noexcept override {
		trade_details_open_window data{};
		data.commodity_id = commodity_id;
		Cyto::Any payload = data;
		parent->impl_get(state, payload);
	}
};
class trade_details_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_details") {
			return make_element_by_type<trade_details_button>(state, id);
		} else if(name == "goods_icon") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if(name == "goods_title") {
			return make_element_by_type<generic_name_text<dcon::commodity_id>>(state, id);
		} else if(name == "goods_price") {
			return make_element_by_type<commodity_price_text>(state, id);
		} else {
			return nullptr;
		}
	}
};

class trade_window : public window_element_base {
	trade_flow_window* trade_flow_win = nullptr;
	trade_details_window* details_win = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto ptr = make_element_by_type<trade_flow_window>(state, state.ui_state.defs_by_name.find("trade_flow")->second.definition);
		trade_flow_win = ptr.get();
		add_child_to_front(std::move(ptr));

		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "market_activity_list") {
			return make_element_by_type<trade_market_activity_listbox>(state, id);
		} else if(name == "common_market_list") {
			return make_element_by_type<trade_common_market_listbox>(state, id);
		} else if(name == "stockpile_list") {
			return make_element_by_type<trade_stockpile_listbox>(state, id);
		} else if(name == "group_raw_material_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::raw_material_goods>>(state, id);
		} else if(name == "group_industrial_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::industrial_goods>>(state, id);
		} else if(name == "group_consumer_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::consumer_goods>>(state, id);
		} else if(name == "group_military_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::military_goods>>(state, id);
		} else if(name == "trade_details") {
			auto ptr = make_element_by_type<trade_details_window>(state, id);
			details_win = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		// Special mesage rebroadcasted by the details button from the hierachy
		if(payload.holds_type<trade_details_open_window>()) {
			trade_flow_win->set_visible(state, true);
			Cyto::Any new_payload = any_cast<trade_details_open_window>(payload).commodity_id;
			trade_flow_win->impl_set(state, new_payload);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::commodity_id>()) {
			details_win->impl_set(state, payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}
