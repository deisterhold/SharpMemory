/*
 * Sharp Memory LCD Test Program
 * By: Daniel Eisterhold
 * 2016 June
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>

using namespace std;

#include "SharpMemory.h"

int main(int argc, char* argv[])
{
  // Initialize random number generator with the current time
  srand(time(NULL));
  
  // Initialize display
  SharpMemory display = SharpMemory();

  // Make sure the display is clear before writing to it
  display.clear();

  // Loop forever
  while(1) {    
    // Fill the screen from the top to the bottom
    for(int y = 0; y < 96; y++) {
      for(int x = 0; x < 96; x++) {
	  display.drawPixel(x, y);
      }
      // Write changes
      display.refresh();

      // Limit frame rate to 20 fps (50 ms)
      usleep(50000);
    }

    display.refresh();

    sleep(1);

    // Clear the display
    display.clear();
  }
  
  return 0;
}
