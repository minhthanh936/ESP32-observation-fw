#include "Mua.h"

/*Mua::Mua() {
  _pin = MUA_PIN;  // Khởi tạo chân cảm biến
}

void Mua::begin() {
  pinMode(_pin, INPUT);  // Thiết lập chân cảm biến là INPUT
}

void Mua::read() {
  _rainValue = analogRead(_pin);  // Đọc giá trị cảm biến (0-1023)
  Serial.print("Giá trị cảm biến: ");
  Serial.println(_rainValue);

  // Ngưỡng: càng thấp thì càng ẩm ướt (có mưa), càng cao thì càng khô
  if (_rainValue < 500) {  // Ngưỡng này có thể thay đổi tùy theo thực tế
    Serial.println("🌧️ Trời mưa");
  } else {
    Serial.println("☀️ Không mưa");
  }
}*/
struct MUAData {
    int MUA;
    unsigned long timestamp;
}; 

String sensorDataToJson(const MUAData& data) {
    String json = "{";
    json += "\"Mua\":" + String(data.MUA) + ",";
    json += "\"timestamp\":" + String(data.timestamp);
    json += "}";
    return json;
}

void setupMUA() {
    pinMode(MUA_PIN, INPUT);  // Thiết lập chân cảm biến là INPUT
}

String getMUAData() {
    int _rainValue = analogRead(MUA_PIN);  // Đọc giá trị cảm biến (0-1023)
  Serial.print("Giá trị cảm biến: ");
  Serial.println(_rainValue);
  MUAData data;
  data.timestamp = millis();
  // Ngưỡng: càng thấp thì càng ẩm ướt (có mưa), càng cao thì càng khô
  if (_rainValue < 500) {  // Ngưỡng này có thể thay đổi tùy theo thực tế
    Serial.println("🌧️ Trời mưa");
    data.MUA = 1;
  } else {
    Serial.println("☀️ Không mưa");
    data.MUA = 0;
  }
  String jsonData = sensorDataToJson(data);
  return jsonData;
}