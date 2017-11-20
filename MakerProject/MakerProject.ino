

//MQTT
#include <PubSubClient.h>
//WIFIMANAGER
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
//https://github.com/tzapu/WiFiManager

#define MQTT_AUTH false
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
#define LIGHT D2
const char* MQTT_CONTROL_TOPIC = "/bhmaker/luz/set";
const char* MQTT_STATE_TOPIC = "/bhmaker/luz";
//MQTT BROKERS GRATUITOS PARA TESTES https://github.com/mqtt/mqtt.github.io/wiki/public_brokers
const char* MQTT_SERVER = "10.1.41.236";
String const HOSTNAME = "bhMaker";
//INIT MQTT
WiFiClient wclient;
PubSubClient client(MQTT_SERVER,1883,wclient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LIGHT,OUTPUT);
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);
  
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect(HOSTNAME.c_str(),"xptoxpto")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  } 

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
 client.setCallback(callback);
}
//Chamada de recepção de mensagem 
void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i=0; i<length; i++) {
    payloadStr += (char)payload[i];
  }
  Serial.println(payloadStr);
  String topicStr = String(topic);
  if(topicStr.equals(MQTT_CONTROL_TOPIC)){
  if(payloadStr.equals("ON")){
      turnOn();
    }else if(payloadStr.equals("OFF")) {
      turnOff();
    }
  } 
} 

void turnOn(){
  digitalWrite(LIGHT,HIGH);
  client.publish(MQTT_STATE_TOPIC,"ON");  
}
void turnOff(){
  digitalWrite(LIGHT,LOW);
  client.publish(MQTT_STATE_TOPIC,"OFF");  
  
}
bool checkMqttConnection(){
  if (!client.connected()) {
    if (MQTT_AUTH ? client.connect(HOSTNAME.c_str(),MQTT_USERNAME, MQTT_PASSWORD) : client.connect(HOSTNAME.c_str())) {
      //SUBSCRIÇÃO DE TOPICOS
      Serial.println("CONNECTED");
      client.subscribe(MQTT_CONTROL_TOPIC);
    }
  }
  return client.connected();
}
void loop() {
if (WiFi.status() == WL_CONNECTED) {
    if (checkMqttConnection()){
      client.loop();
    }
  }
}
