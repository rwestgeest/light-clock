# Lightclock

Light alarm clock based on [instructable circuits light alarm clock](https://www.instructables.com/DIY-22/)

## Components used:

* NodeMCU Amica Modul V2 ESP8266 ESP-12E 
* 0.36" 4-Digit Tube LED 7-Segment Display Module Red Common Anode TM1637
* CHINLY WS2812B 16.4ft/5M Individually Addressable LED Strip 5050 RGB

## Getting started

In the arduino IDE you must select the correct board and install some libraries. After this, you should be able to upload the sketch and get started. Make sure you check the next paragraph "Notes on implementing the clock" as well.

### Select the esp8266 board

In file > preferences add:

https://arduino.esp8266.com/stable/package_esp8266com_index.json

to the additional boards manager urls

In Tools > Board > Boards Manager, search for es8266 and install the latest version.

### Libraries installation

Now try to compile the sketch and fix xxxx.h "file not found" errors by installing the necessary libraries. Sometimes there are multiple choices for a library. It is not always clear which one to choose. I installed:

* AdaFruit NeoPixel (latest version)  
  for driving  the led strip
* TimeLib (latest version)  
  for getting and setting the time on the device
* TM1637 (latest version)  
  for driving the led display

### First steps with components and sketch

Take things step by step

* Put your components in a breadboard and wire them there to try them out. Start with just the ESP8266 and led display and go from there.
* Open the serial monitor on 11500 baud 
* Change the SSID and WPA password for your network in the sketch
* Upload the sketch and see if you can get the correct time displayed in the mean time try to understand the code.
## Notes on implementing the clock

### Uploading the web page

If you have managed to upload your sketch get the time displayed and find the clock's ip address in your network, try and open the web page in your browser. It should be served on port 301. You'll find the web page to be empty. The html page (in the data directory) is not uploaded to the board yet. The web page upload is a separate process. 

You must upload the web page to the SPFF file system. This sounds complicated, and there's a lot of detail behind this, but <a target="_blank" href="https://www.esp8266.com/viewtopic.php?f=32&t=10081">here</a> is a small tutorial for uploading the the html.

### Power and Consumption
The clock can be powered through the micro usb port, given you use one meter of 60 leds. It seems to be drawing:

* 1.3A at 5V (6.5W) when all leds at full intensity
* 0.5A at 5V (2.5W) when all leds at half intensity
* 0.15A at 5V (0.75W) when all leds are off
* 0.17A at 5V (0.85W) when in analog clock mode

### Leds used are RGB (not RGBW)

Since the led strip used is RGB (not RGBW) adjust driver library parameters when playing with the leds accordingly. strip.color() does not take a parameter for white, white needs to be encoded as all RGB colors evenly powered.

### Notes on the original wiring drawing

In the original drawing (see below), __D1__ is wired to __clk__ and __D5__ to __DIO__ of the 7-segment display. The source sketch however takes __D5__ for __clk__ and __D1__ for __DIO__. Make sure to correct either wiring or source sketch.

![drawing](images/FUR48A2JJZN7XDW.png)

When powering the clock through micro-USB, use __VIN__ and __GND__ to power the led display and the led strip.

### Note on wiring the strip

* Do not make the mistake of overpowering the strip like I did. I will destroy some leds. 
* Make sure to wire the led in in the correct direction. The strip contains arrows, showing the the data direction.

### Illegal instruction when using original sketch

When using the original sketch, an illegal instruction exception occurred. The feedback from the exception is suboptimal. The culprit was 

```c
unsigned int sendNTPpacket(IPAddress & address)` {
    // ...
}
```

being declared as `usigned int` but not returning a value. I changed it to void, to remove the exception.

### Case size and the analog clock

The analog clock assumes the circumference of the 3d printed inner-case as designed by [instructable](https://www.instructables.com/DIY-22/). The led strip should be sticked to the inner case clock-wise and the first led should be on top (12 'o Clock so to speak). It will go around the case exactly 3-times.

### PCB 

You do not need an extra PCB, but it may help to keep everything in place.

## TODO

* the code is a bit messy - clean up - maybe even test drive a new implementation
* the web interface does not show the current alarm time value - would be very nice
* remove all whites


