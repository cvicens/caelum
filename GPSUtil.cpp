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
    GPSSerial.println(PMTK_Q_RELEASE);
    initialized = true;
    return true;
  }
  
  return false;
}

bool GPSUtil::read(void){
  // Read data from the GPS => Very often!
  
  return gps.read() != 0;
}

bool GPSUtil::parse(void){
  // if a sentence is received, we can check the checksum, parse it...
  if (gps.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    //Serial.print(gps.lastNMEA()); // this also sets the newNMEAreceived() flag to false

    if (!gps.parse(gps.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return false; // we can fail to parse a sentence in which case we should just wait for another
  }
  return true;
}

uint8_t GPSUtil::day(void){
  return gps.day;
}

uint8_t GPSUtil::month(void){
  return gps.month;
}

uint8_t GPSUtil::year(void){
  return gps.year;
}

uint8_t GPSUtil::hour(void){
  return gps.hour;
}

uint8_t GPSUtil::minute(void){
  return gps.minute;
}

uint8_t GPSUtil::seconds(void){
  return gps.seconds;
}

bool GPSUtil::fix(void){
  return gps.month;
}

uint8_t GPSUtil::fixquality(void){
  return gps.fixquality;
}

nmea_float_t GPSUtil::latitude(void){
  return gps.latitude;
}

char GPSUtil::lat(void){
  return gps.lat;
}

nmea_float_t GPSUtil::longitude(void){
  return gps.longitude;
}

char GPSUtil::lon(void){
  return gps.lon;
}

nmea_float_t GPSUtil::speed(void){
  return gps.speed;
}

nmea_float_t GPSUtil::altitude(void){
  return gps.altitude;
}

nmea_float_t GPSUtil::angle(void){
  return gps.angle;
}

uint8_t GPSUtil::satellites(void){
  return gps.satellites;
}