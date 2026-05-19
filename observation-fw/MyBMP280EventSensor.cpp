#include "MyBMP280Sensor.h"
#include <Adafruit_BMP280.h>
#define BMP280_ADDRESS 0x76
#define SDA_PIN 23
#define SCL_PIN 22

struct BMPData {
    float temperature;
    float pressure;
    float altitude;
};

String sensorDataToJson(const BMPData& data) {
    String json = "{";
    json += "\"temperature\":" + String(data.temperature, 1) + ",";
    json += "\"pressure\":" + String(data.pressure, 1) + ",";
    json += "\"altitude\":" + String(data.altitude, 1);
    json += "}";
    return json;
}

Adafruit_BMP280 bmp;  // Đối tượng cảm biến
unsigned status;

void setupBMP() {
  Wire.begin(SDA_PIN, SCL_PIN);  // BẮT BUỘC với ESP32-C6
  delay(100);

  status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
  }

  Serial.println("BMP280 found and initialized!");

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode.
                  Adafruit_BMP280::SAMPLING_X2,      // Temp. oversampling
                  Adafruit_BMP280::SAMPLING_X16,     // Pressure oversampling
                  Adafruit_BMP280::FILTER_X16,       // Filtering.
                  Adafruit_BMP280::STANDBY_MS_500);  // Standby time.

  Serial.println("Set up BMP280 successfully!");
}

String getBMPData() {
  //Wire.begin(SDA_PIN, SCL_PIN);  // BẮT BUỘC với ESP32-C6
  BMPData data;
  data.temperature = bmp.readTemperature();
  data.pressure = bmp.readPressure();
  data.altitude = bmp.readAltitude(1013.25); // Bạn có thể thay 1013.25 bằng áp suất khí quyển địa phương nếu cần

  if (isnan(data.temperature) || isnan(data.pressure) || isnan(data.altitude)) {
    return "Failed to read from BMP sensor!";
  } else {
    return sensorDataToJson(data);
  }
  //Wire.end();
}
