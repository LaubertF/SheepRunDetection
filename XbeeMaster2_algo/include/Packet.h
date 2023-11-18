#include <stdint.h>
#include <Arduino.h>

#ifndef PACKET_H
#define PACKET_H

#define START_DELIMITER 0x7E

#define FRAME_AT_COMMAND 0x08
#define FRAME_AT_COMMAND_QUEUE_REGISTER 0x09
#define FRAME_TRANSMIT_REQUEST 0x10
#define FRAME_EXPLICIT_ADDRESSING_COMMAND 0x11
#define FRAME_REMOTE_AT_COMMAND 0x17
#define FRAME_SECURED_REMOTE_AT_COMMAND 0x18
#define FRAME_AT_COMMAND_RESPONSE 0x88
#define FRAME_MODEM_STATUS 0x8A
#define FRAME_TRANSMIT_STATUS 0x8B
#define FRAME_ROUTE_INFORMATION 0x8D
#define FRAME_AGGREGATE_ADDRESSING_UPDATE 0x8E
#define FRAME_RECEIVE_PACKET 0x90
#define FRAME_EXPLICIT_RX_INDICATOR 0x91
#define FRAME_IO_DATA_SAMPLE_RX_INDICATOR 0x92
#define FRAME_NODE_IDENTIFICATION_INDICATOR 0x95
#define FRAME_REMOTE_AT_COMMAND_RESPONSE 0x97

#define FRAME_DATA_LEN 70   //max 70: max velkost jedneho paxketu (podla datasheetu moze byt az 255 ale nastava fragmentacia, normalne je max 78B bez sifrovania)

enum PacketState{
    INCOMPLETE,
    START_SYMBOL_MISSING,
    CRC_MISSING,
    EMPTY,
    MORE,
    OK
};

class Packet{
public:
    uint8_t startDelimiter;
    uint8_t length[2];
    uint8_t frameID;
    uint8_t frameData[FRAME_DATA_LEN];
    uint8_t crc;

    int16_t remaining;

    Packet();
    void calculateCrc();
    uint16_t calculateLength();

    //======= SETTERS - GETTERS ========
    uint8_t getStartDelimiter();
    void setLength(uint16_t len16);
    void setLength(uint8_t *len8, uint8_t size);
    uint16_t getLength();
    void setFrameType(uint8_t frameType);
    uint8_t getFrameType();
    uint8_t getCrc();

    //====== POMOCNE METODY ======
    uint16_t convert8To16(uint8_t *ui8);
};
#endif //PACKET_H