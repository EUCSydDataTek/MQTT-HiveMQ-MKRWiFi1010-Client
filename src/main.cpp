/* Opgave 6.5 - MKRWifi1010 sender Telemetry til HiveMQ
  ArduinoMqttClient for HiveMQ Cloud
  The circuit: MKRWiFi1010
  This example connects to HiveMQ MQTT broker and subscribes to a single topic (inTopic),
  it also publishes a message to another topic (outTopic) every 10 seconds.
  When a message is received it prints the message to the Serial Monitor,
  it uses the callback functionality of the library.
  Test subscription on inTopic from MQTT.fx
*/
#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <DHT.h>
#include "secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
char client_id[] = CLIENT_ID;
char hivemq_username[] = HIVEMQ_USERNAME;
char hivemq_password[] = HIVEMQ_PASSWORD;

const char broker[] = BROKER;
int port = 8883;

const char inTopic[] = "arduino/in";
const char outTopic[] = "arduino/out";

const long interval = 10000;
unsigned long previousMillis = 0;
int count = 0;

// Configuration of DHT 11
#define DHTPIN 7      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11

WiFiSSLClient wifiClient;
MqttClient mqttClient(wifiClient);
DHT dht(DHTPIN, DHTTYPE);

/////////////////////////////////// SETUP ///////////////////////////////////////
void setup()
{
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {;}

  dht.begin();
  connect();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  mqttClient.subscribe(inTopic);
  Serial.print("Subscribing to topic: ");
  Serial.println(inTopic);
  Serial.println();

  Serial.println();
  Serial.print("Waiting for messages on topic: ");
  Serial.println(inTopic);
  Serial.println();
}

//////////////////////////////// LOOP /////////////////////////////////////////////
void loop()
{
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    previousMillis = currentMillis;
    publishValues();
  }
}

void onMqttMessage(int messageSize)
{
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available())
  {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();
  Serial.println();
}

void connect()
{
  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    Serial.print(".");
    delay(5000);
  }
  Serial.println("You're connected to the network");
  Serial.println();

  // Each client must have a unique client ID
  mqttClient.setId(client_id);

  mqttClient.setUsernamePassword(hivemq_username, hivemq_password);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port))
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1)
      ;
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void publishValues()
{
  int hum = (int)dht.readHumidity();     // Read humidity
  int temp = (int)dht.readTemperature(); // Read temperature as Celsius (the default)

  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(temp))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  count++;
  String payload = "#" + (String)count + " Temp = " + temp + " Hum = " + hum;
  Serial.println(payload);

  mqttClient.beginMessage(outTopic);
  mqttClient.print(payload);
  mqttClient.endMessage();
}