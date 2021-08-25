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

  apds = new Adafruit_APDS9960();
  bmp = new Adafruit_BMP280();

  //init(2);
}

bool Sensors::initAPDS(void){
  Serial.println("-->Sensors::initAPDS()");
  if (apds->begin()) {  
    //gesture mode will be entered once proximity mode senses something close
    apds->enableProximity(true);
    apds->enableGesture(true);

    isAPDSReady = true;

    return true;
  }

  return false;
}

bool Sensors::initBMP(void){
  Serial.println("-->Sensors::initBMP()");
  if (bmp->begin()) {
    isBMPReady = true;

    return true;
  }

  return false;
}

bool Sensors::init(void){
  initAPDS();
  initBMP();

  return isAPDSReady && isBMPReady;
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

