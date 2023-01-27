include(FetchContent)

add_subdirectory(DataContainerGenerator)
add_subdirectory(catch2)
add_subdirectory(unordered_dense)
add_subdirectory(glew)
add_subdirectory(stb)
add_subdirectory(freetype)

if (NOT WIN32)
	add_subdirectory(glfw)
endif()