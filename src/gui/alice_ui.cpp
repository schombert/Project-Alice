#include "system_state.hpp"
#include "gui_element_base.hpp"
#include "gui_element_types.hpp"
#include "gui_deserialize.hpp"
#include "alice_ui.hpp"
#include "demographics.hpp"

#include "macrobuilder2.cpp"
#include "budgetwindow.cpp"
#include "demographicswindow.cpp"
#include "pop_details.cpp"

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
	case animation_type::page_middle:
		return forwards ? ogl::animation::type::page_flip_mid : ogl::animation::type::page_flip_mid_rev;
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
			state.ui_animation.start_animation(state, pos.x + resolved_x_pos, pos.y + resolved_y_pos, resolved_x_size, resolved_y_size, to_ogl_type(page_animation, true), 220);
		} else if(lflip) {
			auto pos = ui::get_absolute_location(state, container);
			state.ui_animation.start_animation(state, pos.x + resolved_x_pos, pos.y + resolved_y_pos, resolved_x_size, resolved_y_size, to_ogl_type(page_animation, false), 220);
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
			i.ptr->impl_on_update(state);
		} else if(std::holds_alternative<layout_window>(m)) {
			auto& i = std::get<layout_window>(m);
			i.ptr->base_data.position.x = int16_t(x);
			i.ptr->base_data.position.y = int16_t(y);
			destination->children.push_back(i.ptr.get());
			i.ptr->impl_on_update(state);
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

void layout_window_element::impl_on_update(sys::state& state) noexcept {
	on_update(state);
}

void layout_window_element::clear_pages_internal(layout_level& lvl) {
	lvl.page_starts.clear();
	for(auto& m : lvl.contents) {
		if(holds_alternative<sub_layout>(m)) {
			auto& i = std::get<sub_layout>(m);
			clear_pages_internal(*i.layout);
		}
	}
}
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

	if(lvl.page_starts.empty())
		remake_lists = true;

	if(remake_lists) {
		switch(lvl.type) {
		case layout_type::single_horizontal:
		{
			layout_iterator it(lvl.contents);

			// measure loop

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
			if(lvl.page_starts.empty()) {
				lvl.page_starts.push_back(page_info{ int16_t(0), int16_t(0), int16_t(0), int16_t(0), int16_t(0) });
			}
		} break;
		case layout_type::single_vertical:
		{
			layout_iterator it(lvl.contents);

			// measure loop

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
			if(lvl.page_starts.empty()) {
				lvl.page_starts.push_back(page_info{ int16_t(0), int16_t(0), int16_t(0), int16_t(0), int16_t(0) });
			}
		} break;
		case layout_type::overlapped_horizontal:
		{
			layout_iterator it(lvl.contents);

			// measure loop

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
			if(lvl.page_starts.empty()) {
				lvl.page_starts.push_back(page_info{ int16_t(0), int16_t(0), int16_t(0), int16_t(0), int16_t(0) });
			}
		} break;
		case layout_type::overlapped_vertical:
		{
			layout_iterator it(lvl.contents);

			// measure loop

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
			if(lvl.page_starts.empty()) {
				lvl.page_starts.push_back(page_info{ int16_t(0), int16_t(0), int16_t(0), int16_t(0), int16_t(0) });
			}
		} break;
		case layout_type::mulitline_horizontal:
		{
			layout_iterator it(lvl.contents);

			// measure loop

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
			if(lvl.page_starts.empty()) {
				lvl.page_starts.push_back(page_info{ int16_t(0), int16_t(0), int16_t(0), int16_t(0), int16_t(0) });
			}
		} break;
		case layout_type::multiline_vertical:
		{
			layout_iterator it(lvl.contents);

			// measure loop

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
			if(lvl.page_starts.empty()) {
				lvl.page_starts.push_back(page_info{ int16_t(0), int16_t(0), int16_t(0), int16_t(0), int16_t(0) });
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


void describe_conversion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto location = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(location);
	auto conversion_chances = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.conversion_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);

	float base_amount =
		state.defines.conversion_scale *
		(state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::conversion_rate) + 1.0f) *
		(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_conversion_rate) + 1.0f) *
		conversion_chances;

	auto pr = state.world.pop_get_religion(ids);
	auto state_religion = state.world.nation_get_religion(owner);

	// pops of the state religion do not convert
	if(state_religion == pr)
		base_amount = 0.0f;

	// need at least 1 pop following the religion in the province
	if(state.world.province_get_demographics(location, demographics::to_key(state, state_religion.id)) < 1.f)
		base_amount = 0.0f;

	text::add_line(state, contents, "pop_conver_1", text::variable_type::x, int64_t(std::max(0.0f, state.world.pop_get_size(ids) * base_amount)));
	text::add_line_break_to_layout(state, contents);

	if(state_religion == pr) {
		text::add_line(state, contents, "pop_conver_2");
		return;
	}
	// need at least 1 pop following the religion in the province
	if(state.world.province_get_demographics(location, demographics::to_key(state, state_religion.id)) < 1.f) {
		text::add_line(state, contents, "pop_conver_3");
		return; // early exit
	}
	text::add_line(state, contents, "pop_conver_4");
	text::add_line(state, contents, "pop_conver_5", text::variable_type::x, text::fp_three_places{ state.defines.conversion_scale });
	text::add_line(state, contents, "pop_conver_6", text::variable_type::x, text::fp_two_places{ std::max(0.0f,state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_conversion_rate) + 1.0f) });
	ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_conversion_rate, true);
	text::add_line(state, contents, "pop_conver_7", text::variable_type::x, text::fp_two_places{ std::max(0.0f, state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::conversion_rate) + 1.0f) });
	ui::active_modifiers_description(state, contents, location, 15, sys::provincial_mod_offsets::conversion_rate, true);

	text::add_line(state, contents, "pop_conver_8", text::variable_type::x, text::fp_four_places{ conversion_chances });
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.conversion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);
}

void describe_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto loc = state.world.pop_get_province_from_pop_location(ids);

	auto owners = state.world.province_get_nation_from_province_ownership(loc);
	float base = 0.f;
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.bureaucrat
		&& (
			state.world.nation_get_accepted_cultures(owners, state.world.pop_get_culture(ids))
			|| state.world.nation_get_primary_culture(owners) == state.world.pop_get_culture(ids)
			)
	) {
		base = demographics::administration_base_push;
		text::add_line(state, contents, "pop_migration_bureaucracy", text::variable_type::x, text::fp_two_places{ base });
	}

	if(state.world.province_get_is_colonial(loc) && base == 0.f) {
		text::add_line(state, contents, "pop_mig_1");
		return;
	}
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_mig_2");
		return;
	}	

	auto migration_chance = base + std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.migration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);
	auto prov_mod = (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f);
	auto scale = state.defines.immigration_scale;

	text::add_line(state, contents, "pop_mig_3", text::variable_type::x, text::fp_three_places{ scale }, text::variable_type::y,
			text::fp_percentage{ prov_mod }, text::variable_type::val, text::fp_two_places{ migration_chance });

	ui::active_modifiers_description(state, contents, loc, 0, sys::provincial_mod_offsets::immigrant_push, true);

	text::add_line(state, contents, "pop_mig_4");
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.migration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0);
}

void describe_colonial_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);

	
	if(state.world.nation_get_is_colonial_nation(owner) == false) {
		text::add_line(state, contents, "pop_cmig_1");
		return;
	}
	if(state.world.province_get_is_colonial(loc)) {
		text::add_line(state, contents, "pop_cmig_2");
		return;
	}
	auto pt = state.world.pop_get_poptype(ids);
	if(pt == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_cmig_3");
		return;
	}
	if(state.world.pop_type_get_strata(pt) == uint8_t(culture::pop_strata::rich)) {
		text::add_line(state, contents, "pop_cmig_4");
		return;
	}
	if(pt == state.culture_definitions.primary_factory_worker || pt == state.culture_definitions.secondary_factory_worker) {
		text::add_line(state, contents, "pop_cmig_5");
		return;
	}

	float base = 0.f;
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.bureaucrat
		&& (
			state.world.nation_get_accepted_cultures(owner, state.world.pop_get_culture(ids))
			|| state.world.nation_get_primary_culture(owner) == state.world.pop_get_culture(ids)
			)
	) {
		base = demographics::administration_base_push;
		text::add_line(state, contents, "pop_migration_bureaucracy", text::variable_type::x, text::fp_two_places{ base });
	}

	auto mig_chance = base + std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.colonialmigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);
	auto im_push = (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f);
	auto cmig = (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::colonial_migration) + 1.0f);
	auto scale = state.defines.immigration_scale;

	text::add_line(state, contents, "pop_cmig_6", text::variable_type::x, text::fp_three_places{ scale }, text::variable_type::y,
			text::fp_percentage{ im_push }, text::variable_type::num, text::fp_percentage{ cmig }, text::variable_type::val,
			text::fp_two_places{ mig_chance });

	ui::active_modifiers_description(state, contents, loc, 0, sys::provincial_mod_offsets::immigrant_push, true);
	ui::active_modifiers_description(state, contents, owner, 0, sys::national_mod_offsets::colonial_migration, true);

	text::add_line(state, contents, "pop_cmig_7");
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.colonialmigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0);
}

void describe_emigration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owners = state.world.province_get_nation_from_province_ownership(loc);

	if(state.world.nation_get_is_civilized(owners) == false) {
		text::add_line(state, contents, "pop_emg_1");
		return;
	}
	if(state.world.province_get_is_colonial(loc)) {
		text::add_line(state, contents, "pop_emg_2");
		return;
	}
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_emg_3");
		return;
	}
	if(state.world.culture_group_get_is_overseas(
		state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(ids))) == false) {
		text::add_line(state, contents, "pop_emg_4");
		return;
	}

	auto impush = (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f);
	auto emig = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.emigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);
	auto scale = state.defines.immigration_scale;

	text::add_line(state, contents, "pop_emg_5", text::variable_type::x, text::fp_three_places{ scale }, text::variable_type::y,
			text::fp_percentage{ impush }, text::variable_type::val, text::fp_two_places{ emig });

	ui::active_modifiers_description(state, contents, loc, 0, sys::provincial_mod_offsets::immigrant_push, true);

	text::add_line(state, contents, "pop_emg_6");
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.emigration_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);
}

void describe_promotion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {

	auto owner = nations::owner_of_pop(state, ids);
	auto promotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.promotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);
	auto demotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.demotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto si = state.world.province_get_state_membership(loc);
	auto nf = state.world.state_instance_get_owner_focus(si);
	auto promoted_type = state.world.national_focus_get_promotion_type(nf);
	auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);
	auto ptype = state.world.pop_get_poptype(ids);
	auto strata = state.world.pop_type_get_strata(ptype);

	text::add_line(state, contents, "pop_prom_1");
	if(promoted_type) {
		if(promoted_type == ptype) {
			text::add_line(state, contents, "pop_prom_3");
		} else if(state.world.pop_type_get_strata(promoted_type) >= strata) {
			text::add_line(state, contents, "pop_prom_2", text::variable_type::val, text::fp_two_places{ promotion_bonus });
			promotion_chance += promotion_bonus;
		} else if(state.world.pop_type_get_strata(promoted_type) <= strata) {
			demotion_chance += promotion_bonus;
		}
	}
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.promotion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);

	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "pop_prom_4");
	if(promoted_type) {
		if(promoted_type == ptype) {

		} else if(state.world.pop_type_get_strata(promoted_type) >= strata) {

		} else if(state.world.pop_type_get_strata(promoted_type) <= strata) {
			text::add_line(state, contents, "pop_prom_2", text::variable_type::val, text::fp_two_places{ promotion_bonus });
		}
	}
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.demotion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);

	text::add_line_break_to_layout(state, contents);

	if(promotion_chance <= 0.0f && demotion_chance <= 0.0f) {
		text::add_line(state, contents, "pop_prom_7");
		return;
	}

	bool promoting = promotion_chance >= demotion_chance;
	if(promoting) {
		text::add_line(state, contents, "pop_prom_5");
		text::add_line(state, contents, "pop_prom_8", text::variable_type::x, text::fp_three_places{ state.defines.promotion_scale },
				text::variable_type::val, text::fp_two_places{ promotion_chance }, text::variable_type::y,
				text::fp_percentage{ state.world.nation_get_administrative_efficiency(owner) });
	} else {
		text::add_line(state, contents, "pop_prom_6");
	}
}

void describe_demotion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {

	auto owner = nations::owner_of_pop(state, ids);
	auto promotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.promotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);
	auto demotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.demotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto si = state.world.province_get_state_membership(loc);
	auto nf = state.world.state_instance_get_owner_focus(si);
	auto promoted_type = state.world.national_focus_get_promotion_type(nf);
	auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);
	auto ptype = state.world.pop_get_poptype(ids);
	auto strata = state.world.pop_type_get_strata(ptype);

	text::add_line(state, contents, "pop_prom_1");
	if(promoted_type) {
		if(promoted_type == ptype) {
			text::add_line(state, contents, "pop_prom_3");
		} else if(state.world.pop_type_get_strata(promoted_type) >= strata) {
			text::add_line(state, contents, "pop_prom_2", text::variable_type::val, text::fp_two_places{ promotion_bonus });
			promotion_chance += promotion_bonus;
		} else if(state.world.pop_type_get_strata(promoted_type) <= strata) {
			demotion_chance += promotion_bonus;
		}
	}
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.promotion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);

	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "pop_prom_4");
	if(promoted_type) {
		if(promoted_type == ptype) {

		} else if(state.world.pop_type_get_strata(promoted_type) >= strata) {

		} else if(state.world.pop_type_get_strata(promoted_type) <= strata) {
			text::add_line(state, contents, "pop_prom_2", text::variable_type::val, text::fp_two_places{ promotion_bonus });
		}
	}
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.demotion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);

	text::add_line_break_to_layout(state, contents);

	if(promotion_chance <= 0.0f && demotion_chance <= 0.0f) {
		text::add_line(state, contents, "pop_prom_7");
		return;
	}

	bool promoting = promotion_chance >= demotion_chance;
	if(promoting) {
		text::add_line(state, contents, "pop_prom_5");
	} else {
		text::add_line(state, contents, "pop_prom_6");
		text::add_line(state, contents, "pop_prom_9", text::variable_type::x, text::fp_three_places{ state.defines.promotion_scale },
				text::variable_type::val, text::fp_two_places{ demotion_chance });
	}
}

void describe_con(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto const clergy_key = demographics::to_key(state, state.culture_definitions.clergy);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);
	float cfrac =
		state.world.province_get_demographics(loc, clergy_key) / state.world.province_get_demographics(loc, demographics::total);
	auto types = state.world.pop_get_poptype(ids);

	float lx_mod = pop_demographics::get_luxury_needs(state, ids) * state.defines.con_luxury_goods;
	float cl_mod = cfrac * (state.world.pop_type_get_strata(types) == int32_t(culture::pop_strata::poor) ?
														state.defines.con_poor_clergy : state.defines.con_midrich_clergy);
	float lit_mod = ((state.world.nation_get_plurality(owner) / 10.0f) *
								 (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::literacy_con_impact) + 1.0f) *
								 state.defines.con_literacy * pop_demographics::get_literacy(state, ids) *
								 (state.world.province_get_is_colonial(loc) ? state.defines.con_colonial_factor : 1.0f)) / 10.f;

	float pmod = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::pop_consciousness_modifier);
	float omod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_pop_consciousness_modifier);
	float cmod = (state.world.province_get_is_colonial(loc) ? 0.0f :
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::core_pop_consciousness_modifier));

	float local_mod = (pmod + omod) + cmod;

	float sep_mod = (state.world.pop_get_is_primary_or_accepted_culture(ids) ? 0.0f :
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::non_accepted_pop_consciousness_modifier));
	auto old_con = pop_demographics::get_consciousness(state, ids) * 0.01f;
	auto total = (lx_mod + (cl_mod + lit_mod - old_con)) + (local_mod + sep_mod);

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_1");
		if(total >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ total }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ total }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);
	text::add_line(state, contents, "pop_con_2");
	text::add_line(state, contents, "pop_con_3", text::variable_type::x, text::fp_two_places{ lx_mod });
	text::add_line(state, contents, "pop_con_4", text::variable_type::val, text::fp_percentage{ cfrac }, text::variable_type::x,
			text::fp_two_places{ cl_mod });
	text::add_line(state, contents, "pop_con_5", text::variable_type::x, text::fp_two_places{ lit_mod });

	text::add_line(state, contents, "pop_con_6", text::variable_type::x,
			text::fp_two_places{ state.world.nation_get_plurality(owner) }, 15);
	text::add_line(state, contents, "pop_con_7", text::variable_type::x,
			text::fp_percentage{ state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::literacy_con_impact) + 1.0f },
			15);
	ui::active_modifiers_description(state, contents, owner, 30, sys::national_mod_offsets::literacy_con_impact, false);
	text::add_line(state, contents, "pop_con_8", text::variable_type::x, text::fp_two_places{ state.defines.con_literacy }, 15);
	text::add_line(state, contents, "pop_con_9", text::variable_type::x, text::fp_percentage{ pop_demographics::get_literacy(state, ids) },
			15);
	if(state.world.province_get_is_colonial(loc)) {
		text::add_line(state, contents, "pop_con_10", text::variable_type::x, text::fp_two_places{ state.defines.con_colonial_factor },
				15);
	}
	text::add_line(state, contents, "pop_con_11", 15);
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_12");
		if(pmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ pmod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ pmod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, loc, 15, sys::provincial_mod_offsets::pop_consciousness_modifier, false);
	}
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_13");
		if(omod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ omod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ omod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_pop_consciousness_modifier, false);
	}
	if(!state.world.province_get_is_colonial(loc)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_14");
		if(cmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ cmod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ cmod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::core_pop_consciousness_modifier, false);
	}
	if(!state.world.pop_get_is_primary_or_accepted_culture(ids)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_15");
		if(sep_mod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ sep_mod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ sep_mod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::non_accepted_pop_consciousness_modifier,
				false);
	}
	text::add_line(state, contents, "alice_con_decay_description", text::variable_type::x, text::fp_three_places{ pop_demographics::get_consciousness(state, ids) * 0.01f });
}

void describe_mil(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto const conservatism_key = pop_demographics::to_key(state, state.culture_definitions.conservative);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);
	auto ruling_party = state.world.nation_get_ruling_party(owner);
	auto ruling_ideology = state.world.political_party_get_ideology(ruling_party);

	float lx_mod = std::max(pop_demographics::get_luxury_needs(state, ids) - 0.5f, 0.0f) * state.defines.mil_has_luxury_need;
	float con_sup = (pop_demographics::get_demo(state, ids, conservatism_key) * state.defines.mil_ideology);
	float ruling_sup = ruling_ideology ? pop_demographics::get_demo(state, ids, pop_demographics::to_key(state, ruling_ideology)) * state.defines.mil_ruling_party : 0.0f;
	float ref_mod = state.world.province_get_is_colonial(loc)
		? 0.0f
		: (pop_demographics::get_social_reform_desire(state, ids) + pop_demographics::get_political_reform_desire(state, ids)) *
		(state.defines.mil_require_reform * 0.25f);

	float sub_t = (lx_mod + ruling_sup) + (con_sup + ref_mod);

	auto o_spending = state.world.nation_get_overseas_penalty(owner);
	auto spending_level = state.world.nation_get_spending_level(owner);
	auto overseas_mil = 0.f;
	if(province::is_overseas(state, loc)) {
		overseas_mil = 2.f * state.defines.alice_overseas_mil * (0.5f - (o_spending * spending_level));
	}

	float pmod = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::pop_militancy_modifier);
	float omod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_pop_militancy_modifier);
	float cmod = (state.world.province_get_is_colonial(loc) ? 0.0f :
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::core_pop_militancy_modifier));

	float local_mod = (pmod + omod) + cmod;

	float sep_mod = (state.world.pop_get_is_primary_or_accepted_culture(ids) ? 0.0f :
			(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::non_accepted_pop_militancy_modifier) + 1.0f) *
					state.defines.mil_non_accepted);
	float ln_mod = std::min((pop_demographics::get_life_needs(state, ids) - 0.5f), 0.0f) * state.defines.mil_no_life_need;
	float en_mod_a =
		std::min(0.0f, (pop_demographics::get_everyday_needs(state, ids) - 0.5f)) * state.defines.mil_lack_everyday_need;
	float en_mod_b =
		std::max(0.0f, (pop_demographics::get_everyday_needs(state, ids) - 0.5f)) * state.defines.mil_has_everyday_need;
	//Ranges from +0.00 - +0.50 militancy monthly, 0 - 100 war exhaustion
	float war_exhaustion =
		state.world.nation_get_war_exhaustion(owner) * state.defines.mil_war_exhaustion;
	auto old_mil = pop_demographics::get_militancy(state, ids) * 0.01f;
	float total = (sub_t + local_mod) + ((sep_mod - ln_mod - old_mil) + (en_mod_b - en_mod_a) + (war_exhaustion + overseas_mil));

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_1");
		if(total >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::red);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ total }, text::text_color::red);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ total }, text::text_color::green);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);
	text::add_line(state, contents, "pop_mil_2");
	text::add_line(state, contents, "pop_mil_3", text::variable_type::x, text::fp_two_places{ ln_mod < 0.0f ? -ln_mod : 0.0f });
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_4");
		if(en_mod_b - en_mod_a >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::red);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ en_mod_b - en_mod_a }, text::text_color::red);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ en_mod_b - en_mod_a }, text::text_color::green);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line(state, contents, "pop_mil_5", text::variable_type::x, text::fp_two_places{ lx_mod });
	text::add_line(state, contents, "pop_mil_6", text::variable_type::x, text::fp_two_places{ con_sup });
	text::add_line(state, contents, "pop_mil_7", text::variable_type::x, text::fp_two_places{ ruling_sup });
	text::add_line(state, contents, "pop_mil_8", text::variable_type::x, text::fp_two_places{ ref_mod });
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_9");
		if(pmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ pmod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ pmod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, loc, 15, sys::provincial_mod_offsets::pop_militancy_modifier, false);
	}
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_10");
		if(omod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ omod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ omod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_pop_militancy_modifier, false);
	}
	if(!state.world.province_get_is_colonial(loc)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_11");
		if(cmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ cmod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ cmod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::core_pop_militancy_modifier, false);
	}
	if(!state.world.pop_get_is_primary_or_accepted_culture(ids)) {
		text::add_line(state, contents, "pop_mil_12",
			text::variable_type::val, text::fp_two_places{ sep_mod },
			text::variable_type::x, text::fp_two_places{ state.defines.mil_non_accepted },
			text::variable_type::y, text::fp_percentage{ state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::non_accepted_pop_militancy_modifier) + 1.0f });
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::non_accepted_pop_militancy_modifier, false);
	}
	if(war_exhaustion > 0) {
		text::add_line(state, contents, "pop_mil_13", text::variable_type::val, text::fp_three_places{ war_exhaustion });
	}

	auto box = text::open_layout_box(contents);
	text::localised_format_box(state, contents, box, "alice_overseas_mil_description");
	text::add_space_to_layout_box(state, contents, box);
	if(overseas_mil <= 0) {
		text::add_to_layout_box(state, contents, box, text::fp_two_places{ overseas_mil }, text::text_color::green);
	} else {
		text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::red);
		text::add_to_layout_box(state, contents, box, text::fp_two_places{ overseas_mil }, text::text_color::red);
	}
	text::close_layout_box(contents, box);

	text::add_line(state, contents, "alice_mil_decay_description", text::variable_type::x, text::fp_three_places{ pop_demographics::get_militancy(state, ids) * state.defines.alice_militancy_decay });
}

void describe_lit(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto const clergy_key = demographics::to_key(state, state.culture_definitions.clergy);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);
	auto cfrac =
		state.world.province_get_demographics(loc, clergy_key) / state.world.province_get_demographics(loc, demographics::total);

	auto tmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency) + 1.0f;
	auto nmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency_modifier) + 1.0f;
	auto espending =
		(float(state.world.nation_get_education_spending(owner)) / 100.0f) * state.world.nation_get_spending_level(owner);
	auto cmod = std::max(0.0f, std::min(1.0f, (cfrac - state.defines.base_clergy_for_literacy) /
		(state.defines.max_clergy_for_literacy - state.defines.base_clergy_for_literacy)));

	float total = (0.01f * state.defines.literacy_change_speed) * ((espending * cmod) * (tmod * nmod));

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_lit_1");
		if(total >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_three_places{ total }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_three_places{ total }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);
	text::add_line(state, contents, "pop_lit_2");
	text::add_line(state, contents, "pop_lit_3", text::variable_type::val, text::fp_percentage{ cfrac }, text::variable_type::x,
			text::fp_two_places{ cmod });
	text::add_line(state, contents, "pop_lit_4", text::variable_type::x, text::fp_two_places{ espending });
	text::add_line(state, contents, "pop_lit_5", text::variable_type::x, text::fp_percentage{ tmod });
	ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::education_efficiency, false);
	text::add_line(state, contents, "pop_lit_6", text::variable_type::x, text::fp_percentage{ nmod });
	ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::education_efficiency_modifier, false);
	text::add_line(state, contents, "pop_lit_7", text::variable_type::x, text::fp_two_places{ state.defines.literacy_change_speed });
	text::add_line(state, contents, "pop_lit_8");
}

void describe_growth(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto type = state.world.pop_get_poptype(ids);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);

	auto base_life_rating = float(state.world.province_get_life_rating(loc));
	auto mod_life_rating = std::min(
			base_life_rating * (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::life_rating) + 1.0f), 40.0f);
	auto lr_factor =
		std::max((mod_life_rating - state.defines.min_life_rating_for_growth) * state.defines.life_rating_growth_bonus, 0.0f);
	auto province_factor = lr_factor + state.defines.base_popgrowth;

	auto ln_factor = pop_demographics::get_life_needs(state, ids) - state.defines.life_need_starvation_limit;
	auto mod_sum = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::population_growth) + state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::pop_growth);

	auto total_factor = ln_factor * province_factor * 4.0f + mod_sum * 0.1f;

	if(type == state.culture_definitions.slaves)
		total_factor = 0.0f;

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_growth_1");
		if(total_factor >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, int64_t(total_factor * state.world.pop_get_size(ids)), text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, int64_t(total_factor * state.world.pop_get_size(ids)),
					text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);

	if(type == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_growth_2");
		return;
	}
	text::add_line(state, contents, "pop_growth_3");
	text::add_line(state, contents, "pop_growth_4", text::variable_type::x, text::fp_three_places{ ln_factor * province_factor * 4.0f });
	text::add_line(state, contents, "pop_growth_5", text::variable_type::x, text::fp_four_places{ province_factor }, 15);
	text::add_line(state, contents, "pop_growth_6", text::variable_type::x, text::fp_one_place{ mod_life_rating },
			text::variable_type::y, text::fp_one_place{ state.defines.min_life_rating_for_growth }, text::variable_type::val,
			text::fp_four_places{ state.defines.life_rating_growth_bonus },
			30);
	text::add_line(state, contents, "pop_growth_7", text::variable_type::x, text::fp_three_places{ state.defines.base_popgrowth },
			30);
	text::add_line(state, contents, "pop_growth_8", text::variable_type::x, text::fp_two_places{ ln_factor },
			text::variable_type::y, text::fp_two_places{ pop_demographics::get_life_needs(state, ids) }, text::variable_type::val,
			text::fp_two_places{ state.defines.life_need_starvation_limit },
			15);
	text::add_line(state, contents, "pop_growth_9", 15);
	text::add_line(state, contents, "pop_growth_10", text::variable_type::x,
			text::fp_three_places{ state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::population_growth) });
	ui::active_modifiers_description(state, contents, loc, 15, sys::provincial_mod_offsets::population_growth, false);
	text::add_line(state, contents, "pop_growth_11", text::variable_type::x,
			text::fp_three_places{ state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::pop_growth) });
	ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::pop_growth, false);
}

void describe_assimilation(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto location = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(location);
	auto assimilation_chances = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.assimilation_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);

	auto pc = state.world.pop_get_culture(ids);

	float culture_size = state.world.province_get_demographics(location, demographics::to_key(state, pc));
	float total_size = state.world.province_get_demographics(location, demographics::total);
	auto anti_spam_measure = std::max(0.f, 0.04f - culture_size / (total_size + 1.f)) * 50.f;

	float base_amount =
		anti_spam_measure +
		state.defines.assimilation_scale *
		(state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::assimilation_rate) + 1.0f) *
		(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_assimilation_rate) + 1.0f) *
		assimilation_chances;

	/*
	In a colonial province, assimilation numbers for pops with an *non* "overseas"-type culture group are reduced by a
	factor of 100. In a non-colonial province, assimilation numbers for pops with an *non* "overseas"-type culture
	group are reduced by a factor of 10.
	*/

	if(!state.world.culture_group_get_is_overseas(state.world.culture_get_group_from_culture_group_membership(pc))) {
		base_amount /= 10.0f;
	}


	/*
	All pops have their assimilation numbers reduced by a factor of 100 per core in the province sharing their primary
	culture.
	*/
	for(auto core : state.world.province_get_core(location)) {
		if(core.get_identity().get_primary_culture() == pc) {
			base_amount /= 100.0f;
		}
	}

	// slaves do not assimilate
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves)
		base_amount = 0.0f;

	// pops of an accepted culture do not assimilate
	if(state.world.pop_get_is_primary_or_accepted_culture(ids))
		base_amount = 0.0f;

	// pops in an overseas and colonial province do not assimilate
	if(state.world.province_get_is_colonial(location) && province::is_overseas(state, location))
		base_amount = 0.0f;

	text::add_line(state, contents, "pop_assim_1", text::variable_type::x, int64_t(std::max(0.0f, state.world.pop_get_size(ids) * base_amount)));
	text::add_line_break_to_layout(state, contents);

	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_assim_2");
		return;
	}
	if(state.world.pop_get_is_primary_or_accepted_culture(ids)) {
		text::add_line(state, contents, "pop_assim_3");
		return;
	}
	if(state.world.province_get_is_colonial(location)) {
		text::add_line(state, contents, "pop_assim_4");
		return;
	}
	if(province::is_overseas(state, location)) {
		text::add_line(state, contents, "pop_assim_5");
		return;
	}
	text::add_line(state, contents, "pop_assim_6", text::variable_type::x, text::fp_three_places{ anti_spam_measure });
	text::add_line(state, contents, "pop_assim_7", text::variable_type::x, text::fp_three_places{ state.defines.assimilation_scale });
	text::add_line(state, contents, "pop_assim_8", text::variable_type::x, text::fp_two_places{ std::max(0.0f,state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_assimilation_rate) + 1.0f) });
	ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_assimilation_rate, true);
	text::add_line(state, contents, "pop_assim_9", text::variable_type::x, text::fp_two_places{ std::max(0.0f, state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::assimilation_rate) + 1.0f) });
	ui::active_modifiers_description(state, contents, location, 15, sys::provincial_mod_offsets::assimilation_rate, true);

	if(!state.world.culture_group_get_is_overseas(state.world.culture_get_group_from_culture_group_membership(pc))) {
		text::add_line(state, contents, "pop_assim_10");
	}
	float core_factor = 1.0f;
	for(auto core : state.world.province_get_core(location)) {
		if(core.get_identity().get_primary_culture() == pc) {
			core_factor /= 100.0f;
		}
	}
	if(core_factor < 1.0f) {
		text::add_line(state, contents, "pop_assim_11", text::variable_type::x, text::fp_four_places{ core_factor });
	}
	text::add_line(state, contents, "pop_assim_12", text::variable_type::x, text::fp_four_places{ assimilation_chances });
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.assimilation_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);
}

}
