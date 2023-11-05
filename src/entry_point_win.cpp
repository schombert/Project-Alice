#include "system_state.hpp"

#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <shellapi.h>
#include <shellscalingapi.h>
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

LONG WINAPI uef_wrapper( struct _EXCEPTION_POINTERS* lpTopLevelExceptionFilter) {
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
	tGetPolicy pGetPolicy = (tGetPolicy)GetProcAddress(kernel32, "GetProcessUserModeExceptionPolicy");
	tSetPolicy pSetPolicy = (tSetPolicy)GetProcAddress(kernel32, "SetProcessUserModeExceptionPolicy");
	if(pGetPolicy && pSetPolicy) {
		DWORD dwFlags;
		if(pGetPolicy(&dwFlags)) {
			// Turn off the filter
			pSetPolicy(dwFlags & ~EXCEPTION_SWALLOWING);
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

	// Workaround for old machines
	HINSTANCE hUser32dll = LoadLibrary(L"User32.dll");
	if(hUser32dll) {
		auto pSetProcessDpiAwarenessContext = (decltype(&SetProcessDpiAwarenessContext))GetProcAddress(hUser32dll, "SetProcessDpiAwarenessContext");
		if(pSetProcessDpiAwarenessContext != NULL) {
			pSetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		} else {
			// windows 8.1 (not present on windows 8 and only available on desktop apps)
			HINSTANCE hShcoredll = LoadLibrary(L"Shcore.dll");
			if(hShcoredll) {
				auto pSetProcessDpiAwareness = (decltype(&SetProcessDpiAwareness))GetProcAddress(hShcoredll, "SetProcessDpiAwareness");
				if(pSetProcessDpiAwareness != NULL) {
					pSetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
				} else {
					SetProcessDPIAware(); //vista+
				}
				FreeLibrary(hShcoredll);
			} else {
				SetProcessDPIAware(); //vista+
			}
		}
		FreeLibrary(hUser32dll);
	}

	if(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
		// do everything here: create a window, read messages

		int num_params = 0;
		auto parsed_cmd = CommandLineToArgvW(GetCommandLineW(), &num_params);

		if(num_params < 2) {
#ifdef NDEBUG
			auto msg = std::string("Start Alice.exe using the launcher");
			window::emit_error_message(msg, true);
			return 0;
#else
			if(std::string("NONE") != GAME_DIR) {									 // check for user-defined location
				add_root(game_state.common_fs, NATIVE_M(GAME_DIR)); // game files directory is overlaid on top of that
				add_root(game_state.common_fs, NATIVE("."));				 // for the moment this lets us find the shader files
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
				add_root(game_state.common_fs, szBuffer);		// game files directory is overlaid on top of that
				add_root(game_state.common_fs, NATIVE(".")); // for the moment this lets us find the shader files
				RegCloseKey(hKey);
			}

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
			if(sys::try_read_scenario_and_save_file(game_state, parsed_cmd[1])) {
				game_state.fill_unsaved_data();
			} else {
				auto msg = std::string("Scenario file ") +  simple_fs::native_to_utf8(parsed_cmd[1]) + " could not be read";
				window::emit_error_message(msg, true);
				return 0;
			}
		}
		LocalFree(parsed_cmd);

		// scenario loading functions (would have to run these even when scenario is pre-built)
		game_state.load_user_settings();
		text::load_standard_fonts(game_state);
		text::load_bmfonts(game_state);
		ui::populate_definitions_map(game_state);

		std::thread update_thread([&]() { game_state.game_loop(); });

		// entire game runs during this line
		window::create_window(game_state,
				window::creation_parameters{1024, 780, window::window_state::maximized, game_state.user_settings.prefer_fullscreen});

		game_state.quit_signaled.store(true, std::memory_order_release);
		update_thread.join();
		CoUninitialize();
	}
	return 0;
}
