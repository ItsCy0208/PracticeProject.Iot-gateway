#include "iot-gateway.h"

#include <chrono>
#include <iostream>
#include <mosquitto.h>
#include <nlohmann/json.hpp>
#include <thread>

using json = nlohmann::json;

bool on_message_err_temp(const std::string& payload, std::string& device, double& temp);

void on_connect(struct mosquitto* mosq, void* obj, int rc) {
    if (rc == 0) {
        std::cout << "[gateway] connected to broker" << std::endl;
        mosquitto_subscribe(mosq, nullptr, "iot/test", 0);
        std::cout << "[gateway] listening on iot/test ..." << std::endl;
    } else {
        std::cout << "[gateway] connect failed, rc=" << rc << std::endl;
    }
}

void on_message(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg) {
    const std::string payload = static_cast<const char*>(msg->payload);

    std::cout << "[gateway] topic=" << msg->topic
              << " payload=" << payload << std::endl;

              std::string device = "";
              double temp = 0.0;
              bool result = false;
              result = on_message_err_temp(payload,device,temp);
              if(result==false){
                return;
              }
              std::cout << "[gateway] parsed device=" << device << " temp=" << temp << std::endl;
}

bool on_message_err_temp(const std::string& payload,std::string& device,double& temp){

    try {
        const json data = json::parse(payload);
        if (!data.contains("device") || !data["device"].is_string()) {
            std::cout << "[gateway][error] missing/invalid field: device" << std::endl;
            return false;
        }
        if (!data.contains("temp") || !data["temp"].is_number()) {
            std::cout << "[gateway][error] missing/invalid field: temp" << std::endl;
            return false;
        }
        device = data["device"].get<std::string>();
        temp = data["temp"].get<double>();
    } catch (const std::exception& e) {
        std::cout << "[gateway][error] invalid json: " << e.what() << std::endl;
        return false;
    }
    return true;
}



/*bool on_message_err_temp(const std::string& payload){
    try {
        const json data = json::parse(payload);

        if (!data.contains("device") || !data["device"].is_string()) {
            std::cout << "[gateway][error] missing/invalid field: device" << std::endl;
            return false;
        }
        if (!data.contains("temp") || !data["temp"].is_number()) {
            std::cout << "[gateway][error] missing/invalid field: temp" << std::endl;
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cout << "[gateway][error] invalid json: " << e.what() << std::endl;
        return false;
    }
}*/

int main() {
    mosquitto_lib_init();

    mosquitto* client = mosquitto_new("gateway-client", true, nullptr);
    if (!client) {
        std::cerr << "failed to create mosquitto client" << std::endl;
        return 1;
    }

    mosquitto_connect_callback_set(client, on_connect);
    mosquitto_message_callback_set(client, on_message);

    if (mosquitto_connect(client, "localhost", 1883, 60) != MOSQ_ERR_SUCCESS) {
        std::cerr << "failed to connect broker" << std::endl;
        mosquitto_destroy(client);
        mosquitto_lib_cleanup();
        return 1;
    }

    mosquitto_loop_start(client);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    mosquitto_loop_stop(client, true);
    mosquitto_destroy(client);
    mosquitto_lib_cleanup();
    return 0;
}