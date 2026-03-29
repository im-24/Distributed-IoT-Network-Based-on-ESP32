#include <Arduino.h>
#include <WifiEspNow.h>
#include <ESPNowW.h>


typedef struct struct_message {
  char a[32];
} struct_message;

struct_message myData;

void OnDataRecv (const uint8_t *mac, const uint8_t *incomingData , int len){
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Data receiver :");
  Serial.println(len);
  Serial.print("character value :");
  Serial.println(myData.a);
}

void setup() {
Serial.begin(115200);
  Serial.println("the node is here ");
  WiFi.mode(WIFI_STA);
  if(esp_now_init() != ESP_OK)
    {
      Serial.println("error initializing in node esp-now");
      return;
    }
  esp_now_register_recv_cb(OnDataRecv);

  
    
}

void loop() {
}