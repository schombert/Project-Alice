#include "system_state.hpp"

#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "Objbase.h"
#include "window.hpp"

#pragma comment(lib, "Ole32.lib")

int WINAPI wWinMain(
    HINSTANCE /*hInstance*/,
    HINSTANCE /*hPrevInstance*/,
    LPWSTR /*lpCmdLine*/,
    int /*nCmdShow*/
) {

#ifdef _DEBUG
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#endif

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
		// do everything here: create a window, read messages

		std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack

		if (std::string("NONE") != GAME_DIR) {                   // check for user-defined location
			add_root(game_state->common_fs, NATIVE_M(GAME_DIR)); // game files directory is overlaid on top of that
			add_root(game_state->common_fs, NATIVE("."));        // for the moment this lets us find the shader files
		} else {                                                 // before exiting, check if they've installed the game and it's told us where via the registry
			HKEY hKey;
			LSTATUS res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Paradox Interactive\\Victoria 2", 0, KEY_READ, &hKey); // open key if key exists
			if (res != ERROR_SUCCESS) {
				assert(false); // victoria 2 could not be located, see the "Interested in Contributing?" page on the github.
			}
			WCHAR szBuffer[256]; // excessive but just in case someone has their game directory NESTED
			DWORD lnBuffer = 256;
			res = RegQueryValueEx(hKey, L"path", NULL, NULL, reinterpret_cast<LPBYTE>(szBuffer), &lnBuffer);
			if (res != ERROR_SUCCESS) {
				assert(false); // victoria 2 could not be located, see the "Interested in Contributing?" page on the github.
			}
			add_root(game_state->common_fs, szBuffer);    // game files directory is overlaid on top of that
			add_root(game_state->common_fs, NATIVE(".")); // for the moment this lets us find the shader files
			RegCloseKey(hKey);
		}

		if (!sys::try_read_scenario_and_save_file(*game_state, NATIVE("development_test_file.bin"))) {
			// scenario making functions
			game_state->load_scenario_data();
			sys::write_scenario_file(*game_state, NATIVE("development_test_file.bin"));
		} else {
			game_state->fill_unsaved_data();
		}

		// scenario loading functions (would have to run these even when scenario is pre-built
		game_state->load_user_settings();
		text::load_standard_fonts(*game_state);
		text::load_bmfonts(*game_state);
		ui::populate_definitions_map(*game_state);

		std::thread update_thread([&]() {
			game_state->game_loop();
		});

		// entire game runs during this line
		window::create_window(*game_state, window::creation_parameters{1024, 780, sys::window_state::maximized, game_state->user_settings.prefer_fullscreen});

		game_state->quit_signaled.store(true, std::memory_order_release);
		update_thread.join();
		CoUninitialize();
	}
	return 0;
}
