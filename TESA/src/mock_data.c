#include "mock_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

// Initialize simulation state with realistic starting values
void init_simulation(simulation_state_t* state) {
    // Seed random number generator
    srand(time(NULL));
    
    // Initialize drone positions and properties
    for (int i = 0; i < MAX_DRONES; i++) {
        state->drone_x[i] = 0.0;  // Start at left edge
        state->drone_y[i] = 0.3 + (i * 0.4);  // Stagger vertically
        state->drone_direction[i] = 1.0;  // Start moving right
        state->drone_oscillation[i] = (double)i * M_PI / 2.0;  // Phase offset
        state->drone_size[i] = 0.08 + (rand() % 7) / 100.0;  // 0.08-0.15
        state->drone_confidence[i] = 0.75 + (rand() % 20) / 100.0;  // 0.75-0.95
    }
    
    state->frame_count = 0;
    state->miss_detection = false;
}

// Add realistic random noise to values
double add_noise(double value, double noise_factor) {
    double noise = ((double)rand() / RAND_MAX - 0.5) * noise_factor;
    return value + noise;
}

// Generate next detection record based on simulation state
void generate_detection_record(simulation_state_t* state, detection_record_t* record) {
    // Update timestamp
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    record->ts = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    
    // Always detect drones (removed missed detection logic for testing)
    state->miss_detection = false;
    
    record->detected = true;
    record->count = 0;
    
    // Update drone positions and generate detections
    for (int i = 0; i < MAX_DRONES; i++) {
        // Update x position (smooth movement)
        double speed = 0.008;  // Adjust for desired movement speed
        state->drone_x[i] += state->drone_direction[i] * speed;
        
        // Reverse direction at edges
        if (state->drone_x[i] >= 1.0) {
            state->drone_x[i] = 1.0;
            state->drone_direction[i] = -1.0;
        } else if (state->drone_x[i] <= 0.0) {
            state->drone_x[i] = 0.0;
            state->drone_direction[i] = 1.0;
        }
        
        // Update y oscillation (slight up/down movement)
        state->drone_oscillation[i] += 0.1;
        double base_y = 0.3 + (i * 0.4);
        double oscillation = sin(state->drone_oscillation[i]) * 0.05;
        state->drone_y[i] = base_y + oscillation;
        
        // Ensure y stays within bounds
        if (state->drone_y[i] < 0.1) state->drone_y[i] = 0.1;
        if (state->drone_y[i] > 0.9) state->drone_y[i] = 0.9;
        
        // Create detection object
        drone_object_t* obj = &record->objects[record->count];
        
        // Add noise to confidence
        obj->confidence = add_noise(state->drone_confidence[i], 0.02);
        if (obj->confidence < 0.6) obj->confidence = 0.6;
        if (obj->confidence > 0.95) obj->confidence = 0.95;
        
        // Center coordinates with noise
        obj->center[0] = add_noise(state->drone_x[i], 0.01);
        obj->center[1] = add_noise(state->drone_y[i], 0.01);
        
        // Ensure center stays within bounds
        if (obj->center[0] < 0.0) obj->center[0] = 0.0;
        if (obj->center[0] > 1.0) obj->center[0] = 1.0;
        if (obj->center[1] < 0.0) obj->center[1] = 0.0;
        if (obj->center[1] > 1.0) obj->center[1] = 1.0;
        
        // Bounding box with slight size variation
        double bbox_size = add_noise(state->drone_size[i], 0.01);
        if (bbox_size < 0.05) bbox_size = 0.05;
        if (bbox_size > 0.15) bbox_size = 0.15;
        
        obj->bbox[0] = obj->center[0] - bbox_size / 2.0;  // x
        obj->bbox[1] = obj->center[1] - bbox_size / 2.0;  // y
        obj->bbox[2] = bbox_size;  // width
        obj->bbox[3] = bbox_size;  // height
        
        // Ensure bbox stays within frame
        if (obj->bbox[0] < 0.0) obj->bbox[0] = 0.0;
        if (obj->bbox[1] < 0.0) obj->bbox[1] = 0.0;
        if (obj->bbox[0] + obj->bbox[2] > 1.0) obj->bbox[0] = 1.0 - obj->bbox[2];
        if (obj->bbox[1] + obj->bbox[3] > 1.0) obj->bbox[1] = 1.0 - obj->bbox[3];
        
        // Calculate area
        obj->area = obj->bbox[2] * obj->bbox[3];
        
        record->count++;
    }
    
    // Occasionally show 2 drones (20% chance when both are visible)
    if (record->count == 1 && rand() % 100 < 20) {
        // Add a second drone at a different position
        if (record->count < MAX_DRONES) {
            drone_object_t* obj = &record->objects[record->count];
            
            obj->confidence = add_noise(0.7 + (rand() % 25) / 100.0, 0.02);
            obj->center[0] = add_noise(0.2 + (rand() % 60) / 100.0, 0.01);
            obj->center[1] = add_noise(0.2 + (rand() % 60) / 100.0, 0.01);
            
            double bbox_size = add_noise(0.08 + (rand() % 7) / 100.0, 0.01);
            obj->bbox[0] = obj->center[0] - bbox_size / 2.0;
            obj->bbox[1] = obj->center[1] - bbox_size / 2.0;
            obj->bbox[2] = bbox_size;
            obj->bbox[3] = bbox_size;
            obj->area = bbox_size * bbox_size;
            
            record->count++;
        }
    }
    
    state->frame_count++;
}

// Convert detection record to JSON string
char* detection_to_json(const detection_record_t* record) {
    char* json = malloc(2048);  // Allocate buffer for JSON
    if (!json) return NULL;
    
    if (record->detected && record->count > 0) {
        snprintf(json, 2048,
            "{"
            "\"ts\":%lu,"
            "\"detected\":true,"
            "\"count\":%d,"
            "\"objects\":[",
            record->ts, record->count);
        
        for (int i = 0; i < record->count; i++) {
            char obj_json[512];
            snprintf(obj_json, 512,
                "%s{"
                "\"confidence\":%.3f,"
                "\"center\":[%.3f,%.3f],"
                "\"bbox\":[%.3f,%.3f,%.3f,%.3f],"
                "\"area\":%.4f"
                "}",
                (i > 0) ? "," : "",
                record->objects[i].confidence,
                record->objects[i].center[0], record->objects[i].center[1],
                record->objects[i].bbox[0], record->objects[i].bbox[1],
                record->objects[i].bbox[2], record->objects[i].bbox[3],
                record->objects[i].area);
            
            strcat(json, obj_json);
        }
        
        strcat(json, "]}");
    } else {
        snprintf(json, 2048,
            "{"
            "\"ts\":%lu,"
            "\"detected\":false,"
            "\"count\":0,"
            "\"objects\":[]"
            "}",
            record->ts);
    }
    
    return json;
}

// Free JSON string memory
void free_json_string(char* json_str) {
    if (json_str) {
        free(json_str);
    }
}
