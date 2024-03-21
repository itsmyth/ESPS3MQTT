#include <Arduino.h> 
#include <WiFi.h>
#include <WiFiMulti.h>
#include "arduino_secrets.h"
#include <PubSubClient.h> //by Nick O'Leary


WiFiMulti wiFiMulti;
WiFiClient wiFiClient;
PubSubClient mqttClient(wiFiClient);
const uint32_t connectTimeoutMs = 10000;

//Following is from MQTT sender simple example


const char broker[] = "your.broker.url";
int        port     = 1883;
const char topic[]  = "real_unique_topic";

const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)



// WiFi connect timeout per AP. Increase when connecting takes longer.

void setup(){
  Serial.begin(115200);
  delay(10);
  WiFi.mode(WIFI_STA);
  
  // Add list of wifi networks
  wiFiMulti.addAP(ssid, pass);
  //wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  //wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
      Serial.println("no networks found");
  } 
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }

  // Connect to Wi-Fi using wifiMulti (connects to the SSID with strongest connection)
  Serial.println("Connecting Wifi...");
  while(wiFiMulti.run()!=WL_CONNECTED){
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connecting to broker...");
  mqttClient.setServer(broker,1883);
  if (!mqttClient.connect("ESP32")){
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.state());

    while (1);

  }  
  else{
    Serial.println("Connected to broker");
  }

}
void reconnect() {
  // Loop until we're reconnected
  while (wiFiMulti.run()==WL_CONNECTED && !mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("ESP32")) {
      Serial.println("connected");}
    else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop(){
  unsigned long currentMillis = millis();

  //if the connection to the stongest hotstop is lost, it will connect to the next network on the list
  if (currentMillis - previousMillis >=interval){
    if (wiFiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
      Serial.print("WiFi connected: ");
      Serial.print(WiFi.SSID());
      Serial.print(" ");
      Serial.println(WiFi.RSSI());
    }
    else {
      Serial.println("WiFi not connected!");
    }
    previousMillis = currentMillis;
  }

  reconnect();


  mqttClient.publish(topic, "Hello");

  delay(1000);

}