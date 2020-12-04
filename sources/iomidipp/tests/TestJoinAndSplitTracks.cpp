#include <catch2/catch_all.hpp>
#include <iomidipp/MidiFile.h>
#include <iostream>

TEST_CASE("Join tracks and split again, there should not be any spurious events on joined track") {
    // TODO option on split tracks to have events remain on joined track
}
