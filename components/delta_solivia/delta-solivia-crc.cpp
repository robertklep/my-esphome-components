// Solivia packet CRC calculation (page 8/9)
#include "delta-solivia-crc.h"

uint16_t delta_solivia_crc(const uint8_t *sop, const uint8_t *eop) {
  const uint8_t *char_ptr = sop;
  uint16_t crc = 0x0000;

  do {
    uint8_t bit_count = 0;
    crc ^= ((*char_ptr) & 0x00ff);
    do {
      if (crc & 0x0001) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    } while ( bit_count++ < 7 );
  } while ( char_ptr++ < eop );
  return crc;
}
