

#ifndef PICO_MODBUS_MQTTMANAGER_H
#define PICO_MODBUS_MQTTMANAGER_H

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <MQTTClient.h>
#include "IPStack.h"
#include "Countdown.h"
#include <functional>

class MqttManager {
public:
    MqttManager(const char* broker, int port,const char* cmd_topic, const char* status_topic,IPStack &ipstack);

    void connect();
    void publish(std::string message);
    //void checkMessages(void (*callback)(const char*));
    void checkMessages();
    bool isConnected();
    void setCommandCallback(std::function<void(const char*)> cb);

private:
    static MqttManager *instance;
    //const char* ssid;
    //const char* password;
    const char* broker;
    int port;
    const char* cmd_topic;
    const char* status_topic;
    IPStack &ipstack;
    MQTT::Client<IPStack, Countdown> client;
    void setupWiFi();
    static void messageArrived(MQTT::MessageData& md);
};


#endif //PICO_MODBUS_MQTTMANAGER_H