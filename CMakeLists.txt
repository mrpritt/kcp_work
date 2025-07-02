cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(MY_PROJECT_NAME kcp)
project(${MY_PROJECT_NAME})

############# Bitscan dependency
set(BITSCAN_EXTERNAL_DIR "${CMAKE_BINARY_DIR}/external/bitscan")

if(NOT EXISTS "${BITSCAN_EXTERNAL_DIR}/CMakeLists.txt")
    message(STATUS "Cloning BITSCAN into ${BITSCAN_EXTERNAL_DIR}")
    execute_process(
        COMMAND git clone https://github.com/psanse/BITSCAN.git ${BITSCAN_EXTERNAL_DIR}
        RESULT_VARIABLE GIT_CLONE_RESULT
    )
    if(NOT GIT_CLONE_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to clone BITSCAN repository")
    endif()
endif()

# Make CMake aware of custom modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(Bitscan REQUIRED)


######## Cplex Dependency
if(DEFINED ENV{CPLEX_ROOT_DIR})
    set(CPLEX_STUDIO_DIR $ENV{CPLEX_ROOT_DIR} CACHE PATH "CPLEX root directory from environment")
endif()
find_package(CPLEX REQUIRED)
if(CPLEX_FOUND)
    include_directories(${CPLEX_INCLUDE_DIR})
    link_directories(${CPLEX_LIBRARY})
endif()

if(CPLEX_CONCERT_FOUND)
    include_directories(${CPLEX_CONCERT_INCLUDE_DIR})
    link_directories(${CPLEX_CONCERT_LIBRARY})
endif()

if(CPLEX_ILOCPLEX_FOUND)
    include_directories(${CPLEX_ILOCPLEX_INCLUDE_DIR})
    link_directories(${CPLEX_ILOCPLEX_LIBRARY})
endif()

if(CPLEX_CP_FOUND)
    include_directories(${CPLEX_CP_INCLUDE_DIR})
    link_directories(${CPLEX_CP_LIBRARY})
endif()

set(FMT_SEARCH_PATHS ${CMAKE_SOURCE_DIR} $ENV{HOME}/Home/Software/fmt/include ../3rd/fmt/include)
find_path(FMT_INCLUDE_DIR fmt/core.h PATHS ${FMT_SEARCH_PATHS} NO_DEFAULT_PATH)

if (NOT FMT_INCLUDE_DIR)
  find_path(FMT_INCLUDE_DIR fmt/core.h)
endif()

if (FMT_INCLUDE_DIR)
  message(STATUS "fmtlib found at ${FMT_INCLUDE_DIR}.")
  include_directories(${FMT_INCLUDE_DIR})
else()
  message(FATAL_ERROR "fmtlib not found. Please install fmtlib or set FMT_SEARCH_PATHS.")
endif()


set(CMAKE_BUILD_TYPE Release)
message(STATUS "System: ${CMAKE_SYSTEM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

######## Set output directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

######## Set C++ 11 standard
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20)
if (COMPILER_SUPPORTS_CXX20)
    if (CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++20")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    endif ()
else ()
    message(STATUS "Fatal Error: Your compiler doesn't support c++20, please install it!")
endif ()


######## Set executable file name, and add the source files for it.
add_executable(${MY_PROJECT_NAME} src/timer.cpp src/logging.cpp src/utils.cpp src/kpc.cpp src/model.cpp src/kpc_dp.cpp src/kpc_dp2.cpp src/solution.cpp)
add_executable(cplex_kpc src/cplex_kpc.cpp src/timer.cpp src/logging.cpp src/utils.cpp src/model.cpp src/solution.cpp)
add_executable(cplex_mwis src/cplex_mwis.cpp src/timer.cpp src/logging.cpp src/utils.cpp src/model.cpp src/solution.cpp)
add_executable(cplex_ilp2 src/cplex_ilp2.cpp src/timer.cpp src/logging.cpp src/utils.cpp src/model.cpp src/solution.cpp)
add_executable(cplex_lp2 src/cplex_lp2.cpp src/timer.cpp src/logging.cpp src/utils.cpp src/model.cpp src/solution.cpp)
add_executable(cplex_lp1 src/cplex_lp1.cpp src/timer.cpp src/logging.cpp src/utils.cpp src/model.cpp src/solution.cpp)
add_executable(hl src/run_hl.cpp src/timer.cpp src/logging.cpp src/utils.cpp src/model.cpp src/heuristic.cpp src/solution.cpp)
add_executable(ubl2 src/ubl2.cpp src/utils.cpp src/model.cpp)
add_executable(bb src/bb.cpp src/utils.cpp)
######## Add Dependency Library
target_link_libraries(${MY_PROJECT_NAME} cplex-library cplex-concert ilocplex cplex-cp)
target_link_libraries(cplex_kpc cplex-library cplex-concert ilocplex cplex-cp)
target_link_libraries(cplex_mwis cplex-library cplex-concert ilocplex cplex-cp)
target_link_libraries(cplex_ilp2 cplex-library cplex-concert ilocplex cplex-cp)
target_link_libraries(cplex_lp2 cplex-library cplex-concert ilocplex cplex-cp)
target_link_libraries(cplex_lp1 cplex-library cplex-concert ilocplex cplex-cp)
target_link_libraries(hl cplex-library cplex-concert ilocplex cplex-cp)
target_link_libraries(ubl2 cplex-library cplex-concert ilocplex cplex-cp)
target_link_libraries(bb PRIVATE Bitscan::Bitscan cplex-library cplex-concert ilocplex cplex-cp)
