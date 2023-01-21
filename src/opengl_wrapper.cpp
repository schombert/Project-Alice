#include "opengl_wrapper.hpp"

#ifdef _WIN64
#include "opengl_wrapper_win.cpp"
#else
#include "opengl_wrapper_nix.cpp"
#endif

namespace ogl {
	void intialize_opengl(sys::state& state) {
		create_opengl_context(state);

		// do opengl stuff with our context -- create shaders, etc

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_SCISSOR_TEST);
		glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
	}
}