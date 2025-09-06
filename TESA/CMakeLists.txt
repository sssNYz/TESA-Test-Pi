cmake_minimum_required(VERSION 3.10)
project(DroneSimulatorMQTT)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(MOSQUITTO REQUIRED libmosquitto)

# Include directories
include_directories(include)
include_directories(${MOSQUITTO_INCLUDE_DIRS})

# Add executable
add_executable(drone_simulator
    src/main.c
    src/mock_data.c
    src/mqtt_client.c
)

# Link libraries
target_link_libraries(drone_simulator
    ${MOSQUITTO_LIBRARIES}
    m  # math library for random functions
)

# Compiler flags
target_compile_options(drone_simulator PRIVATE ${MOSQUITTO_CFLAGS_OTHER})

# Set output directory
set_target_properties(drone_simulator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Print configuration info
message(STATUS "Mosquitto include dirs: ${MOSQUITTO_INCLUDE_DIRS}")
message(STATUS "Mosquitto libraries: ${MOSQUITTO_LIBRARIES}")
message(STATUS "Mosquitto cflags: ${MOSQUITTO_CFLAGS_OTHER}")
