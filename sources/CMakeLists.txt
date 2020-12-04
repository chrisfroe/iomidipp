project(iomidi)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

# configuration
option(IOMIDIPP_CREATE_TEST_TARGET "Determines if the test target should be added. If using this as submodule this should be off." OFF)

# handle rpaths for install directory

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")

if (IOMIDIPP_CREATE_TEST_TARGET)
    # enable_testing()
    add_subdirectory(external/Catch2)
    # list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/Catch2/extras")
endif()

add_subdirectory(iomidipp)

if(IOMIDIPP_CREATE_TEST_TARGET)

    add_subdirectory(iomidipp/tests)

endif()
