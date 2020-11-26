project(iomidipp)

set(SOURCES
        src/MidiEvent.cpp
        src/MidiEventList.cpp
        src/MidiData.cpp
        src/MidiMessage.cpp
        src/MidiFile.cpp
        )

add_library(iomidipp SHARED ${SOURCES})

target_include_directories(iomidipp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(iomidipp PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
