# Drone Detection Simulator

A C program that simulates drone detection data and sends it to an MQTT broker for testing web applications.

## Features

- **Realistic Drone Movement**: Simulates 1-2 drones moving left-to-right across the camera frame
- **Natural Oscillation**: Drones oscillate up and down for realistic motion
- **MQTT Integration**: Sends JSON data to MQTT broker on port 1883
- **Configurable**: Command-line options for host, port, topic, and update rate
- **Realistic Data**: Includes random noise and confidence variations

## Project Structure

```
TESA/
├── src/
│   ├── main.c          # Main program and command-line parsing
│   ├── mock_data.c     # Drone simulation and data generation
│   └── mqtt_client.c   # MQTT client functionality
├── include/
│   ├── drone_types.h   # Data structures and type definitions
│   ├── mock_data.h     # Mock data generation functions
│   └── mqtt_client.h   # MQTT client functions
├── CMakeLists.txt      # Build configuration
├── README.md           # This file
└── test_simulation.sh  # Test script
```

## Requirements

- CMake 3.10 or higher
- libmosquitto-dev (MQTT client library)
- GCC compiler
- MQTT broker (mosquitto)

## Installation

1. **Install required packages:**
```bash
sudo apt update
sudo apt install cmake libmosquitto-dev build-essential
```

2. **Build the project:**
```bash
cd TESA
mkdir build && cd build
cmake ..
make
```

3. **Or use the test script:**
```bash
./test_simulation.sh
```

## Usage

### Basic Usage
```bash
# Run with default settings (localhost:1883, topic: drone/detections)
./bin/drone_simulator

# Send 10 detections and stop
./bin/drone_simulator --count 10

# Use custom MQTT broker
./bin/drone_simulator --host 192.168.1.100 --port 1883
```

### Command Line Options
- `-h, --host HOST`: MQTT broker host (default: localhost)
- `-p, --port PORT`: MQTT broker port (default: 1883)
- `-t, --topic TOPIC`: MQTT topic (default: drone/detections)
- `-i, --interval MS`: Update interval in milliseconds (default: 100)
- `-c, --count NUM`: Number of detections to send (default: unlimited)
- `--help`: Show help message

### Examples
```bash
# Fast updates (20 FPS)
./bin/drone_simulator --interval 50 --count 20

# Custom topic and broker
./bin/drone_simulator --host 192.168.1.102 --topic myapp/drones --count 5

# Slow updates for testing
./bin/drone_simulator --interval 1000 --count 10
```

## JSON Data Format

Each MQTT message contains a JSON object with:

```json
{
  "ts": 1703123456789,           // timestamp in milliseconds
  "detected": true,              // whether any drone is detected
  "count": 1,                    // number of drones detected
  "objects": [                   // array of detected objects
    {
      "confidence": 0.87,        // detection confidence (0.6-0.95)
      "center": [0.45, 0.52],    // [x, y] normalized coordinates (0-1)
      "bbox": [0.41, 0.48, 0.08, 0.09],  // [x, y, w, h] bounding box
      "area": 0.0072             // computed area (w × h)
    }
  ]
}
```

## Drone Movement Simulation

- **Horizontal Movement**: Drones move smoothly from left to right, then back
- **Vertical Oscillation**: Small up/down movement for natural motion
- **Confidence Variation**: Slight random changes to confidence values
- **Size Variation**: Bounding box size changes subtly
- **Multiple Drones**: Occasionally shows 2 drones simultaneously

## Testing

Run the test script to verify everything works:
```bash
./test_simulation.sh
```

This will:
1. Check if MQTT broker is running
2. Build the project
3. Run several test scenarios
4. Show usage examples

## Troubleshooting

### MQTT Connection Issues
- Make sure mosquitto broker is running: `systemctl status mosquitto`
- Check firewall settings if using remote broker
- Verify host and port are correct

### Build Issues
- Install required packages: `sudo apt install libmosquitto-dev cmake build-essential`
- Check CMake version: `cmake --version` (needs 3.10+)

### Runtime Issues
- Check MQTT broker logs: `journalctl -u mosquitto`
- Use `--help` to see all available options
- Press Ctrl+C to stop the program

## Development

### Adding New Features
1. Add new functions to appropriate `.c` files
2. Add declarations to corresponding `.h` files
3. Update CMakeLists.txt if needed
4. Test with the test script

### Code Structure
- **main.c**: Program entry point, argument parsing, main loop
- **mock_data.c**: Drone simulation logic, JSON generation
- **mqtt_client.c**: MQTT connection and publishing
- **drone_types.h**: All data structures and constants

## License

This project is for educational and testing purposes.
