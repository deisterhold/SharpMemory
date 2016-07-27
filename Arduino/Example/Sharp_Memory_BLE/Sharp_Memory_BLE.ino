
#include <CurieBLE.h>
#include "SPI.h"
#include "String.h"

#include "SharpMemory.h"

#define CS_PIN 10
#define CMD_MAX_LENGTH 15
const uint8_t SHARP_LCD_CLEAR_SCREEN = 0x01;
const uint8_t SHARP_LCD_WRITE_PIXEL  = 0x02;
const uint8_t SHARP_LCD_WRITE_LINE   = 0x04;

// Create all the stuff needed for bluetooth
BLEPeripheral screenPeripheral;
BLEService screenService("BE34A500-0BB0-4570-914F-850C3B7F433C");
BLECharacteristic commandCharacteristic("BE34A501-0BB0-4570-914F-850C3B7F433C", BLERead | BLEWrite, CMD_MAX_LENGTH);

// Initialize display using pin 10 as the chip select
SharpMemory display = SharpMemory(CS_PIN);

uint8_t defaultValue[CMD_MAX_LENGTH] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned long lastRefresh = 0;

void setup() {
  // Attempt to open a serial port
  Serial.begin(9600);

  // Wait until either the serial port opens or 5 seconds has elapsed
  while(!Serial && (millis() < 5000)) { delay(100); }
  
//  Serial.println("Setting up bluetooth.");
  // Setup bluetooth
  screenPeripheral.setLocalName("SHARP");
  screenPeripheral.setAppearance(320); // Generic Display
  screenPeripheral.setAdvertisedServiceUuid(screenService.uuid());
  
  // Add characteristic
//  Serial.println("Adding service and characteristic.");
  screenPeripheral.addAttribute(screenService);
  screenPeripheral.addAttribute(commandCharacteristic);

  // Set characteristic initial value
//  Serial.println("Setting characteristic initial value."); 
  commandCharacteristic.setValue(defaultValue, sizeof(defaultValue));
  
  // Start the bluetooth advertising
//  Serial.println("Starting to advertise.");
  screenPeripheral.begin();
  
  // Draw black from top to bottom
  for(int y = 0; y < 96; y++) {
    for(int x = 0; x < 96; x++) {
      display.drawPixel(x, y);
    }
    display.refresh();
    delay(20);
  }
  
  // Draw white from bottom to top
  for(int y = 95; y >= 0; y--) {
    for(int x = 95; x >= 0; x--) {
      display.drawPixel(x, y, false);
    }
    display.refresh();
    delay(20);
  }
  
  delay(20);
  
  // Draw black from left to right
  for(int x = 0; x < 96; x++) {
    for(int y = 0; y < 96; y++) {
      display.drawPixel(x, y);
    }
    display.refresh();
    delay(1);
  }
  
  // Draw white from right to left
  for(int x = 95; x >= 0; x--) {
    for(int y = 95; y >= 0; y--) {
      display.drawPixel(x, y, false);
    }
    display.refresh();
    delay(1);
  }
  
  // Clear the display
  display.clear();
}

void loop() {
  BLECentral central = screenPeripheral.central();

  if(central) {
//    Serial.println("Device connected.");
    
    // While connected
    while(central.connected()) {
      // If data was written to the characteristic
      if(commandCharacteristic.written()) {
//        Serial.println("Received command");
        // Write the data to the display
        const unsigned char* lineData = commandCharacteristic.value();
        
        switch(lineData[0]) {
          case SHARP_LCD_CLEAR_SCREEN: {
//            Serial.println("Clear command");
            display.clear();
            break;
          }
          case SHARP_LCD_WRITE_PIXEL: {
//            Serial.println("Write pixel command");
            uint16_t x = (uint16_t(lineData[1]) << 8) | lineData[2];
            uint16_t y = (uint16_t(lineData[3]) << 8) | lineData[4];
            display.drawPixel(x, y);
            display.refresh();
            break;
          }
          case SHARP_LCD_WRITE_LINE: {
//            Serial.println("Write line command");
            // Get the line number from the command
            uint16_t lineNum = (uint16_t(lineData[1]) << 8) | lineData[2];
            // Buffer for one line on the screen (initialize to all white)
            uint8_t screenData[12] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
            
            for(int index = 0; index < 12; index++) {
              screenData[index] = lineData[index + 3];
            }
            
            // Write data to the screen
            display.writeLine(screenData, lineNum);
            display.refresh();
            break;
          }
        }
        
        // Reset command
//        Serial.println("Erasing command");
        commandCharacteristic.setValue(defaultValue, sizeof(defaultValue));
      }
      
      // Refresh the display
      // If more than a second has elapsed refresh the display
      if(millis() > (lastRefresh + 1000)) {
        lastRefresh = millis();
        display.refresh();
      }
    }
    
//    Serial.println("Device disconnected.");
  }
  
  // If more than a second has elapsed refresh the display
  if(millis() > (lastRefresh + 1000)) {
    lastRefresh = millis();
    display.refresh();
  }
}

