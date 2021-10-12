/*
  Magnetometer.h - Library for Magnetometer.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef Magnetometer_h
#define Magnetometer_h

#include "Arduino.h"

#include "Sensor.h"

#include <Adafruit_LIS3MDL.h>

#define PAYLOAD_SIZE 10

class Magnetometer : public Sensor {
private:
    Adafruit_LIS3MDL* lis3mdl; // magnetometer

    bool ready { false };

    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;

    bool magnetometerValid { false };
    float magneticX { 0.0 };
    float magneticY { 0.0 };
    float magneticZ { 0.0 };

public:
    Magnetometer(void);

    bool init(void);
    bool read(void);

    uint8_t* uplinkPayload(void);

    float getMagneticX(void);
    float getMagneticY(void);
    float getMagneticZ(void);

    void debug(void);
};

#endif
