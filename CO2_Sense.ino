#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GPS.h>

#include "Display.h"
#include "SCD41.h"
#include "Sensors.h"
#include "DataLogger.h"

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

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

boolean setupGPS(uint16_t retry){
  do {
      // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
      GPS.begin(9600);
      // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
      GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
      // uncomment this line to turn on only the "minimum recommended" data
      //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
      // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
      // the parser doesn't care about other sentences at this time
      // Set the update rate
      GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
      // For the parsing code to work nicely and have time to sort thru the data, and
      // print it out we don't suggest using anything higher than 1 Hz
    
      // Request updates on antenna status, comment out to keep quiet
      //GPS.sendCommand(PGCMD_ANTENNA);
    
      delay(1000);
    
      // Ask for firmware version
      if (GPSSerial) {
        GPSSerial.println(PMTK_Q_RELEASE);
        return true;
      }

    delay(100);
  } while (retry--);
  
  return false;
}

// boolean setupSCD4X(uint16_t retry) {
//   do {
//     if (scd41.init()) {
//         return true;
//     }

//     Serial.println("SCD41 sensor is not ready or available");
    
//     delay(100);
//   } while (retry--);
  
//   return false;
// }

char* parseGPSTime(void) {
  char buffer[256];
  sprintf(buffer, "%02i:%02i:%02i", GPS.hour, GPS.minute, GPS.seconds);
  Serial.println("Hora 1");
  Serial.println(GPS.hour);
  Serial.println("Hora 2");
  Serial.println(buffer);
  Serial.println();
  return buffer;
}

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
  } else {
    Serial.println("DATA_LOGGING KO");
  }

  // Setup GPS
  if (setupGPS(2)) {
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
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  // if (GPSECHO)
  //   if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every PERIOD seconds or so, print out the current stats
  if (millis() - timer > PERIOD) {
    // Read VBat
    float vbat = sensors.readVBat();
    Serial.print("VBAT(V): ");Serial.println(vbat);

    // Read CO2 periodic measurement
    uint16_t error;
    char errorMessage[256];
    uint16_t scd41Co2;
    float scd41Temperature;
    float scd41Humidity;
    Serial.print("CO2:");
    Serial.print(scd41Co2);
    Serial.print("\t");
    Serial.print("Temperature:");
    Serial.print(scd41Temperature);
    Serial.print("\t");
    Serial.print("Humidity:");
    Serial.println(scd41Humidity);
    if (scd41.readMeasurement(scd41Co2, scd41Temperature, scd41Humidity, error, errorMessage, 256)) {
        display.main(scd41Co2, scd41Temperature, scd41Humidity, vbat);
    } else {
        Serial.println(errorMessage);
    }

    // Read Sensors
    float senseTemperature = sensors.readTemperature();
    float sensePressure = sensors.readPressure();
    float senseAltitude = sensors.readAltitude();
    uint8_t senseProximity = sensors.readProximity();

    Serial.print("Temperature:");
    Serial.print(senseTemperature);
    Serial.print("\tTempDiff:");
    Serial.print(scd41Temperature - senseTemperature);
    Serial.print("\t");
    Serial.print("Pressure:");
    Serial.print(sensePressure);
    Serial.print("\t");
    Serial.print("Altitude:");
    Serial.println(senseAltitude);

    // Data logging
    // char data[50];
    // sprintf (data, "CO2 (ppm): %4d \nTemp (C): %4.2f \nHum (%%):  %4.2f", scd41Co2, scd41Temperature, scd41Humidity);
    // dataLogger.writeLine(data);

    // Timer and GPS
    timer = millis(); // reset the timer

    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);

      //display.mainWithLatLong(co2, temperature, humidity, GPS.latitude, GPS.longitude);
    }
  }
}
