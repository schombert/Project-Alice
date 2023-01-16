message(STATUS "Fetching DataContainer...")

FetchContent_Declare(
        DataContainer
        GIT_REPOSITORY https://github.com/schombert/DataContainer
        GIT_TAG v0.2.1
)

FetchContent_MakeAvailable(DataContainer)

# SIMD LIB
add_library(DataContainer STATIC
    "${datacontainer_SOURCE_DIR}/CommonIncludes/catch.hpp"
    "${datacontainer_SOURCE_DIR}/CommonIncludes/common_types.hpp"
    "${datacontainer_SOURCE_DIR}/CommonIncludes/unordered_dense.h"
    "${datacontainer_SOURCE_DIR}/CommonIncludes/ve_avx.hpp"
    "${datacontainer_SOURCE_DIR}/CommonIncludes/ve_avx2.hpp"
    "${datacontainer_SOURCE_DIR}/CommonIncludes/ve_sse.hpp"
    "${datacontainer_SOURCE_DIR}/CommonIncludes/ve.hpp")
target_include_directories(DataContainer PUBLIC "${datacontainer_SOURCE_DIR}/CommonIncludes/")
set_target_properties(DataContainer PROPERTIES LINKER_LANGUAGE CXX)

add_library(dependency_DataContainer INTERFACE)
target_include_directories(dependency_DataContainer SYSTEM INTERFACE $<TARGET_PROPERTY:DataContainer,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(dependency_DataContainer INTERFACE DataContainer)

# CONTAINER GENERATOR
file(GLOB_RECURSE GENERATOR_SOURCE "${datacontainer_SOURCE_DIR}/DataContainerGenerator/*.cpp")

add_executable(GENERATOR ${GENERATOR_SOURCE})
target_include_directories(GENERATOR PRIVATE "${datacontainer_SOURCE_DIR}/DataContainerGenerator/")