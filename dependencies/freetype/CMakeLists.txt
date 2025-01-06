
message(STATUS "Fetching freetype...")

# Disable unwanted Freetype features
set(FT_DISABLE_ZLIB ON)
set(FT_DISABLE_BZIP2 ON)
set(FT_DISABLE_PNG ON)
set(FT_DISABLE_BROTLI ON)
set(FT_DISABLE_HARFBUZZ OFF)
set(SKIP_INSTALL_ALL ON)

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG        VER-2-13-2
    GIT_SHALLOW    1
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(freetype)

if(WIN32)
    # Clear out potential user overrides
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")

    # Shared base flags for freetype on Windows
    set(BASE_MSVC_FLAGS
        "/wd4100 /wd4189 /wd4065 /GR- /W3 /permissive- /GF /w34388 /w34389"
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Additional debug flags
        set(DEBUG_FLAGS "/MTd /RTC1 /EHsc /Od /RTC1")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set_target_properties(freetype PROPERTIES
                COMPILE_FLAGS "${BASE_MSVC_FLAGS} ${DEBUG_FLAGS}"
            )
        else()
            set_target_properties(freetype PROPERTIES
                COMPILE_FLAGS "${BASE_MSVC_FLAGS} /Zc:preprocessor ${DEBUG_FLAGS}"
            )
        endif()
    else()
        # Additional release flags
        set(RELEASE_FLAGS "/MT /wd4530 /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set_target_properties(freetype PROPERTIES
                COMPILE_FLAGS "${BASE_MSVC_FLAGS} ${RELEASE_FLAGS}"
            )
        else()
            set_target_properties(freetype PROPERTIES
                COMPILE_FLAGS "${BASE_MSVC_FLAGS} /Zc:preprocessor /GL ${RELEASE_FLAGS}"
            )
        endif()
    endif()
endif()

message(STATUS "Fetching harfbuzz...")
set(HB_BUILD_UTILS OFF)
set(HB_BUILD_SUBSET OFF)
set(HB_HAVE_FREETYPE ON)

FetchContent_Declare(
    harfbuzz
    GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz
    GIT_TAG        8.5.0
    GIT_SHALLOW    1
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(harfbuzz)

if(WIN32)
    # Reset flags again for harfbuzz
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")

    # Shared base flags for harfbuzz on Windows
    set(BASE_MSVC_FLAGS
        "/wd4100 /wd4189 /wd4065 /GR- /W3 /permissive- /GF /w34388 /w34389"
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(DEBUG_FLAGS "/MTd /RTC1 /EHsc /Od /RTC1")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set_target_properties(harfbuzz PROPERTIES
                COMPILE_FLAGS "${BASE_MSVC_FLAGS} ${DEBUG_FLAGS}"
            )
        else()
            set_target_properties(harfbuzz PROPERTIES
                COMPILE_FLAGS "${BASE_MSVC_FLAGS} /Zc:preprocessor ${DEBUG_FLAGS}"
            )
        endif()
    else()
        set(RELEASE_FLAGS "/MT /wd4530 /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set_target_properties(harfbuzz PROPERTIES
                COMPILE_FLAGS "${BASE_MSVC_FLAGS} ${RELEASE_FLAGS}"
            )
        else()
            set_target_properties(harfbuzz PROPERTIES
                COMPILE_FLAGS "${BASE_MSVC_FLAGS} /Zc:preprocessor /GL ${RELEASE_FLAGS}"
            )
        endif()
    endif()
endif()
