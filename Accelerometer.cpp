/*
  Accelerometer.cpp - Library for Accelerometer.
  Created by Carlos V.
  Released into the public domain.
*/

#include "Arduino.h"
#include <Wire.h>
#include <arduino_lmic.h>

#include "Accelerometer.h"

Accelerometer::Accelerometer()
    : Sensor("ACCELEROMETER", PAYLOAD_SIZE)
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
    // static uint8_t payload[PAYLOAD_SIZE];

    // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
    uint16_t uintAccAbs = Sensor::sf2u16(this->getAccelAbs());
    // place the bytes into the payload
    this->payload[0] = lowByte(uintAccAbs);
    this->payload[1] = highByte(uintAccAbs);

    return this->payload;
}

// uint8_t* Accelerometer::uplinkPayload(void)
// {
//     // static uint8_t payload[PAYLOAD_SIZE];

//     // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
//     uint16_t uintAccX = LMIC_f2sflt16(this->accelerationX);
//     // place the bytes into the payload
//     this->payload[0] = lowByte(uintAccX);
//     this->payload[1] = highByte(uintAccX);

//     // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
//     uint16_t uintAccY = LMIC_f2sflt16(this->accelerationY);
//     // place the bytes into the payload
//     this->payload[2] = lowByte(uintAccY);
//     this->payload[3] = highByte(uintAccY);

//     // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
//     uint16_t uintAccZ = LMIC_f2sflt16(this->accelerationZ);
//     // place the bytes into the payload
//     this->payload[4] = lowByte(uintAccZ);
//     this->payload[5] = highByte(uintAccZ);

//     // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
//     uint16_t uintGyrX = LMIC_f2sflt16(this->gyroX);
//     // place the bytes into the payload
//     this->payload[6] = lowByte(uintGyrX);
//     this->payload[7] = highByte(uintGyrX);

//     // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
//     uint16_t uintGyrY = LMIC_f2sflt16(this->gyroY);
//     // place the bytes into the payload
//     this->payload[8] = lowByte(uintGyrY);
//     this->payload[9] = highByte(uintGyrY);

//     // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
//     uint16_t uintGyrZ = LMIC_f2sflt16(this->gyroZ);
//     // place the bytes into the payload
//     this->payload[10] = lowByte(uintGyrZ);
//     this->payload[11] = highByte(uintGyrZ);

//     return this->payload;
// }

void Accelerometer::debug(void)
{
    if (!initialized) {
        Serial.println("Accelerometer::debug(void) run .init()");
        return;
    }

    Serial.print("AccX: ");Serial.print(getAccelX());Serial.print("\t");Serial.print("AccY: ");Serial.print(getAccelY());Serial.print("\t");Serial.print("AccZ: ");Serial.println(getAccelZ());
    Serial.print("GyrX: ");Serial.print(getGyroX()); Serial.print("\t");Serial.print("GyrX: ");Serial.print(getGyroY()); Serial.print("\t");Serial.print("GyrX: ");Serial.println(getGyroZ());
}