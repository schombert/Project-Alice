# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.15)
project (ProjectAlice LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
file(GLOB_RECURSE MAIN_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(ProjectAlice "${MAIN_SOURCES}")

add_compile_definitions(VE_NO_TBB)
if(MSVC)
	target_compile_options(ProjectAlice PRIVATE 
									-/wd4100 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF
		$<$<CONFIG:Debug>:			/EHsc /MTd /Od>)
	target_link_options(ProjectAlice PRIVATE
									/SUBSYSTEM:CONSOLE
		$<$<CONFIG:Debug>: 			/DEBUG:FULL > 
		$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF>)

	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		# Apply this all targets and subdirectories
		add_compile_options(/wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:inline)
	endif()

	string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	string(REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	string(REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	string(REPLACE "/MD" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
else() # GCC or CLANG
	target_compile_options(ProjectAlice PRIVATE
									-Wall -Wextra -Wpedantic -Werror
		$<$<CONFIG:Debug>:			-g>)

	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		# Apply this all targets and subdirectories
		add_compile_options(-O3)
	endif()
endif()

# Include sub-projects.
add_subdirectory (dependencies)

target_link_libraries(ProjectAlice PRIVATE
	dependency_DataContainer)

# GENERATE CONTAINER
set(CONTAINER_PATH ${PROJECT_SOURCE_DIR}/src/test)

# The command to build the generated file
add_custom_command(
  OUTPUT ${CONTAINER_PATH}.hpp
  COMMAND GENERATOR ${CONTAINER_PATH}.txt
  DEPENDS ${CONTAINER_PATH}.txt
  VERBATIM)

# Sets a dependency on the generated file 
add_custom_target(GENERATE_CONTAINER DEPENDS ${CONTAINER_PATH}.hpp)
add_dependencies(ProjectAlice GENERATE_CONTAINER)