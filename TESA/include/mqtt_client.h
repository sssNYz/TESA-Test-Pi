#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <mosquitto.h>

// MQTT client configuration
typedef struct {
    char* host;
    int port;
    char* topic;
    int keepalive;
    bool clean_session;
} mqtt_config_t;

// Initialize MQTT client
struct mosquitto* init_mqtt_client(const mqtt_config_t* config);

// Connect to MQTT broker
int connect_mqtt_broker(struct mosquitto* mosq, const mqtt_config_t* config);

// Publish message to MQTT broker
int publish_message(struct mosquitto* mosq, const char* topic, const char* message);

// Disconnect and cleanup MQTT client
void cleanup_mqtt_client(struct mosquitto* mosq);

// Default MQTT configuration
mqtt_config_t get_default_mqtt_config(void);

#endif // MQTT_CLIENT_H
