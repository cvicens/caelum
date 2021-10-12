/*
  Magnetometer.cpp - Library for Magnetometer.
  Created by Carlos V.
  Released into the public domain.
*/

#include "Arduino.h"
#include <Wire.h>

#include "Magnetometer.h"

Magnetometer::Magnetometer()
    : Sensor("MAGNETOMETER")
{
    lis3mdl = new Adafruit_LIS3MDL();
}

bool Magnetometer::init(void)
{
    if (lis3mdl->begin_I2C()) {
        initialized = true;
    }

    return initialized;
}

bool Magnetometer::read(void)
{
    if (initialized) {
        lis3mdl->read();
        this->magneticX = this->lis3mdl->x;
        this->magneticY = this->lis3mdl->y;
        this->magneticZ = this->lis3mdl->z;
        this->valid = true;
    } else {
        this->valid = false;
    }
    return this->valid;
}

uint8_t* Magnetometer::uplinkPayload(void)
{
    static uint8_t payload[PAYLOAD_SIZE];

    // TODO

    return payload;
}

float Magnetometer::getMagneticX(void) { return lis3mdl->x; }

float Magnetometer::getMagneticY(void) { return lis3mdl->y; }

float Magnetometer::getMagneticZ(void) { return lis3mdl->z; }

void Magnetometer::debug(void)
{
    if (!initialized) {
        Serial.println("Magnetometer::debug(void) run .init()");
        return;
    }

    Serial.print("MagX: ");Serial.print(getMagneticX());Serial.print("\t");Serial.print("MagY: ");Serial.print(getMagneticY());Serial.print("\t");Serial.print("MagZ: ");Serial.println(getMagneticZ());
}
