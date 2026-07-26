#include <GL/glew.h>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace ogl {

#ifndef NDEBUG
static bool opengl_debug_enabled() {
	static bool enabled = [] {
		auto const* value = std::getenv("ALICE_DEBUG_GL_CONTEXT");
		return value && value[0] == '1';
	}();
	return enabled;
}

static bool opengl_context_debug_flag_set() {
	GLint flags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	return (flags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0;
}
#endif

void create_opengl_context(sys::state& state) {
	assert(state.win_ptr && state.win_ptr->window);

	glfwMakeContextCurrent(state.win_ptr->window);
	glfwSwapInterval(1); // Vsync option

	glewExperimental = GL_TRUE;
	auto const glew_result = glewInit();
	if(glew_result != GLEW_OK) {
		window::emit_error_message(std::string("GLEW failed to initialize: ") + reinterpret_cast<char const*>(glewGetErrorString(glew_result)), true);
	}
#ifndef NDEBUG
	if(opengl_debug_enabled()) {
		while(glGetError() != GL_NO_ERROR) {
		}
		GLint major = 0, minor = 0, flags = 0, profile = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
		std::fprintf(stderr,
			"GL_CONTEXT_INFO vendor=%s renderer=%s version=%s glsl=%s major=%d minor=%d flags=0x%x profile=0x%x debug=%s forward_compat=%s\n",
			glGetString(GL_VENDOR),
			glGetString(GL_RENDERER),
			glGetString(GL_VERSION),
			glGetString(GL_SHADING_LANGUAGE_VERSION),
			major, minor, flags, profile,
			(flags & GL_CONTEXT_FLAG_DEBUG_BIT) ? "true" : "false",
			(flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) ? "true" : "false");
		std::fprintf(stderr,
			"GLEW_STATUS result=%u version_4_3=%d khr_debug=%d arb_debug_output=%d callback=%p control=%p callback_arb=%p control_arb=%p\n",
			unsigned(glew_result),
			int(GLEW_VERSION_4_3),
			int(GLEW_KHR_debug),
			int(GLEW_ARB_debug_output),
			reinterpret_cast<void*>(glDebugMessageCallback),
			reinterpret_cast<void*>(glDebugMessageControl),
			reinterpret_cast<void*>(glDebugMessageCallbackARB),
			reinterpret_cast<void*>(glDebugMessageControlARB));
		std::fflush(stderr);
	}
#endif
#ifndef NDEBUG
	bool installed_debug_output = false;
	if(opengl_context_debug_flag_set() && GLEW_KHR_debug && glDebugMessageCallback && glDebugMessageControl) {
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(debug_callback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
		installed_debug_output = true;
	} else if(opengl_debug_enabled()) {
		std::fprintf(stderr, "OpenGL debug output unavailable; continuing without callback\n");
		std::fflush(stderr);
	}
	if(opengl_debug_enabled()) {
		std::fprintf(stderr, "DEBUG_CALLBACK_INSTALLED=%s\n", installed_debug_output ? "true" : "false");
		std::fflush(stderr);
	}
#endif
}

void shutdown_opengl(sys::state& state) { }
} // namespace ogl
