project(iomidipp_tests)

add_executable(iomidipp_tests TestMain.cpp TestReadMidi.cpp TestJoinAndSplitTracks.cpp)

target_link_libraries(iomidipp_tests PRIVATE Catch2::Catch2)
target_link_libraries(iomidipp_tests PRIVATE iomidipp)

# copy test data to build dir
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

install(TARGETS iomidipp_tests
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# copy test data next to installed test target
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/testdata" DESTINATION bin)
#install(FILES "${CMAKE_CURRENT_BINARY_DIR}/testdata"
#        "${CMAKE_CURRENT_BINARY_DIR}/MyLibConfigVersion.cmake"
#        DESTINATION lib/cmake/MyLib
#        )

#install(file(GET_RUNTIME_DEPENDENCIES))

#include(CTest)
#include(Catch)
#catch_discover_tests(iomidipp_tests)
