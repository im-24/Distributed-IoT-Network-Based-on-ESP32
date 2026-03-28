#include <Arduino.h>
#include <WifiEspNow.h>
#include <WiFi.h>

void setup() {
  
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println("hello esp32");
  Serial.print("MAC Address:");
  Serial.println(WiFi.macAddress());
}

void loop() {


}
