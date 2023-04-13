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

class trade_commodity_entry : public window_element_base {
public:
	dcon::commodity_id commodity_id{};

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if(name == "price") {
			return make_element_by_type<commodity_price_text>(state, id);
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

			auto ptr = make_child(state, "goods_entry", state.ui_state.defs_by_name.find("goods_entry")->second.definition);
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

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_entry") {
			return make_element_by_type<trade_commodity_entry>(state, id);
		} else {
			return nullptr;
		}
	}
};

class trade_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
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
		} else {
			return nullptr;
		}
	}
};

}
