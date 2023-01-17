message(STATUS "Fetching DataContainer...")

FetchContent_Declare(
        DataContainer
        GIT_REPOSITORY https://github.com/schombert/DataContainer
		GIT_TAG 4fc506ad182187e4590352c5e797aee82757e467
        # GIT_TAG v0.2.1
)

FetchContent_MakeAvailable(DataContainer)

add_library(dependency_DataContainer INTERFACE)
target_include_directories(dependency_DataContainer SYSTEM INTERFACE "${datacontainer_SOURCE_DIR}/CommonIncludes/")

# CONTAINER GENERATOR
file(GLOB_RECURSE DCONGENERATOR_SOURCE "${datacontainer_SOURCE_DIR}/DataContainerGenerator/*.cpp")

add_executable(DCONGENERATOR ${DCONGENERATOR_SOURCE})
target_include_directories(DCONGENERATOR PRIVATE "${datacontainer_SOURCE_DIR}/DataContainerGenerator/")

if(MSVC)
	target_compile_options(DCONGENERATOR PRIVATE 
										/wd4100 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF
			$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od>
			$<$<NOT:$<CONFIG:Debug>>: 	/wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:inline>)
	target_link_options(DCONGENERATOR PRIVATE
			$<$<CONFIG:Debug>: 			/DEBUG:FULL > 
			$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF>)
else() # GCC or CLANG
	target_compile_options(ProjectAlice PRIVATE
									-Wall -Wextra -Wpedantic -Werror
		$<$<CONFIG:Debug>:			-g>
		$<$<NOT:$<CONFIG:Debug>>: 	-O3>)
endif()
