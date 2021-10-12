/*
  SCD41.cpp - Library to SCD41.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef SCD41_h
#define SCD41_h

#include "Arduino.h"
#include "Sensor.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <SensirionI2CScd4x.h>

#define DATASHEET_DELAY 5500
#define PAYLOAD_SIZE 6

class SCD41 : public Sensor {
private:
    SensirionI2CScd4x scd4x;
    uint32_t startTimeMillis;

    uint16_t co2 { 0 };
    float temperature { 0.0 };
    float humidity { 0.0 };

    uint16_t error = 0;
    char errorMessage[256];

    void print(int value, int base);
    void print(const char* str);
    void println(void);
    void println(const char* str);
    void printUint16Hex(uint16_t value);
    void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2);

public:
    SCD41(void);
    ~SCD41(void);

    static uint32_t getColor(float co2);

    bool init(void);
    bool read(void);
    bool readMeasurement(uint16_t& co2, float& temperature, float& humidity, uint16_t& error, char errorMessage[],
        size_t errorMessageSize);

    uint8_t* uplinkPayload(void);

    uint16_t getCO2(void);
    float getTemperature(void);
    float getHumidity(void);

    uint16_t getError(void);
    char* getErrorMessage(void);

    void debug(void);
};

#endif
