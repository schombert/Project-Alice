cmake_minimum_required(VERSION 3.13)

project (tests_project LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

if (BUILD_TESTING)
    enable_testing()
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/tests/cmake")

set (WILDCARD_SOURCE *.cpp)

file(GLOB_RECURSE TEST_SOURCES ${WILDCARD_SOURCE})

source_group("generation files" FILES "test_parsers.txt" "test_parsers_generated.hpp")

add_executable(tests_project ${TEST_SOURCES} "${PRIMARY_PROJECT_DIRECTORY}/parsers.cpp" "${PRIMARY_PROJECT_DIRECTORY}/float_from_chars.cpp" "parsers_tests.cpp")

target_compile_options(tests_project PRIVATE
	$<$<CXX_COMPILER_ID:GNU>:     -Wall -Wextra   -Wpedantic      -Werror                     $<$<CONFIG:Debug>:-g>   $<$<NOT:$<CONFIG:Debug>>:-O3>>
	$<$<CXX_COMPILER_ID:Clang>:   -Wall -Wextra   -Wpedantic      -Werror                     $<$<CONFIG:Debug>:-g>   $<$<NOT:$<CONFIG:Debug>>:-O3>>
	$<$<CXX_COMPILER_ID:MSVC>: /wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /RTC1 $<$<CONFIG:Debug>:/EHsc /MTd /Od>  $<$<NOT:$<CONFIG:Debug>>:/wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:inline>>)
target_link_options(tests_project PRIVATE
	$<$<CXX_COMPILER_ID:GNU>:   $<$<CONFIG:Debug>: >   $<$<NOT:$<CONFIG:Debug>>: >>
	$<$<CXX_COMPILER_ID:Clang>:  $<$<CONFIG:Debug>: >   $<$<NOT:$<CONFIG:Debug>>: >>
	$<$<CXX_COMPILER_ID:MSVC>: /SUBSYSTEM:CONSOLE $<$<CONFIG:Debug>: /DEBUG:FULL >  $<$<NOT:$<CONFIG:Debug>>: /OPT:REF /OPT:ICF>>)

FetchContent_MakeAvailable(Catch2)

# Link to the desired libraries
target_link_libraries(tests_project
    PRIVATE
    Catch2::Catch2
)

# GENERATE test parsers
set(CONTAINER_PATH ${PROJECT_SOURCE_DIR}/test_parsers)

# The command to build the generated testparsers file
add_custom_command(
  OUTPUT ${CONTAINER_PATH}_generated.hpp
  COMMAND ParserGenerator ${CONTAINER_PATH}.txt
  DEPENDS ${CONTAINER_PATH}.txt
  VERBATIM)

# Sets a dependency on the generated file 
add_custom_target(GENERATE_TEST_PARSERS DEPENDS ${CONTAINER_PATH}_generated.hpp)
add_dependencies(tests_project GENERATE_TEST_PARSERS)
target_include_directories(tests_project PUBLIC ${PRIMARY_PROJECT_DIRECTORY})

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

include(Catch)
catch_discover_tests(tests_project)

# add_custom_command(
#    TARGET tests_project
#     COMMENT "Run tests"
#     POST_BUILD 
#     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#     COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^tests_project$" --output-on-failures
#)
