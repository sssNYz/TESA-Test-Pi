#ifndef DRONE_TYPES_H
#define DRONE_TYPES_H

#include <stdbool.h>
#include <stdint.h>

// Maximum number of drones to simulate
#define MAX_DRONES 2

// Drone detection object structure
typedef struct {
    double confidence;        // 0.6-0.95 with slight variation
    double center[2];        // [x, y] normalized coordinates (0-1)
    double bbox[4];          // [x, y, w, h] normalized bounding box
    double area;             // computed as w * h
} drone_object_t;

// Main detection record structure
typedef struct {
    uint64_t ts;             // timestamp in milliseconds
    bool detected;           // whether any drone is detected
    int count;               // number of drones detected
    drone_object_t objects[MAX_DRONES];  // array of detected objects
} detection_record_t;

// Simulation state structure
typedef struct {
    double drone_x[MAX_DRONES];      // x position (0-1)
    double drone_y[MAX_DRONES];      // y position (0-1) 
    double drone_direction[MAX_DRONES]; // 1 for right, -1 for left
    double drone_oscillation[MAX_DRONES]; // oscillation phase
    double drone_size[MAX_DRONES];   // bounding box size
    double drone_confidence[MAX_DRONES]; // base confidence
    uint64_t frame_count;            // current frame number
    bool miss_detection;             // flag for occasional missed detections
} simulation_state_t;

#endif // DRONE_TYPES_H
