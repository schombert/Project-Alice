#pragma once

#include <vector>
#include <array>
#include <chrono>
#include <any>
#include "constants_dcon.hpp"
#include "constants.hpp"
#include "dcon_generated_ids.hpp"
#include "unordered_dense.h"
#include "container_types.hpp"
#include "parsers.hpp"
#include "cyto_any.hpp"
#include "military.hpp"
#include "simple_fs.hpp"
#include "constants_ui.hpp"
#include "container_types_ui.hpp"

namespace parsers {
struct building_gfx_context;
}

namespace alice_ui {
class pop_up_menu_container;
}

namespace ui {

class context_menu_window;
class factory_refit_window;

enum class object_type : uint8_t {
	generic_sprite = 0x00,
	bordered_rect = 0x01,
	horizontal_progress_bar = 0x02,
	vertical_progress_bar = 0x03,
	flag_mask = 0x04,
	tile_sprite = 0x05,
	text_sprite = 0x06,
	barchart = 0x07,
	piechart = 0x08,
	linegraph = 0x09
};


struct gfx_object {
	constexpr static uint8_t always_transparent = 0x10;
	constexpr static uint8_t flip_v = 0x20;
	constexpr static uint8_t has_click_sound = 0x40;
	constexpr static uint8_t do_transparency_check = 0x80;

	constexpr static uint8_t type_mask = 0x0F;

	xy_pair size; // 4bytes
	dcon::text_key name;
	dcon::texture_id primary_texture_handle; // 6bytes
	uint16_t type_dependent = 0; // secondary texture handle or border size -- 8bytes
	uint8_t flags = 0; // 9bytes
	uint8_t number_of_frames = 1; // 10bytes

	object_type get_object_type() const {
		return object_type(flags & type_mask);
	}
	bool is_always_transparent() const {
		return (flags & always_transparent) != 0;
	}
	bool is_vertically_flipped() const {
		return (flags & flip_v) != 0;
	}
	bool is_clicky() const {
		return (flags & has_click_sound) != 0;
	}
	bool is_partially_transparent() const {
		return (flags & do_transparency_check) != 0;
	}
};
static_assert(sizeof(gfx_object) == 16);

struct text_base_data {
	static constexpr uint16_t alignment_mask = 0x03;

	dcon::text_key txt; // 4bytes
	uint16_t font_handle = 0; // 6bytes
	uint16_t flags = 0; // 8bytes

	alignment get_alignment() const {
		return alignment(flags & alignment_mask);
	}
};
static_assert(sizeof(text_base_data) == 8);

struct button_data : public text_base_data {
	static constexpr uint16_t clicksound_mask = (0x03 << clicksound_bit_offset);
	static constexpr uint16_t is_checkbox_mask = (0x01 << checkbox_bit_offset);
	static constexpr uint16_t button_scripting_mask = (0x03 << button_scripting_bit_offset);

	//8bytes
	dcon::gfx_object_id button_image; // 8+2bytes
	dcon::trigger_key scriptable_enable; // 8 + 4 bytes
	dcon::effect_key scriptable_effect; // 8 + 6 bytes
	sys::virtual_key shortcut = sys::virtual_key::NONE; // 8+7 bytes

	clicksound get_clicksound() const {
		return clicksound(text_base_data::flags & clicksound_mask);
	}
	bool is_checkbox() const {
		return (text_base_data::flags & is_checkbox_mask) != 0;
	}
	button_scripting get_button_scripting() const {
		return button_scripting(text_base_data::flags & button_scripting_mask);
	}
};
static_assert(sizeof(button_data) == sizeof(text_base_data) + 8);


struct text_data : public text_base_data {
	static constexpr uint8_t background_mask = (0x03 << text_background_bit_offset);
	static constexpr uint8_t is_fixed_size_mask = (0x01 << (text_background_bit_offset + 2));
	static constexpr uint8_t is_instant_mask = (0x01 << (text_background_bit_offset + 3));
	static constexpr uint8_t is_edit_mask = (0x01 << (text_background_bit_offset + 4));

	xy_pair border_size; // 4bytes

	text_background get_text_background() const {
		return text_background(text_base_data::flags & background_mask);
	}
	bool is_fixed_size() const {
		return (text_base_data::flags & is_fixed_size_mask) != 0;
	}
	bool is_instant() const {
		return (text_base_data::flags & is_instant_mask) != 0;
	}
	bool is_edit() const {
		return (text_base_data::flags & is_edit_mask) != 0;
	}
};
static_assert(sizeof(text_data) == sizeof(text_base_data) + 4);

struct image_data {
	static constexpr uint8_t frame_mask = 0x7F;
	static constexpr uint8_t is_mask_mask = 0x80;

	float scale = 1.0f; // 4bytes
	dcon::gfx_object_id gfx_object; // 6bytes
	uint8_t flags = 0; // 7bytes

	bool is_mask() const {
		return (flags & is_mask_mask) != 0;
	}
	uint8_t frame() const {
		return (flags & frame_mask);
	}
};

struct overlapping_data {
	float spacing = 1.0f; // 4bytes
	alignment image_alignment = alignment::left; // 5bytes
};

struct list_box_data {
	xy_pair border_size; // 4bytes
	xy_pair offset; // 8bytes
	dcon::gfx_object_id background_image; // 10bytes
	uint8_t spacing = 0; // 11bytes
};

enum class step_size : uint8_t { // 2 bits
	one = 0x00,
	two = 0x01,
	one_tenth = 0x02,
	one_hundredth = 0x03,
	one_thousandth = 0x04,
	// Non-vanilla
	twenty_five = 0x40
};

struct scrollbar_data {
	static constexpr uint8_t step_size_mask = 0x47;
	static constexpr uint8_t is_range_limited_mask = 0x08;
	static constexpr uint8_t is_lockable_mask = 0x10;
	static constexpr uint8_t is_horizontal_mask = 0x20;

	xy_pair border_size; // 4bytes
	uint16_t max_value = 1; // 6bytes
	dcon::gui_def_id first_child; // 8bytes
	uint8_t num_children = 0; // 9bytes
	uint8_t flags = 0; // 10bytes

	step_size get_step_size() const {
		return step_size(step_size_mask & flags);
	}
	bool is_range_limited() const {
		return (flags & is_range_limited_mask) != 0;
	}
	bool is_lockable() const {
		return (flags & is_lockable_mask) != 0;
	}
	bool is_horizontal() const {
		return (flags & is_horizontal_mask) != 0;
	}
};

struct window_data {
	static constexpr uint8_t is_dialog_mask = 0x01;
	static constexpr uint8_t is_fullscreen_mask = 0x02;
	static constexpr uint8_t is_moveable_mask = 0x04;

	dcon::gui_def_id first_child; // 2bytes
	uint8_t num_children = 0; // 3bytes
	uint8_t flags = 0; // 4bytes

	bool is_dialog() const {
		return (flags & is_dialog_mask) != 0;
	}
	bool is_fullscreen() const {
		return (flags & is_fullscreen_mask) != 0;
	}
	bool is_moveable() const {
		return (flags & is_moveable_mask) != 0;
	}
};

struct position_data { };

struct element_data {
	static constexpr uint8_t type_mask = 0x07;
	static constexpr uint8_t rotation_mask = (0x03 << rotation_bit_offset);
	static constexpr uint8_t orientation_mask = (0x07 << orientation_bit_offset);

	static constexpr uint8_t ex_is_top_level = 0x01;

	xy_pair position; // 4bytes
	xy_pair size; // 8bytes
	dcon::text_key name; // 12bytes

	union alignas(4) internal_data {
		text_base_data text_common; // +5
		button_data button; // +5 + ? +3
		text_data text; // +5 + ? +4
		image_data image; // +6
		overlapping_data overlapping; //+5
		list_box_data list_box; // +11
		scrollbar_data scrollbar; //+10
		window_data window; // +4
		position_data position; //+0

		internal_data() {
			std::memset(this, 0, sizeof(internal_data));
			position = position_data{};
		}
	} data; // +16 = 28
	static_assert(sizeof(internal_data) == 16);

	uint8_t flags = 0; // 29
	uint8_t ex_flags = 0; // 30
	uint8_t padding[2] = { 0, 0 }; // 32

	element_data() {
		std::memset(this, 0, sizeof(element_data));
	}

	element_type get_element_type() const {
		return element_type(flags & type_mask);
	}
	rotation get_rotation() const {
		return rotation(flags & rotation_mask);
	}
	orientation get_orientation() const {
		return orientation(flags & orientation_mask);
	}
	bool is_top_level() const {
		return (ex_flags & ex_is_top_level) != 0;
	}
};
static_assert(sizeof(element_data) == 32);

struct window_extension {
	dcon::text_key window;
	dcon::gui_def_id child;
};

class definitions {
public:
	static constexpr dcon::texture_id small_tiles_dialog = dcon::texture_id(0);
	static constexpr dcon::texture_id tiles_dialog = dcon::texture_id(1);
	static constexpr dcon::texture_id transparency = dcon::texture_id(2);

	tagged_vector<gfx_object, dcon::gfx_object_id> gfx;
	tagged_vector<dcon::text_key, dcon::texture_id> textures;
	tagged_vector<element_data, dcon::gui_def_id> gui;
	std::vector<window_extension> extensions;

};

void load_text_gui_definitions(sys::state& state, parsers::building_gfx_context& context, parsers::error_handler& err);


class element_base;

xy_pair child_relative_location(sys::state& state, element_base const& parent, element_base const& child);
xy_pair get_absolute_location(sys::state& state, element_base const& node);

xy_pair child_relative_non_mirror_location(sys::state& state, element_base const& parent, element_base const& child);
xy_pair get_absolute_non_mirror_location(sys::state& state, element_base const& node);

using ui_hook_fn = std::unique_ptr<element_base> (*)(sys::state&, dcon::gui_def_id);

struct element_target {
	ui_hook_fn generator = nullptr;
	dcon::gui_def_id definition;
};

class tool_tip;
class grid_box;

template<class T>
class unit_details_window;

struct hash_text_key {
	using is_avalanching = void;
	using is_transparent = void;

	hash_text_key() { }

	auto operator()(dcon::text_key sv) const noexcept -> uint64_t {
		return ankerl::unordered_dense::detail::wyhash::hash(&sv, sizeof(sv));
	}
};

template<typename T>
constexpr ui_hook_fn hook() {
	return +[](sys::state&, dcon::gui_def_id) { return std::make_unique<T>(); };
}

void populate_definitions_map(sys::state& state);
void make_size_from_graphics(sys::state& state, ui::element_data& dat);
std::unique_ptr<element_base> make_element(sys::state& state, std::string_view name);
std::unique_ptr<element_base> make_element_immediate(sys::state& state, dcon::gui_def_id id); // bypasses global map
void place_in_drag_and_drop(sys::state& state, element_base& elm, std::any const& data, drag_and_drop_data type);

int32_t ui_width(sys::state const& state);
int32_t ui_height(sys::state const& state);

//void create_in_game_windows(sys::state& state);

} // namespace ui
