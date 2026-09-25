cmake_minimum_required(VERSION 4.2.3)
project(tlpi)

add_executable(solution)

target_sources(solution
  PRIVATE
    ch9/ex3.c
    "../lib/error_functions.c"
    "../lib/get_num.c"

  PUBLIC
    FILE_SET libtlpi
    TYPE HEADERS
    BASE_DIRS
      ../lib/
      .
    FILES
      "../lib/tlpi_hdr.h"
      "check.h"
)

