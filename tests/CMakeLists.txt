#94723 <-- DO NOT REMOVE ... the file system tests are looking for this

add_executable(tests_project "${PROJECT_SOURCE_DIR}/tests/test_main.cpp" "${PROJECT_SOURCE_DIR}/src/float_from_chars.cpp" "${PROJECT_SOURCE_DIR}/tests/parsers_tests.cpp" "file_system_tests.cpp")

target_compile_options(tests_project PRIVATE
	$<$<CXX_COMPILER_ID:GNU>:     -Wall -Wextra   -Wpedantic      -Werror                     $<$<CONFIG:Debug>:-g>   $<$<NOT:$<CONFIG:Debug>>:-O3>>
	$<$<CXX_COMPILER_ID:Clang>:   -Wall -Wextra   -Wpedantic      -Werror                     $<$<CONFIG:Debug>:-g>   $<$<NOT:$<CONFIG:Debug>>:-O3>>
	$<$<CXX_COMPILER_ID:MSVC>: /wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /RTC1 /w34388 /w34389 $<$<CONFIG:Debug>:/EHsc /MTd /Od>  $<$<NOT:$<CONFIG:Debug>>:/wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:inline>>)
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

# The command to build the generated testparsers file
add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/tests/test_parsers_generated.hpp
  COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/tests/test_parsers.txt
  DEPENDS ${PROJECT_SOURCE_DIR}/tests/test_parsers.txt
  VERBATIM)

# Sets a dependency on the generated file 
add_custom_target(GENERATE_TEST_PARSERS DEPENDS ${PROJECT_SOURCE_DIR}/tests/test_parsers_generated.hpp)
add_dependencies(tests_project GENERATE_TEST_PARSERS)

# GENERATE data container from the main project ... we have this as a transitive dependency
set(CONTAINER_PATH ${PROJECT_SOURCE_DIR}/src/dcon_generated)

# Sets a dependency on the generated file 
add_custom_target(GENERATE_CONTAINER_TESTS DEPENDS ${CONTAINER_PATH}.hpp)
add_dependencies(tests_project GENERATE_CONTAINER_TESTS DCONGENERATOR)

target_compile_definitions(tests_project PRIVATE "PROJECT_ROOT=\"${PROJECT_SOURCE_DIR}\"")

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

target_include_directories(tests_project PUBLIC ${PROJECT_SOURCE_DIR}/src "${glew_SOURCE_DIR}/include/GL")
target_link_libraries(tests_project PRIVATE libglew_static)
target_link_libraries(tests_project PRIVATE dependency_DataContainer)

include(Catch)
catch_discover_tests(tests_project)

# add_custom_command(
#    TARGET tests_project
#     COMMENT "Run tests"
#     POST_BUILD 
#     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#     COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^tests_project$" --output-on-failures
#)
