/*
  Sensors.cpp - Library for Sensors.
  Created by Carlos V.
  Released into the public domain.
*/

#include "Arduino.h"
#include <Wire.h>

#include "Sensors.h"

#define VBATPIN A6
#define ADC_RESOL 1024
#define VBAT_STEP_VAL (2 * 3.6 / (ADC_RESOL - 1) )

Sensors::Sensors()
{
  initialized = false;
  isAPDSReady = false;
  isBMPReady = false;
  isLSM6DSReady = false;

  apds = new Adafruit_APDS9960();
  bmp = new Adafruit_BMP280();
  lsm6ds33 = new Adafruit_LSM6DS33();
  lis3mdl = new Adafruit_LIS3MDL();
}

bool Sensors::initAPDS(void){
  Serial.println("-->Sensors::initAPDS()");
  if (apds->begin()) {  
    // gesture mode will be entered once proximity mode senses something close
    apds->enableProximity(true);
    apds->enableGesture(true);
    return true;
  }

  return false;
}

bool Sensors::initBMP(void){
  Serial.println("-->Sensors::initBMP()");
  if (bmp->begin()) {
    return true;
  }
  return false;
}

bool Sensors::initLSM6DS(void){
  Serial.println("-->Sensors::initLSM6DS()");
  if (lsm6ds33->begin_I2C()) {
    return true;
  }
  return false;
}

bool Sensors::initLIS3MDL(void){
  Serial.println("-->Sensors::initLIS3MDL()");
  if (lis3mdl->begin_I2C()) {
    return true;
  }
  return false;
}

bool Sensors::init(void){
  isAPDSReady = initAPDS();
  isBMPReady = initBMP();
  isLSM6DSReady = initLSM6DS();
  isLIS3MDLReady = initLIS3MDL();

  return isAPDSReady && isBMPReady && isLSM6DSReady && isLIS3MDLReady;
}

float Sensors::readVBat(void) {
  return analogRead(VBATPIN) * VBAT_STEP_VAL;
}

uint8_t Sensors::readGesture(void){
  if (isAPDSReady) {
    return apds->readGesture();
  }
  return 9999;
}

uint8_t Sensors::readProximity(void){
  if (isAPDSReady) {
    return apds->readProximity();
  }
  return 9999;
}

float Sensors::readTemperature(void){
  if (isBMPReady) {
    return bmp->readTemperature();
  }
  return 9999.0;
}

float Sensors::readPressure(void){
  if (isBMPReady) {
    return bmp->readPressure();
  }
  return 9999.0;
}

float Sensors::readAltitude(void){
  if (isBMPReady) {
    return bmp->readAltitude(1013.25);
  }
  return 9999.0;
}

void Sensors::readMagnetometer(void){
  if (isLIS3MDLReady) {
    lis3mdl->read();
  }
}

float Sensors::getMagneticX(void){
  return lis3mdl->x;
}

float Sensors::getMagneticY(void){
  return lis3mdl->y;
}

float Sensors::getMagneticZ(void){
  return lis3mdl->z;
}

void Sensors::readAccelerometer(void){
  if (isLSM6DSReady) {
    lsm6ds33->getEvent(&accel, &gyro, &temp);
  }
}

float Sensors::getAccelX(void){
  return accel.acceleration.x;
}

float Sensors::getAccelY(void){
  return accel.acceleration.y;
}

float Sensors::getAccelZ(void){
  return accel.acceleration.z;
}

float Sensors::getAccelAbs(void){
  return sqrt(pow(accel.acceleration.x, 2) + pow(accel.acceleration.y, 2) + pow(accel.acceleration.z - 9.8, 2));
}

float Sensors::getGyroX(void){
  return gyro.gyro.x;
}

float Sensors::getGyroY(void){
  return gyro.gyro.y;
}

float Sensors::getGyroZ(void){
  return gyro.gyro.z;
}

