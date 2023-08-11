#include "system_state.hpp"

int main(int argc, char **argv) {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack

	assert(
			std::string("NONE") !=
			GAME_DIR); // If this fails, then you have not created a local_user_settings.hpp (read the documentation for contributors)
	add_root(game_state->common_fs, NATIVE_M(GAME_DIR)); // game files directory is overlaid on top of that
	add_root(game_state->common_fs,
			NATIVE(".")); // will add the working directory as first root -- for the moment this lets us find the shader files

	if(argv && argc > 1) {
		std::vector<native_string> cmd_args;
		for(int i = 1; i < argc; ++i)
			cmd_args.push_back(native_string{ argv[i] });
		auto root = get_root(game_state->common_fs);
		for(const auto& cmd_arg : cmd_args) {
			if(cmd_arg == "-s") {
				game_state->network_state.init(*game_state, true);
			} else if(cmd_arg == "-c") {
				game_state->network_state.init(*game_state, false);
			} else {
				auto mod_file = open_file(root, cmd_arg);
				if(mod_file) {
					parsers::error_handler err("");
					parsers::scenario_building_context context(*game_state);
					auto content = view_contents(*mod_file);
					err.file_name = cmd_arg;
					parsers::token_generator gen(content.data, content.data + content.file_size);
					parsers::mod_file_context mod_file_context(context);
					parsers::parse_mod_file(gen, err, mod_file_context);
				}
			}
		}
	}

	if(!sys::try_read_scenario_and_save_file(*game_state, NATIVE("development_test_file.bin"))) {
		// scenario making functions
		game_state->load_scenario_data();
		game_state->local_player_nation = dcon::nation_id{8};
		game_state->world.nation_set_is_player_controlled(game_state->local_player_nation, true);
		sys::write_scenario_file(*game_state, NATIVE("development_test_file.bin"));
	} else {
		game_state->fill_unsaved_data();
	}
	std::thread client_thread([&]() { game_state->network_state.server_client_loop(*game_state, 0); });

	// scenario loading functions (would have to run these even when scenario is pre-built
	game_state->load_user_settings();
	text::load_standard_fonts(*game_state);
	text::load_bmfonts(*game_state);
	ui::populate_definitions_map(*game_state);

	std::thread update_thread([&]() { game_state->game_loop(); });

	window::create_window(*game_state, window::creation_parameters());

	game_state->quit_signaled.store(true, std::memory_order_release);
	update_thread.join();
	client_thread.join();

	return EXIT_SUCCESS;
}
