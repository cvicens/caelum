/*
  Morse.h - Library for Sensors.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef Sensors_h
#define Sensors_h

#include "Arduino.h"

#include "Adafruit_APDS9960.h"
#include <Adafruit_BMP280.h>

class Sensors
{
  private:
    Adafruit_APDS9960 *apds;
    Adafruit_BMP280 *bmp;
    bool initialized;

    bool isAPDSReady;
    bool isBMPReady;

    bool initAPDS(void);
    bool initBMP(void);

  public:
    Sensors(void);

    bool init(void);

    float readVBat(void);

    uint8_t readGesture(void);
    uint8_t readProximity(void);

    float readTemperature(void);
    float readPressure(void);
    float readAltitude(void);
};

#endif

