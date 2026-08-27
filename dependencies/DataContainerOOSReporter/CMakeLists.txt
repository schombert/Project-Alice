message(STATUS "Fetching DataContainerOOSReporter...")

FetchContent_Declare(
	DataContainerOOSReporter
	GIT_REPOSITORY https://github.com/ZombieFreak115/DataContainer_OOS_Reporter
	GIT_TAG master
	GIT_SHALLOW 1
	GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(DataContainerOOSReporter)

add_library(dependency_DataContainerOOSReporter INTERFACE)
