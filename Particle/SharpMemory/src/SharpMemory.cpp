/*
 * Library for Controlling Sharp Memory LCDs
 *
 * 2016 Daniel Eisterhold
 *
 * Adapted from Adafruit's Sharp Memory Display Library
 *
 */

#include "SharpMemory.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif
#ifndef _swap_uint16_t
#define _swap_uint16_t(a, b) { uint16_t t = a; a = b; b = t; }
#endif

SharpMemory::SharpMemory(uint8_t cs): Adafruit_GFX(DISPLAY_WIDTH, DISPLAY_HEIGHT) {
  init(&SPI, cs);
}

SharpMemory::SharpMemory(SPIClass* bus, uint8_t cs): Adafruit_GFX(DISPLAY_WIDTH, DISPLAY_HEIGHT) {
  init(bus, cs);
}

void SharpMemory::init(SPIClass* bus, uint8_t cs) {
  // Store the SPI bus to use
  spiBus = bus;
  
  // Store the CS Pin
  chipSelect = cs;

  // Initialize SPI library
  spiBus->begin();

  // Set the pin low
  digitalWrite(chipSelect, LOW);

  // Set the pin as an output
  pinMode(chipSelect, OUTPUT);

  // Initialize VCOM state
  vcomHigh = false;

  // Set the flag for knowing if there are changes that need to be written
  hasChanges = false;

  for(unsigned int index = 0; index < sizeof(lineHasChanges); index++) {
      lineHasChanges[index] = false;
  }

  // Clear the display after initialization
  clearDisplay();
}

void SharpMemory::sendByte(uint8_t data) {
    spiBus->transfer(data);
}

void SharpMemory::sendByteLSB(uint8_t data) {
    sendByte(reverse[data]);
}

void SharpMemory::drawPixel(int16_t x, int16_t y, uint16_t color) {
    // Do nothing if outside of the bounds
    if((x < 0) || (x >= DISPLAY_WIDTH) || (y < 0) || (y >= DISPLAY_HEIGHT)) return;

    switch(rotation) {
        case 1:
            _swap_int16_t(x, y);
            x = WIDTH  - 1 - x;
            break;
        case 2:
            x = WIDTH  - 1 - x;
            y = HEIGHT - 1 - y;
            break;
        case 3:
            _swap_int16_t(x, y);
            y = HEIGHT - 1 - y;
            break;
    }

    hasChanges = true;
    lineHasChanges[y] = true;

    // If writing a black pixel
    if(color == BLACK) {
        // Clear the selected bit
        frameBuffer[y][x/8] &= clr[x % 8];
    }
    else {
        // Set the selected bit
        frameBuffer[y][x/8] |= set[x % 8];
    }
}

uint8_t SharpMemory::getPixel(uint16_t x, uint16_t y) {
    if((x < 0) || (x >= DISPLAY_WIDTH) || (y < 0) || (y >= DISPLAY_HEIGHT)) return false;

    switch(rotation) {
        case 1:
            _swap_uint16_t(x, y);
            x = WIDTH  - 1 - x;
            break;
        case 2:
            x = WIDTH  - 1 - x;
            y = HEIGHT - 1 - y;
            break;
        case 3:
            _swap_uint16_t(x, y);
            y = HEIGHT - 1 - y;
            break;
    }

    return !(frameBuffer[y][x/8] & set[x % 8]) ? BLACK : WHITE;
}

void SharpMemory::clearDisplay() {
    hasChanges = false;

    // Clear the buffer (Set all bits to 1)
    for(int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x  = 0; x < DISPLAY_WIDTH/8; x++) {
            frameBuffer[y][x] = 0xFF;
        }
        // Line has changes if black
        lineHasChanges[y] = false;
    }

    // Start writing to device
    setCS(true);

    // Send clear command
    if(vcomHigh) {
        sendByte(LCD_CLEAR | LCD_VCOM_HIGH);
    }
    else {
        sendByte(LCD_CLEAR | LCD_VCOM_LOW);
    }

    // Send the comamnd trailer
    sendByte(LCD_NOP);

    // Toggle VCOM
    vcomHigh = !vcomHigh;

    // Stop writing to device
    setCS(false);
}

void SharpMemory::writeLine(const unsigned char* lineData, uint16_t lineNumber) {
    // Copy data from the array to the screen buffer
    memcpy(frameBuffer[lineNumber], lineData, (DISPLAY_WIDTH/8));

    // Update flag to know there are changes that have not been written
    hasChanges = true;

    // Update array to know that the line has changes
    lineHasChanges[lineNumber] = true;
}

void SharpMemory::refresh() {
    // Start writing to device
    setCS(true);

    // If there are changes, write them
    if(hasChanges) {
        // Send the command
        if(vcomHigh) {
            sendByte(LCD_WRITE | LCD_VCOM_HIGH);
        }
        else {
            sendByte(LCD_WRITE | LCD_VCOM_LOW);
        }

        // Send the line number and data
        for(int y = 0; y < DISPLAY_HEIGHT; y++) {
            // If the line does not have changes, skip it
            if(!lineHasChanges[y]) continue;

            // Send the line number (not zero based)
            sendByteLSB(y + 1);

            for(int x = 0; x < DISPLAY_WIDTH/8; x++) {
                // Send the data for the specific line
                sendByteLSB(frameBuffer[y][x]);
            }

            // Send the line trailer
            sendByte(LCD_NOP);

            // Update line indicating the changes have been written to the display
            lineHasChanges[y] = false;
        }

        // Send the command trailer
        sendByte(LCD_NOP);

        // Reset flag after writing changes
        hasChanges = false;
    }
    // Otherwise just toggle vcom
    else {
        // Send command based on vcom state
        if(vcomHigh) {
            sendByte(LCD_VCOM_HIGH);
        }
        else {
            sendByte(LCD_VCOM_LOW);
        }

        // Send the command trailer
        sendByte(LCD_NOP);
    }

    // Toggle VCOM
    vcomHigh = !vcomHigh;

    // Stop writing to device
    setCS(false);
}

void SharpMemory::setCS(bool high) {
    if (high) {
        // Begin of transfer
        digitalWrite(chipSelect, HIGH);
        spiBus->beginTransaction(SPISettings(SPI_SPEED, SPI_BIT_ORDER, SPI_MODE));
    }
    else {
        // End of transfer
        spiBus->endTransaction();
        digitalWrite(chipSelect, LOW);
    }
}
