#include "dht22.h"

DHT dht22(DHT22_PIN, DHT22);

struct DHTData {
  float temperature;
  float humidity;
};

String sensorDataToJson(const DHTData& data) {
  String json = "{";
  json += "\"temperature\":" + String(data.temperature, 1) + ",";
  json += "\"humidity\":" + String(data.humidity, 1);
  json += "}";
  return json;
}

void setupDHT() {
  dht22.begin();
}

String getTempHum() {
  DHTData data;
  data.humidity = dht22.readHumidity();
  data.temperature = dht22.readTemperature();
  if (isnan(data.temperature) || isnan(data.humidity)) {
    String b = "Failed to read from DHT22 sensor!";
    return b;
  } else {
    String jsonData = sensorDataToJson(data);
    return jsonData;
  }
}