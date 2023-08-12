#pragma once
#include <string>
#include <string_view>

using native_string = std::wstring;
using native_string_view = std::wstring_view;
using native_char = wchar_t;
template<typename T>
native_string to_native_string(T&& v) noexcept {
    return std::to_wstring(v);
}

#define NATIVE(X) L##X
#define NATIVE_M(X) NATIVE(X)
#define NATIVE_DIR_SEPARATOR L'\\'
