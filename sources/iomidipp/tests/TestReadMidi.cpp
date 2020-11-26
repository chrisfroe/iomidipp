#include <catch2/catch_all.hpp>
#include <iomidipp/MidiFile.h>
#include <iostream>

TEST_CASE("Foo", "") {
    imp::MidiData midiData = imp::File::read("testdata/scratch.mid");
    midiData.doTimeAnalysis();
    midiData.linkNotePairs();

    int numberOfTracks = midiData.getNumberOfTracks();
    std::cout << "TPQ: " << midiData.getTicksPerQuarterNote() << "\n";
    if (numberOfTracks > 1) std::cout << "TRACKS: " << numberOfTracks << "\n";
    for (int track = 0; track < numberOfTracks; track++) {
        if (numberOfTracks > 1) std::cout << "\nTrack " << track << "\n";
        std::cout << "Tick\tSeconds\tDur\tMessage" << "\n";
        for (int event = 0; event < midiData[track].size(); event++) {
            std::cout << std::dec << midiData[track][event].tick;
            std::cout << '\t' << std::dec << midiData[track][event].seconds;
            std::cout << '\t';
            if (midiData[track][event].isNoteOn())
                std::cout << midiData[track][event].getDurationInSeconds();
            std::cout << '\t' << std::hex;
            for (int i = 0; i < midiData[track][event].getSize(); i++)
                std::cout << (int) midiData[track][event][i] << ' ';
            std::cout << "\n";
        }
    }
}
