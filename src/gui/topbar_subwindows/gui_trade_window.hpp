#pragma once

#include "dcon_generated.hpp"
#include "economy.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {
class trade_market_activity_entry : public listbox_row_element_base<dcon::commodity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "goods_type") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if (name == "cost") {
			return make_element_by_type<commodity_player_availability_text>(state, id);
		} else {
			return nullptr;
		}
	}
};
class trade_market_activity_listbox : public listbox_element_base<trade_market_activity_entry, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "market_activity_entry";
	}

public:
	void on_create(sys::state &state) noexcept override {
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
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "goods_type") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if (name == "value") {
			return make_element_by_type<commodity_national_player_stockpile_text>(state, id);
		} else if (name == "change") {
			return make_element_by_type<commodity_player_stockpile_increase_text>(state, id);
		} else {
			return nullptr;
		}
	}
};
class trade_stockpile_listbox : public listbox_element_base<trade_stockpile_entry, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "stockpile_entry";
	}

public:
	void on_create(sys::state &state) noexcept override {
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
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "goods_type") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if (name == "total") {
			return make_element_by_type<commodity_global_market_text>(state, id);
		} else if (name == "produce_change") {
			return make_element_by_type<commodity_player_real_demand_text>(state, id);
		} else if (name == "exported") {
			return make_element_by_type<commodity_player_domestic_market_text>(state, id);
		} else {
			return nullptr;
		}
	}
};
class trade_common_market_listbox : public listbox_element_base<trade_common_market_entry, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "common_market_entry";
	}

public:
	void on_create(sys::state &state) noexcept override {
		listbox_element_base::on_create(state);
		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			row_contents.push_back(id);
		});
		update(state);
	}
};

template <class T>
class trade_goods_needs_entry : public listbox_row_element_base<dcon::commodity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "goods_type") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if (name == "value") {
			return make_element_by_type<T>(state, id);
		} else {
			return nullptr;
		}
	}
};

class trade_government_needs_listbox : public listbox_element_base<trade_goods_needs_entry<commodity_player_government_needs_text>, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "goods_needs_entry";
	}

public:
	void on_create(sys::state &state) noexcept override {
		listbox_element_base::on_create(state);
		on_update(state);
	}
	void on_update(sys::state &state) noexcept override {
		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if (economy::government_consumption(state, state.local_player_nation, id) > 0.f) {
				row_contents.push_back(id);
			}
		});
		update(state);
	}
};

class trade_factory_needs_listbox : public listbox_element_base<trade_goods_needs_entry<commodity_player_factory_needs_text>, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "goods_needs_entry";
	}

public:
	void on_create(sys::state &state) noexcept override {
		listbox_element_base::on_create(state);
		on_update(state);
	}
	void on_update(sys::state &state) noexcept override {
		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if (economy::nation_factory_consumption(state, state.local_player_nation, id) > 0.f) {
				row_contents.push_back(id);
			}
		});
		update(state);
	}
};

class trade_pop_needs_listbox : public listbox_element_base<trade_goods_needs_entry<commodity_player_pop_needs_text>, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "goods_needs_entry";
	}

public:
	void on_create(sys::state &state) noexcept override {
		listbox_element_base::on_create(state);
		on_update(state);
	}
	void on_update(sys::state &state) noexcept override {
		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if (economy::nation_pop_consumption(state, state.local_player_nation, id) > 0.f) {
				row_contents.push_back(id);
			}
		});
		update(state);
	}
};

struct trade_details_select_commodity {
	dcon::commodity_id commodity_id{};
};
class trade_commodity_entry_button : public button_element_base {
	dcon::commodity_id commodity_id{};

public:
	void button_action(sys::state &state) noexcept override {
		Cyto::Any payload = trade_details_select_commodity{commodity_id};
		state.ui_state.trade_subwindow->impl_get(state, payload);
	}

	message_result set(sys::state &state, Cyto::Any &payload) noexcept override {
		if (payload.holds_type<dcon::commodity_id>()) {
			commodity_id = any_cast<dcon::commodity_id>(payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};
class trade_commodity_entry : public window_element_base {
	dcon::commodity_id commodity_id{};

public:
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "entry_button") {
			return make_element_by_type<trade_commodity_entry_button>(state, id);
		} else if (name == "goods_type") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if (name == "price") {
			return make_element_by_type<commodity_price_text>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state &state, Cyto::Any &payload) noexcept override {
		if (payload.holds_type<dcon::commodity_id>()) {
			commodity_id = any_cast<dcon::commodity_id>(payload);
			on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	message_result get(sys::state &state, Cyto::Any &payload) noexcept override {
		if (payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(commodity_id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class trade_flow_data {
public:
	enum class type : uint8_t {
		factory,
		province,
		pop,
		military_army,
		military_navy,
	} type{};
	enum class value_type : uint8_t {
		used_by,
		produced_by,
		may_be_used_by
	} value_type{};
	union {
		dcon::factory_id factory_id;       // factory
		dcon::province_id province_id;     // province
		dcon::province_id pop_province_id; // pop
		dcon::army_id army_id;             // army
		dcon::navy_id navy_id;             // navy
	} data{};
};
class trade_flow_entry : public listbox_row_element_base<trade_flow_data> {
	image_element_base *icon = nullptr;
	simple_text_element_base *title = nullptr;
	simple_text_element_base *value = nullptr;
	image_element_base *output_icon = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			icon = ptr.get();
			return ptr;
		} else if (name == "title") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			title = ptr.get();
			return ptr;
		} else if (name == "value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			value = ptr.get();
			return ptr;
		} else if (name == "output_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			output_icon = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update(sys::state &state) noexcept override {
		if (parent) {
			Cyto::Any payload = dcon::commodity_id{};
			parent->impl_get(state, payload);
			auto commodity_id = any_cast<dcon::commodity_id>(payload);

			icon->frame = int32_t(content.type);
			if (content.type == trade_flow_data::type::military_navy)
				icon->frame = int32_t(trade_flow_data::type::military_army);

			output_icon->set_visible(state, content.value_type != trade_flow_data::value_type::produced_by);
			value->set_visible(state, content.value_type != trade_flow_data::value_type::may_be_used_by);
			float amount = 0.f;
			switch (content.type) {
			case trade_flow_data::type::factory: {
				auto fid = content.data.factory_id;
				auto ftid = state.world.factory_get_building_type(fid);
				switch (content.value_type) {
				case trade_flow_data::value_type::produced_by: {
					amount += state.world.factory_get_actual_production(fid);
				} break;
				case trade_flow_data::value_type::used_by: {
					auto &inputs = state.world.factory_type_get_inputs(ftid);
					for (uint32_t i = 0; i < inputs.set_size; ++i)
						if (inputs.commodity_type[i] == commodity_id)
							amount += inputs.commodity_amounts[i];
					output_icon->frame = state.world.commodity_get_icon(state.world.factory_type_get_output(ftid));
				} break;
				case trade_flow_data::value_type::may_be_used_by: {
					auto &inputs = state.world.factory_type_get_inputs(ftid);
					output_icon->frame = state.world.commodity_get_icon(state.world.factory_type_get_output(ftid));
				} break;
				default:
					break;
				}
				auto name = state.world.factory_type_get_name(ftid);
				title->set_text(state, text::produce_simple_string(state, name));
			} break;
			case trade_flow_data::type::province: {
				auto pid = content.data.province_id;
				switch (content.value_type) {
				case trade_flow_data::value_type::produced_by: {
					amount += state.world.province_get_rgo_actual_production(pid);
				} break;
				case trade_flow_data::value_type::used_by:
				case trade_flow_data::value_type::may_be_used_by:
				default:
					break;
				}
				auto name = state.world.province_get_name(pid);
				title->set_text(state, text::produce_simple_string(state, name));
			} break;
			case trade_flow_data::type::pop:
				break;
			case trade_flow_data::type::military_army:
				break;
			case trade_flow_data::type::military_navy:
				break;
			default:
				break;
			}
			if (value->is_visible())
				value->set_text(state, text::format_float(amount, 2));
		}
	}
};
class trade_flow_listbox_base : public listbox_element_base<trade_flow_entry, trade_flow_data> {
protected:
	std::string_view get_row_element_name() override {
		return "trade_flow_entry";
	}

	template <typename F>
	void populate_rows(sys::state &state, F &&factory_func, enum trade_flow_data::value_type vt) {
		if (parent) {
			Cyto::Any payload = dcon::commodity_id{};
			parent->impl_get(state, payload);
			auto commodity_id = any_cast<dcon::commodity_id>(payload);

			for (const auto fat_stown_id : state.world.nation_get_state_ownership(state.local_player_nation)) {
				province::for_each_province_in_state_instance(state, fat_stown_id.get_state(), [&](dcon::province_id pid) {
					auto fat_id = dcon::fatten(state.world, pid);
					fat_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
						auto fid = state.world.factory_location_get_factory(flid);
						if (factory_func(fid)) {
							trade_flow_data td{};
							td.type = trade_flow_data::type::factory;
							td.value_type = vt;
							td.data.factory_id = fid;
							row_contents.push_back(td);
						}
					});
					if (vt == trade_flow_data::value_type::produced_by)
						if (state.world.province_get_rgo(pid) == commodity_id) {
							trade_flow_data td{};
							td.type = trade_flow_data::type::province;
							td.value_type = vt;
							td.data.province_id = pid;
							row_contents.push_back(td);
						}
				});
			}
		}
	}

public:
};

class trade_flow_produced_by_listbox : public trade_flow_listbox_base {
public:
	void on_update(sys::state &state) noexcept override {
		if (parent) {
			Cyto::Any payload = dcon::commodity_id{};
			parent->impl_get(state, payload);
			auto commodity_id = any_cast<dcon::commodity_id>(payload);

			row_contents.clear();
			populate_rows(
			    state, [&](dcon::factory_id fid) -> bool {
				    auto ftid = state.world.factory_get_building_type(fid);
				    return state.world.factory_type_get_output(ftid) == commodity_id;
			    },
			    trade_flow_data::value_type::produced_by);
			update(state);
		}
	}
};
class trade_flow_used_by_listbox : public trade_flow_listbox_base {
public:
	void on_update(sys::state &state) noexcept override {
		if (parent) {
			Cyto::Any payload = dcon::commodity_id{};
			parent->impl_get(state, payload);
			auto commodity_id = any_cast<dcon::commodity_id>(payload);

			row_contents.clear();
			populate_rows(
			    state, [&](dcon::factory_id fid) -> bool {
				    auto ftid = state.world.factory_get_building_type(fid);
				    auto &inputs = state.world.factory_type_get_inputs(ftid);
				    for (uint32_t i = 0; i < inputs.set_size; ++i)
					    if (inputs.commodity_type[i] == commodity_id)
						    return inputs.commodity_amounts[i] > 0.f; // Some inputs taken
				    return false;
			    },
			    trade_flow_data::value_type::used_by);
			update(state);
		}
	}
};
class trade_flow_may_be_used_by_listbox : public trade_flow_listbox_base {
public:
	void on_update(sys::state &state) noexcept override {
		if (parent) {
			Cyto::Any payload = dcon::commodity_id{};
			parent->impl_get(state, payload);
			auto commodity_id = any_cast<dcon::commodity_id>(payload);

			row_contents.clear();
			populate_rows(
			    state, [&](dcon::factory_id fid) -> bool {
				    auto ftid = state.world.factory_get_building_type(fid);
				    auto &inputs = state.world.factory_type_get_inputs(ftid);
				    for (uint32_t i = 0; i < inputs.set_size; ++i)
					    if (inputs.commodity_type[i] == commodity_id)
						    return inputs.commodity_amounts[i] == 0.f; // No inputs intaken
				    return false;
			    },
			    trade_flow_data::value_type::may_be_used_by);
			update(state);
		}
	}
};

class trade_flow_price_graph_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "current_price_value") {
			return make_element_by_type<commodity_price_text>(state, id);
		} else {
			return nullptr;
		}
	}
};

class trade_flow_total_produced_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state &state, dcon::commodity_id commodity_id) noexcept override {
		auto amount = 0.f;
		for (const auto fat_stown_id : state.world.nation_get_state_ownership(state.local_player_nation)) {
			province::for_each_province_in_state_instance(state, fat_stown_id.get_state(), [&](dcon::province_id pid) {
				auto fat_id = dcon::fatten(state.world, pid);
				fat_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
					auto fid = state.world.factory_location_get_factory(flid);
					auto ftid = state.world.factory_get_building_type(fid);
					if (state.world.factory_type_get_output(ftid) == commodity_id)
						amount += state.world.factory_get_actual_production(fid);
				});
				if (state.world.province_get_rgo(pid) == commodity_id)
					amount += state.world.province_get_rgo_actual_production(pid);
			});
		}
		return text::format_float(amount, 3);
	}
};
class trade_flow_total_used_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state &state, dcon::commodity_id commodity_id) noexcept override {
		auto amount = economy::nation_factory_consumption(state, state.local_player_nation, commodity_id);
		return text::format_float(amount, 3);
	}
};

class trade_flow_window : public window_element_base {
public:
	void on_create(sys::state &state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if (name == "trade_flow_bg") {
			return make_element_by_type<draggable_target>(state, id);
		} else if (name == "material_name") {
			return make_element_by_type<generic_name_text<dcon::commodity_id>>(state, id);
		} else if (name == "material_icon_big") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if (name == "header_produced_by") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_produced");
			return ptr;
		} else if (name == "header_used_by") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_used");
			return ptr;
		} else if (name == "header_may_be_used_by") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_may_be_used");
			return ptr;
		} else if (name == "total_produced_text") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_total_produced");
			ptr->base_data.position.x += 32; // Nudge
			return ptr;
		} else if (name == "total_used_text") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_total_used");
			ptr->base_data.position.x += 32; // Nudge
			return ptr;
		} else if (name == "total_produced_value") {
			return make_element_by_type<trade_flow_total_produced_text>(state, id);
		} else if (name == "total_used_value") {
			return make_element_by_type<trade_flow_total_used_text>(state, id);
		} else if (name == "price_graph") {
			return make_element_by_type<trade_flow_price_graph_window>(state, id);
		} else if (name == "produced_by_listbox") {
			return make_element_by_type<trade_flow_produced_by_listbox>(state, id);
		} else if (name == "used_by_listbox") {
			return make_element_by_type<trade_flow_used_by_listbox>(state, id);
		} else if (name == "may_be_used_by_listbox") {
			return make_element_by_type<trade_flow_may_be_used_by_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};

template <sys::commodity_group Group>
class trade_commodity_group_window : public window_element_base {
public:
	void on_create(sys::state &state) noexcept override {
		window_element_base::on_create(state);
		xy_pair cell_size = state.ui_defs.gui[state.ui_state.defs_by_name.find("goods_entry_offset")->second.definition].position;
		xy_pair offset{0, 0};
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if (sys::commodity_group(state.world.commodity_get_commodity_group(id)) != Group)
				return;
			auto ptr = make_element_by_type<trade_commodity_entry>(state, state.ui_state.defs_by_name.find("goods_entry")->second.definition);
			ptr->base_data.position = offset;
			offset.x += cell_size.x;
			if (offset.x + cell_size.x - 1 >= base_data.size.x) {
				offset.x = 0;
				offset.y += cell_size.y;
				if (offset.y + cell_size.y >= base_data.size.y) {
					offset.x += cell_size.x;
					offset.y = 0;
				}
			}
			Cyto::Any payload = id;
			ptr->impl_set(state, payload);
			add_child_to_front(std::move(ptr));
		});
	}
};

struct trade_details_open_window {
	dcon::commodity_id commodity_id{};
};
class trade_details_button : public button_element_base {
public:
	void button_action(sys::state &state) noexcept override {
		if (parent) {
			Cyto::Any payload = dcon::commodity_id{};
			parent->impl_get(state, payload);
			auto commodity_id = any_cast<dcon::commodity_id>(payload);

			Cyto::Any dt_payload = trade_details_open_window{commodity_id};
			state.ui_state.trade_subwindow->impl_get(state, dt_payload);
		}
	}
};

class trade_confirm_trade_button : public button_element_base {
public:
	void button_action(sys::state &state) noexcept override {
		if (parent) {
			Cyto::Any payload = float{};
			parent->impl_get(state, payload);
			auto famount = any_cast<float>(payload);

			Cyto::Any payload2 = dcon::commodity_id{};
			parent->impl_get(state, payload2);
			auto cid = any_cast<dcon::commodity_id>(payload2);

			command::change_stockpile_settings(state, state.local_player_nation, cid, famount, false);
		}
	}
};

class trade_slider : public scrollbar {
public:
	void on_value_change(sys::state &state, int32_t v) noexcept override {
		if (parent) {
			Cyto::Any payload = element_selection_wrapper<float>{float{scaled_value()}}; // Why
			parent->impl_get(state, payload);
		}
	}
};

class trade_slider_amount : public simple_text_element_base {
public:
	void on_update(sys::state &state) noexcept override {
		Cyto::Any payload = float{};
		parent->impl_get(state, payload);
		auto content = any_cast<float>(payload);
		set_text(state, text::format_float(content, 2));
	}
};

class trade_details_window : public window_element_base {
private:
	float trade_amount = 0.0f;

public:
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "trade_flow_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if (name == "goods_icon") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if (name == "goods_title") {
			return make_element_by_type<generic_name_text<dcon::commodity_id>>(state, id);
		} else if (name == "goods_price") {
			return make_element_by_type<commodity_price_text>(state, id);
		} else if (name == "automate_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if (name == "automate") {
			return make_element_by_type<button_element_base>(state, id);
		} else if (name == "sell_stockpile") {
			return make_element_by_type<button_element_base>(state, id);
		} else if (name == "sell_stockpile_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if (name == "sell_slidier_desc") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if (name == "sell_slider") {
			return make_element_by_type<trade_slider>(state, id);
		} else if (name == "slider_value") {
			return make_element_by_type<trade_slider_amount>(state, id);
		} else if (name == "confirm_trade") {
			return make_element_by_type<trade_confirm_trade_button>(state, id);
		} else if (name == "goods_details") {
			return make_element_by_type<trade_details_button>(state, id);
		} else if (name == "goods_need_gov_desc") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if (name == "goods_need_factory_desc") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if (name == "produced_detail_desc") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if (name == "goods_need_pop_desc") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state &state, Cyto::Any &payload) noexcept override {
		if (payload.holds_type<element_selection_wrapper<float>>()) {
			trade_amount = any_cast<element_selection_wrapper<float>>(payload).data;
			return message_result::consumed;
		}
		//===================================================================
		else if (payload.holds_type<float>()) {
			payload.emplace<float>(trade_amount);
			return message_result::consumed;
		}

		return message_result::unseen;
	}
};

class trade_window : public window_element_base {
	trade_flow_window *trade_flow_win = nullptr;
	trade_details_window *details_win = nullptr;
	dcon::commodity_id commodity_id{};

public:
	void on_create(sys::state &state) noexcept override {
		window_element_base::on_create(state);

		auto ptr = make_element_by_type<trade_flow_window>(state, state.ui_state.defs_by_name.find("trade_flow")->second.definition);
		trade_flow_win = ptr.get();
		add_child_to_front(std::move(ptr));

		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if (name == "market_activity_list") {
			return make_element_by_type<trade_market_activity_listbox>(state, id);
		} else if (name == "common_market_list") {
			return make_element_by_type<trade_common_market_listbox>(state, id);
		} else if (name == "stockpile_list") {
			return make_element_by_type<trade_stockpile_listbox>(state, id);
		} else if (name == "group_raw_material_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::raw_material_goods>>(state, id);
		} else if (name == "group_industrial_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::industrial_goods>>(state, id);
		} else if (name == "group_consumer_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::consumer_goods>>(state, id);
		} else if (name == "group_military_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::military_goods>>(state, id);
		} else if (name == "government_needs_list") {
			return make_element_by_type<trade_government_needs_listbox>(state, id);
		} else if (name == "factory_needs_list") {
			return make_element_by_type<trade_factory_needs_listbox>(state, id);
		} else if (name == "pop_needs_list") {
			return make_element_by_type<trade_pop_needs_listbox>(state, id);
		} else if (name == "trade_details") {
			auto ptr = make_element_by_type<trade_details_window>(state, id);
			details_win = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state &state, Cyto::Any &payload) noexcept override {
		// Special message rebroadcasted by the details button from the hierarchy
		if (payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(commodity_id);
			return message_result::consumed;
		} else if (payload.holds_type<trade_details_open_window>()) {
			commodity_id = any_cast<trade_details_open_window>(payload).commodity_id;
			trade_flow_win->set_visible(state, true);
			trade_flow_win->impl_on_update(state);
			return message_result::consumed;
		} else if (payload.holds_type<trade_details_select_commodity>()) {
			commodity_id = any_cast<trade_details_select_commodity>(payload).commodity_id;
			details_win->impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
