#pragma once

namespace ogl {
enum class color_modification {
	none, disabled, interactable, interactable_disabled
};
}

namespace text {
enum class text_color : uint8_t {
	black,
	white,
	red,
	green,
	yellow,
	unspecified,
	light_blue,
	dark_blue,
	orange,
	lilac,
	light_grey,
	dark_red,
	dark_green,
	gold,
	reset,
	brown
};
enum class alignment : uint8_t {
	left, right, center
};
}

namespace ui{

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

inline constexpr int32_t clicksound_bit_offset = 2;
enum class clicksound : uint16_t { // 2 bits
	none = (0x00 << clicksound_bit_offset),
	click = (0x01 << clicksound_bit_offset),
	close_window = (0x02 << clicksound_bit_offset),
	start_game = (0x03 << clicksound_bit_offset)
};

inline constexpr int32_t checkbox_bit_offset = clicksound_bit_offset + 2;

inline constexpr int32_t button_scripting_bit_offset = checkbox_bit_offset + 1;
enum class button_scripting : uint16_t { // 2 bits
	none = (0x00 << button_scripting_bit_offset),
	province = (0x01 << button_scripting_bit_offset),
	nation = (0x02 << button_scripting_bit_offset)
};

inline constexpr int32_t text_background_bit_offset = 2;
enum class text_background : uint8_t { // 2 bits
	none = (0x00 << text_background_bit_offset),
	tiles_dialog = (0x01 << text_background_bit_offset),
	transparency = (0x02 << text_background_bit_offset),
	small_tiles_dialog = (0x03 << text_background_bit_offset)
};

}


namespace text {
enum class embedded_icon : uint8_t {
	check, xmark, check_desaturated, xmark_desaturated, army, navy
};
}
