#94723 <-- DO NOT REMOVE ... the file system tests are looking for this

if(WIN32)
add_executable(tests_project "${PROJECT_SOURCE_DIR}/tests/test_main.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_state.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_data_loading.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_borders.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map.cpp"
	"${PROJECT_SOURCE_DIR}/src/alice.rc")
else()
add_executable(tests_project "${PROJECT_SOURCE_DIR}/tests/test_main.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_state.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_data_loading.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_borders.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map.cpp")
endif()
target_link_libraries(tests_project PRIVATE AliceCommon)

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
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests_project)

# add_custom_command(
#    TARGET tests_project
#     COMMENT "Run tests"
#     POST_BUILD
#     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#     COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^tests_project$" --output-on-failures
# )
