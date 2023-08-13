if(WIN32)
add_executable(save_editor "${PROJECT_SOURCE_DIR}/SaveEditor/save_editor_main.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_state.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_data_loading.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_borders.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map.cpp"
	"${PROJECT_SOURCE_DIR}/src/alice.rc")
else()
add_executable(save_editor "${PROJECT_SOURCE_DIR}/SaveEditor/save_editor_main.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_state.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_data_loading.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map_borders.cpp"
	"${PROJECT_SOURCE_DIR}/src/map/map.cpp")
endif()

target_link_libraries(save_editor PRIVATE AliceCommon)

add_dependencies(save_editor GENERATE_PARSERS)
add_dependencies(save_editor GENERATE_CONTAINER ParserGenerator)

target_precompile_headers(save_editor REUSE_FROM Alice)

target_link_libraries(save_editor PRIVATE libglew_static)
target_link_libraries(save_editor PRIVATE dependency_DataContainer)
target_link_libraries(save_editor PRIVATE stb_image)
target_link_libraries(save_editor PRIVATE freetype)
target_link_libraries(save_editor PRIVATE glm)
if (WIN32)
	#target_link_libraries(save_editor PRIVATE ws2_32 wsock32)
else()
	target_link_libraries(save_editor PRIVATE dependency_tbb)
	target_link_libraries(save_editor PRIVATE glfw)
	target_link_libraries(save_editor PRIVATE miniaudio)
endif()
