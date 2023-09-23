message(STATUS "Fetching catch2 ...")

FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2
	GIT_TAG v2.13.10
	GIT_SHALLOW 1
	GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(Catch2)
# target_link_libraries(tests Catch2::Catch2)
