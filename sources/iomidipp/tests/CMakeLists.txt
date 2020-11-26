project(iomidipp_tests)

add_executable(iomidipp_tests TestMain.cpp TestReadMidi.cpp)

target_link_libraries(iomidipp_tests PUBLIC Catch2::Catch2)
target_link_libraries(iomidipp_tests PUBLIC iomidipp)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

#include(CTest)
#include(Catch)
#catch_discover_tests(iomidipp_tests)
