#include "system_state.hpp"

#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <shellapi.h>
#include "Objbase.h"
#include "window.hpp"

#pragma comment(lib, "Ole32.lib")

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR lpCmdLine, int /*nCmdShow*/
) {

#ifdef _DEBUG
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#endif

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	if(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
		// do everything here: create a window, read messages

		std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack

		if(std::string("NONE") != GAME_DIR) {									 // check for user-defined location
			add_root(game_state->common_fs, NATIVE_M(GAME_DIR)); // game files directory is overlaid on top of that
			add_root(game_state->common_fs, NATIVE("."));				 // for the moment this lets us find the shader files
		} else { // before exiting, check if they've installed the game and it's told us where via the registry
			HKEY hKey;
			LSTATUS res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Paradox Interactive\\Victoria 2", 0, KEY_READ,
					&hKey); // open key if key exists
			if(res != ERROR_SUCCESS) {
				assert(false); // victoria 2 could not be located, see the "Interested in Contributing?" page on the github.
			}
			WCHAR szBuffer[256]; // excessive but just in case someone has their game directory NESTED
			DWORD lnBuffer = 256;
			res = RegQueryValueEx(hKey, L"path", NULL, NULL, reinterpret_cast<LPBYTE>(szBuffer), &lnBuffer);
			if(res != ERROR_SUCCESS) {
				assert(false); // victoria 2 could not be located, see the "Interested in Contributing?" page on the github.
			}
			add_root(game_state->common_fs, szBuffer);		// game files directory is overlaid on top of that
			add_root(game_state->common_fs, NATIVE(".")); // for the moment this lets us find the shader files
			RegCloseKey(hKey);
		}

		if(lpCmdLine) {
			int argc = 0;
			LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
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
		}
		std::thread client_thread([&]() { game_state->network_state.server_client_loop(*game_state, 0); });

		if(!sys::try_read_scenario_and_save_file(*game_state, NATIVE("development_test_file.bin"))) {
			// scenario making functions
			game_state->load_scenario_data();
			game_state->local_player_nation = dcon::nation_id{8};
			game_state->world.nation_set_is_player_controlled(game_state->local_player_nation, true);
			sys::write_scenario_file(*game_state, NATIVE("development_test_file.bin"));
		} else {
			game_state->fill_unsaved_data();
		}

		// scenario loading functions (would have to run these even when scenario is pre-built
		game_state->load_user_settings();
		text::load_standard_fonts(*game_state);
		text::load_bmfonts(*game_state);
		ui::populate_definitions_map(*game_state);

		std::thread update_thread([&]() { game_state->game_loop(); });

		// entire game runs during this line
		window::create_window(*game_state,
				window::creation_parameters{1024, 780, window::window_state::maximized, game_state->user_settings.prefer_fullscreen});

		game_state->quit_signaled.store(true, std::memory_order_release);
		update_thread.join();
		client_thread.join();
		CoUninitialize();
	}
	return 0;
}
