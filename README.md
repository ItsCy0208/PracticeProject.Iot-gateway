# iot-gateway
A beginner-friendly IoT gateway project in C++ using MQTT (Mosquitto) and CMake.
## Current Features
- Build and run C++ gateway with CMake
- Connect to local MQTT broker (`mosquitto`)
- Subscribe to topic `iot/test`
- Print received payload in terminal
## Project Structure
```text
iot-gateway/
├─ CMakeLists.txt
├─ iot-gateway/
│  ├─ CMakeLists.txt
│  ├─ iot-gateway.cpp
│  └─ iot-gateway.h
└─ docs/
   └─ noteday2.md
Environment
Windows + WSL2 (Ubuntu 24.04)
CMake 3.28+
g++ 13+
Mosquitto + mosquitto-clients
Setup (WSL Ubuntu)
sudo apt update
sudo apt install -y build-essential cmake gdb git pkg-config
sudo apt install -y mosquitto mosquitto-clients libmosquitto-dev
Build and Run Gateway
cd /mnt/c/dev/iot-gateway/iot-gateway
cmake -S . -B build
cmake --build build -j
./build/iot-gateway/iot-gateway
Expected output:

[gateway] connected to broker
[gateway] listening on iot/test ...
MQTT Local Test
Open terminal A (subscribe):

mosquitto_sub -h localhost -t "iot/test" -v
Open terminal B (publish):

mosquitto_pub -h localhost -t iot/test -m '{"device":"dev01","temp":25.1}'
Subscriber should receive:

iot/test {"device":"dev01","temp":25.1}
Gateway terminal should print topic and payload.

Troubleshooting
mosquitto.h: No such file or directory
Install dev package:

sudo apt install -y libmosquitto-dev
MQTT no response
Check topic spelling and broker status:

sudo service mosquitto stat

Next Steps
Parse JSON payload (device, temp)
Validate required fields and error handling
Add structured logging and unit tests
