#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

#include "system_state.hpp"

namespace ogl {
	void create_opengl_context(sys::state& state); // you shouldn't call this directly; only intialize_opengl should call it
	void intialize_opengl(sys::state& state);
	void shutdown_opengl(sys::state& state);
}
