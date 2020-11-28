/**
 * @copyright 1999-2020, Craig Stuart Sapp under BSD-2 license
 * @copyright 2020-2020, Christoph Fröhner under BSD-2 license
 */

#pragma once

#include <string>
#include <vector>

#include <iomidipp/Utils.h>

namespace imp {

// TODO move make functions into factory
// TODO identify util functions that do not use state of MidiMessage

class MidiMessage {
public:
    using Content = std::vector<uchar>;
    MidiMessage() = default;

    explicit MidiMessage(int command);

    MidiMessage(int command, int p1);

    MidiMessage(int command, int p1, int p2);

    explicit MidiMessage(Content content);

    // data access convenience functions (returns -1 if not present):
    [[nodiscard]] int getP0() const;

    [[nodiscard]] int getP1() const;

    [[nodiscard]] int getP2() const;

    [[nodiscard]] int getP3() const;

    void setP0(int value);

    void setP1(int value);

    void setP2(int value);

    void setP3(int value);

    uchar& operator[](int i) {
        return content[i];
    }

    [[nodiscard]] std::size_t getSize() const;

    int resizeToCommand();

    // note-message convenience functions:
    int getKeyNumber() const;

    int getVelocity() const;

    void setKeyNumber(int value);

    void setVelocity(int value);

    void setSpelling(int base7, int accidental);

    void getSpelling(int& base7, int& accidental);

    // controller-message convenience functions:
    int getControllerNumber() const;

    int getControllerValue() const;

    int getCommandNibble() const;

    int getCommandByte() const;

    int getChannelNibble() const;

    int getChannel() const;

    void setCommandByte(int value);

    void setCommand(int value);

    void setCommand(int value, int p1);

    void setCommand(int value, int p1, int p2);

    void setCommandNibble(int value);

    void setChannelNibble(int value);

    void setChannel(int value);

    void setParameters(int p1, int p2);

    void setParameters(int p1);

    void setContent(const std::vector<uchar>& otherContent);

    // message-type convenience functions:
    bool isMetaMessage() const;

    bool isMeta() const;

    bool isNote() const;

    bool isNoteOff() const;

    bool isNoteOn() const;

    bool isAftertouch() const;

    bool isController() const;

    bool isSustain() const;// controller 64
    bool isSustainOn() const;

    bool isSustainOff() const;

    bool isSoft() const;// controller 67
    bool isSoftOn() const;

    bool isSoftOff() const;

    bool isPatchChange() const;

    bool isTimbre() const;

    bool isPressure() const;

    bool isPitchbend() const;

    bool isEmpty() const;// see MidiFile::removeEmpties()

    // helper functions to create various MidiMessages:
    void makeNoteOn(int channel, int key, int velocity);

    void makeNoteOff(int channel, int key, int velocity);

    void makeNoteOff(int channel, int key);

    void makeNoteOff();

    void makePatchChange(int channel, int patchnum);

    void makeTimbre(int channel, int patchnum);

    void makeController(int channel, int num, int value);

    // helper functions to create various continuous controller messages:
    void makeSustain(int channel, int value);

    void makeSustainPedal(int channel, int value);

    void makeSustainOn(int channel);

    void makeSustainPedalOn(int channel);

    void makeSustainOff(int channel);

    void makeSustainPedalOff(int channel);

    // meta-message creation and helper functions:
    void makeMetaMessage(int mnum, const Content& data);

    void makeText(const Content& name);

    void makeCopyright(const Content& text);

    void makeTrackName(const Content& name);

    void makeInstrumentName(const Content& name);

    void makeLyric(const Content& text);

    void makeMarker(const Content& text);

    void makeCue(const Content& text);

    void makeTimeSignature(int top, int bottom,
                           int clocksPerClick = 24,
                           int num32dsPerQuarter = 8);

    void makeTempo(double tempo) { setTempo(tempo); }

    int getTempoMicro() const;

    int getTempoMicroseconds() const;

    double getTempoSeconds() const;

    double getTempoBPM() const;

    double getTempoTPS(int tpq) const;

    double getTempoSPT(int tpq) const;

    int getMetaType() const;

    bool isText() const;

    bool isCopyright() const;

    bool isTrackName() const;

    bool isInstrumentName() const;

    bool isLyricText() const;

    bool isMarkerText() const;

    bool isTempo() const;

    bool isTimeSignature() const;

    bool isKeySignature() const;

    bool isEndOfTrack() const;

    std::string getMetaContent();

    void setMetaContent(std::vector<uchar> const& metaContent);

    void setTempo(double tempo);

    void setTempoMicroseconds(int microseconds);

    void setMetaTempo(double tempo);

private:
    std::vector<uchar> content = {};
};

}// namespace imp
