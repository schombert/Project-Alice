#pragma once

namespace ogl {
enum class color_modification {
	none, disabled, interactable, interactable_disabled
};
namespace parameters {
inline constexpr GLuint screen_width = 0;
inline constexpr GLuint screen_height = 1;
inline constexpr GLuint drawing_rectangle = 2;

inline constexpr GLuint border_size = 6;
inline constexpr GLuint inner_color = 7;
inline constexpr GLuint subrect = 10;

inline constexpr GLuint enabled = 4;
inline constexpr GLuint disabled = 3;
inline constexpr GLuint border_filter = 0;
inline constexpr GLuint filter = 1;
inline constexpr GLuint no_filter = 2;
inline constexpr GLuint sub_sprite = 5;
inline constexpr GLuint use_mask = 6;
inline constexpr GLuint progress_bar = 7;
inline constexpr GLuint frame_stretch = 8;
inline constexpr GLuint piechart = 9;
inline constexpr GLuint barchart = 10;
inline constexpr GLuint linegraph = 11;
inline constexpr GLuint tint = 12;
inline constexpr GLuint interactable = 13;
inline constexpr GLuint interactable_disabled = 14;
inline constexpr GLuint subsprite_b = 15;
inline constexpr GLuint alternate_tint = 16;
inline constexpr GLuint linegraph_color = 17;
inline constexpr GLuint transparent_color = 18;
inline constexpr GLuint solid_color = 19;
inline constexpr GLuint alpha_color = 20;
inline constexpr GLuint subsprite_c = 21;
inline constexpr GLuint linegraph_acolor = 22;
inline constexpr GLuint stripchart = 23;
inline constexpr GLuint triangle_strip = 24;
inline constexpr GLuint border_repeat = 25;
inline constexpr GLuint corner_repeat = 26;
} // namespace parameters
}
