if(NOT APPLE)
	return()
endif()

if(NOT DEFINED LUAJIT_INSTALL_DIR)
	message(FATAL_ERROR "LUAJIT_INSTALL_DIR must be set")
endif()

set(LUAJIT_LIB_DIR "${LUAJIT_INSTALL_DIR}/lib")
file(GLOB LUAJIT_DYLIBS "${LUAJIT_LIB_DIR}/libluajit-5.1*.dylib")

foreach(LUAJIT_DYLIB IN LISTS LUAJIT_DYLIBS)
	if(IS_SYMLINK "${LUAJIT_DYLIB}")
		continue()
	endif()

	execute_process(
		COMMAND install_name_tool -id @rpath/libluajit-5.1.dylib "${LUAJIT_DYLIB}"
		RESULT_VARIABLE LUAJIT_INSTALL_NAME_RESULT
	)

	if(NOT LUAJIT_INSTALL_NAME_RESULT EQUAL 0)
		message(FATAL_ERROR "install_name_tool failed for ${LUAJIT_DYLIB}")
	endif()
endforeach()
