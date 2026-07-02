message(STATUS "Fetching icu...")

add_library(dependency_icu INTERFACE)

if(APPLE)
	find_library(ICUCORE_LIBRARY NAMES icucore REQUIRED)
	find_path(ICU_INCLUDE_DIR unicode/ubrk.h)

	if(NOT ICU_INCLUDE_DIR OR NOT EXISTS "${ICU_INCLUDE_DIR}/unicode/ubrk.h" OR NOT EXISTS "${ICU_INCLUDE_DIR}/unicode/ubidi.h")
		FetchContent_Declare(icu_headers
			URL https://github.com/unicode-org/icu/releases/download/release-76-1/icu4c-76_1-src.tgz
			DOWNLOAD_EXTRACT_TIMESTAMP FALSE
		)
		FetchContent_GetProperties(icu_headers)
		if(NOT icu_headers_POPULATED)
			FetchContent_Populate(icu_headers)
		endif()
		set(ICU_INCLUDE_DIR "${icu_headers_SOURCE_DIR}/source/common")
		set(ICU_INCLUDE_DIR "${ICU_INCLUDE_DIR}" CACHE PATH "ICU header include directory" FORCE)
	endif()

	target_link_libraries(dependency_icu INTERFACE ${ICUCORE_LIBRARY})
	target_include_directories(dependency_icu SYSTEM INTERFACE ${ICU_INCLUDE_DIR})
	target_compile_definitions(dependency_icu INTERFACE U_DISABLE_RENAMING=1)
else()
	find_package(ICU 70.1 COMPONENTS uc i18n REQUIRED)
	target_link_libraries(dependency_icu INTERFACE ICU::uc ICU::i18n)
endif()
