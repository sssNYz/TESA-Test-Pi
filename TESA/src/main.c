#include "mock_data.h"
#include "mqtt_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

// Global variables for cleanup
static struct mosquitto* g_mosq = NULL;
static int g_running = 1;

// Signal handler for graceful shutdown
void signal_handler(int sig) {
    printf("\nReceived signal %d, shutting down gracefully...\n", sig);
    g_running = 0;
}

// Print usage information
void print_usage(const char* program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  -h, --host HOST     MQTT broker host (default: localhost)\n");
    printf("  -p, --port PORT     MQTT broker port (default: 1883)\n");
    printf("  -t, --topic TOPIC   MQTT topic (default: drone/detections)\n");
    printf("  -i, --interval MS   Update interval in milliseconds (default: 100)\n");
    printf("  -c, --count NUM     Number of detections to send (default: unlimited)\n");
    printf("  --help              Show this help message\n");
}

int main(int argc, char* argv[]) {
    // Default configuration
    mqtt_config_t mqtt_config = get_default_mqtt_config();
    int update_interval_ms = 100;  // 10 FPS
    int max_detections = -1;  // Unlimited
    int detection_count = 0;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--host") == 0) {
            if (i + 1 < argc) {
                mqtt_config.host = argv[++i];
            } else {
                fprintf(stderr, "Error: --host requires a value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
            if (i + 1 < argc) {
                mqtt_config.port = atoi(argv[++i]);
            } else {
                fprintf(stderr, "Error: --port requires a value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--topic") == 0) {
            if (i + 1 < argc) {
                mqtt_config.topic = argv[++i];
            } else {
                fprintf(stderr, "Error: --topic requires a value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interval") == 0) {
            if (i + 1 < argc) {
                update_interval_ms = atoi(argv[++i]);
            } else {
                fprintf(stderr, "Error: --interval requires a value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--count") == 0) {
            if (i + 1 < argc) {
                max_detections = atoi(argv[++i]);
            } else {
                fprintf(stderr, "Error: --count requires a value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("=== Drone Detection Simulator ===\n");
    printf("MQTT Broker: %s:%d\n", mqtt_config.host, mqtt_config.port);
    printf("Topic: %s\n", mqtt_config.topic);
    printf("Update Interval: %d ms\n", update_interval_ms);
    if (max_detections > 0) {
        printf("Max Detections: %d\n", max_detections);
    } else {
        printf("Max Detections: Unlimited\n");
    }
    printf("Press Ctrl+C to stop\n\n");
    
    // Initialize simulation state
    simulation_state_t sim_state;
    init_simulation(&sim_state);
    
    // Initialize MQTT client
    g_mosq = init_mqtt_client(&mqtt_config);
    if (!g_mosq) {
        fprintf(stderr, "Failed to initialize MQTT client\n");
        return 1;
    }
    
    // Connect to MQTT broker
    if (connect_mqtt_broker(g_mosq, &mqtt_config) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Failed to connect to MQTT broker\n");
        cleanup_mqtt_client(g_mosq);
        return 1;
    }
    
    // Give MQTT client time to establish connection
    sleep(1);
    
    printf("Starting drone simulation...\n");
    
    // Main simulation loop
    while (g_running && (max_detections < 0 || detection_count < max_detections)) {
        // Generate detection record
        detection_record_t record;
        generate_detection_record(&sim_state, &record);
        
        // Convert to JSON
        char* json_message = detection_to_json(&record);
        if (!json_message) {
            fprintf(stderr, "Failed to generate JSON message\n");
            continue;
        }
        
        // Publish to MQTT
        if (publish_message(g_mosq, mqtt_config.topic, json_message) == MOSQ_ERR_SUCCESS) {
            detection_count++;
            printf("Sent detection #%d: %s\n", detection_count, 
                   record.detected ? "DRONE DETECTED" : "NO DETECTION");
        } else {
            fprintf(stderr, "Failed to publish message\n");
        }
        
        // Free JSON string
        free_json_string(json_message);
        
        // Wait for next update
        usleep(update_interval_ms * 1000);  // Convert ms to microseconds
    }
    
    printf("\nSimulation completed. Sent %d detections.\n", detection_count);
    
    // Cleanup
    cleanup_mqtt_client(g_mosq);
    
    printf("Drone simulator shutdown complete.\n");
    return 0;
}
