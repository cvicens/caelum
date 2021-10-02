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

class LoRaWan
{
  private:
    bool initialized;

  public:
    LoRaWan(void);

    bool init(void);
    bool isInit(void);
    void runLoopOnce(void);
    
};

#endif

