/**
 @file FaBo9Axis_MPU9250.cpp
 @brief This is a library for the FaBo 9Axis I2C Brick.

   http://fabo.io/202.html

   Released under APACHE LICENSE, VERSION 2.0

   http://www.apache.org/licenses/

 @author FaBo<info@fabo.io>
 @author Ondrej Gallo
*/
#include <Arduino.h>
#include "mpu9250.h"


MPU9250::MPU9250(uint8_t addr) {
  _mpu9250addr = addr;
  sensor.TWIInit(); //aktivuje I2C a nastavi frekvensiu SCL = 100kHz 

}
/**
 @brief Begin Device
 @retval true normaly done
 @retval false device error
*/
bool MPU9250::begin() {
  if (searchDevice()) {
    configMPU9250();
    configAK8963();
    return true;
  } 
  else {
    return false;
  }
}

/**
 @brief Search Device
 @retval true device connected
 @retval false device error
*/
bool MPU9250::searchDevice() {
  uint8_t whoami;
  sensor.TWIReadByte(_mpu9250addr, MPU9250_WHO_AM_I, &whoami);
  if(whoami == 0x71){
    return true;
  } 
  else{
    return false;
  }
}

/**
 @brief Configure MPU-9250
 @param [in] gfs Gyro Full Scale Select(default:MPU9250_GFS_250[+250dps])
 @param [in] afs Accel Full Scale Select(default:MPU9250_AFS_2G[2g])
*/
void MPU9250::configMPU9250(uint8_t gfs = MPU9250_GFS_250, uint8_t afs = MPU9250_AFS_2G){
  switch(gfs) {
    case MPU9250_GFS_250:
      _gres = 250.0/32768.0;
      break;
    case MPU9250_GFS_500:
      _gres = 500.0/32768.0;
      break;
    case MPU9250_GFS_1000:
      _gres = 1000.0/32768.0;
      break;
    case MPU9250_GFS_2000:
      _gres = 2000.0/32768.0;
      break;
  }
  switch(afs) {
    case MPU9250_AFS_2G:
      _ares = 2.0/32768.0;
      break;
    case MPU9250_AFS_4G:
      _ares = 4.0/32768.0;
      break;
    case MPU9250_AFS_8G:
      _ares = 8.0/32768.0;
      break;
    case MPU9250_AFS_16G:
      _ares = 16.0/32768.0;
      break;
  }
  // sleep off
  sensor.TWIWriteByte(_mpu9250addr, MPU9250_PWR_MGMT_1, 0x00);
  delay(100);
  // auto select clock source
  sensor.TWIWriteByte(_mpu9250addr, MPU9250_PWR_MGMT_1, 0x01);
  delay(200);
  // DLPF_CFG
  sensor.TWIWriteByte(_mpu9250addr, MPU9250_CONFIG, 0x03);
  // sample rate divider
  sensor.TWIWriteByte(_mpu9250addr, MPU9250_SMPLRT_DIV, 0x04);
  // gyro full scale select
  sensor.TWIWriteByte(_mpu9250addr, MPU9250_GYRO_CONFIG, gfs << 3);
  // accel full scale select
  sensor.TWIWriteByte(_mpu9250addr, MPU9250_ACCEL_CONFIG, afs << 3);
  // A_DLPFCFG
  sensor.TWIWriteByte(_mpu9250addr, MPU9250_ACCEL_CONFIG_2, 0x03);
  // BYPASS_EN
  sensor.TWIWriteByte(_mpu9250addr, MPU9250_INT_PIN_CFG, 0x02);
  delay(100);
}

void MPU9250::configAK8963(uint8_t mode = AK8963_MODE_C8HZ, uint8_t mfs = AK8963_BIT_16){
  uint8_t data[3];

  switch(mfs) {
    case AK8963_BIT_14:
      _mres = 4912.0/8190.0;
      break;
    case AK8963_BIT_16:
      _mres = 4912.0/32760.0;
      break;
  }

  // set software reset
  // writeI2c(AK8963_SLAVE_ADDRESS, AK8963_CNTL2, 0x01);
  // delay(100);
  
  // set power down mode
  sensor.TWIWriteByte(AK8963_SLAVE_ADDRESS, AK8963_CNTL1, 0x00);
  delay(1);
  // set read FuseROM mode
  sensor.TWIWriteByte(AK8963_SLAVE_ADDRESS, AK8963_CNTL1, 0x0F);
  delay(1);
  // read coef data
  sensor.TWIReadMultipleBytes(AK8963_SLAVE_ADDRESS, AK8963_ASAX, data, 3);
  _magXcoef = (float)(data[0] - 128) / 256.0 + 1.0;
  _magYcoef = (float)(data[1] - 128) / 256.0 + 1.0;
  _magZcoef = (float)(data[2] - 128) / 256.0 + 1.0;
  // set power down mode
  sensor.TWIWriteByte(AK8963_SLAVE_ADDRESS, AK8963_CNTL1, 0x00);
  delay(1);
  // set scale&continous mode
  sensor.TWIWriteByte(AK8963_SLAVE_ADDRESS, AK8963_CNTL1, (mfs<<4|mode));
  delay(1);
}

void MPU9250::readAccelXYZ2x8(uint8_t *ax, uint8_t *ay, uint8_t *az) {
  uint8_t data[6];
  sensor.TWIReadMultipleBytes(_mpu9250addr, MPU9250_ACCEL_XOUT_H, data, 6); 
  ax[0] = data[0]; //ACCEL_XOUT_H
  ax[1] = data[1]; //ACCEL_XOUT_L
  ay[0] = data[2]; //ACCEL_YOUT_H
  ay[1] = data[3]; //ACCEL_YOUT_L
  az[0] = data[4]; //ACCEL_ZOUT_H
  az[1] = data[5]; //ACCEL_ZOUT_L
}

TWIState MPU9250::readAccelXYZ2x8Volatile(volatile uint8_t *ax, volatile uint8_t *ay, volatile uint8_t *az){
  uint8_t data[6];
  _twistatus = sensor.TWIReadMultipleBytes(_mpu9250addr, MPU9250_ACCEL_XOUT_H, data, 6);
  //if (_twistatus == ERROR){
  //  sensor.TWIStop();
  //  return _twistatus;
  //} 
  ax[0] = data[0]; //ACCEL_XOUT_H
  ax[1] = data[1]; //ACCEL_XOUT_L
  ay[0] = data[2]; //ACCEL_YOUT_H
  ay[1] = data[3]; //ACCEL_YOUT_L
  az[0] = data[4]; //ACCEL_ZOUT_H
  az[1] = data[5]; //ACCEL_ZOUT_L
  return _twistatus;
}

TWIState MPU9250::readAccGyMag2x8Volatile(volatile uint8_t *ax, volatile uint8_t *ay, volatile uint8_t *az,
                                          volatile uint8_t *gx, volatile uint8_t *gy, volatile uint8_t *gz,
                                          volatile uint8_t *magx, volatile uint8_t *magy, volatile uint8_t *magz){
  TWIState accState, gyState, magState;
  uint8_t accdata[6];
  uint8_t gydata[6];
  uint8_t magdata[7]; //pri magnetometri musim precitat jeden register (ST2) naviac, aby sa v dalsom cykle mohlo spustit meranie znova
  _twistatus = ERROR;
  accState = sensor.TWIReadMultipleBytes(_mpu9250addr, MPU9250_ACCEL_XOUT_H, accdata, 6);
  gyState = sensor.TWIReadMultipleBytes(_mpu9250addr, MPU9250_GYRO_XOUT_H, gydata, 6);
  magState = sensor.TWIReadMultipleBytes(AK8963_SLAVE_ADDRESS, AK8963_HXL, magdata, 7);
  
  //if( !(magdata[6] & 0x08) ){_twistatus = MAG_SENS_OVERFLOW;} //kontrola ci meg senzor bol velmi vybudeny externym mag polom = take meranie je nekorektne (toto nepotrebujem testovat)
  
  //if (_twistatus == ERROR){
  //  sensor.TWIStop();
  //  return _twistatus;
  //} 
  ax[0] = accdata[0]; //MPU9250_ACCEL_XOUT_H
  ax[1] = accdata[1]; //MPU9250_ACCEL_XOUT_L
  ay[0] = accdata[2]; //MPU9250_ACCEL_YOUT_H
  ay[1] = accdata[3]; //MPU9250_ACCEL_YOUT_L
  az[0] = accdata[4]; //MPU9250_ACCEL_ZOUT_H
  az[1] = accdata[5]; //MPU9250_ACCEL_ZOUT_L

  gx[0] = gydata[0]; //MPU9250_GYRO_XOUT_H
  gx[1] = gydata[1]; //MPU9250_GYRO_XOUT_L
  gy[0] = gydata[2]; //MPU9250_GYRO_YOUT_H
  gy[1] = gydata[3]; //MPU9250_GYRO_YOUT_L
  gz[0] = gydata[4]; //MPU9250_GYRO_ZOUT_H
  gz[1] = gydata[5]; //MPU9250_GYRO_ZOUT_L

  magx[0] = magdata[1]; //AK8963_HXH
  magx[1] = magdata[0]; //AK8963_HXL
  magy[0] = magdata[3]; //AK8963_HYH
  magy[1] = magdata[2]; //AK8963_HYL
  magz[0] = magdata[5]; //AK8963_HZH
  magz[1] = magdata[4]; //AK8963_HZL

  if ((accState == SUCCESS) && (gyState == SUCCESS) && (magState == SUCCESS)){
    _twistatus = SUCCESS;  
  }
  return _twistatus;
}