#! /usr/bin/env wish BUILD

# This CMake script has some complications to consider:
#
#   -DARCH=32          Architecture bits to use non-MSVC targets 
#   -DARCH=64          (GCC/Clang/etc) that take the -m32 and -m64 flags.
#                      (For MSVC targets, select the correct -G option.)
#
#   -DLINKAGE=direct   Compile csprng.c/cpp directly into the exe
#   -DLINKAGE=static   Build libcsprng.a/csprng.lib and link that with exe
#
# The BUILD script is designed to handle these considerations for you.

cmake_minimum_required (VERSION 2.6)

set (ARCH    "0"      CACHE STRING "Specify 32 or 64")
set (LINKAGE "direct" CACHE STRING "Specify direct or static")

MESSAGE (STATUS "ARCH = " ${ARCH})
MESSAGE (STATUS "LINKAGE = " ${LINKAGE})

if (${ARCH} EQUAL "32")
  set (ARCH -m${ARCH})
  set (BINDIR ../bin32)
  set (LIBDIR ../lib32)
else (${ARCH} EQUAL "32")
  set (BINDIR ../bin)
  set (LIBDIR ../lib)
  if (${ARCH} EQUAL "64")
    set (ARCH -m${ARCH})
  endif (${ARCH} EQUAL "64")
endif (${ARCH} EQUAL "32")

if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
  if (NOT ${CMAKE_GENERATOR} MATCHES "Win64")
    set (BINDIR ../bin32)
    set (LIBDIR ../lib32)
  endif (NOT ${CMAKE_GENERATOR} MATCHES "Win64")
endif (${CMAKE_GENERATOR} MATCHES "Visual Studio")

set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_STANDARD 11)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY         ${BINDIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BINDIR})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY         ${LIBDIR})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIBDIR})

include_directories (source)

if (${LINKAGE} MATCHES "direct")
  add_executable (c-example-${LINKAGE}   ../example.c   ../source/csprng.c)
  add_executable (cpp-example-${LINKAGE} ../example.cpp ../source/csprng.cpp)
endif (${LINKAGE} MATCHES "direct")

if (${LINKAGE} MATCHES "static")
  add_executable (c-example-${LINKAGE}   ../example.c  )
  add_executable (cpp-example-${LINKAGE} ../example.cpp)
  add_library (csprng STATIC ../source/csprng.c)
endif (${LINKAGE} MATCHES "static")

if (NOT ${LINKAGE} MATCHES "direct")
  if (NOT ${ARCH} EQUAL "0")
    set_target_properties (csprng PROPERTIES COMPILE_FLAGS ${ARCH} LINK_FLAGS ${ARCH})
  endif (NOT ${ARCH} EQUAL "0")
  target_link_libraries (c-example-${LINKAGE}   csprng)
  target_link_libraries (cpp-example-${LINKAGE} csprng)
endif (NOT ${LINKAGE} MATCHES "direct")

if (NOT ${ARCH} EQUAL "0")
  set_target_properties (c-example-${LINKAGE}   PROPERTIES COMPILE_FLAGS ${ARCH} LINK_FLAGS ${ARCH})
  set_target_properties (cpp-example-${LINKAGE} PROPERTIES COMPILE_FLAGS ${ARCH} LINK_FLAGS ${ARCH})
endif (NOT ${ARCH} EQUAL "0")
