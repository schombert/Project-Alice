if(WIN32)
add_executable(save_editor "${PROJECT_SOURCE_DIR}/SaveEditor/save_editor_main.cpp")
else()
add_executable(save_editor "${PROJECT_SOURCE_DIR}/SaveEditor/save_editor_main.cpp")
endif()

if (WIN32)
	target_link_libraries(save_editor PRIVATE ${PROJECT_SOURCE_DIR}/libs/LLVM-C.lib)
endif()

target_compile_definitions(save_editor PUBLIC ALICE_NO_ENTRY_POINT)

target_link_libraries(save_editor PRIVATE AliceCommon)
target_link_libraries(save_editor PRIVATE dbghelp)
target_link_libraries(save_editor INTERFACE libglew_static)
target_link_libraries(save_editor INTERFACE stb_image)
target_link_libraries(save_editor INTERFACE freetype)
target_link_libraries(save_editor INTERFACE harfbuzz)

target_precompile_headers(Alice
	PRIVATE [["simple_fs.hpp"]]
	PRIVATE [["constants.hpp"]]
	PRIVATE [["unordered_dense.h"]]
	PRIVATE [["dcon_generated.hpp"]]
	PRIVATE [["container_types.hpp"]]
	PRIVATE [["parsers_declarations.hpp"]]
	PRIVATE [["cyto_any.hpp"]]
	PRIVATE [["date_interface.hpp"]]
	PRIVATE [["stb_image.h"]]
)

add_dependencies(save_editor GENERATE_PARSERS)
add_dependencies(save_editor GENERATE_CONTAINER ParserGenerator)

target_precompile_headers(save_editor REUSE_FROM Alice)
