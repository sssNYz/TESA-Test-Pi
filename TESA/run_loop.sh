#!/bin/bash

echo "Starting drone simulator loop..."

while true; do
    echo "Starting drone simulator at $(date)"
    ./build/bin/drone_simulator --interval 100
    
    echo "Drone simulator stopped at $(date)"
    echo "Restarting in 5 seconds..."
    sleep 5
done
