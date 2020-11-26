/**
 * @copyright 1999-2020, Craig Stuart Sapp under BSD-2 license
 * @copyright 2020-2020, Christoph Fröhner under BSD-2 license
 */

#pragma once

#include <fstream>
#include <istream>
#include <string>
#include <vector>

#include <iomidipp/MidiEventList.h>

#define TIME_STATE_DELTA 0
#define TIME_STATE_ABSOLUTE 1

#define TRACK_STATE_SPLIT 0
#define TRACK_STATE_JOINED 1

namespace imp {

class TickTime {
public:
    int tick;
    double seconds;
};


class MidiData {
public:
    MidiData() = default;

    // track-related functions:
    std::vector<MidiEventList> &tracks() {
        return _tracks;
    }

    const MidiEventList &operator[](int aTrack) const;

    MidiEventList &operator[](int aTrack);

    std::size_t getNumberOfTracks() const;

    void removeEmpties();

    // tick-related functions:
    void makeDeltaTicks();

    void deltaTicks();

    void makeAbsoluteTicks();

    void absoluteTicks();

    int getTickState() const;

    bool isDeltaTicks() const;

    bool isAbsoluteTicks() const;

    // join/split track functionality:
    void joinTracks();

    void splitTracks();

    void splitTracksByChannel();

    int getTrackState() const;

    int hasJoinedTracks() const;

    int hasSplitTracks() const;

    int getSplitTrack(int track, int index);

    int getSplitTrack(int index);

    // track sorting
    void sortTracks();

    void markSequence();

    // track manipulation functionality:
    int addTrack();

    int addTrack(int count);

    int addTracks(int count);

    void deleteTrack(int aTrack);

    void mergeTracks(int aTrack1, int aTrack2);

    int getTrackCountAsType1();

    // ticks-per-quarter related functions:
    void setMillisecondTicks();

    int getTicksPerQuarterNote() const;

    int getTPQ() const;

    void setTicksPerQuarterNote(int ticks);

    void setTPQ(int ticks);

    int getTimeState() const {
        return _timeState;
    }

    void setTimeState(int value) {
        _timeState = value;
    }

    // physical-time analysis functions:
    void doTimeAnalysis();

    double getTimeInSeconds(int aTrack, int anIndex);

    double getTimeInSeconds(int tickvalue);

    double getAbsoluteTickTime(double starttime);

    int getFileDurationInTicks();

    double getFileDurationInQuarters();

    double getFileDurationInSeconds();

    // note-analysis functions:
    int linkNotePairs();

    int linkEventPairs();

    void clearLinks();

    // filename functions:
    void setFilename(const std::string &aname);

    std::string getFilename() const;

    // event functionality:
    MidiEvent addEvent(int aTrack, int aTick,
                       std::vector<uchar> &midiData);

    MidiEvent addEvent(MidiEvent &mfevent);

    MidiEvent addEvent(int aTrack, MidiEvent &mfevent);

    MidiEvent &getEvent(int aTrack, int anIndex);

    int getNumberOfEvents(int aTrack) const;

    void clear();

    // Meta-event adding convenience functions:
    MidiEvent addMetaEvent(int aTrack, int aTick,
                           int aType,
                           std::vector<uchar> &metaData);

    MidiEvent addMetaEvent(int aTrack, int aTick,
                           int aType,
                           const std::string &metaData);

protected:
    // lists of MidiEvents for each MIDI file track.
    std::vector<MidiEventList> _tracks;

    // m_ticksPerQuarterNote == A value for the MIDI file header
    // which represents the number of ticks in a quarter note
    // that are used as units for the delta times for MIDI events
    // in MIDI file track data.
    int m_ticksPerQuarterNote = 120;

    // _trackState == state variable for whether the tracks
    // are joined or split.
    int _trackState = TRACK_STATE_SPLIT;

    // _timeState == state variable for whether the MidiEvent::tick
    // variable contain absolute ticks since the start of the file's
    // time, or delta ticks since the last MIDI event in the track.
    int _timeState = TIME_STATE_ABSOLUTE;

    // _readFileName == the filename of the last file read into
    // the object.
    std::string _readFileName;

    // _timemapvalid ==
    bool _timemapvalid = false;

    // m_timemap ==
    std::vector<TickTime> m_timemap;

    // m_linkedEventQ == True if link analysis has been done.
    bool m_linkedEventsQ = false;

private:
    int makeVLV(uchar *buffer, int number);

    static int ticksearch(const void *A, const void *B);

    static int secondsearch(const void *A, const void *B);

    void buildTimeMap();

    double linearTickInterpolationAtSecond(double seconds);

    double linearSecondInterpolationAtTick(int ticktime);
};

}

std::ostream &operator<<(std::ostream &out, imp::MidiData &aMidiFile);
