# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#

cmake_minimum_required (VERSION 3.15)

project (ProjectAlice LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Turn on compile_commands.json export for wider editor/IDE support.
# This option is ignored on anything but Unix makefiles/Ninja.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

# Copy everything from the assets directory into the CMAKE_CURRENT_BINARY_DIR/assets directory
file(GLOB_RECURSE TESTING ${PROJECT_SOURCE_DIR}/assets/*)
foreach(file ${TESTING})
	string(LENGTH "${PROJECT_SOURCE_DIR}/" SOURCE_DIR_LENGHT)
	string(SUBSTRING ${file} ${SOURCE_DIR_LENGHT} -1 file)
	list(APPEND ASSET_FILES "${CMAKE_CURRENT_BINARY_DIR}/${file}")

	add_custom_command(
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}
			DEPENDS ${PROJECT_SOURCE_DIR}/${file}
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
					${PROJECT_SOURCE_DIR}/${file}
					${CMAKE_CURRENT_BINARY_DIR}/${file})
endforeach()

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

list(APPEND ALICE_SOURCES_LIST
	"src/main.cpp"
	"src/ai/ai.cpp"
	"src/common_types/blake2.cpp"
	"src/common_types/date_interface.cpp"
	"src/common_types/prng.cpp"
	"src/culture/culture.cpp"
	"src/culture/politics.cpp"
	"src/culture/rebels.cpp"
	"src/economy/demographics.cpp"
	"src/economy/economy.cpp"
	"src/gamestate/commands.cpp"
	"src/gamestate/diplomatic_messages.cpp"
	"src/gamestate/modifiers.cpp"
	"src/gamestate/notifications.cpp"
	"src/gamestate/serialization.cpp"
	"src/graphics/opengl_wrapper.cpp"
	"src/graphics/texture.cpp"
	"src/gui/gui_common_elements.cpp"
	"src/gui/gui_effect_tooltips.cpp"
	"src/gui/gui_graphics.cpp"
	"src/gui/gui_leader_select.cpp"
	"src/gui/gui_leader_tooltip.cpp"
	"src/gui/gui_modifier_tooltips.cpp"
	"src/gui/gui_province_window.cpp"
	"src/gui/gui_trigger_tooltips.cpp"
	"src/gui/map_tooltip.cpp"
	"src/gui/topbar_subwindows/gui_population_window.cpp"
	"src/gui/topbar_subwindows/gui_production_window.cpp"
	"src/gui/topbar_subwindows/gui_technology_window.cpp"
	"src/gui/topbar_subwindows/gui_budget_window.cpp"
	"src/gui/unit_tooltip.cpp"
	"src/map/map.cpp"
	"src/map/map_borders.cpp"
	"src/map/map_data_loading.cpp"
	"src/map/map_modes.cpp"
	"src/map/map_state.cpp"
	"src/military/military.cpp"
	"src/nations/nations.cpp"
	"src/network/network.cpp"
	"src/parsing/float_from_chars.cpp"
	"src/parsing/parsers.cpp"
	"src/platform_specific.cpp"
	"src/provinces/province.cpp"
	"src/scripting/effects.cpp"
	"src/scripting/events.cpp"
	"src/scripting/triggers.cpp"
	"src/text/bmfont.cpp"
	"src/text/fonts.cpp"
	"src/text/text.cpp"
	"src/zstd/zstd.cpp"
	"src/graphics/xac.cpp"
)

list(APPEND ALICE_MAIN_SOURCES_LIST
	"src/main.cpp"
	"src/map/map_state.cpp"
	"src/map/map_data_loading.cpp"
	"src/map/map_borders.cpp"
	"src/map/map.cpp"
	"src/graphics/xac.cpp"
)
if(WIN32)
	list(APPEND ALICE_MAIN_SOURCES_LIST "src/alice.rc")
	add_executable(AliceAVX512 WIN32 ${ALICE_MAIN_SOURCES_LIST} ${ASSET_FILES})
	add_executable(AliceAVX2 WIN32 ${ALICE_MAIN_SOURCES_LIST} ${ASSET_FILES})
	add_executable(AliceAVX WIN32 ${ALICE_MAIN_SOURCES_LIST} ${ASSET_FILES})
	add_executable(AliceSSE42 WIN32 ${ALICE_MAIN_SOURCES_LIST} ${ASSET_FILES})
	add_executable(AliceSSE3 WIN32 ${ALICE_MAIN_SOURCES_LIST} ${ASSET_FILES})
	add_executable(AliceIncremental WIN32 ${ALICE_SOURCES_LIST} "src/alice.rc" ${ASSET_FILES})
else()
	add_executable(AliceAVX512 WIN32 ${ALICE_MAIN_SOURCES_LIST} ${ASSET_FILES})
	add_executable(AliceAVX2 WIN32 ${ALICE_MAIN_SOURCES_LIST} ${ASSET_FILES})
	add_executable(AliceAVX WIN32 ${ALICE_MAIN_SOURCES_LIST} ${ASSET_FILES})
	add_executable(AliceSSE42 WIN32 ${ALICE_MAIN_SOURCES_LIST} ${ASSET_FILES})
	add_executable(AliceSSE3 WIN32 ${ALICE_MAIN_SOURCES_LIST} ${ASSET_FILES})
	add_executable(AliceIncremental WIN32 ${ALICE_SOURCES_LIST} ${ASSET_FILES})
endif()

target_compile_definitions(AliceIncremental PRIVATE INCREMENTAL=1)

if(NOT WIN32)
	add_compile_definitions(PREFER_ONE_TBB)
endif()

# Include sub-projects.
add_subdirectory(dependencies)
add_subdirectory(ParserGenerator)

add_library(AliceCommon INTERFACE)
# Used for storing the local path to the Victoria 2 directory
if(EXISTS ${PROJECT_SOURCE_DIR}/src/local_user_settings.hpp)
	target_compile_definitions(AliceCommon INTERFACE LOCAL_USER_SETTINGS)
else()
	target_compile_definitions(AliceCommon INTERFACE "GAME_DIR=\"NONE\"")
	target_compile_definitions(AliceCommon INTERFACE "IGNORE_REAL_FILES_TESTS=1")
endif()
target_compile_definitions(AliceCommon INTERFACE "PROJECT_ROOT=\"${PROJECT_SOURCE_DIR}\"")
if(WIN32)
	# string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	# string(REPLACE "/MD" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	# string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

	# To build Non-AVX version : change all "/arch:AVX2" to "/d2archSSE42"
	# -march=nehalem for clang release
	set(CMAKE_CXX_FLAGS "")
	set(CMAKE_CXX_FLAGS_DEBUG "")
	set(CMAKE_CXX_FLAGS_RELEASE "")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(AliceCommon INTERFACE /bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /GR- /W4 /permissive- /WX /GF /w34388 /w34389 -Wno-unused-macros -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field /Z7 -Wno-invalid-offsetof -Wno-deprecated-volatile -Wno-missing-prototypes -Wno-reserved-identifier -Wno-implicit-int-float-conversion -Wno-unsafe-buffer-usage -Wno-float-equal -Wno-shadow-field-in-constructor -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-double-promotion -Wno-shadow-uncaptured-local -Wno-nonportable-system-include-path -Wno-format-nonliteral -Wno-shorten-64-to-32 -Wno-ctad-maybe-unsupported -Wno-implicit-int-conversion -Wno-disabled-macro-expansion -Wno-comma -Wno-cast-function-type-strict -Wno-cast-function-type)
		if(OPTIMIZE_MODE STREQUAL "On")
			message(STATUS "Optimizing with PGO data")
			target_compile_options(AliceCommon INTERFACE /DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline -Wno-profile-instr-missing -Wno-profile-instr-out-of-date -fprofile-instr-use=code.profdata)
			target_link_options(AliceCommon INTERFACE /OPT:REF /OPT:ICF /LTCG -fprofile-instr-use=code.profdata)
		elseif(PROFILE_MODE STREQUAL "On")
			message(STATUS "Compiling for PGO instrumentation")
			target_compile_options(AliceCommon INTERFACE  /DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline -fprofile-instr-generate)
			target_link_options(AliceCommon INTERFACE /DEBUG:FULL /OPT:REF /OPT:ICF /LTCG -fprofile-instr-generate)
		else()
			target_compile_options(AliceCommon INTERFACE
				$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od>
				$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline>)
			target_link_options(AliceCommon INTERFACE
				$<$<CONFIG:Debug>: 			/DEBUG:FULL>
				$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF /LTCG>)
		endif()
	else()
		set_target_properties(AliceCommon PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
		target_compile_options(AliceCommon INTERFACE /bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /wd4723 /GR- /W4 /permissive- /Zc:preprocessor /WX /GF /w34388 /w34389)
		if(OPTIMIZE_MODE STREQUAL "On")
			message(STATUS "Optimizing with PGO data")
			target_compile_options(AliceCommon INTERFACE /DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:preprocessor /Zc:inline)
			target_link_options(AliceCommon INTERFACE /OPT:REF /OPT:ICF /LTCG /USEPROFILE)
		elseif(PROFILE_MODE STREQUAL "On")
			message(STATUS "Compiling for PGO instrumentation")
			target_compile_options(AliceCommon INTERFACE /Z7 /DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:preprocessor /Zc:inline)
			target_link_options(AliceCommon INTERFACE /DEBUG:FULL /OPT:REF /OPT:ICF /LTCG /GENPROFILE)
		else()
			message(STATUS "Normal, not PGO, build")
			target_compile_options(AliceCommon INTERFACE /Z7
				$<$<CONFIG:Debug>: /EHsc /MTd /RTC1 /Od>
# for faster debug builds, replace /RTC1 /Od with /O1 -- should be able to use /Ox, but for some reason that crashes ZSTD
				$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:preprocessor /Zc:inline>)
			target_link_options(AliceCommon INTERFACE
				$<$<CONFIG:Debug>: 			/DEBUG:FULL >
				$<$<NOT:$<CONFIG:Debug>>: 	/DEBUG:FULL /OPT:REF /OPT:ICF /LTCG>)
		endif()
	endif()
else() # GCC or CLANG
	target_compile_options(AliceCommon INTERFACE -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-switch  -Wno-unused-private-field)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(AliceCommon INTERFACE
			-Wdangling-else -std=c++20
			$<$<CONFIG:Debug>:			-g -pipe>
			$<$<NOT:$<CONFIG:Debug>>: 	-O3 -pipe -fno-rtti -DNDEBUG>)
	else()
		target_compile_options(AliceCommon INTERFACE
			-Wno-unused-but-set-variable -Wno-parentheses
			$<$<CONFIG:Debug>:			-g -pipe>
			$<$<NOT:$<CONFIG:Debug>>: 	-O3 -pipe -fno-rtti -DNDEBUG>)
	endif()
endif()

target_link_libraries(AliceCommon INTERFACE dependency_DataContainer)
target_link_libraries(AliceCommon INTERFACE libglew_static)
target_link_libraries(AliceCommon INTERFACE stb_image)
target_link_libraries(AliceCommon INTERFACE freetype)
target_link_libraries(AliceCommon INTERFACE glm)
if (WIN32)
	target_link_libraries(AliceCommon INTERFACE ws2_32)
	target_link_libraries(AliceCommon INTERFACE wsock32)
else()
	target_link_libraries(AliceCommon INTERFACE dependency_tbb)
	target_link_libraries(AliceCommon INTERFACE glfw)
	target_link_libraries(AliceCommon INTERFACE miniaudio)
endif()
target_include_directories(AliceCommon INTERFACE
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/ai
	${PROJECT_SOURCE_DIR}/src/common_types
	${PROJECT_SOURCE_DIR}/src/filesystem
	${PROJECT_SOURCE_DIR}/src/gamestate
	${PROJECT_SOURCE_DIR}/src/gui
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/diplomacy_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/production_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/politics_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/military_subwindows
	${PROJECT_SOURCE_DIR}/src/graphics
	${PROJECT_SOURCE_DIR}/src/parsing
	${PROJECT_SOURCE_DIR}/src/window
	${PROJECT_SOURCE_DIR}/src/text
	${PROJECT_SOURCE_DIR}/src/sound
	${PROJECT_SOURCE_DIR}/src/map
	${PROJECT_SOURCE_DIR}/src/network
	${PROJECT_SOURCE_DIR}/src/nations
	${PROJECT_SOURCE_DIR}/src/provinces
	${PROJECT_SOURCE_DIR}/src/economy
	${PROJECT_SOURCE_DIR}/src/culture
	${PROJECT_SOURCE_DIR}/src/military
	${PROJECT_SOURCE_DIR}/src/scripting
	${PROJECT_SOURCE_DIR}/src/zstd
	"${glew_SOURCE_DIR}/include/GL"
	"ankerl")
# System headers

if(NOT WIN32)
	target_precompile_headers(AliceCommon INTERFACE [["miniaudio.h"]])
endif()

#
# AVX512
#
target_link_libraries(AliceAVX512 PRIVATE AliceCommon)
if(WIN32)
	target_compile_options(AliceAVX512 PRIVATE /arch:AVX512)
else() # GCC or CLANG
	# See for FMA support: https://stackoverflow.com/questions/21001388/fma3-in-gcc-how-to-enable
	target_compile_options(AliceAVX512 PRIVATE -mfpmath=sse -mavx512 -mfma)
endif()
#
# AVX2
#
target_link_libraries(AliceAVX2 PRIVATE AliceCommon)
if(WIN32)
	target_compile_options(AliceAVX2 PRIVATE /arch:AVX2)
else() # GCC or CLANG
	# See for FMA support: https://stackoverflow.com/questions/21001388/fma3-in-gcc-how-to-enable
	target_compile_options(AliceAVX2 PRIVATE -mfpmath=sse -mavx2 -mfma)
endif()
#
# AVX
#
target_link_libraries(AliceAVX PRIVATE AliceCommon)
if(WIN32)
	target_compile_options(AliceAVX PRIVATE /arch:AVX)
else()
	target_compile_options(AliceAVX PRIVATE -mfpmath=sse -mavx)
endif()
#
# SSE 4.2
#
target_link_libraries(AliceSSE42 PRIVATE AliceCommon)
if(WIN32)
	if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(AliceSSE42 PRIVATE /d2archSSE42)
	endif()
else()
	target_compile_options(AliceSSE42 PRIVATE -mfpmath=sse)
endif()
#
# SSE 3
#
target_link_libraries(AliceSSE3 PRIVATE AliceCommon)
if(WIN32)
	target_compile_options(AliceSSE3 PRIVATE /D__SSE3__)
else()
	target_compile_options(AliceSSE3 PRIVATE -mfpmath=sse -D__SSE3__)
endif()
#
# Incremental
#
target_link_libraries(AliceIncremental PRIVATE AliceCommon)
target_precompile_headers(AliceIncremental PRIVATE [["system_state.hpp"]])

# GENERATE CONTAINER
set(CONTAINER_PATH ${PROJECT_SOURCE_DIR}/src/gamestate/dcon_generated)

# The command to build the generated file
add_custom_command(
	OUTPUT ${CONTAINER_PATH}.hpp
	COMMAND DCONGENERATOR ${CONTAINER_PATH}.txt
	DEPENDS ${CONTAINER_PATH}.txt
	VERBATIM)

# Sets a dependency on the generated file
add_custom_target(GENERATE_CONTAINER DEPENDS ${CONTAINER_PATH}.hpp)
add_dependencies(AliceAVX512 GENERATE_CONTAINER ParserGenerator)
add_dependencies(AliceAVX2 GENERATE_CONTAINER ParserGenerator)
add_dependencies(AliceAVX GENERATE_CONTAINER ParserGenerator)
add_dependencies(AliceSSE42 GENERATE_CONTAINER ParserGenerator)
add_dependencies(AliceSSE3 GENERATE_CONTAINER ParserGenerator)
add_dependencies(AliceIncremental GENERATE_CONTAINER ParserGenerator)

# The command to build the generated parsers file
add_custom_command(
	OUTPUT ${PROJECT_SOURCE_DIR}/src/parsing/parser_defs_generated.hpp
	COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/parsing/parser_defs.txt
	DEPENDS ${PROJECT_SOURCE_DIR}/src/parsing/parser_defs.txt
	VERBATIM)

add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/src/text/bmfont_defs_generated.hpp
  COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/text/bmfont_defs.txt
  DEPENDS ${PROJECT_SOURCE_DIR}/src/text/bmfont_defs.txt
  VERBATIM)

add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/src/text/font_defs_generated.hpp
  COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/text/font_defs.txt
  DEPENDS ${PROJECT_SOURCE_DIR}/src/text/font_defs.txt
  VERBATIM)

# Sets a dependency on the generated file
add_custom_target(GENERATE_PARSERS DEPENDS
	${PROJECT_SOURCE_DIR}/src/parsing/parser_defs_generated.hpp
	${PROJECT_SOURCE_DIR}/src/text/bmfont_defs_generated.hpp
	${PROJECT_SOURCE_DIR}/src/text/font_defs_generated.hpp)
add_dependencies(AliceAVX512 GENERATE_PARSERS)
add_dependencies(AliceAVX2 GENERATE_PARSERS)
add_dependencies(AliceAVX GENERATE_PARSERS)
add_dependencies(AliceSSE42 GENERATE_PARSERS)
add_dependencies(AliceSSE3 GENERATE_PARSERS)
add_dependencies(AliceIncremental GENERATE_PARSERS)

if (BUILD_TESTING)
#    enable_testing()
#	add_subdirectory(tests)
endif()
if(WIN32)
	add_subdirectory(DbgAlice)
	add_subdirectory(Launcher)
endif()

# Installation
if(NOT WIN32)
    install(FILES ${PROJECT_SOURCE_DIR}/src/alice.desktop DESTINATION /usr/share/applications)
    install(FILES ${PROJECT_SOURCE_DIR}/src/alice.png DESTINATION /usr/share/icons/hicolor/128x128/apps)
endif()

