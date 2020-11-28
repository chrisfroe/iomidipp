/**
 * @copyright 1999-2020, Craig Stuart Sapp under BSD-2 license
 * @copyright 2020-2020, Christoph Fröhner under BSD-2 license
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <iomidipp/MidiData.h>

namespace imp {

// MidiFile::operator[] -- return the event list for the specified track.
MidiEventList& MidiData::operator[](int aTrack) {
    return _tracks[aTrack];
}

const MidiEventList& MidiData::operator[](int aTrack) const {
    return _tracks[aTrack];
}

std::size_t MidiData::getNumberOfTracks() const {
    return _tracks.size();
}

// MidiFile::removeEmpties -- Remove any MIDI message that
//     contains no bytes.
void MidiData::removeEmpties() {
    for (auto& track : _tracks) {
        auto _ = std::remove_if(track.begin(), track.end(), [](MidiEvent const& event) { return event.isEmpty(); });
    }
}

// MidiFile::markSequence -- Assign a sequence serial number to
//   every MidiEvent in every track in the MIDI file.  This is
//   useful if you want to preseve the order of MIDI messages in
//   a track when they occur at the same tick time.  Particularly
//   for use with joinTracks() or sortTracks().  markSequence will
//   be done automatically when a MIDI file is read, in case the
//   ordering of m_events occuring at the same time is important.
//   Use clearSequence() to use the default sorting behavior of
//   sortTracks().

void MidiData::markSequence() {
    int sequence = 1;
    for (auto& track : _tracks) {
        sequence = imp::markSequence(track, sequence);
    }
}

// MidiFile::joinTracks -- Interleave the data from all tracks,
//   but keeping the identity of the tracks unique so that
//   the function splitTracks can be called to split the
//   tracks into separate units again.  The style of the
//   MidiFile when read from a file is with tracks split.
//   The original track index is stored in the MidiEvent::track
//   variable.
void MidiData::joinTracks() {
    if (getTrackState() == TRACK_STATE_JOINED) {
        return;
    }
    if (getNumberOfTracks() == 1) {
        _trackState = TRACK_STATE_JOINED;
        return;
    }

    MidiEventList joinedTrack;

    int messagesum = 0;
    int length = getNumberOfTracks();
    int i, j;
    for (i = 0; i < length; i++) {
        messagesum += (_tracks[i]).size();
    }
    joinedTrack.reserve((int) (messagesum + 32 + messagesum * 0.1));

    int oldTimeState = getTickState();
    if (oldTimeState == TIME_STATE_DELTA) {
        makeAbsoluteTicks();
    }
    for (i = 0; i < length; i++) {
        for (j = 0; j < (int) _tracks[i].size(); j++) {
            joinedTrack.push_back(_tracks[i][j]);
        }
    }

    _tracks.resize(0);
    _tracks.push_back(joinedTrack);
    sortTracks();
    if (oldTimeState == TIME_STATE_DELTA) {
        makeDeltaTicks();
    }

    _trackState = TRACK_STATE_JOINED;
}

// MidiFile::splitTracks -- Take the joined tracks and split them
//   back into their separate track identities.
void MidiData::splitTracks() {
    if (getTrackState() == TRACK_STATE_SPLIT) {
        return;
    }
    int oldTimeState = getTickState();
    if (oldTimeState == TIME_STATE_DELTA) {
        makeAbsoluteTicks();
    }

    int maxTrack = 0;
    int i;
    int length = _tracks[0].size();
    for (i = 0; i < length; i++) {
        if ((_tracks[0])[i].track > maxTrack) {
            maxTrack = (_tracks[0])[i].track;
        }
    }
    int m_trackCount = maxTrack + 1;

    if (m_trackCount <= 1) {
        return;
    }

    MidiEventList joinedTrack = _tracks[0];
    _tracks.resize(m_trackCount);
    for (i = 0; i < length; i++) {
        int trackValue = joinedTrack[i].track;
        _tracks[trackValue].push_back(joinedTrack[i]);
    }

    if (oldTimeState == TIME_STATE_DELTA) {
        makeDeltaTicks();
    }

    _trackState = TRACK_STATE_SPLIT;
}

// MidiFile::splitTracksByChannel -- Take the joined tracks and split them
//   back into their separate track identities.
void MidiData::splitTracksByChannel() {
    joinTracks();
    if (getTrackState() == TRACK_STATE_SPLIT) {
        return;
    }

    int oldTimeState = getTickState();
    if (oldTimeState == TIME_STATE_DELTA) {
        makeAbsoluteTicks();
    }

    int maxTrack = 0;
    int i;
    MidiEventList& eventlist = _tracks[0];
    int length = eventlist.size();
    for (i = 0; i < length; i++) {
        if (eventlist[i].getSize() == 0) {
            continue;
        }
        if ((eventlist[i][0] & 0xf0) == 0xf0) {
            // ignore system and meta messages.
            continue;
        }
        if (maxTrack < (eventlist[i][0] & 0x0f)) {
            maxTrack = eventlist[i][0] & 0x0f;
        }
    }
    int m_trackCount = maxTrack + 2;// + 1 for expression track

    if (m_trackCount <= 1) {
        // only one channel, so don't do anything (leave as Type-0 file).
        return;
    }

    _tracks.resize(m_trackCount);
    for (i = 0; i < length; i++) {
        int trackValue = 0;
        if ((eventlist[i][0] & 0xf0) == 0xf0) {
            trackValue = 0;
        } else if (eventlist[i].getSize() > 0) {
            trackValue = (eventlist[i][0] & 0x0f) + 1;
        }
        _tracks[trackValue].push_back(eventlist[i]);
    }

    if (oldTimeState == TIME_STATE_DELTA) {
        makeDeltaTicks();
    }

    _trackState = TRACK_STATE_SPLIT;
}

// MidiFile::getTrackState -- returns what type of track method
//     is being used: either TRACK_STATE_JOINED or TRACK_STATE_SPLIT.
int MidiData::getTrackState() const {
    return _trackState;
}

// MidiFile::hasJoinedTracks -- Returns true if the MidiFile tracks
//    are in a joined state.
int MidiData::hasJoinedTracks() const {
    return _trackState == TRACK_STATE_JOINED;
}

// MidiFile::hasSplitTracks -- Returns true if the MidiFile tracks
//     are in a split state.
int MidiData::hasSplitTracks() const {
    return _trackState == TRACK_STATE_SPLIT;
}

// MidiFile::getSplitTrack --  Return the track index when the MidiFile
//   is in the split state.  This function returns the original track
//   when the MidiFile is in the joined state.  The MidiEvent::track
//   variable is used to store the original track index when the
//   MidiFile is converted to the joined-track state.

int MidiData::getSplitTrack(int track, int index) {
    if (hasSplitTracks()) {
        return track;
    } else {
        return getEvent(track, index).track;
    }
}

// When the parameter is void, assume track 0:
int MidiData::getSplitTrack(int index) {
    if (hasSplitTracks()) {
        return 0;
    } else {
        return getEvent(0, index).track;
    }
}

// MidiFile::makeDeltaTicks -- convert the time data to
//     delta time, which means that the time field
//     in the MidiEvent struct represents the time
//     since the last event was played. When a MIDI file
//     is read from a file, this is the default setting.
void MidiData::makeDeltaTicks() {
    if (getTickState() == TIME_STATE_DELTA) {
        return;
    }
    int i, j;
    int temp;
    int length = getNumberOfTracks();
    int* timedata = new int[length];
    for (i = 0; i < length; i++) {
        timedata[i] = 0;
        if (!_tracks[i].empty()) {
            timedata[i] = (_tracks[i])[0].tick;
        } else {
            continue;
        }
        for (j = 1; j < (int) _tracks[i].size(); j++) {
            temp = (_tracks[i])[j].tick;
            int deltatick = temp - timedata[i];
            if (deltatick < 0) {
                std::cerr << "Error: negative delta tick value: " << deltatick << std::endl
                          << "Timestamps must be sorted first"
                          << " (use MidiFile::sortTracks() before writing)." << std::endl;
            }
            (_tracks[i])[j].tick = deltatick;
            timedata[i] = temp;
        }
    }
    _timeState = TIME_STATE_DELTA;
    delete[] timedata;
}

// MidiFile::makeAbsoluteTicks -- convert the time data to
//    absolute time, which means that the time field
//    in the MidiEvent struct represents the exact tick
//    time to play the event rather than the time since
//    the last event to wait untill playing the current
//    event.
void MidiData::makeAbsoluteTicks() {
    if (getTickState() == TIME_STATE_ABSOLUTE) {
        return;
    }
    int i, j;
    int length = getNumberOfTracks();
    int* timedata = new int[length];
    for (i = 0; i < length; i++) {
        timedata[i] = 0;
        if (!_tracks[i].empty()) {
            timedata[i] = (_tracks[i])[0].tick;
        } else {
            continue;
        }
        for (j = 1; j < (int) _tracks[i].size(); j++) {
            timedata[i] += (_tracks[i])[j].tick;
            (_tracks[i])[j].tick = timedata[i];
        }
    }
    _timeState = TIME_STATE_ABSOLUTE;
    delete[] timedata;
}

// MidiFile::absoluteTicks -- Alias for MidiFile::makeAbsoluteTicks().
void MidiData::absoluteTicks() {
    makeAbsoluteTicks();
}

// MidiFile::getTickState -- returns what type of time method is
//   being used: either TIME_STATE_ABSOLUTE or TIME_STATE_DELTA.
int MidiData::getTickState() const {
    return _timeState;
}

// MidiFile::isDeltaTicks -- Returns true if MidiEvent .tick
//    variables are in delta time mode.
bool MidiData::isDeltaTicks() const {
    return _timeState == TIME_STATE_DELTA;
}

// MidiFile::isAbsoluteTicks -- Returns true if MidiEvent .tick
//    variables are in absolute time mode.
bool MidiData::isAbsoluteTicks() const {
    return _timeState == TIME_STATE_ABSOLUTE;
}

// MidiFile::getFileDurationInTicks -- Returns the largest
//    tick value in any track.  The tracks must be sorted
//    before calling this function, since this function
//    assumes that the last MidiEvent in the track has the
//    highest tick timestamp.  The file state can be in delta
//    ticks since this function will temporarily go to absolute
//    tick mode for the calculation of the max tick.
int MidiData::getFileDurationInTicks() {
    bool revertToDelta = false;
    if (isDeltaTicks()) {
        makeAbsoluteTicks();
        revertToDelta = true;
    }
    const MidiData& mf = *this;
    int output = 0;
    for (int i = 0; i < mf.getNumberOfTracks(); i++) {
        if (mf[i].back().tick > output) {
            output = mf[i].back().tick;
        }
    }
    if (revertToDelta) {
        makeDeltaTicks();
    }
    return output;
}

// MidiFile::getFileDurationInSeconds -- returns the duration of the
//    longest track in the file.  The tracks must be sorted before
//    calling this function, since this function assumes that the
//    last MidiEvent in the track has the highest timestamp.
//    The file state can be in delta ticks since this function
//    will temporarily go to absolute tick mode for the calculation
//    of the max time.

double MidiData::getFileDurationInSeconds() {
    if (_timemapvalid == 0) {
        buildTimeMap();
        if (_timemapvalid == 0) {
            return -1.0;// something went wrong
        }
    }
    bool revertToDelta = false;
    if (isDeltaTicks()) {
        makeAbsoluteTicks();
        revertToDelta = true;
    }
    const MidiData& mf = *this;
    double output = 0.0;
    for (int i = 0; i < mf.getNumberOfTracks(); i++) {
        if (mf[i].back().seconds > output) {
            output = mf[i].back().seconds;
        }
    }
    if (revertToDelta) {
        makeDeltaTicks();
    }
    return output;
}

// MidiFile::doTimeAnalysis -- Identify the real-time position of
//    all events by monitoring the tempo in relations to the tick
//    times in the file.
//

void MidiData::doTimeAnalysis() {
    buildTimeMap();
}

// MidiFile::getTimeInSeconds -- return the time in seconds for
//     the current message.

double MidiData::getTimeInSeconds(int aTrack, int anIndex) {
    return getTimeInSeconds(getEvent(aTrack, anIndex).tick);
}

double MidiData::getTimeInSeconds(int tickvalue) {
    if (_timemapvalid == 0) {
        buildTimeMap();
        if (_timemapvalid == 0) {
            return -1.0;// something went wrong
        }
    }

    TickTime key;
    key.tick = tickvalue;
    key.seconds = -1;

    void* ptr = bsearch(&key, m_timemap.data(), m_timemap.size(),
                        sizeof(TickTime), ticksearch);

    if (ptr == nullptr) {
        // The specific tick value was not found, so do a linear
        // search for the two tick values which occur before and
        // after the tick value, and do a linear interpolation of
        // the time in seconds values to figure out the final
        // time in seconds.
        // Since the code is not yet written, kill the program at this point:
        return linearSecondInterpolationAtTick(tickvalue);
    } else {
        return ((TickTime*) ptr)->seconds;
    }
}

//////////////////////////////
//
// MidiFile::getAbsoluteTickTime -- return the tick value represented
//    by the input time in seconds.  If there is not tick entry at
//    the given time in seconds, then interpolate between two values.
//

double MidiData::getAbsoluteTickTime(double starttime) {
    if (_timemapvalid == 0) {
        buildTimeMap();
        if (_timemapvalid == 0) {
            if (_timemapvalid == 0) {
                return -1.0;// something went wrong
            }
        }
    }

    TickTime key;
    key.tick = -1;
    key.seconds = starttime;

    void* ptr = bsearch(&key, m_timemap.data(), m_timemap.size(),
                        sizeof(TickTime), secondsearch);

    if (ptr == NULL) {
        // The specific seconds value was not found, so do a linear
        // search for the two time values which occur before and
        // after the given time value, and do a linear interpolation of
        // the time in tick values to figure out the final time in ticks.
        return linearTickInterpolationAtSecond(starttime);
    } else {
        return ((TickTime*) ptr)->tick;
    }
}

// MidiFile::linkNotePairs --  Link note-ons to note-offs separately
//     for each track.  Returns the total number of note message pairs
//     that were linked.
int MidiData::linkNotePairs() {
    int i;
    int sum = 0;
    for (i = 0; i < getNumberOfTracks(); i++) {
        sum += imp::linkNotePairs(_tracks[i]);
    }
    m_linkedEventsQ = true;
    return sum;
}

// MidiFile::linkEventPairs -- Alias for MidiFile::linkNotePairs().
int MidiData::linkEventPairs() {
    return linkNotePairs();
}

// MidiFile::setFilename -- sets the filename of the MIDI file.
//      Currently removed any directory path.
void MidiData::setFilename(const std::string& aname) {
    auto loc = aname.rfind('/');
    if (loc != std::string::npos) {
        _readFileName = aname.substr(loc + 1);
    } else {
        _readFileName = aname;
    }
}

// MidiFile::getFilename -- returns the name of the file read into the
//    structure (if the data was read from a file).
std::string MidiData::getFilename() const {
    return _readFileName;
}

// MidiFile::addEvent --
MidiEvent MidiData::addEvent(int aTrack, int aTick,
                             std::vector<uchar>& midiData) {
    _timemapvalid = 0;
    MidiEvent me;
    me.tick = aTick;
    me.track = aTrack;
    me.setContent(midiData);
    _tracks[aTrack].push_back(me);
    return me;
}

// MidiFile::addEvent -- Some bug here when joinedTracks(), but track==1...
MidiEvent MidiData::addEvent(MidiEvent& mfevent) {
    if (getTrackState() == TRACK_STATE_JOINED) {
        _tracks[0].push_back(mfevent);
        return _tracks[0].back();
    } else {
        _tracks.at(mfevent.track).push_back(mfevent);
        return _tracks.at(mfevent.track).back();
    }
}

// Variant where the track is an input parameter:
MidiEvent MidiData::addEvent(int aTrack, MidiEvent& mfevent) {
    if (getTrackState() == TRACK_STATE_JOINED) {
        _tracks[0].push_back(mfevent);
        _tracks[0].back().track = aTrack;
        return _tracks[0].back();
    } else {
        _tracks.at(aTrack).push_back(mfevent);
        _tracks.at(aTrack).back().track = aTrack;
        return _tracks.at(aTrack).back();
    }
}

// MidiFile::addMetaEvent --
MidiEvent MidiData::addMetaEvent(int aTrack, int aTick, int aType,
                                 std::vector<uchar>& metaData) {
    _timemapvalid = 0;
    int i;
    int length = (int) metaData.size();
    std::vector<uchar> fulldata;
    uchar size[23] = {0};
    int lengthsize = makeVLV(size, length);

    fulldata.resize(2 + lengthsize + length);
    fulldata[0] = 0xff;
    fulldata[1] = aType & 0x7F;
    for (i = 0; i < lengthsize; i++) {
        fulldata[2 + i] = size[i];
    }
    for (i = 0; i < length; i++) {
        fulldata[2 + lengthsize + i] = metaData[i];
    }

    return addEvent(aTrack, aTick, fulldata);
}

MidiEvent MidiData::addMetaEvent(int aTrack, int aTick, int aType,
                                 const std::string& metaData) {
    int length = (int) metaData.size();
    std::vector<uchar> buffer;
    buffer.resize(length);
    int i;
    for (i = 0; i < length; i++) {
        buffer[i] = (uchar) metaData[i];
    }
    return addMetaEvent(aTrack, aTick, aType, buffer);
}

// MidiFile::makeVLV --  This function is used to create
//   size byte(s) for meta-messages.  If the size of the data
//   in the meta-message is greater than 127, then the size
//   should (?) be specified as a VLV.
int MidiData::makeVLV(uchar* buffer, int number) {
    unsigned long value = (unsigned long) number;

    if (value >= (1 << 28)) {
        std::cerr << "Error: Meta-message size too large to handle" << std::endl;
        buffer[0] = 0;
        buffer[1] = 0;
        buffer[2] = 0;
        buffer[3] = 0;
        return 1;
    }

    buffer[0] = (value >> 21) & 0x7f;
    buffer[1] = (value >> 14) & 0x7f;
    buffer[2] = (value >> 7) & 0x7f;
    buffer[3] = (value >> 0) & 0x7f;

    int i;
    int flag = 0;
    int length = -1;
    for (i = 0; i < 3; i++) {
        if (buffer[i] != 0) {
            flag = 1;
        }
        if (flag) {
            buffer[i] |= 0x80;
        }
        if (length == -1 && buffer[i] >= 0x80) {
            length = 4 - i;
        }
    }

    if (length == -1) {
        length = 1;
    }

    if (length < 4) {
        for (i = 0; i < length; i++) {
            buffer[i] = buffer[4 - length + i];
        }
    }

    return length;
}

// MidiFile::deleteTrack -- remove a track from the MidiFile.
//   Tracks are numbered starting at track 0.
void MidiData::deleteTrack(int aTrack) {
    auto it = _tracks.begin();
    std::advance(it, aTrack);
    _tracks.erase(it);
}

// MidiFile::clear -- make the MIDI file empty with one
//     track with no data in it.
void MidiData::clear() {
    _tracks.clear();
    _tracks.resize(1);
    _timemapvalid = false;
    m_timemap.clear();
    _trackState = TRACK_STATE_SPLIT;
    _timeState = TIME_STATE_ABSOLUTE;
}

// MidiFile::getEvent -- return the event at the given index in the
//    specified track.
MidiEvent& MidiData::getEvent(int aTrack, int anIndex) {
    return (_tracks[aTrack])[anIndex];
}

// MidiFile::getTicksPerQuarterNote -- returns the number of
//   time units that are supposed to occur during a quarternote.
int MidiData::getTicksPerQuarterNote() const {
    return m_ticksPerQuarterNote;
}

int MidiData::getTPQ() const {
    return getTicksPerQuarterNote();
}

int MidiData::getNumberOfEvents(int aTrack) const {
    return _tracks[aTrack].size();
}

void MidiData::setTicksPerQuarterNote(int ticks) {
    m_ticksPerQuarterNote = ticks;
}

void MidiData::setTPQ(int ticks) {
    setTicksPerQuarterNote(ticks);
}

// MidiFile::setMillisecondTicks -- set the ticks per quarter note
//   value to milliseconds.  The format for this specification is
//   highest 8-bits: SMPTE Frame rate (as a negative 2's compliment value).
//   lowest 8-bits: divisions per frame (as a positive number).
//   for millisecond resolution, the SMPTE value is -25, and the
//   frame rate is 40 frame per division.  In hexadecimal, these
//   values are: -25 = 1110,0111 = 0xE7 and 40 = 0010,1000 = 0x28
//   So setting the ticks per quarter note value to 0xE728 will cause
//   delta times in the MIDI file to represent milliseconds.  Calling
//   this function will not change any exiting timestamps, it will
//   only change the meaning of the timestamps.
void MidiData::setMillisecondTicks() {
    m_ticksPerQuarterNote = 0xE728;
}

// MidiFile::sortTracks -- sort all tracks in the MidiFile.
void MidiData::sortTracks() {
    if (_timeState == TIME_STATE_ABSOLUTE) {
        for (int i = 0; i < getNumberOfTracks(); i++) {
            imp::sort(_tracks.at(i));
        }
    } else {
        std::cerr << "Warning: Sorting only allowed in absolute tick mode.";
    }
}

// MidiFile::getTrackCountAsType1 --  Return the number of tracks in the
//    MIDI file.  Returns the size of the events if not in joined state.
//    If in joined state, reads track 0 to find the maximum track
//    value from the original unjoined tracks.
int MidiData::getTrackCountAsType1() {
    if (getTrackState() == TRACK_STATE_JOINED) {
        int output = 0;
        int i;
        for (i = 0; i < (int) _tracks[0].size(); i++) {
            if (getEvent(0, i).track > output) {
                output = getEvent(0, i).track;
            }
        }
        return output + 1;// I think the track values are 0 offset...
    } else {
        return (int) _tracks.size();
    }
}

void MidiData::clearLinks() {
    for (int i = 0; i < getNumberOfTracks(); i++) {
        imp::clearLinks(_tracks[i]);
    }
    m_linkedEventsQ = false;
}

// MidiFile::linearTickInterpolationAtSecond -- return the tick value at the
//    given input time.
double MidiData::linearTickInterpolationAtSecond(double seconds) {
    if (_timemapvalid == 0) {
        buildTimeMap();
        if (_timemapvalid == 0) {
            return -1.0;// something went wrong
        }
    }

    int i;
    double lasttime = m_timemap[m_timemap.size() - 1].seconds;
    // give an error value of -1 if time is out of range of data.
    if (seconds < 0.0) {
        return -1.0;
    }
    if (seconds > m_timemap[m_timemap.size() - 1].seconds) {
        return -1.0;
    }

    // Guess which side of the list is closest to target:
    // Could do a more efficient algorithm since time values are sorted,
    // but good enough for now...
    int startindex = -1;
    if (seconds < lasttime / 2) {
        for (i = 0; i < (int) m_timemap.size(); i++) {
            if (m_timemap[i].seconds > seconds) {
                startindex = i - 1;
                break;
            } else if (m_timemap[i].seconds == seconds) {
                startindex = i;
                break;
            }
        }
    } else {
        for (i = (int) m_timemap.size() - 1; i > 0; i--) {
            if (m_timemap[i].seconds < seconds) {
                startindex = i + 1;
                break;
            } else if (m_timemap[i].seconds == seconds) {
                startindex = i;
                break;
            }
        }
    }

    if (startindex < 0) {
        return -1.0;
    }
    if (startindex >= (int) m_timemap.size() - 1) {
        return -1.0;
    }

    double x1 = m_timemap[startindex].seconds;
    double x2 = m_timemap[startindex + 1].seconds;
    double y1 = m_timemap[startindex].tick;
    double y2 = m_timemap[startindex + 1].tick;
    double xi = seconds;

    return (xi - x1) * ((y2 - y1) / (x2 - x1)) + y1;
}

// MidiFile::linearSecondInterpolationAtTick -- return the time in seconds
//    value at the given input tick time. (Ticks input could be made double).
double MidiData::linearSecondInterpolationAtTick(int ticktime) {
    if (_timemapvalid == 0) {
        buildTimeMap();
        if (_timemapvalid == 0) {
            return -1.0;// something went wrong
        }
    }

    int i;
    double lasttick = m_timemap[m_timemap.size() - 1].tick;
    // give an error value of -1 if time is out of range of data.
    if (ticktime < 0.0) {
        return -1;
    }
    if (ticktime > m_timemap.back().tick) {
        return -1;// don't try to extrapolate
    }

    // Guess which side of the list is closest to target:
    // Could do a more efficient algorithm since time values are sorted,
    // but good enough for now...
    int startindex = -1;
    if (ticktime < lasttick / 2) {
        for (i = 0; i < (int) m_timemap.size(); i++) {
            if (m_timemap[i].tick > ticktime) {
                startindex = i - 1;
                break;
            } else if (m_timemap[i].tick == ticktime) {
                startindex = i;
                break;
            }
        }
    } else {
        for (i = (int) m_timemap.size() - 1; i > 0; i--) {
            if (m_timemap[i].tick < ticktime) {
                startindex = i;
                break;
            } else if (m_timemap[i].tick == ticktime) {
                startindex = i;
                break;
            }
        }
    }

    if (startindex < 0) {
        return -1;
    }
    if (startindex >= (int) m_timemap.size() - 1) {
        return -1;
    }

    if (m_timemap[startindex].tick == ticktime) {
        return m_timemap[startindex].seconds;
    }

    double x1 = m_timemap[startindex].tick;
    double x2 = m_timemap[startindex + 1].tick;
    double y1 = m_timemap[startindex].seconds;
    double y2 = m_timemap[startindex + 1].seconds;
    double xi = ticktime;

    return (xi - x1) * ((y2 - y1) / (x2 - x1)) + y1;
}

// MidiFile::buildTimeMap -- build an index of the absolute tick values
//      found in a MIDI file, and their corresponding time values in
//      seconds, taking into consideration tempo change messages.  If no
//      tempo messages are given (or untill they are given, then the
//      tempo is set to 120 beats per minute).  If SMPTE time code is
//      used, then ticks are actually time values.  So don't build
//      a time map for SMPTE ticks, and just calculate the time in
//      seconds from the tick value (1000 ticks per second SMPTE
//      is the only mode tested (25 frames per second and 40 subframes
//      per frame).
void MidiData::buildTimeMap() {
    // convert the MIDI file to absolute time representation
    // in single track mode (and undo if the MIDI file was not
    // in that state when this function was called.
    //
    int trackstate = getTrackState();
    int timestate = getTickState();

    makeAbsoluteTicks();
    joinTracks();

    int allocsize = getNumberOfEvents(0);
    m_timemap.reserve(allocsize + 10);
    m_timemap.clear();

    TickTime value;

    int lasttick = 0;
    int tickinit = 0;

    int i;
    int tpq = getTicksPerQuarterNote();
    double defaultTempo = 120.0;
    double secondsPerTick = 60.0 / (defaultTempo * tpq);

    double lastsec = 0.0;
    double cursec = 0.0;

    for (i = 0; i < getNumberOfEvents(0); i++) {
        int curtick = getEvent(0, i).tick;
        getEvent(0, i).seconds = cursec;
        if ((curtick > lasttick) || !tickinit) {
            tickinit = 1;

            // calculate the current time in seconds:
            cursec = lastsec + (curtick - lasttick) * secondsPerTick;
            getEvent(0, i).seconds = cursec;

            // store the new tick to second mapping
            value.tick = curtick;
            value.seconds = cursec;
            m_timemap.push_back(value);
            lasttick = curtick;
            lastsec = cursec;
        }

        // update the tempo if needed:
        if (getEvent(0, i).isTempo()) {
            secondsPerTick = getEvent(0, i).getTempoSPT(getTicksPerQuarterNote());
        }
    }

    // reset the states of the tracks or time values if necessary here:
    if (timestate == TIME_STATE_DELTA) {
        makeDeltaTicks();
    }
    if (trackstate == TRACK_STATE_SPLIT) {
        splitTracks();
    }

    _timemapvalid = 1;
}

// MidiFile::ticksearch -- for finding a tick entry in the time map.
int MidiData::ticksearch(const void* A, const void* B) {
    TickTime& a = *((TickTime*) A);
    TickTime& b = *((TickTime*) B);

    if (a.tick < b.tick) {
        return -1;
    } else if (a.tick > b.tick) {
        return 1;
    }
    return 0;
}

// MidiFile::secondsearch -- for finding a second entry in the time map.
int MidiData::secondsearch(const void* A, const void* B) {
    TickTime& a = *((TickTime*) A);
    TickTime& b = *((TickTime*) B);

    if (a.seconds < b.seconds) {
        return -1;
    } else if (a.seconds > b.seconds) {
        return 1;
    }
    return 0;
}

}// namespace imp
