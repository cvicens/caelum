#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GPS.h>
#include <Adafruit_NeoPixel.h>
#include <stdbool.h>
#include <nrf.h>

#include "Display.h"
#include "DataLogger.h"
#include "Timers.h"

#include "SCD41.h"
#include "Accelerometer.h"
#include "APDS9960.h"
#include "Battery.h"
#include "BMP280.h"
#include "Magnetometer.h"
#include "GPSUtil.h"

#include "LoRaWan.h"


// Manual timer
uint32_t timer = millis();
const uint32_t PERIOD = 15000;

// OLED Display wrapper
Display display = Display();

// CO2 Sensor wrapper
SCD41 scd41;

// Accelerometer
Accelerometer accelerometer;

// Accelerometer
APDS9960 apds9960;

// Battery
Battery battery;

// BMP280 temp/press/alt
BMP280 bmp280;

// Magnetometer
Magnetometer magnetometer;

// Data logging
DataLogger dataLogger;

// GPS util
GPSUtil gps;

// NeoPixel
#define PIN PIN_NEOPIXEL
#define NEOPIXEL_BRIGHTNESS 5
Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);;

// Sensors queue
std::unique_ptr<Sensor> sensors[] = { 
  // std::unique_ptr<BMP280>(&bmp280),               // BMP280
  // std::unique_ptr<Magnetometer>(&magnetometer),    // Magnetometer
  std::unique_ptr<Accelerometer>(&accelerometer),  // Accelerometer
  std::unique_ptr<Battery>(&battery),              // Battery
  std::unique_ptr<SCD41>(&scd41),                  // CO2
  std::unique_ptr<GPSUtil>(&gps),                  // GPS
};

// LoRaWan class only exposes static methods... the library is not very C++ friendly
bool LoRaWan::initialized = false;
std::unique_ptr<Sensor>* LoRaWan::sensor = sensors;
uint8_t LoRaWan::numSensors = sizeof(sensors)/sizeof(sensors[0]);
		
extern "C"
{
  /** TIMTER2 peripheral interrupt handler. This interrupt handler is called whenever there it a TIMER2 interrupt
   */
  void TIMER2_IRQHandler(void)
  {
    // gps.readBytes();
    //LoRaWan::runLoopOnce();
    if ((NRF_TIMER2->EVENTS_COMPARE[0] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0))
    {
      NRF_TIMER2->EVENTS_COMPARE[0] = 0;           //Clear compare register 0 event	
      //nrf_gpio_pin_set(GPIO_TOGGLE_PIN);           //Set LED
      // digitalWrite(LED_BUILTIN, HIGH);
      digitalToggle(LED_BUILTIN); // turn the LED on (HIGH is the voltage level)
      //Serial.print("ON");Serial.println(millis()); 
    }
    
    if ((NRF_TIMER2->EVENTS_COMPARE[1] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE1_Msk) != 0))
    {
      NRF_TIMER2->EVENTS_COMPARE[1] = 0;           //Clear compare register 1 event
      //nrf_gpio_pin_clear(GPIO_TOGGLE_PIN);         //Clear LED
      //digitalWrite(LED_BUILTIN, LOW);
      // Serial.print("OFF");Serial.println(millis()); 
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Init OLED display 2 retries
  if (display.init(2)) {
    Serial.println("OLED OK");
  } else {
    Serial.println("OLED KO");
  }

  // Init selected sensors
  for(const auto& sensor : sensors) {
    if (sensor->init()) {
      Serial.print(sensor->getName());Serial.println(" OK");
    } else {
      Serial.print(sensor->getName());Serial.println(" KO");
    }  
  }

  if (dataLogger.init()) {
    Serial.println("DATA_LOGGING OK");
  } else {
    Serial.println("DATA_LOGGING KO");
  }

  if (gps.init()) {
    Serial.println("GPS OK");
  } else {
    Serial.println("GPS KO");
  }

  // Config Neopixels
  neopixel.begin();
  neopixel.clear();
  neopixel.setBrightness(NEOPIXEL_BRIGHTNESS);
  neopixel.show(); // Update the pixel state

  // Init LoraWan layer
  LoRaWan::init();

  // IRQ Timers
  pinMode(LED_BUILTIN, OUTPUT);
  start_timer();
}

void loop() {
  //NVIC_SystemReset(); DON'T USE FOR NOW!!!
  gps.readBytes();
  LoRaWan::runLoopOnce();

  yield();

  // approximately every PERIOD milliseconds or so, print out the current stats
  if (millis() - timer > PERIOD) {
    // Timer and GPS
    timer = millis(); // reset the timer
    gps.read();
    Serial.print("FIX: ");Serial.println(gps.fix());

    // Read Sensors
    // for(const auto& sensor : sensors) {
    //   if (sensor->read()) {
    //     sensor->debug();
    //   } else {
    //     Serial.print("ERROR READING "); Serial.println(sensor->getName());
    //   }  
    // }
    
    // Read VBat
    // float vBat = battery.isValid() ? battery.getVBat() : 0.0;

    // Read CO2 periodic measurement
    if (scd41.isValid()) {
      if (!gps.fix()) {
        display.main(scd41.getCO2(), scd41.getTemperature(), scd41.getHumidity(), battery.getVBat());
      } else {
        display.main(scd41.getCO2(), scd41.getTemperature(), scd41.getHumidity(), battery.getVBat(), gps.latitude(), gps.lat(), gps.longitude(), gps.lon());
      }
    } else {
        Serial.println(scd41.getErrorMessage());
    }

    // Display info or not?
    accelerometer.read();
    //Serial.print("\nAccelAbs: ");Serial.print(accelerometer.getAccelAbs());Serial.println();
    if (accelerometer.getAccelAbs() < ACCEL_ABS_THRESHOLD) {
      display.off();
      neopixel.clear();
      neopixel.show();
    } else {
      display.on();
      neopixel.show();
    }

    // Color code
    neopixel.setPixelColor(0, SCD41::getColor(scd41.getCO2()));

    // Data logging
    char data[50];
    // CO2 (ppm), Temp (C), Hum (%)
    //sprintf (data, "%02d/%02d/%04d %02d:%02d:%02d,%d,%.2f,%.2f,%.2f,%.2f,%.2f", gps.day(), gps.month(), gps.year(), gps.hour(), gps.minute(), gps.seconds(), scd41.getCO2(), scd41.getTemperature(), scd41.getHumidity(), bmp280.getTemperature(), bmp280.getPressure(), bmp280.getAltitude());
    sprintf (data, "%02d/%02d/%04d %02d:%02d:%02d,%d,%.2f,%.2f", gps.day(), gps.month(), gps.year(), gps.hour(), gps.minute(), gps.seconds(), scd41.getCO2(), scd41.getTemperature(), scd41.getHumidity());
    dataLogger.writeLine(data);

    //gps.debug();
  }
}
