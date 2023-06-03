#include <GL/glew.h>
#include <string>

namespace ogl {

void create_opengl_context(sys::state& state) {
	assert(state.win_ptr && state.win_ptr->window);

	glfwMakeContextCurrent(state.win_ptr->window);
	glfwSwapInterval(1); // Vsync option

	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK)
		std::abort(); // throw "Failed to init glew\n";

#ifndef NDEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(debug_callback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
#endif
}

void shutdown_opengl(sys::state& state) {
}
} // namespace ogl
