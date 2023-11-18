#include <stdio.h>

#ifndef TWILIGHT_H
#define TWILIGHT_H

#define TWI_FREQMAX 230400    //rychlost CLK [Hz]
#define TWI_FREQSTD 100000    //rychlost CLK [Hz]

// TWI Status Codes
#define TWI_START_SENT			0x08 // Start sent
#define TWI_REP_START_SENT		0x10 // Repeated Start sent
// Master Transmitter Mode
#define TWI_MT_SLAW_ACK			0x18 // SLA+W sent and ACK received
#define TWI_MT_SLAW_NACK		0x20 // SLA+W sent and NACK received
#define TWI_MT_DATA_ACK			0x28 // DATA sent and ACK received
#define TWI_MT_DATA_NACK		0x30 // DATA sent and NACK received
// Master Receiver Mode
#define TWI_MR_SLAR_ACK			0x40 // SLA+R sent, ACK received
#define TWI_MR_SLAR_NACK		0x48 // SLA+R sent, NACK received
#define TWI_MR_DATA_ACK			0x50 // Data received, ACK returned
#define TWI_MR_DATA_NACK		0x58 // Data received, NACK returned
// Miscellaneous States
#define TWI_LOST_ARBIT			0x38 // Arbitration has been lost
#define TWI_NO_RELEVANT_INFO	0xF8 // No relevant information available
#define TWI_ILLEGAL_START_STOP	0x00 // Illegal START or STOP condition has been detected
#define TWI_SUCCESS				0xFF // Successful transfer, this state is impossible from TWSR as bit2 is 0 and read only

enum TWIState{
    ERROR,
    SUCCESS,
    MAG_SENS_OVERFLOW
};

class TWILight
{
private:
public:
    TWILight();
    void TWIInit();
    void TWIStart();
    void TWIStop();
    void TWIWrite(uint8_t u8data);
    uint8_t TWIReadACK();
    uint8_t TWIReadNACK();
    uint8_t TWIGetStatus();
    
    TWIState TWIReadByte(uint8_t slaveAddress, uint8_t regAddr, uint8_t *u8data);
    TWIState TWIReadMultipleBytes(uint8_t slaveAddress, uint8_t regAddr, uint8_t *u8data, uint8_t length);
    TWIState TWIReadMultipleBytesFrom(uint8_t slaveAddress, uint8_t regAddr, uint8_t *u8data, uint8_t u8dataIndexStart, uint8_t count, uint8_t maxLength);

    TWIState TWIWriteByte(uint8_t slaveAddress, uint8_t regAddr, uint8_t u8data);
    TWIState TWIWriteMultipleBytes(uint8_t slaveAddress, uint8_t regAddr, uint8_t *u8data, uint8_t length);
    TWIState TWIWriteMultipleBytesFrom(uint8_t slaveAddress, uint8_t regAddr, uint8_t *u8data, uint8_t u8dataIndexStart, uint8_t count, uint8_t maxLength);
};

#endif //TWILIGHT_H


