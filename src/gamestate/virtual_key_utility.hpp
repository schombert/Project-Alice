#pragma once
#include "constants.hpp"

namespace sys {

bool is_shift(const virtual_key key) {
    return key == virtual_key::SHIFT || key == virtual_key::LSHIFT || key == virtual_key::RSHIFT;
}
bool is_ctrl(const virtual_key key) {
    return key == virtual_key::CONTROL || key == virtual_key::LCONTROL || key == virtual_key::RCONTROL;
}
}
