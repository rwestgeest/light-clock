/******************
 - Light alarm Mk1 7/24/2018
 - tutorial on SPIFFS https://tttapa.github.io/ESP8266/Chap11%20-%20SPIFFS.html
 - how to flash SPIFF https://www.esp8266.com/viewtopic.php?f=32&t=10081
******************/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <TM1637Display.h>//clock driving library
#include <FS.h>   // Include the SPIFFS library  
const char* ssid = "<your ssid>";
const char* password = "<your password>";
int ledPin = D2;
#define clockPin D5
#define DIO D1
long estTime;
#define NUM_LEDS 60//number of LEDs in connected strip
#define BRIGHTNESS 255
String form;
ESP8266WebServer server(301);//set this to the port you want the esp to listen to
TM1637Display display(clockPin, DIO);
unsigned long millis(void);
unsigned long t1 = 0UL;//counting variables
unsigned long t2 = 0UL;
unsigned long t3 = 0UL;
unsigned long t4 = 0UL;
const unsigned long reset = 3435973836UL;
// settings variables
bool dc = true;
bool milt = true;
bool rand1 = false;
bool rand2 = false;
bool rand3 = false;
bool lighton = false;
int cbrite = 8;
int sysmode = 0;
String atm;//alarm Time
int temp = 0;
int ahr = 00;
int amn = 00;
int ass = 00;
int s1 = 0;
int s2 = 0;
int wT;//wake time
String c1;
String c2;
String c3;
long int r1;
long int g1;
long int b1;
long int r2;
long int g2;
long int b2;
long int r3;
long int g3;
long int b3;
long int minfromMid;//minutes from midnight
long int rightNow;//seconds from midnight
int hrs = 0;
int mins = 0;
int secs = 0;
int prevh;
int prevm;
int prevs;
int wake = 0;
int rrand1;//random color variables
int grand1;
int brand1;
int rrand2;
int grand2;
int brand2;
int rrand3;
int grand3;
int brand3;
byte vortexR[NUM_LEDS] = {0};//matrices for vortex mode
byte vortexG[NUM_LEDS] = {0};
byte vortexB[NUM_LEDS] = {0};
byte vortexW[NUM_LEDS] = {0};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, D2, NEO_GRB + NEO_KHZ800);
//matrix for gamma correction of LED colors
byte neopix_gamma[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};
IPAddress timeServerIP;
const char* ntpServerName = "nl.pool.ntp.org";

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];
WiFiUDP udp;
/*********************************************************************************/
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(DIO, OUTPUT);
  Serial.begin(115200);//set this baudrate in arduino
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  udp.begin(301);
  WiFi.hostByName(ntpServerName, timeServerIP);
  sendNTPpacket(timeServerIP);
  delay(1000);
  int cb = udp.parsePacket();
  while (!cb) {
  }
  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  Serial.println("Received NTP packet");

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  const unsigned long seventyYears = 2208988800UL;
  unsigned long epoch = secsSince1900 - seventyYears;
  Serial.print("UNX");
  Serial.println(epoch);
  estTime = epoch + 3600;//sets timezone time. subtract (hours from GMT *3600)
  setTime(estTime);
  delay(10);
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  SPIFFS.begin();
  randomSeed(analogRead(2));//don't use this pin
  server.on("/", HTTP_GET, handleRoot);
  server.on("/Clock", HTTP_POST, handleClock);
  server.on("/Clockm", HTTP_POST, handleClockm);
  server.on("/lightsOut", HTTP_POST, handleLightsOut);
  server.on("/Light", HTTP_POST, handleLight);
  server.on("/admin", HTTP_GET, handleAdmin);
  server.on("/Admin", HTTP_GET, handleAdmin);
  server.on("/ADMIN", HTTP_GET, handleAdmin);
  server.onNotFound(handleNotFound);
  server.begin();

}

void loop() {
  if (millis() >= reset) {
    ESP.restart();//restart ESP if clock will overflow, should be ~40 days
  }
  time_t t = now();
  minfromMid = hour(t) * 60 + minute(t);
  rightNow = (hour(t) * 3600) + (minute(t) * 60 ) + second(t);
  if (secs != second(t)) {//only updates the clock every second, prevents overloading ESP processor
    secs = second(t);
    displayTime(dc, milt, cbrite, sysmode, hour(t), minute(t));
    if (wake != 0 && wT != 0) {
      if (wake - rightNow < (wT * 60) && wake - rightNow > 0) {
        alarmLight(wake, rightNow);
      }
      if (rightNow - wake == (wT * 60)) {//turns lights off after length wT
        lightsOut();
      }
    }
    if (sysmode == 2) { //analog clock
      analogTime(hour(t), minute(t), second(t));
    }


  }
  if (hour(t) == 2 && minute(t) == 0) { //turn off lights at 2am
    sysmode = 1;
    lightsOut();
  }
  server.handleClient();
  if (rand1 == true) {
    r1 = random(0, 255);
    g1 = random(0, 255);
    b1 = random(0, 255);
  }
  if (rand2 == true) {
    r2 = random(0, 255);
    g2 = random(0, 255);
    b2 = random(0, 255);
  }
  if (rand3 == true) {
    r3 = random(0, 255);
    g3 = random(0, 255);
    b3 = random(0, 255);
  }
  if (sysmode == 0) { //3 red flashes, not working for some reason
    errorLight();
    sysmode = 1;
  }

  if (sysmode == 3) { //Color
    if (lighton == false) {
      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(neopix_gamma[r1], neopix_gamma[g1], neopix_gamma[b1] ) );
        strip.show();
        //  delay(0);
      }
      lighton = true;//only sets LED strip once, prevents overload
    }
  }
  if (sysmode == 4) { //Color cycle
    colorCycle();
  }
  if (sysmode == 5) { //Color cylinder
    cylinder();
  }
  if (sysmode == 6) { //Color Vortex
    vortex();
  }
  if (sysmode == 7) { //RAINbow
    rainbowCycle(s1);
  }
  /*if (sysmode == 8) { //Music
  Thought adding a mic for responsive lighting would be fun, too much work
    }*/
  else {
    errorLight();
  }
}


void handleRoot() {
  File file = SPIFFS.open("/html_code.html", "r");                 // Open it
  size_t sent = server.streamFile(file, "text/html"); // And send it to the client
  file.close();
}
void handleClock() {//toggles digital clock on/off
  dc = !dc;
  cbrite = server.arg("clkbrite").toInt();
  server.sendHeader("Location", "/");//this and the following line send client back to 'homepage'
  server.send(303);
}
void handleClockm() {//toggles 12/24 time
  milt = !milt;
  cbrite = server.arg("clkbrite").toInt();
  server.sendHeader("Location", "/");
  server.send(303);
}
void handleLight() {//handles all other settings
  int sysmodecheck = server.arg("sm").toInt();
  if (sysmodecheck != sysmode) {//wipe current display settings
    lightsOut();
    lighton = false;
    sysmode = sysmodecheck;
  }
  if (sysmodecheck == 1) {//sets alarm settings only if sysmode == 1
    atm = server.arg("atm");
    wT = server.arg("wT").toInt();
    String chr = atm.substring(0, 2);
    String cmm = atm.substring(3, 5);
    String css = atm.substring(6);
    ahr =    chr.toInt();
    amn =    cmm.toInt();
    ass =    css.toInt();//ass lol
    wake = (ahr * 3600) + (amn * 60) + ass;
  }

  s1 = server.arg("sp").toInt();
  s2 = server.arg("sp2").toInt();
  c1 = server.arg("c1");
  c2 = server.arg("c2");
  c3 = server.arg("c3");
  char red1[3];
  char green1[3];
  char blue1[3];
  char red2[3];
  char green2[3];
  char blue2[3];
  char red3[3];
  char green3[3];
  char blue3[3];
  String re1 = c1.substring(1, 3);
  String gr1 = c1.substring(3, 5);
  String bl1 = c1.substring(5, 7);
  String re2 = c2.substring(1, 3);
  String gr2 = c2.substring(3, 5);
  String bl2 = c2.substring(5, 7);
  String re3 = c3.substring(1, 3);
  String gr3 = c3.substring(3, 5);
  String bl3 = c3.substring(5, 7);
  re1.toCharArray(red1, 3);
  gr1.toCharArray(green1, 3);
  bl1.toCharArray(blue1, 3);
  re2.toCharArray(red2, 3);
  gr2.toCharArray(green2, 3);
  bl2.toCharArray(blue2, 3);
  re3.toCharArray(red3, 3);
  gr3.toCharArray(green3, 3);
  bl3.toCharArray(blue3, 3);

  r1 = strtol(red1, NULL, 16);
  g1 = strtol(green1, NULL, 16);
  b1 = strtol(blue1, NULL, 16);
  r2 = strtol(red2, NULL, 16);
  g2 = strtol(green2, NULL, 16);
  b2 = strtol(blue2, NULL, 16);
  r3 = strtol(red3, NULL, 16);
  g3 = strtol(green3, NULL, 16);
  b3 = strtol(blue3, NULL, 16);
  if (r1 == 0 && g1 == 0 && b1 == 0) {
    rand1 = true;
  }
  else {
    rand1 = false;
  }
  if (r2 == 0 && g2 == 0 && b2 == 0) {
    rand2 = true;
  }
  else {
    rand2 = false;
  }
  if (r3 == 0 && g3 == 0 && b3 == 0) {
    rand3 = true;
  }
  else {
    rand3 = false;
  }

  server.sendHeader("Location", "/");       // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);
 
  /*  Serial.println(dic);//debug info
    Serial.println(sysmode);
    Serial.println(atm);
    Serial.println(wT);
    Serial.println(wake);
    Serial.println(s1);
    Serial.println(s2);
    Serial.println(temp);
    Serial.println(c1 + ", " + red1 + "," + green1 + "," + blue1 + ", " + r1 + "," + g1 + "," + b1);
    Serial.println(c2 + ", " + red2 + "," + green2 + "," + blue2 + ", " + r2 + "," + g2 + "," + b2);
    Serial.println(c3 + ", " + red3 + "," + green3 + "," + blue3 + ", " + r3 + "," + g3 + "," + b3);
  */

  return;
}
void handleNotFound() {//error if navigating to wrong address
  server.send(404, "text/plain", "ERROR 404: LAMP NOT FOUND");
  delay(10000);
  server.sendHeader("Location", "/");       // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);

}
void handleAdmin() {//fun for hackers
  server.send(404, "text/plain", "I see you are trying to hack my lamp. This is most certainly just a lamp. A very awesome lamp, but nonetheless, a lamp. If you still wish to hack it, please make the 7-segment display read 'hack' or 'pwnd' so I know I got hacked and pwnd.");
  delay(10000);
  server.sendHeader("Location", "/");       // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);

}
void handleLightsOut() {//turns off LED strip
  lightsOut();
  server.sendHeader("Location", "/");       // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);
  sysmode = 1;
  return;
}
void lightsOut() {//turns off LED strip
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0 ) );
  }
  strip.show();
}

void displayTime(bool dc, bool milt, int cbrite, int sysmode, int hhh, int mmm) {
  int dtime;
  uint8_t colon = 0b01000000;
  if (milt == false && hhh > 12) {
    hhh = hhh - 12;
    dtime = (100 * hhh + mmm);
  }
  else {
    dtime = (100 * hhh + mmm);
  }
  if (dc == true) {
    display.setBrightness(cbrite, true);
    display.showNumberDecEx(dtime, colon, false, 4, 0); //see TM1637Display.h for explaination
  }
  else {
    display.setBrightness(0x00, false);
    display.showNumberDecEx(dtime, colon, false, 4, 0);
  }
}

void analogTime(int hr, int mn, int ss) {//Mode 2
  float h;
  if (hr < 10) {
    h = ((float)hr / 12.0 * 20.0) + 40.0;
  }
  else {
    h = (((float)hr - 12.0) / 12.0 * 20.0) + 40.0;
  }
  //min
  float m = (float)mn / 60.0 * 20.0 + 20.0;
  //sec
  float s = (float)ss / 60.0 * 20.0;
  int hh = (int)h;
  int mm = (int)m;
  int sss = (int)s;
  server.handleClient();
  strip.setPixelColor((uint16_t)prevh, strip.Color(0, 0, 0));
  strip.setPixelColor((uint16_t)prevm, strip.Color(0, 0, 0));
  strip.setPixelColor((uint16_t)prevs, strip.Color(0, 0, 0));
  strip.setPixelColor((uint16_t) hh, strip.Color(neopix_gamma[r1], neopix_gamma[g1], neopix_gamma[b1]) );
  strip.setPixelColor((uint16_t) mm, strip.Color(neopix_gamma[r2], neopix_gamma[g2], neopix_gamma[b2]) );
  strip.setPixelColor((uint16_t) sss, strip.Color(neopix_gamma[r3], neopix_gamma[g3], neopix_gamma[b3]) );
  strip.show();
  prevh = hh ;
  prevm = mm ;
  prevs = sss ;
}
void errorLight() {//should flash red LEDs
  for (int z = 0; z < 2, z++;) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(128, 0, 0 ) );
    }
    strip.show();
    delay(250);
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0 ) );
    }
    strip.show();
    delay(250);
  }
}
void alarmLight(int a, int b) {//wake, rightNow
  float intensity = 1.0 - (((float)a - (float)b) / ((float)wT * 60.0));
  float Whitef = 255.0 * (intensity);
  uint16_t White = (uint16_t)Whitef;
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(neopix_gamma[White], neopix_gamma[White], neopix_gamma[White]) );
  }
  strip.show();
}

void colorCycle() {
  t1 = millis();
  t2 = t1;
  bool start = true;
  bool loopOK = true;
  while (t1 - t2 <= 14400000 && loopOK == true) { //no more than 4 hours of color cycle at a time{
    t1 = millis();
    if (start == true) {
      for (int f = 0; f < 256; f++) {
        for (uint16_t i = 0; i < strip.numPixels(); i++) {
          int red1 = neopix_gamma[r1] * f / 256;
          int green1 = neopix_gamma[g1] * f / 256;
          int blue1 = neopix_gamma[b1] * f / 256;
          strip.setPixelColor(i, strip.Color(red1, green1, blue1));
          delay(0);
        }
        strip.show();
      }
      start = false;
    }
    for (int q = 1; q <= 3; q++) {
      if (q = 1) {
        for (int f = 0; f < 256; f++) {
          float intens1 = f / 256.0;
          float intens2 = 1.0 - f / 256.0;
          for (uint16_t i = 0; i < strip.numPixels(); i++) {
            int red = (int)((float)neopix_gamma[r1] * intens2) + ((float)neopix_gamma[r2] * intens1);
            int green = (int)((float)neopix_gamma[g1] * intens2) + ((float)neopix_gamma[g2] * intens1);
            int blue = (int)((float)neopix_gamma[b1] * intens2) + ((float)neopix_gamma[b2] * intens1);
            strip.setPixelColor(i, strip.Color(red, green, blue));
            delay(0);
          }
          strip.show();
        }
        t3 = millis();
        t4 = t3;
        while (t3 - t4 <= (s1 * 1000)) {
          t3 = millis();
          server.handleClient();
          if (sysmode != 4) {
            loopOK = false;
            return;
          }
          time_t t = now();
          displayTime(dc, milt, cbrite, sysmode, hour(t), minute(t));
        }

      }
      if (q = 2) {
        for (int f = 0; f < 256; f++) {
          float intens1 = f / 256.0;
          float intens2 = 1.0 - f / 256.0;
          for (uint16_t i = 0; i < strip.numPixels(); i++) {
            int red = (int)((float)neopix_gamma[r2] * intens2) + ((float)neopix_gamma[r3] * intens1);
            int green = (int)((float)neopix_gamma[g2] * intens2) + ((float)neopix_gamma[g3] * intens1);
            int blue = (int)((float)neopix_gamma[b2] * intens2) + ((float)neopix_gamma[b3] * intens1);
            strip.setPixelColor(i, strip.Color(red, green, blue));
            delay(0);
          }
          strip.show();
        }
        t3 = millis();
        t4 = t3;
        while (t3 - t4 <= (s1 * 1000)) {
          t3 = millis();
          server.handleClient();
          if (sysmode != 4) {
            loopOK = false;
            return;
          }
          time_t t = now();
          displayTime(dc, milt, cbrite, sysmode, hour(t), minute(t));
        }

      }
      if (q = 3) {
        for (int f = 0; f < 256; f++) {
          float intens1 = f / 256.0;
          float intens2 = 1.0 - f / 256.0;
          for (uint16_t i = 0; i < strip.numPixels(); i++) {
            int red = (int)((float)neopix_gamma[r3] * intens2) + ((float)neopix_gamma[r1] * intens1);
            int green = (int)((float)neopix_gamma[g3] * intens2) + ((float)neopix_gamma[g1] * intens1);
            int blue = (int)((float)neopix_gamma[b3] * intens2) + ((float)neopix_gamma[b1] * intens1);
            strip.setPixelColor(i, strip.Color(red, green, blue));
            delay(0);
          }
          strip.show();
        }
        t3 = millis();
        t4 = t3;
        while (t3 - t4 <= (s1 * 1000)) {
          t3 = millis();
          server.handleClient();
          if (sysmode != 4) {
            loopOK = false;
            return;
          }
          time_t t = now();
          displayTime(dc, milt, cbrite, sysmode, hour(t), minute(t));
        }
      }
    }
  }
}

void cylinder() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0 + i; j < strip.numPixels(); j = j + 3) {
      strip.setPixelColor( j, strip.Color( r1, g1, b1 ) );
    }
    for (int j = 1 + i; j < strip.numPixels(); j = j + 3) {
      strip.setPixelColor( j, strip.Color( r2, g2, b2 ) );
    }
    for (int j = 2 + i; j < strip.numPixels(); j = j + 3) {
      strip.setPixelColor( j, strip.Color( r3, g3, b3 ) );
    }
    strip.show();
    int delTime = (60 / s2) * 50;
    delay(delTime);
    time_t t = now();
    if (secs != second(t)) {
      secs = second(t);
      displayTime(dc, milt, cbrite, sysmode, hour(t), minute(t));
    }
    server.handleClient();
    if (sysmode != 5) {
      return;
    }
  }
}
void vortex() {
  int delTime = (60 / s2) * 50;
  int ind = 0;
  int u = 1;
  t1 = millis();
  t2 = t1;
  while (t1 - t2 <= 14400000) { //no more than 4 hours of vortex
    t1 = millis();
    rrand1 = r1 + (int)random(-1, 1);
    grand1 = g1 + (int)random(-1, 1);
    brand1 = b1 + (int)random(-1, 1);
    rrand2 = r2 + (int)random(-1, 1);
    grand2 = g2 + (int)random(-1, 1);
    brand2 = b2 + (int)random(-1, 1);
    rrand3 = r3 + (int)random(-1, 1);
    grand3 = g3 + (int)random(-1, 1);
    brand3 = b3 + (int)random(-1, 1);
    time_t t = now();
    if (secs != second(t)) {
      secs = second(t);
      displayTime(dc, milt, cbrite, sysmode, hour(t), minute(t));
    }
    server.handleClient();
    if (sysmode != 6) {
      return;
    }
    if (u == 1) {
      vortexR[ind] = rrand1;
      vortexG[ind] = grand1;
      vortexB[ind] = brand1;
    }
    if (u == 2) {
      vortexR[ind] = rrand2;
      vortexG[ind] = grand2;
      vortexB[ind] = brand2;
    }
    if (u == 3) {
      vortexR[ind] = rrand3;
      vortexG[ind] = grand3;
      vortexB[ind] = brand3;
    }
    u++;
    if (u == 4) {
      u = 1;
    }
    ind++;
    if (ind == strip.numPixels()) {
      ind = 0;
    }
    //pointer=(++pointer)%total_element
    int k=ind;
    for (int j = 0; j < strip.numPixels(); j ++) {
      strip.setPixelColor( j, strip.Color( vortexR[k], vortexG[k], vortexB[k]));
      k++;
      if(k>strip.numPixels()-1){
        k=0;
      }
    }
    strip.show();
    delay(delTime);
  }
  server.handleClient();
  delay(delTime);
}




//example neopixel light patterns
/**************************************************************************************/

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void pulseWhite(uint8_t wait) {
  for (int j = 0; j < 256 ; j++) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0, neopix_gamma[j] ) );
    }
    delay(wait);
    strip.show();
  }

  for (int j = 255; j >= 0 ; j--) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0, neopix_gamma[j] ) );
    }
    delay(wait);
    strip.show();
  }
}


void rainbowFade2White(uint8_t wait, int rainbowLoops, int whiteLoops) {
  float fadeMax = 100.0;
  int fadeVal = 0;
  uint32_t wheelVal;
  int redVal, greenVal, blueVal;

  for (int k = 0 ; k < rainbowLoops ; k ++) {

    for (int j = 0; j < 256; j++) { // 5 cycles of all colors on wheel

      for (int i = 0; i < strip.numPixels(); i++) {

        wheelVal = Wheel(((i * 256 / strip.numPixels()) + j) & 255);

        redVal = red(wheelVal) * float(fadeVal / fadeMax);
        greenVal = green(wheelVal) * float(fadeVal / fadeMax);
        blueVal = blue(wheelVal) * float(fadeVal / fadeMax);

        strip.setPixelColor( i, strip.Color( redVal, greenVal, blueVal ) );

      }

      //First loop, fade in!
      if (k == 0 && fadeVal < fadeMax - 1) {
        fadeVal++;
      }

      //Last loop, fade out!
      else if (k == rainbowLoops - 1 && j > 255 - fadeMax ) {
        fadeVal--;
      }

      strip.show();
      delay(wait);
    }

  }



  delay(500);


  for (int k = 0 ; k < whiteLoops ; k ++) {

    for (int j = 0; j < 256 ; j++) {

      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0, neopix_gamma[j] ) );
      }
      strip.show();
    }

    delay(2000);
    for (int j = 255; j >= 0 ; j--) {

      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0, neopix_gamma[j] ) );
      }
      strip.show();
    }
  }

  delay(500);


}

void whiteOverRainbow(uint8_t wait, uint8_t whiteSpeed, uint8_t whiteLength ) {

  if (whiteLength >= strip.numPixels()) whiteLength = strip.numPixels() - 1;

  int head = whiteLength - 1;
  int tail = 0;

  int loops = 3;
  int loopNum = 0;

  static unsigned long lastTime = 0;


  while (true) {
    for (int j = 0; j < 256; j++) {
      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        if ((i >= tail && i <= head) || (tail > head && i >= tail) || (tail > head && i <= head) ) {
          strip.setPixelColor(i, strip.Color(0, 0, 0, 255 ) );
        }
        else {
          strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
        }

      }

      if (millis() - lastTime > whiteSpeed) {
        head++;
        tail++;
        if (head == strip.numPixels()) {
          loopNum++;
        }
        lastTime = millis();
      }

      if (loopNum == loops) return;

      head %= strip.numPixels();
      tail %= strip.numPixels();
      strip.show();
      delay(wait);
    }
  }

}
void fullWhite() {

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0, 255 ) );
  }
  strip.show();
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    t1 = millis();
    t2 = t1;
    while (t1 - t2 <= wait) { //no more than 4 hours of color cycle at a time{
      t1 = millis();
      server.handleClient();
      if (sysmode != 7) {
        return;
      }
    }
  }
}
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3, 0);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0, 0);
}

uint8_t red(uint32_t c) {
  return (c >> 16);
}
uint8_t green(uint32_t c) {
  return (c >> 8);
}
uint8_t blue(uint32_t c) {
  return (c);
}

void sendNTPpacket(IPAddress & address)
{
  Serial.println("sending NTP packet...");
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  udp.beginPacket(address, 123);
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
