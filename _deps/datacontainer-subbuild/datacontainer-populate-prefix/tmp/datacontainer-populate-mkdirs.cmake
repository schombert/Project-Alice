# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/breizh/Project-Alice/_deps/datacontainer-src"
  "/home/breizh/Project-Alice/_deps/datacontainer-build"
  "/home/breizh/Project-Alice/_deps/datacontainer-subbuild/datacontainer-populate-prefix"
  "/home/breizh/Project-Alice/_deps/datacontainer-subbuild/datacontainer-populate-prefix/tmp"
  "/home/breizh/Project-Alice/_deps/datacontainer-subbuild/datacontainer-populate-prefix/src/datacontainer-populate-stamp"
  "/home/breizh/Project-Alice/_deps/datacontainer-subbuild/datacontainer-populate-prefix/src"
  "/home/breizh/Project-Alice/_deps/datacontainer-subbuild/datacontainer-populate-prefix/src/datacontainer-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/breizh/Project-Alice/_deps/datacontainer-subbuild/datacontainer-populate-prefix/src/datacontainer-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/breizh/Project-Alice/_deps/datacontainer-subbuild/datacontainer-populate-prefix/src/datacontainer-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
