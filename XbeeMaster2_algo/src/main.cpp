// funkcna verzia na logovanie dat z akcelerometra (iba) so synchronizaciou casu s PC cez Xbee
#include <Arduino.h>
#include "mpu9250.h"
#include "DS1337.h"
#include "sdCircularBuffer.h"
#include "XbeeMaster.h"
#include "utils.h"

//piny je potrebne znacit podla "MegaCore Arduino Mega" --> https://github.com/MCUdude/MegaCore#pinout
#define PIN_SHTWD_SDCARD 48 //(novy dizajn PL1=48, stary dizajn PA2=25)
#define PIN_CHIPSELECT_SD 8
#define PIN_CHIPSELECT_SD_DEFAULT 53  //nepouzivam ale kniznica SD ho potrebuje nastavit na OUTPUT

#define PIN_RTC_INT 2

#define PIN_ADC_ON_OFF 40 //(novy dizajn PG1=40, stary dizajn PJ7=85)

#define PIN_CS_GPS 10
#define PIN_WAKEUP_GPS 11
#define PIN_ON_OFF_GPS 12

#define PIN_XBEE_SLEEP 38
#define PIN_XBEE_RESET 77 //nie je v originalnom ArduineMega ale v MegaCore Arduine je!
#define PIN_XBEE_RTS 78 //nie je v originalnom ArduineMega ale v MegaCore Arduine je!
#define PIN_XBEE_CTS 79 //nie je v originalnom ArduineMega ale v MegaCore Arduine je!

#define XBEE_TX_BUFFER 42 //max velkost payload-u (nie packetu)

// GLOBALNE PREMENNE
//RTC  
DS1337 rtc;
Date dt;
uint32_t sync_timestamp=1;
volatile boolean alarm = false;
volatile boolean samplingReady = false;

//Senzor MPU9250 - akcelero, gyro, mag
MPU9250 sensorMPU9250;
volatile LOG sample[1];
volatile LOG sampleloop[1];
volatile SD_CIRCULAR_BUF_T sdbuff[1];
volatile TWIState twistat;

//Xbee
XbeeMaster xbeeM(57600); //MAX = 115200
uint8_t slave1Addr64[] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0x07, 0xC3, 0x09};  //slave 64-bit address (destination)
uint8_t slave1Addr16[] = {0xFF, 0xFE};  //broadcast all Xbee slave
uint8_t xbeebuff[XBEE_TX_BUFFER];
Packet packet;

// HLAVICKY MOJICH FUNKCII
void rtcInterrupt();
void printDateTime(Date date);
void printPacket(Packet p);

void setup() {
  //******** inicializacia I/O pinov ********
  //SD karta
  pinMode(PIN_CHIPSELECT_SD_DEFAULT, OUTPUT); //nutne, aj ked sa pin nepouzije!
  pinMode(PIN_SHTWD_SDCARD, OUTPUT);
  digitalWrite(PIN_SHTWD_SDCARD, LOW);  //napajanie SD karty vypnute
  
  //Xbee
  pinMode(PIN_XBEE_SLEEP, OUTPUT);
  digitalWrite(PIN_XBEE_SLEEP, LOW);  //sleep xbee vypnute = (LOW = zobudeny, HIGH = sleep)
  pinMode(PIN_XBEE_RESET, OUTPUT);
  digitalWrite(PIN_XBEE_RESET, LOW);  //reset xbee neaktivny

  //RTC
  pinMode(PIN_RTC_INT, INPUT_PULLUP);
  digitalWrite(PIN_RTC_INT, HIGH);
    
  //ADC
  pinMode(PIN_ADC_ON_OFF, OUTPUT);
  digitalWrite(PIN_ADC_ON_OFF, LOW);  //ADC je odpojene od zroja meraneho napatia

  //GPS
  pinMode(PIN_CS_GPS, OUTPUT);    //!!!!!pozor, zmenit na INPUT, ak zacnemm pouzivat GPS
  digitalWrite(PIN_CS_GPS, LOW);  //ak sa pouzije UART nema sa pouzivat (INPUT a disable pull-up)
  pinMode(PIN_WAKEUP_GPS, INPUT);
  digitalWrite(PIN_WAKEUP_GPS, LOW);  //detekuje stav GPS (ON/OFF)
  pinMode(PIN_ON_OFF_GPS, OUTPUT);
  digitalWrite(PIN_ON_OFF_GPS, LOW);  //default = LOW; specialny toggle pulz = ON/OFF (vid manual A2235-H)

  //********** inicializacia MODULOV *************
  //Serial
  Serial.begin(460800); //max rychlost pri F_CPU = 3.6864MHz je 460800
  
  //MPU9250 senzor
  sd_circular_buf_init(sdbuff);

  //RTC - TODO: sync presneho casu s PC cez Xbee (primo v SETUP-e => nepojde sa dalej pokial nenastavim presny cas)
  while(1){
    xbeeM.receivePackets(); //nabafruj do kruhoveho uint8 buffera data z UART xbee
    if(circular_buf_empty(xbeeM.cbuf) == false){  //ak je nieco v kruhovom buffery
      if(xbeeM.parseToPacketsDev() == true){  //ak je dokonceny prijem packetu (packet ma zaciatok a koniec)
        paCircular_buf_get(xbeeM.packetsRx, &packet); //vyber z bufera pre packety jeden packet
        sync_timestamp = xbeeM.getRTCfromPacket(packet);
        break;
        //Serial.print("cas=");
        //Serial.println(sync_timestamp, DEC);
        //printPacket(packet);
      }
      else{
        continue;
      }
    } 
  }

  attachInterrupt(digitalPinToInterrupt(PIN_RTC_INT), rtcInterrupt, FALLING); //nastavujem externe prerusenie INT4
  rtc.init();
  rtc.customSettingsCNTRL();
  Serial.print("sync_timestamp");
  Serial.println(sync_timestamp, DEC);
  rtc.setDateTimeFromTimestamp(sync_timestamp);
  rtc.setAlarmMode(DS1337_ALARM_EVERY_SECOND);

  sample[0].timestamp = rtc.getTimestamp();
  sample[0].num_sample_persec = 0;  //pre istotu
  Serial.println(sample[0].timestamp);

  //Senzor MPU9250 - akcelero, gyro, mag
  if (!sensorMPU9250.begin()) {
    Serial.println(F("MPU9250 error! STOP!"));
    while(1);
  }
  sensorMPU9250.configMPU9250(MPU9250_GFS_500, MPU9250_AFS_8G);


  //TIMER1 - zabezpeci rovnomerne meranie vzoriek z MPU9250 (v SETUP-e nastavit ako posledne!)
  //https://www.arduinoslovakia.eu/application/timer-calculator
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 47;          // 75 Hz (3686400/((47+1)*1024)) [10ms]
  TCCR1B |= (1 << WGM12); // CTC
  TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
  TIMSK1 |= (1 << OCIE1A);  // Output Compare Match A Interrupt Enable
  //TIMSK1 &= ~(1 << OCIE1A);  // Output Compare Match A Interrupt Disable 
  interrupts(); 
}

void loop() {
  uint8_t lenXbeePayload;
  uint8_t buff16[2];
  //obsluha RTC
  if (alarm == true){ //obsluha = 3.46ms
      digitalWrite(PIN_ON_OFF_GPS, LOW); //kanal C
      alarm = false;
      rtc.clearAlarm();
      rtc.getDate2();
      sample[0].num_sample_persec = 0;
      sample[0].timestamp = rtc.getTimestamp();
  }
  //obsluha senzora MPU9250
  if (samplingReady == true){ //obsluha = 1.07ms
    digitalWrite(PIN_ON_OFF_GPS, LOW); //kanal C
    samplingReady = false;
    sensorMPU9250.readAccGyMag2x8Volatile(sample[0].accx, sample[0].accy, sample[0].accz,
                                          sample[0].gx, sample[0].gy, sample[0].gz,
                                          sample[0].magx, sample[0].magy, sample[0].magz);
    //sensorMPU9250.readAccelXYZ2x8Volatile(sample[0].accx, sample[0].accy, sample[0].accz);
    if( (sdbuff[0].max - sd_circular_buf_size(sdbuff)) >= sizeof(LOG)){
      sample[0].num_sample_persec++;
      sd_circular_buf_put2(sdbuff, sample);
    }
  }

  if(sd_circular_buf_empty(sdbuff) == false){ //obsluha = 1.84ms (max)
    digitalWrite(PIN_CS_GPS, HIGH); //kanal D
    sd_circular_buf_get(sdbuff, sampleloop);
    if(sampleloop[0].num_sample_persec == 1){
      lenXbeePayload = 24;  //velkost jedneho payloadu, ak sa posiela timestamp
      convertLongToBytes(sampleloop[0].timestamp, xbeebuff);
      convertUint16ToUint8(sampleloop[0].num_sample_persec, buff16);
      xbeebuff[4] = buff16[0];
      xbeebuff[5] = buff16[1];
      xbeebuff[6] = sampleloop[0].accx[0];
      xbeebuff[7] = sampleloop[0].accx[1];
      xbeebuff[8] = sampleloop[0].accy[0];
      xbeebuff[9] = sampleloop[0].accy[1];
      xbeebuff[10] = sampleloop[0].accz[0];
      xbeebuff[11] = sampleloop[0].accz[1];

      xbeebuff[12] = sampleloop[0].gx[0];
      xbeebuff[13] = sampleloop[0].gx[1];
      xbeebuff[14] = sampleloop[0].gy[0];
      xbeebuff[15] = sampleloop[0].gy[1];
      xbeebuff[16] = sampleloop[0].gz[0];
      xbeebuff[17] = sampleloop[0].gz[1];

      xbeebuff[18] = sampleloop[0].magx[0];
      xbeebuff[19] = sampleloop[0].magx[1];
      xbeebuff[20] = sampleloop[0].magy[0];
      xbeebuff[21] = sampleloop[0].magy[1];
      xbeebuff[22] = sampleloop[0].magz[0];
      xbeebuff[23] = sampleloop[0].magz[1];      
      xbeeM.buildTransmitRequest(0x00, slave1Addr64, slave1Addr16, 0x03, 0xC0, xbeebuff, lenXbeePayload);
    }
    else{
      lenXbeePayload = 20;  //velkost jedneho payloadu, ak sa neposiela timestamp
      convertUint16ToUint8(sampleloop[0].num_sample_persec, buff16);
      xbeebuff[0] = buff16[0];
      xbeebuff[1] = buff16[1];
      xbeebuff[2] = sampleloop[0].accx[0];
      xbeebuff[3] = sampleloop[0].accx[1];
      xbeebuff[4] = sampleloop[0].accy[0];
      xbeebuff[5] = sampleloop[0].accy[1];
      xbeebuff[6] = sampleloop[0].accz[0];
      xbeebuff[7] = sampleloop[0].accz[1];

      xbeebuff[8] = sampleloop[0].gx[0];
      xbeebuff[9] = sampleloop[0].gx[1];
      xbeebuff[10] = sampleloop[0].gy[0];
      xbeebuff[11] = sampleloop[0].gy[1];
      xbeebuff[12] = sampleloop[0].gz[0];
      xbeebuff[13] = sampleloop[0].gz[1];

      xbeebuff[14] = sampleloop[0].magx[0];
      xbeebuff[15] = sampleloop[0].magx[1];
      xbeebuff[16] = sampleloop[0].magy[0];
      xbeebuff[17] = sampleloop[0].magy[1];
      xbeebuff[18] = sampleloop[0].magz[0];
      xbeebuff[19] = sampleloop[0].magz[1];
      xbeeM.buildTransmitRequest(0x00, slave1Addr64, slave1Addr16, 0x03, 0xC0, xbeebuff, lenXbeePayload);
    } 
    xbeeM.transmitPacket();
    /*
    Serial.print(sampleloop[0].timestamp, DEC);
    Serial.print(" ");
    Serial.print(sampleloop[0].num_sample_persec, DEC);
    Serial.print(",");
    Serial.print(sampleloop[0].accx[0], HEX);
    Serial.print(",");
    Serial.print(sampleloop[0].accx[1], HEX);
    Serial.print(",");
    Serial.print(sampleloop[0].accy[0], HEX);
    Serial.print(",");
    Serial.print(sampleloop[0].accy[1], HEX);
    Serial.print(",");
    Serial.print(sampleloop[0].accz[0], HEX);
    Serial.print(",");
    Serial.print(sampleloop[0].accz[1], HEX);
    Serial.println();
    */
    digitalWrite(PIN_CS_GPS, LOW); //kanal D
  }
  //nemam ziaden packet v buffery (nudim sa...)
  else{
    digitalWrite(PIN_ON_OFF_GPS, HIGH); //kanal C
    //delayMicroseconds(50);
    //digitalWrite(PIN_ON_OFF_GPS, LOW); //kanal C
  }
} //koniec loop()

void printDateTime(Date date){
    Serial.print(date.getDay(), DEC);
    Serial.print(".");
    Serial.print(date.getMonth(), DEC);
    Serial.print(".");
    Serial.print(date.getYear(), DEC);
    Serial.print(" ");
    Serial.print(date.getHour(), DEC);
    Serial.print(":");
    Serial.print(date.getMinutes(), DEC);
    Serial.print(":");
    Serial.println(date.getSeconds(), DEC);
}

void printPacket(Packet p){
  int i;
  Serial.print(p.getStartDelimiter(), HEX);
  Serial.print(" ");
  Serial.print(p.getLength(), HEX);
  Serial.print(" ");
  for (i = 0; i < p.getLength(); i++){
    Serial.print(p.frameData[i], HEX);
    Serial.print(" ");
  }
  Serial.print(p.getCrc(), HEX);
  Serial.println();
}

// ***********************************************************
// ***************** P R E R U S E N I A *********************
// ***********************************************************
ISR(TIMER1_COMPA_vect) {
  noInterrupts(); 
  //digitalWrite(PIN_CS_GPS, HIGH); //kanal D
  samplingReady = true;
  //digitalWrite(PIN_CS_GPS, LOW); //kanal D //1.642ms
  interrupts();
}

void rtcInterrupt(){
  noInterrupts();  
  //digitalWrite(PIN_ON_OFF_GPS, HIGH); //kanal C
  alarm = true;
  //digitalWrite(PIN_ON_OFF_GPS, LOW); //kanal C
  interrupts();
}