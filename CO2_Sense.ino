#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GPS.h>
#include <Adafruit_NeoPixel.h>

#include "Display.h"
#include "SCD41.h"

#include "Accelerometer.h"
#include "APDS9960.h"
#include "Battery.h"
#include "BMP280.h"
#include "Magnetometer.h"

#include "DataLogger.h"
#include "GPSUtil.h"
#include "LoRaWan.h"

#include <stdbool.h>
#include "nrf.h"

// Manual timer
uint32_t timer = millis();
const uint32_t PERIOD = 5000;

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
DataLogger dataLogger = DataLogger();

// GPS util
GPSUtil gps = GPSUtil();

// NeoPixel
#define PIN PIN_NEOPIXEL
#define NEOPIXEL_BRIGHTNESS 5
Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);;

// Sensors queue
// std::unique_ptr<Sensor> sensors[2];
// sensors[0] = std::make_unique<SCD41>();
// sensors[1] = std::make_unique<Accelerometer>(&accelerometer);
// sensorQueue.push_back(std::make_unique<Accelerometer>(&accelerometer));
std::unique_ptr<Sensor> sensors[] = { 
  std::unique_ptr<SCD41>(&scd41),                 // CO2
  std::unique_ptr<Accelerometer>(&accelerometer), // Accelerometer
  std::unique_ptr<Magnetometer>(&magnetometer),   // Magnetometer
  std::unique_ptr<Battery>(&battery),             // Battery
  std::unique_ptr<BMP280>(&bmp280),               // BMP280
};

// Lora
// LoRaWan lorawan {sensors, sizeof(sensors)};

bool LoRaWan::initialized = false;
std::unique_ptr<Sensor>* LoRaWan::sensor = sensors;
uint8_t LoRaWan::numSensors = sizeof(sensors)/sizeof(sensors[0]);

void start_timer(void)
{		
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;          // Set the timer in Counter Mode
  NRF_TIMER2->TASKS_CLEAR = 1;                       // clear the task first to be usable for later
	NRF_TIMER2->PRESCALER = 0;                        // Set prescaler. Higher number gives slower timer. 
                                                     // Prescaler = 0 gives 16MHz timer. f = 16 MHz / 2^(n)
                                                     // f 1kHz ==> T 1ms ==> log2(16000) ==> 13.96
	NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit; // Set counter to 16 bit resolution
	NRF_TIMER2->CC[0] = 10000;                         // Set value for TIMER2 compare register 0
	NRF_TIMER2->CC[1] = 5;                          // Set value for TIMER2 compare register 1
		
  // Enable interrupt on Timer 2, both for CC[0] and CC[1] compare match events
	NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos) | (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos);
  NVIC_EnableIRQ(TIMER2_IRQn);
		
  NRF_TIMER2->TASKS_START = 1;               // Start TIMER2
}
		
extern "C"
{
  /** TIMTER2 peripheral interrupt handler. This interrupt handler is called whenever there it a TIMER2 interrupt
   */
  void TIMER2_IRQHandler(void)
  {
    gps.read();
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

  for(const auto& sensor : sensors) {
    if (sensor->init()) {
      Serial.print(sensor->getName());Serial.println(" OK");
    } else {
      Serial.print(sensor->getName());Serial.println(" KO");
    }  
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

  // Init LoraWan layer
  LoRaWan::init();

  // IRQ Timers
  pinMode(LED_BUILTIN, OUTPUT);
  start_timer();
}

void loop() {
  // we call the LMIC's runloop processor. This will cause things to happen based on events and time. One
  // of the things that will happen is callbacks for transmission complete or received messages. We also
  // use this loop to queue periodic data transmissions.  You can put other things here in the `loop()` routine,
  // but beware that LoRaWAN timing is pretty tight, so if you do more than a few milliseconds of work, you
  // will want to call `os_runloop_once()` every so often, to keep the radio running.
  LoRaWan::runLoopOnce();

  yield();

  // approximately every PERIOD seconds or so, print out the current stats
  if (millis() - timer > PERIOD) {
    if (!gps.isInit()) {
      gps.init();
    }

    // Timer and GPS
    timer = millis(); // reset the timer
    gps.parse();

    // Read Sensors
    for(const auto& sensor : sensors) {
      if (sensor->read()) {
        sensor->debug();
      } else {
        Serial.println("ERROR READING "); Serial.println(sensor->getName());
      }  
    }
    
    //Serial.print("\nAccelAbs: ");Serial.print(accelerometer.getAccelAbs());Serial.println();
    if (accelerometer.getAccelAbs() < ACCEL_ABS_THRESHOLD) {
      display.off();
      neopixel.clear();
      neopixel.show();
    } else {
      display.on();
      neopixel.show();
    }

    // Read VBat
    float vBat = battery.isValid() ? battery.getVBat() : 0.0;

    // Read CO2 periodic measurement
    if (scd41.isValid()) {
      if (!gps.fix()) {
        display.main(scd41.getCO2(), scd41.getTemperature(), scd41.getHumidity(), vBat);
      } else {
        display.main(scd41.getCO2(), scd41.getTemperature(), scd41.getHumidity(), vBat, gps.latitude(), gps.lat(), gps.longitude(), gps.lon());
      }
    } else {
        Serial.println(scd41.getErrorMessage());
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
