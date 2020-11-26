/**
 * @copyright 1999-2020, Craig Stuart Sapp under BSD-2 license
 * @copyright 2020-2020, Christoph Fröhner under BSD-2 license
 */

#pragma once

#include <vector>

#include <iomidipp/Utils.h>
#include <iomidipp/MidiMessage.h>

namespace imp {

class MidiEvent : public MidiMessage {
public:
    MidiEvent();

    explicit MidiEvent(int command);

    MidiEvent(int command, int param1);

    MidiEvent(int command, int param1, int param2);

    MidiEvent(int aTime, int aTrack,
              std::vector<uchar> &message);

    // functions related to event linking (note-ons to note-offs).
    void unlinkEvent();

    void linkEvent(MidiEvent *mev);

    void linkEvent(MidiEvent &mev);

    int isLinked() const;

    MidiEvent *getLinkedEvent(void);

    const MidiEvent *getLinkedEvent(void) const;

    int getTickDuration(void) const;

    double getDurationInSeconds(void) const;

    int tick{};      // delta or absolute MIDI ticks
    int track{};     // [original] track number of event in MIDI file
    double seconds{};// calculated time in sec. (after doTimeAnalysis())
    int seq{};       // sorting sequence number of event

private:
    MidiEvent *linkedEvent{};// used to match note-ons and note-offs
};

}
