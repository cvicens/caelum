/*
  Accelerometer.cpp - Library for Accelerometer.
  Created by Carlos V.
  Released into the public domain.
*/

#include "Arduino.h"
#include <Wire.h>

#include "Accelerometer.h"

Accelerometer::Accelerometer()
    : Sensor("ACCELEROMETER")
{
    lsm6ds33 = new Adafruit_LSM6DS33();
}

bool Accelerometer::init(void)
{
    if (lsm6ds33->begin_I2C()) {
        initialized = true;
    } else {
        initialized = false;
    }

    return initialized;
}

bool Accelerometer::read(void)
{
    if (initialized) {
        lsm6ds33->getEvent(&accel, &gyro, &temp);
        this->accelerationX = this->accel.acceleration.x;
        this->accelerationY = this->accel.acceleration.y;
        this->accelerationZ = this->accel.acceleration.z;
        this->accelerationAbs = this->getAccelAbs();
        this->gyroX = this->gyro.gyro.x;
        this->gyroY = this->gyro.gyro.y;
        this->gyroZ = this->gyro.gyro.z;
        this->valid = true;
    } else {
        this->valid = false;
    }
    return this->valid;
}

float Accelerometer::getAccelX(void) { return this->accelerationX; }

float Accelerometer::getAccelY(void) { return this->accelerationY; }

float Accelerometer::getAccelZ(void) { return this->accelerationZ; }

float Accelerometer::getAccelAbs(void)
{
    return sqrt(pow(this->accelerationX, 2) + pow(this->accelerationY, 2) + pow(this->accelerationZ - 9.8, 2));
}

float Accelerometer::getGyroX(void) { return this->gyroX; }

float Accelerometer::getGyroY(void) { return this->gyroY; }

float Accelerometer::getGyroZ(void) { return this->gyroZ; }

uint8_t* Accelerometer::uplinkPayload(void)
{
    static uint8_t payload[PAYLOAD_SIZE];

    // TODO

    return payload;
}

void Accelerometer::debug(void)
{
    if (!initialized) {
        Serial.println("Accelerometer::debug(void) run .init()");
        return;
    }

    Serial.print("AccX: ");Serial.print(getAccelX());Serial.print("\t");Serial.print("AccY: ");Serial.print(getAccelY());Serial.print("\t");Serial.print("AccZ: ");Serial.println(getAccelZ());
    Serial.print("GyrX: ");Serial.print(getGyroX()); Serial.print("\t");Serial.print("GyrX: ");Serial.print(getGyroY()); Serial.print("\t");Serial.print("GyrX: ");Serial.println(getGyroZ());
}