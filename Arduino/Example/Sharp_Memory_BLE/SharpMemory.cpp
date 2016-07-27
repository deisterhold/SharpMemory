
#include "SharpMemory.h"

SharpMemory::SharpMemory(uint8_t chipSelect) {
    // Store the CS Pin
    csPin = chipSelect;
    
    // Set the pin low
    digitalWrite(csPin, LOW);
    
    // Set the pin as an output
    pinMode(csPin, OUTPUT);
    
    // Initialize VCOM state
    vcomHigh = false;

    // Set the flag for knowing if there are changes that need to be written
    hasChanges = false;

    for(unsigned int index = 0; index < sizeof(lineHasChanges); index++) {
      lineHasChanges[index] = false;
    }
    
    // Initialize SPI library
    SPI.begin();

    // Clear the display after initialization
    clear();
}

void SharpMemory::sendByte(uint8_t data) {
    SPI.transfer(data);
}

void SharpMemory::sendByteLSB(uint8_t data) {
    sendByte(reverse[data]);
}

void SharpMemory::drawPixel(int16_t x, int16_t y, bool black) {
    // Do nothing if outside of the bounds
    if((x < 0) || (x >= DISPLAY_WIDTH) || (y < 0) || (y >= DISPLAY_HEIGHT)) return;
    
    hasChanges = true;
    lineHasChanges[y] = true;
    
    // If writing a black pixel
    if(black) {
        // Clear the selected bit
        frameBuffer[y][x/8] &= clr[x % 8];
    }
    else {
        // Set the selected bit
        frameBuffer[y][x/8] |= set[x % 8];
    }
}

bool SharpMemory::getPixel(uint16_t x, uint16_t y) {
  if((x < 0) || (x >= DISPLAY_WIDTH) || (y < 0) || (y >= DISPLAY_HEIGHT)) return false;
  
  return !(frameBuffer[y][x/8] & set[x % 8]);
}

void SharpMemory::clear(bool black) {
    hasChanges = black;
    
    // Clear the buffer (Set all bits to 1)
    for(int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x  = 0; x < DISPLAY_WIDTH/8; x++) {
            frameBuffer[y][x] = black ? 0x00 : 0xFF;
        }
        // Line has changes if black
        lineHasChanges[y] = black;
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
        digitalWrite(csPin, HIGH);
        SPI.beginTransaction(SPISettings(SPI_SPEED, SPI_BIT_ORDER, SPI_MODE));
    }
    else {
        // End of transfer
        SPI.endTransaction();
        digitalWrite(csPin, LOW);
    }
}
