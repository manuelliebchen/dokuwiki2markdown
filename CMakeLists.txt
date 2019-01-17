cmake_minimum_required(VERSION 2.8)
# Project Name
PROJECT(dokuwiki2markdown)

set (CMAKE_CXX_STANDARD 17)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set( SOURCE
  ${SRC_DIR}/main.cpp
)

include_directories(
  ${PROJECT_SOURCE_DIR}/src
)

set( CMAKE_CXX_FLAGS "-O0 -Wall ")

add_executable( dokuwiki2markdown ${SOURCE})

install(TARGETS dokuwiki2markdown DESTINATION /usr/bin)
