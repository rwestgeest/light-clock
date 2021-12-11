#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

const char* ssid = "qworbi";
const char* password = "QW35t9335t19";

IPAddress timeServerIP;
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];

WiFiUDP udp;

void setup() {
  Serial.begin(115200);//set this baudrate in arduino

  Serial.println("connecting to wifi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("..");
    
    delay(500);
  }
  Serial.println("connected");

  udp.begin(301);
  WiFi.hostByName(ntpServerName, timeServerIP);
  sendNTPpacket(timeServerIP);

  Serial.println("udp sent");

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Doing Nothing");
  delay(1000);
}

unsigned long sendNTPpacket(IPAddress & address)
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
  return 0;
}
