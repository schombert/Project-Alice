set(PROJECT_SRC "parser_generator.cpp")

add_executable(ParserGenerator ${PROJECT_SRC})

target_compile_features(ParserGenerator PRIVATE cxx_std_20 c_std_11)
 
target_compile_options(ParserGenerator PRIVATE
	$<$<CXX_COMPILER_ID:GNU>:     -Wall -Wextra   -Wpedantic      -Werror                     $<$<CONFIG:Debug>:-g>   $<$<NOT:$<CONFIG:Debug>>:-O3>>
	$<$<CXX_COMPILER_ID:Clang>:   -Wall -Wextra   -Wpedantic      -Werror                     $<$<CONFIG:Debug>:-g>   $<$<NOT:$<CONFIG:Debug>>:-O3>>
	$<$<CXX_COMPILER_ID:MSVC>: /wd4100 /GR- /W4 /permissive- /Zc:preprocessor /WX /arch:AVX2 /GF /RTC1 $<$<CONFIG:Debug>:/EHsc /MTd /Od>  $<$<NOT:$<CONFIG:Debug>>:/wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:inline>>)
target_link_options(ParserGenerator PRIVATE
	$<$<CXX_COMPILER_ID:GNU>:   $<$<CONFIG:Debug>: >   $<$<NOT:$<CONFIG:Debug>>: >>
	$<$<CXX_COMPILER_ID:Clang>:  $<$<CONFIG:Debug>: >   $<$<NOT:$<CONFIG:Debug>>: >>
	$<$<CXX_COMPILER_ID:MSVC>: /SUBSYSTEM:CONSOLE $<$<CONFIG:Debug>: /DEBUG:FULL >  $<$<NOT:$<CONFIG:Debug>>: /OPT:REF /OPT:ICF>>)