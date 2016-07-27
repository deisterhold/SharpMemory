# Sharp Memory

Library for Controlling Sharp Memory LCDs.
There are two different libraries, one for Arduino and the other for Raspberry Pi
For Arduino, there are two different versions. One is optimized for speed by keeping track of changes and only redrawing lines were data has been changed.
The unoptimized version does not have those modifications and uses slightly less RAM.

Credit to Adafruit from which I referenced my library:
<https://github.com/adafruit/Adafruit_SHARP_Memory_Display>

## Arduino

For testing this library I used and [Arduino 101](https://www.arduino.cc/en/Main/ArduinoBoard101) and a [Sharp Memory BoosterPack](http://www.ti.com/tool/430boost-sharp96) from Texas Instruments

The wiring is rather simple.
Connect 3.3V -> VCC, GND -> GND, MISO -> SIMO, Pin 10 -> CS, CLK -> CLK, and don't forget 3.3V -> LCD Enable.

[Video of iPhone App](https://github.com/deisterhold/SharpMemory/raw/master/iOS%20App/App%20Video.mov)

### Clone the Repository

```
git clone https://github.com/deisterhold/SharpMemory/
```

### Open and run the Arduino sketch at:

```
~/SharpMemory/Arduino/Example/Sharp_Memory_BLE/Sharp_Memory_BLE.ino
```

## Raspberry Pi

### Clone the Repository

```
git clone https://github.com/deisterhold/SharpMemory/
```

###  Install the bcm2835 library first

```
cd "SharpMemory/Raspberry Pi"
tar zxvf bcm2835-1.xx.tar.gz
cd bcm2835-1.xx
./configure
make
sudo make check
sudo make install
```

### Compile and Run the Example Program

```
cd "Example/SharpMemory"
make
sudo ./test
```

### Wiring
![alt text](https://github.com/deisterhold/SharpMemory/raw/master/iOS%20App/Screen%201.JPG "Screen 1")
![alt text](https://github.com/deisterhold/SharpMemory/raw/master/iOS%20App/Screen%202.JPG "Screen 2")
![alt text](https://github.com/deisterhold/SharpMemory/raw/master/iOS%20App/Screen%203.JPG "Screen 3")
