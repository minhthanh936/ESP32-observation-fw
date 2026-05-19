#ifndef BMP280SENSOR_H
#define BMP280SENSOR_H

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

void setupBMP();
String getBMPData();

#endif
