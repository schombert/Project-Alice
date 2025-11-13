#include <vector>
#include "uitemplate.hpp"
#include "stools.hpp"

namespace template_project {

project bytes_to_project(serialization::in_buffer& buffer) {
	project result;
	auto header_section = buffer.read_section();
	header_section.read(result.svg_directory);

	std::string discard_string;

		auto colors_section = buffer.read_section();
		while(colors_section) {
			result.colors.emplace_back();
			auto individual_color = colors_section.read_section();
			std::string name_string;
			individual_color.read(name_string);
			result.colors_by_name.insert_or_assign(name_string, int32_t(result.colors.size() - 1));
			individual_color.read(result.colors.back().r);
			individual_color.read(result.colors.back().g);
			individual_color.read(result.colors.back().b);
			individual_color.read(result.colors.back().a);
		}

		auto icons_section = buffer.read_section();
		while(icons_section) {
			result.icons.emplace_back();
			auto indv_icon = icons_section.read_section();
			indv_icon.read(result.icons.back().file_name);
			result.icons_by_name.insert_or_assign(result.icons.back().file_name, int32_t(result.colors.size() - 1));
		}

		auto bg_section = buffer.read_section();
		while(bg_section) {
			result.backgrounds.emplace_back();
			auto indv_bg = bg_section.read_section();
			indv_bg.read(result.backgrounds.back().file_name);
			indv_bg.read(result.backgrounds.back().base_x);
			indv_bg.read(result.backgrounds.back().base_y);
			result.backgrounds_by_name.insert_or_assign(result.backgrounds.back().file_name, int32_t(result.colors.size() - 1));
		}

		auto labels_section = buffer.read_section();
		while(labels_section) {
			result.label_t.emplace_back();
			auto indv_label = labels_section.read_section();
			indv_label.read(discard_string);
			indv_label.read(result.label_t.back().primary.bg);
			indv_label.read(result.label_t.back().primary.text_color);
			indv_label.read(result.label_t.back().primary.font_choice);
			indv_label.read(result.label_t.back().primary.font_scale);
			indv_label.read(result.label_t.back().primary.h_text_margins);
			indv_label.read(result.label_t.back().primary.v_text_margins);
			indv_label.read(result.label_t.back().primary.h_text_alignment);
			indv_label.read(result.label_t.back().primary.v_text_alignment);
		}

		auto buttons_section = buffer.read_section();
		while(buttons_section) {
			result.button_t.emplace_back();
			auto indv_button = buttons_section.read_section();
			indv_button.read(discard_string);
			indv_button.read(result.button_t.back().animate_active_transition);
			indv_button.read(result.button_t.back().primary.bg);
			indv_button.read(result.button_t.back().primary.text_color);
			indv_button.read(result.button_t.back().primary.font_choice);
			indv_button.read(result.button_t.back().primary.font_scale);
			indv_button.read(result.button_t.back().primary.h_text_margins);
			indv_button.read(result.button_t.back().primary.v_text_margins);
			indv_button.read(result.button_t.back().primary.h_text_alignment);
			indv_button.read(result.button_t.back().primary.v_text_alignment);
			indv_button.read(result.button_t.back().active.bg);
			indv_button.read(result.button_t.back().active.text_color);
			indv_button.read(result.button_t.back().active.font_choice);
			indv_button.read(result.button_t.back().active.font_scale);
			indv_button.read(result.button_t.back().active.h_text_margins);
			indv_button.read(result.button_t.back().active.v_text_margins);
			indv_button.read(result.button_t.back().active.h_text_alignment);
			indv_button.read(result.button_t.back().active.v_text_alignment);
			indv_button.read(result.button_t.back().disabled.bg);
			indv_button.read(result.button_t.back().disabled.text_color);
			indv_button.read(result.button_t.back().disabled.font_choice);
			indv_button.read(result.button_t.back().disabled.font_scale);
			indv_button.read(result.button_t.back().disabled.h_text_margins);
			indv_button.read(result.button_t.back().disabled.v_text_margins);
			indv_button.read(result.button_t.back().disabled.h_text_alignment);
			indv_button.read(result.button_t.back().disabled.v_text_alignment);
		}

		auto pbs_section = buffer.read_section();
		while(pbs_section) {
			result.progress_bar_t.emplace_back();
			auto indv_pb = pbs_section.read_section();
			indv_pb.read(discard_string);
			indv_pb.read(result.progress_bar_t.back().bg_a);
			indv_pb.read(result.progress_bar_t.back().bg_b);
			indv_pb.read(result.progress_bar_t.back().text_color);
			indv_pb.read(result.progress_bar_t.back().font_choice);
			indv_pb.read(result.progress_bar_t.back().h_text_margins);
			indv_pb.read(result.progress_bar_t.back().v_text_margins);
			indv_pb.read(result.progress_bar_t.back().h_text_alignment);
			indv_pb.read(result.progress_bar_t.back().v_text_alignment);
			indv_pb.read(result.progress_bar_t.back().display_percentage_text);
		}

		auto windows_section = buffer.read_section();
		while(windows_section) {
			result.window_t.emplace_back();
			auto indv_win = windows_section.read_section();
			indv_win.read(discard_string);
			indv_win.read(result.window_t.back().bg);
			indv_win.read(result.window_t.back().layout_region_definition);
			indv_win.read(result.window_t.back().close_button_definition);
			indv_win.read(result.window_t.back().close_button_icon);
			indv_win.read(result.window_t.back().h_close_button_margin);
			indv_win.read(result.window_t.back().v_close_button_margin);
		}

		auto ibuttons_section = buffer.read_section();
		while(ibuttons_section) {
			result.iconic_button_t.emplace_back();
			auto indv_ibutton = ibuttons_section.read_section();
			indv_ibutton.read(discard_string);
			indv_ibutton.read(result.iconic_button_t.back().animate_active_transition);
			indv_ibutton.read(result.iconic_button_t.back().primary.bg);
			indv_ibutton.read(result.iconic_button_t.back().primary.icon_color);
			indv_ibutton.read(result.iconic_button_t.back().primary.icon_top);
			indv_ibutton.read(result.iconic_button_t.back().primary.icon_left);
			indv_ibutton.read(result.iconic_button_t.back().primary.icon_bottom);
			indv_ibutton.read(result.iconic_button_t.back().primary.icon_right);
			indv_ibutton.read(result.iconic_button_t.back().active.bg);
			indv_ibutton.read(result.iconic_button_t.back().active.icon_color);
			indv_ibutton.read(result.iconic_button_t.back().active.icon_top);
			indv_ibutton.read(result.iconic_button_t.back().active.icon_left);
			indv_ibutton.read(result.iconic_button_t.back().active.icon_bottom);
			indv_ibutton.read(result.iconic_button_t.back().active.icon_right);
			indv_ibutton.read(result.iconic_button_t.back().disabled.bg);
			indv_ibutton.read(result.iconic_button_t.back().disabled.icon_color);
			indv_ibutton.read(result.iconic_button_t.back().disabled.icon_top);
			indv_ibutton.read(result.iconic_button_t.back().disabled.icon_left);
			indv_ibutton.read(result.iconic_button_t.back().disabled.icon_bottom);
			indv_ibutton.read(result.iconic_button_t.back().disabled.icon_right);
		}

		auto layout_regions_section = buffer.read_section();
		while(layout_regions_section) {
			result.layout_region_t.emplace_back();
			auto indv_lr = layout_regions_section.read_section();
			indv_lr.read(discard_string);
			indv_lr.read(result.layout_region_t.back().page_number_text.bg);
			indv_lr.read(result.layout_region_t.back().page_number_text.text_color);
			indv_lr.read(result.layout_region_t.back().page_number_text.font_choice);
			indv_lr.read(result.layout_region_t.back().page_number_text.font_scale);
			indv_lr.read(result.layout_region_t.back().page_number_text.h_text_margins);
			indv_lr.read(result.layout_region_t.back().page_number_text.v_text_margins);
			indv_lr.read(result.layout_region_t.back().page_number_text.h_text_alignment);
			indv_lr.read(result.layout_region_t.back().page_number_text.v_text_alignment);
			indv_lr.read(result.layout_region_t.back().bg);
			indv_lr.read(result.layout_region_t.back().left_button);
			indv_lr.read(result.layout_region_t.back().left_button_icon);
			indv_lr.read(result.layout_region_t.back().right_button);
			indv_lr.read(result.layout_region_t.back().right_button_icon);
		}

		auto mb_section = buffer.read_section();
		while(mb_section) {
			result.mixed_button_t.emplace_back();
			auto indv_mb = mb_section.read_section();
			auto& i = result.mixed_button_t.back();

			indv_mb.read(discard_string);

			indv_mb.read(i.primary.bg);
			indv_mb.read(i.primary.shared_color);
			indv_mb.read(i.primary.font_choice);
			indv_mb.read(i.primary.font_scale);
			indv_mb.read(i.primary.h_text_margins);
			indv_mb.read(i.primary.v_text_margins);
			indv_mb.read(i.primary.h_text_alignment);
			indv_mb.read(i.primary.v_text_alignment);
			indv_mb.read(i.primary.icon_top);
			indv_mb.read(i.primary.icon_left);
			indv_mb.read(i.primary.icon_bottom);
			indv_mb.read(i.primary.icon_right);

			indv_mb.read(i.active.bg);
			indv_mb.read(i.active.shared_color);
			indv_mb.read(i.active.font_choice);
			indv_mb.read(i.active.font_scale);
			indv_mb.read(i.active.h_text_margins);
			indv_mb.read(i.active.v_text_margins);
			indv_mb.read(i.active.h_text_alignment);
			indv_mb.read(i.active.v_text_alignment);
			indv_mb.read(i.active.icon_top);
			indv_mb.read(i.active.icon_left);
			indv_mb.read(i.active.icon_bottom);
			indv_mb.read(i.active.icon_right);

			indv_mb.read(i.disabled.bg);
			indv_mb.read(i.disabled.shared_color);
			indv_mb.read(i.disabled.font_choice);
			indv_mb.read(i.disabled.font_scale);
			indv_mb.read(i.disabled.h_text_margins);
			indv_mb.read(i.disabled.v_text_margins);
			indv_mb.read(i.disabled.h_text_alignment);
			indv_mb.read(i.disabled.v_text_alignment);
			indv_mb.read(i.disabled.icon_top);
			indv_mb.read(i.disabled.icon_left);
			indv_mb.read(i.disabled.icon_bottom);
			indv_mb.read(i.disabled.icon_right);

			indv_mb.read(i.animate_active_transition);
		}
	
	return result;
}

}

