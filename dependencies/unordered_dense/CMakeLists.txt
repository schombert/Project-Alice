message(STATUS "Fetching unordered_dense ...")

FetchContent_Declare(
        unordered_dense
        GIT_REPOSITORY https://github.com/martinus/unordered_dense
        GIT_TAG v3.1.1
)

FetchContent_MakeAvailable(unordered_dense)

add_library(dependency_unordered_dense INTERFACE)
target_link_libraries(dependency_unordered_dense INTERFACE unordered_dense)
