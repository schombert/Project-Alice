message(STATUS "Fetching DataContainer...")

FetchContent_Declare(
        DataContainer
        GIT_REPOSITORY https://github.com/schombert/DataContainer
		GIT_TAG 5d2e6b7
        # GIT_TAG v0.2.2
)

FetchContent_MakeAvailable(DataContainer)

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
											/bigobj /wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /WX /arch:AVX2 /GF /wd4388 /wd4389 -Wno-unused-parameter -Wno-sign-compare
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
