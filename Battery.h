/*
  Battery.h - Library for Battery.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef Battery_h
#define Battery_h

#include "Arduino.h"

#include "Sensor.h"

#define PAYLOAD_SIZE 2

class Battery : public Sensor
{
  private:
    float vBat {0.0};

  public:
    Battery(void) : Sensor("BATTERY", PAYLOAD_SIZE) {}

    bool init(void);
    bool read(void);

    uint8_t* uplinkPayload(void);

    float getVBat(void);

    void debug(void);
};

#endif

