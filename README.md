rpi_ddbooster
==========

Raspberry Pi userspace library to simplify the use of the Digi-Dot-Booster for driving WS2812 LED stripes (and compatible). This is based on an existing Arduino library by Daniel Wiese - see https://github.com/Gamadril/DD-Booster-Library.

This library includes a python wrapper that works with Python 2 and 3. See the python subdirectory for more information.

### DIGI-DOT-BOOSTER
The raspberry sends SPI commands to the Digi-Dot-Booster which generates the data signal for up to 256 LEDs. 
Detailed information about available SPI commands can be found here: http://www.led-genial.de/mediafiles//Sonstiges/digi-dot-booster_ENG.pdf

### Building

Build and install the library:
```
scons
sudo scons install
```

Clean compiled objects and remove the library
```
scons -c
sudo scons -c install
```

(Scons is required for building)
