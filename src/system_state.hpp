#pragma once

#include <memory>
#include <stdint.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

#include "dcon_generated.hpp"

// this header will eventually contain the highest-level objects
// that represent the overall state of the program
// it will also include the game state itself eventually as a member

namespace window {
	class window_data_impl;
}

namespace sys {
	enum class key_modifiers : uint8_t {
		modifiers_none = 0x0,
		modifiers_alt = 0x4,
		modifiers_ctrl = 0x2,
		modifiers_shift = 0x1,
		modifiers_ctrl_shift = 0x3,
		modifiers_ctrl_alt = 0x6,
		modifiers_alt_shift = 0x5,
		modifiers_all = 0x7
	};

	enum class window_state : uint8_t {
		normal, maximized, minimized
	};

	enum class virtual_key : uint8_t {
		NONE = 0x00,
		LBUTTON = 0x01,
		RBUTTON = 0x02,
		CANCEL = 0x03,
		MBUTTON = 0x04,
		XBUTTON_1 = 0x05,
		XBUTTON_2 = 0x06,
		BACK = 0x08,
		TAB = 0x09,
		CLEAR = 0x0C,
		RETURN = 0x0D,
		SHIFT = 0x10,
		CONTROL = 0x11,
		MENU = 0x12,
		PAUSE = 0x13,
		CAPITAL = 0x14,
		KANA = 0x15,
		JUNJA = 0x17,
		FINAL = 0x18,
		KANJI = 0x19,
		ESCAPE = 0x1B,
		CONVERT = 0x1C,
		NONCONVERT = 0x1D,
		ACCEPT = 0x1E,
		MODECHANGE = 0x1F,
		SPACE = 0x20,
		PRIOR = 0x21,
		NEXT = 0x22,
		END = 0x23,
		HOME = 0x24,
		LEFT = 0x25,
		UP = 0x26,
		RIGHT = 0x27,
		DOWN = 0x28,
		SELECT = 0x29,
		PRINT = 0x2A,
		EXECUTE = 0x2B,
		SNAPSHOT = 0x2C,
		INSERT = 0x2D,
		DELETE_KEY = 0x2E,
		HELP = 0x2F,
		NUM_0 = 0x30,
		NUM_1 = 0x31,
		NUM_2 = 0x32,
		NUM_3 = 0x33,
		NUM_4 = 0x34,
		NUM_5 = 0x35,
		NUM_6 = 0x36,
		NUM_7 = 0x37,
		NUM_8 = 0x38,
		NUM_9 = 0x39,
		A = 0x41,
		B = 0x42,
		C = 0x43,
		D = 0x44,
		E = 0x45,
		F = 0x46,
		G = 0x47,
		H = 0x48,
		I = 0x49,
		J = 0x4A,
		K = 0x4B,
		L = 0x4C,
		M = 0x4D,
		N = 0x4E,
		O = 0x4F,
		P = 0x50,
		Q = 0x51,
		R = 0x52,
		S = 0x53,
		T = 0x54,
		U = 0x55,
		V = 0x56,
		W = 0x57,
		X = 0x58,
		Y = 0x59,
		Z = 0x5A,
		LWIN = 0x5B,
		RWIN = 0x5C,
		APPS = 0x5D,
		SLEEP = 0x5F,
		NUMPAD0 = 0x60,
		NUMPAD1 = 0x61,
		NUMPAD2 = 0x62,
		NUMPAD3 = 0x63,
		NUMPAD4 = 0x64,
		NUMPAD5 = 0x65,
		NUMPAD6 = 0x66,
		NUMPAD7 = 0x67,
		NUMPAD8 = 0x68,
		NUMPAD9 = 0x69,
		MULTIPLY = 0x6A,
		ADD = 0x6B,
		SEPARATOR = 0x6C,
		SUBTRACT = 0x6D,
		DECIMAL = 0x6E,
		DIVIDE = 0x6F,
		F1 = 0x70,
		F2 = 0x71,
		F3 = 0x72,
		F4 = 0x73,
		F5 = 0x74,
		F6 = 0x75,
		F7 = 0x76,
		F8 = 0x77,
		F9 = 0x78,
		F10 = 0x79,
		F11 = 0x7A,
		F12 = 0x7B,
		F13 = 0x7C,
		F14 = 0x7D,
		F15 = 0x7E,
		F16 = 0x7F,
		F17 = 0x80,
		F18 = 0x81,
		F19 = 0x82,
		F20 = 0x83,
		F21 = 0x84,
		F22 = 0x85,
		F23 = 0x86,
		F24 = 0x87,
		NAVIGATION_VIEW = 0x88,
		NAVIGATION_MENU = 0x89,
		NAVIGATION_UP = 0x8A,
		NAVIGATION_DOWN = 0x8B,
		NAVIGATION_LEFT = 0x8C,
		NAVIGATION_RIGHT = 0x8D,
		NAVIGATION_ACCEPT = 0x8E,
		NAVIGATION_CANCEL = 0x8F,
		NUMLOCK = 0x90,
		SCROLL = 0x91,
		OEM_NEC_EQUAL = 0x92,
		LSHIFT = 0xA0,
		RSHIFT = 0xA1,
		LCONTROL = 0xA2,
		RCONTROL = 0xA3,
		LMENU = 0xA4,
		RMENU = 0xA5,
		SEMICOLON = 0xBA,
		PLUS = 0xBB,
		COMMA = 0xBC,
		MINUS = 0xBD,
		PERIOD = 0xBE,
		FORWARD_SLASH = 0xBF,
		TILDA = 0xC0,
		OPEN_BRACKET = 0xDB,
		BACK_SLASH = 0xDC,
		CLOSED_BRACKET = 0xDD,
		QUOTE = 0xDE
	};

	struct text_tag {
		dcon::text_key start;
		uint16_t length = 0;
	};

	struct alignas(64) state {
		// the state struct will eventually include (at least pointers to)
		// the state of the sound system, the state of the windowing system,
		// and the game data / state itself

		// utlimately it is the system struct that gets passed around everywhere
		// so that bits of the ui, for example, can control the overall state of
		// the game

		// implementation dependent data that needs to be stored for the window
		// subsystem

		dcon::data_container world;

		std::vector<char> text_data; // stores string data in the win1250 codepage

		std::unique_ptr<window::window_data_impl> win_ptr = nullptr; 

		// common data for the window
		int32_t x_size = 0;
		int32_t y_size = 0;
		int32_t mouse_x_position = 0;
		int32_t mouse_y_position = 0;
		bool in_edit_control = false;

		// graphics data
		void* opengl_context = nullptr;

		// the following functions will be invoked by the window subsystem

		void on_rbutton_down(int32_t x, int32_t y, key_modifiers mod);
		void on_lbutton_down(int32_t x, int32_t y, key_modifiers mod);
		void on_rbutton_up(int32_t x, int32_t y, key_modifiers mod);
		void on_lbutton_up(int32_t x, int32_t y, key_modifiers mod);
		void on_mouse_move(int32_t x, int32_t y, key_modifiers mod);
		void on_mouse_drag(int32_t x, int32_t y, key_modifiers mod); // called when the left button is held down
		void on_resize(int32_t x, int32_t y, window_state win_state);
		void on_mouse_wheel(int32_t x, int32_t y, key_modifiers mod, float amount); // an amount of 1.0 is one "click" of the wheel
		void on_key_down(virtual_key keycode, key_modifiers mod);
		void on_key_up(virtual_key keycode, key_modifiers mod);
		void on_text(char c); // c is win1250 codepage value
		void render(); // called to render the frame may (and should) delay returning until the frame is rendered, including waiting for vsync

		// the following function are for interacting with the string pool

		std::string_view to_string_view(text_tag tag) const; // takes a stored tag and give you the text

		text_tag add_to_pool(std::string_view text); // returns the newly added text

		// searches the string pool for any existing string, appends if it is new
		// use this function sparingly; i.e. only when you think it is likely that
		// the text has already been added. Searching *all* the text may not be cheap
		text_tag add_unique_to_pool(std::string_view text); 

		~state();
	};
}