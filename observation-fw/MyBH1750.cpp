#include "MyBH1750.h"

BH1750 lightMeter;
struct BH1750Data {
    float lux;
    unsigned long timestamp;
}; 

String sensorDataToJson(const BH1750Data& data) {
    String json = "{";
    json += "\"lux\":" + String(data.lux, 1) + ",";
    json += "\"timestamp\":" + String(data.timestamp);
    json += "}";
    return json;
}

void setupBH1750() {
    Wire1.begin(SDA, SCL);
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire1);
}

String getLightLevel() {
    //Wire.begin(SDA, SCL);
    BH1750Data data;
    data.lux = lightMeter.readLightLevel();
    data.timestamp = millis();
    if (isnan(data.lux)) {
        String b = "Failed to read from BH1750 sensor!";
        return b;
    }
    else{
        String jsonData = sensorDataToJson(data);
        return jsonData;
    }
    //Wire.end();
}
