/*
  APDS9960.h - Library for APDS9960.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef APDS9960_h
#define APDS9960_h

#include <Adafruit_APDS9960.h>
#include <Arduino.h>

#include "Sensor.h"

#define PAYLOAD_SIZE 2

class APDS9960 : public Sensor {
private:
    Adafruit_APDS9960* apds; // temperature, barometric pressure

    uint8_t gesture { 0 };
    uint8_t proximity { 0 };

public:
    APDS9960(void);

    bool init(void);
    bool read(void);

    uint8_t* uplinkPayload(void);

    void debug(void);

    uint8_t getGesture(void);
    uint8_t getProximity(void);
};

#endif
