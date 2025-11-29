#include "window.hpp"
#include "map.hpp"
#include "opengl_wrapper.hpp"
#include "resource.h"
#include "system_state.hpp"
#include "gui_element_base.hpp"
#include "user_interactions.hpp"

#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "Windows.h"
#include "Windowsx.h"
#include "wglew.h"
#include "sound.hpp"
#include <Shlobj.h>
#include <usp10.h>
#include <initguid.h>
#include <inputscope.h>
#include <Textstor.h>
#include <tsattrs.h>
#include <msctf.h>
#include <olectl.h>

#define WM_GRAPHNOTIFY (WM_APP + 1)

namespace window {

bool is_key_depressed(sys::state const& game_state, sys::virtual_key key) {
	return GetKeyState(int32_t(key)) & 0x8000;
}

void get_window_size(sys::state const& game_state, int& width, int& height) {
	RECT getRect{};
	GetWindowRect(game_state.win_ptr->hwnd, &getRect);
	width = (getRect.right - getRect.left);
	height = (getRect.bottom - getRect.top);
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

			DWORD win32Style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

			RECT rectangle = {left, top, left + game_state.win_ptr->creation_x_size, top + game_state.win_ptr->creation_y_size};
			AdjustWindowRectExForDpi(&rectangle, win32Style, false, 0, GetDpiForWindow(game_state.win_ptr->hwnd));
			int32_t final_width = rectangle.right - rectangle.left;
			int32_t final_height = rectangle.bottom - rectangle.top;

			SetWindowLongW(game_state.win_ptr->hwnd, GWL_STYLE, win32Style);
			SetWindowPos(game_state.win_ptr->hwnd, HWND_NOTOPMOST, rectangle.left, rectangle.top, final_width, final_height, SWP_NOREDRAW);
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
	case WM_APPCOMMAND:
	{
		auto cmd = GET_APPCOMMAND_LPARAM(lParam);
		if(cmd == APPCOMMAND_COPY) {
			if(state->ui_state.edit_target_internal)
				state->pass_edit_command(ui::edit_command::copy, sys::key_modifiers::modifiers_none);
			return TRUE;
		} else if(cmd == APPCOMMAND_CUT) {
			if(state->ui_state.edit_target_internal)
				state->pass_edit_command(ui::edit_command::cut, sys::key_modifiers::modifiers_none);
			return TRUE;
		} else if(cmd == APPCOMMAND_PASTE) {
			if(state->ui_state.edit_target_internal)
				state->pass_edit_command(ui::edit_command::paste, sys::key_modifiers::modifiers_none);
			return TRUE;
		} else if(cmd == APPCOMMAND_REDO) {
			if(state->ui_state.edit_target_internal)
				state->pass_edit_command(ui::edit_command::redo, sys::key_modifiers::modifiers_none);
			return TRUE;
		} else if(cmd == APPCOMMAND_UNDO) {
			if(state->ui_state.edit_target_internal)
				state->pass_edit_command(ui::edit_command::undo, sys::key_modifiers::modifiers_none);
			return TRUE;
		}
		break;
	}
	case WM_SETFOCUS:
		if(state->win_ptr->in_fullscreen)
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOSIZE | SWP_NOMOVE);
		if(state->ui_state.edit_target_internal)
			state->ui_state.edit_target_internal->set_cursor_visibility(*state, true);
		if(state->user_settings.mute_on_focus_lost) {
			sound::resume_all(*state);
		}
		return 0;
	case WM_KILLFOCUS:
		if(state->win_ptr->in_fullscreen)
			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		state->ui_state.selecting_edit_text = ui::edit_selection_mode::none;
		if(state->ui_state.edit_target_internal)
			state->ui_state.edit_target_internal->set_cursor_visibility(*state, false);
		if(state->user_settings.mute_on_focus_lost) {
			sound::pause_all(*state);
		}
		return 0;
	case WM_LBUTTONDOWN: {
		SetCapture(hwnd);
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);

		if(state->ui_state.edit_target_internal) {
			if(state->filter_tso_mouse_events(x, y, uint32_t(wParam))) {
				state->mouse_x_position = x;
				state->mouse_y_position = y;
				state->win_ptr->left_mouse_down = true;
				return 0;
			}

			auto duration = std::chrono::steady_clock::now() - state->win_ptr->last_dbl_click;
			auto in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

			if(in_ms.count() <= window::double_click_ms()) {
				state->pass_edit_command(ui::edit_command::select_current_section, get_current_modifiers());
				state->ui_state.selecting_edit_text = ui::edit_selection_mode::line;
				state->mouse_x_position = x;
				state->mouse_y_position = y;
				state->win_ptr->left_mouse_down = true;
				return 0;
			} else {
				state->ui_state.selecting_edit_text = ui::edit_selection_mode::standard;
			}
		}

		state->on_lbutton_down(x, y, get_current_modifiers());
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		state->win_ptr->left_mouse_down = true;
		return 0;
	}
	case WM_LBUTTONDBLCLK:
	{
		SetCapture(hwnd);
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);

		if(state->ui_state.edit_target_internal) {
			if(state->filter_tso_mouse_events(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), uint32_t(wParam))) {
				state->mouse_x_position = x;
				state->mouse_y_position = y;
				state->win_ptr->left_mouse_down = true;
				return 0;
			}
			state->pass_edit_command(ui::edit_command::select_current_word, get_current_modifiers());
			state->ui_state.selecting_edit_text = ui::edit_selection_mode::word;
			state->win_ptr->last_dbl_click = std::chrono::steady_clock::now();
			state->mouse_x_position = x;
			state->mouse_y_position = y;
			state->win_ptr->left_mouse_down = true;
			return 0;
		}

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
		state->ui_state.selecting_edit_text = ui::edit_selection_mode::none;
		return 0;
	}
	case WM_MOUSEMOVE: {
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);

		if(state->filter_tso_mouse_events(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), uint32_t(wParam))) {
			state->mouse_x_position = x;
			state->mouse_y_position = y;
			return 0;
		}
		if(state->ui_state.edit_target_internal && state->ui_state.selecting_edit_text != ui::edit_selection_mode::none) {
			state->mouse_x_position = x;
			state->mouse_y_position = y;
			state->send_edit_mouse_move(x, y, true);
			return 0;
		}
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

		if(state->filter_tso_mouse_events(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), uint32_t(wParam))) {
			state->mouse_x_position = x;
			state->mouse_y_position = y;
			return 0;
		}

		state->on_rbutton_down(x, y, get_current_modifiers());
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		return 0;
	}
	case WM_RBUTTONDBLCLK:
	{
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);

		if(state->filter_tso_mouse_events(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), uint32_t(wParam))) {
			state->mouse_x_position = x;
			state->mouse_y_position = y;
			return 0;
		}

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

		if(state->filter_tso_mouse_events(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), uint32_t(wParam))) {
			state->mouse_x_position = x;
			state->mouse_y_position = y;
			return 0;
		}

		state->on_mbutton_down(x, y, get_current_modifiers());
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		return 0;
	}
	case WM_MBUTTONDBLCLK:
	{
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);

		if(state->filter_tso_mouse_events(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), uint32_t(wParam))) {
			state->mouse_x_position = x;
			state->mouse_y_position = y;
			return 0;
		}

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
	}
	case WM_MOUSEWHEEL: {
		sys::on_mouse_wheel(*state, state->mouse_x_position, state->mouse_y_position, get_current_modifiers(), (float)(GET_WHEEL_DELTA_WPARAM(wParam)) / 120.0f);
		return 0;
	}
	case WM_KEYDOWN: // fallthrough
	case WM_SYSKEYDOWN:
	{
		if(wParam == VK_PROCESSKEY) {
			wParam = ImmGetVirtualKey(hwnd);
		}
		sys::virtual_key key = sys::virtual_key(wParam);
		switch(key) {
		case sys::virtual_key::RETURN: [[fallthrough]];
		case sys::virtual_key::BACK: [[fallthrough]];
		case sys::virtual_key::DELETE_KEY: [[fallthrough]];
		case sys::virtual_key::LEFT: [[fallthrough]];
		case sys::virtual_key::RIGHT: [[fallthrough]];
		case sys::virtual_key::UP: [[fallthrough]];
		case sys::virtual_key::DOWN:
			break;
		default:
			if((HIWORD(lParam) & KF_REPEAT) != 0)
				return 0;
		}
		state->on_key_down(sys::virtual_key(wParam), get_current_modifiers());
		return 0;
	}
	case WM_SYSKEYUP:
	case WM_KEYUP:
		state->on_key_up(sys::virtual_key(wParam), get_current_modifiers());
		return 0;
	case WM_CHAR: {
		if(state->ui_state.edit_target_internal && wParam >= 0x20 && !(wParam >= 0x7F && wParam <= 0x9F)) {
			state->on_text(char32_t(wParam));
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
	case WM_GETMINMAXINFO:
		LPMINMAXINFO info = (LPMINMAXINFO)lParam;
		info->ptMinTrackSize.x = 640;
		info->ptMinTrackSize.y = 400;
	}
	return DefWindowProcW(hwnd, message, wParam, lParam);
}

void create_window(sys::state& game_state, creation_parameters const& params) {
	game_state.win_ptr = std::make_unique<window_data_impl>();
	game_state.win_ptr->creation_x_size = params.size_x;
	game_state.win_ptr->creation_y_size = params.size_y;
	game_state.win_ptr->in_fullscreen = params.borderless_fullscreen;
	game_state.win_ptr->last_dbl_click = std::chrono::steady_clock::now();

	game_state.win_ptr->text_services.start_text_services();

	// create window
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_OWNDC | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = GetModuleHandleW(nullptr);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = (HICON)LoadImage(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
		GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
	wcex.lpszClassName = L"project_alice_class";
	if(RegisterClassExW(&wcex) == 0) {
		window::emit_error_message("Unable to register window class", true);
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

	change_cursor(game_state, cursor_type::busy);
	game_state.on_create();
	change_cursor(game_state, cursor_type::normal);

	

	MSG msg;
	// pump message loop
	while(true) {
		std::unique_lock lock(game_state.ui_lock);
		game_state.ui_lock_cv.wait(lock, [&] { return !game_state.yield_ui_lock; });
		if(PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) {
				break;
			}
			if(game_state.ui_state.edit_target_internal)
				TranslateMessage(&msg);
			DispatchMessageW(&msg);
		} else {
			// Run game code
			game_state.render();
			SwapBuffers(game_state.win_ptr->opengl_window_dc);
		}
	}

	game_state.win_ptr->text_services.end_text_services();
}

void change_cursor(sys::state& state, cursor_type type) {
	auto root = simple_fs::get_root(state.common_fs);
	auto gfx_dir = simple_fs::open_directory(root, NATIVE("gfx"));
	auto cursors_dir = simple_fs::open_directory(gfx_dir, NATIVE("cursors"));

	if(state.win_ptr->cursors[uint8_t(type)] == HCURSOR(NULL)) {
		native_string_view fname = NATIVE("normal.cur");
		switch(type) {
		case cursor_type::normal:
			fname = NATIVE("normal.cur");
			break;
		case cursor_type::busy:
			fname = NATIVE("busy.ani");
			break;
		case cursor_type::drag_select:
			fname = NATIVE("dragselect.ani");
			break;
		case cursor_type::hostile_move:
			fname = NATIVE("attack_move.ani");
			break;
		case cursor_type::friendly_move:
			fname = NATIVE("friendly_move.ani");
			break;
		case cursor_type::no_move:
			fname = NATIVE("no_move.ani");
			break;
		case cursor_type::text:
			state.win_ptr->cursors[uint8_t(type)] = LoadCursor(NULL, IDC_IBEAM);
			SetCursor(state.win_ptr->cursors[uint8_t(type)]);
			SetClassLongPtr(state.win_ptr->hwnd, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(state.win_ptr->cursors[uint8_t(type)]));
			return;
		default:
			fname = NATIVE("normal.cur");
			break;
		}

		// adapted from https://stackoverflow.com/questions/34065/how-to-read-a-value-from-the-windows-registry

		HKEY hKey;
		auto response = RegOpenKeyExW(HKEY_CURRENT_USER, NATIVE("Software\\Microsoft\\Accessibility"), 0, KEY_READ, &hKey);
		bool exists = (response == ERROR_SUCCESS);
		auto cursor_size_key = NATIVE("CursorSize");

		uint32_t cursor_size = 1;

		if(exists) {
			DWORD dwBufferSize(sizeof(DWORD));
			DWORD nResult(0);
			LONG get_cursor_size_error = RegQueryValueExW(hKey,
				cursor_size_key,
				0,
				NULL,
				reinterpret_cast<LPBYTE>(&nResult),
				&dwBufferSize);
			if(get_cursor_size_error == ERROR_SUCCESS) {
				cursor_size = nResult;
			}
		}

		RegCloseKey(hKey);

		if(auto f = simple_fs::peek_file(cursors_dir, fname); f) {
			auto path = simple_fs::get_full_name(*f);
			state.win_ptr->cursors[uint8_t(type)] = (HCURSOR)LoadImageW(nullptr, path.c_str(), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE); //.cur or .ani

			if(state.win_ptr->cursors[uint8_t(type)] == HCURSOR(NULL)) {
				state.win_ptr->cursors[uint8_t(type)] = LoadCursor(nullptr, IDC_ARROW); //default system cursor
				state.ui_state.cursor_size = GetSystemMetrics(SM_CXCURSOR) * cursor_size / 2;
			} else {
				state.ui_state.cursor_size = GetSystemMetrics(SM_CXCURSOR) * cursor_size / 2;
			}
		} else {
			state.win_ptr->cursors[uint8_t(type)] = LoadCursor(nullptr, IDC_ARROW); //default system cursor
			state.ui_state.cursor_size = GetSystemMetrics(SM_CXCURSOR) * cursor_size / 2;
		}
	}
	SetCursor(state.win_ptr->cursors[uint8_t(type)]);
	SetClassLongPtr(state.win_ptr->hwnd, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(state.win_ptr->cursors[uint8_t(type)]));
}

int32_t cursor_blink_ms() {
	static int32_t ms = []() {auto t = GetCaretBlinkTime(); return t == INFINITE ? 0 : t * 2; }();
	return ms;
}
int32_t double_click_ms() {
	static int32_t ms = GetDoubleClickTime();
	return ms;
}

void emit_error_message(std::string const& content, bool fatal) {
	static const char* msg1 = "Project Alice has encountered a fatal error";
	static const char* msg2 = "Project Alice has encountered the following problems";
	MessageBoxA(nullptr, content.c_str(), fatal ? msg1 : msg2, MB_OK | (fatal ? MB_ICONERROR : MB_ICONWARNING));
	if(fatal) {
		std::exit(EXIT_FAILURE);
	}
}

enum class lock_state : uint8_t {
	unlocked, locked_read, locked_readwrite
};

struct mouse_sink {
	ITfMouseSink* sink;
	LONG range_start;
	LONG range_length;
};

template<class Interface>
inline void safe_release(Interface*& i) {
	if(i) {
		i->Release();
		i = nullptr;
	}
}

struct text_services_object : public ITextStoreACP2, public ITfInputScope, public ITfContextOwnerCompositionSink, public ITfMouseTrackerACP {
	std::vector<TS_ATTRVAL> gathered_attributes;
	std::vector<mouse_sink> installed_mouse_sinks;
	sys::state& win;
	ui::element_base* ei = nullptr;
	ITfDocumentMgr* document = nullptr;
	ITfContext* primary_context = nullptr;
	TfEditCookie content_identifier = 0;
	ITextStoreACPSink* advise_sink = nullptr;
	lock_state document_lock_state = lock_state::unlocked;
	bool notify_on_text_change = false;
	bool notify_on_selection_change = false;
	bool relock_pending = false;
	bool in_composition = false;

	ULONG m_refCount;

	void free_gathered_attributes() {
		for(auto& i : gathered_attributes) {
			VariantClear(&(i.varValue));
		}
		gathered_attributes.clear();
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override {
		if(riid == __uuidof(IUnknown))
			*ppvObject = static_cast<ITextStoreACP2*>(this);
		else if(riid == __uuidof(ITextStoreACP2))
			*ppvObject = static_cast<ITextStoreACP2*>(this);
		else if(riid == __uuidof(ITfInputScope))
			*ppvObject = static_cast<ITfInputScope*>(this);
		else if(riid == __uuidof(ITfContextOwnerCompositionSink))
			*ppvObject = static_cast<ITfContextOwnerCompositionSink*>(this);
		else if(riid == __uuidof(ITfMouseTrackerACP))
			*ppvObject = static_cast<ITfMouseTrackerACP*>(this);
		else {
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		(static_cast<IUnknown*>(*ppvObject))->AddRef();
		return S_OK;
	}
	ULONG STDMETHODCALLTYPE AddRef() override {
		return InterlockedIncrement(&m_refCount);
	}
	ULONG STDMETHODCALLTYPE Release() override {
		long val = InterlockedDecrement(&m_refCount);
		if(val == 0) {
			delete this;
		}
		return val;
	}

	//ITfMouseTrackerACP
	HRESULT STDMETHODCALLTYPE AdviseMouseSink(__RPC__in_opt ITfRangeACP* range, __RPC__in_opt ITfMouseSink* pSink, __RPC__out DWORD* pdwCookie) override {
		if(!range || !pSink || !pdwCookie)
			return E_INVALIDARG;
		for(uint32_t i = 0; i < installed_mouse_sinks.size(); ++i) {
			if(installed_mouse_sinks[i].sink == nullptr) {
				installed_mouse_sinks[i].sink = pSink;
				pSink->AddRef();
				installed_mouse_sinks[i].range_start = 0;
				installed_mouse_sinks[i].range_length = 0;
				range->GetExtent(&(installed_mouse_sinks[i].range_start), &(installed_mouse_sinks[i].range_length));
				*pdwCookie = DWORD(i);
				return S_OK;
			}
		}
		installed_mouse_sinks.emplace_back();
		installed_mouse_sinks.back().sink = pSink;
		pSink->AddRef();
		installed_mouse_sinks.back().range_start = 0;
		installed_mouse_sinks.back().range_length = 0;
		range->GetExtent(&(installed_mouse_sinks.back().range_start), &(installed_mouse_sinks.back().range_length));
		*pdwCookie = DWORD(installed_mouse_sinks.size() - 1);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE UnadviseMouseSink(DWORD dwCookie) override {
		if(dwCookie < installed_mouse_sinks.size()) {
			safe_release(installed_mouse_sinks[dwCookie].sink);
		}
		return S_OK;
	}

	bool send_mouse_event(int32_t x, int32_t y, uint32_t buttons) {
		if(installed_mouse_sinks.empty())
			return false;
		if(!ei)
			return false;

		auto detailed_pos = win.detailed_text_mouse_test(x, y);
		for(auto& ms : installed_mouse_sinks) {
			if(ms.sink && int32_t(detailed_pos.position) >= ms.range_start && int32_t(detailed_pos.position) <= ms.range_start + ms.range_length) {
				BOOL eaten = false;
				ms.sink->OnMouseEvent(detailed_pos.position, detailed_pos.quadrent, buttons, &eaten);
				if(eaten)
					return true;
			}
		}
		return false;
	}

	//ITfContextOwnerCompositionSink
	HRESULT STDMETHODCALLTYPE OnStartComposition(__RPC__in_opt ITfCompositionView* /*pComposition*/, __RPC__out BOOL* pfOk) override {
		*pfOk = TRUE;
		in_composition = true;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnUpdateComposition(__RPC__in_opt ITfCompositionView* pComposition, __RPC__in_opt ITfRange* /*pRangeNew*/) override {
		ITfRange* view_range = nullptr;
		pComposition->GetRange(&view_range);
		if(view_range) {
			ITfRangeACP* acp_range = nullptr;
			view_range->QueryInterface(IID_PPV_ARGS(&acp_range));
			if(ei && acp_range) {
				LONG start = 0;
				LONG count = 0;
				acp_range->GetExtent(&start, &count);
				if(count > 0) {
					ei->set_temporary_selection(win, int32_t(start), int32_t(start + count));
				}
			}
			safe_release(acp_range);
		}
		safe_release(view_range);

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnEndComposition(__RPC__in_opt ITfCompositionView* /*pComposition*/) override {
		if(ei) {
			ei->register_composition_result(win);
		}
		in_composition = false;
		return S_OK;
	}

	// ITextStoreACP2
	HRESULT STDMETHODCALLTYPE AdviseSink(__RPC__in REFIID riid, __RPC__in_opt IUnknown* punk, DWORD dwMask) override {
		if(!IsEqualGUID(riid, IID_ITextStoreACPSink)) {
			return E_INVALIDARG;
		}
		ITextStoreACPSink* temp = nullptr;
		if(FAILED(punk->QueryInterface(IID_ITextStoreACPSink, (void**)&temp))) {
			return E_NOINTERFACE;
		}
		if(advise_sink) {
			if(advise_sink == temp) {
				safe_release(temp);
			} else {
				return CONNECT_E_ADVISELIMIT;
			}
		} else {
			advise_sink = temp;
		}
		notify_on_text_change = (TS_AS_TEXT_CHANGE & dwMask) != 0;
		notify_on_selection_change = (TS_AS_SEL_CHANGE & dwMask) != 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE UnadviseSink(__RPC__in_opt IUnknown* /*punk*/) override {
		safe_release(advise_sink);
		notify_on_text_change = false;
		notify_on_selection_change = false;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE RequestLock(DWORD dwLockFlags, __RPC__out HRESULT* phrSession) override {
		if(!advise_sink) {
			*phrSession = E_FAIL;
			return E_UNEXPECTED;
		}

		relock_pending = false;

		if(document_lock_state != lock_state::unlocked) {
			if(dwLockFlags & TS_LF_SYNC) {
				*phrSession = TS_E_SYNCHRONOUS;
				return S_OK;
			} else {
				if(document_lock_state == lock_state::locked_read && (dwLockFlags & TS_LF_READWRITE) == TS_LF_READWRITE) {
					relock_pending = true;
					*phrSession = TS_S_ASYNC;
					return S_OK;
				}
			}
			return E_FAIL;
		}

		if((TS_LF_READ & dwLockFlags) != 0) {
			document_lock_state = lock_state::locked_read;
		}
		if((TS_LF_READWRITE & dwLockFlags) != 0) {
			document_lock_state = lock_state::locked_readwrite;
		}

		*phrSession = advise_sink->OnLockGranted(dwLockFlags);
		document_lock_state = lock_state::unlocked;

		if(relock_pending) {
			relock_pending = false;
			HRESULT hr = S_OK;
			RequestLock(TS_LF_READWRITE, &hr);
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetStatus(__RPC__out TS_STATUS* pdcs) override {
		if(!pdcs)
			return E_INVALIDARG;
		pdcs->dwStaticFlags = TS_SS_NOHIDDENTEXT;
		pdcs->dwDynamicFlags = 0;
		//pdcs->dwDynamicFlags = (ei && ei->is_read_only() ? TS_SD_READONLY : 0);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE QueryInsert(LONG acpTestStart, LONG acpTestEnd, ULONG /*cch*/, __RPC__out LONG* pacpResultStart, __RPC__out LONG* pacpResultEnd) override {
		if(!pacpResultStart || !pacpResultEnd || acpTestStart > acpTestEnd)
			return E_INVALIDARG;
		if(!ei)
			return TF_E_DISCONNECTED;
		*pacpResultStart = std::min(acpTestStart, LONG(ei->text_content().length()));
		*pacpResultEnd = std::min(acpTestEnd, LONG(ei->text_content().length()));
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetSelection(ULONG ulIndex, ULONG ulCount, __RPC__out_ecount_part(ulCount, *pcFetched) TS_SELECTION_ACP* pSelection, __RPC__out ULONG* pcFetched) override {
		if(!pcFetched)
			return E_INVALIDARG;
		if(document_lock_state == lock_state::unlocked)
			return TS_E_NOLOCK;

		if(ei && ulCount > 0 && (ulIndex == 0 || ulIndex == TF_DEFAULT_SELECTION)) {
			if(!pSelection)
				return E_INVALIDARG;
			auto range = ei->text_selection();
			pSelection[0].acpStart = int32_t(std::min(range.first, range.second));
			pSelection[0].acpEnd = int32_t(std::max(range.first, range.second));
			pSelection[0].style.ase = range.first < range.second ? TS_AE_START : TS_AE_END;
			pSelection[0].style.fInterimChar = FALSE;
			*pcFetched = 1;
		} else {
			*pcFetched = 0;
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetSelection(ULONG ulCount, __RPC__in_ecount_full(ulCount) const TS_SELECTION_ACP* pSelection) override {
		if(document_lock_state != lock_state::locked_readwrite)
			return TF_E_NOLOCK;
		if(!ei)
			return TF_E_DISCONNECTED;
		if(ulCount > 0) {
			if(!pSelection)
				return E_INVALIDARG;

			auto start = pSelection->style.ase == TS_AE_START ? pSelection->acpEnd : pSelection->acpStart;
			auto end = pSelection->style.ase == TS_AE_START ? pSelection->acpStart : pSelection->acpEnd;
			ei->set_text_selection(win, start, end);
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetText(LONG acpStart, LONG acpEnd, __RPC__out_ecount_part(cchPlainReq, *pcchPlainRet) WCHAR* pchPlain, ULONG cchPlainReq, __RPC__out ULONG* pcchPlainRet, __RPC__out_ecount_part(cRunInfoReq, *pcRunInfoRet) TS_RUNINFO* prgRunInfo, ULONG cRunInfoReq, __RPC__out ULONG* pcRunInfoRet, __RPC__out LONG* pacpNext) override {

		if(!pcchPlainRet || !pcRunInfoRet)
			return E_INVALIDARG;
		if(!pchPlain && cchPlainReq != 0)
			return E_INVALIDARG;
		if(!prgRunInfo && cRunInfoReq != 0)
			return E_INVALIDARG;
		if(!pacpNext)
			return E_INVALIDARG;
		if(document_lock_state == lock_state::unlocked)
			return TF_E_NOLOCK;

		*pcchPlainRet = 0;

		if(!ei)
			return TF_E_DISCONNECTED;

		if((cchPlainReq == 0) && (cRunInfoReq == 0)) {
			return S_OK;
		}
		auto len = LONG(ei->text_content().length());
		acpEnd = std::min(acpEnd, len);
		if(acpEnd == -1)
			acpEnd = len;

		acpEnd = std::min(acpEnd, acpStart + LONG(cchPlainReq));
		if(acpStart != acpEnd) {
			auto text = ei->text_content();
			std::copy(text.data() + acpStart, text.data() + acpEnd, pchPlain);
			*pcchPlainRet = (acpEnd - acpStart);
		}
		if(*pcchPlainRet < cchPlainReq) {
			*(pchPlain + *pcchPlainRet) = 0;
		}
		if(cRunInfoReq != 0) {
			prgRunInfo[0].uCount = acpEnd - acpStart;
			prgRunInfo[0].type = TS_RT_PLAIN;
			*pcRunInfoRet = 1;
		} else {
			*pcRunInfoRet = 0;
		}

		*pacpNext = acpEnd;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetText(DWORD /*dwFlags*/, LONG acpStart, LONG acpEnd, __RPC__in_ecount_full(cch) const WCHAR* pchText, ULONG cch, __RPC__out TS_TEXTCHANGE* pChange) override {
		if(document_lock_state != lock_state::locked_readwrite)
			return TF_E_NOLOCK;
		if(!ei)
			return TF_E_DISCONNECTED;
		if(!pChange)
			return E_INVALIDARG;
		if(!pchText && cch > 0)
			return E_INVALIDARG;

		auto len = LONG(ei->text_content().length());

		if(acpStart > len)
			return E_INVALIDARG;

		LONG acpRemovingEnd = acpEnd >= acpStart ? std::min(acpEnd, len) : acpStart;


		ei->insert_text(win, uint32_t(acpStart), uint32_t(acpRemovingEnd), std::u16string_view((char16_t*)const_cast<WCHAR*>(pchText), cch), ui::insertion_source::text_services);

		pChange->acpStart = acpStart;
		pChange->acpOldEnd = acpRemovingEnd;
		pChange->acpNewEnd = acpStart + cch;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetFormattedText(LONG /*acpStart*/, LONG /*acpEnd*/, __RPC__deref_out_opt IDataObject** /*ppDataObject*/) override {
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetEmbedded(LONG /*acpPos*/, __RPC__in REFGUID /*rguidService*/, __RPC__in REFIID /*riid*/, __RPC__deref_out_opt IUnknown** ppunk) override {
		if(!ppunk)
			return E_INVALIDARG;
		*ppunk = nullptr;
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE QueryInsertEmbedded(__RPC__in const GUID* /*pguidService*/, __RPC__in const FORMATETC* pFormatEtc, __RPC__out BOOL* pfInsertable) override {
		if(!pFormatEtc || !pfInsertable)
			return E_INVALIDARG;
		if(pfInsertable)
			*pfInsertable = FALSE;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE InsertEmbedded(DWORD /*dwFlags*/, LONG /*acpStart*/, LONG /*acpEnd*/, __RPC__in_opt IDataObject* /*pDataObject*/, __RPC__out TS_TEXTCHANGE* /*pChange*/) override {
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE InsertTextAtSelection(DWORD dwFlags, __RPC__in_ecount_full(cch) const WCHAR* pchText, ULONG cch, __RPC__out LONG* pacpStart, __RPC__out LONG* pacpEnd, __RPC__out TS_TEXTCHANGE* pChange) override {


		if(!ei)
			return TF_E_DISCONNECTED;

		auto range = ei->text_selection();
		LONG acpStart = LONG(std::min(range.first, range.second));
		LONG acpEnd = LONG(std::max(range.first, range.second));

		if((dwFlags & TS_IAS_QUERYONLY) != 0) {
			if(document_lock_state == lock_state::unlocked)
				return TS_E_NOLOCK;
			if(pacpStart)
				*pacpStart = acpStart;
			if(pacpEnd)
				*pacpEnd = acpStart + cch;
			return S_OK;
		}

		if(document_lock_state != lock_state::locked_readwrite)
			return TF_E_NOLOCK;
		if(!pchText)
			return E_INVALIDARG;


		ei->insert_text(win, uint32_t(acpStart), uint32_t(acpEnd), std::u16string_view((char16_t*)const_cast<WCHAR*>(pchText), cch), ui::insertion_source::text_services);

		if(pacpStart)
			*pacpStart = acpStart;
		if(pacpEnd)
			*pacpEnd = acpStart + cch;

		if(pChange) {
			pChange->acpStart = acpStart;
			pChange->acpOldEnd = acpEnd;
			pChange->acpNewEnd = acpStart + cch;
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE InsertEmbeddedAtSelection(DWORD /*dwFlags*/, __RPC__in_opt IDataObject* /*pDataObject*/, __RPC__out LONG* /*pacpStart*/, __RPC__out LONG* /*pacpEnd*/, __RPC__out TS_TEXTCHANGE* /*pChange*/) override {

		return E_NOTIMPL;
	}

	void fill_gathered_attributes(ULONG cFilterAttrs, __RPC__in_ecount_full_opt(cFilterAttrs) const TS_ATTRID* paFilterAttrs, bool fill_variants, int32_t position) {
		free_gathered_attributes();

		for(uint32_t i = 0; i < cFilterAttrs; ++i) {
			if(IsEqualGUID(paFilterAttrs[i], GUID_PROP_INPUTSCOPE)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_UNKNOWN;
					ITfInputScope* is = nullptr;
					this->QueryInterface(IID_PPV_ARGS(&is));
					gathered_attributes.back().varValue.punkVal = is;
				}
			/* } else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Font_FaceName)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BSTR;
					gathered_attributes.back().varValue.bstrVal = SysAllocString(win.dynamic_settings.primary_font.name.c_str());
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Font_SizePts)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_I4;
					gathered_attributes.back().varValue.lVal = int32_t(win.dynamic_settings.primary_font.font_size * 96.0f / win.dpi);
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Font_Style_Bold)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BOOL;
					gathered_attributes.back().varValue.boolVal = win.dynamic_settings.primary_font.weight > 400 ? VARIANT_TRUE : VARIANT_FALSE;
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Font_Style_Height)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_I4;
					gathered_attributes.back().varValue.lVal = int32_t(win.dynamic_settings.primary_font.font_size);
				} */
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Font_Style_Hidden)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BOOL;
					gathered_attributes.back().varValue.boolVal = VARIANT_FALSE;
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Font_Style_Italic)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BOOL;
					gathered_attributes.back().varValue.boolVal = VARIANT_FALSE;
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Font_Style_Weight)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_I4;
					gathered_attributes.back().varValue.lVal = 400;
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Text_Alignment_Center)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BOOL;
					gathered_attributes.back().varValue.boolVal = VARIANT_FALSE;
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Text_Alignment_Justify)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BOOL;
					gathered_attributes.back().varValue.boolVal = VARIANT_FALSE;
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Text_Alignment_Left)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BOOL;
					gathered_attributes.back().varValue.boolVal = VARIANT_TRUE;
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Text_Alignment_Right)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BOOL;
					gathered_attributes.back().varValue.boolVal = VARIANT_FALSE;
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Text_Orientation)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_I4;
					gathered_attributes.back().varValue.lVal = 0;
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Text_ReadOnly)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BOOL;
					gathered_attributes.back().varValue.boolVal = VARIANT_FALSE;
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Text_RightToLeft)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BOOL;
					if(position >= 0) {
						gathered_attributes.back().varValue.boolVal = ei && ei->position_is_ltr(position) ? VARIANT_FALSE : VARIANT_TRUE;
					} else {
						gathered_attributes.back().varValue.boolVal = win.world.locale_get_native_rtl(win.font_collection.get_current_locale()) ? VARIANT_TRUE : VARIANT_FALSE;
					}
				}
			} else if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Text_VerticalWriting)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BOOL;
					gathered_attributes.back().varValue.boolVal = VARIANT_FALSE;
				}
			} else if(IsEqualGUID(paFilterAttrs[i], GUID_PROP_COMPOSING)) {
				gathered_attributes.emplace_back();
				gathered_attributes.back().idAttr = paFilterAttrs[i];
				gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
				if(fill_variants) {
					gathered_attributes.back().varValue.vt = VT_BOOL;
					if(position >= 0) {
						auto range = ei->temporary_text_range();
						gathered_attributes.back().varValue.boolVal = ei && (position >= range.first) && (position < range.second) ? VARIANT_TRUE : VARIANT_FALSE;
					} else {
						gathered_attributes.back().varValue.boolVal = VARIANT_FALSE;
					}
				}
			}
		}
	}

	HRESULT STDMETHODCALLTYPE RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs, __RPC__in_ecount_full_opt(cFilterAttrs) const TS_ATTRID* paFilterAttrs) override {
		if(!paFilterAttrs && cFilterAttrs > 0)
			return E_INVALIDARG;
		if(!ei)
			return TF_E_DISCONNECTED;

		bool fill_variants = (TS_ATTR_FIND_WANT_VALUE & dwFlags) != 0;
		fill_gathered_attributes(cFilterAttrs, paFilterAttrs, fill_variants, -1);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, __RPC__in_ecount_full_opt(cFilterAttrs) const TS_ATTRID* paFilterAttrs, DWORD dwFlags) override {
		if(!paFilterAttrs && cFilterAttrs > 0)
			return E_INVALIDARG;
		if(!ei)
			return TF_E_DISCONNECTED;

		bool fill_variants = (TS_ATTR_FIND_WANT_VALUE & dwFlags) != 0;
		fill_gathered_attributes(cFilterAttrs, paFilterAttrs, fill_variants, acpPos);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs, __RPC__in_ecount_full_opt(cFilterAttrs) const TS_ATTRID* paFilterAttrs, DWORD dwFlags) override {
		if(!paFilterAttrs && cFilterAttrs > 0)
			return E_INVALIDARG;
		if(!ei)
			return TF_E_DISCONNECTED;

		free_gathered_attributes();
		bool fill_variants = (TS_ATTR_FIND_WANT_VALUE & dwFlags) != 0;
		bool attributes_that_end = (TS_ATTR_FIND_WANT_END & dwFlags) != 0;
		for(uint32_t i = 0; i < cFilterAttrs; ++i) {
			if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Text_RightToLeft)) {
				if(acpPos > 0 && ei && ei->position_is_ltr(uint32_t(acpPos - 1)) != ei->position_is_ltr(uint32_t(acpPos))) {

					gathered_attributes.emplace_back();
					gathered_attributes.back().idAttr = paFilterAttrs[i];
					gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
					if(fill_variants) {
						gathered_attributes.back().varValue.vt = VT_BOOL;

						if(attributes_that_end)
							gathered_attributes.back().varValue.boolVal = ei && ei->position_is_ltr(uint32_t(acpPos - 1)) ? VARIANT_FALSE : VARIANT_TRUE;
						else
							gathered_attributes.back().varValue.boolVal = ei && ei->position_is_ltr(uint32_t(acpPos)) ? VARIANT_FALSE : VARIANT_TRUE;
					}
				}
			} else if(IsEqualGUID(paFilterAttrs[i], GUID_PROP_COMPOSING)) {
				auto range = ei->temporary_text_range();
				if(acpPos > 0 && ei &&
					(((acpPos - 1 >= range.first) && (acpPos - 1 < range.second))
						!=
						((acpPos >= range.first) && (acpPos < range.second)))) {

					gathered_attributes.emplace_back();
					gathered_attributes.back().idAttr = paFilterAttrs[i];
					gathered_attributes.back().dwOverlapId = int32_t(gathered_attributes.size());
					if(fill_variants) {
						gathered_attributes.back().varValue.vt = VT_BOOL;
						if(attributes_that_end)
							gathered_attributes.back().varValue.boolVal = ei && (acpPos - 1 >= range.first) && (acpPos - 1 < range.second) ? VARIANT_TRUE : VARIANT_FALSE;
						else
							gathered_attributes.back().varValue.boolVal = ei && (acpPos >= range.first) && (acpPos < range.second) ? VARIANT_TRUE : VARIANT_FALSE;

					}
				}
			}
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FindNextAttrTransition(LONG acpStart, LONG acpHalt, ULONG cFilterAttrs, __RPC__in_ecount_full_opt(cFilterAttrs) const TS_ATTRID* paFilterAttrs, DWORD dwFlags, __RPC__out LONG* pacpNext, __RPC__out BOOL* pfFound, __RPC__out LONG* plFoundOffset) override {
		if(!pacpNext || !pfFound || !plFoundOffset)
			return E_INVALIDARG;

		*pfFound = FALSE;
		if(plFoundOffset)
			*plFoundOffset = 0;
		*pacpNext = acpStart;

		if(!ei)
			return TF_E_DISCONNECTED;

		LONG initial_position = std::max(acpStart, LONG(1));
		LONG end_position = std::min(acpHalt, LONG(ei->text_content().length() - 1));
		end_position = std::max(initial_position, end_position);

		if((TS_ATTR_FIND_BACKWARDS & dwFlags) != 0) {
			std::swap(initial_position, end_position);
		}
		while(initial_position != end_position) {
			for(uint32_t i = 0; i < cFilterAttrs; ++i) {
				if(IsEqualGUID(paFilterAttrs[i], TSATTRID_Text_RightToLeft)) {
					if(ei->position_is_ltr(uint32_t(initial_position - 1)) != ei->position_is_ltr(uint32_t(initial_position))) {
						*pfFound = TRUE;
						if(plFoundOffset)
							*plFoundOffset = initial_position;
						*pacpNext = initial_position;
						return S_OK;
					}
				} else if(IsEqualGUID(paFilterAttrs[i], GUID_PROP_COMPOSING)) {
					auto range = ei->temporary_text_range();
					if(((initial_position - 1 >= range.first) && (initial_position - 1 < range.second))
						!=
						((initial_position >= range.first) && (initial_position < range.second))) {
						*pfFound = TRUE;
						if(plFoundOffset)
							*plFoundOffset = initial_position;
						*pacpNext = initial_position;
						return S_OK;
					}
				}
			}
			if((TS_ATTR_FIND_BACKWARDS & dwFlags) != 0) {
				--initial_position;
			} else {
				++initial_position;
			}
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE RetrieveRequestedAttrs(ULONG ulCount, __RPC__out_ecount_part(ulCount, *pcFetched) TS_ATTRVAL* paAttrVals, __RPC__out ULONG* pcFetched) override {

		*pcFetched = 0;
		uint32_t i = 0;
		for(; i < ulCount && i < gathered_attributes.size(); ++i) {
			paAttrVals[i] = gathered_attributes[i];
			(*pcFetched)++;
		}
		for(; i < gathered_attributes.size(); ++i) {
			VariantClear(&(gathered_attributes[i].varValue));
		}
		gathered_attributes.clear();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetEndACP(__RPC__out LONG* pacp) override {
		if(!pacp)
			return E_INVALIDARG;
		if(document_lock_state == lock_state::unlocked)
			return TS_E_NOLOCK;
		*pacp = 0;
		if(!ei)
			return TF_E_DISCONNECTED;
		*pacp = LONG(ei->text_content().length());
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetActiveView(__RPC__out TsViewCookie* pvcView) override {
		if(!pvcView)
			return E_INVALIDARG;
		*pvcView = 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetACPFromPoint(TsViewCookie /*vcView*/, __RPC__in const POINT* ptScreen, DWORD dwFlags, __RPC__out LONG* pacp) override {

		*pacp = 0;
		if(!ei)
			return TF_E_DISCONNECTED;

		POINT client_space;
		ScreenToClient(win.win_ptr->hwnd, &client_space);
		auto client_pos = ui::get_absolute_location(win, *ei);
		auto client_size = ei->base_data.size;

		if((GXFPF_NEAREST & dwFlags) == 0) {
			if(client_space.x <  int32_t(client_pos.x * win.user_settings.ui_scale) || client_space.y < int32_t(client_pos.y * win.user_settings.ui_scale)
				|| client_space.x > int32_t((client_pos.x + client_size.x) * win.user_settings.ui_scale) || client_space.y > int32_t((client_pos.y + client_size.y) * win.user_settings.ui_scale)) {
				return TS_E_INVALIDPOINT;
			}
		}

		auto from_point = ei->detailed_text_mouse_test(win, client_space.x - int32_t(client_pos.x * win.user_settings.ui_scale), client_space.y - int32_t(client_pos.y * win.user_settings.ui_scale)).position;
		*pacp = LONG(from_point);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetTextExt(TsViewCookie /*vcView*/, LONG acpStart, LONG acpEnd, __RPC__out RECT* prc, __RPC__out BOOL* pfClipped) override {
		if(!pfClipped || !prc)
			return E_INVALIDARG;
		if(document_lock_state == lock_state::unlocked)
			return TS_E_NOLOCK;

		*pfClipped = FALSE;
		*prc = RECT{ 0,0,0,0 };

		if(IsIconic(win.win_ptr->hwnd)) {
			*pfClipped = TRUE;
			return S_OK;
		}
		if(!ei)
			return TF_E_DISCONNECTED;

		auto raw_bounds = ei->text_bounds(win, acpStart, acpEnd);
		POINT top_left;
		top_left.x = int32_t(raw_bounds.top_left.x * win.user_settings.ui_scale);
		top_left.y = int32_t(raw_bounds.top_left.y * win.user_settings.ui_scale);
		ClientToScreen(win.win_ptr->hwnd, &top_left);

		prc->left = top_left.x;
		prc->top = top_left.y;
		prc->right = top_left.x + int32_t(raw_bounds.size.x * win.user_settings.ui_scale);
		prc->bottom = top_left.y + int32_t(raw_bounds.size.y * win.user_settings.ui_scale);

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetScreenExt(TsViewCookie /*vcView*/, __RPC__out RECT* prc) override {
		if(!prc)
			return E_INVALIDARG;

		*prc = RECT{ 0,0,0,0 };

		if(!ei)
			return TF_E_DISCONNECTED;

		if(IsIconic(win.win_ptr->hwnd))
			return S_OK;

		POINT top_left;
		auto pos = ui::get_absolute_location(win, *ei);
		top_left.x = int32_t(pos.x * win.user_settings.ui_scale);
		top_left.y = int32_t(pos.y * win.user_settings.ui_scale);
		ClientToScreen(win.win_ptr->hwnd, &top_left);

		prc->left = top_left.x;
		prc->top = top_left.y;
		prc->right = top_left.x + int32_t(ei->base_data.size.x * win.user_settings.ui_scale);
		prc->bottom = top_left.y + int32_t(ei->base_data.size.y * win.user_settings.ui_scale);

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetInputScopes(__RPC__deref_out_ecount_full_opt(*pcCount) InputScope** pprgInputScopes, __RPC__out UINT* pcCount) override {
		*pprgInputScopes = (InputScope*)CoTaskMemAlloc(sizeof(InputScope));
		*(*pprgInputScopes) = IS_TEXT;
		/*
		if(ei) {
			switch(ei->get_type()) {
			case edit_contents::generic_text:
				*(*pprgInputScopes) = IS_TEXT;
				break;
			case edit_contents::number:
				*(*pprgInputScopes) = IS_NUMBER;
				break;
			case edit_contents::single_char:
				*(*pprgInputScopes) = IS_ONECHAR;
				break;
			case edit_contents::email:
				*(*pprgInputScopes) = IS_EMAIL_SMTPEMAILADDRESS;
				break;
			case edit_contents::date:
				*(*pprgInputScopes) = IS_DATE_FULLDATE;
				break;
			case edit_contents::time:
				*(*pprgInputScopes) = IS_TIME_FULLTIME;
				break;
			case edit_contents::url:
				*(*pprgInputScopes) = IS_URL;
				break;
			}
		} else {
			*(*pprgInputScopes) = IS_DEFAULT;
		}
		*/
		*pcCount = 1;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetPhrase(__RPC__deref_out_ecount_full_opt(*pcCount) BSTR** ppbstrPhrases, __RPC__out UINT* pcCount) override {
		*ppbstrPhrases = nullptr;
		*pcCount = 0;
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetRegularExpression(__RPC__deref_out_opt BSTR* pbstrRegExp) override {
		*pbstrRegExp = nullptr;
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetSRGS(__RPC__deref_out_opt BSTR* pbstrSRGS) override {
		*pbstrSRGS = nullptr;
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetXML(__RPC__deref_out_opt BSTR* pbstrXML) override {
		*pbstrXML = nullptr;
		return E_NOTIMPL;
	}

	text_services_object(ITfThreadMgr* manager_ptr, TfClientId owner, sys::state& win, ui::element_base* ei) : win(win), ei(ei), m_refCount(1) {
		manager_ptr->CreateDocumentMgr(&document);
		auto hr = document->CreateContext(owner, 0, static_cast<ITextStoreACP2*>(this), &primary_context, &content_identifier);
		hr = document->Push(primary_context);
	}
	virtual ~text_services_object() {
		free_gathered_attributes();
	}
};


void release_text_services_object(text_services_object* ptr) {
	ptr->ei = nullptr;
	safe_release(ptr->primary_context);
	safe_release(ptr->document);
	ptr->Release();
}


win32_text_services::win32_text_services() {
	CoCreateInstance(CLSID_TF_ThreadMgr, NULL, CLSCTX_INPROC_SERVER,
		IID_ITfThreadMgr, (void**)&manager_ptr);
}
win32_text_services::~win32_text_services() {
	safe_release(manager_ptr);
}

void win32_text_services::start_text_services() {
	manager_ptr->Activate(&client_id);
	//manager_ptr->SuspendKeystrokeHandling();
}
void win32_text_services::end_text_services() {
	manager_ptr->Deactivate();
}
void win32_text_services::on_text_change(text_services_object* ts, uint32_t old_start, uint32_t old_end, uint32_t new_end) {
	if(send_notifications && ts->advise_sink && ts->notify_on_text_change) {
		TS_TEXTCHANGE chng{ LONG(old_start), LONG(old_end), LONG(new_end) };
		ts->advise_sink->OnTextChange(0, &chng);
	}
}
void win32_text_services::on_selection_change(text_services_object* ts) {
	if(send_notifications && ts->advise_sink && ts->notify_on_selection_change) {
		ts->advise_sink->OnSelectionChange();
	}
}
bool win32_text_services::send_mouse_event_to_tso(text_services_object* ts, int32_t x, int32_t y, uint32_t buttons) {
	return ts->send_mouse_event(x, y, buttons);
}

void win32_text_services::set_focus(sys::state& win, text_services_object* o) {
	//manager_ptr->SetFocus(o->document);

	auto hwnd = win.win_ptr->hwnd;
	if(hwnd) {
		ITfDocumentMgr* old_doc = nullptr;
		manager_ptr->AssociateFocus((HWND)hwnd, o ? o->document : nullptr, &old_doc);
		safe_release(old_doc);
	}
}
void win32_text_services::suspend_keystroke_handling() {
	//manager_ptr->SuspendKeystrokeHandling();
}
void win32_text_services::resume_keystroke_handling() {
	//manager_ptr->ResumeKeystrokeHandling();
}

text_services_object* win32_text_services::create_text_service_object(sys::state& win, ui::element_base& ei) {
	return new text_services_object(manager_ptr, client_id, win, &ei);
}
} // namespace window
