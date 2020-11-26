project(iomidipp)

set(SOURCES
        src/MidiEvent.cpp
        src/MidiEventList.cpp
        src/MidiFile.cpp
        src/MidiMessage.cpp
        )

add_library(iomidipp SHARED ${SOURCES})

target_include_directories(iomidipp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (IODMIDIPP_CREATE_TEST_TARGET)
    add_subdirectory(tests)
endif ()
