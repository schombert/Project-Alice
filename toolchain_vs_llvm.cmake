include_guard(GLOBAL)
message(STATUS "Loading clang-cl toolchain using VS!")
include("${CMAKE_CURRENT_LIST_DIR}/toolchain_msvc_general.cmake")

#Warnings from https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
set(CLANG_WARNINGS
    -Wall
    -Wextra # reasonable and standard
    -Wshadow # warn the user if a variable declaration shadows one from a parent context
    -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    -Wold-style-cast # warn for c-style casts
    -Wcast-align # warn for potential performance problem casts
    -Wunused # warn on anything being unused
    -Woverloaded-virtual # warn if you overload (not override) a virtual function
    -Wpedantic # warn if non-standard C++ is used
    -Wconversion # warn on type conversions that may lose data
    #-Wsign-conversion # warn on sign conversions
    -Wnull-dereference # warn if a null dereference is detected
    -Wdouble-promotion # warn if float is implicit promoted to double
    -Wformat=2 # warn on security issues around functions that format output (ie printf)
    -Wmisleading-indentation # warn if identation implies blocks where blocks do not exist
    -Weffc++
)

set(CLANG_SILENCED 
    -Wno-c++98-compat
    -Wno-c++98-compat-pedantic
    -Wno-documentation-deprecated-sync
    -Wno-undef
    -Wno-documentation
    -Wno-global-constructors
    -Wno-exit-time-destructors
    -Wno-documentation-unknown-command
    -Wno-zero-as-null-pointer-constant
    -Wno-old-style-cast
    -Wno-switch-enum
    -Wno-cast-align
    -Wno-covered-switch-default
    -Wno-deprecated-dynamic-exception-spec
    -Wno-date-time
    -Wno-language-extension-token
    -Wno-extra-semi
    -Wno-reserved-id-macro
    -Wno-missing-noreturn
    -Wno-unused-template
    -Wno-anon-enum-enum-conversion
    -Wno-c99-extensions
    -Wno-deprecated-anon-enum-enum-conversion   #too noisy with eigen3
    -Wno-deprecated-enum-compare                #too noisy with eigen3
    -Wno-unused-parameter                       #a bit noisy
    -Wno-extra-semi-stmt                        # too noisy
    -Wno-sign-conversion                        # too noisy
)

string(APPEND CMAKE_C_FLAGS " -ftime-trace")
string(APPEND CMAKE_CXX_FLAGS " -ftime-trace")
add_compile_options(-fcolor-diagnostics)
#add_compile_options(-ftime-trace)
#add_compile_options(-ftime-report)
#string(APPEND CMAKE_CXX_FLAGS_RELEASE " -fsanitize=address")
#add_compile_options(-fsanitize=address) # Only works for release

add_compile_options(${CLANG_WARNINGS})
add_compile_options(${CLANG_SILENCED})
#add_compile_options(/clang:-flto=full)
#add_compile_options(-v)
if(DEFINED _VCPKG_INSTALLED_DIR AND EXISTS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
    # Nothing works
    #add_compile_options("/clang:--system-header-prefix ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
    #add_compile_options("/clang:-isystem ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
    #add_compile_options(-Xclang "-isystem-prefix ${VCPKG_TARGET_TRIPLET}\\include")
    #add_compile_options(-Xclang ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/)
endif()
