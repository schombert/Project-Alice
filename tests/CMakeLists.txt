cmake_minimum_required(VERSION 3.13)

project (tests_project LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/tests/cmake")

set (WILDCARD_SOURCE *.cpp)

file(GLOB_RECURSE TEST_SOURCES ${WILDCARD_SOURCE})

add_executable(tests_project ${TEST_SOURCES})

target_compile_options(tests_project PRIVATE
	$<$<CXX_COMPILER_ID:GNU>:     -Wall -Wextra   -Wpedantic      -Werror                     $<$<CONFIG:Debug>:-g>   $<$<NOT:$<CONFIG:Debug>>:-O3>>
	$<$<CXX_COMPILER_ID:Clang>:   -Wall -Wextra   -Wpedantic      -Werror                     $<$<CONFIG:Debug>:-g>   $<$<NOT:$<CONFIG:Debug>>:-O3>>
	$<$<CXX_COMPILER_ID:MSVC>: /wd4100 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /RTC1 $<$<CONFIG:Debug>:/EHsc /MTd /Od>  $<$<NOT:$<CONFIG:Debug>>:/wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:inline>>)
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

message(${catch2_SOURCE_DIR})

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)
catch_discover_tests(tests_project)

