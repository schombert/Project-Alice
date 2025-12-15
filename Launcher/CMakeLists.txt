# Copy everything from the assets directory into the CMAKE_CURRENT_BINARY_DIR/assets directory
file(GLOB_RECURSE TESTING ${PROJECT_SOURCE_DIR}/assets/*)
foreach(file ${TESTING})
	string(LENGTH "${PROJECT_SOURCE_DIR}/" SOURCE_DIR_LENGTH)
	string(SUBSTRING ${file} ${SOURCE_DIR_LENGTH} -1 file)
	list(APPEND LAUNCHER_ASSET_FILES "${CMAKE_CURRENT_BINARY_DIR}/${file}")

	add_custom_command(
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}
			DEPENDS ${PROJECT_SOURCE_DIR}/${file}
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
					${PROJECT_SOURCE_DIR}/${file}
					${CMAKE_CURRENT_BINARY_DIR}/${file})
endforeach()

if (WIN32)
	add_executable(launch_alice WIN32
		"${PROJECT_SOURCE_DIR}/src/gamerule/gamerule.cpp"
		"${PROJECT_SOURCE_DIR}/src/launcher/launcher_main_win.cpp"
		"${PROJECT_SOURCE_DIR}/src/gui/alice_ui.cpp"
		"${PROJECT_SOURCE_DIR}/src/map/projections.cpp"
		"${PROJECT_SOURCE_DIR}/src/map/map_state.cpp"
		"${PROJECT_SOURCE_DIR}/src/map/map_data_loading.cpp"
		"${PROJECT_SOURCE_DIR}/src/map/map_borders.cpp"
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
		"${PROJECT_SOURCE_DIR}/src/graphics/asvg.cpp"
		"${PROJECT_SOURCE_DIR}/src/gamestate/uitemplate_serialization.cpp"
		"${PROJECT_SOURCE_DIR}/src/gui/production.cpp"
		"${PROJECT_SOURCE_DIR}/src/alice.rc"
		${LAUNCHER_ASSET_FILES})
else()
	add_executable(launch_alice
		"${PROJECT_SOURCE_DIR}/src/gamerule/gamerule.cpp"
		"${PROJECT_SOURCE_DIR}/src/launcher/launcher_main_nix.cpp"
		"${PROJECT_SOURCE_DIR}/src/gui/alice_ui.cpp"
		"${PROJECT_SOURCE_DIR}/src/map/projections.cpp"
		"${PROJECT_SOURCE_DIR}/src/map/map_state.cpp"
		"${PROJECT_SOURCE_DIR}/src/map/map_data_loading.cpp"
		"${PROJECT_SOURCE_DIR}/src/map/map_borders.cpp"
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
		"${PROJECT_SOURCE_DIR}/src/graphics/asvg.cpp"
		"${PROJECT_SOURCE_DIR}/src/gamestate/uitemplate_serialization.cpp"
		"${PROJECT_SOURCE_DIR}/src/gui/production.cpp"
		${LAUNCHER_ASSET_FILES})
endif()

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

target_compile_definitions(launch_alice PUBLIC ALICE_NO_ENTRY_POINT)

if (WIN32)
	target_link_libraries(launch_alice PRIVATE ${PROJECT_SOURCE_DIR}/libs/LLVM-C.lib)
endif()

target_link_libraries(launch_alice PRIVATE AliceCommon)
if (WIN32)
	target_link_libraries(launch_alice PRIVATE dbghelp)
else()
	target_link_libraries(launch_alice PRIVATE fmt::fmt)
endif()
target_link_libraries(launch_alice INTERFACE libglew_static)
target_link_libraries(launch_alice INTERFACE stb_image)
target_link_libraries(launch_alice INTERFACE freetype)
target_link_libraries(launch_alice INTERFACE harfbuzz)

target_precompile_headers(launch_alice REUSE_FROM Alice)
