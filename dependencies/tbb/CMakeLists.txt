message(STATUS "Fetching TBB...")
IF(WIN32)
    FetchContent_Declare(hwloc URL "https://download.open-mpi.org/release/hwloc/v2.7/hwloc-win64-build-2.7.1.zip")
    FetchContent_MakeAvailable(hwloc)
    set(CMAKE_HWLOC_2_5_DLL_PATH "${hwloc_SOURCE_DIR}/lib/libhwloc.dll")
    set(CMAKE_HWLOC_2_5_LIBRARY_PATH "${hwloc_SOURCE_DIR}/lib/libhwloc.lib" )
    set(CMAKE_HWLOC_2_5_INCLUDE_PATH "${hwloc_SOURCE_DIR}/include" )
ENDIF()

set(TBB_DISABLE_HWLOC_AUTOMATIC_SEARCH ON)
set(TBB_TEST OFF)
set(TBB_EXAMPLES OFF)
set(TBB_BENCH OFF)
set(TBB_ENABLE_IPO ON)
set(TBB_CPF OFF)
set(TBBMALLOC_BUILD ON)
set(TBBMALLOC_PROXY_BUILD ON)
set(TBB4PY_BUILD OFF)
set(TBB_SIGNTOOL OFF)
set(TBB_DYNAMIC_LOAD_ENABLED OFF)
set(TBB_ENABLE_IPO OFF)
set(TBB_STRICT OFF)

FetchContent_Declare(tbb 
    GIT_REPOSITORY "https://github.com/oneapi-src/oneTBB" 
    GIT_TAG v2021.8.0
)
FetchContent_MakeAvailable(tbb)

add_library(dependency_tbb INTERFACE)
# target_include_directories(dependency_tbb SYSTEM INTERFACE "${tbb_SOURCE_DIR}/include/oneapi/tbb")
target_link_libraries(dependency_tbb INTERFACE tbb)

if(WIN32)
	set(CMAKE_CXX_FLAGS "")
	set(CMAKE_CXX_FLAGS_DEBUG "")
	set(CMAKE_CXX_FLAGS_RELEASE "")

	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			set_target_properties(tbb PROPERTIES COMPILE_FLAGS "/MTd /wd4100 /wd4189 /wd4065 /GR- /W3 /permissive- /GF /w34388 /w34389 /RTC1 /EHsc /Od /RTC1")
		else()
			set_target_properties(tbb PROPERTIES COMPILE_FLAGS "/MTd /wd4100 /wd4189 /wd4065 /GR- /W3 /permissive- /Zc:preprocessor /GF /w34388 /w34389 /RTC1 /EHsc /Od /RTC1")
		endif()
	else()
		if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			set_target_properties(tbb PROPERTIES COMPILE_FLAGS "/MT /wd4100 /wd4189 /wd4065 /GR- /W3 /permissive- /GF /w34388 /w34389 /wd4530 /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline")
		else()
			set_target_properties(tbb PROPERTIES COMPILE_FLAGS "/MT /wd4100 /wd4189 /wd4065 /GR- /W3 /permissive- /Zc:preprocessor /GF /w34388 /w34389 /wd4530 /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:inline")
		endif()
	endif()
else() # GCC or CLANG
	target_compile_options(tbb PRIVATE
		$<$<CONFIG:Debug>:			-fexceptions>
		$<$<NOT:$<CONFIG:Debug>>: 	-fexceptions>)
endif()
