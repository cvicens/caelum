#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GPS.h>
//#include "Adafruit_ZeroTimer.h"

#include "Display.h"
#include "SCD41.h"
#include "Sensors.h"
#include "DataLogger.h"
#include "GPSUtil.h"

// // what's the name of the hardware serial port?
// #define GPSSerial Serial1

// // Connect to the GPS on the hardware port
// Adafruit_GPS GPS(&GPSSerial);

// // Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// // Set to 'true' if you want to debug and listen to the raw GPS sentences
// #define GPSECHO false

// Manual timer
uint32_t timer = millis();
const uint32_t PERIOD = 5000;

// OLED Display wrapper
Display display = Display();

// CO2 Sensor wrapper
SCD41 scd41 = SCD41();

// Sensors wrapper
Sensors sensors = Sensors();

// Data logging
DataLogger dataLogger = DataLogger();

// GPS util
GPSUtil gps = GPSUtil();

// boolean setupGPS(uint16_t retry){
//   do {
//       // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
//       GPS.begin(9600);
//       // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
//       GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
//       // uncomment this line to turn on only the "minimum recommended" data
//       //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
//       // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
//       // the parser doesn't care about other sentences at this time
//       // Set the update rate
//       GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
//       // For the parsing code to work nicely and have time to sort thru the data, and
//       // print it out we don't suggest using anything higher than 1 Hz
    
//       // Request updates on antenna status, comment out to keep quiet
//       //GPS.sendCommand(PGCMD_ANTENNA);
    
//       delay(1000);
    
//       // Ask for firmware version
//       if (GPSSerial) {
//         GPSSerial.println(PMTK_Q_RELEASE);
//         return true;
//       }

//     delay(100);
//   } while (retry--);
  
//   return false;
// }

void setup() {
  Serial.begin(115200);

  // Init OLED display 2 retries
  if (display.init(2)) {
    Serial.println("OLED OK");
  } else {
    Serial.println("OLED KO");
  }

  if (scd41.init()) {
    Serial.println("SCD41 OK");
  } else {
    Serial.println("SCD41 KO");
  }

  if (sensors.init()) {
    Serial.println("SENSORS OK");
  } else {
    Serial.println("SENSORS KO");
  }

  if (dataLogger.init()) {
    Serial.println("DATA_LOGGING OK");
    //dataLogger.dumpToSerial(); // This makes the micro to stop working
  } else {
    Serial.println("DATA_LOGGING KO");
  }

  // Setup GPS
  // if (setupGPS(2)) {
  //   Serial.println("GPS OK");
  // } else {
  //   Serial.println("GPS KO");
  // }
  if (gps.init()) {
    Serial.println("GPS OK");
  } else {
    Serial.println("GPS KO");
  }

}

void loop() {
  delay(10);

  yield();

  // uint8_t gesture = sensors.readGesture();
  // if(gesture == APDS9960_DOWN) Serial.println("APDS9960_DOWN");
  // else if(gesture == APDS9960_UP) Serial.println("APDS9960_UP");
  // else if(gesture == APDS9960_LEFT) Serial.println("APDS9960_LEFT");
  // else if(gesture == APDS9960_RIGHT) Serial.println("APDS9960_RIGHT");
  // else Serial.println("==========================");

  // read data from the GPS in the 'main loop'
  // char c = GPS.read();
  gps.read();

  // approximately every PERIOD seconds or so, print out the current stats
  if (millis() - timer > PERIOD) {
    // Timer and GPS
    timer = millis(); // reset the timer
    gps.parse();

    // Read Sensors
    float senseTemperature = sensors.readTemperature();
    float sensePressure = sensors.readPressure();
    float senseAltitude = sensors.readAltitude();
    uint8_t senseProximity = sensors.readProximity();

    Serial.print("\nProximity: ");Serial.println(senseProximity);
    if (senseProximity < 25) {
      display.off();
    } else {
      display.on();
    }

    // Read VBat
    float vbat = sensors.readVBat();

    // Read CO2 periodic measurement
    uint16_t error = 0;
    char errorMessage[256];
    uint16_t scd41Co2;
    float scd41Temperature;
    float scd41Humidity;
    if (scd41.readMeasurement(scd41Co2, scd41Temperature, scd41Humidity, error, errorMessage, 256)) {
      if (!gps.fix()) {
        display.main(scd41Co2, scd41Temperature, scd41Humidity, vbat);
      } else {
        display.main(scd41Co2, scd41Temperature, scd41Humidity, vbat, gps.latitude(), gps.lat(), gps.longitude(), gps.lon());
      }
    } else {
        Serial.println(errorMessage);
    }

    // Data logging
    char data[50];
    // CO2 (ppm), Temp (C), Hum (%)
    //sprintf (data, "CO2 (ppm): %4d Temp (C): %4.2f \nHum (%%):  %4.2f", scd41Co2, scd41Temperature, scd41Humidity);
    sprintf (data, "%02d/%02d/%04d %02d:%02d:%02d,%d,%.2f,%.2f,%.2f,%.2f,%.2f", gps.day(), gps.month(), gps.year(), gps.hour(), gps.minute(), gps.seconds(), scd41Co2, scd41Temperature, scd41Humidity, senseTemperature, sensePressure, senseAltitude);
    dataLogger.writeLine(data);

    Serial.print("Fix: "); Serial.print((int)gps.fix());
    Serial.print(" quality: "); Serial.println((int)gps.fixquality());
    if (gps.fix()) {
      Serial.print("Location: ");
      Serial.print(gps.latitude(), 4); Serial.print(gps.lat());
      Serial.print(", ");
      Serial.print(gps.longitude(), 4); Serial.println(gps.lon());
      Serial.print("Speed (knots): "); Serial.println(gps.speed());
      Serial.print("Angle: "); Serial.println(gps.angle());
      Serial.print("Altitude: "); Serial.println(gps.altitude());
      Serial.print("Satellites: "); Serial.println((int)gps.satellites());
    }
  }
}
