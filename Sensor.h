/*
  Sensor.h - Base Class for sensors.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef Sensor_h
#define Sensor_h

#include "Arduino.h"
#include <stdint.h>

class Sensor
{
  protected:
    const char* name {nullptr};
    bool initialized {false};
    bool valid {false};
    uint8_t *payload;
    const uint8_t payloadSize;

  public:
    Sensor(const char* name, uint8_t payloadSize) : name (name), initialized (false), valid (false), payload(new uint8_t[payloadSize]), payloadSize(payloadSize) {}
    ~Sensor(void) { delete payload; }

    virtual bool init(void) = 0;

    virtual bool read(void) = 0;
    virtual uint8_t* uplinkPayload(void) = 0;

    virtual void debug(void) = 0;

    inline const char* getName(void) {
      return this->name;
    }

    inline const uint8_t getPayloadSize(void) {
      return this->payloadSize;
    }
    
    inline bool isInitialized(void) {
      return this->initialized;
    }

    inline bool isValid(void) {
      return this->valid;
    }

    static inline uint16_t sf2u16(float f){
      float i {0};
      float d = modf(f, &i);

      return (uint16_t)(d * 100) << 8 | (uint8_t)i & 0xff;
    }

    static inline uint32_t gps2u32(float f){
      uint32_t fbits = 0;
      memcpy(&fbits, &f, sizeof fbits);
      return fbits;
    }
};

#endif

