project(iomidi)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

# configuration
SET(IOMIDIPP_CREATE_TEST_TARGET OFF CACHE BOOL "Determines if the test target should be added. If using this as submodule this should be off.")

if (IOMIDIPP_CREATE_TEST_TARGET)
    enable_testing()
    add_subdirectory(external/Catch2)
    # list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/Catch2/extras")
endif()

add_subdirectory(iomidipp)
