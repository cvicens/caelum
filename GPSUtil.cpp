/*
  GPSUtil.cpp - Library for GPSUtil.
  Created by Carlos V.
  Released into the public domain.
*/

#include "Arduino.h"
#include <Wire.h>

#include "GPSUtil.h"

GPSUtil::GPSUtil()
{
  initialized = false;

  gps = Adafruit_GPS(&GPSSerial);
}

bool GPSUtil::init(void){
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  gps.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  //gps.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  if (GPSSerial) {
    gps.sendCommand(PMTK_Q_RELEASE);
    initialized = true;
    return true;
  }
  
  return false;
}

bool GPSUtil::isInit(void){
  return initialized;
}

bool GPSUtil::read(void){
  if (!initialized){
      Serial.println("GPSUtil::read(void) run .init()");
      return false; // we can fail to parse a sentence in which case we should just wait for another
  }

  // Read data from the GPS => Very often!
  char c = gps.read();
  return c != 0;
}

bool GPSUtil::parse(void){
  if (!initialized){
      Serial.println("GPSUtil::parse(void) run .init()");
      return false; // we can fail to parse a sentence in which case we should just wait for another
  }

  //Serial.print("GPSUtil::parse(void)");
  // if a sentence is received, we can check the checksum, parse it...
  if (gps.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    //Serial.println("GPSUtil::parse(void)->newNMEAreceived");
    Serial.print(gps.lastNMEA()); // this also sets the newNMEAreceived() flag to false

    if (!gps.parse(gps.lastNMEA())){ // this also sets the newNMEAreceived() flag to false
      Serial.println("GPSUtil::parse(void) FAILURE PARSING");
      return false; // we can fail to parse a sentence in which case we should just wait for another
    }
    //Serial.print("\nGPSUtil::parse(void)-> OK");
  }
  return true;
}

void GPSUtil::debug(void){
  if (!initialized){
      Serial.println("GPSUtil::debug(void) run .init()");
      return;
  }

  Serial.print("Fix: "); Serial.print((int)gps.fix);
  Serial.print(" quality: "); Serial.println((int)gps.fixquality);
  if (gps.fix) {
    Serial.print("Location: ");
    Serial.print(gps.latitude, 4); Serial.print(gps.lat);
    Serial.print(", ");
    Serial.print(gps.longitude, 4); Serial.println(gps.lon);
    Serial.print("Speed (knots): "); Serial.println(gps.speed);
    Serial.print("Angle: "); Serial.println(gps.angle);
    Serial.print("Altitude: "); Serial.println(gps.altitude);
    Serial.print("Satellites: "); Serial.println((int)gps.satellites);
  }

}

uint8_t GPSUtil::day(void){
  return initialized ? gps.day : 0;
}

uint8_t GPSUtil::month(void){
  return initialized ? gps.month : 0;
}

uint8_t GPSUtil::year(void){
  return initialized ? gps.year : 0;
}

uint8_t GPSUtil::hour(void){
  return initialized ? gps.hour : 0;
}

uint8_t GPSUtil::minute(void){
  return initialized ? gps.minute : 0;
}

uint8_t GPSUtil::seconds(void){
  return initialized ? gps.seconds : 0;
}

bool GPSUtil::fix(void){
  return initialized ? gps.fix : 0;
}

uint8_t GPSUtil::fixquality(void){
  return initialized ? gps.fixquality : 0;
}

nmea_float_t GPSUtil::latitude(void){
  return initialized ? gps.latitude : 0;
}

char GPSUtil::lat(void){
  return initialized ? gps.lat : 0;
}

nmea_float_t GPSUtil::longitude(void){
  return initialized ? gps.longitude : 0;
}

char GPSUtil::lon(void){
  return initialized ? gps.lon : 0;
}

nmea_float_t GPSUtil::speed(void){
  return initialized ? gps.speed : 0;
}

nmea_float_t GPSUtil::altitude(void){
  return initialized ? gps.altitude : 0;
}

nmea_float_t GPSUtil::angle(void){
  return initialized ? gps.angle : 0;
}

uint8_t GPSUtil::satellites(void){
  return initialized ? gps.satellites : 0;
}