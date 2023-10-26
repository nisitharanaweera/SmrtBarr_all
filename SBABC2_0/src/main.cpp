#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "time.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

// Wifi Credentials
#define WIFI_SSID "RAND-MIFI"
#define WIFI_PASSWORD "reddeadred"

// #define WIFI_SSID "RAND Sams"
// #define WIFI_PASSWORD "reddeadred"

// #define WIFI_SSID "UOC_Staff"
// #define WIFI_PASSWORD "admin106"


String GOOGLE_SCRIPT_ID = "AKfycbwZNXferlDm-ufubQ91T9pBclif7jMDNW74CdjHQLeMuA_U4j-y1f1w5l2MPMXk_U-F"; 

int timestamp;

String logD = "";



//----------------------Functions----------------------------------------

void initWiFi() {
  
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  Serial.flush();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}


void sendData(String m){
  String msg = m;
  if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    timestamp = getTime();

    // char timeStringBuff[50]; //50 chars should be enough
    // strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    String asString(timestamp);
    // asString.replace(" ", "-");
    Serial.print("Time:");
    Serial.println(asString);
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"timestamp=" + asString + "&log=" + String(msg);
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }
    //---------------------------------------------------------------------
    http.end();
  }
  delay(1000);
}

const int masterPwr = 14;
const int panicStop = 12;
const int reset = 27;
const int arming = 26;
const int disarming = 25;

const int masterPwrLed = 16;
const int panicStopLed = 17;
const int resetLed = 5;
const int armingLed = 18;
const int disarmingLed = 19;

void setup() {
  delay(500);
  Serial.begin(115200);
  delay(500);

  initWiFi();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  pinMode(masterPwr, INPUT_PULLUP);
  pinMode(panicStop, INPUT_PULLUP);
  pinMode(reset, INPUT_PULLUP);
  pinMode(arming, INPUT_PULLUP);
  pinMode(disarming, INPUT_PULLUP);
  
  pinMode(masterPwrLed, OUTPUT);
  pinMode(panicStopLed, OUTPUT);
  pinMode(resetLed, OUTPUT);
  pinMode(armingLed, OUTPUT);
  pinMode(disarmingLed, OUTPUT);
 
}







void loop() {
  String m = "";

  if (digitalRead(masterPwr) == LOW) {
    // m = "Master Power";
    m = "0";
    digitalWrite(masterPwrLed, HIGH);
    delay(200);
    while (digitalRead(masterPwr) == LOW) {
    }
    digitalWrite(masterPwrLed, LOW);
  }
  
  if (digitalRead(panicStop) == LOW) {
    // m = "Panic Stop Initiated!";
    m = "1";
    digitalWrite(panicStopLed, HIGH);
    delay(200);
    while (digitalRead(panicStop) == LOW) {
    }
    digitalWrite(panicStopLed, LOW);
  }
  if (digitalRead(reset) == LOW) {
    //m = "System Reset"
    m = "2";
    digitalWrite(resetLed, HIGH);
    delay(200);
    while (digitalRead(reset) == LOW) {
    }
    digitalWrite(resetLed, LOW);
  }

  if (digitalRead(arming) == LOW) {
    //m = "System Arming"
    m = "3";
    digitalWrite(armingLed, HIGH);
    delay(200);
    while (digitalRead(arming) == LOW) {
    }
    digitalWrite(armingLed, LOW);
  }

  if (digitalRead(disarming) == LOW) {
    //m = "System Disarming"
    m = "4";
    digitalWrite(disarmingLed, HIGH);
    delay(200); // Debounce delay
    while (digitalRead(disarming) == LOW) {
    }
    digitalWrite(disarmingLed, LOW);
  }

  
  if ( m !="" ){
    sendData(m);
    Serial.println(m);

  }

}


