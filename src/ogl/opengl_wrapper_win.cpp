#include "wglew.h"

#include <cassert>

#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "Windows.h"

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

		OutputDebugStringA(full_message.c_str());
	}
#endif

	void create_opengl_context(sys::state& state) {
		assert(state.win_ptr && state.win_ptr->hwnd && !state.opengl_context);

		PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;

		HDC window_dc = state.win_ptr->opengl_window_dc;

		const int pixel_format = ChoosePixelFormat(window_dc, &pfd);
		SetPixelFormat(window_dc, pixel_format, &pfd);

		auto handle_to_ogl_dc = wglCreateContext(window_dc);
		wglMakeCurrent(window_dc, handle_to_ogl_dc);

		glewExperimental = GL_TRUE;

		if(glewInit() != 0) {
			MessageBoxW(state.win_ptr->hwnd, L"GLEW failed to initialize", L"GLEW error", MB_OK);
		}

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 5,
			WGL_CONTEXT_FLAGS_ARB,
#ifndef NDEBUG
			WGL_CONTEXT_DEBUG_BIT_ARB |
#endif
			0,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		if(wglewIsSupported("WGL_ARB_create_context") != 1) {
			MessageBoxW(state.win_ptr->hwnd, L"WGL_ARB_create_context not supported", L"OpenGL error", MB_OK);
			std::abort();
		} else {
			state.opengl_context = wglCreateContextAttribsARB(window_dc, nullptr, attribs);

			wglMakeCurrent(window_dc, HGLRC(state.opengl_context));
			wglDeleteContext(handle_to_ogl_dc);

			//wglMakeCurrent(window_dc, HGLRC(state.opengl_context));
#ifndef NDEBUG
			glDebugMessageCallback(debug_callback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
			glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
#endif

			if(wglewIsSupported("WGL_EXT_swap_control_tear") == 1) {
				wglSwapIntervalEXT(-1);
			} else if(wglewIsSupported("WGL_EXT_swap_control") == 1) {
				wglSwapIntervalEXT(1);
			} else {
				MessageBoxW(state.win_ptr->hwnd, L"WGL_EXT_swap_control_tear and WGL_EXT_swap_control not supported", L"OpenGL error", MB_OK);
			}
		}
	}

	void shutdown_opengl(sys::state& state) {
		assert(state.win_ptr && state.win_ptr->hwnd && state.opengl_context);
		wglMakeCurrent(state.win_ptr->opengl_window_dc, nullptr);
		wglDeleteContext(HGLRC(state.opengl_context));
		state.opengl_context = nullptr;
	}
}