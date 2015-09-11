/*
  # disco ball
  (c) by Christoph (doebi) Döberl


*/


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>

// pins
const int led1  = 14;
const int led2 = 15;
const int motor = 5;

int led1State = LOW;
int led2State = LOW;
int motorState = LOW;

int isAlive = millis();
int mil = 0;

ESP8266WiFiMulti wifiMulti;

IPAddress MQTTserver(158, 255, 212, 248);
WiFiClient wclient;
PubSubClient client(wclient, MQTTserver);

void mqtt_callback(const MQTT::Publish& pub) {
  String topic = pub.topic();
  String msg = pub.payload_string();
  if(topic == "devlol/h19/mainroom/discoball/led1") {
    if (msg == "ON") {
      led1State = HIGH;
    }
    if (msg == "OFF") {
      led1State = LOW;
    }
    digitalWrite(led1, led1State);
  }
  if(topic == "devlol/h19/mainroom/discoball/led2") {
    if (msg == "ON") {
      led2State = HIGH;
    }
    if (msg == "OFF") {
      led2State = LOW;
    }
    digitalWrite(led2, led2State);
  }
  if(topic == "devlol/h19/mainroom/discoball/motor") {
    if (msg == "ON") {
      motorState = HIGH;
    }
    if (msg == "OFF") {
      motorState = LOW;
    }
    digitalWrite(motor, motorState);
  }
}

void setup() {
    Serial.begin(115200);
    delay(10);
	
    wifiMulti.addAP("/dev/lol", "4dprinter");
    wifiMulti.addAP("ESP-Spielwiese", "ovomaltine");
    
    if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println("Wifi connected");
    }
    
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(motor, OUTPUT);

    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
}

void loop() {
  // heartbeat
  mil = millis();
  if(mil - isAlive >= 30000) {
    client.publish("devlol/h19/mainroom/discoball/heartbeat", (String)mil);
    isAlive = mil;  
  }
  
  // wifi
  if(WiFi.status() != WL_CONNECTED) {
    if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println("Wifi connected");
    }
  }

    // mqtt
    if (client.connected()) {
      client.loop();
    } else {
      if (client.connect("discoball", "devlol/h19/mainroom/discoball/online", 0, true, "false")) {
        client.publish("devlol/h19/mainroom/discoball/online", "true", true);
        Serial.println("MQTT connected");
	      client.set_callback(mqtt_callback);
	      client.subscribe("devlol/h19/mainroom/discoball/#");
      }
    }
}
