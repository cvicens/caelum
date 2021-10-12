/*
  APDS9960.cpp - Library for APDS9960.
  Created by Carlos V.
  Released into the public domain.
*/

#include <Wire.h>

#include "APDS9960.h"

APDS9960::APDS9960()
    : Sensor("APDS9960")
{
    apds = new Adafruit_APDS9960();
}

bool APDS9960::init(void)
{
    if (apds->begin()) {
        // gesture mode will be entered once proximity mode senses something close
        apds->enableProximity(true);
        apds->enableGesture(true);
        initialized = true;
    } else {
        initialized = false;
    }

    return initialized;
}

bool APDS9960::read(void)
{
    if (initialized) {
        this->gesture = apds->readGesture();
        this->proximity = apds->readProximity();
        this->valid = true;
    } else {
        this->valid = false;
    }
    return this->valid;
}

uint8_t* APDS9960::uplinkPayload(void)
{
    static uint8_t payload[PAYLOAD_SIZE];

    // TODO

    return payload;
}

void APDS9960::debug(void)
{
    if (!initialized) {
        Serial.println("APDS9960::debug(void) run .init()");
        return;
    }

    Serial.print("APDS9960::Gesture: ");
    Serial.println(getGesture());
    Serial.print("APDS9960::Proximity: ");
    Serial.println(getProximity());
}

uint8_t APDS9960::getGesture(void) { return this->gesture; }

uint8_t APDS9960::getProximity(void) { return this->proximity; }
