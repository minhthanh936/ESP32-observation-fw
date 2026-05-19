#include <WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <sstream>
#include "Stream.h"
#include "dht22.h"
#include "MyBMP280Sensor.h"
#include "MyBH1750.h"
#include "Mua.h"

#include "esp_ota_ops.h"
#include "esp_partition.h"
#include <Preferences.h>
#include "Ticker.h"
#include "nvs_flash.h"
Ticker apiTicker;
Preferences preferences;
#include <Arduino_JSON.h>

// WiFi
const char *ssid = "XSOLAR_2.4";
const char *password = "xsolar@123";
//const char *ssid = "HCMUT_03";
//const char *password = "20242024";
//const char *ssid = "T2"; // Enter your Wi-Fi nameta: {"temperature":27.2,"humidity":61.3}
//const char *password = "22155312";  // Enter Wi-Fi password
// MQTT Broker
//const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_broker = "192.168.63.40";
//const char *topic = "Pad/E";
//const char *mqtt_username = "public";
//const char *mqtt_password = "public";
const int mqtt_port = 1883;

const char *DHTtopic = "topic/dht22/sub";
const char *BMPtopic = "topic/BMP280/sub";
const char *BH1750topic = "topic/BH1750/sub";
const char *MUAtopic = "topic/MUA/sub";
const char *ota_sub_topic = "topic/OTA/sub";
const char *mqtt_username = "SmartDesk";
const char *mqtt_password = "@123";
//const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OTA /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//char ota_sub_topic[32];

#define FIRMWARE_VERSION "1.0.0"
#define DEVICE_TYPE "weather_forcast"

String new_version;
String device_type; 
bool is_sync_all; 
std :: vector<String> mac_id;

void store_version_in_nvs(void) {
  const esp_partition_t* running_partition = esp_ota_get_running_partition();
    if (!running_partition) {
        ESP_LOGE("store_version", "Unable to find running partition");
        return;
    }
    preferences.begin("storage", false); 
    preferences.putString("ver", FIRMWARE_VERSION);
    Serial.println("Firmware version stored in NVS: " +String(FIRMWARE_VERSION));
    preferences.end();
}
bool isVersionNewer(String currentVersion, String newVersion) {
    int currentMajor, currentMinor, currentPatch;
    int newMajor, newMinor, newPatch;

    sscanf(currentVersion.c_str(), "%d.%d.%d", &currentMajor, &currentMinor, &currentPatch);
    sscanf(newVersion.c_str(), "%d.%d.%d", &newMajor, &newMinor, &newPatch);

    if (newMajor > currentMajor) return true;
    else if (newMajor == currentMajor) {
        if (newMinor > currentMinor) return true;
        else if (newMinor == currentMinor) {
            if (newPatch > currentPatch) return true;
        }
    }
    return false;
}

String json_message; 
bool waiting_for_json = false;
unsigned long startMillis;

void mqttCallback(char* topic, byte* payload, unsigned int length) {

    if (waiting_for_json) {
        json_message = "";  // Clear message
        for (unsigned int i = 0; i < length; i++) {
            json_message += (char)payload[i];
        }
    }
}

void getJsonFromMqttBroker(const char* topic) {
  if (!client.connected()) {
        Serial.println("MQTT client disconnected, reconnecting...");
        check_reconnect_mqtt();
  }
  waiting_for_json = true;
  client.setCallback(mqttCallback);
  client.subscribe(topic);
  startMillis = millis();
}

void get_info_from_json (String json) {
    JSONVar doc = JSON.parse(json);

    if (JSON.typeof(doc) == "undefined") {
        Serial.println("Parsing JSON failed!");
        return;
    }
    new_version = (const char*) doc["version"];
    device_type = (const char*) doc["deviceType"];
    // if (JSON.typeof(doc["macID"]) == "array") {
    //   mac_id.clear();
    //   for (int i = 0; i<doc["macID"].length(); i++){
    //     mac_id.push_back((const char*) doc["macID"][i]);
    //   }
    // }else{
    //   return;
    // }
    // is_sync_all = (bool) doc["isSyncAll"];
}

void Task_reboot() {
  if (isVersionNewer(FIRMWARE_VERSION, new_version)) {
        printf("\n OTA update available");

        // Tìm phân vùng factory
        const esp_partition_t *factory_partition = esp_partition_find_first(
            ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);

        if (factory_partition != NULL) {
            printf("\n Found factory partition, switching boot partition...");

            // Chuyển boot partition sang phân vùng factory
            esp_err_t err = esp_ota_set_boot_partition(factory_partition);
            if (err == ESP_OK) {
                printf("\n Factory partition set successfully, restarting...");
                esp_restart(); 
            } else {
                printf("\n Failed to set factory partition, error: %d", err);
            }
        } else {
            printf("\n No factory partition found!");
        }
    } else {
        printf("\n");
        printf("\n No new version available ");
    }
  json_message = "";
}

bool check_mac_id (const String& g_mac_id){
  return(std :: find(mac_id.begin(), mac_id.end(), g_mac_id) != mac_id.end());
}

void timer_callback() {
    getJsonFromMqttBroker(ota_sub_topic); 
    if (json_message == "") {
      printf("\n No message from broker");
      return;
    }

    get_info_from_json(json_message);
    if (new_version == "") {
        printf("\n Cannot parse version from JSON");
        return;
    }

    else {
      printf("\n Processing ... ");
      Task_reboot();
    }
    printf("\n OTA is not available now !!!");
    
}
/////////////////////////////////////////////////////////store wifi data to nvs
void store_wifi_info_in_nvs(void) {
  const esp_partition_t* running_partition = esp_ota_get_running_partition();
    if (!running_partition) {
        ESP_LOGE("store_wifi_data", "Unable to find running partition");
        return;
    }
    preferences.begin("storage", false); 
    preferences.putString("ssid", ssid);
    preferences.putString("password",password);
    Serial.println("wifi data stored in NVS: " +String(ssid)+ " " + String(password));
    preferences.end();
}

/////////////////////////////////////////////////////////////////////////////

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
    check_reconnect_mqtt();

    //apiTicker.attach(5, timer_callback);
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
void check_reconnect_mqtt(){
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

