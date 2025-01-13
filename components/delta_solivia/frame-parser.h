#pragma once

#include <memory>
#include "frame-parser-base.h"
#include "frame-parser-variant-15.h"

namespace esphome {
namespace delta_solivia {

using FrameParserInstance = std::unique_ptr<FrameParserBase>;

class FrameParser {
public:
  // pick the correct parser for this variant
  static FrameParserInstance get_parser(uint8_t variant) {
    if (variant == 15 || variant == 18 || variant == 19 || variant == 20 || variant == 31 ||
        variant == 34 || variant == 35 || variant == 36 || variant == 38 || variant == 39 ||
        variant == 55 || variant == 58 || variant == 59 || variant == 60) {
      return FrameParserInstance(new FrameParserVariant15());
    }
    return nullptr;
  }
};

}
}
