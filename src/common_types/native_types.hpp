#pragma once

#ifdef _WIN64
// WINDOWS typedefs go here
#include "native_types_win.hpp"

#else
// LINUX typedefs go here
#include "native_types_nix.hpp"
#endif
