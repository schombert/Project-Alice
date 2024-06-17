message(STATUS "Fetching glm...")

find_package(ICU 70.1 COMPONENTS uc i18n REQUIRED)

add_library(dependency_icu INTERFACE)
target_link_libraries(dependency_icu INTERFACE ICU::uc ICU::i18n)
