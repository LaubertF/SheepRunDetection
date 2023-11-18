#include "utils.h"

// konvertuje 1 cislo uint32 na pole cisel uint8
void convertLongToBytes(uint32_t number, uint8_t *out){
  out[0] = (number >> 24) & 0xFF; //MSB
  out[1] = (number >> 16) & 0xFF;
  out[2] = (number >> 8) & 0xFF;
  out[3] = number & 0xFF;         //LSB
}

// konvertuje 1 cislo uint16 na pole cisel uint8
void convertUint16ToUint8(uint16_t number, uint8_t *out){
  out[0] = (number >> 8) & 0xFF;  //MSB
  out[1] = number & 0xFF;         //LSB
}