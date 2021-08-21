/*
  DataLogger.cpp - Library for DataLogger.
  Created by Carlos V.
  Released into the public domain.
*/

#include "Arduino.h"
#include <Wire.h>

#include "DataLogger.h"

#define MAX_FILE_SIZE 1024

// On-board external flash (QSPI or SPI) macros should already
// defined in your board variant if supported
// - EXTERNAL_FLASH_USE_QSPI
// - EXTERNAL_FLASH_USE_CS/EXTERNAL_FLASH_USE_SPI
#if defined(EXTERNAL_FLASH_USE_QSPI)
  Adafruit_FlashTransport_QSPI flashTransport;

#elif defined(EXTERNAL_FLASH_USE_SPI)
  Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS, EXTERNAL_FLASH_USE_SPI);

#else
  #error No QSPI/SPI flash are defined on your board variant.h !
#endif

DataLogger::DataLogger()
{
  initialized = false;

  flash = Adafruit_SPIFlash(&flashTransport);
}

bool DataLogger::init(void){

  
// Initialize flash library and check its chip ID.
  if (!flash.begin()) {
    Serial.println("Error, failed to initialize flash chip!");
    return false;
  }
  Serial.print("Flash chip JEDEC ID: 0x"); Serial.println(flash.getJEDECID(), HEX);

  // First call begin to mount the filesystem.  Check that it returns true
  // to make sure the filesystem was mounted.
  if (!fatfs.begin(&flash)) {
    Serial.println("Error, failed to mount newly formatted filesystem!");
    Serial.println("Was the flash chip formatted with the fatfs_format example?");
    return false;
  }
  Serial.println("Mounted filesystem!");

  return true;
}

bool DataLogger::writeLine(char* line){
  File dataFile = fatfs.open(FILE_NAME, FILE_WRITE);
  // Check that the file opened successfully and write a line to it.
  if (dataFile) {
    // Write a line to the file.  You can use all the same print functions
    // as if you're writing to the serial monitor.
    if (line && strlen(line) > 0) {
      dataFile.println(line);
    } else {
      Serial.println("DataLogger::writeLine line size is <= 0 or null");
    }
    // Finally close the file when done writing.  This is smart to do to make
    // sure all the data is written to the file.
    dataFile.close();
    Serial.println("Wrote new measurement to data file!");
    return true;
  }

  Serial.println("Failed to open data file for writing!");

}


