#94723 <-- DO NOT REMOVE ... the file system tests are looking for this

add_executable(tests_project "${PROJECT_SOURCE_DIR}/tests/test_main.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/alice_ui.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_state.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_data_loading.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_borders.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/lunasvg.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/projections.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map.cpp"
	"${PROJECT_SOURCE_DIR}/src/graphics/xac.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/graphics.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/lunasvg.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/svgelement.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/svggeometryelement.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/svglayoutstate.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/svgpaintelement.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/svgparser.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/svgproperty.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/svgrenderstate.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/svgtextelement.cpp"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-blend.c"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-canvas.c"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-font.c"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-ft-math.c"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-ft-raster.c"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-ft-stroker.c"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-matrix.c"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-paint.c"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-path.c"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-rasterize.c"
	"${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-surface.c"
	"${PROJECT_SOURCE_DIR}/src/alice.rc")


set_source_files_properties(${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-blend.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-canvas.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-font.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-ft-math.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-ft-raster.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-ft-stroker.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-matrix.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-paint.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-path.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-rasterize.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/lunasvg/plutovg-surface.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)


target_link_libraries(tests_project PRIVATE AliceCommon)

FetchContent_MakeAvailable(Catch2)

# Link to the desired libraries
target_link_libraries(tests_project
    PRIVATE
    Catch2::Catch2
)

if (WIN32)
	target_link_libraries(tests_project PRIVATE ${PROJECT_SOURCE_DIR}/libs/LLVM-C.lib)
else()
	target_link_libraries(tests_project PRIVATE fmt::fmt)
endif()

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
