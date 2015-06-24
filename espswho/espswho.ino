
#define DEBUG_MODE

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttWrapper.h>
#include <PubSubClient.h>

const char* ssid     = "CMMC.47";
const char* pass = "guestnetwork";

MqttWrapper *mqtt;

byte state1=0;
void connect_wifi()
{
  WiFi.begin(ssid, pass);

  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    retries++;
    delay(500);
  }

  Serial.println("WIFI CONNECTED ");
}

void reconnect_wifi_if_link_down() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WIFI DISCONNECTED");
    connect_wifi();
  }
}

void callback(const MQTT::Publish& pub) {
  if (pub.payload_string() == "0") {
    Serial.print(" => ");
    Serial.println(pub.payload_string());
    digitalWrite(2, 1);
    state1=0;
  }
  else if (pub.payload_string() == "1") {
    Serial.print(" => ");
    Serial.println(pub.payload_string());
    digitalWrite(2, 0);
    state1=1;
  }
  else {
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
  }
}

void hook_before_publish(JsonObject** root) {
  JsonObject& data = (*(*root))["d"];

  data["myName"] = "TONG";
}

void setup() {
  Serial.begin(115200);
  pinMode(0, INPUT_PULLUP);
  pinMode(2, OUTPUT);
  delay(10);
  digitalWrite(2, HIGH);
  connect_wifi();

  mqtt = new MqttWrapper("128.199.104.122");
  mqtt->connect(callback);
  mqtt->set_prepare_data_hook(hook_before_publish, 5000);
}



void loop() {
  reconnect_wifi_if_link_down();
  mqtt->loop();

  // ตรวจจับการกด Switch
  if (digitalRead(0) == LOW) {
    // วนลูปจนกว่าจะเอาปล่อย Switch
    while (digitalRead(0) == LOW) {
      mqtt->loop();
      yield();
    }
    String status = " ";
    if(state1== 1)
     status = "0";
    if(state1== 0)
     status = "1";
   
    mqtt->sync_pub(status);
  }
}
