cmake_minimum_required(VERSION 3.15)

if(WIN32)
	message(STATUS "A certain luajit version is distributed with Alice")
	add_library(lib_luajit INTERFACE)
	target_link_libraries(lib_luajit INTERFACE ${PROJECT_SOURCE_DIR}/libs/luajit.lib)
else()
	message(STATUS "Fetching luajit...")
	include(ExternalProject)
	ExternalProject_Add(
		luajit

		SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/luajit/source

		GIT_REPOSITORY https://luajit.org/git/luajit.git
		GIT_TAG origin/v2.1
		GIT_SHALLOW FALSE
		GIT_PROGRESS TRUE

		CONFIGURE_COMMAND ""
		BUILD_COMMAND make

		BUILD_IN_SOURCE TRUE
		BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/luajit/source/luajit.h
		BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/luajit/source/src/libluajit.so
		INSTALL_DIR ./luajit/install
		INSTALL_COMMAND make install PREFIX=<INSTALL_DIR>
	)
	add_library(lib_luajit INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/luajit/source/luajit.h)
	add_dependencies(lib_luajit luajit)
	target_link_libraries(lib_luajit INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/luajit/source/src/libluajit.so)
endif()

