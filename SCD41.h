/*
  SCD41.cpp - Library to SCD41.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef SCD41_h
#define SCD41_h

#include "Arduino.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <SensirionI2CScd4x.h>

#define DATASHEET_DELAY 5000

class SCD41
{
  private:
    SensirionI2CScd4x scd4x;
    bool initialized;
    uint32_t startTimeMillis;

    void print(int value, int base);
    void print(const char* str);
    void println(void);
    void println(const char* str);
    void printUint16Hex(uint16_t value);
    void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2);

  public:
    SCD41(void);

    bool init(void);
    bool readMeasurement(uint16_t& co2, float& temperature, float& humidity, uint16_t& error, char errorMessage[], size_t errorMessageSize);

};

#endif

