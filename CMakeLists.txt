# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#

cmake_minimum_required (VERSION 3.15)

project (ProjectAlice LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

add_executable(Alice WIN32 "src/main.cpp")

set(PRIMARY_PROJECT_DIRECTORY "${PROJECT_SOURCE_DIR}/src")

add_compile_definitions(VE_NO_TBB)
if(MSVC)
	string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	string(REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	string(REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	string(REPLACE "/MD" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

	target_compile_options(Alice PRIVATE 
									/wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /w34388 /w34389
		$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od /RTC1>
		$<$<NOT:$<CONFIG:Debug>>: 	/wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:inline>)
	target_link_options(Alice PRIVATE
		$<$<CONFIG:Debug>: 			/DEBUG:FULL > 
		$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF>)

else() # GCC or CLANG
	target_compile_options(Alice PRIVATE
									-Wall -Wextra -Wpedantic -Werror -Wno-unknown-pragmas -Wno-unused-parameter
		$<$<CONFIG:Debug>:			-g>
		$<$<NOT:$<CONFIG:Debug>>: 	-O3>)
endif()

# Include sub-projects.
add_subdirectory (dependencies)
add_subdirectory (ParserGenerator)

target_link_libraries(Alice PRIVATE
	dependency_DataContainer)
target_link_libraries(Alice PRIVATE libglew_static)

target_include_directories(Alice PUBLIC ${PROJECT_SOURCE_DIR}/src "${glew_SOURCE_DIR}/include/GL")

# GENERATE CONTAINER
set(CONTAINER_PATH ${PROJECT_SOURCE_DIR}/src/dcon_generated)

# The command to build the generated file
add_custom_command(
  OUTPUT ${CONTAINER_PATH}.hpp
  COMMAND DCONGENERATOR ${CONTAINER_PATH}.txt
  DEPENDS ${CONTAINER_PATH}.txt
  VERBATIM)

# Sets a dependency on the generated file 
add_custom_target(GENERATE_CONTAINER DEPENDS ${CONTAINER_PATH}.hpp)
add_dependencies(Alice GENERATE_CONTAINER ParserGenerator)

set(GAME_FILES_DIRECTORY "" CACHE PATH "location of game files (i.e. the folder containing the exe)")

message("Will attempt to find game files in: \"${GAME_FILES_DIRECTORY}\"")

if (BUILD_TESTING)
    enable_testing()
	add_subdirectory(tests)
endif()

