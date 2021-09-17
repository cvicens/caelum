#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GPS.h>
#include <Adafruit_NeoPixel.h>

#include "Display.h"
#include "SCD41.h"
#include "Sensors.h"
#include "DataLogger.h"
#include "GPSUtil.h"

#include <stdbool.h>
#include "nrf.h"
#include "nrf_gpio.h"

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

// NeoPixel
#define PIN PIN_NEOPIXEL
#define NEOPIXEL_BRIGHTNESS 5
Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);;

void start_timer(void)
{		
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;  // Set the timer in Counter Mode
  NRF_TIMER2->TASKS_CLEAR = 1;               // clear the task first to be usable for later
	NRF_TIMER2->PRESCALER = 160;                             //Set prescaler. Higher number gives slower timer. Prescaler = 0 gives 16MHz timer
	NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit;		 //Set counter to 16 bit resolution
	NRF_TIMER2->CC[0] = 25000;                             //Set value for TIMER2 compare register 0
	NRF_TIMER2->CC[1] = 5;                                 //Set value for TIMER2 compare register 1
		
  // Enable interrupt on Timer 2, both for CC[0] and CC[1] compare match events
	NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos) | (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos);
  NVIC_EnableIRQ(TIMER2_IRQn);
		
  NRF_TIMER2->TASKS_START = 1;               // Start TIMER2
}
		
/** TIMTER2 peripheral interrupt handler. This interrupt handler is called whenever there it a TIMER2 interrupt
 */
void TIMER2_IRQHandler(void)
{
  Serial.println("TIMER2_IRQHandler");
	if ((NRF_TIMER2->EVENTS_COMPARE[0] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0))
  {
		NRF_TIMER2->EVENTS_COMPARE[0] = 0;           //Clear compare register 0 event	
		//nrf_gpio_pin_set(GPIO_TOGGLE_PIN);           //Set LED
    digitalWrite(LED_BUILTIN, HIGH);
    //digitalToggle(LED_BUILTIN); // turn the LED on (HIGH is the voltage level)
    Serial.println("ON");

  }
	
	if ((NRF_TIMER2->EVENTS_COMPARE[1] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE1_Msk) != 0))
  {
		NRF_TIMER2->EVENTS_COMPARE[1] = 0;           //Clear compare register 1 event
		//nrf_gpio_pin_clear(GPIO_TOGGLE_PIN);         //Clear LED
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("ON");
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

  pinMode(LED_BUILTIN, OUTPUT);
  //start_timer();
}

void loop() {
  //delay(10);

  yield();

  // read data from the GPS in the 'main loop'
  // char c = GPS.read();
  gps.read();

  // approximately every PERIOD seconds or so, print out the current stats
  if (millis() - timer > PERIOD) {
    if (!gps.isInit()) {
      gps.init();
    }

    // Timer and GPS
    timer = millis(); // reset the timer
    gps.parse();

    // Read Sensors
    float senseTemperature = sensors.readTemperature();
    float sensePressure = sensors.readPressure();
    float senseAltitude = sensors.readAltitude();
    uint8_t senseProximity = sensors.readProximity();
    
    sensors.readMagnetometer();
    sensors.readAccelerometer();

    Serial.print("\nAccelAbs: ");Serial.print(sensors.getAccelAbs());;Serial.println();
    if (sensors.getAccelAbs() < ACCEL_ABS_THRESHOLD) {
      display.off();
      neopixel.clear();
      neopixel.show();
    } else {
      display.on();
      neopixel.show();
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

    // Color code
    neopixel.setPixelColor(0, SCD41::getColor(scd41Co2));

    // Data logging
    char data[50];
    // CO2 (ppm), Temp (C), Hum (%)
    //sprintf (data, "CO2 (ppm): %4d Temp (C): %4.2f \nHum (%%):  %4.2f", scd41Co2, scd41Temperature, scd41Humidity);
    sprintf (data, "%02d/%02d/%04d %02d:%02d:%02d,%d,%.2f,%.2f,%.2f,%.2f,%.2f", gps.day(), gps.month(), gps.year(), gps.hour(), gps.minute(), gps.seconds(), scd41Co2, scd41Temperature, scd41Humidity, senseTemperature, sensePressure, senseAltitude);
    dataLogger.writeLine(data);

    gps.debug();
    sensors.debug();
  }
}
