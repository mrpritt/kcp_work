# cmake/FindBitscan.cmake

if(NOT TARGET bitscan)
    file(GLOB BITSCAN_SRC
        "${CMAKE_BINARY_DIR}/external/bitscan/*.cpp"
    )

    add_library(bitscan STATIC ${BITSCAN_SRC})

    target_include_directories(bitscan PUBLIC
        "${CMAKE_BINARY_DIR}/external/bitscan"
    )

    add_library(Bitscan::Bitscan ALIAS bitscan)
endif()

set(BITSCAN_FOUND TRUE)

