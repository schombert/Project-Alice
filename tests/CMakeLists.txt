#94723 <-- DO NOT REMOVE ... the file system tests are looking for this

add_executable(tests_project "${PROJECT_SOURCE_DIR}/tests/test_main.cpp")


if(WIN32)
	set(CMAKE_CXX_FLAGS "")
	set(CMAKE_CXX_FLAGS_DEBUG "")
	set(CMAKE_CXX_FLAGS_RELEASE "")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(tests_project PRIVATE
										/bigobj /wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /WX /arch:AVX2 /GF /w34388 /w34389 -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field /Z7 -Wno-invalid-offsetof -Wno-deprecated-volatile
			$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od>
			$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline>)
		target_link_options(tests_project PRIVATE
			$<$<CONFIG:Debug>: 			/DEBUG:FULL >
			$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF /LTCG>)
	else()
		target_compile_options(tests_project PRIVATE
										/bigobj /wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /w34388 /w34389 /Z7
			$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od>
			$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:preprocessor /Zc:inline>)
		target_link_options(tests_project PRIVATE
			$<$<CONFIG:Debug>: 			/DEBUG:FULL >
			$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF /LTCG>)
	endif()
else() # GCC or CLANG
	target_compile_options(tests_project PRIVATE
									# -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter
		$<$<CONFIG:Debug>:			-msse4.1 -g>
		$<$<NOT:$<CONFIG:Debug>>: 	-msse4.1 -O3>)
endif()

FetchContent_MakeAvailable(Catch2)

# Link to the desired libraries
target_link_libraries(tests_project
    PRIVATE
    Catch2::Catch2
)

# GENERATE test parsers

# The command to build the generated testparsers file
add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/tests/test_parsers_generated.hpp
  COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/tests/test_parsers.txt
  DEPENDS ${PROJECT_SOURCE_DIR}/tests/test_parsers.txt
  VERBATIM)

# Sets a dependency on the generated file
add_custom_target(GENERATE_TEST_PARSERS DEPENDS ${PROJECT_SOURCE_DIR}/tests/test_parsers_generated.hpp)
add_dependencies(tests_project GENERATE_TEST_PARSERS)

add_dependencies(tests_project GENERATE_PARSERS)
add_dependencies(tests_project GENERATE_CONTAINER ParserGenerator)

target_precompile_headers(tests_project REUSE_FROM Alice)

if(EXISTS ${PROJECT_SOURCE_DIR}/src/local_user_settings.hpp)
	target_compile_definitions(tests_project PRIVATE LOCAL_USER_SETTINGS)
else()
	target_compile_definitions(tests_project PRIVATE "GAME_DIR=\"NONE\"")
	target_compile_definitions(tests_project PRIVATE "IGNORE_REAL_FILES_TESTS=1")
endif()

target_compile_definitions(tests_project PRIVATE "PROJECT_ROOT=\"${PROJECT_SOURCE_DIR}\"")

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)


target_include_directories(tests_project PUBLIC
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/common_types
	${PROJECT_SOURCE_DIR}/src/filesystem
	${PROJECT_SOURCE_DIR}/src/gamestate
	${PROJECT_SOURCE_DIR}/src/gui
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/production_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/politics_subwindows
	${PROJECT_SOURCE_DIR}/src/ogl
	${PROJECT_SOURCE_DIR}/src/parsing
	${PROJECT_SOURCE_DIR}/src/window
	${PROJECT_SOURCE_DIR}/src/text
	${PROJECT_SOURCE_DIR}/src/text/bmfontgl
	${PROJECT_SOURCE_DIR}/src/sound
	${PROJECT_SOURCE_DIR}/src/map
	${PROJECT_SOURCE_DIR}/src/nations
	${PROJECT_SOURCE_DIR}/src/provinces
	${PROJECT_SOURCE_DIR}/src/economy
	${PROJECT_SOURCE_DIR}/src/culture
	${PROJECT_SOURCE_DIR}/src/military
	${PROJECT_SOURCE_DIR}/src/scripting
	${PROJECT_SOURCE_DIR}/src/zstd
	"${glew_SOURCE_DIR}/include/GL"
	"ankerl")

message("ZSTD_INCLUDE_DIR: \"${ZSTD_INCLUDE_DIR}\"")

target_link_libraries(tests_project PRIVATE libglew_static)
target_link_libraries(tests_project PRIVATE dependency_DataContainer)
target_link_libraries(tests_project PRIVATE stb_image)
target_link_libraries(tests_project PRIVATE freetype)
target_link_libraries(tests_project PRIVATE glm)

if (NOT WIN32)
	target_link_libraries(tests_project PRIVATE dependency_tbb)
	target_link_libraries(tests_project PRIVATE glfw)
	target_link_libraries(tests_project PRIVATE miniaudio)
endif()

include(Catch)
catch_discover_tests(tests_project)

# add_custom_command(
#    TARGET tests_project
#     COMMENT "Run tests"
#     POST_BUILD
#     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#     COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^tests_project$" --output-on-failures
# )
