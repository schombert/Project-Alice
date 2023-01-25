#pragma once
#include <string>
#include <string_view>

using native_string = std::string;
using native_string_view = std::string_view;
using native_char = char;

#define NATIVE(X) X
#define NATIVE_M(X) NATIVE(X)
