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

if(WIN32)
	# string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	# string(REPLACE "/MD" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	# string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

	set(CMAKE_CXX_FLAGS "")
	set(CMAKE_CXX_FLAGS_DEBUG "")
	set(CMAKE_CXX_FLAGS_RELEASE "")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		if(OPTIMIZE_MODE STREQUAL "On")
			message(STATUS "Optimizing with PGO data")
			target_compile_options(save_editor PRIVATE
				/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /GR- /W4 /permissive- /WX /arch:AVX2 /GF /w34388 /w34389 -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field /Z7 -Wno-invalid-offsetof -Wno-deprecated-volatile -Wno-missing-prototypes -Wno-reserved-identifier -Wno-implicit-int-float-conversion -Wno-unsafe-buffer-usage -Wno-float-equal -Wno-shadow-field-in-constructor -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-double-promotion -Wno-shadow-uncaptured-local -Wno-nonportable-system-include-path -Wno-format-nonliteral -Wno-shorten-64-to-32 -Wno-ctad-maybe-unsupported -Wno-implicit-int-conversion -Wno-disabled-macro-expansion -Wno-comma /DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline -Wno-profile-instr-missing -fprofile-instr-use=code.profdata)
			target_link_options(save_editor PRIVATE /OPT:REF /OPT:ICF /LTCG -fprofile-instr-use=code.profdata)
		elseif(PROFILE_MODE STREQUAL "On")
			message(STATUS "Compiling for PGO instrumentation")
			target_compile_options(save_editor PRIVATE
				/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /GR- /W4 /permissive- /WX /arch:AVX2 /GF /w34388 /w34389 -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field /Z7 -Wno-invalid-offsetof -Wno-deprecated-volatile -Wno-missing-prototypes -Wno-reserved-identifier -Wno-implicit-int-float-conversion -Wno-unsafe-buffer-usage -Wno-float-equal -Wno-shadow-field-in-constructor -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-double-promotion -Wno-shadow-uncaptured-local -Wno-nonportable-system-include-path -Wno-format-nonliteral -Wno-shorten-64-to-32 -Wno-ctad-maybe-unsupported -Wno-implicit-int-conversion -Wno-disabled-macro-expansion -Wno-comma /DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline -fprofile-instr-generate)
			target_link_options(save_editor PRIVATE /DEBUG:FULL /OPT:REF /OPT:ICF /LTCG -fprofile-instr-generate)
		else()
			target_compile_options(save_editor PRIVATE
											/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /GR- /W4 /permissive- /WX /arch:AVX2 /GF /w34388 /w34389 -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field /Z7 -Wno-invalid-offsetof -Wno-deprecated-volatile -Wno-missing-prototypes -Wno-reserved-identifier -Wno-implicit-int-float-conversion -Wno-unsafe-buffer-usage -Wno-float-equal -Wno-shadow-field-in-constructor -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-double-promotion -Wno-shadow-uncaptured-local -Wno-nonportable-system-include-path -Wno-format-nonliteral -Wno-shorten-64-to-32 -Wno-ctad-maybe-unsupported -Wno-implicit-int-conversion -Wno-disabled-macro-expansion -Wno-comma
				$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od>
				$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline>)
			target_link_options(save_editor PRIVATE
				$<$<CONFIG:Debug>: 			/DEBUG:FULL >
				$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF /LTCG>)
		endif()
	else()
		set_target_properties(save_editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})

		if(OPTIMIZE_MODE STREQUAL "On")
			message(STATUS "Optimizing with PGO data")
			target_compile_options(save_editor PRIVATE
				/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /wd4723 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /w34388 /w34389 /DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:preprocessor /Zc:inline)
			target_link_options(save_editor PRIVATE /OPT:REF /OPT:ICF /LTCG /USEPROFILE)
		elseif(PROFILE_MODE STREQUAL "On")
			message(STATUS "Compiling for PGO instrumentation")
			target_compile_options(save_editor PRIVATE
				/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /wd4723 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /w34388 /w34389 /Z7 /DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:preprocessor /Zc:inline)
			target_link_options(save_editor PRIVATE /DEBUG:FULL /OPT:REF /OPT:ICF /LTCG /GENPROFILE)
		else()
			message(STATUS "Normal, not PGO, build")
			target_compile_options(save_editor PRIVATE
											/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /w34388 /w34389 /Z7
				$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od>
				$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:preprocessor /Zc:inline>)
			target_link_options(save_editor PRIVATE
				$<$<CONFIG:Debug>: 			/DEBUG:FULL >
				$<$<NOT:$<CONFIG:Debug>>: 	/DEBUG:FULL /OPT:REF /OPT:ICF /LTCG>)
		endif()
	endif()
else() # GCC or CLANG
	# See for FMA support: https://stackoverflow.com/questions/21001388/fma3-in-gcc-how-to-enable
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(save_editor PRIVATE
			-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-switch -Wdangling-else -Wno-unused-private-field
			$<$<CONFIG:Debug>:			-mfpmath=sse -mavx2 -mfma -g -pipe -D_FORTIFY_SOURCE=2>
			$<$<NOT:$<CONFIG:Debug>>: 	-mfpmath=sse -mavx2 -mfma -O3 -pipe -fno-rtti -DNDEBUG>)
	else()
		target_compile_options(save_editor PRIVATE
										# -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-switch -Wno-unused-private-field -Wno-unused-but-set-variable -Wno-parentheses
			$<$<CONFIG:Debug>:			-mfpmath=sse -mavx2 -mfma -g -pipe -D_FORTIFY_SOURCE=2>
			$<$<NOT:$<CONFIG:Debug>>: 	-mfpmath=sse -mavx2 -mfma -O3 -pipe -fno-rtti -DNDEBUG>)
	endif()
endif()

add_dependencies(save_editor GENERATE_PARSERS)
add_dependencies(save_editor GENERATE_CONTAINER ParserGenerator)

target_precompile_headers(save_editor REUSE_FROM Alice)

if(EXISTS ${PROJECT_SOURCE_DIR}/src/local_user_settings.hpp)
	target_compile_definitions(save_editor PRIVATE LOCAL_USER_SETTINGS)
else()
	target_compile_definitions(save_editor PRIVATE "GAME_DIR=\"NONE\"")
endif()

target_compile_definitions(save_editor PRIVATE "PROJECT_ROOT=\"${PROJECT_SOURCE_DIR}\"")

target_include_directories(save_editor PUBLIC
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/ai
	${PROJECT_SOURCE_DIR}/src/common_types
	${PROJECT_SOURCE_DIR}/src/filesystem
	${PROJECT_SOURCE_DIR}/src/gamestate
	${PROJECT_SOURCE_DIR}/src/gui
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/diplomacy_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/production_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/politics_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/military_subwindows
	${PROJECT_SOURCE_DIR}/src/ogl
	${PROJECT_SOURCE_DIR}/src/parsing
	${PROJECT_SOURCE_DIR}/src/window
	${PROJECT_SOURCE_DIR}/src/text
	${PROJECT_SOURCE_DIR}/src/text/bmfontgl
	${PROJECT_SOURCE_DIR}/src/sound
	${PROJECT_SOURCE_DIR}/src/map
	#${PROJECT_SOURCE_DIR}/src/network
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
