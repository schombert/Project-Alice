include(FetchContent)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

add_subdirectory(DataContainerGenerator)
add_subdirectory(catch2)
add_subdirectory(glew)
add_subdirectory(stb)
add_subdirectory(glm)
add_subdirectory(freetype)
if (NOT WIN32)
	add_subdirectory(glfw)
	add_subdirectory(miniaudio)
	add_subdirectory(tbb)
	add_subdirectory(icu)
	add_subdirectory(fmt)
endif()
