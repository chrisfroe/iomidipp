project(iomidipp_tests)

add_executable(iomidipp_tests TestMain.cpp TestReadMidi.cpp)

target_link_libraries(iomidipp_tests Catch2::Catch2)

#include(CTest)
#include(Catch)
#catch_discover_tests(iomidipp_tests)
