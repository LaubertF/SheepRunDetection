#include "twiLight.h"
#include <Arduino.h>

TWILight::TWILight()
{
}

void TWILight::TWIInit () {
	// Set pre-scalers (no pre-scaling)
	TWSR = 0;
	// Set bit rate
	TWBR = ((F_CPU / TWI_FREQSTD) - 16) / 2;
	// Enable TWI
	TWCR = (1 << TWEN);
}

void TWILight::TWIStart () {
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);   //caka sa na odvisielania START bitu
}

void TWILight::TWIStop () {
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void TWILight::TWIWrite(uint8_t u8data){
    TWDR = u8data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);   //caka sa na odvisielanie dat v TWDR
}
uint8_t TWILight::TWIReadACK(){
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
    while ((TWCR & (1<<TWINT)) == 0);   //MASTER posle ACK, Slave zacne posielat dalsie data
    return TWDR;
}

uint8_t TWILight::TWIReadNACK(void){
    TWCR = (1<<TWINT) | (1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);   //MASTER posle NACK, Slave zastavi posielanie dalsich dat
    return TWDR;
}

uint8_t TWILight::TWIGetStatus(void){
    uint8_t status;
    //mask status
    status = TWSR & 0xF8;
    return status;
}

TWIState TWILight::TWIWriteByte(uint8_t slaveAddress, uint8_t regAddr, uint8_t u8data){
    //start communication with Slave
    TWIStart();
    if (TWIGetStatus() != TWI_START_SENT){
        return ERROR;
    }
    //send Slave address (SLA+W)
    TWIWrite( (slaveAddress << 1) & 0xFE );
    if (TWIGetStatus() != TWI_MT_SLAW_ACK){
        return ERROR;
    }
    //send the register address
    TWIWrite(regAddr);
    if (TWIGetStatus() != TWI_MT_DATA_ACK){
        return ERROR;
    }
    //send the u8data to register previous address
    TWIWrite(u8data);
    if (TWIGetStatus() != TWI_MT_DATA_ACK){
        return ERROR;
    }
    TWIStop();
    return SUCCESS;          
}

TWIState TWILight::TWIReadByte(uint8_t slaveAddress, uint8_t regAddr, uint8_t *u8data){
    //start communication with Slave
    TWIStart();
    if (TWIGetStatus() != TWI_START_SENT){
        return ERROR;
    }
    //send Slave address (SLA+W)
    TWIWrite( (slaveAddress << 1) & 0xFE);
    if (TWIGetStatus() != TWI_MT_SLAW_ACK){
        return ERROR;
    }
    //send the register address to Slave
    TWIWrite(regAddr);
    if (TWIGetStatus() != TWI_MT_DATA_ACK){
        return ERROR;
    }
    TWIStart();
    //send repeated start to Slave
    if (TWIGetStatus() != TWI_REP_START_SENT){
        return ERROR;
    }
    //send Slave address (SLA+R)
    TWIWrite( (slaveAddress << 1) | 0x01);
    if (TWIGetStatus() != TWI_MR_SLAR_ACK){
        return ERROR;
    }
    //Master precita data zo Slave a zakaze Slave-ovi posielat dalsie data
    *u8data = TWIReadNACK();
    if (TWIGetStatus() != TWI_MR_DATA_NACK){
        return ERROR;
    }
    TWIStop();
    return SUCCESS;    
}

TWIState TWILight::TWIReadMultipleBytes(uint8_t slaveAddress, uint8_t regAddr, uint8_t *u8data, uint8_t length){
    int8_t i = 0;

    //start communication with Slave
    TWIStart();
    if (TWIGetStatus() != TWI_START_SENT){
        return ERROR;
    }
    //send Slave address (SLA+W)
    TWIWrite( (slaveAddress << 1) & 0xFE);
    if (TWIGetStatus() != TWI_MT_SLAW_ACK){
        return ERROR;
    }
    //send the register address to Slave
    TWIWrite(regAddr);
    if (TWIGetStatus() != TWI_MT_DATA_ACK){
        return ERROR;
    }
    TWIStart();
    //send repeated start to Slave
    if (TWIGetStatus() != TWI_REP_START_SENT){
        return ERROR;
    }
    //send Slave address (SLA+R)
    TWIWrite( (slaveAddress << 1) | 0x01);
    if (TWIGetStatus() != TWI_MR_SLAR_ACK){
        return ERROR;
    }
    for (i = 0; i < length - 1; i++){
        //Master precita data zo Slave a povoli Slave-ovi posielat dalsie data
        u8data[i] = TWIReadACK();
        if (TWIGetStatus() != TWI_MR_DATA_ACK){
            return ERROR;
        }
    }
    //Master precita posledny Byte zo Slave a zakaze Slave-ovi posielat dalsie data
    u8data[i] = TWIReadNACK();
    if (TWIGetStatus() != TWI_MR_DATA_NACK){
        return ERROR;
    }
    TWIStop();
    return SUCCESS;    
}

TWIState TWILight::TWIReadMultipleBytesFrom(uint8_t slaveAddress, uint8_t regAddr, uint8_t *u8data, uint8_t u8dataIndexStart, uint8_t count, uint8_t maxLength){
    int8_t i = u8dataIndexStart;

    //start communication with Slave
    TWIStart();
    if (TWIGetStatus() != TWI_START_SENT){
        return ERROR;
    }
    //send Slave address (SLA+W)
    TWIWrite( (slaveAddress << 1) & 0xFE);
    if (TWIGetStatus() != TWI_MT_SLAW_ACK){
        return ERROR;
    }
    //send the register address to Slave
    TWIWrite(regAddr);
    if (TWIGetStatus() != TWI_MT_DATA_ACK){
        return ERROR;
    }
    TWIStart();
    //send repeated start to Slave
    if (TWIGetStatus() != TWI_REP_START_SENT){
        return ERROR;
    }
    //send Slave address (SLA+R)
    TWIWrite( (slaveAddress << 1) | 0x01);
    if (TWIGetStatus() != TWI_MR_SLAR_ACK){
        return ERROR;
    }
    //nacitam urcity pocet bajtov (count) do "u8data" zacnuc od urciteho indexu "u8dataIndexStart"
    if (u8dataIndexStart + count <= maxLength){
        for (i = u8dataIndexStart; i < u8dataIndexStart + count - 1; i++){
            //Master precita data zo Slave a povoli Slave-ovi posielat dalsie data
            u8data[u8dataIndexStart] = TWIReadACK();
            if (TWIGetStatus() != TWI_MR_DATA_ACK){
                return ERROR;
            }
        }
    }
    else{
        return ERROR;
    }
    //precitam este posledny bajt v poradi a zaslem NACK
    if (u8dataIndexStart < maxLength){
        u8data[i] = TWIReadNACK();
        if (TWIGetStatus() != TWI_MR_DATA_NACK){
            return ERROR;
        }
    }
    else{
        return ERROR;
    }

    TWIStop();
    return SUCCESS;  
}

TWIState TWILight::TWIWriteMultipleBytes(uint8_t slaveAddress, uint8_t regAddr, uint8_t *u8data, uint8_t length){
    uint8_t i = 0;
    //start communication with Slave
    TWIStart();
    if (TWIGetStatus() != TWI_START_SENT){
        return ERROR;
    }
    //send Slave address (SLA+W)
    TWIWrite( (slaveAddress << 1) & 0xFE );
    if (TWIGetStatus() != TWI_MT_SLAW_ACK){
        return ERROR;
    }
    //send the register address
    TWIWrite(regAddr);
    if (TWIGetStatus() != TWI_MT_DATA_ACK){
        return ERROR;
    }
    for (i = 0; i < length; i++){
        //send the u8data to register previous address
        TWIWrite(u8data[i]);
        if (TWIGetStatus() != TWI_MT_DATA_ACK){
            return ERROR;
        }
    }
    TWIStop();
    return SUCCESS; 
}
TWIState TWILight::TWIWriteMultipleBytesFrom(uint8_t slaveAddress, uint8_t regAddr, uint8_t *u8data, uint8_t u8dataIndexStart, uint8_t count, uint8_t maxLength){
    uint8_t i = 0;
    //start communication with Slave
    TWIStart();
    if (TWIGetStatus() != TWI_START_SENT){
        return ERROR;
    }
    //send Slave address (SLA+W)
    TWIWrite( (slaveAddress << 1) & 0xFE );
    if (TWIGetStatus() != TWI_MT_SLAW_ACK){
        return ERROR;
    }
    //send the register address
    TWIWrite(regAddr);
    if (TWIGetStatus() != TWI_MT_DATA_ACK){
        return ERROR;
    }
    //poslem urcity pocet bajtov (count) z "u8data" zacnuc od urciteho indexu "u8dataIndexStart"
    if (u8dataIndexStart + count <= maxLength){
        for (i = u8dataIndexStart; i < u8dataIndexStart + count; i++){
            //send the u8data to register previous address
            TWIWrite(u8data[i]);
            if (TWIGetStatus() != TWI_MT_DATA_ACK){
                return ERROR;
            }
        }
    }
    else{
        return ERROR;
    }
    TWIStop();
    return SUCCESS; 
}
