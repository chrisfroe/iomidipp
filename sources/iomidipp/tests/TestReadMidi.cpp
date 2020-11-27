#include <catch2/catch_all.hpp>
#include <iomidipp/MidiFile.h>
#include <iostream>

TEST_CASE("Read midi file and print output") {
    WHEN("Example file is read and we do common operations") {
        imp::MidiData midiData = imp::File::read("testdata/scratch.mid");
        midiData.doTimeAnalysis();
        midiData.linkNotePairs();

        std::stringstream ss;
        
        int numberOfTracks = midiData.getNumberOfTracks();
        ss << "TPQ: " << midiData.getTicksPerQuarterNote() << "\n";
        if (numberOfTracks > 1) ss << "TRACKS: " << numberOfTracks << "\n";
        for (int track = 0; track < numberOfTracks; track++) {
            if (numberOfTracks > 1) ss << "\nTrack " << track << "\n";
            ss << "Tick\tSeconds\tDur\tMessage" << "\n";
            for (int event = 0; event < midiData[track].size(); event++) {
                ss << std::dec << midiData[track][event].tick;
                ss << '\t' << std::dec << midiData[track][event].seconds;
                ss << '\t';
                if (midiData[track][event].isNoteOn())
                    ss << midiData[track][event].getDurationInSeconds();
                ss << '\t' << std::hex;
                for (int i = 0; i < midiData[track][event].getSize(); i++)
                    ss << (int) midiData[track][event][i] << ' ';
                ss << "\n";
            }
        }
        THEN("Nothing bad happens along the way") {
            REQUIRE(true);
            // std::cout << ss.str() << std::endl;
        }
    }
}
