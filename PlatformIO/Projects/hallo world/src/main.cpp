#include <Arduino.h>


void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  Serial1.begin(921600);
  Serial.println("\nhallo world");
}

void loop() {
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial1.println("\nthe light is on");
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("\nthe light is off");
}
