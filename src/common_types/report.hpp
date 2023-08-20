#pragma once
#include <cstdio>
#include <string_view>
#ifdef _WIN64

#else
#include <csignal>
#endif

namespace report {

inline void fatal_error(std::string_view msg) noexcept {
	std::string text = "Fatal: " + std::string(msg) + "\n";
#ifdef _WIN64
	if(IsDebuggerPresent()) {
		OutputDebugStringA(text.c_str());
		FatalExit(EXIT_FAILURE);
	} else {
		MessageBoxA(nullptr, text.c_str(), "Project Alice has encountered a fatal error", MB_OK | MB_ICONERROR);
		if(GetConsoleWindow() != nullptr) {
			WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), text.c_str(), DWORD(text.length()), nullptr, nullptr);
		}
	}
#else
	std::fprintf(stderr, "%s", text.c_str());
	std::raise(SIGTRAP);
#endif
    std::exit(EXIT_FAILURE);
}

inline void error(std::string_view msg) noexcept {
	std::string text = "Error: " + std::string(msg) + "\n";
#ifdef _WIN64
	if(IsDebuggerPresent()) {
		OutputDebugStringA(text.c_str());
		DebugBreak();
	} else {
		MessageBoxA(nullptr, text.c_str(), "Project Alice has encountered the following problem", MB_OK | MB_ICONWARNING);
		if(GetConsoleWindow() != nullptr) {
			WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), text.c_str(), DWORD(text.length()), nullptr, nullptr);
		}
	}
#else
	std::fprintf(stderr, "%s", text.c_str());
	std::raise(SIGTRAP);
#endif
}

inline void warning(std::string_view msg) noexcept {
	std::string text = "Warning: " + std::string(msg) + "\n";
#ifdef _WIN64
	if(IsDebuggerPresent()) {
		OutputDebugStringA(text.c_str());
	} else if(GetConsoleWindow() != nullptr) {
		WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), text.c_str(), DWORD(text.length()), nullptr, nullptr);
	}
#else
	std::fprintf(stderr, "%s", text.c_str());
#endif
}

inline void debug(std::string_view msg) noexcept {
	std::string text = "Debug: " + std::string(msg) + "\n";
#ifdef _WIN64
	if(IsDebuggerPresent()) {
		OutputDebugStringA(text.c_str());
	} else if(GetConsoleWindow() != nullptr) {
		WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), text.c_str(), DWORD(text.length()), nullptr, nullptr);
	}
#else
	std::fprintf(stderr, "%s", text.c_str());
#endif
}

}
