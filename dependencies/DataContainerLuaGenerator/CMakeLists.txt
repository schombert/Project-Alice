message(STATUS "Fetching DataContainerLua...")

FetchContent_Declare(
	DataContainerLua
	GIT_REPOSITORY https://github.com/ineveraskedforthis/DataContainer-Lua
	GIT_TAG allow-creation
	GIT_SHALLOW 1
	GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(DataContainerLua)

add_library(dependency_DataContainerLua INTERFACE)
