#include <Arduino.h>
#include <datapack.h>

DataPack parser;

void setup() {
    Serial.begin(115200);
}

void loop() {
    if(parser.available(Serial)){
        uint8_t counter = parser.inWaiting();
        uint8_t* data_keys = parser.getKeys();
        for(int i=0;i<counter;i++){
            Serial.print("Key: ");Serial.println(data_keys[i]);
            Serial.print("Data: ");Serial.println(parser.getData(data_keys[i]));
        }
        delete(data_keys);
        if(parser.hasKey(0xA1)){
            Serial.println("Key Found!");
        }
    }
}