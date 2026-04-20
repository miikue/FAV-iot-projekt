#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
#endif

//mq-5
#define pinA A0
float ovzdusi = 0;

//dht11
#include "DHT.h"
#define pinD 4
#define pinDHT 2
#define typDHT22 DHT11
DHT mojeDHT(pinDHT, typDHT22);

#include "arduino_secrets.h"
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.0.102";
int        port     = 1883;
const char topic[]  = "venku/hod";

void setup() {
  Serial.begin(9600);
  mojeDHT.begin();
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }
  mqttClient.setId("venku");

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }
  mqttClient.subscribe("venku/ov");
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
  Serial.println("Zahrivani! cca 2min");
  delay(120000);
}

void loop() {
  mqttClient.poll();
  int messageSize = mqttClient.parseMessage();
  if (messageSize) {
    ovzdusi = analogRead(pinA) / 100;
    float tep = mojeDHT.readTemperature();
    float vlh = mojeDHT.readHumidity();
    String hodnoty = String(tep) + "  " + String(vlh) + "  " + ovzdusi;
    mqttClient.beginMessage(topic);
    mqttClient.print(hodnoty);
    mqttClient.endMessage();
    delay(100);
  }
}