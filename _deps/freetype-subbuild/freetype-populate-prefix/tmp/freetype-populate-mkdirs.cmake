# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/breizh/Project-Alice/_deps/freetype-src"
  "/home/breizh/Project-Alice/_deps/freetype-build"
  "/home/breizh/Project-Alice/_deps/freetype-subbuild/freetype-populate-prefix"
  "/home/breizh/Project-Alice/_deps/freetype-subbuild/freetype-populate-prefix/tmp"
  "/home/breizh/Project-Alice/_deps/freetype-subbuild/freetype-populate-prefix/src/freetype-populate-stamp"
  "/home/breizh/Project-Alice/_deps/freetype-subbuild/freetype-populate-prefix/src"
  "/home/breizh/Project-Alice/_deps/freetype-subbuild/freetype-populate-prefix/src/freetype-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/breizh/Project-Alice/_deps/freetype-subbuild/freetype-populate-prefix/src/freetype-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/breizh/Project-Alice/_deps/freetype-subbuild/freetype-populate-prefix/src/freetype-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
