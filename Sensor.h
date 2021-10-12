/*
  Sensor.h - Base Class for sensors.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef Sensor_h
#define Sensor_h

// #include <Arduino_LoRaWAN_network.h>
// #include <Arduino_LoRaWAN_EventLog.h>
//#include <arduino_lmic.h>

#include "Arduino.h"
#include <stdint.h>

class Sensor
{
  protected:
    const char* name {nullptr};
    bool initialized {false};
    bool valid {false};

  public:
    Sensor(const char* _name) : name (_name), initialized (false), valid (false) {}

    virtual bool init(void) = 0;

    virtual bool read(void) = 0;
    virtual uint8_t* uplinkPayload(void) = 0;

    virtual void debug(void) = 0;

    inline const char* getName(void) {
      return this->name;
    }
    
    inline bool isInitialized(void) {
      return this->initialized;
    }

    inline bool isValid(void) {
      return this->valid;
    }
};

#endif

