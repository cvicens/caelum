/*
  Sensors.h - Library for Sensors.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef Sensors_h
#define Sensors_h

#include "Arduino.h"

#include <Adafruit_APDS9960.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_LSM6DS33.h>
#include <Adafruit_LIS3MDL.h>

#define ACCEL_ABS_THRESHOLD 1.5

class Sensors
{
  private:
    Adafruit_APDS9960 *apds;     // proximity, light, color, gesture
    Adafruit_BMP280 *bmp;        // temperautre, barometric pressure
    Adafruit_LSM6DS33 *lsm6ds33; // accelerometer, gyroscope
    Adafruit_LIS3MDL *lis3mdl;    // magnetometer
    
    bool initialized;
    bool isAPDSReady;
    bool isBMPReady;
    bool isLSM6DSReady;
    bool isLIS3MDLReady;

    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;

    bool initAPDS(void);
    bool initBMP(void);
    bool initLSM6DS(void);
    bool initLIS3MDL(void);

  public:
    Sensors(void);

    bool init(void);

    float readVBat(void);

    uint8_t readGesture(void);
    uint8_t readProximity(void);

    float readTemperature(void);
    float readPressure(void);
    float readAltitude(void);

    void readMagnetometer(void);
    float getMagneticX(void);
    float getMagneticY(void);
    float getMagneticZ(void);
    
    void readAccelerometer(void);
    float getAccelX(void);
    float getAccelY(void);
    float getAccelZ(void);
    float getAccelAbs(void);
    float getGyroX(void);
    float getGyroY(void);
    float getGyroZ(void);
};

#endif

