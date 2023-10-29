#include "system_state.hpp"

static sys::state game_state; // too big for the stack

int main(int argc, char **argv) {
	assert(
			std::string("NONE") !=
			GAME_DIR); // If this fails, then you have not created a local_user_settings.hpp (read the documentation for contributors)
	add_root(game_state.common_fs, NATIVE_M(GAME_DIR)); // game files directory is overlaid on top of that
	add_root(game_state.common_fs,
			NATIVE(".")); // will add the working directory as first root -- for the moment this lets us find the shader files

	if(argc >= 2) {
#ifndef NDEBUG
		{
			auto msg = std::string("Loading scenario  ") + simple_fs::native_to_utf8(argv[1]);
			window::emit_error_message(msg, false);
		}
#endif
		for(int i = 1; i < argc; ++i) {
			if(native_string(argv[i]) == NATIVE("-host")) {
				game_state.network_mode = sys::network_mode_type::host;
			} else if(native_string(argv[i]) == NATIVE("-join")) {
				game_state.network_mode = sys::network_mode_type::client;
				game_state.network_state.ip_address = "127.0.0.1";
				if(i + 1 < argc) {
					game_state.network_state.ip_address = simple_fs::native_to_utf8(native_string(argv[i + 1]));
					i++;
				}
			} else if(native_string(argv[i]) == NATIVE("-name")) {
				if(i + 1 < argc) {
					std::string nickname = simple_fs::native_to_utf8(native_string(argv[i + 1]));
					memcpy(game_state.network_state.nickname.data, nickname.data(), std::min<size_t>(nickname.length(), 8));
					i++;
				}
			} else if(native_string(argv[i]) == NATIVE("-v6")) {
				game_state.network_state.as_v6 = true;
			} else if(native_string(argv[i]) == NATIVE("-v4")) {
				game_state.network_state.as_v6 = false;
			}
		}

		if(sys::try_read_scenario_and_save_file(game_state, argv[1])) {
			game_state.fill_unsaved_data();
		} else {
			auto msg = std::string("Scenario file ") + simple_fs::native_to_utf8(argv[1]) + " could not be read";
			window::emit_error_message(msg, true);
			return 0;
		}

		network::init(game_state);
	}

	// scenario loading functions (would have to run these even when scenario is pre-built
	game_state.load_user_settings();
	text::load_standard_fonts(game_state);
	text::load_bmfonts(game_state);
	ui::populate_definitions_map(game_state);

	std::thread update_thread([&]() { game_state.game_loop(); });

	window::create_window(game_state, window::creation_parameters());

	game_state.quit_signaled.store(true, std::memory_order_release);
	update_thread.join();

	return EXIT_SUCCESS;
}
