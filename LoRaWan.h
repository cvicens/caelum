/*
  LoRaWan.h - Library for LoRaWan.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef LoRaWan_h
#define LoRaWan_h

#include "Arduino.h"

#include <arduino_lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <memory>

#include "Sensor.h"

class LoRaWan
{
  private:
    void send(osjob_t* j);

  public:
    static bool initialized;
    static std::unique_ptr<Sensor> *sensor;
    static uint8_t numSensors;


    // static bool init(std::unique_ptr<Sensor> *sensor, uint8_t numSensors);
    static bool init(void);
    static inline bool isInitialized(void) {
      return initialized;
    }
    static void runLoopOnce(void);
    
};

#endif

