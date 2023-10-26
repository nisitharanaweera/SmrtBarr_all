void setup() {
  pinMode(16, OUTPUT);

}

void loop() {
  digitalWrite(16, HIGH);
  Serial.println("Master Power");
  delay(200);
  digitalWrite(16,LOW);
  delay(200);

}
