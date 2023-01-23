#include "system_state.hpp"

int main() {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack

	window::create_window(*game_state, window::creation_parameters());
	return EXIT_SUCCESS;
}