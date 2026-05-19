#ifndef DHT22_H
#define DHT22_H

#include <DHT.h>
#include <string>

#define DHT22_PIN 21  // ESP32 pin GPIO21 connected to DHT22 sensor

void setupDHT();
String getTempHum();

#endif
