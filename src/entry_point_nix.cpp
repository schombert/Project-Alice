#include "system_state.hpp"

int main() {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack

	assert(std::string("NONE") != GAME_DIR); // If this fails, then you have not created a local_user_settings.hpp (read the documentation for contributors)
	add_root(game_state->common_fs, NATIVE(".")); // will add the working directory as first root -- for the moment this lets us find the shader files
	add_root(game_state->common_fs, NATIVE_M(GAME_DIR)); // game files directory is overlaid on top of that


	// scenario making functions
	game_state->load_scenario_data();

	// scenario loading functions (would have to run these even when scenario is pre-built
	game_state->load_user_settings();
	text::load_standard_fonts(*game_state);
	ui::populate_definitions_map(*game_state);
	game_state->open_gl.asset_textures.resize(game_state->ui_defs.textures.size());

	window::create_window(*game_state, window::creation_parameters());
	return EXIT_SUCCESS;
}
