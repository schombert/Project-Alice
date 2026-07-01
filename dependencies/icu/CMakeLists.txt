message(STATUS "Fetching icu...")

add_library(dependency_icu INTERFACE)

if(APPLE)
	find_library(ICUCORE_LIBRARY NAMES icucore REQUIRED)
	find_path(ICU_INCLUDE_DIR unicode/utypes.h)

	target_link_libraries(dependency_icu INTERFACE ${ICUCORE_LIBRARY})
	if(ICU_INCLUDE_DIR)
		target_include_directories(dependency_icu SYSTEM INTERFACE ${ICU_INCLUDE_DIR})
	endif()
else()
	find_package(ICU 70.1 COMPONENTS uc i18n REQUIRED)
	target_link_libraries(dependency_icu INTERFACE ICU::uc ICU::i18n)
endif()
