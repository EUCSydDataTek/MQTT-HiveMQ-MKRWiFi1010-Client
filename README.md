# MQTT-HiveMQ-MKRWiFi1010-client

Opgave 8 - MKRWifi1010 sender Telemetry til HiveMQ

ArduinoMqttClient for HiveMQ Cloud

The circuit: MKRWiFi1010

This example connects to HiveMQ MQTT broker and subscribes to a single topic **led**,
it also publishes a message to another topic **telemetry** every 10 seconds.
When a message is received it prints the message to the Serial Monitor,
it uses the callback functionality of the library.

Use together with the project **MqttWebApiInfluxDB**

Test subscription on inTopic from MQTT.fx