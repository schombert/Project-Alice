#pragma once

#include <vector>
#include <array>
#include "constants.hpp"
#include "dcon_generated.hpp"
#include "unordered_dense.h"
#include "container_types.hpp"
#include "constants.hpp"
#include "parsers.hpp"
#include "cyto_any.hpp"

namespace parsers {
struct building_gfx_context;
}

namespace ui {
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

struct xy_pair {
	int16_t x = 0;
	int16_t y = 0;
};

struct gfx_object {
	constexpr static uint8_t always_transparent = 0x10;
	constexpr static uint8_t flip_v = 0x20;
	constexpr static uint8_t has_click_sound = 0x40;
	constexpr static uint8_t do_transparency_check = 0x80;

	constexpr static uint8_t type_mask = 0x0F;

	xy_pair size; // 4bytes

	dcon::texture_id primary_texture_handle; // 6bytes
	uint16_t type_dependent = 0;						 // secondary texture handle or border size -- 8bytes

	uint8_t flags = 0;						// 9bytes
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

enum class element_type : uint8_t { // 3 bits
	button = 0x01,
	text = 0x02,
	image = 0x03,
	position = 0x04,
	overlapping = 0x05,
	listbox = 0x06,
	scrollbar = 0x07,
	window = 0x00
};

enum class alignment : uint8_t { // 2 bits
	left = 0x00,
	right = 0x01,
	centered = 0x02,
	justified = 0x03
};

inline constexpr int32_t rotation_bit_offset = 3;
enum class rotation : uint8_t { // 2 bits
	upright = (0x00 << rotation_bit_offset),
	r90_left = (0x01 << rotation_bit_offset),
	r90_right = (0x02 << rotation_bit_offset)
};

inline constexpr int32_t orientation_bit_offset = 5;
enum class orientation : uint8_t { // 3 bits
	upper_left = (0x00 << orientation_bit_offset),
	upper_right = (0x01 << orientation_bit_offset),
	lower_left = (0x02 << orientation_bit_offset),
	lower_right = (0x03 << orientation_bit_offset),
	upper_center = (0x04 << orientation_bit_offset),
	lower_center = (0x05 << orientation_bit_offset),
	center = (0x06 << orientation_bit_offset)
};

struct text_base_data {
	static constexpr uint8_t alignment_mask = 0x03;

	dcon::text_sequence_id txt;
	uint16_t font_handle = 0;
	uint8_t flags = 0;

	alignment get_alignment() const {
		return alignment(flags & alignment_mask);
	}
};

inline constexpr int32_t clicksound_bit_offset = 2;
enum class clicksound : uint8_t { // 2 bits
	none = (0x00 << clicksound_bit_offset),
	click = (0x01 << clicksound_bit_offset),
	close_window = (0x02 << clicksound_bit_offset),
	start_game = (0x03 << clicksound_bit_offset)
};

struct button_data : public text_base_data {
	static constexpr uint8_t clicksound_mask = (0x03 << clicksound_bit_offset);
	static constexpr uint8_t is_checkbox_mask = (0x01 << (clicksound_bit_offset + 2));

	dcon::gfx_object_id button_image;
	sys::virtual_key shortcut = sys::virtual_key::NONE;

	clicksound get_clicksound() const {
		return clicksound(text_base_data::flags & clicksound_mask);
	}
	bool is_checkbox() const {
		return (text_base_data::flags & is_checkbox_mask) != 0;
	}
};

inline constexpr int32_t text_background_bit_offset = 2;
enum class text_background : uint8_t { // 2 bits
	none = (0x00 << text_background_bit_offset),
	tiles_dialog = (0x01 << text_background_bit_offset),
	transparency = (0x02 << text_background_bit_offset),
	small_tiles_dialog = (0x03 << text_background_bit_offset)
};

struct text_data : public text_base_data {
	static constexpr uint8_t background_mask = (0x03 << text_background_bit_offset);
	static constexpr uint8_t is_fixed_size_mask = (0x01 << (text_background_bit_offset + 2));
	static constexpr uint8_t is_instant_mask = (0x01 << (text_background_bit_offset + 3));
	static constexpr uint8_t is_edit_mask = (0x01 << (text_background_bit_offset + 4));

	xy_pair border_size;

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

struct image_data {
	static constexpr uint8_t frame_mask = 0x7F;
	static constexpr uint8_t is_mask_mask = 0x80;

	float scale = 1.0f;
	dcon::gfx_object_id gfx_object;

	uint8_t flags = 0;

	bool is_mask() const {
		return (flags & is_mask_mask) != 0;
	}
	uint8_t frame() const {
		return (flags & frame_mask);
	}
};

struct overlapping_data {
	float spacing = 1.0f;
	alignment image_alignment = alignment::left;
};

struct list_box_data {
	xy_pair border_size;
	xy_pair offset;
	dcon::gfx_object_id background_image;
	uint8_t spacing = 0;
};

enum class step_size : uint8_t { // 2 bits
	one = 0x00,
	two = 0x01,
	one_tenth = 0x02,
	one_hundredth = 0x03,
	one_thousandth = 0x04
};

struct scrollbar_data {
	static constexpr uint8_t step_size_mask = 0x07;
	static constexpr uint8_t is_range_limited_mask = 0x08;
	static constexpr uint8_t is_lockable_mask = 0x10;
	static constexpr uint8_t is_horizontal_mask = 0x20;

	xy_pair border_size;
	uint16_t max_value = 1;
	dcon::gui_def_id first_child;
	uint8_t num_children = 0;

	uint8_t flags = 0;

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

	dcon::gui_def_id first_child;
	uint8_t num_children = 0;
	uint8_t flags = 0;

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

	xy_pair position;		 // 4
	xy_pair size;				 // 8
	dcon::text_key name; // 12

	union alignas(4) internal_data {
		text_base_data text_common;		// +5
		button_data button;						// +5 + ? +3
		text_data text;								// +5 + ? +4
		image_data image;							// +6
		overlapping_data overlapping; //+5
		list_box_data list_box;				// +11
		scrollbar_data scrollbar;			//+10
		window_data window;						// +4
		position_data position;				//+0

		internal_data() {
			position = position_data{};
		}
	} data; // +12 = 24

	uint8_t flags = 0;		// 25
	uint8_t ex_flags = 0; // 26

	element_data() {
		memset(this, 0, sizeof(element_data));
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
static_assert(sizeof(element_data) == 28);

class definitions {
public:
	static constexpr dcon::texture_id small_tiles_dialog = dcon::texture_id(0);
	static constexpr dcon::texture_id tiles_dialog = dcon::texture_id(1);
	static constexpr dcon::texture_id transparency = dcon::texture_id(2);

	tagged_vector<gfx_object, dcon::gfx_object_id> gfx;
	tagged_vector<dcon::text_key, dcon::texture_id> textures;
	tagged_vector<element_data, dcon::gui_def_id> gui;
};

void load_text_gui_definitions(sys::state& state, parsers::building_gfx_context& context, parsers::error_handler& err);

enum class message_result { unseen, seen, consumed };
enum class focus_result { ignored, accepted };
enum class tooltip_behavior { tooltip, variable_tooltip, position_sensitive_tooltip, no_tooltip };

class element_base;

xy_pair child_relative_location(element_base const& parent, element_base const& child);
xy_pair get_absolute_location(element_base const& node);

using ui_hook_fn = std::unique_ptr<element_base> (*)(sys::state&, dcon::gui_def_id);

struct element_target {
	ui_hook_fn generator = nullptr;
	dcon::gui_def_id definition;
};

class tool_tip;
class grid_box;

template<class T>
class unit_details_window;

struct chat_message {
	dcon::nation_id source{};
	dcon::nation_id target{};
	std::string body;

	chat_message() = default;
	chat_message(const chat_message&) = default;
	chat_message(chat_message&&) = default;
	chat_message& operator=(const chat_message&) = default;
	chat_message& operator=(chat_message&&) = default;
	~chat_message() = default;
};

struct state {
	element_base* under_mouse = nullptr;
	element_base* scroll_target = nullptr;
	element_base* drag_target = nullptr;
	element_base* edit_target = nullptr;
	element_base* last_tooltip = nullptr;
	element_base* mouse_sensitive_target = nullptr;
	xy_pair target_ul_bounds = xy_pair{ 0, 0 };
	xy_pair target_lr_bounds = xy_pair{ 0, 0 };

	xy_pair relative_mouse_location = xy_pair{0, 0};
	std::unique_ptr<element_base> units_root;
	std::unique_ptr<element_base> rgos_root;
	std::unique_ptr<element_base> root;
	std::unique_ptr<element_base> nation_picker;
	std::unique_ptr<element_base> end_screen;
	std::unique_ptr<tool_tip> tooltip;
	std::unique_ptr<grid_box> unit_details_box;
	ankerl::unordered_dense::map<std::string_view, element_target> defs_by_name;

	// elements we are keeping track of
	element_base* main_menu = nullptr; // Settings window
	element_base* r_main_menu = nullptr; // Settings window for non-in-game modes
	element_base* fps_counter = nullptr;
	element_base* console_window = nullptr; // console window
	element_base* topbar_window = nullptr;
	element_base* topbar_subwindow = nullptr; // current tab window
	element_base* province_window = nullptr;
	element_base* search_window = nullptr;
	element_base* ledger_window = nullptr;
	element_base* diplomacy_subwindow = nullptr;
	element_base* politics_subwindow = nullptr;
	element_base* population_subwindow = nullptr;
	element_base* production_subwindow = nullptr;
	element_base* trade_subwindow = nullptr;
	element_base* unit_window_army = nullptr;
	element_base* unit_window_navy = nullptr;
	element_base* build_unit_window = nullptr;
	element_base* msg_filters_window = nullptr;
	element_base* outliner_window = nullptr;
	element_base* technology_subwindow = nullptr;
	element_base* military_subwindow = nullptr;
	element_base* election_window = nullptr;
	element_base* request_window = nullptr;
	unit_details_window<dcon::army_id>* army_status_window = nullptr;
	unit_details_window<dcon::navy_id>* navy_status_window = nullptr;
	element_base* multi_unit_selection_window = nullptr;
	element_base* msg_log_window = nullptr;
	element_base* msg_window = nullptr;
	element_base* main_menu_win = nullptr; // The actual main menu
	element_base* chat_window = nullptr;
	std::array<chat_message, 32> chat_messages;
	uint8_t chat_messages_index = 0;

	element_base* major_event_window = nullptr;
	element_base* national_event_window = nullptr;
	element_base* provincial_event_window = nullptr;

	element_base* naval_combat_window = nullptr;
	element_base* army_combat_window = nullptr;

	element_base* change_leader_window = nullptr;

	std::vector<std::unique_ptr<element_base>> endof_landcombat_windows;
	std::vector<std::unique_ptr<element_base>> endof_navalcombat_windows;

	int32_t held_game_speed = 1; // used to keep track of speed while paused

	uint16_t tooltip_font = 0;

	state();
	~state();
};

struct mouse_probe {
	element_base* under_mouse;
	xy_pair relative_location;
};

template<typename T>
constexpr ui_hook_fn hook() {
	return +[](sys::state&, dcon::gui_def_id) { return std::make_unique<T>(); };
}

void populate_definitions_map(sys::state& state);
void make_size_from_graphics(sys::state& state, ui::element_data& dat);
std::unique_ptr<element_base> make_element(sys::state& state, std::string_view name);
std::unique_ptr<element_base> make_element_immediate(sys::state& state, dcon::gui_def_id id); // bypasses global map

void show_main_menu(sys::state& state);
int32_t ui_width(sys::state const& state);
int32_t ui_height(sys::state const& state);
} // namespace ui
