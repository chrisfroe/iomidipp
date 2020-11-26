project(iomidi)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

# configuration
option(IOMIDIPP_CREATE_TEST_TARGET "Determines if the test target should be added. If using this as submodule this should be off." OFF)

if (IOMIDIPP_CREATE_TEST_TARGET)
    enable_testing()
    add_subdirectory(external/Catch2)
    # list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/Catch2/extras")
endif()

add_subdirectory(iomidipp)

if(IOMIDIPP_CREATE_TEST_TARGET)

    add_subdirectory(iomidipp/tests)

endif()
