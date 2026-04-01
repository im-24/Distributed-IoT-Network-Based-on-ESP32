#include <Arduino.h>
#include <WifiEspNow.h>


void setup(){
    Serial.begin(115200);
    WiFi.mode(WIFI_MODE_STA);
    Serial.println("mac address");
    Serial.println(WiFi.macAddress());
}
void loop(){}