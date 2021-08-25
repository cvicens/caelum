/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#ifndef Display_h
#define Display_h

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define BUTTON_A 31
#define BUTTON_B 30
#define BUTTON_C 27

class Display
{
  private:
    Adafruit_SH1107 *sh1107;
    bool initialized;
    bool enabled;

  public:
    Display(void);

    bool init(uint16_t);

    void off(void);
    void on(void);

    void gps(const char*, const char*, const char*);
    void main(uint16_t co2, float temperature, float humidity, float vbat);
    void main(uint16_t co2, float temperature, float humidity, float vbat, float latitude, char lat, float longitude, char lon);
    
};

#endif

