message(STATUS "Fetching glm...")

find_package(ICU 61.0 COMPONENTS uc i18n REQUIRED)

add_library(dependency_icu INTERFACE)
target_link_libraries(dependency_icu ICU::uc ICU::i18n)
