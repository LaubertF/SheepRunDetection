#include "XbeeMaster.h"
#include <stdlib.h>

XbeeMaster::XbeeMaster(long uartBaudrate){
    Serial1.begin(uartBaudrate);
    circular_buf_init(cbuf);
    paCircular_buf_init(packetsRx);
    numValidRxPackets = 0;  //na zaciatku mam zasobnik packetsRx prazdny
}
void XbeeMaster::transmitPacket(){
    //packetTx.calculateCrc();
    Serial1.write(packetTx.startDelimiter);
    Serial1.write(packetTx.length[0]);    //MSB
    Serial1.write(packetTx.length[1]);    //LSB
    for(uint16_t i = 0; i < packetTx.getLength(); i++){
        Serial1.write(packetTx.frameData[i]);
    }
    Serial1.write(packetTx.crc);
}
// na zistenie poctu kompletne priajatych packetov sluzi "numValidRxPackets"
// prijate packety su v "packetsRx"
boolean XbeeMaster::parseToPacketsDev(){
    uint8_t data;   //aktualne citane data z circ. buffera
    boolean isComplete = false;    //zatial neviuzivam, NESPOLIEHAT SA NA TO
    static uint8_t indxlen = 0;
    static uint16_t debugCounter = 0;
    static CurrentPhase currentPhase = SEARCHING_START_SYMBOL;   //Stavova masina
    static uint16_t indxFrameData = 0;   //index prekopirovanie dat do frameData 
    while(!circular_buf_empty(cbuf))
	{
		circular_buf_get(cbuf, &data);
        
        //zistujem, kde zacina prijaty packet
		if (currentPhase == SEARCHING_START_SYMBOL){
            if(data == START_DELIMITER){
                m_onePacket.startDelimiter = data;
                currentPhase = DETERMINE_PACKET_LEN;   //priznak = nasiel som zaciatok packetu
                debugCounter++;
                //Serial.println("SEARCHING_START_SYMBOL");
            }
        }
        //zistujem dlzku prijateho packetu
        else if (currentPhase == DETERMINE_PACKET_LEN){
            m_onePacket.length[indxlen]=data;
            //Serial.print(data,HEX);
            //Serial.print(" ");
            indxlen++;               
            debugCounter++;              
            if(indxlen >= 2){
                currentPhase = COPY_FRAMEDATA;   //priznak = je znama dlzka packetu, dalsie bajty su FRAMEDATA
                indxlen = 0;    //uz mozem vynulovat pocitadlo
                //Serial.println("DETERMINE_PACKET_LEN");
            }
        }
        //kopirujem cely frame prijateho packetu
        else if (currentPhase == COPY_FRAMEDATA){
            m_onePacket.frameData[indxFrameData]=data;
            debugCounter++;
            indxFrameData++;
            if(indxFrameData >= m_onePacket.getLength()){
                //Serial.print("last indxFrameData=");
                //Serial.println(indxFrameData,DEC);
                indxFrameData = 0;   //priprava na kopirovanie dalsieho packetu
                currentPhase = DETERMINE_CRC;   //priznak = treba skontrolovat CRC
                //Serial.println("COPY_FRAMEDATA");
            }
        }
        //zistim CRC a prekopirujem kompletny packet do zasobnika packetov
        else if (currentPhase == DETERMINE_CRC){
            //skopirujem prijate CRC
            m_onePacket.crc = data;
            debugCounter++;
            //Serial.println("DETERMINE_CRC");

            //skopirujem kompletny jeden packet do zasobnika packetov
            //TODO: skopirujem iba vtedy ked je platny CRC...
            paCircular_buf_put2(packetsRx, m_onePacket);
            currentPhase = SEARCHING_START_SYMBOL;  //priznak = mozem hladat dalsi packet
            debugCounter = 0;
            isComplete = true;
        }
	}
    return isComplete; 
}
uint32_t XbeeMaster::getRTCfromPacket(Packet p){    
    const uint8_t cmdLen = FRAME_DATA_LEN + 1;
    const char searchStr[] = "timestamp=";  //{'t', 'i', 'm', 'e', 's', 't', 'a', 'm', 'p','=','\0'};
    const uint8_t lenSearchStr = strlen(searchStr);
    char timestamp[10+1];
    char *found;
    char cmd[cmdLen];
    uint16_t startIndx=0;
    uint16_t offset;
    uint8_t i;
    
    //uint8_t lenCMD;
    //char printbuff[FRAME_DATA_LEN];
    
    //Serial.print("frameType=");
    //Serial.println(p.getFrameType(), HEX);
    if(p.getFrameType() == FRAMETYPE_RECEIVE_PACKET){
        for (i = 0; i < FRAME_DATA_LEN; i++){
            if(p.frameData[i] != 0){
                cmd[i] = p.frameData[i];
            }
            else{
                cmd[i] = p.frameData[i] + '0';
            }
        }
        cmd[i] = '\0';
        
        //lenCMD = strlen(cmd);        
        /*
        for (i = 0; i < FRAME_DATA_LEN; i++){
            Serial.print(cmd[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        */
        found = strstr(cmd,searchStr);
        /*
        sprintf(printbuff, "found=%p cmd=%p lenSearchStr=%d lenCMD=%d\n",found, cmd, lenSearchStr, lenCMD);
        Serial.print(printbuff);
        for(i = 0; i < 50; i++){
            printbuff[i] = '\0';
        }
        */
        if (found != NULL){
            if(cmd >= found){
                startIndx = cmd - found;
            }
            else{
                startIndx = found - cmd;
            }
            offset = startIndx + lenSearchStr;
            for(i = 0; i < lenSearchStr; i++){
                timestamp[i]=p.frameData[offset + i];
            }
            return strtoul(timestamp, NULL, 0); 
        }
    }
    return 0;
}

PacketState XbeeMaster::receivePackets(){
    uint16_t RxBuffLen = 0;
    uint16_t countBajt = 0;
    uint8_t c;

    RxBuffLen=Serial1.available();
    if(RxBuffLen > 0){
        //precitam cely Serial1 buffer a dam ho do vacsieho cbuf
        while (countBajt < RxBuffLen)
        {
            c = Serial1.read();
            circular_buf_put2(cbuf, c);
            countBajt++;
            //Serial.print(c,HEX);
            //Serial.print(" ");
        }
        //Serial.println();
        return OK;
    }
    else{
        return EMPTY;
        //Serial.println("empty");
    }
}

void XbeeMaster::buildTransmitRequest(uint8_t frameId, uint8_t *destAddr64, uint8_t *destAddr16, 
    uint8_t broadcastRadius, uint8_t options, uint8_t *rfData, uint8_t rfDataLen){
        
    uint8_t i, countLen;
    packetTx.frameData[0] = FRAME_TRANSMIT_REQUEST;
    countLen =  1;
    packetTx.frameData[countLen++] = frameId;
    packetTx.frameID = frameId;
    for (i = 0; i < ADDR64_LEN; i++){
        packetTx.frameData[countLen++] = destAddr64[i];
    }

    for (i = 0; i < ADDR16_LEN; i++){
        packetTx.frameData[countLen++] = destAddr16[i];
    }

    packetTx.frameData[countLen++] = broadcastRadius;
    packetTx.frameData[countLen++] = options;

    for (i = 0; i < rfDataLen; i++){
        packetTx.frameData[countLen++] = rfData[i];
    }

    packetTx.setLength(countLen);
    packetTx.calculateCrc();
}

//TODO: doplnit podla potreby
void XbeeMaster::printTxPacket(){

}
//TODO: doplnit podla potreby
uint8_t XbeeMaster::printPacket(Packet *packet, uint8_t indx){
    return 1;    
}
//TODO: dorobit podla potreby
uint8_t XbeeMaster::printPackets(){
    uint8_t n = 0;
    return n;   //kolko packetov sa vypisalo
}

void XbeeMaster::clearPacket(Packet *packet, uint8_t index){
        packet[index].startDelimiter = 0x00;
        packet[index].setLength(0);
        for (uint8_t i = 0; i < packet[index].getLength() + 1; i++){
            packet[index].frameData[i] = 0;
    }
}

//TODO: dorobit podla potreby - asi to uz nebude treba ked mam circular buffer
void XbeeMaster::clearAllRxPackets(){

}
// gettre a settre
uint8_t XbeeMaster::getLastValidRxPacket(){
    return numValidRxPackets;
}

CIRCULAR_BUF_T* XbeeMaster::getCbuf(){
    return cbuf;
}

void XbeeMaster::setNumValidRxPackets(uint8_t pocet){
    numValidRxPackets = pocet;
}