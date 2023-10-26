#include <Arduino.h>

const char sw1 = 14;
const char sw2 = 12;
const char lowLim = 2;
const char highLim = 15;
const char introptPin = 13;


namespace MT{
  const char relay1 = 21;
  const char relay2 = 22;

  void setup_MT(){
    pinMode(relay1,OUTPUT);
    pinMode(relay2,OUTPUT);
    pinMode(sw1,INPUT_PULLUP);
    pinMode(sw2,INPUT_PULLUP);

    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
  }

  void anticlockw(){
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, LOW);
  }
  void clockw(){
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, HIGH);
  }
  void stop(){
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
  }

} // namespace MT


namespace US{
  const int trigPin = 33;
  const int echoPin = 32;
  const int nReads = 5;

  float duration, distance;

  void setup_US(){
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
  }

  bool isClose(){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // float t = 0;
    // for (size_t i = 0; i < nReads; i++){
    //   duration = pulseIn(echoPin, HIGH);
    //   distance = (duration*.0343)/2;
    //   t += distance;
    // }
    

    duration = pulseIn(echoPin, HIGH);
    distance = (duration*.0343)/2;
    Serial.print("Distance: ");
    Serial.println(distance);
    if (distance<100) return true;
    else return false;
    
  }
}

namespace funcs{
  void listen(){
    Serial.println("Armed and Waiting");
    while(digitalRead(sw1));
    Serial.println("Disarming");
    MT::clockw();
    delay(2700);
    MT::stop();
    Serial.println("Disarmed");
  }

  void US_wait(){
    Serial.println("Vehicle Passing");
    while(US::isClose()==1);
    delay(1000);
    Serial.println("Re-Arming");
    MT::anticlockw();
    delay(3000);
    MT::stop();
    Serial.println("Re-Armed");
  }
}



void setup() {
  Serial.begin(9600);
  MT::setup_MT();
  US::setup_US();
}
  
void loop() {
  funcs::listen();
  delay(4000);
  funcs::US_wait();
}