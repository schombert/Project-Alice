#pragma once
#include <string>
#include <string_view>

using native_string = std::string;
using native_string_view = std::string_view;
using native_char = char;
template<typename T>
native_string to_native_string(T&& v) noexcept {
    return std::to_string(v);
}

#define NATIVE(X) X
#define NATIVE_M(X) NATIVE(X)
#define NATIVE_DIR_SEPARATOR '/'
