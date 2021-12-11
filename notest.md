# Notes on building the clock
RWE

## Board installation

In preferences add:

https://arduino.esp8266.com/stable/package_esp8266com_index.json

to the additional boards manager urls

In Tools > Board > Boards Manager, search for es8266 and install latest
version.

## Libraries installation

When compiling Lights_Alarm_Final fix xxxx.h file not found errors by
installing the necessary libraries. Sometimes there are multiple choices
for a library. It is not always clear which one to choose. I installed:

* AdaFruit NeoPixel
* TimeLib
* TM1637
