cmake_minimum_required(VERSION 3.10)

project(kcp)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(CMakeBuildTypes)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build configuration: " ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_FLAGS "-DIL_STD -g -ggdb -Wall -Wno-unknown-pragmas -fexceptions -std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb -fsanitize=address -fno-omit-frame-pointer")

find_package(cplex)
include_directories(SYSTEM ${CPLEX_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable(kcp src/kcp.cpp src/model.cpp)
target_link_libraries(kcp ${CPLEX_LIBRARIES})
