#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <Time.h>
#include <TimeAlarms.h>  //https://github.com/PaulStoffregen/TimeAlarms
#include <NTPClient.h>   //https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//pins
const int RED_DATA_PIN = 0;
const int WIFI_INFO_LED_PIN = 2;
const int SENSOR_INFO_LED_PIN = 5;
const int PUMP_ACTIVATE_PIN = 12;
#define echoPin 13
const int WIFI_RESET_PIN = 14;
#define trigPin 15

//vars
#define mqtt_server "6.13.0.120"
#define mqtt_user "garden"
#define mqtt_password "garden"

const int  statusPagePort = 8266;
const char* SOFTWARE_VERSION = "3.1 Garden Controller";
const char* DEVICENAME = "GardenController"; 
const long ACTIVATE_DURATION = 300000;
const long CHECK_WIFI_INTERVAL = 120000;
const long CHECK_MQTT_INTERVAL = 120000;
const long CHECK_SENSORS_INTERVAL = 60000;
const long WATER_CHECK_SENSORS_INTERVAL = 120000;
#define SEALEVELPRESSURE_HPA (1013.25)

long duration;
int distance;
int distanceIn;
unsigned long deviceActivateStart;
unsigned long prevMillisSensors = 0;
unsigned long wprevMillisSensors = 0;
unsigned long prevMillisWiFi = 0;
unsigned long prevMillisMQTT = 0;
unsigned long currMillis = millis();

const char*   ntpServer   = "0.us.pool.ntp.org";
const int8_t  ntpOffset   = -4; // hours
const uint8_t ntpInterval = 5; // minutes
struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t nextNtp;
} timeStruct;

MDNSResponder mdns;
ESP8266WebServer server(statusPagePort); //this cant be running on port 80 because of the wifimanager
WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wifiManager;
Adafruit_BME280 bme; // I2C

WiFiUDP time_udp;
NTPClient timeClient(time_udp, ntpServer, ntpOffset * 3600);

void setup() {
  unsigned bmestatus;
  Serial.begin(9600);
  pinMode(WIFI_RESET_PIN,INPUT_PULLUP);
  pinMode(WIFI_INFO_LED_PIN,OUTPUT);
  pinMode(RED_DATA_PIN,OUTPUT);
  digitalWrite(RED_DATA_PIN,HIGH);
  pinMode(PUMP_ACTIVATE_PIN,OUTPUT);
  digitalWrite(PUMP_ACTIVATE_PIN,HIGH);
  pinMode(SENSOR_INFO_LED_PIN,OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  bmestatus = bme.begin(0x76); //changed to bme280 that was running on different than default
  Serial.println(bmestatus);
  if (!bmestatus) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }  
  //wifiManager.setSTAStaticIPConfig(IPAddress(6,13,0,218), IPAddress(6,13,0,1), IPAddress(255,255,255,0)); //Remove this for DHCP
  wifiManager.setHostname(DEVICENAME);
  if(!wifiManager.autoConnect("ESPSetup", "wifiSetup1")){
    wifiManager.autoConnect("ESPSetup", "wifiSetup1");
  }
  client.setServer(mqtt_server, 1883);
  
  setupWeb();
  getTimeFromNtp();
  Serial.println((String)"Current Time: " + (int)timeStruct.hours + ":" + (int)timeStruct.minutes + ":" + (int)timeStruct.seconds);
  //scheduler
  setTime((int)timeStruct.hours,(int)timeStruct.minutes,(char)timeStruct.seconds,1,1,19);
  Alarm.alarmRepeat(7,0,0, activatePump);
  Alarm.alarmRepeat(12,30,0, activatePump);
  //Alarm.timerRepeat(15, activatePump);
  //connect mqtt
  reconnect();
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
  checkWaterLevels();
  pumpRunning(); // wire a relay to pin 12 to the in1 on the relay, 5v to vcc and ground to ground
  //need to add logic to handle multiple different devices in the server in the wifi.connect where the server.on are declared. need to add
  //sprinkler - this will be a relay that triggers the water relay thing connected to the inlet from the house water when the humidity is low - need to run some sprinkler things from the roof runners
  //soak - this will be connected to a water relay thinger thats run off of the rain barrels if they are above a certain level
  //manual water - this will handle watering if the rain barrels are empty
  server.handleClient();
  Alarm.delay(100);
}
