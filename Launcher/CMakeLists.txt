add_executable(launch_alice WIN32
	"${PROJECT_SOURCE_DIR}/src/launcher/launcher_main.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_state.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_data_loading.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_borders.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_modes.cpp"
	"${PROJECT_SOURCE_DIR}/src/alice.rc")

target_link_libraries(launch_alice PRIVATE AliceCommon)
target_link_libraries(launch_alice PRIVATE dbghelp)
target_link_libraries(launch_alice INTERFACE libglew_static)
target_link_libraries(launch_alice INTERFACE stb_image)
target_link_libraries(launch_alice INTERFACE freetype)

target_precompile_headers(launch_alice REUSE_FROM Alice)
