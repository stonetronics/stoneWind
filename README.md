# stoneWind

anemometer firmware for arduino uno based anemometer

This firmware and hardware enables an Arduino Uno to read the anemometer sensor found on https://www.thingiverse.com/thing:4216504
The Arduino shield (/hw) is drawn in Eagle 9.2.2, the Gerbers were generated for JLCPCB manufacturing
The Firmware (/src) is programmed in platformio

Features:
- 7 segment display
- sd card for logging and storage of lookup table
- ptc temperature sensor
- frequency counter
- schmitt trigger and signal conditioning/supply of the anemometer
