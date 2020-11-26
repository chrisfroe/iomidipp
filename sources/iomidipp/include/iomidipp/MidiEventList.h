/**
 * @copyright 1999-2020, Craig Stuart Sapp under BSD-2 license
 * @copyright 2020-2020, Christoph Fröhner under BSD-2 license
 */

#pragma once

#include <iomidipp/MidiEvent.h>
#include <vector>

namespace imp {

// TODO remove this, can be vector<MidiEvent>. Move link and sort funtionality to utils.
class MidiEventList {
public:
    MidiEventList(void);

    MidiEventList(const MidiEventList &other);

    MidiEventList(MidiEventList &&other);

    MidiEventList &operator=(MidiEventList &other);

    MidiEvent &operator[](int index);

    const MidiEvent &operator[](int index) const;

    MidiEvent &back(void);

    const MidiEvent &back(void) const;

    MidiEvent &last(void);

    const MidiEvent &last(void) const;

    MidiEvent &getEvent(int index);

    const MidiEvent &getEvent(int index) const;

    void clear(void);

    void reserve(int rsize);

    int getEventCount(void) const;

    int getSize(void) const;

    int size(void) const;

    void removeEmpties(void);

    int linkNotePairs(void);

    int linkEventPairs(void);

    void clearLinks(void);

    void clearSequence(void);

    int markSequence(int sequence = 1);

    int push(MidiEvent &event);

    int push_back(MidiEvent &event);

    int append(MidiEvent &event);

    // careful when using these, intended for internal use in MidiFile class:
    void detach(void);

    int push_back_no_copy(MidiEvent *event);

    // access to the list of MidiEvents for sorting with an external function:
    MidiEvent **data(void);

protected:
    std::vector<MidiEvent> list;

private:
    void sort(void);

    // MidiFile class calls sort()
    friend class MidiFile;
};

int eventcompare(const void *a, const void *b);

}
