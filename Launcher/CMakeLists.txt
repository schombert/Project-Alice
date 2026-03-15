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
		"${PROJECT_SOURCE_DIR}/src/launcher/launcher_main_win.cpp"
		${ALICE_INCREMENTAL_SOURCES_LIST}
		${LAUNCHER_ASSET_FILES})
else()
	add_executable(launch_alice		
		"${PROJECT_SOURCE_DIR}/src/launcher/launcher_main_nix.cpp"
		${ALICE_INCREMENTAL_SOURCES_LIST}
		${LAUNCHER_ASSET_FILES})
endif()

target_compile_definitions(launch_alice PUBLIC ALICE_NO_ENTRY_POINT=1)
target_compile_definitions(launch_alice PUBLIC INCREMENTAL=1)

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

# System headers
target_precompile_headers(launch_alice
	PRIVATE <stddef.h>
	PRIVATE <stdint.h>
	PRIVATE <assert.h>
	PRIVATE <stdarg.h>
	PRIVATE <stdlib.h>
	PRIVATE <math.h>
	PRIVATE <signal.h>
	PRIVATE <ctype.h>
	PRIVATE <vector>
	PRIVATE <queue>		
	PRIVATE <array>
	PRIVATE <optional>
	PRIVATE <memory>
	PRIVATE <variant>
	PRIVATE <string>
	PRIVATE <string_view>
	PRIVATE <charconv>
	PRIVATE <algorithm>
	PRIVATE <iterator>
	PRIVATE <functional>
	PRIVATE <atomic>
	PRIVATE <chrono>
	PRIVATE <tuple>
	PRIVATE <type_traits>
	PRIVATE <new>
	PRIVATE <limits>
	PRIVATE <iterator>
	PRIVATE <utility>
	PRIVATE <shared_mutex>
	PRIVATE <cstdlib>
	PRIVATE <cstddef>
	PRIVATE <cstdint>
	PRIVATE <cassert>
	PRIVATE <cstdarg>
	PRIVATE <cmath>
	PRIVATE <limits>
)

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
