#include <WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <sstream>
#include "Stream.h"
#include "dht22.h"
#include "MyBMP280Sensor.h"
#include "MyBH1750.h"
#include "Mua.h"
// WiFi
//const char *ssid = "XSOLAR_2.4";
//const char *password = "xsolar@123";
const char *ssid = "The Coffee House";
const char *password = "thecoffeehouse";
//const char *ssid = "HCMUT_03";
//const char *password = "20242024";
//const char *ssid = "T2"; // Enter your Wi-Fi nameta: {"temperature":27.2,"humidity":61.3}
//const char *password = "22155312";  // Enter Wi-Fi password
// MQTT Broker
//const char *mqtt_broker = "broker.emqx.io";
//const char *topic = "Pad/E";
//const char *mqtt_username = "public";
//const char *mqtt_password = "public";
//const int mqtt_port = 1883;

const char *mqtt_broker = "192.168.0.108";
const char *DHTtopic = "topic/dht22/sub";
const char *BMPtopic = "topic/BMP280/sub";
const char *BH1750topic = "topic/BH1750/sub";
const char *MUAtopic = "topic/MUA/sub";
const char *ota_sub_topic = "topic/OTA/sub";
const char *mqtt_username = "SmartDesk";
const char *mqtt_password = "@123";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  //dht22
  setupDHT();

  //light sensor
  setupBH1750();

  setupMUA();

  //bmp280
  setupBMP();

  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
  SendMessage();
  client.loop();
  delay(5000);
}

void SendMessage(){
    if (client.connected()){
        Serial.println("-----------------------");
        //light data
        String bh1750 = getLightLevel();
        Serial.println(bh1750);
        client.publish(BH1750topic, bh1750.c_str());

        //dht data.
        String dht = getTempHum();
        Serial.println(dht);
        client.publish(DHTtopic, dht.c_str());

        //MUA data
        String MUA = getMUAData();
        Serial.println(MUA);
        client.publish(MUAtopic, MUA.c_str());

        //bmp data
        String bmp = getBMPData();
        Serial.println(bmp);
        client.publish(BMPtopic, bmp.c_str());
  
        Serial.println("-----------------------");
        // Publish and subscribe
        Serial.print("|Client's state: ");
        Serial.println(client.state());
    }
}

