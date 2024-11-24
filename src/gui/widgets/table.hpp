#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <functional>
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace table {

enum column_type {
	button,
	text,
};

enum sort_order {
	undefined,
	ascending,
	descending
};

inline sort_order toggle_sort_order(const sort_order order) {
	switch(order) {
	case sort_order::undefined:
		return sort_order::ascending;
	case sort_order::ascending:
		return sort_order::descending;
	case sort_order::descending:
		return sort_order::ascending;
	default:
		return sort_order::ascending;
	}
}

struct sort_data {
	sort_order order;
	uint8_t sorted_index;
};

template<typename item_type>
void tooltip_fallback (sys::state& state, text::columnar_layout& contents, const item_type& a, std::string fallback) {
	auto box = text::open_layout_box(contents, 0);
	text::localised_format_box(state, contents, box, std::string_view(fallback));
	text::close_layout_box(contents, box);
}

template<typename item_type>
struct column {
	bool sortable = true;
	std::string header = "???";
	std::function<bool(sys::state& state, ui::element_base* container, const item_type & a, const item_type & b)> compare;
	std::function<std::string(sys::state& state, ui::element_base* container, const item_type& a)> view;
	std::function<void(
		sys::state& state,
		text::columnar_layout& contents,
		const item_type& a,
		std::string fallback
	)> update_tooltip = tooltip_fallback<item_type>;
	std::string cell_definition_string="thin_cell_number";
	std::string header_definition_string="thin_cell_number";
	bool has_tooltip = false;
};

template<typename item_type>
struct data {
	std::vector<column<item_type>> columns;
	std::vector<sort_data> sort_priority;
	std::vector<item_type> data;

	void toggle_sort_column_by_index(uint8_t column_index) {
		if(!columns[column_index].sortable) {
			return;
		}

		uint8_t offset = 0;
		bool offset_found = false;
		sort_order current_order = sort_order::undefined;
		for(uint8_t i = 0; i < sort_priority.size(); i++) {
			if(sort_priority[i].sorted_index == column_index) {
				offset_found = true;
				offset = i;
				current_order = sort_priority[i].order;
			}
		}

		if(offset_found) {
			auto begin = sort_priority.begin();
			sort_priority.erase(begin + offset);
		}

		sort_data new_sort_data = {};
		new_sort_data.order = toggle_sort_order(current_order);
		new_sort_data.sorted_index = column_index;
		sort_priority.push_back(new_sort_data);
	}

	void update_rows_order(sys::state& state, ui::element_base* container) {
		for(auto& order : sort_priority) {
			if(order.order == sort_order::ascending) {
				sys::merge_sort(
					data.begin(),
					data.end(),
					[&](const item_type& a, const item_type& b) {
						return columns[order.sorted_index].compare(state, container, a, b);
					}
				);
			} else {
				sys::merge_sort(
					data.begin(),
					data.end(),
					[&](const item_type& a, const item_type& b) {
						return columns[order.sorted_index].compare(state, container, b, a);
					}
				);
			}
		}
	}
};

struct sort_signal {
	uint8_t column;
};

template<typename item_type>
class sort_button : public ui::button_element_base {
public:
	uint8_t column;
	std::string name;

	sort_button(uint8_t in_column, std::string in_name) {
		column = in_column;
		name = in_name;
	}

	void button_action(sys::state& state) noexcept override {
		sort_signal signal = { };
		signal.column = column;
		send(state, parent, signal);
	}

	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return ui::tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view(name));
		text::close_layout_box(contents, box);
	}
};

template<typename item_type>
struct table_signal {
	uint8_t column;
	uint8_t row;
	std::string text_to_set;
	bool is_hovered;
};

struct table_signal_cell_hover {
	uint8_t column;
	uint8_t row;
};

struct table_signal_cell_click {
	uint8_t column;
	uint8_t row;
};

struct table_signal_scroll {
	float scroll_amount;
};

struct table_has_tooltip_signal {
	bool has_tooltip = false;
	uint8_t column;
};

struct table_tooltip_callback_signal {
	text::columnar_layout* tooltip_layout;
	uint8_t column;
	uint8_t row;
};

template<typename item_type>
class entry : public ui::button_element_base {
public:
	uint8_t column;
	uint8_t row;
	bool is_hovered;

	entry(uint8_t in_row, uint8_t in_column) {
		row = in_row;
		column = in_column;
	}

	void on_update(sys::state& state) noexcept override {
		table_signal<item_type> signal{ };
		signal.column = column;
		signal.row = row;
		auto response = send_and_retrieve<table_signal<item_type>>(state, parent, signal);
		set_button_text(state, response.text_to_set);
	}

	virtual ui::message_result on_mouse_move(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		auto result = ui::button_element_base::on_mouse_move(state, x, y, mods);

		table_signal_cell_hover signal = { };
		signal.column = column;
		signal.row = row;

		send<table_signal_cell_hover>(state, parent, signal);
		return result;
	}

	void button_action(sys::state& state) noexcept override {
		table_signal_cell_click signal = { };
		signal.column = column;
		signal.row = row;

		send<table_signal_cell_click>(state, parent, signal);
	}

	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type t) noexcept override {
		return ui::message_result::consumed;
	}

	ui::message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		table_signal_scroll signal{ };
		signal.scroll_amount = amount;
		send<table_signal_scroll>(state, parent, signal);

		return ui::message_result::consumed;
	}

	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		table_has_tooltip_signal signal = { };
		signal.column = column;
		signal = send_and_retrieve<table_has_tooltip_signal>(state, parent, signal);
		if(signal.has_tooltip) {
			return ui::tooltip_behavior::tooltip;
		} else {
			return ui::tooltip_behavior::no_tooltip;
		}
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		table_tooltip_callback_signal signal { };
		signal.row = row;
		signal.column = column;
		signal.tooltip_layout = &contents;
		send<table_tooltip_callback_signal>(state, parent, signal);
	}
};


template<typename item_type>
class body : public ui::container_base {
protected:
	class _scrollbar : public ui::autoscaling_scrollbar {
	public:
		void on_value_change(sys::state& state, int32_t v) noexcept override {
			static_cast<body*>(parent)->update(state);
		};
	};
public:
	_scrollbar* list_scrollbar = nullptr;
	std::vector<entry<item_type>*> cells{};
	int32_t scroll_pos;
	uint8_t columns;
	uint16_t rows;
	float scroll_impulse;

	body(uint8_t in_columns, uint8_t in_rows) {
		scroll_pos = 0;
		columns = in_columns;
		rows = in_rows;
		scroll_impulse = 0;
	}

	void update(sys::state& state) {
		auto rows_visible = cells.size() / columns;
		auto content_off_screen = int32_t(rows) - int32_t(rows_visible);

		scroll_pos = list_scrollbar->raw_value();
		if(content_off_screen <= 0) {
			list_scrollbar->set_visible(state, false);
			scroll_pos = 0;
		} else {
			list_scrollbar->change_settings(state, ui::mutable_scrollbar_settings{ 0, content_off_screen, 0, 0, false });
			list_scrollbar->set_visible(state, true);
			scroll_pos = std::min((int32_t)scroll_pos, content_off_screen);
		}

		for(size_t current_row = 0; current_row < rows_visible; current_row++) {
			for(uint8_t current_column = 0; current_column < columns; current_column++) {
				auto cell_index = current_row * columns + current_column;
				if(current_row > rows) {
					cells[cell_index]->set_visible(state, false);
				} else {
					cells[cell_index]->set_visible(state, true);
					cells[cell_index]->on_update(state);
				}
			}
		}
	}

	void on_create(sys::state& state) noexcept override {
		ui::container_base::on_create(state);

		auto ptr = ui::make_element_by_type<_scrollbar>(state, "standardlistbox_slider");
		list_scrollbar = static_cast<_scrollbar*>(ptr.get());
		add_child_to_front(std::move(ptr));
		list_scrollbar->scale_to_parent();
		update(state);
	}

	ui::message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<table_signal_scroll>()) {
			table_signal_scroll signal = any_cast<table_signal_scroll>(payload);
			auto amount = signal.scroll_amount;
			auto rows_visible = cells.size() / columns;
			auto content_off_screen = int32_t(rows - rows_visible);
			if(content_off_screen > 0) {	
				// for the sake of smooth scrolling:
				scroll_impulse += amount > 0 ? 1 : -1;
			} else {
				scroll_impulse = 0.f;
			}
		}
		return ui::container_base::get(state, payload);
	}

	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return ui::tooltip_behavior::no_tooltip;
	}
};

template<typename item_type>
class display : public ui::window_element_base {
public:
	body<item_type>* table_body = nullptr;
	std::string body_def;
	data<item_type> content;
	std::vector<uint16_t> widths;
	uint16_t row_height;

	uint8_t hovered_column = 0;
	uint8_t hovered_row = 0;
	bool column_is_hovered = false;

	std::function<void(sys::state& state, ui::element_base* container, const item_type& a)> row_callback;

	display(std::string body_definition, std::vector<column<item_type>> columns) {
		content.columns = columns;
		body_def = body_definition;
		row_callback = [](sys::state& state, ui::element_base* container, const item_type& a) {
			return;
		};
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == body_def) {
			auto ptr_body = ui::make_element_by_type<body<item_type>>(
				state,
				id,
				(uint8_t)content.columns.size(), (uint8_t)0
			);
			table_body = static_cast<body<item_type>*>(ptr_body.get());

			// generate cells
			int16_t offset_y = 0;
			int16_t offset_x = 0;
			row_height = 0;
			uint8_t row = 0;
			while(offset_y + row_height <= base_data.size.y - 40) {
				for(uint8_t column_index = 0; column_index < content.columns.size(); column_index++) {
					auto& column = content.columns[column_index];
					auto ptr = ui::make_element_by_type<entry<item_type>>(
						state,
						column.cell_definition_string,
						row, column_index
					);
					ptr->base_data.position.x += offset_x;
					ptr->base_data.position.y += offset_y;
					table_body->cells.push_back(static_cast<entry<item_type>*>(ptr.get()));
					offset_x += ptr->base_data.size.x;
					row_height = std::max<uint16_t>(row_height, (uint16_t)(ptr->base_data.size.y));
					table_body->add_child_to_front(std::move(ptr));
				}

				offset_x = 0;
				offset_y = offset_y + row_height;
				row++;
			}
			return ptr_body;
		}
		return nullptr;
	}

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		int16_t offset_x = 0;
		for(uint8_t column_index = 0; column_index < content.columns.size(); column_index++) {
			auto& column = content.columns[column_index];
			auto def = state.ui_state.defs_by_name.find(
				state.lookup_key(column.header_definition_string)
			)->second.definition;
			auto ptr = ui::make_element_by_type<sort_button<item_type>>(
				state,
				def,
				column_index,
				column.header
			);
			ptr->base_data.position.x += offset_x;
			ptr->set_button_text(state, text::produce_simple_string(state, column.header));

			widths.push_back(ptr->base_data.size.x);
			offset_x += ptr->base_data.size.x;
			add_child_to_front(std::move(ptr));
		}
	}

	ui::message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<table_signal<item_type>>()) {
			table_signal<item_type> signal = any_cast<table_signal<item_type>>(payload);
			auto index = signal.column;
			if((uint16_t)signal.row + (uint16_t)(table_body->scroll_pos) < (uint16_t)content.data.size()) {
				auto raw_data = content.data[signal.row + table_body->scroll_pos];
				signal.text_to_set = content.columns[index].view(state, this, raw_data);
			} else {
				signal.text_to_set = "???";
			}

			if(column_is_hovered) {
				if(signal.column == hovered_column) {
					signal.is_hovered = true;
				}
			}

			payload.emplace<table_signal<item_type>>(signal);
			return ui::message_result::consumed;
		} else if(payload.holds_type<sort_signal>()) {
			sort_signal signal = any_cast<sort_signal>(payload);
			content.toggle_sort_column_by_index(signal.column);
			on_update(state);
			return ui::message_result::consumed;
		} else if(payload.holds_type<table_signal_cell_hover>()) {
			table_signal_cell_hover signal = any_cast<table_signal_cell_hover>(payload);
			hovered_column = signal.column;
			hovered_row = signal.row;
			return ui::message_result::consumed;
		} else if(payload.holds_type<table_signal_cell_click>()) {
			table_signal_cell_click signal = any_cast<table_signal_cell_click>(payload);
			if((uint16_t)signal.row + (uint16_t)(table_body->scroll_pos) < (uint16_t)content.data.size()) {
				row_callback(state, this, content.data[signal.row + table_body->scroll_pos]);
			}
			return ui::message_result::consumed;
		} else if(payload.holds_type<table_has_tooltip_signal>()) {
			table_has_tooltip_signal signal = any_cast<table_has_tooltip_signal>(payload);
			signal.has_tooltip = content.columns[signal.column].has_tooltip;
			payload.emplace<table_has_tooltip_signal>(signal);
			return ui::message_result::consumed;
		} else if(payload.holds_type<table_tooltip_callback_signal>()) {
			table_tooltip_callback_signal signal = any_cast<table_tooltip_callback_signal>(payload);
			content.columns[signal.column].update_tooltip(
				state,
				*(signal.tooltip_layout),
				content.data[signal.row],
				content.columns[signal.column].header
			);
			return ui::message_result::consumed;
		}
		return ui::container_base::get(state, payload);
	}

	void on_update(sys::state& state) noexcept override {
		content.update_rows_order(state, this);
		if(table_body != nullptr) {
			table_body->rows = (uint16_t)content.data.size();
			table_body->update(state);
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		//smooth scolling
		constexpr float dt = 0.99f;

		float impulse = table_body->scroll_impulse;
		if(impulse > dt) {
			table_body->scroll_impulse -= dt;
		} else if (impulse < -dt) {
			table_body->scroll_impulse += dt;
		} else {
			table_body->scroll_impulse = 0;
		}

		// if we jumped to another step: make a step
		if(std::floor(impulse) != std::floor(table_body->scroll_impulse)) {
			table_body->list_scrollbar->update_raw_value(state, table_body->list_scrollbar->raw_value() + (impulse < 0 ? 1 : -1));
			state.ui_state.last_tooltip = nullptr; //force update of tooltip
			table_body->update(state);
		}


		float tinted_column_x = (float)x;
		for(uint8_t i = 0; i < hovered_column; i++) {
			float width = (float)(widths[i]);
			tinted_column_x += width;
		}
		float tinted_column_width = (float)(widths[hovered_column]);

		float tinted_column_y = (float)y;
		float tinted_column_height = (float)(base_data.size.y);

		auto rtl = state.world.locale_get_native_rtl(
			state.font_collection.get_current_locale()
		);

		ogl::render_tinted_rect(state,
			tinted_column_x, tinted_column_y,
			tinted_column_width, tinted_column_height,
			0.9f, 0.8f, 0.8f,
			base_data.get_rotation(), false,
			rtl
		);

		float hovered_row_x = float(x);
		float hovered_row_width = float(base_data.size.x);

		float hovered_row_height = float(row_height);
		float hovered_row_y = float(y) + float(table_body->base_data.position.y) + row_height * hovered_row;

		ogl::render_tinted_rect(state,
			hovered_row_x, hovered_row_y,
			hovered_row_width, hovered_row_height,
			0.9f, 0.8f, 0.8f,
			base_data.get_rotation(), false,
			rtl
		);

		ui::window_element_base::render(state, x, y);
	}
};

}
