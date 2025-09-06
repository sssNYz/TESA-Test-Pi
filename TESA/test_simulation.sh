#!/bin/bash

echo "=== Drone Detection Simulator Test Script ==="
echo

# Check if MQTT broker is running
echo "Checking MQTT broker status..."
if systemctl is-active --quiet mosquitto; then
    echo "✓ MQTT broker is running"
else
    echo "✗ MQTT broker is not running. Starting it..."
    sudo systemctl start mosquitto
    sleep 2
    if systemctl is-active --quiet mosquitto; then
        echo "✓ MQTT broker started successfully"
    else
        echo "✗ Failed to start MQTT broker"
        exit 1
    fi
fi

echo

# Build the project
echo "Building drone simulator..."
mkdir -p build
cd build

if cmake .. && make; then
    echo "✓ Build successful"
else
    echo "✗ Build failed"
    exit 1
fi

echo

# Test with different configurations
echo "Testing drone simulator..."

echo "1. Testing with default settings (5 detections):"
echo "   Command: ./bin/drone_simulator --count 5"
echo "   Press Ctrl+C to stop early"
echo
./bin/drone_simulator --count 5

echo
echo "2. Testing with custom host and topic:"
echo "   Command: ./bin/drone_simulator --host localhost --topic test/drone --count 3"
echo
./bin/drone_simulator --host localhost --topic test/drone --count 3

echo
echo "3. Testing with faster updates:"
echo "   Command: ./bin/drone_simulator --interval 50 --count 10"
echo
./bin/drone_simulator --interval 50 --count 10

echo
echo "=== Test completed ==="
echo "You can also run the simulator manually with:"
echo "  ./bin/drone_simulator [options]"
echo
echo "Available options:"
echo "  --host HOST     MQTT broker host (default: localhost)"
echo "  --port PORT     MQTT broker port (default: 1883)"
echo "  --topic TOPIC   MQTT topic (default: drone/detections)"
echo "  --interval MS   Update interval in milliseconds (default: 100)"
echo "  --count NUM     Number of detections to send (default: unlimited)"
echo "  --help          Show help message"
