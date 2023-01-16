message(STATUS "Fetching DataContainer...")

FetchContent_Declare(
        DataContainer
        GIT_REPOSITORY https://github.com/schombert/DataContainer
        GIT_TAG v0.2.1
)

FetchContent_MakeAvailable(DataContainer)

add_library(dependency_DataContainer INTERFACE)
target_include_directories(dependency_DataContainer SYSTEM INTERFACE "${datacontainer_SOURCE_DIR}/CommonIncludes/")

# CONTAINER GENERATOR
file(GLOB_RECURSE GENERATOR_SOURCE "${datacontainer_SOURCE_DIR}/DataContainerGenerator/*.cpp")

add_executable(GENERATOR ${GENERATOR_SOURCE})
target_include_directories(GENERATOR PRIVATE "${datacontainer_SOURCE_DIR}/DataContainerGenerator/")