/*
  BMP280.h - Library for BMP280.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef BMP280_h
#define BMP280_h

#include <Arduino.h>
#include <Adafruit_BMP280.h>

#include "Sensor.h"

#define PAYLOAD_SIZE 10

class BMP280 : public Sensor
{
  private:
    Adafruit_BMP280 *bmp;        // temperature, barometric pressure
    
    float temperature {0.0};
    float pressure {0.0};
    float altitude {0.0};

  public:
    BMP280(void);

    bool init(void);
    bool read(void);

    uint8_t* uplinkPayload(void);

    void debug(void);

    float getTemperature(void);
    float getPressure(void);
    float getAltitude(void);
};

#endif

