/*
  Battery.cpp - Library for Battery.
  Created by Carlos V.
  Released into the public domain.
*/

#include "Arduino.h"
#include <arduino_lmic.h>

#include "Battery.h"

#define VBATPIN A6
#define ADC_RESOL 1024
#define VBAT_STEP_VAL (2 * 3.6 / (ADC_RESOL - 1) )

bool Battery::init(void){

  initialized = true;

  return initialized;
}

bool Battery::read(void)
{
    this->vBat = analogRead(VBATPIN) * VBAT_STEP_VAL;
    this->valid = true;
    
    return this->valid;
}

uint8_t* Battery::uplinkPayload(void){
  static uint8_t payload[PAYLOAD_SIZE];

  // float -> int
  // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
  uint16_t payloadVBat = LMIC_f2sflt16(this->vBat);
  // place the bytes into the payload
  payload[0] = lowByte(payloadVBat);
  payload[1] = highByte(payloadVBat);

  return payload;
}

float Battery::getVBat(void) {
  return this->vBat;
}

void Battery::debug(void){
  if (!initialized){
      Serial.println("Battery::debug(void) run .init()");
      return;
  }

  Serial.print("VBat: ");Serial.println(getVBat());
}