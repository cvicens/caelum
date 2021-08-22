/*
  DataLogger.cpp - Library for DataLogger.
  Created by Carlos V.
  Released into the public domain.
*/

#include "Arduino.h"
#include <Wire.h>

#include "DataLogger.h"

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

  // If control file does not exist 
  File controlFile;
  if (!fatfs.exists(CONTROL_FILE_NAME)) {
    // Reset index and position
    index = 0;
    position = 0;
    // Create it and open it in RW mode
    controlFile  = fatfs.open(CONTROL_FILE_NAME, FILE_WRITE);
    if (controlFile) {
      // Write index and position
      controlFile.println(index);
      controlFile.println(position);
    } else {
      Serial.println("Error while creating control file");
    }
  } else {
    controlFile  = fatfs.open(CONTROL_FILE_NAME, FILE_READ);
    index = controlFile.parseInt();
    position = controlFile.parseInt();
  }
  // Close control file
  controlFile.close();

  return true;
}

bool DataLogger::rotate(void){
  uint8_t _index = (index++ < ROTATION_NUMBER) ? index++ : 0;
  uint32_t _position = 0;

  if (updateControlFile(_index, _position)) {
    index = _index;
    position = _position;

    return truncateDataFile(index);
  }
  return false;
}

bool DataLogger::updateControlFile(uint8_t _index, uint32_t _position){
  // If control file does not exist 
  File controlFile = fatfs.open(CONTROL_FILE_NAME, FILE_WRITE);
  if (controlFile) {
    controlFile.truncate(0);
    // Write index and position
    controlFile.println(_index, DEC);
    controlFile.println(_position, DEC);
    // Close control file
    controlFile.close();
    return true;
  }
  return false;
}

bool DataLogger::truncateDataFile(uint8_t _index){
  char fileName[10];
  sprintf(fileName, "%s-%d.%s", DATA_FILE_NAME_PREFIX, index, DATA_FILE_NAME_EXTENSION);

  File dataFile = fatfs.open(fileName, FILE_WRITE_START);
  if (dataFile) {
    dataFile.truncate(0);
    dataFile.close();
    return true;
  }
  return false;
}

bool DataLogger::writeLine(char* line){
  if (!line || strlen(line) <= 0) {
      Serial.println("DataLogger::writeLine() line cannot be empty or NULL");
      return false;
  }

  // Calculate data file name 'data-#.csv' 
  char fileName[10];
  sprintf(fileName, "%s-%d.%s", DATA_FILE_NAME_PREFIX, index, DATA_FILE_NAME_EXTENSION);
  Serial.print("fileName: "); Serial.print(fileName); Serial.print(" index: "); Serial.print(index); Serial.print(" position: "); Serial.println(position);
  // Check data file size and rotate if necessary
  if (fatfs.exists(fileName)) {
    File dataFileRead = fatfs.open(fileName, FILE_READ);
    // Check that the file opened successfully
    if (dataFileRead) {
      // If size if bigger than MAX we have to rotate to the next file
      Serial.print("DataLogger::writeLine() File size: ");Serial.println(dataFileRead.size());
      if (dataFileRead.size() + strlen(line) >= MAX_FILE_SIZE) {
        if (!rotate()) {
          Serial.println("DataLogger::writeLine() error while rotating!");
          return false;
        }
      }
    }
  } 
  
  // Calculate file name again, just if rotation happened and open it in RW mode
  sprintf(fileName, "%s-%d.%s", DATA_FILE_NAME_PREFIX, index, DATA_FILE_NAME_EXTENSION);
  File dataFileWrite = fatfs.open(fileName, FILE_WRITE_START);
  if (dataFileWrite) {
    dataFileWrite.seek(position);
    position += dataFileWrite.println(line);
    dataFileWrite.close();
    updateControlFile(index, position);
    Serial.println("DataLogger::Wrote new measurement to data file!");
    return true;
  }

  Serial.println("DataLogger::writeLine() Failed to open data file for writing!");
  return false;
}

bool DataLogger::dumpToSerial(void){
  // Calculate data file name 'data-#.csv' 
  char fileName[10];
  sprintf("%s-%d.%s", DATA_FILE_NAME_PREFIX, index, DATA_FILE_NAME_EXTENSION, fileName, sizeof(fileName));

  File dataFile = fatfs.open(fileName, FILE_READ);
  // Check that the file opened successfully and read
  if (dataFile) {
    Serial.println("DUMPING FILE:");

    // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    // close the file:
    dataFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("Failed to open data file for reading!");
  }
}
