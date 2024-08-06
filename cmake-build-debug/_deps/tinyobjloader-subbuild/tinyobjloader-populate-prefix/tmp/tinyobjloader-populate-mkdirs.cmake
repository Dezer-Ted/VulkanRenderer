# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Git/GraphicsProgRestored/cmake-build-debug/_deps/tinyobjloader-src"
  "C:/Git/GraphicsProgRestored/cmake-build-debug/_deps/tinyobjloader-build"
  "C:/Git/GraphicsProgRestored/cmake-build-debug/_deps/tinyobjloader-subbuild/tinyobjloader-populate-prefix"
  "C:/Git/GraphicsProgRestored/cmake-build-debug/_deps/tinyobjloader-subbuild/tinyobjloader-populate-prefix/tmp"
  "C:/Git/GraphicsProgRestored/cmake-build-debug/_deps/tinyobjloader-subbuild/tinyobjloader-populate-prefix/src/tinyobjloader-populate-stamp"
  "C:/Git/GraphicsProgRestored/cmake-build-debug/_deps/tinyobjloader-subbuild/tinyobjloader-populate-prefix/src"
  "C:/Git/GraphicsProgRestored/cmake-build-debug/_deps/tinyobjloader-subbuild/tinyobjloader-populate-prefix/src/tinyobjloader-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Git/GraphicsProgRestored/cmake-build-debug/_deps/tinyobjloader-subbuild/tinyobjloader-populate-prefix/src/tinyobjloader-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Git/GraphicsProgRestored/cmake-build-debug/_deps/tinyobjloader-subbuild/tinyobjloader-populate-prefix/src/tinyobjloader-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
