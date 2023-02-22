#define SECRET_SSID "<SSID>"
#define SECRET_PASS "<Password>"

// Broker URL findes i console.hivemq.cloud under Manage Cluster
#define BROKER "<URL for Cluster>"
#define CLIENT_ID "MKRWiFi1010"
#define HIVEMQ_USERNAME "<Credentials Username>"
#define HIVEMQ_PASSWORD "<Credentials Password>"

void onMqttMessage(int);
void connect();