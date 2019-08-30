#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <string>

#define mqtt_server "192.168.1.1"
#define mqtt_port 1883
#define mqtt_user "mqtt_user"
#define mqtt_pass "mqtt_pass"
#define power_topic "sensor/power"

#define LED D4
#define RECEIVER A0
#define MIN_IR_VALUE 10
#define READ_AMOUNT 5
#define READING_COOLDOWN 20
#define CLIENT_ID "POWERSENSOR"

int reads = 0;
volatile long currentTime = 0;
long startPulse = 0;
long lastPulse = 0;

WiFiClient espClient;
PubSubClient client(espClient);

boolean reconnect() {  
  while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected to MQTT");
      return true;           
    } else {      
      delay(5000);
    }
  }
  return false;
}

boolean publish(const char topic[], const char payload[]) {  
    if (!client.connected() && !reconnect()){
        return false;
    }    
    return client.publish(topic, payload);
}

void configModeCallback(WiFiManager *myWiFiManager){
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup() {
    Serial.begin(9600);

    WiFiManager wifiManager;
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.autoConnect("Energy measure device AP");
    
    pinMode(LED, OUTPUT);    
    pinMode(RECEIVER, INPUT);
    digitalWrite(LED, LOW);
    client.setServer(mqtt_server, mqtt_port);    
    reconnect();
}

long getCurrentMeasure(){
    long elapsed = currentTime - startPulse;
    long factor = 3600000 / elapsed;
    return factor * (READ_AMOUNT -1);
}

void reset(){
    reads = 0;
    startPulse = 0;    
}

void keepAlive(){
    if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void update(){    
    long value = getCurrentMeasure();
    Serial.print("Current value: ");
    Serial.println(value);
    
    Serial.print("MQTT state ");
    Serial.println(client.state());

    if (client.publish(power_topic, String(value).c_str())){
        Serial.print("Published value ");
        Serial.println(String(value).c_str());
    }else{
        Serial.print("Publish failed");
    }
    
}

void onDiodePulse(){
    lastPulse = currentTime;
    digitalWrite(LED, HIGH);
    reads++;    
    if(reads == READ_AMOUNT){
        update();
        reset();
        return;
    }
    
    if(startPulse == 0){
        startPulse = currentTime;        
    }
}

boolean canCheck(){
    return (currentTime - lastPulse) > READING_COOLDOWN;
}

boolean checkDiode(){
    int sensorValue = analogRead(RECEIVER);    
    if(sensorValue > MIN_IR_VALUE){
        lastPulse = currentTime;
        return true;
    }
    return false;
}

void loop() {
    currentTime = millis();
    keepAlive();    
   
    if(currentTime - lastPulse > 100){
        digitalWrite(LED, LOW);
    }
    
    if(canCheck() && checkDiode()){
        onDiodePulse();
    }
}