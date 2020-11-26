/**
 * @copyright 1999-2020, Craig Stuart Sapp under BSD-2 license
 * @copyright 2020-2020, Christoph Fröhner under BSD-2 license
 */

#pragma once

#include <iomidipp/MidiEvent.h>
#include <vector>

namespace imp {

using MidiEventList = std::vector<MidiEvent>;

void removeEmpties(MidiEventList &list);

int linkNotePairs(MidiEventList &list);

void clearLinks(MidiEventList &list);

void clearSequence(MidiEventList &list);

int markSequence(MidiEventList &list, int sequence = 1);

void sort(MidiEventList& list);

int eventcompare(MidiEvent const &a, MidiEvent const &b);

}
