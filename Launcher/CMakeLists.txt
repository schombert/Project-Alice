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

add_executable(launch_alice WIN32
	"${PROJECT_SOURCE_DIR}/src/launcher/launcher_main.cpp"
	"${PROJECT_SOURCE_DIR}/src/ai/ai.cpp"
	"${PROJECT_SOURCE_DIR}/src/common_types/blake2.cpp"
	"${PROJECT_SOURCE_DIR}/src/common_types/date_interface.cpp"
	"${PROJECT_SOURCE_DIR}/src/common_types/prng.cpp"
	"${PROJECT_SOURCE_DIR}/src/culture/culture.cpp"
	"${PROJECT_SOURCE_DIR}/src/culture/politics.cpp"
	"${PROJECT_SOURCE_DIR}/src/culture/rebels.cpp"
	"${PROJECT_SOURCE_DIR}/src/economy/demographics.cpp"
	"${PROJECT_SOURCE_DIR}/src/economy/economy.cpp"
	"${PROJECT_SOURCE_DIR}/src/gamestate/commands.cpp"
	"${PROJECT_SOURCE_DIR}/src/gamestate/diplomatic_messages.cpp"
	"${PROJECT_SOURCE_DIR}/src/gamestate/modifiers.cpp"
	"${PROJECT_SOURCE_DIR}/src/gamestate/notifications.cpp"
	"${PROJECT_SOURCE_DIR}/src/gamestate/serialization.cpp"
	"${PROJECT_SOURCE_DIR}/src/graphics/opengl_wrapper.cpp"
	"${PROJECT_SOURCE_DIR}/src/graphics/texture.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/gui_common_elements.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/gui_effect_tooltips.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/gui_graphics.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/gui_leader_select.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/gui_leader_tooltip.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/gui_modifier_tooltips.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/gui_province_window.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/gui_trigger_tooltips.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/map_tooltip.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/gui_population_window.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/gui_production_window.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/gui_technology_window.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/gui_budget_window.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/unit_tooltip.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_borders.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_data_loading.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_modes.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_state.cpp"
	"${PROJECT_SOURCE_DIR}/src/military/military.cpp"
	"${PROJECT_SOURCE_DIR}/src/nations/nations.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/network.cpp"
	"${PROJECT_SOURCE_DIR}/src/parsing/float_from_chars.cpp"
	"${PROJECT_SOURCE_DIR}/src/parsing/parsers.cpp"
	"${PROJECT_SOURCE_DIR}/src/platform_specific.cpp"
	"${PROJECT_SOURCE_DIR}/src/provinces/province.cpp"
	"${PROJECT_SOURCE_DIR}/src/scripting/effects.cpp"
	"${PROJECT_SOURCE_DIR}/src/scripting/events.cpp"
	"${PROJECT_SOURCE_DIR}/src/scripting/triggers.cpp"
	"${PROJECT_SOURCE_DIR}/src/text/bmfont.cpp"
	"${PROJECT_SOURCE_DIR}/src/text/fonts.cpp"
	"${PROJECT_SOURCE_DIR}/src/text/text.cpp"
	"${PROJECT_SOURCE_DIR}/src/zstd/zstd.cpp"
	${LAUNCHER_ASSET_FILES})

target_link_libraries(launch_alice PRIVATE AliceCommon)
target_link_libraries(launch_alice PRIVATE dbghelp)
target_link_libraries(launch_alice INTERFACE libglew_static)
target_link_libraries(launch_alice INTERFACE stb_image)
target_link_libraries(launch_alice INTERFACE freetype)

target_precompile_headers(launch_alice REUSE_FROM Alice)
