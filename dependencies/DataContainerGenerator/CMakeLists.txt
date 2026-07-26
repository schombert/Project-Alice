message(STATUS "Fetching DataContainer...")

FetchContent_Declare(
	DataContainer
	GIT_REPOSITORY https://github.com/schombert/DataContainer
	GIT_TAG 8203e5c56a1f00854c04807a15fa486e2d19a422
	GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(DataContainer)
set(DCON_COMMON_TYPES_HEADER "${datacontainer_SOURCE_DIR}/CommonIncludes/common_types.hpp")
set(DCON_VE_HEADER "${datacontainer_SOURCE_DIR}/CommonIncludes/ve.hpp")
set(DCON_VE_SCALAR_HEADER "${datacontainer_SOURCE_DIR}/CommonIncludes/ve_scalar.hpp")

configure_file(
	"${CMAKE_CURRENT_LIST_DIR}/patches/CommonIncludes/ve_scalar.hpp"
	"${DCON_VE_SCALAR_HEADER}"
	COPYONLY
)

file(READ "${DCON_COMMON_TYPES_HEADER}" DCON_COMMON_TYPES_CONTENTS)
set(DCON_COMMON_TYPES_OLD_INTRINSICS "#ifdef _MSC_VER \n#include <intrin.h>\n#else\n#include <x86intrin.h>\n#endif")
set(DCON_COMMON_TYPES_NEW_INTRINSICS "#ifdef _MSC_VER \n#include <intrin.h>\n#elif defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86)\n#include <x86intrin.h>\n#endif")
if(NOT DCON_COMMON_TYPES_CONTENTS MATCHES "#include <intrin.h>")
	message(FATAL_ERROR "DataContainer common_types.hpp format changed; update patch logic.")
endif()
string(REPLACE "${DCON_COMMON_TYPES_OLD_INTRINSICS}" "${DCON_COMMON_TYPES_NEW_INTRINSICS}" DCON_COMMON_TYPES_CONTENTS "${DCON_COMMON_TYPES_CONTENTS}")
file(WRITE "${DCON_COMMON_TYPES_HEADER}" "${DCON_COMMON_TYPES_CONTENTS}")

file(READ "${DCON_VE_HEADER}" DCON_VE_CONTENTS)
set(DCON_VE_OLD_INCLUDE "#include <immintrin.h>\n")
set(DCON_VE_NEW_INCLUDE "")
string(REPLACE "${DCON_VE_OLD_INCLUDE}" "${DCON_VE_NEW_INCLUDE}" DCON_VE_CONTENTS "${DCON_VE_CONTENTS}")
set(DCON_VE_OLD_BACKEND "#ifdef __AVX512BW__\n#include \"ve_avx512.hpp\"\n#else\n#ifdef __AVX2__\n#include \"ve_avx2.hpp\"\n#else\n#ifdef __AVX__\n#include \"ve_avx.hpp\"\n#else\n#ifdef __SSE4_2__ \n#include \"ve_sse.hpp\"\n#else // Worst case, fallback to SSE3\n#include \"ve_sse3.hpp\"\n#endif\n#endif\n#endif\n#endif")
set(DCON_VE_NEW_BACKEND "#if defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86)\n#include <immintrin.h>\n#ifdef __AVX512BW__\n#include \"ve_avx512.hpp\"\n#else\n#ifdef __AVX2__\n#include \"ve_avx2.hpp\"\n#else\n#ifdef __AVX__\n#include \"ve_avx.hpp\"\n#else\n#ifdef __SSE4_2__ \n#include \"ve_sse.hpp\"\n#else\n#include \"ve_sse3.hpp\"\n#endif\n#endif\n#endif\n#endif\n#elif defined(__aarch64__) || defined(__arm64__) || defined(_M_ARM64)\n#include \"ve_scalar.hpp\"\n#else\n#error \"DataContainer ve backend is unsupported on this architecture\"\n#endif")
if(NOT DCON_VE_CONTENTS MATCHES "ve_sse3.hpp")
	message(FATAL_ERROR "DataContainer ve.hpp format changed; update patch logic.")
endif()
string(REPLACE "${DCON_VE_OLD_BACKEND}" "${DCON_VE_NEW_BACKEND}" DCON_VE_CONTENTS "${DCON_VE_CONTENTS}")
file(WRITE "${DCON_VE_HEADER}" "${DCON_VE_CONTENTS}")

add_library(dependency_DataContainer INTERFACE)
target_include_directories(dependency_DataContainer SYSTEM INTERFACE "${datacontainer_SOURCE_DIR}/CommonIncludes/")

# CONTAINER GENERATOR
file(GLOB_RECURSE DCONGENERATOR_SOURCE "${datacontainer_SOURCE_DIR}/DataContainerGenerator/*.cpp")

add_executable(DCONGENERATOR ${DCONGENERATOR_SOURCE})
target_include_directories(DCONGENERATOR PRIVATE "${datacontainer_SOURCE_DIR}/DataContainerGenerator/")

if(WIN32)
	set(CMAKE_CXX_FLAGS "")
	set(CMAKE_CXX_FLAGS_DEBUG "")
	set(CMAKE_CXX_FLAGS_RELEASE "")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(DCONGENERATOR PRIVATE
											/bigobj /wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /WX /arch:AVX2 /GF /wd4388 /wd4389 -Wno-switch-default -Wno-unused-parameter -Wno-sign-compare -Wno-newline-eof -Wno-missing-prototypes -Wno-unsafe-buffer-usage -Wno-shadow-uncaptured-local -Wno-unreachable-code-break -Wno-padded -Wno-unique-object-duplication
				$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od>
				$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline>)
			target_link_options(DCONGENERATOR PRIVATE
				$<$<CONFIG:Debug>: 			/DEBUG:FULL >
				$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF /LTCG>)
	else()
		target_compile_options(DCONGENERATOR PRIVATE
										/bigobj /wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /wd4388 /wd4389
			$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od>
			$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:inline>)
		target_link_options(DCONGENERATOR PRIVATE
			$<$<CONFIG:Debug>: 			/DEBUG:FULL >
			$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF /LTCG>)
	endif()
else() # GCC or CLANG
	target_compile_options(DCONGENERATOR PRIVATE
									# -Wall -Wextra -Wpedantic -Werror
		$<$<CONFIG:Debug>:			-g>
		$<$<NOT:$<CONFIG:Debug>>: 	-O3>)
endif()
