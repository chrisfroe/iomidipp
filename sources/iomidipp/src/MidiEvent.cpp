/**
 * @copyright 1999-2020, Craig Stuart Sapp under BSD-2 license
 * @copyright 2020-2020, Christoph Fröhner under BSD-2 license
 */

#include <iomidipp/MidiEvent.h>

namespace imp {

MidiEvent::MidiEvent(int command)
    : MidiMessage(command) {}

MidiEvent::MidiEvent(int command, int p1)
    : MidiMessage(command, p1) {}

MidiEvent::MidiEvent(int command, int p1, int p2)
    : MidiMessage(command, p1, p2) {}

MidiEvent::MidiEvent(int aTime, int aTrack, std::vector<uchar>& message)
    : MidiMessage(message)
    , track(aTrack)
    , tick(aTime) {}

// MidiEvent::unlinkEvent -- Disassociate this event with another.
//   Also tell the other event to disassociate from this event.
void MidiEvent::unlinkEvent() {
    if (linkedEvent == nullptr) {
        return;
    }
    MidiEvent* mev = linkedEvent;
    linkedEvent = nullptr;
    mev->unlinkEvent();
}

// MidiEvent::linkEvent -- Make a link between two messages.
//   Unlinking
void MidiEvent::linkEvent(MidiEvent* mev) {
    if (mev->linkedEvent != nullptr) {
        // unlink other event if it is linked to something else;
        mev->unlinkEvent();
    }
    // if this is already linked to something else, then unlink:
    if (linkedEvent != nullptr) {
        linkedEvent->unlinkEvent();
    }
    unlinkEvent();

    mev->linkedEvent = this;
    linkedEvent = mev;
}

void MidiEvent::linkEvent(MidiEvent& mev) {
    linkEvent(&mev);
}

// MidiEvent::getLinkedEvent -- Returns a linked event.  Usually
//   this is the note-off message for a note-on message and vice-versa.
//   Returns null if there are no links.
MidiEvent* MidiEvent::getLinkedEvent() {
    return linkedEvent;
}

// MidiEvent::isLinked -- Returns true if there is an event which is not
//   nullptr.  This function is similar to getLinkedEvent().

bool MidiEvent::isLinked() const {
    return linkedEvent == nullptr;
}

// MidiEvent::getTickDuration --  For linked events (note-ons and note-offs),
//    return the absolute tick time difference between the two events.
//    The tick values are presumed to be in absolute tick mode rather than
//    delta tick mode.  Returns 0 if not linked.
int MidiEvent::getTickDuration() {
    MidiEvent* mev = getLinkedEvent();
    if (mev == nullptr) {
        return 0;
    }
    int tick2 = mev->tick;
    if (tick2 > tick) {
        return tick2 - tick;
    } else {
        return tick - tick2;
    }
}

// MidiEvent::getDurationInSeconds -- For linked events (note-ons and
//     note-offs), return the duration of the note in seconds.  The
//     seconds analysis must be done first; otherwise the duration will be
//     reported as zero.
double MidiEvent::getDurationInSeconds() {
    MidiEvent* mev = getLinkedEvent();
    if (mev == nullptr) {
        return 0;
    }
    double seconds2 = mev->seconds;
    if (seconds2 > seconds) {
        return seconds2 - seconds;
    } else {
        return seconds - seconds2;
    }
}

}// namespace imp
