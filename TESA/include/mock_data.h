#ifndef MOCK_DATA_H
#define MOCK_DATA_H

#include "drone_types.h"

// Initialize simulation state
void init_simulation(simulation_state_t* state);

// Generate next detection record
void generate_detection_record(simulation_state_t* state, detection_record_t* record);

// Add random noise to make data more realistic
double add_noise(double value, double noise_factor);

// Generate JSON string from detection record
char* detection_to_json(const detection_record_t* record);

// Free JSON string memory
void free_json_string(char* json_str);

#endif // MOCK_DATA_H
