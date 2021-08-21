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
    Serial.println("APDS9960 begun");
  
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
    Serial.println("BMP280 begun");

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
  Serial.print("\nSensors::isAPDSReady "); Serial.print("false");
  return 9999;
}

uint8_t Sensors::readProximity(void){
  if (isAPDSReady) {
    return apds->readProximity();
  }
  Serial.print("\nSensors::isAPDSReady "); Serial.print("false");
  return 9999;
}

float Sensors::readTemperature(void){
  if (isBMPReady) {
    return bmp->readTemperature();
  }
  Serial.print("\nSensors::isBMPReady "); Serial.print("false");
  return 9999.0;
}

float Sensors::readPressure(void){
  if (isBMPReady) {
    return bmp->readPressure();
  }
  Serial.print("\nSensors::isBMPReady "); Serial.print("false");
  return 9999.0;
}

float Sensors::readAltitude(void){
  if (isBMPReady) {
    return bmp->readAltitude(1013.25);
  }
  Serial.print("\nSensors::isBMPReady "); Serial.print("false");
  return 9999.0;
}

