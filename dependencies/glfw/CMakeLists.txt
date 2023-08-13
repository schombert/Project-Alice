message(STATUS "Fetching GLFW...")

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.3.8
    GIT_SHALLOW 1
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(glfw)

add_library(dependency_GLFW INTERFACE)
target_include_directories(dependency_GLFW SYSTEM INTERFACE "${glfw_SOURCE_DIR}/include")
target_link_libraries(dependency_GLFW INTERFACE glfw)