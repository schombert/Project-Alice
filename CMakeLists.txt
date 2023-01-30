# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#

cmake_minimum_required (VERSION 3.15)

project (ProjectAlice LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

add_executable(Alice WIN32 "src/main.cpp")

# Used for storing the local path to the Victoria 2 directory
if(EXISTS ${PROJECT_SOURCE_DIR}/src/local_user_settings.hpp)
	target_compile_definitions(Alice PRIVATE LOCAL_USER_SETTINGS)
else()
	target_compile_definitions(Alice PRIVATE "GAME_DIR=\"NONE\"")
	target_compile_definitions(Alice PRIVATE "IGNORE_REAL_FILES_TESTS=1")
endif()

add_compile_definitions(VE_NO_TBB)
if(WIN32)
	# string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	# string(REPLACE "/MD" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	# string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

	set(CMAKE_CXX_FLAGS "")
	set(CMAKE_CXX_FLAGS_DEBUG "")
	set(CMAKE_CXX_FLAGS_RELEASE "")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(Alice PRIVATE
										/bigobj /wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /WX /arch:AVX2 /GF /w34388 /w34389 -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field
			$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od>
			$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline>)
		target_link_options(Alice PRIVATE
			$<$<CONFIG:Debug>: 			/DEBUG:FULL >
			$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF /LTCG>)
	else()
		target_compile_options(Alice PRIVATE
										/bigobj /wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /w34388 /w34389
			$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od>
			$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:preprocessor /Zc:inline>)
		target_link_options(Alice PRIVATE
			$<$<CONFIG:Debug>: 			/DEBUG:FULL >
			$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF /LTCG>)
	endif()
else() # GCC or CLANG
	target_compile_options(Alice PRIVATE
									# -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter
		$<$<CONFIG:Debug>:			-g>
		$<$<NOT:$<CONFIG:Debug>>: 	-O3>)
endif()

# Include sub-projects.
add_subdirectory (dependencies)
add_subdirectory (ParserGenerator)

message("leftover CMAKE_CXX_FLAGS: \"${CMAKE_CXX_FLAGS}\"")
message("leftover CMAKE_CXX_FLAGS_DEBUG: \"${CMAKE_CXX_FLAGS_DEBUG}\"")
message("leftover CMAKE_CXX_FLAGS_RELEASE: \"${CMAKE_CXX_FLAGS_RELEASE}\"")

add_compile_definitions(VE_NO_TBB)

target_link_libraries(Alice PRIVATE dependency_DataContainer)
target_link_libraries(Alice PRIVATE libglew_static)
target_link_libraries(Alice PRIVATE dependency_unordered_dense)
target_link_libraries(Alice PRIVATE stb_image)
target_link_libraries(Alice PRIVATE freetype)
if (NOT WIN32)
	target_link_libraries(Alice PRIVATE glfw)
endif()

target_include_directories(Alice PUBLIC
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/common_types
	${PROJECT_SOURCE_DIR}/src/filesystem
	${PROJECT_SOURCE_DIR}/src/gamestate
	${PROJECT_SOURCE_DIR}/src/gui
	${PROJECT_SOURCE_DIR}/src/ogl
	${PROJECT_SOURCE_DIR}/src/parsing
	${PROJECT_SOURCE_DIR}/src/window
	${PROJECT_SOURCE_DIR}/src/text
	${PROJECT_SOURCE_DIR}/src/sound
	${PROJECT_SOURCE_DIR}/src/nations
	"${glew_SOURCE_DIR}/include/GL"
	"ankerl")

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
add_dependencies(Alice GENERATE_CONTAINER ParserGenerator)

# The command to build the generated parsers file
add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/src/parsing/parser_defs_generated.hpp
  COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/parsing/parser_defs.txt
  DEPENDS ${PROJECT_SOURCE_DIR}/src/parsing/parser_defs.txt
  VERBATIM)

# Sets a dependency on the generated file
add_custom_target(GENERATE_PARSERS DEPENDS ${PROJECT_SOURCE_DIR}/src/parsing/parser_defs_generated.hpp)
add_dependencies(Alice GENERATE_PARSERS)

add_custom_command(
        TARGET Alice POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/src/ogl/ui_f_shader.txt
                ${CMAKE_CURRENT_BINARY_DIR}/ui_f_shader.txt)
add_custom_command(
        TARGET Alice POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/src/ogl/ui_v_shader.txt
                ${CMAKE_CURRENT_BINARY_DIR}/ui_v_shader.txt)
add_custom_command(
        TARGET Alice POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/src/mod/alice.gui
                ${CMAKE_CURRENT_BINARY_DIR}/alice.gui)
add_custom_command(
        TARGET Alice POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/src/mod/AndadaSC-Regular.otf
                ${CMAKE_CURRENT_BINARY_DIR}/AndadaSC-Regular.otf)
add_custom_command(
        TARGET Alice POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/src/mod/LibreCaslonText-Regular.ttf
                ${CMAKE_CURRENT_BINARY_DIR}/LibreCaslonText-Regular.ttf)
add_custom_command(
        TARGET Alice POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/src/mod/alice.csv
                ${CMAKE_CURRENT_BINARY_DIR}/alice.csv)


if (BUILD_TESTING)
    enable_testing()
	add_subdirectory(tests)
endif()

