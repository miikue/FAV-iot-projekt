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

#include "DHT.h"
#define pinD 4
#define pinDHT 2
#define typDHT22 DHT22 
DHT mojeDHT(pinDHT, typDHT22);

//hall senzor
int hallSensorValue = 0;
int rain = 0;
unsigned int time;
int posledniHodnota = 5;
int hallHodnota = 0;
int hallvenek = 0;

#include "arduino_secrets.h"
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.0.102";
int        port     = 1883;
const char topic[]  = "venku/hod";

void setup() {
  //pinMode(6, INPUT);
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
}



void loop() {
  //time = millis();
  hallSensorValue = analogRead(A7);
  if (hallSensorValue < 500){
    hallHodnota = 1;
  }else{
    hallHodnota = 0;
  }

  if(posledniHodnota == 5){
    if(hallHodnota == 0){
      posledniHodnota = 0;
    }else{
      posledniHodnota = 1;
    }
  }

  if(posledniHodnota == 0 && hallHodnota != 0){
    rain++;
    posledniHodnota = 1;
  }
  if(posledniHodnota == 1 && hallHodnota != 1){
    rain++;
    posledniHodnota = 0;
  }


  if (millis() %60000 <= 1){
    hallvenek = (hallvenek + (rain*138))/2;
    Serial.print("Obnova pocitani deste ");
    Serial.println(hallvenek);
    rain = 0;
  }
  //mqttClient.poll();
  int messageSize = mqttClient.parseMessage();

  if (messageSize) {
    float valcteni = analogRead(A6);
    float Vout = (valcteni * 3.30) / 1024.00;
    float napeti = (Vout / (7500.0 / (7500+30000)));
    float tep = mojeDHT.readTemperature();
    float vlh = mojeDHT.readHumidity();
    float hallvn = 0.0 + hallvenek;
    String hodnoty = String(tep) + "  " + String(vlh) +  "  " + napeti + "  " + hallvn;
    Serial.println("Posilam hodnoty: <" + hodnoty + ">");
    mqttClient.beginMessage(topic);
    mqttClient.print(hodnoty);
    mqttClient.endMessage();
  }
}

