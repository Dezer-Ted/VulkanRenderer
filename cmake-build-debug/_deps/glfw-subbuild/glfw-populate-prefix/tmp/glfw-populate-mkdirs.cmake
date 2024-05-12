# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Github Repositories/GraphicsProgramming/cmake-build-debug/_deps/glfw-src"
  "D:/Github Repositories/GraphicsProgramming/cmake-build-debug/_deps/glfw-build"
  "D:/Github Repositories/GraphicsProgramming/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix"
  "D:/Github Repositories/GraphicsProgramming/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/tmp"
  "D:/Github Repositories/GraphicsProgramming/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp"
  "D:/Github Repositories/GraphicsProgramming/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/src"
  "D:/Github Repositories/GraphicsProgramming/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Github Repositories/GraphicsProgramming/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Github Repositories/GraphicsProgramming/cmake-build-debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()