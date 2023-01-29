set(PROJECT_SRC "parser_generator.cpp")

project (ParserGenerator LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ParserGenerator ${PROJECT_SRC})


if(WIN32)
	# removed /Zc:preprocessor for clang
	set(CMAKE_CXX_FLAGS "")
	set(CMAKE_CXX_FLAGS_DEBUG "")
	set(CMAKE_CXX_FLAGS_RELEASE "")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

		target_compile_options(ParserGenerator PRIVATE
										/bigobj /wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /WX /arch:AVX2 /GF /w34388 /w34389
			$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od /RTC1>
			$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline>)
		target_link_options(ParserGenerator PRIVATE
			$<$<CONFIG:Debug>: 			/DEBUG:FULL >
			$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF /LTCG>)
	else()
		target_compile_options(ParserGenerator PRIVATE
										/bigobj /wd4100 /wd4189 /wd4065 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /w34388 /w34389
			$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od /RTC1>
			$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /Zc:preprocessor /GS- /Gy /Gw /Zc:inline>)
		target_link_options(ParserGenerator PRIVATE
			$<$<CONFIG:Debug>: 			/DEBUG:FULL >
			$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF /LTCG>)
	endif()
else() # GCC or CLANG
	target_compile_options(ParserGenerator PRIVATE
		$<$<CXX_COMPILER_ID:GNU>:     -Wall -Wextra   -Wpedantic      -Werror                     $<$<CONFIG:Debug>:-g>   $<$<NOT:$<CONFIG:Debug>>:-O3>>
		$<$<CXX_COMPILER_ID:Clang>:   -Wall -Wextra   -Wpedantic      -Werror                     $<$<CONFIG:Debug>:-g>   $<$<NOT:$<CONFIG:Debug>>:-O3>>)
	target_link_options(ParserGenerator PRIVATE
		$<$<CXX_COMPILER_ID:GNU>:   $<$<CONFIG:Debug>: >   $<$<NOT:$<CONFIG:Debug>>: >>
		$<$<CXX_COMPILER_ID:Clang>:  $<$<CONFIG:Debug>: >   $<$<NOT:$<CONFIG:Debug>>: >>)
endif()


