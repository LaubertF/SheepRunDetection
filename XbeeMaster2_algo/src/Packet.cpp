#include "Packet.h"

Packet::Packet(){
    startDelimiter = START_DELIMITER;
    setLength(0);
}
uint8_t Packet::getStartDelimiter(){
    return startDelimiter;
}
void Packet::setLength(uint16_t len16){
    length[0]=len16 >> 8;	//MSB
	length[1]=len16 & 0xFF;	//LSB
}
void Packet::setLength(uint8_t *len8, uint8_t size){
    for (uint8_t i = 0; i < size; i++){
        length[i]=len8[i];	//MSB[0]:LSB[1]
    }
}
uint16_t Packet::getLength(){
    return (length[0] << 8) | (length[1] & 0xFF);
}

uint8_t Packet::getFrameType(){
    return frameData[0];   //na 0-tej pozicii bude vzdy frameType
}

void Packet::calculateCrc(){
	crc = 0;
    for(uint16_t i = 0; i < getLength(); i++){
        crc = crc + frameData[i];
    }
	crc = 0xFF - crc;
}

//========= GETTER - SETTER ==========
uint8_t Packet::getCrc(){
    return crc;
}

//========= POMOCNE METODY ===========

uint16_t Packet::convert8To16(uint8_t *len8){
    //len8[0] -> MSB
    //len8[1] -> LSB
    return (uint16_t)(len8[0] | (len8[1] << 8));
}