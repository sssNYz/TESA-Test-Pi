#include "mqtt_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Initialize MQTT client
struct mosquitto* init_mqtt_client(const mqtt_config_t* config) {
    // Initialize mosquitto library
    mosquitto_lib_init();
    
    // Create mosquitto instance
    struct mosquitto* mosq = mosquitto_new("drone_simulator", true, NULL);
    if (!mosq) {
        fprintf(stderr, "Error: Cannot create mosquitto instance\n");
        return NULL;
    }
    
    return mosq;
}

// Connect to MQTT broker
int connect_mqtt_broker(struct mosquitto* mosq, const mqtt_config_t* config) {
    if (!mosq || !config) {
        return MOSQ_ERR_INVAL;
    }
    
    // Connect to broker
    int result = mosquitto_connect(mosq, config->host, config->port, config->keepalive);
    if (result != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Cannot connect to MQTT broker at %s:%d - %s\n", 
                config->host, config->port, mosquitto_strerror(result));
        return result;
    }
    
    printf("Connected to MQTT broker at %s:%d\n", config->host, config->port);
    return MOSQ_ERR_SUCCESS;
}

// Publish message to MQTT broker
int publish_message(struct mosquitto* mosq, const char* topic, const char* message) {
    if (!mosq || !topic || !message) {
        return MOSQ_ERR_INVAL;
    }
    
    // Publish message
    int result = mosquitto_publish(mosq, NULL, topic, strlen(message), message, 0, false);
    if (result != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error publishing message: %s\n", mosquitto_strerror(result));
        return result;
    }
    
    return MOSQ_ERR_SUCCESS;
}

// Disconnect and cleanup MQTT client
void cleanup_mqtt_client(struct mosquitto* mosq) {
    if (mosq) {
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
    }
    mosquitto_lib_cleanup();
}

// Default MQTT configuration
mqtt_config_t get_default_mqtt_config(void) {
    mqtt_config_t config;
    config.host = "localhost";
    config.port = 1883;
    config.topic = "drone/detections";
    config.keepalive = 60;
    config.clean_session = true;
    return config;
}
