#undef _USE_STD_VECTOR_ALGORITHMS
#define _USE_STD_VECTOR_ALGORITHMS 0

#ifdef LOCAL_USER_SETTINGS
#include "local_user_settings.hpp"
#endif
#include "common_types.cpp"
#include "system_state.cpp"
#ifndef INCREMENTAL
#include "parsers.cpp"
#include "text.cpp"
#include "float_from_chars.cpp"
#include "fonts.cpp"
#include "texture.cpp"
#include "date_interface.cpp"
#include "serialization.cpp"
#include "nations.cpp"
#include "culture.cpp"
#include "military.cpp"
#include "modifiers.cpp"
#include "province.cpp"
#include "triggers.cpp"
#include "effects.cpp"
#include "economy.cpp"

#include "ai.cpp"
#else
#include "text.hpp"
#include "fonts.hpp"
#include "texture.hpp"
#include "date_interface.hpp"
#include "serialization.hpp"
#endif
#include "demographics.cpp"
#include "bmfont.cpp"
#include "rebels.cpp"
#include "politics.cpp"
#include "events.cpp"
#include "gui_graphics.cpp"
#include "gui_element_types.cpp"
#include "gui_common_elements.cpp"
#include "gui_main_menu.cpp"
#include "gui_console.cpp"
#include "gui_province_window.cpp"
#include "gui_budget_window.cpp"
#include "gui_technology_window.cpp"
#include "gui_event.cpp"
#include "gui_message_settings_window.cpp"
#include "gui_trigger_tooltips.cpp"
#include "gui_effect_tooltips.cpp"
#include "gui_modifier_tooltips.cpp"
#include "commands.cpp"
#include "network.cpp"
#include "diplomatic_messages.cpp"
#include "notifications.cpp"
#include "map_tooltip.cpp"
#include "unit_tooltip.cpp"

#ifdef _WIN64
// WINDOWS implementations go here

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

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
#include "map_modes.cpp"
#include "prng.cpp"
extern "C" {
#include "blake2.c"
};

// zstd
extern "C" {
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
};
