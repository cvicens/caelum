/*
  GPSUtil.h - Library for GPSUtil.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef GPSUtil_h
#define GPSUtil_h

#include "Arduino.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GPS.h>

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

class GPSUtil
{
  private:
    bool initialized;

    // Connect to the GPS on the hardware port
    //Adafruit_GPS GPS;

  public:
    // Connect to the GPS on the hardware port
    Adafruit_GPS gps;

    GPSUtil(void);

    bool init(void);
    bool read(void);
    bool parse(void);

    uint8_t day(void);
    uint8_t month(void);
    uint8_t year(void);
    uint8_t hour(void);
    uint8_t minute(void);
    uint8_t seconds(void);
    bool fix(void);
    uint8_t fixquality(void);
    nmea_float_t latitude(void);
    char lat(void);
    nmea_float_t longitude(void);
    char lon(void);
    nmea_float_t speed(void);
    nmea_float_t altitude(void);
    nmea_float_t angle(void);
    uint8_t satellites(void);
};

#endif

