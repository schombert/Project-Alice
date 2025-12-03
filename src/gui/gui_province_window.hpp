#pragma once

#include "dcon_generated_ids.hpp"
#include "gui_element_types.hpp"

namespace ui {

class province_window_header;
class province_view_foreign_details;
class province_view_statistics;
class province_view_buildings;
class province_window_colony;

class province_terrain_image : public opaque_element_base {
public:
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override;
};

class province_view_window : public window_element_base {
private:
	dcon::province_id active_province{};
	province_window_header* header_window = nullptr;
	province_view_foreign_details* foreign_details_window = nullptr;
	province_view_statistics* local_details_window = nullptr;
	province_view_buildings* local_buildings_window = nullptr;
	province_window_colony* colony_window = nullptr;
	element_base* economy_window = nullptr;
	element_base* nf_win = nullptr;
	element_base* tiles_window = nullptr;
	element_base* market_window = nullptr;


public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;	
	void set_active_province(sys::state& state, dcon::province_id map_province);
	void on_update(sys::state& state) noexcept override;
	friend class province_national_focus_button;
};

} // namespace ui
