#include <Arduino.h>
#include <WifiEspNow.h>

int int_data;
float float_data;
bool bool_data;

uint8_t broadcastAddress[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
typedef struct struct_message {
    char string_message[32];
    int int_mess;
    float float_mess;
    bool bool_mess;
} struct_message;

struct_message myData;

