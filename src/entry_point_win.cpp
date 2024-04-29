#include "system_state.hpp"
#include "serialization.hpp"

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
#pragma comment(lib, "Shell32.lib")

static sys::state game_state; // too big for the stack
static CRITICAL_SECTION guard_abort_handler;

void signal_abort_handler(int) {
	static bool run_once = false;

	EnterCriticalSection(&guard_abort_handler);
	if(run_once == false) {
		run_once = true;

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));
		// Start the child process. 
		if(CreateProcessW(
			L"dbg_alice.exe",   // Module name
			NULL, // Command line
			NULL, // Process handle not inheritable
			NULL, // Thread handle not inheritable
			FALSE, // Set handle inheritance to FALSE
			0, // No creation flags
			NULL, // Use parent's environment block
			NULL, // Use parent's starting directory 
			&si, // Pointer to STARTUPINFO structure
			&pi) == 0) {

			// create process failed
			LeaveCriticalSection(&guard_abort_handler);
			return;

		}
		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);
		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	LeaveCriticalSection(&guard_abort_handler);
}

LONG WINAPI uef_wrapper(struct _EXCEPTION_POINTERS* lpTopLevelExceptionFilter) {
	signal_abort_handler(0);
	return EXCEPTION_CONTINUE_SEARCH;
}

void generic_wrapper() {
	signal_abort_handler(0);
}
void invalid_parameter_wrapper(
   const wchar_t* expression,
   const wchar_t* function,
   const wchar_t* file,
   unsigned int line,
   uintptr_t pReserved
) {
	signal_abort_handler(0);
}

void EnableCrashingOnCrashes() {
	typedef BOOL(WINAPI* tGetPolicy)(LPDWORD lpFlags);
	typedef BOOL(WINAPI* tSetPolicy)(DWORD dwFlags);
	const DWORD EXCEPTION_SWALLOWING = 0x1;

	HMODULE kernel32 = LoadLibraryA("kernel32.dll");
	if(kernel32) {
		tGetPolicy pGetPolicy = (tGetPolicy)GetProcAddress(kernel32, "GetProcessUserModeExceptionPolicy");
		tSetPolicy pSetPolicy = (tSetPolicy)GetProcAddress(kernel32, "SetProcessUserModeExceptionPolicy");
		if(pGetPolicy && pSetPolicy) {
			DWORD dwFlags;
			if(pGetPolicy(&dwFlags)) {
				// Turn off the filter
				pSetPolicy(dwFlags & ~EXCEPTION_SWALLOWING);
			}
		}
	}
	BOOL insanity = FALSE;
	SetUserObjectInformationA(GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &insanity, sizeof(insanity));
}

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*commandline*/, int /*nCmdShow*/
) {

#ifdef _DEBUG
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#endif

	InitializeCriticalSection(&guard_abort_handler);

	if(!IsDebuggerPresent()) {
		EnableCrashingOnCrashes();
		_set_purecall_handler(generic_wrapper);
		_set_invalid_parameter_handler(invalid_parameter_wrapper);
		_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
		SetUnhandledExceptionFilter(uef_wrapper);
		signal(SIGABRT, signal_abort_handler);
	}

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	if(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
		// do everything here: create a window, read messages

		int num_params = 0;
		auto parsed_cmd = CommandLineToArgvW(GetCommandLineW(), &num_params);

		int headless_speed = 6;
		bool headless_repeat = false;
		bool headless = false;
		if(num_params < 2) {
#ifdef NDEBUG
			auto msg = std::string("Start Alice.exe using the launcher");
			window::emit_error_message(msg, true);
			return 0;
#else
			
			add_root(game_state.common_fs, NATIVE("."));				 // for the moment this lets us find the shader files
			

			if(!sys::try_read_scenario_and_save_file(game_state, NATIVE("development_test_file.bin"))) {
				// scenario making functions
				parsers::error_handler err{ "" };
				game_state.load_scenario_data(err);
				if(!err.accumulated_errors.empty())
					window::emit_error_message(err.accumulated_errors, true);
				sys::write_scenario_file(game_state, NATIVE("development_test_file.bin"), 0);
				game_state.loaded_scenario_file = NATIVE("development_test_file.bin");
			} else {
				game_state.fill_unsaved_data();
			}
#endif
		} else {
			for(int i = 1; i < num_params; ++i) {
				if(native_string(parsed_cmd[i]) == NATIVE("-host")) {
					game_state.network_mode = sys::network_mode_type::host;
				} else if(native_string(parsed_cmd[i]) == NATIVE("-join")) {
					game_state.network_mode = sys::network_mode_type::client;
					game_state.network_state.ip_address = "127.0.0.1";
					if(i + 1 < num_params) {
						game_state.network_state.ip_address = simple_fs::native_to_utf8(native_string(parsed_cmd[i + 1]));
						i++;
					}
				} else if(native_string(parsed_cmd[i]) == NATIVE("-name")) {
					if(i + 1 < num_params) {
						std::string nickname = simple_fs::native_to_utf8(native_string(parsed_cmd[i + 1]));
						memcpy(game_state.network_state.nickname.data, nickname.data(), std::min<size_t>(nickname.length(), 8));
						i++;
					}
				} else if(native_string(parsed_cmd[i]) == NATIVE("-password")) {
					if(i + 1 < num_params) {
						auto str = simple_fs::native_to_utf8(native_string(parsed_cmd[i + 1]));
						std::memset(game_state.network_state.password, '\0', sizeof(game_state.network_state.password));
						std::memcpy(game_state.network_state.password, str.c_str(), std::min(sizeof(game_state.network_state.password), str.length()));
						i++;
					}
				} else if(native_string(parsed_cmd[i]) == NATIVE("-v6")) {
					game_state.network_state.as_v6 = true;
				} else if(native_string(parsed_cmd[i]) == NATIVE("-v4")) {
					game_state.network_state.as_v6 = false;
				} else if(native_string(parsed_cmd[i]) == NATIVE("-headless")) {
					headless = true;
				} else if(native_string(parsed_cmd[i]) == NATIVE("-repeat")) {
					headless_repeat = true;
				} else if(native_string(parsed_cmd[i]) == NATIVE("-speed")) {
					if(i + 1 < num_params) {
						auto str = simple_fs::native_to_utf8(native_string(parsed_cmd[i + 1]));
						headless_speed = std::atoi(str.c_str());
						i++;
					}
				}
			}

			if(sys::try_read_scenario_and_save_file(game_state, parsed_cmd[1])) {
				game_state.fill_unsaved_data();
			} else {
				auto msg = std::string("Scenario file ") + simple_fs::native_to_utf8(parsed_cmd[1]) + " could not be read";
				window::emit_error_message(msg, true);
				return 0;
			}

			network::init(game_state);
		}
		LocalFree(parsed_cmd);

		// scenario loading functions (would have to run these even when scenario is pre-built)
		game_state.load_user_settings();
		text::load_standard_fonts(game_state);
		text::load_bmfonts(game_state);
		ui::populate_definitions_map(game_state);

		if(headless) {
			game_state.actual_game_speed = headless_speed;
			game_state.ui_pause.store(false, std::memory_order::release);
			game_state.mode = sys::game_mode_type::in_game;
			game_state.local_player_nation = dcon::nation_id{};
			if(headless_repeat) {
				std::thread update_thread([&]() { game_state.game_loop(); });
				while(!game_state.quit_signaled.load(std::memory_order::acquire)) {
					while(game_state.mode == sys::game_mode_type::in_game) {
						std::this_thread::sleep_for(std::chrono::milliseconds(15));
					}
					//Reload savefile
					network::finish(game_state, true);
					game_state.actual_game_speed = 0;
					//
					if(sys::try_read_scenario_and_save_file(game_state, parsed_cmd[1])) {
						game_state.fill_unsaved_data();
					} else {
						auto msg = std::string("Scenario file ") + simple_fs::native_to_utf8(parsed_cmd[1]) + " could not be read";
						window::emit_error_message(msg, true);
						return 0;
					}
					//
					network::init(game_state);
					game_state.mode = sys::game_mode_type::in_game;
					game_state.actual_game_speed = headless_speed;
				};
				update_thread.join();
			} else {
				game_state.game_loop();
			}
		} else {
			std::thread update_thread([&]() { game_state.game_loop(); });
			// entire game runs during this line
			window::create_window(game_state,
					window::creation_parameters{ 1024, 780, window::window_state::maximized, game_state.user_settings.prefer_fullscreen });
			game_state.quit_signaled.store(true, std::memory_order_release);
			update_thread.join();
		}

		network::finish(game_state, true);
		CoUninitialize();
	}
	return 0;
}
