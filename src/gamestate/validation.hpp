#pragma once

template <bool VALIDATE>
bool inline assertive_identity(bool input) {
	if constexpr(VALIDATE) {
		assert(input);
	}
	return input;
}
