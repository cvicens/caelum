/*
  BMP280.cpp - Library for BMP280.
  Created by Carlos V.
  Released into the public domain.
*/

#include <Wire.h>
#include <arduino_lmic.h>

#include "BMP280.h"

BMP280::BMP280()
    : Sensor("BMP280", PAYLOAD_SIZE)
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
    // static uint8_t payload[PAYLOAD_SIZE];

    // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
    uint16_t uintTemp = LMIC_f2sflt16(this->temperature);
    // place the bytes into the payload
    this->payload[0] = lowByte(uintTemp);
    this->payload[1] = highByte(uintTemp);

    // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
    uint16_t uintPress = LMIC_f2sflt16(this->pressure);
    // place the bytes into the payload
    this->payload[2] = lowByte(uintPress);
    this->payload[3] = highByte(uintPress);

    // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
    uint16_t uintAlt = LMIC_f2sflt16(this->altitude);
    // place the bytes into the payload
    this->payload[4] = lowByte(uintAlt);
    this->payload[5] = highByte(uintAlt);

    return this->payload;
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