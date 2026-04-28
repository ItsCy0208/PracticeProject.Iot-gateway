# iot-gateway

A beginner-friendly IoT gateway project in C++ using MQTT (Mosquitto) and CMake.

## Current Features

- Build and run a C++ gateway with CMake
- Connect to local MQTT broker (`mosquitto`)
- Subscribe to topic `iot/test`
- Parse JSON payload and validate fields (`device`, `temp`)
- Print structured logs for success and error cases

## Project Structure

```text
iot-gateway/
├─ CMakeLists.txt
├─ iot-gateway/
│  ├─ CMakeLists.txt
│  ├─ iot-gateway.cpp
│  └─ iot-gateway.h
└─ docs/
   ├─ noteday2.md
   ├─ noteday3.md
   ├─ noteday4.md
   └─ noteday5.md
```

## Environment

- Windows + WSL2 (Ubuntu 24.04)
- CMake 3.28+
- g++ 13+
- Mosquitto + mosquitto-clients

## Setup (WSL Ubuntu)

```bash
sudo apt update
sudo apt install -y build-essential cmake gdb git pkg-config
sudo apt install -y mosquitto mosquitto-clients libmosquitto-dev nlohmann-json3-dev
```

## Build and Run Gateway

```bash
cd /mnt/c/dev/iot-gateway/iot-gateway
cmake -S . -B build
cmake --build build -j
./build/iot-gateway/iot-gateway
```

Expected startup logs:

```text
[gateway] connected to broker
[gateway] listening on iot/test ...
```

## MQTT Local Test

Terminal A (run gateway):

```bash
cd /mnt/c/dev/iot-gateway/iot-gateway
./build/iot-gateway/iot-gateway
```

Terminal B (publish test messages):

```bash
mosquitto_pub -h localhost -t iot/test -m '{"device":"dev01","temp":25.1}'
mosquitto_pub -h localhost -t iot/test -m '{"device":"dev01"}'
mosquitto_pub -h localhost -t iot/test -m '{"device":'
```

Expected gateway output:

- Valid JSON: `[gateway] parsed device=dev01 temp=25.1`
- Missing field: `[gateway][error] missing/invalid field: temp`
- Invalid JSON: `[gateway][error] invalid json: ...`

## Troubleshooting

- `mosquitto.h: No such file or directory`

```bash
sudo apt install -y libmosquitto-dev
```

- MQTT no response

```bash
sudo service mosquitto status
```

Check topic spelling: publisher and subscriber must use the same topic (for example, `iot/test`).

## Next Steps

- Forward parsed telemetry to local HTTP mock service
- Add configurable topic/broker via config file
- Add basic unit tests for JSON validation
