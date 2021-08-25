/*
  SCD41.cpp - Library to SCD41 screens.
  Created by Carlos V.
  Released into the public domain.
*/

#include "Arduino.h"

#include <SensirionI2CScd4x.h>
#include <Wire.h>

#include "SCD41.h"

SCD41::SCD41()
{
  startTimeMillis = millis();
  initialized = false;
  scd4x = SensirionI2CScd4x();

  //init();
}

void SCD41::print(int value, int base) {
    if (Serial) Serial.print(value, base);
}

void SCD41::print(const char* str) {
    if (Serial) Serial.print(str);
}

void SCD41::println(void) {
    if (Serial) Serial.println();
}

void SCD41::println(const char* str) {
    if (Serial) Serial.println(str);
}

void SCD41::printUint16Hex(uint16_t value) {
    print(value < 4096 ? "0" : "");
    print(value < 256 ? "0" : "");
    print(value < 16 ? "0" : "");
    print(value, HEX);
}

void SCD41::printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) {
    print("Serial: 0x");
    printUint16Hex(serial0);
    printUint16Hex(serial1);
    printUint16Hex(serial2);
    println();
}

bool SCD41::init(void){
    Wire.begin();

    uint16_t error;
    char errorMessage[256];

    scd4x.begin(Wire);

    // stop potentially previously started measurement
    error = scd4x.stopPeriodicMeasurement();
    if (error) {
        print("Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        println(errorMessage);
        return false;
    }

    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;
    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error) {
        print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        println(errorMessage);
        return false;
    } else {
        printSerialNumber(serial0, serial1, serial2);
    }

    // Start Measurement
    error = scd4x.startPeriodicMeasurement();
    if (error) {
        print("Error trying to execute startPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        println(errorMessage);
        return false;
    }

    // Force 5sec there should be a better way...
    uint32_t elapsedTimeMillis = millis() - startTimeMillis;
    if ((DATASHEET_DELAY - elapsedTimeMillis) > 0) {
        Serial.print("About to wait (ms)... ");
        Serial.println(DATASHEET_DELAY - elapsedTimeMillis);
        delay(DATASHEET_DELAY - elapsedTimeMillis);
    } else {
        Serial.print("No wait needed time elapsed (ms)... ");
        Serial.println(elapsedTimeMillis);
    }
    
    return true;
}

bool SCD41::readMeasurement(uint16_t& co2, float& temperature, float& humidity, uint16_t& error, char errorMessage[], size_t errorMessageSize){
    // Read Measurement
    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        println(errorMessage);
        return false;
    } else if (co2 == 0) {
        println("Invalid sample detected, skipping.");
        strncpy(errorMessage, "Invalid sample detected, skipping.", 256);
        return false;
    }

    #ifdef DEBUG
    Serial.print("CO2: ");
    Serial.print(co2);
    Serial.print("\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("\t");
    Serial.print("Humidity: ");
    Serial.println(humidity);
    #endif

    return true;
}
