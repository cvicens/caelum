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

// Lines buffer size. To avoid writing everytime...
#define LINES_BUFFER_SIZE 10

// Max size of the logging files
#define MAX_FILE_SIZE 25*1024

// Number of files to maintain while rotating
#define ROTATION_NUMBER 6

// Configuration for the datalogging file:
#define CONTROL_FILE_NAME "control.dat"
#define DATA_FILE_NAME_PREFIX "data"
#define DATA_FILE_NAME_EXTENSION "csv"

#define FILE_WRITE_START (O_RDWR | O_CREAT)

class DataLogger
{
  private:
    bool initialized;

    // index selects which file to log to
    uint8_t index;
    // `position` points to the byte to log to
    uint32_t position;

    // Lines buffer
    int lines_buffer_index;
    char *lines_buffer[LINES_BUFFER_SIZE];

  public:
    // File related libraries
    Adafruit_SPIFlash flash;
    FatFileSystem fatfs;
    
  public:
    DataLogger(void);

    bool init(void);

    bool rotate(void);
    bool updateControlFile(uint8_t _index, uint32_t _position);
    bool truncateDataFile(uint8_t _index);
    bool writeLine(char* line);
    bool dumpToSerial(void);

};

#endif

