#define SECRET_SSID "SibirienAP"
#define SECRET_PASS "Siberia51244"

// #define SECRET_SSID "ORBI30"
// #define SECRET_PASS "15711571"

// Broker URL findes i console.hivemq.cloud under Manage Cluster
#define BROKER "4611dbea64354d3f81615ee6651bc7aa.s1.eu.hivemq.cloud"
#define CLIENT_ID "MKRWiFi1010"
#define HIVEMQ_USERNAME "MKRWiFi1010#1"
#define HIVEMQ_PASSWORD "qfh3pdw-WKH1eud9xnv"

void connectWiFi();
void connectMQTT();
void publishMessage();
void onMqttMessage(int);
void myshutdown();