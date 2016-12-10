rpi_ddbooster
==========

Raspberry Pi userspace library to simplify the use of the Digi-Dot-Booster for driving WS2812 LED stripes (and compatible). This is based on an existing Arduino library by Daniel Wiese - see https://github.com/Gamadril/DD-Booster-Library.

### DIGI-DOT-BOOSTER
The raspberry sends SPI commands to the Digi-Dot-Booster which generates the data signal for up to 256 LEDs. 
Detailed information about available SPI commands can be found here: http://www.led-genial.de/mediafiles//Sonstiges/digi-dot-booster_ENG.pdf

### Building
Scons is required to build the example application.
  - call <code>scons</code> and <code>sudo scons install</code> to build and install the library
  - call <code>./main</code> to run the example application

  - <code>scons -c</code> will clear all built objects
  - <code>sudo scons -c install</code> will remove the installed shared library
