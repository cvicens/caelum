/*
  Display.cpp - Library to display screens.
  Created by Carlos V.
  Released into the public domain.
*/

#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include "Display.h"

//Display::sh1107 = Adafruit_SH1107(64, 128, &Wire);

Display::Display()
{
  this->initialized = false;
  this->sh1107 = new Adafruit_SH1107(64, 128, &Wire);

  //init(2);
}

bool Display::init(uint16_t retry){
  do {
    Serial.println("->Display::init()");
    if (sh1107->begin(0x3C, true)) {
      Serial.println("OLED begun");
    
      // Show image buffer on the display hardware.
      // Since the buffer is intialized with an Adafruit splashscreen
      // internally, this will display the splashscreen.
      sh1107->display();
      delay(1000);
    
      // Clear the buffer.
      sh1107->clearDisplay();
      sh1107->display();
    
      sh1107->setRotation(1);

      Serial.println("Button test");
      pinMode(BUTTON_A, INPUT_PULLUP);
      pinMode(BUTTON_B, INPUT_PULLUP);
      pinMode(BUTTON_C, INPUT_PULLUP);

      this->initialized = true;
      this->enabled = true;

      return true;
    }

    delay(100);
  } while (retry--);
  
  return false;
}

void Display::on(void){
  Serial.println("Display::on");
  this->enabled = true;
}

void Display::off(void){
  Serial.println("Display::off");
  this->enabled = false;
  
  sh1107->setCursor(0,0);
  sh1107->clearDisplay();
  sh1107->display();
}

void Display::gps(const char* time, const char* latitude, const char* longitude)
{
  Serial.println(time);
  if (initialized && enabled) {
    sh1107->setTextSize(1);
    sh1107->setTextColor(SH110X_WHITE);
    sh1107->setCursor(0,0);
    sh1107->clearDisplay();
    sh1107->print("Time: ");
    sh1107->print(time);
    sh1107->print("\nLat:  ");
    sh1107->print(latitude);
    sh1107->print("\nLong: ");
    sh1107->print(longitude);

    sh1107->display();
  }
}

void Display::main(uint16_t co2, float temperature, float humidity, float vbat)
{
  char buffer[100];
  sprintf (buffer, "CO2 (ppm): %4d\nTemp (C): %4.2f\nHum (%%): %6.2f\nVBat(V): %6.2f", co2, temperature, humidity, vbat);

  if (initialized && enabled) {
    sh1107->setTextSize(1);
    sh1107->setTextColor(SH110X_WHITE);
    sh1107->setCursor(0,0);
    sh1107->clearDisplay();
    sh1107->print(buffer);

    sh1107->display();
  }
}

void Display::main(uint16_t co2, float temperature, float humidity, float vbat, float latitude, char lat, float longitude, char lon)
{
  char buffer[150];
  sprintf (buffer, "CO2 (ppm): %4d\nTemp (C): %4.2f\nHum (%%): %6.2f\nVBat(V): %6.2f\nLat: %f %c\nLon: %f %c", co2, temperature, humidity, vbat, latitude, lat, longitude, lon);

  if (initialized && enabled) {
    sh1107->setTextSize(1);
    sh1107->setTextColor(SH110X_WHITE);
    sh1107->setCursor(0,0);
    sh1107->clearDisplay();
    sh1107->print(buffer);

    sh1107->display();
  }
}


