#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient
#include "DHT.h"
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <Time.h>
#include <TimeAlarms.h>  //https://github.com/PaulStoffregen/TimeAlarms
#include <NTPClient.h>   //https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>

#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define mqtt_server "6.13.0.120"
#define mqtt_user "garden"
#define mqtt_password "garden"

const int  statusPagePort = 8266;
const char* SOFTWARE_VERSION = "3.1 Garden Controller";
const char* DEVICENAME = "GardenController"; 
const int RED_DATA_PIN = 0;
const int WIFI_INFO_LED_PIN = 2;
const int WIFI_RESET_PIN = 14;
const long CHECK_WIFI_INTERVAL = 30000;
const long CHECK_MQTT_INTERVAL = 30000;
const long CHECK_SENSORS_INTERVAL = 5000;
const long WATER_CHECK_SENSORS_INTERVAL = 5000;
unsigned long prevMillisSensors = 0;
unsigned long wprevMillisSensors = 0;
unsigned long currMillis = millis();
unsigned long prevMillisMQTT = 0;
const char*   ntpServer   = "0.us.pool.ntp.org";
const int8_t  ntpOffset   = -4; // hours
const uint8_t ntpInterval = 5; // minutes
struct {
  char hours;
  char minutes;
  char seconds;
  uint8_t nextNtp;
} timeStruct;

MDNSResponder mdns;
ESP8266WebServer server(statusPagePort); //this cant be running on port 80 because of the wifimanager
WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wifiManager;
DHT dht(DHTPIN, DHTTYPE);

WiFiUDP time_udp;
NTPClient timeClient(time_udp, ntpServer, ntpOffset * 3600);

void setup() {
  Serial.begin(9600);
  pinMode(WIFI_RESET_PIN,INPUT_PULLUP);
  pinMode(WIFI_INFO_LED_PIN,OUTPUT);
  pinMode(RED_DATA_PIN,OUTPUT);
  digitalWrite(RED_DATA_PIN,HIGH);
  //wifiManager.setSTAStaticIPConfig(IPAddress(6,13,0,218), IPAddress(6,13,0,1), IPAddress(255,255,255,0)); //Remove this for DHCP
  wifiManager.autoConnect("ESPSetup", "wifiSetup1");
  client.setServer(mqtt_server, 1883);
  dht.begin();

  setupWeb();
  getTimeFromNtp();
  Serial.println((String)"Current Time: " + timeStruct.hours + ":" + timeStruct.minutes + ":" + timeStruct.seconds);
  //scheduler
  setTime((char)timeStruct.hours,(char)timeStruct.minutes,(char)timeStruct.seconds,1,1,19);
  //Schedule disabled while reorg controller
  //Alarm.alarmRepeat(7,0,0, activatePump);
  //Alarm.alarmRepeat(12,30,0, activatePump);
  //Alarm.timerRepeat(15, activatePump);
}

//main loop
void loop() {
  // wire a button with a 10k resistor to ground and the other end to pin 14 for resetting and to prompt for new network
  if ( digitalRead(WIFI_RESET_PIN) == LOW ) {
    WiFiManager wifiManager;
    wifiManager.setTimeout(600);
    WiFi.mode(WIFI_STA);
    wifiManager.startConfigPortal("OnDemandAP");
    Serial.println("Autoconnect portal running");
  }
  
  client.loop();
  checkSensors();
  server.handleClient();
}
