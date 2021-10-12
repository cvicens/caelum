/*
  BMP280.cpp - Library for BMP280.
  Created by Carlos V.
  Released into the public domain.
*/

#include <Wire.h>

#include "BMP280.h"

BMP280::BMP280()
    : Sensor("BMP280")
{
    bmp = new Adafruit_BMP280();
}

bool BMP280::init(void)
{
    if (bmp->begin()) {
        initialized = true;
    } else {
        initialized = false;
    }

    return initialized;
}

bool BMP280::read(void)
{
    if (initialized) {
        this->temperature = bmp->readTemperature();
        this->pressure = bmp->readPressure();
        this->altitude = bmp->readAltitude(1013.25);
        this->valid = true;
    } else {
        this->valid = false;
    }
    return this->valid;
}

uint8_t* BMP280::uplinkPayload(void)
{
    static uint8_t payload[PAYLOAD_SIZE];

    // TODO

    return payload;
}

void BMP280::debug(void)
{
    if (!initialized) {
        Serial.println("BMP280::debug(void) run .init()");
        return;
    }
    Serial.print("BMP280::Temperature: ");
    Serial.print(getTemperature());
    Serial.print("\t");
    Serial.print("BMP280::Pressure: ");
    Serial.print(getPressure());
    Serial.print("\t");
    Serial.print("BMP280::Altitude: ");
    Serial.println(getAltitude());
}

float BMP280::getTemperature(void) { return this->temperature; }

float BMP280::getPressure(void) { return this->pressure; }

float BMP280::getAltitude(void) { return this->altitude; }