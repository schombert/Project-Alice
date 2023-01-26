#pragma once

#include <vector>
#include "constants.hpp"
#include "dcon_generated.hpp"
#include "unordered_dense.h"
#include "container_types.hpp"
#include "constants.hpp"
#include "parsers.hpp"
#include "cyto_any.hpp"

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

		xy_pair size; //4bytes

		dcon::texture_id primary_texture_handle; //6bytes
		uint16_t type_dependant = 0; // secondary texture handle or border size -- 8bytes

		uint8_t flags = 0; //9bytes
		uint8_t number_of_frames = 1; //10bytes
	};

	enum class element_type : uint8_t {  // 3 bits
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

	struct position_data {

	};

	struct element_data {
		static constexpr uint8_t type_mask = 0x07;
		static constexpr uint8_t rotation_mask = (0x03 << rotation_bit_offset);
		static constexpr uint8_t orientation_mask = (0x07 << orientation_bit_offset);

		xy_pair position; // 4
		xy_pair size;     // 8

		union internal_data {
			text_base_data text_common;
			button_data button;
			text_data text;
			image_data image;
			overlapping_data overlapping;
			list_box_data list_box;
			scrollbar_data scrollbar;
			window_data window;
			position_data position;

			internal_data() { 
				position = position_data{};
			}
		} data;

		dcon::text_key name;
		uint8_t flags = 0;

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
	};


	class definitions {
	public:
		static constexpr dcon::texture_id small_tiles_dialog = dcon::texture_id(0);
		static constexpr dcon::texture_id tiles_dialog = dcon::texture_id(1);
		static constexpr dcon::texture_id transparency = dcon::texture_id(2);

		tagged_vector<gfx_object, dcon::gfx_object_id> gfx;
		tagged_vector<dcon::text_key, dcon::texture_id> textures;
		tagged_vector<element_data, dcon::gui_def_id> gui;
	};

	void load_text_gui_definitions(sys::state& state, parsers::error_handler& err);

	enum class message_result {
		unseen, seen, consumed
	};
	enum class focus_result {
		ingored, accepted
	};
	enum class tooltip_behavior {
		tooltip,
		variable_tooltip,
		no_tooltip,
		transparent
	};
	class element_base {
	public:
		static constexpr uint8_t is_invisible_mask = 0x01;

		element_data base_data;
		element_base* parent = nullptr;
		uint8_t flags = 0;


		bool is_visible() const {
			return (flags & is_invisible_mask) == 0;
		}
		void set_visible(sys::state& state, bool vis) {
			flags = uint8_t((flags & ~is_invisible_mask) | (vis ? 0 : is_invisible_mask));
			if(vis)
				on_visible(state);
		}

		element_base() { }


		// impl members: to be overridden only for the very basic container / not a container distinction
		//       - are responsible for propagating messages and responses
		//       - should be called from outside the class
		virtual message_result impl_test_mouse(sys::state& state, int32_t x, int32_t y) noexcept;
		virtual message_result impl_on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept;
		virtual message_result impl_on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept;
		virtual message_result impl_on_drag(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept;
		virtual message_result impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept;
		virtual message_result impl_on_text(sys::state& state, char ch) noexcept;
		virtual message_result impl_on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept;
		virtual void impl_on_update(sys::state& state) noexcept;
		message_result impl_get(Cyto::Any& payload) noexcept;
		virtual message_result impl_set(Cyto::Any& payload) noexcept;

		// these message handlers can be overridden by basically anyone
		//        - generally *should not* be called directly

		virtual message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept; // asks whether the mouse would be intercepted here, but without taking an action
		virtual message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept;
		virtual message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept;
		virtual message_result on_drag(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept; // as drag events are generated
		virtual message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept;
		virtual message_result on_text(sys::state& state, char ch) noexcept;
		virtual message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept;
		virtual void on_update(sys::state& state) noexcept;
		virtual message_result get(Cyto::Any& payload) noexcept;
		virtual message_result set(Cyto::Any& payload) noexcept;

		// as above, but they need no internal framework to pass messages up or down
		//         - may be called directly
		virtual void on_resize(sys::state& state, int32_t x, int32_t y) noexcept { } // used to get a parent to reposition its children
		virtual focus_result on_get_focus(sys::state& state) noexcept { // used to both react to getting the focus and to accept or reject it
			return focus_result::ingored;
		}
		virtual void on_lose_focus(sys::state& state) noexcept { } // called when the focus is taken away
		virtual void on_drag_finish(sys::state& state) noexcept { } // when the mouse is released, and drag ends
		virtual void on_visible(sys::state& state) noexcept { }
		virtual tooltip_behavior has_tooltip(sys::state& state, int32_t x, int32_t y) noexcept { // used to test whether a tooltip is possible
			return tooltip_behavior::transparent;
		}
		virtual void create_tooltip(sys::state& state, int32_t x, int32_t y, element_base& /*tooltip_window*/) noexcept { }

		virtual void on_create(sys::state& state) noexcept { } // called automatically after the element has been created by the system

		// these commands are meaningful only if the element has children
		virtual std::unique_ptr<element_base> remove_child(element_base* child) noexcept {
			return std::unique_ptr<element_base>{};
		}
		virtual void move_child_to_front(element_base* child) noexcept { }
		virtual void move_child_to_back(element_base* child) noexcept { }
		virtual void add_child_to_front(std::unique_ptr<element_base> child) noexcept { }
		virtual void add_child_to_back(std::unique_ptr<element_base> child) noexcept { }

		virtual ~element_base() { }
	};
}
