#include "window.hpp"
#include "map.hpp"
#include "opengl_wrapper.hpp"

#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "Windows.h"
#include "Windowsx.h"
#include "wglew.h"
#include "sound.hpp"

#define WM_GRAPHNOTIFY (WM_APP + 1)

namespace window {

bool is_key_depressed(sys::state const& game_state, sys::virtual_key key) {
	return GetKeyState(int32_t(key)) & 0x8000;
}

void get_window_size(sys::state const& game_state, int& width, int& height) {
	RECT getRectangle{};
	GetWindowRect(game_state.win_ptr->hwnd, &getRectangle);
	*width = (right - left);
	*height = (bottom - top);
}

bool is_in_fullscreen(sys::state const& game_state) {
	return (game_state.win_ptr) && game_state.win_ptr->in_fullscreen;
}

void set_borderless_full_screen(sys::state& game_state, bool fullscreen) {
	if(game_state.win_ptr && game_state.win_ptr->hwnd && game_state.win_ptr->in_fullscreen != fullscreen) {
		if(!fullscreen) {

			auto monitor_handle = MonitorFromWindow(game_state.win_ptr->hwnd, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFO mi;
			mi.cbSize = sizeof(mi);
			GetMonitorInfoW(monitor_handle, &mi);

			int left = (mi.rcWork.right - mi.rcWork.left) / 2 - game_state.win_ptr->creation_x_size / 2;
			int top = (mi.rcWork.bottom - mi.rcWork.top) / 2 - game_state.win_ptr->creation_y_size / 2;

			DWORD win32Style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_SYSMENU |
												 WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

			RECT rectangle = {left, top, left + game_state.win_ptr->creation_x_size, top + game_state.win_ptr->creation_y_size};
			AdjustWindowRectExForDpi(&rectangle, win32Style, false, 0, GetDpiForWindow(game_state.win_ptr->hwnd));
			int32_t final_width = rectangle.right - rectangle.left;
			int32_t final_height = rectangle.bottom - rectangle.top;

			SetWindowLongW(game_state.win_ptr->hwnd, GWL_STYLE, win32Style);
			SetWindowPos(game_state.win_ptr->hwnd, HWND_NOTOPMOST, rectangle.left, rectangle.top, final_width, final_height,
					SWP_NOREDRAW);
			SetWindowRgn(game_state.win_ptr->hwnd, NULL, TRUE);
			ShowWindow(game_state.win_ptr->hwnd, SW_MAXIMIZE);

			game_state.win_ptr->in_fullscreen = false;
		} else {
			// ShowWindow(game_state.win_ptr->hwnd, SW_SHOWNORMAL);

			auto monitor_handle = MonitorFromWindow(game_state.win_ptr->hwnd, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFO mi;
			mi.cbSize = sizeof(mi);
			GetMonitorInfoW(monitor_handle, &mi);

			DWORD win32Style = WS_VISIBLE | WS_BORDER | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

			RECT rectangle = mi.rcMonitor;
			AdjustWindowRectExForDpi(&rectangle, win32Style, false, WS_EX_TOPMOST, GetDpiForWindow(game_state.win_ptr->hwnd));
			int32_t win_width = (rectangle.right - rectangle.left);
			int32_t win_height = (rectangle.bottom - rectangle.top);

			SetWindowLongW(game_state.win_ptr->hwnd, GWL_STYLE, win32Style);
			SetWindowPos(game_state.win_ptr->hwnd, HWND_TOPMOST, rectangle.left, rectangle.top, win_width, win_height, SWP_NOREDRAW);

			game_state.win_ptr->in_fullscreen = true;
		}
	}
}

void close_window(sys::state& game_state) {
	if(game_state.win_ptr && game_state.win_ptr->hwnd)
		PostMessageW(game_state.win_ptr->hwnd, WM_CLOSE, 0, 0);
}

bool is_low_surrogate(uint16_t char_code) noexcept {
	return char_code >= 0xDC00 && char_code <= 0xDFFF;
}
bool is_high_surrogate(uint16_t char_code) noexcept {
	return char_code >= 0xD800 && char_code <= 0xDBFF;
}

char process_utf16_to_win1250(wchar_t c) {
	if(c <= 127)
		return char(c);
	if(is_low_surrogate(c) || is_high_surrogate(c))
		return 0;
	char char_out = 0;
	WideCharToMultiByte(1250, 0, &c, 1, &char_out, 1, nullptr, nullptr);
	return char_out;
}

sys::key_modifiers get_current_modifiers() {
	uint32_t val =
			uint32_t((GetKeyState(VK_CONTROL) & 0x8000) ? sys::key_modifiers::modifiers_ctrl : sys::key_modifiers::modifiers_none) |
			uint32_t((GetKeyState(VK_MENU) & 0x8000) ? sys::key_modifiers::modifiers_alt : sys::key_modifiers::modifiers_none) |
			uint32_t((GetKeyState(VK_SHIFT) & 0x8000) ? sys::key_modifiers::modifiers_shift : sys::key_modifiers::modifiers_none);
	return sys::key_modifiers(val);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static int drag_x_start = 0;
	static int drag_y_start = 0;

	if(message == WM_CREATE) {
		CREATESTRUCTW* cptr = (CREATESTRUCTW*)lParam;
		sys::state* create_state = (sys::state*)(cptr->lpCreateParams);

		create_state->win_ptr->hwnd = hwnd;
		create_state->win_ptr->opengl_window_dc = GetDC(hwnd);

		// setup opengl here
		ogl::initialize_opengl(*create_state);

		RECT crect{};
		GetClientRect(hwnd, &crect);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)create_state);

		return 0;
	}

	sys::state* state = (sys::state*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
	if(!state || !(state->win_ptr))
		return DefWindowProcW(hwnd, message, wParam, lParam);

	switch(message) {
	case WM_CLOSE:
		DestroyWindow(hwnd);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR) nullptr);
		return 0;
	case WM_DESTROY:
		ogl::shutdown_opengl(*state);
		ReleaseDC(hwnd, state->win_ptr->opengl_window_dc);
		PostQuitMessage(0);
		return 0;
	case WM_SETFOCUS:
		if(state->win_ptr->in_fullscreen)
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOSIZE | SWP_NOMOVE);
		return 0;
	case WM_KILLFOCUS:
		if(state->win_ptr->in_fullscreen)
			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		return 0;
	case WM_LBUTTONDOWN: {
		SetCapture(hwnd);
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
		state->on_lbutton_down(x, y, get_current_modifiers());
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		state->win_ptr->left_mouse_down = true;
		return 0;
	}
	case WM_LBUTTONUP: {
		ReleaseCapture();
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
		state->on_lbutton_up(x, y, get_current_modifiers());
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		state->win_ptr->left_mouse_down = false;
		return 0;
	}
	case WM_MOUSEMOVE: {
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
		state->on_mouse_move(x, y, get_current_modifiers());

		if(wParam & MK_LBUTTON)
			state->on_mouse_drag(x, y, get_current_modifiers());

		state->mouse_x_position = x;
		state->mouse_y_position = y;

		return 0;
	}
	case WM_RBUTTONDOWN: {
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
		state->on_rbutton_down(x, y, get_current_modifiers());
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		return 0;
	}
	case WM_RBUTTONUP: {
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
		state->on_rbutton_up(x, y, get_current_modifiers());
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		return 0;
	}
	case WM_MBUTTONDOWN: {
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
		state->on_mbutton_down(x, y, get_current_modifiers());
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		return 0;
	}
	case WM_MBUTTONUP: {
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
		state->on_mbutton_up(x, y, get_current_modifiers());
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		return 0;
	}
	case WM_SIZE: {
		window::window_state t = window::window_state::normal;

		if(wParam == SIZE_MAXIMIZED) {
			t = window_state::maximized;
		} else if(wParam == SIZE_MINIMIZED) {
			t = window_state::minimized;
		} else if(wParam == SIZE_RESTORED) {
			t = window_state::normal;
		} else {
			// other
			break;
		}

		state->on_resize(LOWORD(lParam), HIWORD(lParam), t);
		state->x_size = LOWORD(lParam);
		state->y_size = HIWORD(lParam);

		// TODO MAP CAMERA HERE CODE HERE
		// state->map_camera = map::flat_camera(glm::vec2{ state->x_size, state->y_size }, glm::vec2{
		// state->map_provinces_texture.size_x, state->map_provinces_texture.size_y });

		return 0;
	};
	case WM_MOUSEWHEEL: {
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
		state->on_mouse_wheel(x, y, get_current_modifiers(), (float)(GET_WHEEL_DELTA_WPARAM(wParam)) / 120.0f);
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		return 0;
	}
	case WM_KEYDOWN: // fallthrough
	case WM_SYSKEYDOWN:
		if((HIWORD(lParam) & KF_REPEAT) != 0)
			return 0;
		state->on_key_down(sys::virtual_key(wParam), get_current_modifiers());
		return 0;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		state->on_key_up(sys::virtual_key(wParam), get_current_modifiers());
		return 0;
	case WM_CHAR: {
		if(state->ui_state.edit_target) {
			char turned_into = process_utf16_to_win1250(wchar_t(wParam));
			if(turned_into)
				state->on_text(turned_into);
		}
		return 0;
	}

	case WM_PAINT:
	case WM_DISPLAYCHANGE: {
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_DPICHANGED:
		return 0;

	case WM_GRAPHNOTIFY:
		// this is the message that tells us there is a DirectShow event
		sound::update_music_track(*state);
		break;
	}
	return DefWindowProcW(hwnd, message, wParam, lParam);
}

void create_window(sys::state& game_state, creation_parameters const& params) {
	game_state.win_ptr = std::make_unique<window_data_impl>();
	game_state.win_ptr->creation_x_size = params.size_x;
	game_state.win_ptr->creation_y_size = params.size_y;
	game_state.win_ptr->in_fullscreen = params.borderless_fullscreen;

	// create window
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = GetModuleHandleW(nullptr);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = nullptr;
	wcex.lpszClassName = L"project_alice_class";

	if(RegisterClassExW(&wcex) == 0) {
		std::abort();
	}

	DWORD win32Style = !params.borderless_fullscreen ? (WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX |
																												 WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS)
																									 : WS_VISIBLE | WS_BORDER | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	game_state.win_ptr->hwnd = CreateWindowExW(0, L"project_alice_class", L"Project Alice", win32Style, CW_USEDEFAULT,
			CW_USEDEFAULT, 0, 0, NULL, NULL, GetModuleHandleW(nullptr), &game_state);

	if(!game_state.win_ptr->hwnd)
		return;

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	if(!params.borderless_fullscreen) {

		auto monitor_handle = MonitorFromWindow(game_state.win_ptr->hwnd, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfoW(monitor_handle, &mi);

		int left = (mi.rcWork.right - mi.rcWork.left) / 2 - game_state.win_ptr->creation_x_size / 2;
		int top = (mi.rcWork.bottom - mi.rcWork.top) / 2 - game_state.win_ptr->creation_y_size / 2;

		RECT rectangle = {left, top, left + game_state.win_ptr->creation_x_size, top + game_state.win_ptr->creation_y_size};
		AdjustWindowRectExForDpi(&rectangle, win32Style, false, 0, GetDpiForWindow(game_state.win_ptr->hwnd));
		int32_t final_width = rectangle.right - rectangle.left;
		int32_t final_height = rectangle.bottom - rectangle.top;

		SetWindowLongW(game_state.win_ptr->hwnd, GWL_STYLE, win32Style);
		SetWindowPos(game_state.win_ptr->hwnd, HWND_NOTOPMOST, rectangle.left, rectangle.top, final_width, final_height,
				SWP_FRAMECHANGED);
		SetWindowRgn(game_state.win_ptr->hwnd, NULL, TRUE);

		if(params.initial_state == window_state::maximized)
			ShowWindow(game_state.win_ptr->hwnd, SW_MAXIMIZE);
		else if(params.initial_state == window_state::minimized)
			ShowWindow(game_state.win_ptr->hwnd, SW_MINIMIZE);
		else
			ShowWindow(game_state.win_ptr->hwnd, SW_SHOWNORMAL);
	} else {

		auto monitor_handle = MonitorFromWindow(game_state.win_ptr->hwnd, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfoW(monitor_handle, &mi);

		RECT rectangle = mi.rcMonitor;
		AdjustWindowRectExForDpi(&rectangle, win32Style, false, WS_EX_TOPMOST, GetDpiForWindow(game_state.win_ptr->hwnd));
		int32_t win_width = (rectangle.right - rectangle.left);
		int32_t win_height = (rectangle.bottom - rectangle.top);

		SetWindowLongW(game_state.win_ptr->hwnd, GWL_STYLE, win32Style);
		SetWindowPos(game_state.win_ptr->hwnd, HWND_TOPMOST, rectangle.left, rectangle.top, win_width, win_height, SWP_FRAMECHANGED);
		ShowWindow(game_state.win_ptr->hwnd, SW_SHOWNORMAL);
	}

	UpdateWindow(game_state.win_ptr->hwnd);

	sound::initialize_sound_system(game_state);
	sound::start_music(game_state, game_state.user_settings.master_volume * game_state.user_settings.music_volume);

	game_state.on_create();

	MSG msg;
	// pump message loop
	while(true) {
		if(PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) {
				break;
			}
			if(game_state.ui_state.edit_target)
				TranslateMessage(&msg);
			DispatchMessageW(&msg);
		} else {
			// Run game code

			game_state.render();
			SwapBuffers(game_state.win_ptr->opengl_window_dc);
		}
	}
}

void emit_error_message(std::string const& content, bool fatal) {
	MessageBoxA(nullptr, content.c_str(),
			fatal ? "Project Alice has encountered a fatal error:" : "Project Alice has encountered the following problems:",
			MB_OK | (fatal ? MB_ICONERROR : MB_ICONWARNING));
	if(fatal) {
		std::terminate();
	}
}
} // namespace window
