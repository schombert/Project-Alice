#ifdef LOCAL_USER_SETTINGS
#include "local_user_settings.hpp"
#endif
#include "parsers.cpp"
#include "defines.cpp"
#include "float_from_chars.cpp"
#include "system_state.cpp"
#include "gui_graphics_parsers.cpp"
#include "text.cpp"
#include "fonts.cpp"
#include "texture.cpp"
#include "gui_graphics.cpp"
#include "gui_element_types.cpp"
#include "gui_main_menu.cpp"
#include "gui_console.cpp"
#include "gui_province_window.cpp"
#include "gui_budget_window.cpp"
#include "gui_technology_window.cpp"
#include "gui_event.cpp"
#include "nations_parsing.cpp"
#include "cultures_parsing.cpp"
#include "econ_parsing.cpp"
#include "military_parsing.cpp"
#include "date_interface.cpp"
#include "provinces_parsing.cpp"
#include "trigger_parsing.cpp"
#include "effect_parsing.cpp"
#include "serialization.cpp"
#include "nations.cpp"
#include "culture.cpp"
#include "military.cpp"
#include "modifiers.cpp"
#include "province.cpp"
#include "triggers.cpp"
#include "effects.cpp"
#include "economy.cpp"
#include "demographics.cpp"
#include "bmfont.cpp"
#include "rebels.cpp"
#include "parsers_declarations.cpp"
#include "politics.cpp"
#include "events.cpp"
#include "gui_effect_tooltips.cpp"
#include "gui_trigger_tooltips.cpp"
#include "gui_modifier_tooltips.cpp"

#ifdef _WIN64
// WINDOWS implementations go here

#include "simple_fs_win.cpp"
#include "window_win.cpp"
#include "sound_win.cpp"
#include "opengl_wrapper_win.cpp"

#ifndef ALICE_NO_ENTRY_POINT
#include "entry_point_win.cpp"
#endif

#else
// LINUX implementations go here

#include "simple_fs_nix.cpp"
#include "window_nix.cpp"
#include "sound_nix.cpp"
#include "opengl_wrapper_nix.cpp"

#ifndef ALICE_NO_ENTRY_POINT
#include "entry_point_nix.cpp"
#endif

#endif

#include "opengl_wrapper.cpp"
#include "map.cpp"
#include "map_modes.cpp"
#include "prng.cpp"

namespace sys {
	state::~state() {
		// why does this exist ? So that the destructor of the unique pointer doesn't have to be known before it is implemented
	}
}

// zstd

#define XXH_NAMESPACE ZSTD_
#define ZSTD_DISABLE_ASM

#include "zstd/xxhash.c"
#include "zstd/zstd_decompress_block.c"
#include "zstd/zstd_ddict.c"
#include "zstd/huf_compress.c"
#include "zstd/fse_compress.c"
#include "zstd/huf_decompress.c"
#include "zstd/fse_decompress.c"
#include "zstd/zstd_common.c"
#include "zstd/entropy_common.c"
#include "zstd/hist.c"
#include "zstd/zstd_compress_superblock.c"
#include "zstd/zstd_ldm.c"
#include "zstd/zstd_opt.c"
#include "zstd/zstd_lazy.c"
#include "zstd/zstd_double_fast.c"
#include "zstd/zstd_fast.c"
#include "zstd/zstd_compress_literals.c"
#include "zstd/zstd_compress_sequences.c"
#include "zstd/error_private.c"
#include "zstd/zstd_decompress.c"
#include "zstd/zstd_compress.c"
