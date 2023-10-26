#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "time.h"


// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


// Wifi Credentials
// #define WIFI_SSID "RAND-MIFI"
// #define WIFI_PASSWORD "reddeadred"

#define WIFI_SSID "RAND Sams"
#define WIFI_PASSWORD "reddeadred"

// #define WIFI_SSID "UOC_Staff"
// #define WIFI_PASSWORD "admin106"

// Firebase project API Key
#define API_KEY "AIzaSyBgOX3RQvZEk8YrYz8EMM6TNEVOlWYgI64"

// Authorized Email and Corresponding Password
#define USER_EMAIL "smartbarrierabc@gmail.com"
#define USER_PASSWORD "smartBarrier@ABCUOC"

// RTDB URLefine the RTDB URL
#define DATABASE_URL "https://smart-barrier-42818-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String logPath = "/logdata";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;
FirebaseJson json;

const char* ntpServer = "pool.ntp.org";

// Timer variables (send new readings every three minutes)
// unsigned long sendDataPrevMillis = 0;
// unsigned long timerDelay = 180000;
String logD = "";



//--------------------------------------------------------------

void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
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

void sendData(String msg){
  logD = msg;

    if (Firebase.ready()){
    
    //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);

    parentPath= databasePath + "/" + String(timestamp);

    json.set(logPath.c_str(), logD);
    json.set(timePath, String(timestamp));

    Serial.printf("Set json... %s\n",
     Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    }
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
  Serial.begin(115200);
  initWiFi();
  configTime(0, 0, ntpServer);

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

//---------------------------------Firebase U ID claiming-----------------------------------
  // Assign the api key 
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL 
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";

//---------------------------------Firebase U ID claiming end----------------------------



}







void loop() {
  String m = "";

  if (digitalRead(masterPwr) == LOW) {
    m = "Master Power";
    digitalWrite(masterPwrLed, HIGH);
    delay(200);
    while (digitalRead(masterPwr) == LOW) {
    }
    digitalWrite(masterPwrLed, LOW);
  }
  
  if (digitalRead(panicStop) == LOW) {
    m = "Panic Stop Initiated!";
    digitalWrite(panicStopLed, HIGH);
    delay(200);
    while (digitalRead(panicStop) == LOW) {
    }
    digitalWrite(panicStopLed, LOW);
  }
  if (digitalRead(reset) == LOW) {
    m = "System Reset";
    digitalWrite(resetLed, HIGH);
    delay(200);
    while (digitalRead(reset) == LOW) {
    }
    digitalWrite(resetLed, LOW);
  }

  if (digitalRead(arming) == LOW) {
    m = "System Arming";
    digitalWrite(armingLed, HIGH);
    delay(200);
    while (digitalRead(arming) == LOW) {
    }
    digitalWrite(armingLed, LOW);
  }

  if (digitalRead(disarming) == LOW) {
    m = "System Disarming";
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
