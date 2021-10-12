/*
  Accelerometer.h - Library for Accelerometer.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef Accelerometer_h
#define Accelerometer_h

#include "Arduino.h"

#include "Sensor.h"

#include <Adafruit_LSM6DS33.h>

#define PAYLOAD_SIZE 10
#define ACCEL_ABS_THRESHOLD 1.5

class Accelerometer : public Sensor
{
  private:
    Adafruit_LSM6DS33 *lsm6ds33; // accelerometer, gyroscope
    
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    
    float accelerationX {0.0};
    float accelerationY {0.0};
    float accelerationZ {0.0};
    float accelerationAbs {0.0};
    float gyroX {0.0};
    float gyroY {0.0};
    float gyroZ {0.0};

    bool initAPDS(void);
    bool initBMP(void);
    bool initLSM6DS(void);
    bool initLIS3MDL(void);

  public:
    Accelerometer(void);

    bool init(void);
    bool read(void);

    uint8_t* uplinkPayload(void);
    
    float getAccelX(void);
    float getAccelY(void);
    float getAccelZ(void);
    float getAccelAbs(void);
    float getGyroX(void);
    float getGyroY(void);
    float getGyroZ(void);

    void debug(void);
};

#endif

