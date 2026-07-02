cmake_minimum_required(VERSION 3.15)

if(WIN32)
	message(STATUS "A certain luajit version is distributed with Alice")
	add_library(lib_luajit INTERFACE)
	target_link_libraries(lib_luajit INTERFACE ${PROJECT_SOURCE_DIR}/libs/luajit.lib)
else()
	message(STATUS "Fetching luajit...")
	include(ExternalProject)
	set(LUAJIT_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/luajit/source)
	set(LUAJIT_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/luajit/luajit-prefix)
	set(LUAJIT_BUILD_LIBRARY ${LUAJIT_SOURCE_DIR}/src/libluajit.so)
	set(LUAJIT_INSTALLED_LIBRARY ${LUAJIT_INSTALL_DIR}/lib/libluajit-5.1.so)
	if(APPLE)
		set(LUAJIT_MACOSX_DEPLOYMENT_TARGET "${CMAKE_OSX_DEPLOYMENT_TARGET}")
		if(NOT LUAJIT_MACOSX_DEPLOYMENT_TARGET)
			set(LUAJIT_MACOSX_DEPLOYMENT_TARGET "11.0")
		endif()
		set(LUAJIT_BUILD_ENV ${CMAKE_COMMAND} -E env MACOSX_DEPLOYMENT_TARGET=${LUAJIT_MACOSX_DEPLOYMENT_TARGET})
		set(LUAJIT_INSTALLED_LIBRARY ${LUAJIT_INSTALL_DIR}/lib/libluajit-5.1.dylib)
	else()
		set(LUAJIT_BUILD_ENV)
	endif()
	ExternalProject_Add(
		luajit

		SOURCE_DIR ${LUAJIT_SOURCE_DIR}
		INSTALL_DIR ${LUAJIT_INSTALL_DIR}

		GIT_REPOSITORY https://luajit.org/git/luajit.git
		GIT_TAG origin/v2.1
		GIT_SHALLOW FALSE
		GIT_PROGRESS TRUE

		CONFIGURE_COMMAND ""
		BUILD_COMMAND ${LUAJIT_BUILD_ENV} make

		BUILD_IN_SOURCE TRUE
		BUILD_BYPRODUCTS
			${LUAJIT_SOURCE_DIR}/luajit.h
			${LUAJIT_BUILD_LIBRARY}
			${LUAJIT_INSTALLED_LIBRARY}
		INSTALL_COMMAND
			${LUAJIT_BUILD_ENV} make install DESTDIR=${DESTDIR} PREFIX=<INSTALL_DIR>
			COMMAND ${CMAKE_COMMAND}
				-DLUAJIT_INSTALL_DIR=<INSTALL_DIR>
				-P ${CMAKE_CURRENT_SOURCE_DIR}/fix_luajit_install_name.cmake
	)
	add_library(lib_luajit INTERFACE ${LUAJIT_SOURCE_DIR}/luajit.h)
	add_dependencies(lib_luajit luajit)
	target_link_libraries(lib_luajit INTERFACE ${LUAJIT_INSTALLED_LIBRARY})
	if(APPLE)
		target_link_options(lib_luajit INTERFACE "LINKER:-rpath,${LUAJIT_INSTALL_DIR}/lib")
	endif()
endif()
