/**
 * @copyright 1999-2020, Craig Stuart Sapp under BSD-2 license
 * @copyright 2020-2020, Christoph Fröhner under BSD-2 license
 */

#pragma once

#include <iomidipp/MidiData.h>

namespace imp::File {

    MidiData read(const std::string &filename);

    //bool read(std::istream &instream);

    bool write(const std::string &filename, MidiData const& data);

    //bool write(std::ostream &out);

//    bool writeHex(const std::string &filename,
//                  int width = 25);
//
//    bool writeHex(std::ostream &out,
//                  int width = 25);

}