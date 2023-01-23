#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

#include "system_state.hpp"

namespace ogl {

#ifndef NDEBUG
	void debug_callback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei, // length
		const GLchar* message,
		const void*) {

		std::string source_str;
		switch(source) {
			case GL_DEBUG_SOURCE_API:
				source_str = "OpenGL API call"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
				source_str = "Window system API"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER:
				source_str = "Shading language compiler"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:
				source_str = "Application associated with OpenGL"; break;
			case GL_DEBUG_SOURCE_APPLICATION:
				source_str = "User generated"; break;
			case GL_DEBUG_SOURCE_OTHER:
				source_str = "Unknown source"; break;
		}
		std::string error_type;
		switch(type) {
			case GL_DEBUG_TYPE_ERROR:
				error_type = "General error"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
				error_type = "Deprecated behavior"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
				error_type = "Undefined behavior"; break;
			case GL_DEBUG_TYPE_PORTABILITY:
				error_type = "Portability issue"; break;
			case GL_DEBUG_TYPE_PERFORMANCE:
				error_type = "Performance issue"; break;
			case GL_DEBUG_TYPE_MARKER:
				error_type = "Command stream annotation"; break;
			case GL_DEBUG_TYPE_PUSH_GROUP:
				error_type = "Error group push"; break;
			case GL_DEBUG_TYPE_POP_GROUP:
				error_type = "Error group pop"; break;
			case GL_DEBUG_TYPE_OTHER:
				error_type = "Uknown error type"; break;
		}
		std::string severity_str;
		switch(severity) {
			case GL_DEBUG_SEVERITY_HIGH:
				severity_str = "High"; break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				severity_str = "Medium"; break;
			case GL_DEBUG_SEVERITY_LOW:
				severity_str = "Low"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				severity_str = "Notification"; break;
		}
		std::string full_message("OpenGL error ");
		full_message += std::to_string(id);
		full_message += " ";
		full_message += " source: ";
		full_message += source_str;
		full_message += " type: ";
		full_message += error_type;
		full_message += " severity: ";
		full_message += severity_str;
		full_message += "; ";
		full_message += message;
		full_message += "\n";

#ifdef _WIN64
		OutputDebugStringA(full_message.c_str());
#else
		printf("%s", full_message.c_str());
#endif
	}
#endif

	void create_opengl_context(sys::state& state); // you shouldn't call this directly; only intialize_opengl should call it
	void intialize_opengl(sys::state& state);
	void shutdown_opengl(sys::state& state);
}
