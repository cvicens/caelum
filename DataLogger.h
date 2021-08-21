/*
  DataLogger.h - Library for DataLogger.
  Created by Carlos V.
  Released into the public domain.
*/

#ifndef DataLogger_h
#define DataLogger_h

#include "Arduino.h"

#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>

// Configuration for the datalogging file:
#define FILE_NAME "data.csv"

class DataLogger
{
  private:
    bool initialized;

    Adafruit_SPIFlash flash;
    FatFileSystem fatfs;

  public:
    DataLogger(void);

    bool init(void);

    bool writeLine(char* line);
};

#endif

