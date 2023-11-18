#include "Packet.h"
#include "circular_buffer.h"
#include "PacketCircBuffer.h"

#define ADDR64_LEN 8
#define ADDR16_LEN 2
#define MAX_NUM_OF_RX_PACKETS 8

#define FRAMETYPE_RECEIVE_PACKET 0x90

enum CurrentPhase{
    SEARCHING_START_SYMBOL,
    DETERMINE_PACKET_LEN,
    COPY_FRAMEDATA,
    DETERMINE_CRC
};

class XbeeMaster{
private: 
  Packet m_onePacket;
  uint8_t numValidRxPackets;
public:
  CIRCULAR_BUF_T cbuf[1]; //pouzivam na prijem raw dat cez xbee
  PACKET_CIRCULAR_BUF packetsRx[1];  //obsahuje rozparsovane prijate raw data cez xbee do Packetov
  Packet packetTx;

  XbeeMaster(long uartBaudrate);
  void transmitPacket();
  PacketState receivePackets();
  void buildTransmitRequest(uint8_t frameId, uint8_t *destAddr64, uint8_t *destAddr16, 
                            uint8_t broadcastRadius, uint8_t options, uint8_t *rfData, uint8_t rfDataLen);
  Packet getPacket();
  boolean parseToPacketsDev();
  uint32_t getRTCfromPacket(Packet p); //vyparsuje unixtime z packetu
  void decodePacket();
  void clearPacket(Packet *packet, uint8_t index);
  void clearAllRxPackets();
  
  //===== getters - setters ======  
  uint8_t getLastValidRxPacket();
  void setNumValidRxPackets(uint8_t pocet);
  CIRCULAR_BUF_T* getCbuf();

  //===== pomocne ======
  void printTxPacket();
  uint8_t printPacket(Packet *packet, uint8_t indx);
  uint8_t printPackets();
};
