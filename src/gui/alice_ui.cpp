#include "system_state.hpp"
#include "gui_element_base.hpp"
#include "gui_element_types.hpp"
#include "gui_deserialize.hpp"
#include "alice_ui.hpp"
#include "demographics.hpp"

#include "macrobuilder2.cpp"
#include "budgetwindow.cpp"
#include "demographicswindow.cpp"

namespace alice_ui {


ogl::animation::type to_ogl_type(animation_type type, bool forwards) {
	switch(type) {
	case animation_type::page_left:
		return forwards ? ogl::animation::type::page_flip_left : ogl::animation::type::page_flip_left_rev;
	case animation_type::page_right:
		return forwards ? ogl::animation::type::page_flip_right : ogl::animation::type::page_flip_right_rev;
	case animation_type::page_up:
		return forwards ? ogl::animation::type::page_flip_up : ogl::animation::type::page_flip_up_rev;
	case animation_type::none:
		return ogl::animation::type::page_flip_left;
	default:
		return ogl::animation::type::page_flip_left;
	}
}

void layout_level::change_page(sys::state& state, layout_window_element& container, int32_t new_page) {
	if(page_animation != animation_type::none) {
		bool lflip = new_page < current_page && current_page > 0;
		bool rflip = new_page > current_page && current_page < int32_t(page_starts.size() - 1);
		if(rflip) {
			auto pos = ui::get_absolute_location(state, container);
			state.ui_animation.start_animation(state, pos.x + resolved_x_pos, pos.y + resolved_y_pos, resolved_x_size, resolved_y_size, to_ogl_type(page_animation, true), 200);
		} else if(lflip) {
			auto pos = ui::get_absolute_location(state, container);
			state.ui_animation.start_animation(state, pos.x + resolved_x_pos, pos.y + resolved_y_pos, resolved_x_size, resolved_y_size, to_ogl_type(page_animation, false), 200);
		}

		current_page = int16_t(std::clamp(new_page, 0, int32_t(page_starts.size() - 1)));

		if(rflip || lflip) {
			container.remake_layout(state, false);
			state.ui_animation.post_update_frame(state);
		}
	} else if(new_page != current_page) {
		current_page = int16_t(std::clamp(new_page, 0, int32_t(page_starts.size() - 1)));
		container.remake_layout(state, false);
	}
}

bool pop_passes_filter(sys::state& state, dcon::pop_id p) {
	if(popwindow::excluded_cultures.contains(state.world.pop_get_culture(p).id.index()))
		return false;
	if(popwindow::excluded_types.contains(state.world.pop_get_poptype(p).id.index()))
		return false;
	if(popwindow::excluded_religions.contains(state.world.pop_get_religion(p).id.index()))
		return false;
	if(popwindow::excluded_states.contains(state.world.province_get_state_membership(state.world.pop_get_province_from_pop_location(p)).id.index()))
		return false;
	if(!popwindow::show_colonial && state.world.province_get_is_colonial(state.world.pop_get_province_from_pop_location(p)))
		return false;
	if(!popwindow::show_non_colonial && !state.world.province_get_is_colonial(state.world.pop_get_province_from_pop_location(p)))
		return false;
	return true;
}


void page_buttons::render(sys::state& state, int32_t x, int32_t y) noexcept {
	int16_t p = for_layout->current_page;
	int16_t z = int16_t(for_layout->page_starts.size());

	if(p != last_page || z != last_size) {
		text_layout.contents.clear();
		text_layout.number_of_lines = 0;
		last_page = p;
		last_size = z;
		if(z > 1) {
			std::string display = std::to_string(p + 1) + "/" + std::to_string(z);
			text::single_line_layout sl{ text_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, base_data.size.y), 0, text::alignment::center, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
			sl.add_text(state, display);
		}
	}

	if(last_size <= 1)
		return;

	auto fh = text::make_font_id(state, false, base_data.size.y);
	auto linesz = state.font_collection.line_height(state, fh);
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false);
	for(auto& t : text_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)), fh, ui::get_text_color(state, ((layout_window_element*)parent)->page_text_color), cmod);
	}

	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;

	// buttons, left
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && rel_mouse_x <= base_data.size.y, for_layout->current_page == 0, true), float(x), float(y), float(base_data.size.y), float(base_data.size.y), ogl::get_late_load_texture_handle(state, ((layout_window_element*)parent)->page_left_texture_id, ((layout_window_element*)parent)->page_left_texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));

	// right
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && (base_data.size.x - base_data.size.y) <= rel_mouse_x, for_layout->current_page >= int32_t(for_layout->page_starts.size()) - 1, true), float(x + base_data.size.x - base_data.size.y), float(y), float(base_data.size.y), float(base_data.size.y), ogl::get_late_load_texture_handle(state, ((layout_window_element*)parent)->page_right_texture_id, ((layout_window_element*)parent)->page_right_texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
ui::message_result page_buttons::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(last_size <= 1)
		return ui::message_result::unseen;

	if(x <= base_data.size.y) { // left button
		if(for_layout->current_page > 0) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			for_layout->change_page(state, *((layout_window_element*)parent), for_layout->current_page - 1);
		}
	} else if(base_data.size.x - base_data.size.y <= x) { // right button
		if(for_layout->current_page < int32_t(for_layout->page_starts.size()) - 1) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			for_layout->change_page(state, *((layout_window_element*)parent), for_layout->current_page + 1);
		}
	}

	return ui::message_result::consumed;
}
void page_buttons::on_update(sys::state& state) noexcept {
	
}

layout_level* innermost_scroll_level(layout_level& current, int32_t x, int32_t y) {
	if(x < current.resolved_x_pos || y < current.resolved_y_pos || current.resolved_x_pos + current.resolved_x_size < x || current.resolved_y_pos + current.resolved_y_size < y)
		return nullptr;

	for(auto& m : current.contents) {
		if(std::holds_alternative< sub_layout>(m)) {
			auto r = innermost_scroll_level(*std::get<sub_layout>(m).layout, x, y);
			if(r)
				return r;
		}
	}

	if(current.paged)
		return &current;

	return nullptr;
}

ui::message_result layout_window_element::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	auto sub_layout = innermost_scroll_level(layout, x, y);
	if(sub_layout) {
		sub_layout->change_page(state, *this, sub_layout->current_page + ((amount < 0) ? 1 : -1));
	}
	return ui::message_result::consumed;
}


struct layout_iterator {
	std::vector<layout_item>& backing;
	int32_t index = 0;
	int32_t sub_index = 0;

	layout_iterator(std::vector<layout_item>& backing) : backing(backing) {
	}

	bool current_is_glue() {
		return has_more() && std::holds_alternative<layout_glue>(backing[index]);
	}
	measure_result measure_current(sys::state& state, bool glue_horizontal, int32_t max_crosswise, bool first_in_section) {
		if(!has_more())
			return measure_result{ 0, 0, measure_result::special::none };
		auto& m = backing[index];

		if(std::holds_alternative<layout_control>(m)) {
			auto& i = std::get<layout_control>(m);
			if(i.absolute_position) {
				return  measure_result{ 0, 0, measure_result::special::none };
			}
			return  measure_result{ i.ptr->base_data.size.x, i.ptr->base_data.size.y, measure_result::special::none };
		} else if(std::holds_alternative<layout_window>(m)) {
			auto& i = std::get<layout_window>(m);
			if(i.absolute_position) {
				return  measure_result{ 0, 0, measure_result::special::none };
			}
			return  measure_result{ i.ptr->base_data.size.x, i.ptr->base_data.size.y, measure_result::special::none };
		} else if(std::holds_alternative<layout_glue>(m)) {
			auto& i = std::get<layout_glue>(m);
			if(glue_horizontal) {
				switch(i.type) {
				case glue_type::standard: return measure_result{ i.amount, 0, measure_result::special::none };
				case glue_type::at_least: return measure_result{ i.amount, 0, measure_result::special::space_consumer };
				case glue_type::line_break: return measure_result{ 0, 0, measure_result::special::end_line };
				case glue_type::page_break: return measure_result{ 0, 0, measure_result::special::end_page };
				case glue_type::glue_don_t_break: return measure_result{ i.amount, 0, measure_result::special::no_break };
				}
			} else {
				switch(i.type) {
				case glue_type::standard: return measure_result{ 0, i.amount, measure_result::special::none };
				case glue_type::at_least: return measure_result{ 0, i.amount, measure_result::special::space_consumer };
				case glue_type::line_break: return measure_result{ 0, 0, measure_result::special::end_line };
				case glue_type::page_break: return measure_result{ 0, 0, measure_result::special::end_page };
				case glue_type::glue_don_t_break: return measure_result{ 0, i.amount, measure_result::special::no_break };
				}
			}
		} else if(std::holds_alternative<generator_instance>(m)) {
			auto& i = std::get<generator_instance>(m);
			bool alt = false;
			return i.generator->place_item(state, nullptr, size_t(sub_index), 0, 0, first_in_section, alt);
		} else if(std::holds_alternative<sub_layout>(m)) {
			auto& i = std::get<sub_layout>(m);
			int32_t x = 0;
			int32_t y = 0;
			bool consume_fill = false;
			if(i.layout->size_x != -1)
				x = i.layout->size_x;
			else {
				if(glue_horizontal)
					consume_fill = true;
				else
					x = max_crosswise;
			}
			if(i.layout->size_y != -1)
				y = i.layout->size_y;
			else {
				if(!glue_horizontal)
					consume_fill = true;
				else
					y = max_crosswise;
			}

			return measure_result{ x, y, consume_fill ? measure_result::special::space_consumer : measure_result::special::none };
		}
		return measure_result{ 0, 0, measure_result::special::none };
	}
	void place_current(sys::state& state, layout_window_element* destination, bool first_in_section, bool& alternate, int32_t x, int32_t y, int32_t lsz_x, int32_t lsz_y, bool remake_lists) {
		if(!has_more())
			return;
		auto& m = backing[index];

		if(std::holds_alternative<layout_control>(m)) {
			auto& i = std::get<layout_control>(m);
			i.ptr->base_data.position.x = int16_t(x);
			i.ptr->base_data.position.y = int16_t(y);
			destination->children.push_back(i.ptr);
		} else if(std::holds_alternative<layout_window>(m)) {
			auto& i = std::get<layout_window>(m);
			i.ptr->base_data.position.x = int16_t(x);
			i.ptr->base_data.position.y = int16_t(y);
			destination->children.push_back(i.ptr.get());
		} else if(std::holds_alternative<layout_glue>(m)) {

		} else if(std::holds_alternative<generator_instance>(m)) {
			auto& i = std::get<generator_instance>(m);
			i.generator->place_item(state, destination, size_t(sub_index), x, y, first_in_section, alternate);
		} else if(std::holds_alternative<sub_layout>(m)) {
			auto& i = std::get<sub_layout>(m);
			destination->remake_layout_internal(*i.layout, state, x, y, lsz_x, lsz_y, remake_lists);
		}
	}
	void move_position(int32_t n) {
		while(n > 0 && has_more()) {
			if(std::holds_alternative<generator_instance>(backing[index])) {
				auto& g = std::get<generator_instance>(backing[index]);
				++sub_index;
				--n;
				if(sub_index >= int32_t(g.generator->item_count())) {
					sub_index = 0;
					++index;
				}
			} else {
				++index;
				--n;
			}
		}
		while(n < 0 && index >= 0) {
			if(std::holds_alternative<generator_instance>(backing[index])) {
				auto& g = std::get<generator_instance>(backing[index]);
				--sub_index;
				++n;
				if(sub_index < 0) {
					sub_index = 0;
					--index;
				} else {
					continue; // to avoid resetting sub index
				}
			} else {
				--index;
				if(index < 0) {
					index = 0; return;
				}
				++n;
			}

			if(index < 0) {
				index = 0; return;
			}
			if(std::holds_alternative<generator_instance>(backing[index])) {
				auto& g = std::get<generator_instance>(backing[index]);
				sub_index = std::max(int32_t(g.generator->item_count()) - 1, 0);
			}
		}
	}
	bool has_more() {
		return index < int32_t(backing.size());
	}
	void reset() {
		index = 0;
		sub_index = 0;
	}
};

void layout_window_element::remake_layout_internal(layout_level& lvl, sys::state& state, int32_t x, int32_t y, int32_t w, int32_t h, bool remake_lists) {
	auto base_x_size = lvl.size_x != -1 ? int32_t(lvl.size_x) : w;
	auto base_y_size = lvl.size_y != -1 ? int32_t(lvl.size_y) : h;
	auto top_margin = int32_t(lvl.margin_top);
	auto bottom_margin = lvl.margin_bottom != -1 ? int32_t(lvl.margin_bottom) : top_margin;
	auto left_margin = lvl.margin_left != -1 ? int32_t(lvl.margin_left) : bottom_margin;
	auto right_margin = lvl.margin_right != -1 ? int32_t(lvl.margin_right) : left_margin;
	auto effective_x_size = base_x_size - (left_margin + right_margin);
	auto effective_y_size = base_y_size - (top_margin + bottom_margin);
	if(lvl.paged) {
		effective_y_size -= lvl.page_controls->base_data.size.y;
		lvl.page_controls->base_data.position.x = int16_t(x + left_margin + (effective_x_size / 2) - (lvl.page_controls->base_data.size.x / 2));
		lvl.page_controls->base_data.position.y = int16_t(y + top_margin + effective_y_size);
		children.push_back(lvl.page_controls.get());
	}

	lvl.resolved_x_pos = int16_t(x + left_margin);
	lvl.resolved_y_pos = int16_t(y + top_margin);
	lvl.resolved_x_size = int16_t(effective_x_size);
	lvl.resolved_y_size = int16_t(effective_y_size);

	if(remake_lists) {
		switch(lvl.type) {
		case layout_type::single_horizontal:
		{
			layout_iterator it(lvl.contents);

			// measure loop
			lvl.page_starts.clear();

			while(it.has_more()) {
				bool page_first = true;
				int32_t space_used = 0;
				int32_t fill_consumer_count = 0;

				while(it.has_more()) {
					auto mr = it.measure_current(state, true, effective_y_size, page_first);
					if(!page_first && lvl.paged && (space_used + mr.x_space > effective_x_size)) {
						//check if previous was glue, and erase
						if(it.index != 0) {
							it.move_position(-1);
							if(it.current_is_glue()) {
								space_used -= it.measure_current(state, true, effective_y_size, page_first).x_space;
							}
							it.move_position(1);
						}
						while(it.has_more() && it.current_is_glue()) {
							it.move_position(1);
						}

						lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(0) });
						page_first = true;

						break; // only layout one page
					}
					page_first = false;
					if(mr.other == measure_result::special::space_consumer)
						++fill_consumer_count;
					if(lvl.paged && mr.other == measure_result::special::end_page) {
						it.move_position(1);
						while(it.has_more() && it.current_is_glue()) {
							it.move_position(1);
						}
						lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(0) });
						page_first = true;
						break; // only layout one page
					}

					space_used += mr.x_space;
					it.move_position(1);
				}

				if(page_first == false || !it.has_more()) {
					lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(0) });
				}
			}

		} break;
		case layout_type::single_vertical:
		{
			layout_iterator it(lvl.contents);

			// measure loop
			lvl.page_starts.clear();

			while(it.has_more()) {
				bool page_first = true;
				int32_t space_used = 0;
				int32_t fill_consumer_count = 0;

				while(it.has_more()) {
					auto mr = it.measure_current(state, false, effective_x_size, page_first);
					if(!page_first && lvl.paged && (space_used + mr.y_space > effective_y_size)) {
						//check if previous was glue, and erase
						if(it.index != 0) {
							it.move_position(-1);
							if(it.current_is_glue()) {
								space_used -= it.measure_current(state, false, effective_x_size, page_first).y_space;
							}
							it.move_position(1);
						}
						while(it.has_more() && it.current_is_glue()) {
							it.move_position(1);
						}

						lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(0) });
						page_first = true;

						break; // only layout one page
					}
					page_first = false;
					if(mr.other == measure_result::special::space_consumer)
						++fill_consumer_count;
					if(lvl.paged && mr.other == measure_result::special::end_page) {
						it.move_position(1);
						while(it.has_more() && it.current_is_glue()) {
							it.move_position(1);
						}
						lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(0) });
						page_first = true;
						break; // only layout one page
					}

					space_used += mr.y_space;
					it.move_position(1);
				}

				if(page_first == false || !it.has_more()) {
					lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(0) });
				}
			}
		} break;
		case layout_type::overlapped_horizontal:
		{
			layout_iterator it(lvl.contents);

			// measure loop
			lvl.page_starts.clear();

			while(it.has_more()) {
				bool page_first = true;
				int32_t space_used = 0;
				int32_t fill_consumer_count = 0;
				int32_t non_glue_count = 0;

				while(it.has_more()) {
					auto mr = it.measure_current(state, true, effective_y_size, page_first);
					if(!page_first && lvl.paged && (space_used + mr.x_space > effective_x_size)) {
						//check if previous was glue, and erase
						if(it.index != 0) {
							it.move_position(-1);
							if(it.current_is_glue()) {
								space_used -= it.measure_current(state, true, effective_y_size, page_first).x_space;
							}
							it.move_position(1);
						}
						while(it.has_more() && it.current_is_glue()) {
							it.move_position(1);
						}

						lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(non_glue_count) });
						page_first = true;

						break; // only layout one page
					}
					page_first = false;
					if(mr.other == measure_result::special::space_consumer)
						++fill_consumer_count;

					if(lvl.paged && mr.other == measure_result::special::end_page) {
						it.move_position(1);
						while(it.has_more() && it.current_is_glue()) {
							it.move_position(1);
						}
						lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(non_glue_count) });
						page_first = true;
						break; // only layout one page
					}
					if(it.current_is_glue() || (std::holds_alternative<layout_control>(it.backing[it.index]) && std::get<layout_control>(it.backing[it.index]).absolute_position)
						|| (std::holds_alternative<layout_window>(it.backing[it.index]) && std::get<layout_window>(it.backing[it.index]).absolute_position)) {

					} else {
						++non_glue_count;
					}

					space_used += mr.x_space;
					it.move_position(1);
				}

				if(page_first == false || !it.has_more()) {
					lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(non_glue_count) });
				}
			}
		} break;
		case layout_type::overlapped_vertical:
		{
			layout_iterator it(lvl.contents);

			// measure loop
			lvl.page_starts.clear();

			while(it.has_more()) {
				bool page_first = true;
				int32_t space_used = 0;
				int32_t fill_consumer_count = 0;
				int32_t non_glue_count = 0;

				while(it.has_more()) {
					auto mr = it.measure_current(state, false, effective_x_size, page_first);
					if(!page_first && lvl.paged && (space_used + mr.y_space > effective_y_size)) {
						//check if previous was glue, and erase
						if(it.index != 0) {
							it.move_position(-1);
							if(it.current_is_glue()) {
								space_used -= it.measure_current(state, false, effective_x_size, page_first).y_space;
							}
							it.move_position(1);
						}
						while(it.has_more() && it.current_is_glue()) {
							it.move_position(1);
						}

						lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(non_glue_count) });
						page_first = true;

						break; // only layout one page
					}
					page_first = false;
					if(mr.other == measure_result::special::space_consumer)
						++fill_consumer_count;

					if(lvl.paged && mr.other == measure_result::special::end_page) {
						it.move_position(1);
						while(it.has_more() && it.current_is_glue()) {
							it.move_position(1);
						}
						lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(non_glue_count) });
						page_first = true;
						break; // only layout one page
					}
					if(it.current_is_glue() || (std::holds_alternative<layout_control>(it.backing[it.index]) && std::get<layout_control>(it.backing[it.index]).absolute_position)
						|| (std::holds_alternative<layout_window>(it.backing[it.index]) && std::get<layout_window>(it.backing[it.index]).absolute_position)) {

					} else {
						++non_glue_count;
					}

					space_used += mr.y_space;
					it.move_position(1);
				}

				if(page_first == false || !it.has_more()) {
					lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(space_used), int16_t(fill_consumer_count), int16_t(non_glue_count) });
				}
			}
		} break;
		case layout_type::mulitline_horizontal:
		{
			layout_iterator it(lvl.contents);

			// measure loop
			lvl.page_starts.clear();

			while(it.has_more()) { // pages
				int32_t total_crosswise = 0;
				bool page_first = true;
				bool page_done = false;

				while(it.has_more() && !page_done) { // lines
					bool col_first = true;
					int32_t max_crosswise = 0;
					int32_t space_used = 0;

					while(it.has_more()) { // line content
						auto mr = it.measure_current(state, true, effective_y_size - total_crosswise, col_first);
						if(!page_first && lvl.paged && total_crosswise + mr.y_space > effective_y_size) {
							while(it.has_more() && it.current_is_glue()) {
								it.move_position(1);
							}
							lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(0), int16_t(0), int16_t(0) });

							page_done = true;
							break; // end page
						} else if(!col_first && lvl.paged && (space_used + mr.x_space > effective_x_size)) {
							//check if previous was glue, and erase
							if(it.index != 0) {
								it.move_position(-1);
								if(it.current_is_glue()) {
									space_used -= it.measure_current(state, true, effective_y_size - total_crosswise, col_first).x_space;
								}
								it.move_position(1);
							}
							while(it.has_more() && it.current_is_glue()) {
								it.move_position(1);
							}
							break; // end line
						}

						col_first = false;
						page_first = false;

						if(mr.other == measure_result::special::end_line) {
							it.move_position(1);
							while(it.has_more() && it.current_is_glue()) {
								it.move_position(1);
							}
							space_used += mr.x_space;
							max_crosswise = std::max(max_crosswise, mr.y_space);
							break; // end line
						} else if(lvl.paged && mr.other == measure_result::special::end_page) {
							it.move_position(1);
							while(it.has_more() && it.current_is_glue()) {
								it.move_position(1);
							}
							lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(0), int16_t(0), int16_t(0) });
							page_done = true;
							break; // end page
						}

						space_used += mr.x_space;
						max_crosswise = std::max(max_crosswise, mr.y_space);
						it.move_position(1);
					} // end single line loop

					if(!page_done)
						total_crosswise += (max_crosswise + lvl.interline_spacing);
					else
						total_crosswise = 0;
				} // end lines loop

				if(page_done == false || !it.has_more()) {
					lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(0), int16_t(0), int16_t(0) });
				}
			}
		} break;
		case layout_type::multiline_vertical:
		{
			layout_iterator it(lvl.contents);

			// measure loop
			lvl.page_starts.clear();

			while(it.has_more()) { // pages
				int32_t total_crosswise = 0;
				bool page_first = true;
				bool page_done = false;

				while(it.has_more() && !page_done) { // lines
					bool col_first = true;
					int32_t max_crosswise = 0;
					int32_t space_used = 0;

					while(it.has_more()) { // line content
						auto mr = it.measure_current(state, false, effective_x_size - total_crosswise, col_first);
						if(!page_first && lvl.paged && total_crosswise + mr.x_space > effective_x_size) {
							while(it.has_more() && it.current_is_glue()) {
								it.move_position(1);
							}
							lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(0), int16_t(0), int16_t(0) });

							page_done = true;
							break; // end page
						} else if(!col_first && lvl.paged && (space_used + mr.y_space > effective_y_size)) {
							//check if previous was glue, and erase
							if(it.index != 0) {
								it.move_position(-1);
								if(it.current_is_glue()) {
									space_used -= it.measure_current(state, false, effective_x_size - total_crosswise, col_first).y_space;
								}
								it.move_position(1);
							}
							while(it.has_more() && it.current_is_glue()) {
								it.move_position(1);
							}
							break; // end line
						}

						col_first = false;
						page_first = false;

						if(mr.other == measure_result::special::end_line) {
							it.move_position(1);
							while(it.has_more() && it.current_is_glue()) {
								it.move_position(1);
							}
							space_used += mr.y_space;
							max_crosswise = std::max(max_crosswise, mr.x_space);
							break; // end line
						} else if(lvl.paged && mr.other == measure_result::special::end_page) {
							it.move_position(1);
							while(it.has_more() && it.current_is_glue()) {
								it.move_position(1);
							}
							lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(0), int16_t(0), int16_t(0) });
							page_done = true;
							break; // end page
						}

						space_used += mr.y_space;
						max_crosswise = std::max(max_crosswise, mr.x_space);
						it.move_position(1);
					} // end single line loop

					if(!page_done)
						total_crosswise += (max_crosswise + lvl.interline_spacing);
					else
						total_crosswise = 0;
				} // end lines loop

				if(page_done == false || !it.has_more()) {
					lvl.page_starts.push_back(page_info{ int16_t(it.index), int16_t(it.sub_index), int16_t(0), int16_t(0), int16_t(0) });
				}
			}
		} break;
		}
	} // end: remake lists

	for(auto& m : lvl.contents) {
		if(holds_alternative<generator_instance>(m)) {
			auto& i = std::get<generator_instance>(m);
			i.generator->reset_pools();
		}
	}

	switch(lvl.type) {
	case layout_type::single_horizontal:
	{
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));

		layout_iterator place_it(lvl.contents);
		if(lvl.current_page > 0) {
			place_it.index = lvl.page_starts[lvl.current_page - 1].last_index;
			place_it.sub_index = lvl.page_starts[lvl.current_page - 1].last_sub_index;
		}
		int32_t space_used = lvl.page_starts[lvl.current_page].space_used;
		int32_t fill_consumer_count = lvl.page_starts[lvl.current_page].space_consumer_count;
		// place / render

		int32_t extra_runlength = int32_t(effective_x_size - space_used);
		int32_t per_fill_consumer = fill_consumer_count != 0 ? (extra_runlength / fill_consumer_count) : 0;
		int32_t extra_lead = 0;
		switch(lvl.line_alignment) {
		case layout_line_alignment::leading: break;
		case layout_line_alignment::trailing: extra_lead = extra_runlength - fill_consumer_count * per_fill_consumer; break;
		case layout_line_alignment::centered: extra_lead = (extra_runlength - fill_consumer_count * per_fill_consumer) / 2;  break;
		}
		space_used = x + extra_lead + left_margin;
		bool page_first = true;
		bool alternate = true;
		while(place_it.has_more() && (!lvl.paged || place_it.index < lvl.page_starts[lvl.current_page].last_index || (place_it.index == lvl.page_starts[lvl.current_page].last_index && place_it.sub_index < lvl.page_starts[lvl.current_page].last_sub_index))) {
			auto mr = place_it.measure_current(state, true, effective_y_size, page_first);
			int32_t yoff = 0;
			int32_t xoff = space_used;
			switch(lvl.line_internal_alignment) {
			case layout_line_alignment::leading: yoff = y + top_margin; break;
			case layout_line_alignment::trailing: yoff = y + top_margin + effective_y_size - mr.y_space; break;
			case layout_line_alignment::centered: yoff = y + top_margin + (effective_y_size - mr.y_space) / 2;  break;
			}
			if(std::holds_alternative< layout_control>(lvl.contents[place_it.index])) {
				auto& i = std::get<layout_control>(lvl.contents[place_it.index]);
				if(i.absolute_position) {
					xoff = x + i.abs_x;
					yoff = y + i.abs_y;
				}
			} else if(std::holds_alternative<layout_window>(lvl.contents[place_it.index])) {
				auto& i = std::get<layout_window>(lvl.contents[place_it.index]);
				if(i.absolute_position) {
					xoff = x + i.abs_x;
					yoff = y + i.abs_y;
				}
			}
			place_it.place_current(state, this, page_first, alternate, xoff, yoff, mr.x_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), mr.y_space, remake_lists);

			if(!place_it.current_is_glue()) {
				page_first = false;
			}

			space_used += mr.x_space;
			if(mr.other == measure_result::special::space_consumer) {
				space_used += per_fill_consumer;
			}
			place_it.move_position(1);
		}
	} break;
	case layout_type::single_vertical:
	{
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));

		layout_iterator place_it(lvl.contents);
		if(lvl.current_page > 0) {
			place_it.index = lvl.page_starts[lvl.current_page - 1].last_index;
			place_it.sub_index = lvl.page_starts[lvl.current_page - 1].last_sub_index;
		}
		int32_t space_used = lvl.page_starts[lvl.current_page].space_used;
		int32_t fill_consumer_count = lvl.page_starts[lvl.current_page].space_consumer_count;
		// place / render

		int32_t extra_runlength = int32_t(effective_y_size - space_used);
		int32_t per_fill_consumer = fill_consumer_count != 0 ? (extra_runlength / fill_consumer_count) : 0;
		int32_t extra_lead = 0;
		switch(lvl.line_alignment) {
		case layout_line_alignment::leading: break;
		case layout_line_alignment::trailing: extra_lead = extra_runlength - fill_consumer_count * per_fill_consumer; break;
		case layout_line_alignment::centered: extra_lead = (extra_runlength - fill_consumer_count * per_fill_consumer) / 2;  break;
		}
		space_used = y + extra_lead + top_margin;
		bool page_first = true;
		bool alternate = true;
		while(place_it.has_more() && (!lvl.paged || place_it.index < lvl.page_starts[lvl.current_page].last_index || (place_it.index == lvl.page_starts[lvl.current_page].last_index && place_it.sub_index < lvl.page_starts[lvl.current_page].last_sub_index))) {
			auto mr = place_it.measure_current(state, false, effective_x_size, page_first);
			int32_t xoff = 0;
			int32_t yoff = space_used;
			switch(lvl.line_internal_alignment) {
			case layout_line_alignment::leading: xoff = x + left_margin; break;
			case layout_line_alignment::trailing: xoff = x + left_margin + effective_x_size - mr.x_space; break;
			case layout_line_alignment::centered: xoff = x + left_margin + (effective_x_size - mr.x_space) / 2;  break;
			}
			if(std::holds_alternative< layout_control>(lvl.contents[place_it.index])) {
				auto& i = std::get<layout_control>(lvl.contents[place_it.index]);
				if(i.absolute_position) {
					xoff = x + i.abs_x;
					yoff = y + i.abs_y;
				}
			} else if(std::holds_alternative<layout_window>(lvl.contents[place_it.index])) {
				auto& i = std::get<layout_window>(lvl.contents[place_it.index]);
				if(i.absolute_position) {
					xoff = x + i.abs_x;
					yoff = y + i.abs_y;
				}
			}
			place_it.place_current(state, this, page_first, alternate, xoff, yoff, mr.x_space, mr.y_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), remake_lists);

			if(!place_it.current_is_glue()) {
				page_first = false;
			}

			space_used += mr.y_space;
			if(mr.other == measure_result::special::space_consumer) {
				space_used += per_fill_consumer;
			}
			place_it.move_position(1);
		}
	} break;
	case layout_type::overlapped_horizontal:
	{
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));

		layout_iterator place_it(lvl.contents);
		if(lvl.current_page > 0) {
			place_it.index = lvl.page_starts[lvl.current_page - 1].last_index;
			place_it.sub_index = lvl.page_starts[lvl.current_page - 1].last_sub_index;
		}
		int32_t space_used = lvl.page_starts[lvl.current_page].space_used;
		int32_t fill_consumer_count = lvl.page_starts[lvl.current_page ].space_consumer_count;
		int32_t non_glue_count = lvl.page_starts[lvl.current_page ].non_glue_count;

		int32_t extra_runlength = std::max(0, int32_t(effective_x_size - space_used));
		int32_t per_fill_consumer = fill_consumer_count != 0 ? (extra_runlength / fill_consumer_count) : 0;
		int32_t extra_lead = 0;
		switch(lvl.line_alignment) {
		case layout_line_alignment::leading: break;
		case layout_line_alignment::trailing: extra_lead = extra_runlength - fill_consumer_count * per_fill_consumer; break;
		case layout_line_alignment::centered: extra_lead = (extra_runlength - fill_consumer_count * per_fill_consumer) / 2;  break;
		}
		int32_t overlap_subtraction = (non_glue_count > 1 && space_used > effective_x_size) ? int32_t(space_used - effective_x_size) / (non_glue_count - 1) : 0;
		space_used = x + extra_lead + left_margin;

		bool page_first = true;
		bool alternate = true;
		while(place_it.has_more() && (!lvl.paged || place_it.index < lvl.page_starts[lvl.current_page].last_index || (place_it.index == lvl.page_starts[lvl.current_page].last_index && place_it.sub_index < lvl.page_starts[lvl.current_page].last_sub_index))) {
			auto mr = place_it.measure_current(state, true, effective_y_size, page_first);
			int32_t yoff = 0;
			int32_t xoff = space_used;
			switch(lvl.line_internal_alignment) {
			case layout_line_alignment::leading: yoff = y + top_margin; break;
			case layout_line_alignment::trailing: yoff = y + top_margin + effective_y_size - mr.y_space; break;
			case layout_line_alignment::centered: yoff = y + top_margin + (effective_y_size - mr.y_space) / 2;  break;
			}
			bool was_abs = false;
			if(std::holds_alternative< layout_control>(lvl.contents[place_it.index])) {
				auto& i = std::get<layout_control>(lvl.contents[place_it.index]);
				if(i.absolute_position) {
					xoff = x + i.abs_x;
					yoff = y + i.abs_y;
					was_abs = true;
				}
			} else if(std::holds_alternative< layout_window>(lvl.contents[place_it.index])) {
				auto& i = std::get<layout_window>(lvl.contents[place_it.index]);
				if(i.absolute_position) {
					xoff = x + i.abs_x;
					yoff = y + i.abs_y;
					was_abs = true;
				}
			}
			place_it.place_current(state, this, page_first, alternate, xoff, yoff, mr.x_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), mr.y_space, remake_lists);

			if(!place_it.current_is_glue()) {
				page_first = false;
			}

			space_used += mr.x_space;
			if(mr.other == measure_result::special::space_consumer) {
				space_used += per_fill_consumer;
			}
			if(!place_it.current_is_glue() && !was_abs)
				space_used -= overlap_subtraction;

			place_it.move_position(1);
		}
	} break;
	case layout_type::overlapped_vertical:
	{
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));

		layout_iterator place_it(lvl.contents);
		if(lvl.current_page > 0) {
			place_it.index = lvl.page_starts[lvl.current_page - 1].last_index;
			place_it.sub_index = lvl.page_starts[lvl.current_page - 1].last_sub_index;
		}
		int32_t space_used = lvl.page_starts[lvl.current_page].space_used;
		int32_t fill_consumer_count = lvl.page_starts[lvl.current_page].space_consumer_count;
		int32_t non_glue_count = lvl.page_starts[lvl.current_page].non_glue_count;

		int32_t extra_runlength = std::max(0, int32_t(effective_y_size - space_used));
		int32_t per_fill_consumer = fill_consumer_count != 0 ? (extra_runlength / fill_consumer_count) : 0;
		int32_t extra_lead = 0;
		switch(lvl.line_alignment) {
		case layout_line_alignment::leading: break;
		case layout_line_alignment::trailing: extra_lead = extra_runlength - fill_consumer_count * per_fill_consumer; break;
		case layout_line_alignment::centered: extra_lead = (extra_runlength - fill_consumer_count * per_fill_consumer) / 2;  break;
		}
		int32_t overlap_subtraction = (non_glue_count > 1 && space_used > effective_y_size) ? int32_t(space_used - effective_y_size) / (non_glue_count - 1) : 0;
		space_used = y + extra_lead + top_margin;

		bool page_first = true;
		bool alternate = true;
		while(place_it.has_more() && (!lvl.paged || place_it.index < lvl.page_starts[lvl.current_page].last_index || (place_it.index == lvl.page_starts[lvl.current_page].last_index && place_it.sub_index < lvl.page_starts[lvl.current_page].last_sub_index))) {
			auto mr = place_it.measure_current(state, false, effective_x_size, page_first);
			int32_t xoff = 0;
			int32_t yoff = space_used;
			switch(lvl.line_internal_alignment) {
			case layout_line_alignment::leading: xoff = x + left_margin; break;
			case layout_line_alignment::trailing: xoff = x + left_margin + effective_x_size - mr.x_space; break;
			case layout_line_alignment::centered: xoff = x + left_margin + (effective_x_size - mr.x_space) / 2;  break;
			}
			bool was_abs = false;
			if(std::holds_alternative< layout_control>(lvl.contents[place_it.index])) {
				auto& i = std::get<layout_control>(lvl.contents[place_it.index]);
				if(i.absolute_position) {
					xoff = x + i.abs_x;
					yoff = y + i.abs_y;
					was_abs = true;
				}
			} else if(std::holds_alternative< layout_window>(lvl.contents[place_it.index])) {
				auto& i = std::get<layout_window>(lvl.contents[place_it.index]);
				if(i.absolute_position) {
					xoff = x + i.abs_x;
					yoff = y + i.abs_y;
					was_abs = true;
				}
			}
			place_it.place_current(state, this, page_first, alternate, xoff, yoff, mr.x_space, mr.y_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), remake_lists);

			if(!place_it.current_is_glue()) {
				page_first = false;
			}

			space_used += mr.y_space;
			if(mr.other == measure_result::special::space_consumer) {
				space_used += per_fill_consumer;
			}
			if(!place_it.current_is_glue() && !was_abs)
				space_used -= overlap_subtraction;

			place_it.move_position(1);
		}
	} break;
	case layout_type::mulitline_horizontal:
	{
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));

		layout_iterator place_it(lvl.contents);
		if(lvl.current_page > 0) {
			place_it.index = lvl.page_starts[lvl.current_page - 1].last_index;
			place_it.sub_index = lvl.page_starts[lvl.current_page - 1].last_sub_index;
		}

		int32_t total_crosswise = 0;
		bool page_first = true;

		while(place_it.has_more() && (!lvl.paged || place_it.index < lvl.page_starts[lvl.current_page].last_index || (place_it.index == lvl.page_starts[lvl.current_page].last_index && place_it.sub_index < lvl.page_starts[lvl.current_page].last_sub_index))) { // lines

			bool col_first = true;
			int32_t max_crosswise = 0;
			int32_t space_used = 0;
			int32_t fill_consumer_count = 0;
			layout_iterator line_start_it = place_it;
			
			while(place_it.has_more() && (!lvl.paged || place_it.index < lvl.page_starts[lvl.current_page].last_index || (place_it.index == lvl.page_starts[lvl.current_page].last_index && place_it.sub_index < lvl.page_starts[lvl.current_page].last_sub_index))) { // line content

				auto mr = place_it.measure_current(state, true, effective_y_size - total_crosswise, col_first);
				if(!page_first && lvl.paged && total_crosswise + mr.y_space > effective_y_size) {
					while(place_it.has_more() && place_it.current_is_glue()) {
						place_it.move_position(1);
					}
					break; // end page
				} else if(!col_first && lvl.paged && (space_used + mr.x_space > effective_x_size)) {
					//check if previous was glue, and erase
					if(place_it.index != 0) {
						place_it.move_position(-1);
						if(place_it.current_is_glue()) {
							space_used -= place_it.measure_current(state, true, effective_y_size - total_crosswise, col_first).x_space;
						}
						place_it.move_position(1);
					}
					while(place_it.has_more() && place_it.current_is_glue()) {
						place_it.move_position(1);
					}
					break; // end line
				}

				col_first = false;
				page_first = false;
				if(mr.other == measure_result::special::space_consumer)
					++fill_consumer_count;

				if(mr.other == measure_result::special::end_line) {
					place_it.move_position(1);
					while(place_it.has_more() && place_it.current_is_glue()) {
						place_it.move_position(1);
					}
					space_used += mr.x_space;
					max_crosswise = std::max(max_crosswise, mr.y_space);
					break; // end line
				} else if(lvl.paged && mr.other == measure_result::special::end_page) {
					place_it.move_position(1);
					while(place_it.has_more() && place_it.current_is_glue()) {
						place_it.move_position(1);
					}
					space_used += mr.x_space;
					max_crosswise = std::max(max_crosswise, mr.y_space);
					break; // end page
				}

				space_used += mr.x_space;
				max_crosswise = std::max(max_crosswise, mr.y_space);
				place_it.move_position(1);
			} // end single line loop

			// arrange line here
			int32_t extra_runlength = int32_t(effective_x_size - space_used);
			int32_t per_fill_consumer = fill_consumer_count != 0 ? (extra_runlength / fill_consumer_count) : 0;
			int32_t extra_lead = 0;
			switch(lvl.line_alignment) {
			case layout_line_alignment::leading: break;
			case layout_line_alignment::trailing: extra_lead = extra_runlength - fill_consumer_count * per_fill_consumer; break;
			case layout_line_alignment::centered: extra_lead = (extra_runlength - fill_consumer_count * per_fill_consumer) / 2;  break;
			}
			space_used = x + extra_lead + left_margin;

			col_first = true;
			bool alternate = true;
			while(line_start_it.has_more() && (!lvl.paged || line_start_it.index < place_it.index || (line_start_it.index == place_it.index && line_start_it.sub_index < place_it.sub_index))) {
				auto mr = line_start_it.measure_current(state, true, max_crosswise, col_first);
				int32_t yoff = 0;
				int32_t xoff = space_used;
				switch(lvl.line_internal_alignment) {
				case layout_line_alignment::leading: yoff = y + top_margin + total_crosswise; break;
				case layout_line_alignment::trailing: yoff = y + top_margin + total_crosswise + max_crosswise - mr.y_space; break;
				case layout_line_alignment::centered: yoff = y + top_margin  + total_crosswise + (max_crosswise - mr.y_space) / 2;  break;
				}
				if(std::holds_alternative< layout_control>(lvl.contents[line_start_it.index])) {
					auto& i = std::get<layout_control>(lvl.contents[line_start_it.index]);
					if(i.absolute_position) {
						xoff = x + i.abs_x;
						yoff = y + i.abs_y;
					}
				} else if(std::holds_alternative<layout_window>(lvl.contents[line_start_it.index])) {
					auto& i = std::get<layout_window>(lvl.contents[line_start_it.index]);
					if(i.absolute_position) {
						xoff = x + i.abs_x;
						yoff = y + i.abs_y;
					}
				}
				line_start_it.place_current(state, this, col_first, alternate, xoff, yoff, mr.x_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), mr.y_space, remake_lists);

				col_first = false;
				
				space_used += mr.x_space;
				if(mr.other == measure_result::special::space_consumer) {
					space_used += per_fill_consumer;
				}
				line_start_it.move_position(1);
			}
			total_crosswise += (max_crosswise + lvl.interline_spacing);
		} // end lines loop
	} break;
	case layout_type::multiline_vertical:
	{
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));

		layout_iterator place_it(lvl.contents);
		if(lvl.current_page > 0) {
			place_it.index = lvl.page_starts[lvl.current_page - 1].last_index;
			place_it.sub_index = lvl.page_starts[lvl.current_page - 1].last_sub_index;
		}

		int32_t total_crosswise = 0;
		bool page_first = true;

		while(place_it.has_more() && (!lvl.paged || place_it.index < lvl.page_starts[lvl.current_page].last_index || (place_it.index == lvl.page_starts[lvl.current_page].last_index && place_it.sub_index < lvl.page_starts[lvl.current_page].last_sub_index))) { // lines

			bool col_first = true;
			int32_t max_crosswise = 0;
			int32_t space_used = 0;
			int32_t fill_consumer_count = 0;
			layout_iterator line_start_it = place_it;

			while(place_it.has_more() && (!lvl.paged || place_it.index < lvl.page_starts[lvl.current_page].last_index || (place_it.index == lvl.page_starts[lvl.current_page].last_index && place_it.sub_index < lvl.page_starts[lvl.current_page].last_sub_index))) { // line content

				auto mr = place_it.measure_current(state, false , effective_x_size - total_crosswise, col_first);
				if(!page_first && lvl.paged && total_crosswise + mr.x_space > effective_x_size) {
					while(place_it.has_more() && place_it.current_is_glue()) {
						place_it.move_position(1);
					}
					break; // end page
				} else if(!col_first && lvl.paged && (space_used + mr.y_space > effective_y_size)) {
					//check if previous was glue, and erase
					if(place_it.index != 0) {
						place_it.move_position(-1);
						if(place_it.current_is_glue()) {
							space_used -= place_it.measure_current(state, false, effective_x_size - total_crosswise, col_first).y_space;
						}
						place_it.move_position(1);
					}
					while(place_it.has_more() && place_it.current_is_glue()) {
						place_it.move_position(1);
					}
					break; // end line
				}

				col_first = false;
				page_first = false;
				if(mr.other == measure_result::special::space_consumer)
					++fill_consumer_count;

				if(mr.other == measure_result::special::end_line) {
					place_it.move_position(1);
					while(place_it.has_more() && place_it.current_is_glue()) {
						place_it.move_position(1);
					}
					space_used += mr.y_space;
					max_crosswise = std::max(max_crosswise, mr.x_space);
					break; // end line
				} else if(lvl.paged && mr.other == measure_result::special::end_page) {
					place_it.move_position(1);
					while(place_it.has_more() && place_it.current_is_glue()) {
						place_it.move_position(1);
					}
					space_used += mr.y_space;
					max_crosswise = std::max(max_crosswise, mr.x_space);
					break; // end page
				}

				space_used += mr.y_space;
				max_crosswise = std::max(max_crosswise, mr.x_space);
				place_it.move_position(1);
			} // end single line loop

			// arrange line here
			int32_t extra_runlength = int32_t(effective_y_size - space_used);
			int32_t per_fill_consumer = fill_consumer_count != 0 ? (extra_runlength / fill_consumer_count) : 0;
			int32_t extra_lead = 0;
			switch(lvl.line_alignment) {
			case layout_line_alignment::leading: break;
			case layout_line_alignment::trailing: extra_lead = extra_runlength - fill_consumer_count * per_fill_consumer; break;
			case layout_line_alignment::centered: extra_lead = (extra_runlength - fill_consumer_count * per_fill_consumer) / 2;  break;
			}
			space_used = y + extra_lead + top_margin;
			col_first = true;
			bool alternate = true;
			while(line_start_it.has_more() && (!lvl.paged || line_start_it.index < place_it.index || (line_start_it.index == place_it.index && line_start_it.sub_index < place_it.sub_index))) {
				auto mr = line_start_it.measure_current(state, false, max_crosswise, col_first);
				int32_t xoff = 0;
				int32_t yoff = space_used;
				switch(lvl.line_internal_alignment) {
				case layout_line_alignment::leading: xoff = x + left_margin + total_crosswise; break;
				case layout_line_alignment::trailing: xoff = x + left_margin + total_crosswise + max_crosswise - mr.x_space; break;
				case layout_line_alignment::centered: xoff = x + left_margin + total_crosswise + (max_crosswise - mr.x_space) / 2;  break;
				}
				if(std::holds_alternative< layout_control>(lvl.contents[line_start_it.index])) {
					auto& i = std::get<layout_control>(lvl.contents[line_start_it.index]);
					if(i.absolute_position) {
						xoff = x + i.abs_x;
						yoff = y + i.abs_y;
					}
				} else if(std::holds_alternative<layout_window>(lvl.contents[line_start_it.index])) {
					auto& i = std::get<layout_window>(lvl.contents[line_start_it.index]);
					if(i.absolute_position) {
						xoff = x + i.abs_x;
						yoff = y + i.abs_y;
					}
				}
				line_start_it.place_current(state, this, col_first, alternate, xoff, yoff, mr.x_space, mr.y_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), remake_lists);

				col_first = false;

				space_used += mr.y_space;
				if(mr.other == measure_result::special::space_consumer) {
					space_used += per_fill_consumer;
				}
				line_start_it.move_position(1);
			}
			total_crosswise += (max_crosswise + lvl.interline_spacing);
		} // end lines loop
	} break;
	}
}

}
