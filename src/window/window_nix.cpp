#include "window.hpp"
#include "map.hpp"

#include <GLFW/glfw3.h>
#include <unordered_map>

namespace window {
class window_data_impl {
public:
	// HWND hwnd = nullptr;
	// HDC opengl_window_dc = nullptr;
	GLFWwindow* window = nullptr;

	int32_t creation_x_size = 600;
	int32_t creation_y_size = 400;

	bool in_fullscreen = false;
	bool left_mouse_down = false;
};

static const std::unordered_map<int, sys::virtual_key> glfw_key_to_virtual_key = {
    {GLFW_KEY_UNKNOWN, sys::virtual_key::NONE},
    {GLFW_KEY_SPACE, sys::virtual_key::SPACE},
    {GLFW_KEY_APOSTROPHE, sys::virtual_key::QUOTE},
    {GLFW_KEY_COMMA, sys::virtual_key::COMMA},
    {GLFW_KEY_EQUAL, sys::virtual_key::PLUS},
    {GLFW_KEY_MINUS, sys::virtual_key::MINUS},
    {GLFW_KEY_PERIOD, sys::virtual_key::PERIOD},
    {GLFW_KEY_SLASH, sys::virtual_key::FORWARD_SLASH},
    {GLFW_KEY_0, sys::virtual_key::NUM_0},
    {GLFW_KEY_1, sys::virtual_key::NUM_1},
    {GLFW_KEY_2, sys::virtual_key::NUM_2},
    {GLFW_KEY_3, sys::virtual_key::NUM_3},
    {GLFW_KEY_4, sys::virtual_key::NUM_4},
    {GLFW_KEY_5, sys::virtual_key::NUM_5},
    {GLFW_KEY_6, sys::virtual_key::NUM_6},
    {GLFW_KEY_7, sys::virtual_key::NUM_7},
    {GLFW_KEY_8, sys::virtual_key::NUM_8},
    {GLFW_KEY_9, sys::virtual_key::NUM_9},
    {GLFW_KEY_SEMICOLON, sys::virtual_key::SEMICOLON},
    {GLFW_KEY_EQUAL, sys::virtual_key::OEM_NEC_EQUAL},
    {GLFW_KEY_A, sys::virtual_key::A},
    {GLFW_KEY_B, sys::virtual_key::B},
    {GLFW_KEY_C, sys::virtual_key::C},
    {GLFW_KEY_D, sys::virtual_key::D},
    {GLFW_KEY_E, sys::virtual_key::E},
    {GLFW_KEY_F, sys::virtual_key::F},
    {GLFW_KEY_G, sys::virtual_key::G},
    {GLFW_KEY_H, sys::virtual_key::H},
    {GLFW_KEY_I, sys::virtual_key::I},
    {GLFW_KEY_J, sys::virtual_key::J},
    {GLFW_KEY_K, sys::virtual_key::K},
    {GLFW_KEY_L, sys::virtual_key::L},
    {GLFW_KEY_M, sys::virtual_key::M},
    {GLFW_KEY_N, sys::virtual_key::N},
    {GLFW_KEY_O, sys::virtual_key::O},
    {GLFW_KEY_P, sys::virtual_key::P},
    {GLFW_KEY_Q, sys::virtual_key::Q},
    {GLFW_KEY_R, sys::virtual_key::R},
    {GLFW_KEY_S, sys::virtual_key::S},
    {GLFW_KEY_T, sys::virtual_key::T},
    {GLFW_KEY_U, sys::virtual_key::U},
    {GLFW_KEY_V, sys::virtual_key::V},
    {GLFW_KEY_W, sys::virtual_key::W},
    {GLFW_KEY_X, sys::virtual_key::X},
    {GLFW_KEY_Y, sys::virtual_key::Y},
    {GLFW_KEY_Z, sys::virtual_key::Z},
    {GLFW_KEY_LEFT_BRACKET, sys::virtual_key::OPEN_BRACKET},
    {GLFW_KEY_BACKSLASH, sys::virtual_key::BACK_SLASH},
    {GLFW_KEY_RIGHT_BRACKET, sys::virtual_key::CLOSED_BRACKET},
    {GLFW_KEY_GRAVE_ACCENT, sys::virtual_key::TILDA},
    {GLFW_KEY_WORLD_1, sys::virtual_key::NONE},
    {GLFW_KEY_WORLD_2, sys::virtual_key::NONE},
    {GLFW_KEY_ESCAPE, sys::virtual_key::ESCAPE},
    {GLFW_KEY_ENTER, sys::virtual_key::RETURN},
    {GLFW_KEY_TAB, sys::virtual_key::TAB},
    {GLFW_KEY_BACKSPACE, sys::virtual_key::BACK},
    {GLFW_KEY_INSERT, sys::virtual_key::INSERT},
    {GLFW_KEY_DELETE, sys::virtual_key::DELETE_KEY},
    {GLFW_KEY_RIGHT, sys::virtual_key::RIGHT},
    {GLFW_KEY_LEFT, sys::virtual_key::LEFT},
    {GLFW_KEY_DOWN, sys::virtual_key::DOWN},
    {GLFW_KEY_UP, sys::virtual_key::UP},
    {GLFW_KEY_PAGE_UP, sys::virtual_key::PRIOR},
    {GLFW_KEY_PAGE_DOWN, sys::virtual_key::NEXT},
    {GLFW_KEY_HOME, sys::virtual_key::HOME},
    {GLFW_KEY_END, sys::virtual_key::END},
    {GLFW_KEY_CAPS_LOCK, sys::virtual_key::CAPITAL},
    {GLFW_KEY_SCROLL_LOCK, sys::virtual_key::SCROLL},
    {GLFW_KEY_NUM_LOCK, sys::virtual_key::NUMLOCK},
    {GLFW_KEY_PRINT_SCREEN, sys::virtual_key::PRINT},
    {GLFW_KEY_PAUSE, sys::virtual_key::PAUSE},
    {GLFW_KEY_F1, sys::virtual_key::F1},
    {GLFW_KEY_F2, sys::virtual_key::F2},
    {GLFW_KEY_F3, sys::virtual_key::F3},
    {GLFW_KEY_F4, sys::virtual_key::F4},
    {GLFW_KEY_F5, sys::virtual_key::F5},
    {GLFW_KEY_F6, sys::virtual_key::F6},
    {GLFW_KEY_F7, sys::virtual_key::F7},
    {GLFW_KEY_F8, sys::virtual_key::F8},
    {GLFW_KEY_F9, sys::virtual_key::F9},
    {GLFW_KEY_F10, sys::virtual_key::F10},
    {GLFW_KEY_F11, sys::virtual_key::F11},
    {GLFW_KEY_F12, sys::virtual_key::F12},
    {GLFW_KEY_F13, sys::virtual_key::F13},
    {GLFW_KEY_F14, sys::virtual_key::F14},
    {GLFW_KEY_F15, sys::virtual_key::F15},
    {GLFW_KEY_F16, sys::virtual_key::F16},
    {GLFW_KEY_F17, sys::virtual_key::F17},
    {GLFW_KEY_F18, sys::virtual_key::F18},
    {GLFW_KEY_F19, sys::virtual_key::F19},
    {GLFW_KEY_F20, sys::virtual_key::F20},
    {GLFW_KEY_F21, sys::virtual_key::F21},
    {GLFW_KEY_F22, sys::virtual_key::F22},
    {GLFW_KEY_F23, sys::virtual_key::F23},
    {GLFW_KEY_F24, sys::virtual_key::F24},
    {GLFW_KEY_F25, sys::virtual_key::NONE},
    {GLFW_KEY_KP_0, sys::virtual_key::NUMPAD0},
    {GLFW_KEY_KP_1, sys::virtual_key::NUMPAD1},
    {GLFW_KEY_KP_2, sys::virtual_key::NUMPAD2},
    {GLFW_KEY_KP_3, sys::virtual_key::NUMPAD3},
    {GLFW_KEY_KP_4, sys::virtual_key::NUMPAD4},
    {GLFW_KEY_KP_5, sys::virtual_key::NUMPAD5},
    {GLFW_KEY_KP_6, sys::virtual_key::NUMPAD6},
    {GLFW_KEY_KP_7, sys::virtual_key::NUMPAD7},
    {GLFW_KEY_KP_8, sys::virtual_key::NUMPAD8},
    {GLFW_KEY_KP_9, sys::virtual_key::NUMPAD9},
    {GLFW_KEY_KP_DECIMAL, sys::virtual_key::DECIMAL},
    {GLFW_KEY_KP_DIVIDE, sys::virtual_key::DIVIDE},
    {GLFW_KEY_KP_MULTIPLY, sys::virtual_key::MULTIPLY},
    {GLFW_KEY_KP_SUBTRACT, sys::virtual_key::SUBTRACT},
    {GLFW_KEY_KP_ADD, sys::virtual_key::ADD},
    {GLFW_KEY_KP_ENTER, sys::virtual_key::RETURN},
    {GLFW_KEY_LEFT_SHIFT, sys::virtual_key::LSHIFT},
    {GLFW_KEY_LEFT_CONTROL, sys::virtual_key::LCONTROL},
    {GLFW_KEY_LEFT_ALT, sys::virtual_key::LMENU},
    {GLFW_KEY_LEFT_SUPER, sys::virtual_key::LWIN},
    {GLFW_KEY_RIGHT_SHIFT, sys::virtual_key::RSHIFT},
    {GLFW_KEY_RIGHT_CONTROL, sys::virtual_key::RCONTROL},
    {GLFW_KEY_RIGHT_ALT, sys::virtual_key::RMENU},
    {GLFW_KEY_RIGHT_SUPER, sys::virtual_key::RWIN},
    {GLFW_KEY_MENU, sys::virtual_key::MENU}};

bool is_key_depressed(sys::state const& game_state, sys::virtual_key key) {
	for(auto it = glfw_key_to_virtual_key.begin(); it != glfw_key_to_virtual_key.end(); ++it)
		if(it->second == key)
			return glfwGetKey(game_state.win_ptr->window, it->first) == GLFW_PRESS;

	return false;
}

bool is_in_fullscreen(sys::state const& game_state) { return (game_state.win_ptr) && game_state.win_ptr->in_fullscreen; }

void set_borderless_full_screen(sys::state& game_state, bool fullscreen) {
	if(game_state.win_ptr && game_state.win_ptr->window) {
		// Maybe fix this at some point. Just maximize atm
		if(fullscreen)
			glfwMaximizeWindow(game_state.win_ptr->window);
		else
			glfwRestoreWindow(game_state.win_ptr->window);
	}
}

void close_window(sys::state& game_state) {
	// Signal to close window (should close = yes)
	glfwSetWindowShouldClose(game_state.win_ptr->window, 1);
}

sys::key_modifiers get_current_modifiers(GLFWwindow* window) {
	bool control =
	    (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
	bool alt = (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS);
	bool shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
	uint32_t val = uint32_t(control ? sys::key_modifiers::modifiers_ctrl : sys::key_modifiers::modifiers_none) |
	               uint32_t(alt ? sys::key_modifiers::modifiers_alt : sys::key_modifiers::modifiers_none) |
	               uint32_t(shift ? sys::key_modifiers::modifiers_shift : sys::key_modifiers::modifiers_none);
	return sys::key_modifiers(val);
}

sys::key_modifiers get_current_modifiers(int glfw_mods) {
	uint32_t val = uint32_t((glfw_mods & GLFW_MOD_CONTROL) ? sys::key_modifiers::modifiers_ctrl : sys::key_modifiers::modifiers_none) |
	               uint32_t((glfw_mods & GLFW_MOD_ALT) ? sys::key_modifiers::modifiers_alt : sys::key_modifiers::modifiers_none) |
	               uint32_t((glfw_mods & GLFW_MOD_SHIFT) ? sys::key_modifiers::modifiers_shift : sys::key_modifiers::modifiers_none);
	return sys::key_modifiers(val);
}

static void glfw_error_callback(int error, char const* description) { fprintf(stderr, "Glfw Error %d: %s\n", error, description); }

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	sys::state* state = (sys::state*)glfwGetWindowUserPointer(window);

	sys::virtual_key virtual_key = glfw_key_to_virtual_key.at(key);
	switch(action) {
	case GLFW_PRESS:
		state->on_key_down(virtual_key, get_current_modifiers(mods));
		break;
	case GLFW_RELEASE:
		state->on_key_up(virtual_key, get_current_modifiers(mods));
		break;
	}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	sys::state* state = (sys::state*)glfwGetWindowUserPointer(window);

	int32_t x = (xpos > 0 ? (int32_t)std::round(xpos) : 0);
	int32_t y = (ypos > 0 ? (int32_t)std::round(ypos) : 0);
	state->on_mouse_move(x, y, get_current_modifiers(window));

	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		state->on_mouse_drag(x, y, get_current_modifiers(window));

	state->mouse_x_position = x;
	state->mouse_y_position = y;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	sys::state* state = (sys::state*)glfwGetWindowUserPointer(window);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	int32_t x = (xpos > 0 ? (int32_t)std::round(xpos) : 0);
	int32_t y = (ypos > 0 ? (int32_t)std::round(ypos) : 0);

	switch(action) {
	case GLFW_PRESS:
		if(button == GLFW_MOUSE_BUTTON_LEFT) {
			state->on_lbutton_down(x, y, get_current_modifiers(window));
			state->win_ptr->left_mouse_down = true;
		} else if(button == GLFW_MOUSE_BUTTON_RIGHT) {
			state->on_rbutton_down(x, y, get_current_modifiers(window));
		} else if(button == GLFW_MOUSE_BUTTON_MIDDLE) {
			state->on_mbutton_down(x, y, get_current_modifiers(window));
		}
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		break;
	case GLFW_RELEASE:
		if(button == GLFW_MOUSE_BUTTON_LEFT) {
			state->on_lbutton_up(x, y, get_current_modifiers(window));
			state->win_ptr->left_mouse_down = false;
		} else if(button == GLFW_MOUSE_BUTTON_RIGHT) {
			state->on_rbutton_up(x, y, get_current_modifiers(window));
		} else if(button == GLFW_MOUSE_BUTTON_MIDDLE) {
			state->on_mbutton_up(x, y, get_current_modifiers(window));
		}
		state->mouse_x_position = x;
		state->mouse_y_position = y;
		break;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	sys::state* state = (sys::state*)glfwGetWindowUserPointer(window);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	int32_t x = (xpos > 0 ? (int32_t)std::round(xpos) : 0);
	int32_t y = (ypos > 0 ? (int32_t)std::round(ypos) : 0);

	state->on_mouse_wheel(x, y, get_current_modifiers(window), (float)yoffset);
	state->mouse_x_position = x;
	state->mouse_y_position = y;
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
	sys::state* state = (sys::state*)glfwGetWindowUserPointer(window);
	if(state->ui_state.edit_target) {
		// TODO change UTF32 to (win1250??)
		state->on_text(char(codepoint));
	}
}

void on_window_change(GLFWwindow* window) {
	sys::state* state = (sys::state*)glfwGetWindowUserPointer(window);

	sys::window_state t = sys::window_state::normal;
	if(glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_MAXIMIZED)
		t = sys::window_state::maximized;
	else if(glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_ICONIFIED)
		t = sys::window_state::minimized;

	// redo OpenGL viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	state->on_resize(width, height, t);
	state->x_size = width;
	state->y_size = height;
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	// on_window_change(window);
	// framebuffer_size_callback should be enough
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { on_window_change(window); }

void window_iconify_callback(GLFWwindow* window, int iconified) { on_window_change(window); }

void window_maximize_callback(GLFWwindow* window, int maximized) { on_window_change(window); }

void create_window(sys::state& game_state, creation_parameters const& params) {
	game_state.win_ptr = std::make_unique<window_data_impl>();
	game_state.win_ptr->creation_x_size = params.size_x;
	game_state.win_ptr->creation_y_size = params.size_y;
	game_state.win_ptr->in_fullscreen = params.borderless_fullscreen;

	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if(!glfwInit())
		std::abort(); // throw "Failed to init glfw\n";

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// Create window with graphics context
	auto* window = glfwCreateWindow(params.size_x, params.size_y, "Project Alice", NULL, NULL);
	if(window == NULL)
		std::abort(); // throw "Failed to create window\n";
	game_state.win_ptr->window = window;

	glfwSetWindowUserPointer(window, &game_state);

	// event callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCharCallback(window, character_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetWindowIconifyCallback(window, window_iconify_callback);
	glfwSetWindowMaximizeCallback(window, window_maximize_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	ogl::initialize_opengl(game_state);

	sound::initialize_sound_system(game_state);
	sound::start_music(game_state, game_state.user_settings.master_volume * game_state.user_settings.music_volume);

	on_window_change(window); // Init the window size

	game_state.on_create();

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		// Run game code

		game_state.render();
		glfwSwapBuffers(window);

		sound::update_music_track(game_state);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}

void emit_error_message(std::string const& content, bool fatal) {
	printf("%s", content.c_str());
	if(fatal) {
		std::terminate();
	}
}

} // namespace window
