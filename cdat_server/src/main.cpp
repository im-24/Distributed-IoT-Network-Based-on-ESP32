#include <Arduino.h>
#include <WifiEspNow.h>
#include <ESPNowW.h>

typedef struct struct_message {
  char a[32];
} struct_message;

struct_message myData;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status ){
  Serial.print("\r\nlast packet send status\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS);

}

void OnDataRecv (const uint8_t *mac, const uint8_t *incomingData , int len){
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Data receiver :");
  Serial.println(len);
  Serial.print("character value :");
  Serial.println(myData.a);
}

int int_v;
float float_v;
bool bool_v = true;
String mac = WiFi.macAddress();

uint8_t broadcastAddress[] = {0x24, 0x6F , 0x28, 0x7A, 0xAE , 0x0C };
esp_now_peer_info_t peerInfo;


void setup()
{
//init esp-now
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("error initializing esp-now");
    return;
  }

  //test which board
  if (mac == "24:6F:28:7A:AE:0C")
  {
    Serial.println("here the node ");
    esp_now_register_recv_cb(OnDataRecv);
  }else{
    esp_now_register_send_cb(OnDataSent);
    memcpy(peerInfo.peer_addr, broadcastAddress ,6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo)!= ESP_OK ){
      Serial.println("fialed to peer");
      return;
    }
}
}

void loop() {

  if (mac != "24:6F:28:7A:AE:0C"){
  int_v = random(1,200);
  float_v = 1.55 * int_v;
  bool_v = !bool_v;
  strcpy(myData.a, "salam from server ");
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)& myData, sizeof(myData));
  if (result == ESP_OK){
    Serial.println("sending confirmed");

  }else{
    Serial.println("sending error ");

  }
  delay(2000);
}

}