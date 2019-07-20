#include "DHT.h"
#include <kpn_senml.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        

const char* mqtt_server = "MQTT_SERVER_IP"; 
const char* mqtt_user = "MQTT_USER";
const char* mqtt_pass= "MQTT_PASS";

String macid=WiFi.macAddress();
const char* conv_macid=macid.c_str();

WiFiClient espClient;
PubSubClient client(espClient);

SenMLPack doc(conv_macid);

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void reconnect() {
  // Loop until we're reconnected
  Serial.println("In reconnect...");
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("egm-lora.84:F3:EB:0C:8A:71", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    //WiFiManager
    WiFiManager wifiManager;
    wifiManager.autoConnect();
   
    //if you get here you have connected to the WiFi
    Serial.println("");
    Serial.println("WiFi connection Successful");
    Serial.print("The IP Address of ESP8266 Module is: ");
    Serial.print(WiFi.localIP());// Print the IP address
    client.setServer(mqtt_server, 1883);
    senMLSetLogger(&Serial);
    delay(1000);
    Serial.println("start");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  //WiFiManager wifiManager;           
  //wifiManager.setConfigPortalTimeout(180);
  //wifiManager.startConfigPortal("ESPtest", "password");
  // put your main code here, to run repeatedly:
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  Serial.println(t);
  Serial.println(h);
  SenMLFloatRecord rec_t(KPN_SENML_TEMPERATURE, SENML_UNIT_DEGREES_CELSIUS, t);
  SenMLFloatRecord rec_h(KPN_SENML_HUMIDITY, SENML_UNIT_RELATIVE_HUMIDITY, h);
  doc.add(&rec_t);
  doc.add(&rec_h);
  doc.toJson(&Serial);        //print to screen
  Serial.println();
  char buffer[150];    
  memset(buffer, 0, sizeof(buffer));        
  doc.toJson(buffer, sizeof(buffer));
  client.publish("egm-lora.84:F3:EB:0C:8A:71", buffer);
  delay(3000);
}
