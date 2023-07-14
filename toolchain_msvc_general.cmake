include_guard(GLOBAL)
include(CMakePrintHelpers)
message(STATUS "Loading general MSVC toolchain!")

string(APPEND CMAKE_SHARED_LINKER_FLAGS_RELEASE " /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF")
string(APPEND CMAKE_EXE_LINKER_FLAGS_RELEASE " /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF")

#Remove MD Flags from compiler flags
foreach(_config IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(REGEX REPLACE "(/|-)MDd?" "" CMAKE_CXX_FLAGS_${_config} "${CMAKE_CXX_FLAGS_${_config}}")
    string(REGEX REPLACE "(/|-)MDd?" "" CMAKE_C_FLAGS_${_config} "${CMAKE_C_FLAGS_${_config}}")
endforeach()
string(REGEX REPLACE "(/|-)MDd?" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REGEX REPLACE "(/|-)MDd?" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

string(APPEND CMAKE_CXX_FLAGS_RELEASE " /Z7")

if(General_FAST_MATH)
    add_compile_options(/fp:fast /fp:except-)
endif()

if(NOT General_AVX512 AND NOT General_AVX2)
    set(ARCH_FLAG /arch:AVX)
elseif(General_AVX512)
    set(ARCH_FLAG /arch:AVX512)
    add_definitions(EIGEN_ENABLE_AVX512 __AVX512F__ __FMA__ __AVX512DQ__ __AVX512ER__) # Eigen is unable to deduce those with visual studio
elseif(General_AVX2)
    set(ARCH_FLAG /arch:AVX2)
else()
endif()

add_compile_options($<$<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>:/MP> /bigobj /EHsc /permissive-)

#Warnings from https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
set(MSVC_WARNINGS
  /W4 # Baseline reasonable warnings
  /w14242 # 'identfier': conversion from 'type1' to 'type1', possible loss of data
  /w14254 # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
  /w14263 # 'function': member function does not override any base class virtual member function
  /w14265 # 'classname': class has virtual functions, but destructor is not virtual instances of this class may not be destructed correctly
  /w14287 # 'operator': unsigned/negative constant mismatch
  /we4289 # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside the for-loop scope
  /w14296 # 'operator': expression is always 'boolean_value'
  /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
  /w14545 # expression before comma evaluates to a function which is missing an argument list
  /w14546 # function call before comma missing argument list
  /w14547 # 'operator': operator before comma has no effect; expected operator with side-effect
  /w14549 # 'operator': operator before comma has no effect; did you intend 'operator'?
  /w14555 # expression has no effect; expected expression with side- effect
  /w14619 # pragma warning: there is no warning number 'number'
  /w14640 # Enable warning on thread un-safe static member initialization
  /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior.
  /w14905 # wide string literal cast to 'LPSTR'
  /w14906 # string literal cast to 'LPWSTR'
  /w14928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
)
add_compile_options(${MSVC_WARNINGS})
add_compile_options(/utf-8)
add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_HAS_DEPRECATED_RESULT_OF)

if(DEFINED _VCPKG_INSTALLED_DIR 
   AND EXISTS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include" 
   AND CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
    #add_compile_options(/experimental:external /external:I "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
endif()
#add_compile_options("$<$<NOT:$<CONFIG:DEBUG>>:d>")