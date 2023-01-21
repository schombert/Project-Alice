#include "parsers.cpp"
#include "float_from_chars.cpp"
#include "system_state.cpp"
#include "opengl_wrapper.cpp"

#ifdef _WIN64
// WINDOWS implementations go here

#include "simple_fs_win.cpp"

#include "entry_point_win.cpp"

#else
// LINUX implementations go here

#include "simple_fs_nix.cpp"

#include "entry_point_nix.cpp"

#endif
