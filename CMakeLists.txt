cmake_minimum_required(VERSION 3.10)

project(kcp)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(CMakeBuildTypes)

find_package(cplex)
include_directories(SYSTEM ${CPLEX_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable(kcp src/kcp.cpp src/model.cpp)
target_link_libraries(kcp ${CPLEX_LIBRARIES})
