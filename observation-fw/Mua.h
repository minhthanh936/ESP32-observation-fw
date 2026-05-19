#ifndef MUA_H
#define MUA_H

#include <Arduino.h>
#include <string>

#define MUA_PIN 19
/*class Mua {
  public:
    Mua();  // Constructor, nhận chân của cảm biến
    void begin();  // Khởi tạo cảm biến
    void read();   // Đọc giá trị cảm biến và in ra Serial Monitor

  private:
    int _pin;      // Chân của cảm biến
    int _rainValue; // Giá trị cảm biến mưa
};*/
void setupMUA();
String getMUAData();

#endif
