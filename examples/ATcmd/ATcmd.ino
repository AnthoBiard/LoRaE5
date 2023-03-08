#include <lorae5.h>
#include "config_application.h"
#include "LoRaWANNode.h"
#include <Arduino.h>

bool otaa = (ACTIVATION_MODE==OTAA)? true : false;

LoraE5_send loraE5_send(SPREADING_FACTOR, ADAPTIVE_DR, otaa, devEUI, devAddr, nwkSKey, appSKey, appKey, appEUI); 

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  loraE5_send.printInfo();
  loraE5_send.sendATConfig();
}

void loop() {
    delay(10000);
    loraE5_send.sendMsg("AT+MSG");
}
