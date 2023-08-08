/* Opgave 8 - MKRWifi1010 sender Telemetry til HiveMQ
  ArduinoMqttClient for HiveMQ Cloud
  The circuit: MKRWiFi1010
  This example connects to HiveMQ MQTT broker and subscribes to a single topic (led),
  it also publishes a message to another topic (telemetry) every 60 seconds.
  When a message is received it prints the message to the Serial Monitor,
  it uses the callback functionality of the library.

  Use together with the project MqttWebApiInfluxDB
*/
#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <utility/wifi_drv.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include "secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
char client_id[] = CLIENT_ID;
char hivemq_username[] = HIVEMQ_USERNAME;
char hivemq_password[] = HIVEMQ_PASSWORD;

const char broker[] = BROKER;
int port = 8883;

const char inTopic[] = "led";
const char outTopic[] = "telemetry";

int        servoport = 10;

unsigned long ledLastMillis = 0;
unsigned long lastMillis = 0;
const long intervalMillis = 60000;
int count = 0;

// Configuration of DHT 11
#define DHTPIN 7      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 11/22

WiFiSSLClient wifiClient;
MqttClient mqttClient(wifiClient);
DHT dht(DHTPIN, DHTTYPE);
Servo myservo;

/////////////////////////////////// SETUP ///////////////////////////////////////
void setup()
{
  delay(5000);  

  pinMode(LED_BUILTIN, OUTPUT);
  // RGB LEDS on board
  WiFiDrv::pinMode(25, OUTPUT);
  WiFiDrv::pinMode(26, OUTPUT);
  WiFiDrv::pinMode(27, OUTPUT);

  Serial.begin(9600);

  dht.begin();
  myservo.attach(servoport);
}

//////////////////////////////// LOOP /////////////////////////////////////////////
void loop()
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    WiFiDrv::digitalWrite(26, HIGH);   //Red LED on
    connectWiFi();
    WiFiDrv::digitalWrite(26, LOW);    //Red LED off
  }

  if (!mqttClient.connected()) 
  {
     WiFiDrv::digitalWrite(27, HIGH);   //Blue LED on
    connectMQTT();
    WiFiDrv::digitalWrite(27, LOW);     //Blue LED off
  }
  
  // poll for new MQTT messages and send keep alives
  mqttClient.poll();

  // publish a message roughly every 60 seconds.
  if (millis() - lastMillis > intervalMillis) 
  {
    lastMillis = millis();

    publishMessage();
  }

  // Green LED heartbeet 50ms blink every 3 seconds
  if (millis() - ledLastMillis > 3000) 
  { 
    WiFiDrv::digitalWrite(25, HIGH);  //Green LED on
  }
  if (millis() - ledLastMillis > 3050) 
  { 
    WiFiDrv::digitalWrite(25, LOW);  //Green LED off
    ledLastMillis = millis();
  }
}

/////////////////////////// HELPER FUNCTIONS ///////////////////////////////////////
void connectWiFi() 
{
  Serial.print("Attempting to connect to SSID: ");Serial.print(ssid);Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) 
  {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println("\nYou're connected to the network\n");
}

void connectMQTT() 
{
  mqttClient.setId(client_id);
  mqttClient.setUsernamePassword(hivemq_username, hivemq_password);

  Serial.print("Attempting to MQTT broker: ");Serial.print(broker);Serial.println(" ");

  if (!mqttClient.connect(broker, port))
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1)
      ;
  }

  Serial.println("You're connected to the MQTT broker!\n");

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  mqttClient.subscribe(inTopic);
  Serial.print("Subscribing to topic: ");Serial.println(inTopic);

  Serial.print("Waiting for messages on topic: ");Serial.println(inTopic);Serial.println();
}

void publishMessage()
{
  int hum = (int)dht.readHumidity();     // Read humidity
  int temp = (int)dht.readTemperature(); // Read temperature as Celsius (the default)

  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(temp))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  String payload;
  StaticJsonDocument<256> doc;
  doc["temperature"] = temp;
  doc["humidity"] = hum;
  serializeJson(doc, payload);

  Serial.println(payload);

  mqttClient.beginMessage(outTopic);
  mqttClient.print(payload);
  mqttClient.endMessage();
}

void onMqttMessage(int messageSize)
{
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

 String msg = mqttClient.readString();
 Serial.println(msg);

 if (msg == "on")
  {
    Serial.println("ON");
    digitalWrite(LED_BUILTIN, HIGH);
    myservo.write(0);
  }
  else
  {
    Serial.println("OFF");
    digitalWrite(LED_BUILTIN, LOW);
    myservo.write(180);
  }
  Serial.println();
 }