#include "MqttManager.h"
#include <cstring>
#include <iostream>

MqttManager* MqttManager::instance = nullptr;


MqttManager::MqttManager(
        const char* broker, int port,
        const char* cmd_topic, const char* status_topic,IPStack &ipstack) :
        broker(broker), port(port),
        cmd_topic(cmd_topic), status_topic(status_topic),ipstack(ipstack), client(ipstack) {
    //IPStack ipstack(ssid, password);
    instance = this;
    connect();
}
void messageArrived(MQTT::MessageData &md) {
    MQTT::Message &message = md.message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload %s\n", (char *) message.payload);
}

void MqttManager::connect()
{
    /* Connect to WiFi
    //if (cyw43_arch_init()) return false;
    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password,
                                         CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        return false;
    }*/

    // Connect to MQTT broker
    int rc = ipstack.connect(broker, port);
    if (rc != 1) {
        std::cout << "rc from TCP connect is "<< rc << std::endl;
    }
    std::cout << "MQTT connecting" <<std::endl;

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char*)"garage_door_controller";
    rc = client.connect(data);
    if (rc != 0) {
        std::cout << "Connection Failed.rc from MQTT connect is "<< rc << std::endl;
    }else{
        std::cout << "MQTT connected." <<std::endl;
    }

    // Subscribe to command topic
    client.subscribe(cmd_topic, MQTT::QOS0, messageArrived);
    //   return true;//TODO change to comment instead return

}

void MqttManager::publish(std::string message) {
    if(!client.isConnected())
    {
        connect();
    }
    MQTT::Message msg;
    const char *topic = "garage1/door/status";
    msg.qos = MQTT::QOS0;
    msg.retained = false;
    msg.dup = false;
    msg.payload = (void*)message.c_str();
    msg.payloadlen = strlen(message.c_str()) + 1;
    client.publish(topic, msg);
}

void MqttManager::checkMessages() {
    client.yield(1);
}


void MqttManager::messageArrived(MQTT::MessageData& md) {
    MQTT::Message &message = md.message;
    char* payload = static_cast<char*>(message.payload);
    payload[message.payloadlen] = '\0'; // Null-terminate

    // Forward to application callback
    //  instance-> //TODO whatever function to handle message
    //   if (md.topicName == std::string(cmd_topic)) {
    // You'll need to implement a way to pass this to GarageDoor
}