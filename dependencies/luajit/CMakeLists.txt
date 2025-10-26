cmake_minimum_required(VERSION 3.15)

add_library(lib_luajit INTERFACE)

if(WIN32)
	target_link_libraries(lib_luajit INTERFACE ${PROJECT_SOURCE_DIR}/libs/luajit.lib)
else()
	include(ExternalProject)
		ExternalProject_Add(
		luajit

		GIT_REPOSITORY https://luajit.org/git/luajit.git
		GIT_TAG origin/v2.1
		GIT_SHALLOW FALSE
		GIT_PROGRESS TRUE

		CONFIGURE_COMMAND ""
		BUILD_COMMAND make

		BUILD_IN_SOURCE TRUE

		INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install/luajit
		INSTALL_COMMAND make install PREFIX=<INSTALL_DIR>
	)
	target_link_libraries(lib_luajit INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/install/luajit/lib/libluajit-5.1.so)
endif

