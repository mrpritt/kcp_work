cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(MY_PROJECT_NAME kcp)
project(${MY_PROJECT_NAME})

######## Cplex Dependency
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


######## Complier message
set(CMAKE_BUILD_TYPE Release)
message(STATUS "System: ${CMAKE_SYSTEM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

######## Set output directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build//bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)

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
add_executable(hl src/run_hl.cpp src/timer.cpp src/logging.cpp src/utils.cpp src/model.cpp src/heuristic.cpp src/solution.cpp)
######## Add Dependency Library
target_link_libraries(${MY_PROJECT_NAME} cplex-library cplex-concert ilocplex cplex-cp)
target_link_libraries(cplex_kpc cplex-library cplex-concert ilocplex cplex-cp)
target_link_libraries(cplex_mwis cplex-library cplex-concert ilocplex cplex-cp)
target_link_libraries(hl cplex-library cplex-concert ilocplex cplex-cp)
