cmake_minimum_required(VERSION 3.18.4)

set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

## add your own library here
add_library(
  mm SHARED
  include/mm.h
  src/mm.c
)
target_include_directories(mm PUBLIC "${LIB_INCLUDE_DIR}")

## external link_libraries
add_library(
  memlib SHARED
  include/memlib.h
  src/memlib.c
)
target_include_directories(memlib PUBLIC "${LIB_INCLUDE_DIR}")

## aggregate all auxiliary libraries into libcomm 
## todo: add library to import in libcomm
add_library(
  libexternal INTERFACE
)

target_link_libraries(
  libexternal INTERFACE
  memlib 
)
target_include_directories(libexternal INTERFACE "${LIB_INCLUDE_DIR}")
