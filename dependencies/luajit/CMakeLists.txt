cmake_minimum_required(VERSION 3.15)

if(WIN32)
	message(STATUS "A certain luajit version is distributed with Alice")
	add_library(lib_luajit INTERFACE)
	target_link_libraries(lib_luajit INTERFACE ${PROJECT_SOURCE_DIR}/libs/luajit.lib)
else()
	message(STATUS "Fetching luajit...")
	include(ExternalProject)
	if(APPLE)
		set(LUAJIT_MACOSX_DEPLOYMENT_TARGET "${CMAKE_OSX_DEPLOYMENT_TARGET}")
		if(NOT LUAJIT_MACOSX_DEPLOYMENT_TARGET)
			set(LUAJIT_MACOSX_DEPLOYMENT_TARGET "11.0")
		endif()
		set(LUAJIT_BUILD_ENV ${CMAKE_COMMAND} -E env MACOSX_DEPLOYMENT_TARGET=${LUAJIT_MACOSX_DEPLOYMENT_TARGET})
	else()
		set(LUAJIT_BUILD_ENV)
	endif()
	ExternalProject_Add(
		luajit

		SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/luajit/source

		GIT_REPOSITORY https://luajit.org/git/luajit.git
		GIT_TAG origin/v2.1
		GIT_SHALLOW FALSE
		GIT_PROGRESS TRUE

		CONFIGURE_COMMAND ""
		BUILD_COMMAND ${LUAJIT_BUILD_ENV} make

		BUILD_IN_SOURCE TRUE
		BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/luajit/source/luajit.h
		BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/luajit/source/src/libluajit.so
		INSTALL_COMMAND ${LUAJIT_BUILD_ENV} make install DESTDIR=${DESTDIR} PREFIX=<INSTALL_DIR>
	)
	add_library(lib_luajit INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/luajit/source/luajit.h)
	add_dependencies(lib_luajit luajit)
	target_link_libraries(lib_luajit INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/luajit/source/src/libluajit.so)
endif()
