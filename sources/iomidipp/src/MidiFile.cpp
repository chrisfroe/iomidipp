/**
 * @copyright 1999-2020, Craig Stuart Sapp under BSD-2 license
 * @copyright 2020-2020, Christoph Fröhner under BSD-2 license
 */

#include <iomidipp/MidiFile.h>
#include <iostream>

namespace imp::File {

// MidiFile::readLittleEndian4Bytes -- Read four bytes which are in
//      little-endian order (smallest byte is first).  Then flip
//      the order of the bytes to create the return value.
ulong readLittleEndian4Bytes(std::istream& input) {
    uchar buffer[4] = {0};
    input.read((char*) buffer, 4);
    if (input.eof()) {
        std::cerr << "Error: unexpected end of file." << std::endl;
        return 0;
    }
    return buffer[3] | (buffer[2] << 8) | (buffer[1] << 16) | (buffer[0] << 24);
}

// MidiFile::readLittleEndian2Bytes -- Read two bytes which are in
//       little-endian order (smallest byte is first).  Then flip
//       the order of the bytes to create the return value.
ushort readLittleEndian2Bytes(std::istream& input) {
    uchar buffer[2] = {0};
    input.read((char*) buffer, 2);
    if (input.eof()) {
        std::cerr << "Error: unexpected end of file." << std::endl;
        return 0;
    }
    return buffer[1] | (buffer[0] << 8);
}

// MidiFile::readByte -- Read one byte from input stream.  Set
//     fail status error if there was a problem (calling function
//     has to check this status for an error after reading).
uchar readByte(std::istream& input) {
    uchar buffer[1] = {0};
    input.read((char*) buffer, 1);
    if (input.eof()) {
        std::cerr << "Error: unexpected end of file." << std::endl;
        return 0;
    }
    return buffer[0];
}

std::ostream& writeLittleEndianUShort(std::ostream& out, ushort value) {
    union {
        char bytes[2];
        ushort us;
    } data{};
    data.us = value;
    out << data.bytes[0];
    out << data.bytes[1];
    return out;
}

std::ostream& writeBigEndianUShort(std::ostream& out, ushort value) {
    union {
        char bytes[2];
        ushort us;
    } data{};
    data.us = value;
    out << data.bytes[1];
    out << data.bytes[0];
    return out;
}

std::ostream& writeLittleEndianShort(std::ostream& out, short value) {
    union {
        char bytes[2];
        short s;
    } data{};
    data.s = value;
    out << data.bytes[0];
    out << data.bytes[1];
    return out;
}

std::ostream& writeBigEndianShort(std::ostream& out, short value) {
    union {
        char bytes[2];
        short s;
    } data{};
    data.s = value;
    out << data.bytes[1];
    out << data.bytes[0];
    return out;
}

std::ostream& writeLittleEndianULong(std::ostream& out, ulong value) {
    union {
        char bytes[4];
        ulong ul;
    } data{};
    data.ul = value;
    out << data.bytes[0];
    out << data.bytes[1];
    out << data.bytes[2];
    out << data.bytes[3];
    return out;
}

std::ostream& writeBigEndianULong(std::ostream& out, ulong value) {
    union {
        char bytes[4];
        long ul;
    } data{};
    data.ul = value;
    out << data.bytes[3];
    out << data.bytes[2];
    out << data.bytes[1];
    out << data.bytes[0];
    return out;
}

std::ostream& writeLittleEndianLong(std::ostream& out, long value) {
    union {
        char bytes[4];
        long l;
    } data{};
    data.l = value;
    out << data.bytes[0];
    out << data.bytes[1];
    out << data.bytes[2];
    out << data.bytes[3];
    return out;
}

std::ostream& writeBigEndianLong(std::ostream& out, long value) {
    union {
        char bytes[4];
        long l;
    } data{};
    data.l = value;
    out << data.bytes[3];
    out << data.bytes[2];
    out << data.bytes[1];
    out << data.bytes[0];
    return out;
}

std::ostream& writeBigEndianFloat(std::ostream& out, float value) {
    union {
        char bytes[4];
        float f;
    } data{};
    data.f = value;
    out << data.bytes[3];
    out << data.bytes[2];
    out << data.bytes[1];
    out << data.bytes[0];
    return out;
}

std::ostream& writeLittleEndianFloat(std::ostream& out, float value) {
    union {
        char bytes[4];
        float f;
    } data{};
    data.f = value;
    out << data.bytes[0];
    out << data.bytes[1];
    out << data.bytes[2];
    out << data.bytes[3];
    return out;
}

std::ostream& writeBigEndianDouble(std::ostream& out, double value) {
    union {
        char bytes[8];
        double d;
    } data{};
    data.d = value;
    out << data.bytes[7];
    out << data.bytes[6];
    out << data.bytes[5];
    out << data.bytes[4];
    out << data.bytes[3];
    out << data.bytes[2];
    out << data.bytes[1];
    out << data.bytes[0];
    return out;
}

std::ostream& writeLittleEndianDouble(std::ostream& out, double value) {
    union {
        char bytes[8];
        double d;
    } data{};
    data.d = value;
    out << data.bytes[0];
    out << data.bytes[1];
    out << data.bytes[2];
    out << data.bytes[3];
    out << data.bytes[4];
    out << data.bytes[5];
    out << data.bytes[6];
    out << data.bytes[7];
    return out;
}

// MidiFile::unpackVLV -- converts a VLV value to an unsigned long value.
//     The bytes a, b, c, d, e are in big-endian order (the order they would
//     be read out of the MIDI file).
// default values: a = b = c = d = 0;
ulong unpackVLV(uchar a = 0, uchar b = 0, uchar c = 0, uchar d = 0, uchar e = 0) {
    uchar bytes[5] = {a, b, c, d, e};
    int count = 0;
    while ((count < 5) && (bytes[count] > 0x7f)) {
        count++;
    }
    count++;
    if (count >= 6) {
        std::cerr << "VLV number is too large" << std::endl;
        return 0;
    }

    ulong output = 0;
    for (int i = 0; i < count; i++) {
        output = output << 7;
        output = output | (bytes[i] & 0x7f);
    }

    return output;
}

// MidiFile::readVLValue -- The VLV value is expected to be unpacked into
//   a 4-byte integer no greater than 0x0fffFFFF, so a VLV value up to
//   4-bytes in size (FF FF FF 7F) will only be considered.  Longer
//   VLV values are not allowed in standard MIDI files, so the extract
//   delta time would be truncated and the extra byte(s) will be parsed
//   incorrectly as a MIDI command.
ulong readVLValue(std::istream& input) {
    uchar b[5] = {0};

    for (int i = 0; i < 5; i++) {
        b[i] = readByte(input);
        if (b[i] < 0x80) {
            break;
        }
    }

    return unpackVLV(b[0], b[1], b[2], b[3], b[4]);
}

// MidiFile::extractMidiData -- Extract MIDI data from input
//    stream.  Return value is 0 if failure; otherwise, returns 1.
int extractMidiData(std::istream& input, std::vector<uchar>& array,
                    uchar& runningCommand) {
    int character;
    uchar byte;
    array.clear();
    int runningQ;

    character = input.get();
    if (character == EOF) {
        std::cerr << "Error: unexpected end of file." << std::endl;
        return 0;
    } else {
        byte = (uchar) character;
    }

    if (byte < 0x80) {
        runningQ = 1;
        if (runningCommand == 0) {
            std::cerr << "Error: running command with no previous command" << std::endl;
            return 0;
        }
        if (runningCommand >= 0xf0) {
            std::cerr << "Error: running status not permitted with meta and sysex"
                      << " event." << std::endl;
            std::cerr << "Byte is 0x" << std::hex << (int) byte << std::dec << std::endl;
            return 0;
        }
    } else {
        runningCommand = byte;
        runningQ = 0;
    }

    array.push_back(runningCommand);
    if (runningQ) {
        array.push_back(byte);
    }

    switch (runningCommand & 0xf0) {
        case 0x80:// note off (2 more bytes)
        case 0x90:// note on (2 more bytes)
        case 0xA0:// aftertouch (2 more bytes)
        case 0xB0:// cont. controller (2 more bytes)
        case 0xE0:// pitch wheel (2 more bytes)
            byte = readByte(input);
            if (byte > 0x7f) {
                std::cerr << "MIDI data byte too large: " << (int) byte << std::endl;
                return {};
            }
            array.push_back(byte);
            if (!runningQ) {
                byte = readByte(input);
                if (byte > 0x7f) {
                    std::cerr << "MIDI data byte too large: " << (int) byte << std::endl;
                    return {};
                }
                array.push_back(byte);
            }
            break;
        case 0xC0:// patch change (1 more byte)
        case 0xD0:// channel pressure (1 more byte)
            if (!runningQ) {
                byte = readByte(input);
                if (byte > 0x7f) {
                    std::cerr << "MIDI data byte too large: " << (int) byte << std::endl;
                    return {};
                }
                array.push_back(byte);
            }
            break;
        case 0xF0:
            switch (runningCommand) {
                case 0xff:// meta event
                {
                    if (!runningQ) {
                        byte = readByte(input);// meta type
                        array.push_back(byte);
                    }
                    ulong length = 0;
                    uchar byte1 = 0;
                    uchar byte2 = 0;
                    uchar byte3 = 0;
                    uchar byte4 = 0;
                    byte1 = readByte(input);
                    array.push_back(byte1);
                    if (byte1 >= 0x80) {
                        byte2 = readByte(input);
                        array.push_back(byte2);
                        if (byte2 > 0x80) {
                            byte3 = readByte(input);
                            array.push_back(byte3);
                            if (byte3 >= 0x80) {
                                byte4 = readByte(input);
                                array.push_back(byte4);
                                if (byte4 >= 0x80) {
                                    std::cerr << "Error: cannot handle large VLVs" << std::endl;
                                    return {};
                                } else {
                                    length = unpackVLV(byte1, byte2, byte3, byte4);
                                }
                            } else {
                                length = unpackVLV(byte1, byte2, byte3);
                            }
                        } else {
                            length = unpackVLV(byte1, byte2);
                        }
                    } else {
                        length = byte1;
                    }
                    for (int j = 0; j < (int) length; j++) {
                        byte = readByte(input);// meta type
                        array.push_back(byte);
                    }
                } break;

                    // The 0xf0 and 0xf7 meta commands deal with system-exclusive
                    // messages. 0xf0 is used to either start a message or to store
                    // a complete message.  The 0xf0 is part of the outgoing MIDI
                    // bytes.  The 0xf7 message is used to send arbitrary bytes,
                    // typically the middle or ends of system exclusive messages.  The
                    // 0xf7 byte at the start of the message is not part of the
                    // outgoing raw MIDI bytes, but is kept in the MidiFile message
                    // to indicate a raw MIDI byte message (typically a partial
                    // system exclusive message).
                case 0xf7:// Raw bytes. 0xf7 is not part of the raw
                        // bytes, but are included to indicate
                        // that this is a raw byte message.
                case 0xf0:// System Exclusive message
                {         // (complete, or start of message).
                    int length = (int) readVLValue(input);
                    for (int i = 0; i < length; i++) {
                        byte = readByte(input);
                        array.push_back(byte);
                    }
                } break;

                    // other "F" MIDI commands are not expected, but can be
                    // handled here if they exist.
            }
            break;
        default:
            std::cout << "Error reading midifile" << std::endl;
            std::cout << "Command byte was " << (int) runningCommand << std::endl;
            return 0;
    }
    return 1;
}

MidiData read(std::istream& input) {
    if (input.peek() != 'M') {
        throw std::runtime_error("Bad MIDI data input");
    }

    int character;
    // uchar  buffer[123456] = {0};
    ulong longdata;
    ushort shortdata;

    // Read the MIDI header (4 bytes of ID, 4 byte data size,
    // anticipated 6 bytes of data.

    character = input.get();
    if (character == EOF) {
        std::cerr << "unexpected end of file." << std::endl;
        std::cerr << "Expecting 'M' at first byte, but found nothing." << std::endl;
        return {};
    } else if (character != 'M') {
        std::cerr << "is not a MIDI file" << std::endl;
        std::cerr << "Expecting 'M' at first byte but got '"
                  << (char) character << "'" << std::endl;
        return {};
    }

    character = input.get();
    if (character == EOF) {
        std::cerr << "unexpected end of file." << std::endl;
        std::cerr << "Expecting 'T' at second byte, but found nothing." << std::endl;
        return {};
    } else if (character != 'T') {
        std::cerr << "is not a MIDI file" << std::endl;
        std::cerr << "Expecting 'T' at second byte but got '"
                  << (char) character << "'" << std::endl;
        return {};
    }

    character = input.get();
    if (character == EOF) {
        std::cerr << "unexpected end of file." << std::endl;
        std::cerr << "Expecting 'h' at third byte, but found nothing." << std::endl;
        return {};
    } else if (character != 'h') {
        std::cerr << "not a MIDI file" << std::endl;
        std::cerr << "Expecting 'h' at third byte but got '"
                  << (char) character << "'" << std::endl;
        return {};
    }

    character = input.get();
    if (character == EOF) {
        std::cerr << "unexpected end of file." << std::endl;
        std::cerr << "Expecting 'd' at fourth byte, but found nothing." << std::endl;
        return {};
    } else if (character != 'd') {
        std::cerr << "not a MIDI file" << std::endl;
        std::cerr << "Expecting 'd' at fourth byte but got '"
                  << (char) character << "'" << std::endl;
        return {};
    }

    // read header size (allow larger header size?)
    longdata = readLittleEndian4Bytes(input);
    if (longdata != 6) {
        std::cerr << "Not a MIDI 1.0 Standard MIDI file." << std::endl;
        std::cerr << "The header size is " << longdata << " bytes." << std::endl;
        return {};
    }

    // Header parameter #1: format type
    int type;
    shortdata = readLittleEndian2Bytes(input);
    switch (shortdata) {
        case 0:
            type = 0;
            break;
        case 1:
            type = 1;
            break;
        case 2:
            // Type-2 MIDI files should probably be allowed as well,
            // but I have never seen one in the wild to test with.
        default:
            std::cerr << "Error: cannot handle a type-" << shortdata
                      << " MIDI file" << std::endl;
            return {};
    }

    // Header parameter #2: track count
    int n;
    shortdata = readLittleEndian2Bytes(input);
    if (type == 0 && shortdata != 1) {
        std::cerr << "Error: Type 0 MIDI file can only contain one track" << std::endl;
        std::cerr << "Instead track count is: " << shortdata << std::endl;
        return {};
    } else {
        n = shortdata;
    }

    MidiData data;
    data.tracks().resize(n);

    // Header parameter #3: Ticks per quarter note
    shortdata = readLittleEndian2Bytes(input);
    if (shortdata >= 0x8000) {
        int framespersecond = 255 - ((shortdata >> 8) & 0x00ff) + 1;
        int subframes = shortdata & 0x00ff;
        switch (framespersecond) {
            case 25:
                framespersecond = 25;
                break;
            case 24:
                framespersecond = 24;
                break;
            case 29:
                framespersecond = 29;
                break;// really 29.97 for color television
            case 30:
                framespersecond = 30;
                break;
            default:
                std::cerr << "Warning: unknown FPS: " << framespersecond << std::endl;
                std::cerr << "Using non-standard FPS: " << framespersecond << std::endl;
        }
        data.setTicksPerQuarterNote(framespersecond * subframes);

        // std::cerr << "SMPTE ticks: " << m_ticksPerQuarterNote << " ticks/sec" << std::endl;
        // std::cerr << "SMPTE frames per second: " << framespersecond << std::endl;
        // std::cerr << "SMPTE subframes per frame: " << subframes << std::endl;
    } else {
        data.setTicksPerQuarterNote(shortdata);
    }

    // now read individual tracks:
    uchar runningCommand;
    MidiEvent event;
    std::vector<uchar> bytes;
    int xstatus;
    // int barline;

    for (int i = 0; i < n; i++) {
        runningCommand = 0;

        // std::cout << "\nReading Track: " << i + 1 << flush;

        // read track header...

        character = input.get();
        if (character == EOF) {
            std::cerr << "unexpected end of file." << std::endl;
            std::cerr << "Expecting 'M' at first byte in track, but found nothing."
                      << std::endl;
            return {};
        } else if (character != 'M') {
            std::cerr << "is not a MIDI file" << std::endl;
            std::cerr << "Expecting 'M' at first byte in track but got '"
                      << (char) character << "'" << std::endl;
            return {};
        }

        character = input.get();
        if (character == EOF) {
            std::cerr << "unexpected end of file." << std::endl;
            std::cerr << "Expecting 'T' at second byte in track, but found nothing."
                      << std::endl;
            return {};
        } else if (character != 'T') {
            std::cerr << " is not a MIDI file" << std::endl;
            std::cerr << "Expecting 'T' at second byte in track but got '"
                      << (char) character << "'" << std::endl;
            return {};
        }

        character = input.get();
        if (character == EOF) {
            std::cerr << "unexpected end of file." << std::endl;
            std::cerr << "Expecting 'r' at third byte in track, but found nothing."
                      << std::endl;
            return {};
        } else if (character != 'r') {
            std::cerr << "is not a MIDI file" << std::endl;
            std::cerr << "Expecting 'r' at third byte in track but got '"
                      << (char) character << "'" << std::endl;
            return {};
        }

        character = input.get();
        if (character == EOF) {
            std::cerr << " unexpected end of file." << std::endl;
            std::cerr << "Expecting 'k' at fourth byte in track, but found nothing."
                      << std::endl;
            return {};
        } else if (character != 'k') {
            std::cerr << "File is not a MIDI file" << std::endl;
            std::cerr << "Expecting 'k' at fourth byte in track but got '"
                      << (char) character << "'" << std::endl;
            return {};
        }

        // Now read track chunk size and throw it away because it is
        // not really necessary since the track MUST end with an
        // end of track meta event, and many MIDI files found in the wild
        // do not correctly give the track size.
        longdata = readLittleEndian4Bytes(input);

        // set the size of the track allocation so that it might
        // approximately fit the data.
        data.tracks()[i].reserve((int) longdata / 2);
        data.tracks()[i].clear();

        // process the track
        int absticks = 0;
        // barline = 1;
        while (!input.eof()) {
            longdata = readVLValue(input);
            //std::cout << "ticks = " << longdata << std::endl;
            absticks += longdata;
            xstatus = extractMidiData(input, bytes, runningCommand);
            if (xstatus == 0) {
                return {};
            }
            event.setContent(bytes);
            //std::cout << "command = " << std::hex << (int)event.data[0] << std::dec << std::endl;
            if (bytes[0] == 0xff && (bytes[1] == 1 ||
                                     bytes[1] == 2 || bytes[1] == 3 || bytes[1] == 4)) {
                // mididata.push_back('\0');
                // std::cout << '\t';
                // for (int m=0; m<event.data[2]; m++) {
                //    std::cout << event.data[m+3];
                // }
                // std::cout.flush();
            } else if (bytes[0] == 0xff && bytes[1] == 0x2f) {
                // end of track message
                // uncomment out the following three lines if you don't want
                // to see the end of track message (which is always required,
                // and added automatically when a MIDI is written.
                event.tick = absticks;
                event.track = i;
                data.tracks()[i].push_back(event);
                break;
            }

            if (bytes[0] != 0xff && bytes[0] != 0xf0) {
                event.tick = absticks;
                event.track = i;
                data.tracks()[i].push_back(event);
            } else {
                event.tick = absticks;
                event.track = i;
                data.tracks()[i].push_back(event);
            }
        }
    }

    data.setTimeState(TIME_STATE_ABSOLUTE);
    data.markSequence();
    return data;
}

// MidiFile::read -- Parse a Standard MIDI File and store its contents
//      in the object.
MidiData read(const std::string& filename) {
    std::ifstream input;
    input.open(filename.c_str(), std::ios::binary | std::ios::in);

    if (!input.is_open()) {
        throw std::runtime_error("file could not be opened");
    }

    return read(input);
}

// MidiFile::writeVLValue -- write a number to the midifile
//    as a variable length value which segments a file into 7-bit
//    values and adds a contination bit to each.  Maximum size of input
//    aValue is 0x0FFFffff.
void writeVLValue(long aValue, std::vector<uchar>& outdata) {
    uchar bytes[4] = {0};

    if ((unsigned long) aValue >= (1 << 28)) {
        std::cerr << "Error: number too large to convert to VLV" << std::endl;
        aValue = 0x0FFFffff;
    }

    bytes[0] = (uchar)(((ulong) aValue >> 21) & 0x7f);// most significant 7 bits
    bytes[1] = (uchar)(((ulong) aValue >> 14) & 0x7f);
    bytes[2] = (uchar)(((ulong) aValue >> 7) & 0x7f);
    bytes[3] = (uchar)(((ulong) aValue) & 0x7f);// least significant 7 bits

    int start = 0;
    while ((start < 4) && (bytes[start] == 0)) start++;

    for (int i = start; i < 3; i++) {
        bytes[i] = bytes[i] | 0x80;
        outdata.push_back(bytes[i]);
    }
    outdata.push_back(bytes[3]);
}

// ostream version of MidiFile::write().
bool write(std::ostream& out, MidiData& data) {
    int oldTimeState = data.getTickState();
    if (oldTimeState == TIME_STATE_ABSOLUTE) {
        data.makeDeltaTicks();
    }

    // write the header of the Standard MIDI File
    char ch;
    // 1. The characters "MThd"
    ch = 'M';
    out << ch;
    ch = 'T';
    out << ch;
    ch = 'h';
    out << ch;
    ch = 'd';
    out << ch;

    // 2. write the size of the header (always a "6" stored in unsigned long
    //    (4 bytes).
    ulong longdata = 6;
    writeBigEndianULong(out, longdata);

    // 3. MIDI file format, type 0, 1, or 2
    ushort shortdata;
    shortdata = (data.getNumberOfTracks() == 1) ? 0 : 1;
    writeBigEndianUShort(out, shortdata);

    // 4. write out the number of tracks.
    shortdata = data.getNumberOfTracks();
    writeBigEndianUShort(out, shortdata);

    // 5. write out the number of ticks per quarternote. (avoiding SMTPE for now)
    shortdata = data.getTicksPerQuarterNote();
    writeBigEndianUShort(out, shortdata);

    // now write each track.
    std::vector<uchar> trackdata;
    uchar endoftrack[4] = {0, 0xff, 0x2f, 0x00};
    int i, j, k;
    int size;
    for (i = 0; i < data.getNumberOfTracks(); i++) {
        trackdata.reserve(123456);// make the track data larger than
        // expected data input
        trackdata.clear();
        for (j = 0; j < (int) data.tracks()[i].size(); j++) {
            if (data.tracks()[i][j].isEmpty()) {
                // Don't write empty m_events (probably a delete message).
                continue;
            }
            if (data.tracks()[i][j].isEndOfTrack()) {
                // Suppress end-of-track meta messages (one will be added
                // automatically after all track data has been written).
                continue;
            }
            writeVLValue(data.tracks()[i][j].tick, trackdata);
            if ((data.tracks()[i][j].getCommandByte() == 0xf0) ||
                (data.tracks()[i][j].getCommandByte() == 0xf7)) {
                // 0xf0 == Complete sysex message (0xf0 is part of the raw MIDI).
                // 0xf7 == Raw byte message (0xf7 not part of the raw MIDI).
                // Print the first byte of the message (0xf0 or 0xf7), then
                // print a VLV length for the rest of the bytes in the message.
                // In other words, when creating a 0xf0 or 0xf7 MIDI message,
                // do not insert the VLV byte length yourself, as this code will
                // do it for you automatically.
                trackdata.push_back((data.tracks()[i])[j][0]);// 0xf0 or 0xf7;
                writeVLValue(((int) (data.tracks()[i])[j].getSize()) - 1, trackdata);
                for (k = 1; k < (int) (data.tracks()[i])[j].getSize(); k++) {
                    trackdata.push_back((data.tracks()[i])[j][k]);
                }
            } else {
                // non-sysex type of message, so just output the
                // bytes of the message:
                for (k = 0; k < (int) (data.tracks()[i])[j].getSize(); k++) {
                    trackdata.push_back((data.tracks()[i])[j][k]);
                }
            }
        }
        size = (int) trackdata.size();
        if ((size < 3) || !((trackdata[size - 3] == 0xff) && (trackdata[size - 2] == 0x2f))) {
            trackdata.push_back(endoftrack[0]);
            trackdata.push_back(endoftrack[1]);
            trackdata.push_back(endoftrack[2]);
            trackdata.push_back(endoftrack[3]);
        }

        // now ready to write to MIDI file.

        // first write the track ID marker "MTrk":
        ch = 'M';
        out << ch;
        ch = 'T';
        out << ch;
        ch = 'r';
        out << ch;
        ch = 'k';
        out << ch;

        // A. write the size of the MIDI data to follow:
        longdata = (int) trackdata.size();
        writeBigEndianULong(out, longdata);

        // B. write the actual data
        out.write((char*) trackdata.data(), trackdata.size());
    }

    if (oldTimeState == TIME_STATE_ABSOLUTE) {
        data.makeAbsoluteTicks();
    }

    return true;
}

// MidiFile::write -- write a standard MIDI file to a file or an output
//    stream.
bool write(const std::string& filename, MidiData& data) {
    std::fstream output(filename.c_str(), std::ios::binary | std::ios::out);

    if (!output.is_open()) {
        std::cerr << "Error: could not write: " << filename << std::endl;
        return false;
    }
    bool status = write(output, data);
    output.close();
    return status;
}

//// MidiFile::writeHex -- print the Standard MIDI file as a list of
////    ASCII Hex bytes, formatted 25 to a line by default, and
////    two digits for each hex byte code.  If the input width is 0,
////    then don't wrap lines.
////
////  default value: width=25
//bool writeHex(const std::string &filename, int width) {
//    std::fstream output(filename.c_str(), std::ios::out);
//    if (!output.is_open()) {
//        std::cerr << "Error: could not write: " << filename << std::endl;
//        return false;
//    }
//    _status = writeHex(output, width);
//    output.close();
//    return _status;
//}
//
//// ostream version of MidiFile::writeHex().
//bool writeHex(std::ostream &out, int width) {
//    std::stringstream tempstream;
//    MidiFile::write(tempstream);
//    int len = (int) tempstream.str().length();
//    int wordcount = 1;
//    int linewidth = width >= 0 ? width : 25;
//    for (int i = 0; i < len; i++) {
//        auto value = (unsigned char) tempstream.str()[i];
//        out << std::hex << std::setw(2) << std::setfill('0') << value;
//        if (linewidth) {
//            if (i < len - 1) {
//                out << ((wordcount % linewidth) ? ' ' : '\n');
//            }
//            wordcount++;
//        } else {
//            // print with no line breaks
//            if (i < len - 1) {
//                out << ' ';
//            }
//        }
//    }
//    if (linewidth) {
//        out << '\n';
//    }
//    return true;
//}

}// namespace imp::File