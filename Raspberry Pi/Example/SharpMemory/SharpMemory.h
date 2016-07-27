
#ifndef __SHARP_MEMORY_H__
#define __SHARP_MEMORY_H__

#include <wiringPiSPI.h>

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define DISPLAY_HEIGHT   96
#define DISPLAY_WIDTH    96
#define BUFFER_SIZE (2 + (DISPLAY_HEIGHT * ((DISPLAY_WIDTH/8) + 2)))

class SharpMemory {
  public:
    SharpMemory();
    void drawPixel(int16_t x, int16_t y, bool black = true);
    bool getPixel(uint16_t x, uint16_t y);
    void clear();
    void refresh();
    void writeLine(uint16_t y);
  private:
    bool vcomHigh;
    bool hasChanges;
    char screenBuffer[BUFFER_SIZE];
    uint16_t bufferIndex;
    uint8_t frameBuffer[DISPLAY_HEIGHT][DISPLAY_WIDTH/8];
    void sendByte(uint8_t data);
    void sendByteLSB(uint8_t data);
    void setCS(bool high);
};

#endif
