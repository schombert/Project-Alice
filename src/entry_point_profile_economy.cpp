#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "system_state.hpp"
#include "economy.hpp"
#include "game_scene.hpp"
#include "serialization.hpp"
#include "parsers_declarations.hpp"

#include <Windows.h>
#include <shellapi.h>
#include "Objbase.h"
#include "window.hpp"

#include "network/webapi/controllers.hpp"

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "icu.lib")

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

	if(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
		// do everything here: create a window, read messages

		int num_params = 0;
		auto parsed_cmd = CommandLineToArgvW(GetCommandLineW(), &num_params);
		if(num_params < 2) return 0;
		if(sys::try_read_scenario_and_save_file(game_state, parsed_cmd[1])) {
			game_state.fill_unsaved_data();
		} else {
			return 0;
		}
		LocalFree(parsed_cmd);

		// scenario loading functions (would have to run these even when scenario is pre-built)
		game_state.load_user_settings();
		ui::populate_definitions_map(game_state);
		network::init(game_state);

		game_state.single_game_tick();

		for(int i = 0; i < 1000; i++) {
			economy::update_employment(game_state, false, 1.f);
			economy::daily_update(game_state, false, 1.f);
		}

		network::finish(game_state, true);
		CoUninitialize();
	}
	return 0;
}
