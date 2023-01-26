#include "window.hpp"

#include <GLFW/glfw3.h>

namespace window {
	class window_data_impl {
	public:
		// HWND hwnd = nullptr;
		// HDC opengl_window_dc = nullptr;
		GLFWwindow *window = nullptr;

		int32_t creation_x_size = 600;
		int32_t creation_y_size = 400;

		bool in_fullscreen = false;
		bool left_mouse_down = false;
	};

	bool is_key_depressed(sys::state const& game_state, sys::virtual_key key) {
		return glfwGetKey(game_state.win_ptr->window, int32_t(key)) == GLFW_PRESS;
	}

	bool is_in_fullscreen(sys::state const& game_state) {
		return (game_state.win_ptr) && game_state.win_ptr->in_fullscreen;
	}

	void set_borderless_full_screen(sys::state& game_state, bool fullscreen) {
		if(game_state.win_ptr && game_state.win_ptr->window) {
			// Maybe fix this at some point. Just maximize atm
			if (fullscreen)
				glfwMaximizeWindow(game_state.win_ptr->window);
			else
				glfwRestoreWindow(game_state.win_ptr->window);
		}
	}

	void close_window(sys::state& game_state) {
		if(game_state.win_ptr->window) {
			glfwDestroyWindow(game_state.win_ptr->window);
			glfwTerminate();
		}
	}

	sys::key_modifiers get_current_modifiers(GLFWwindow *window) {
		bool control = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) ||
			(glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
		bool alt = (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) ||
			(glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS);
		bool shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ||
			(glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
		uint32_t val =  
			uint32_t(control ? sys::key_modifiers::modifiers_ctrl : sys::key_modifiers::modifiers_none) |
			uint32_t(alt ? sys::key_modifiers::modifiers_alt : sys::key_modifiers::modifiers_none) |
			uint32_t(shift ? sys::key_modifiers::modifiers_shift : sys::key_modifiers::modifiers_none);
		return sys::key_modifiers(val);
	}
	
	sys::key_modifiers get_current_modifiers(int glfw_mods) {
		uint32_t val =  
			uint32_t((glfw_mods & GLFW_MOD_CONTROL) ? sys::key_modifiers::modifiers_ctrl : sys::key_modifiers::modifiers_none) |
			uint32_t((glfw_mods & GLFW_MOD_ALT) ? sys::key_modifiers::modifiers_alt : sys::key_modifiers::modifiers_none) |
			uint32_t((glfw_mods & GLFW_MOD_SHIFT) ? sys::key_modifiers::modifiers_shift : sys::key_modifiers::modifiers_none);
		return sys::key_modifiers(val);
	}
	
	static void glfw_error_callback(int error, const char *description) {
		fprintf(stderr, "Glfw Error %d: %s\n", error, description);
	}

	static void key_callback (GLFWwindow* window, int key, int scancode, int action, int mods) {
		sys::state* state = (sys::state*)glfwGetWindowUserPointer(window);
		switch (action) {
			case GLFW_PRESS:
				state->on_key_down(sys::virtual_key(key), get_current_modifiers(mods));
				break;
			case GLFW_RELEASE:
				state->on_key_up(sys::virtual_key(key), get_current_modifiers(mods));
				break;
		}
	}

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
		sys::state* state = (sys::state*)glfwGetWindowUserPointer(window);

		state->on_mouse_move((int32_t)xpos, (int32_t)ypos, get_current_modifiers(window));

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
			state->on_mouse_drag((int32_t)xpos, (int32_t)ypos, get_current_modifiers(window));
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		sys::state* state = (sys::state*)glfwGetWindowUserPointer(window);

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		int32_t x = (int32_t) xpos;
		int32_t y = (int32_t) xpos;

		switch (action) {
			case GLFW_PRESS:
				if (button == GLFW_MOUSE_BUTTON_LEFT) {
					state->on_lbutton_down(x, y, get_current_modifiers(window));
					state->win_ptr->left_mouse_down = true;
				} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
					state->on_rbutton_down(x, y, get_current_modifiers(window));
				}
				state->mouse_x_position = x;
				state->mouse_y_position = y;
				break;
			case GLFW_RELEASE:
				if (button == GLFW_MOUSE_BUTTON_LEFT) {
					state->on_lbutton_up(x, y, get_current_modifiers(window));
					state->win_ptr->left_mouse_down = false;
				} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
					state->on_rbutton_up(x, y, get_current_modifiers(window));
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
		int32_t x = (int32_t) xpos;
		int32_t y = (int32_t) xpos;

		state->on_mouse_wheel(x, y, get_current_modifiers(window), (float)(yoffset) / 120.0f);
		state->mouse_x_position = x;
		state->mouse_y_position = y;
	}

	void character_callback(GLFWwindow* window, unsigned int codepoint) {
		sys::state* state = (sys::state*)glfwGetWindowUserPointer(window);
		if(state->in_edit_control) {
			// TODO change UTF32 to (win1250??)
			// state->on_text(turned_into);
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
		glViewport(0, 0, width, height);
		state->on_resize(width, height, t);
		state->x_size = width;
		state->y_size = height;
	}

	void window_size_callback(GLFWwindow* window, int width, int height) {
		on_window_change(window);
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		on_window_change(window);
	}


	void window_iconify_callback(GLFWwindow* window, int iconified) {
		on_window_change(window);
	}

	void window_maximize_callback(GLFWwindow* window, int maximized) {
		on_window_change(window);
	}


	void create_window(sys::state& game_state, creation_parameters const& params) {
		game_state.win_ptr = std::make_unique<window_data_impl>();
		game_state.win_ptr->creation_x_size = params.size_x;
		game_state.win_ptr->creation_y_size = params.size_y;
		game_state.win_ptr->in_fullscreen = params.borderless_fullscreen;

		// Setup window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
			std::abort(); // throw "Failed to init glfw\n";

		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

		// Create window with graphics context
		auto *window = glfwCreateWindow(params.size_x, params.size_y, "Project Alice", NULL, NULL);
		if (window == NULL)
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

		glViewport(0, 0, params.size_x, params.size_y);

		while(!glfwWindowShouldClose (window)) {
			glfwPollEvents();
			// Run game code

			game_state.render();
			glfwSwapBuffers(window);
		}

		close_window(game_state);
	}
}
