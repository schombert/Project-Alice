#pragma once
#include <string>
#include <string_view>

using native_string = std::wstring;
using native_string_view = std::wstring_view;
using native_char = wchar_t;

#define NATIVE(X) L ## X
#define NATIVE_M(X) NATIVE(X)
