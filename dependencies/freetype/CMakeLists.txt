message(STATUS "Fetching freetype ...")

FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
  GIT_TAG        VER-2-12-1
)

FetchContent_MakeAvailable(freetype)