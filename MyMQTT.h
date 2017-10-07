#ifndef MYMQTT_H
#define MYMQTT_H

#include "Arduino.h"
#include <string>
#include <vector>

#include <PubSubClient.h>
#include <WiFiManager.h>

#define MQTT_PORT 1883

class hand
{
  public:
    hand(const std::string& topic, void (*handler)(const std::string& topic, byte* payload, unsigned int length))
    {
      m_topic = topic;
      m_handler = handler;
    };
    hand(const std::string& topic, void (*handler)(const std::string& topic, const std::string& payload))
    {
      m_topic = topic;
      m_handler_string = handler;
    };
    
    const std::string& topic() {return m_topic;};
    
    void exe (const std::string& topic, byte* payload, unsigned int length)
    {
      if(m_handler != NULL)
      {
        m_handler(topic, payload, length);
      }
      if(m_handler_string != NULL)
      {
        std::string str{payload, payload+length};
        m_handler_string(topic, str);
      };
    };
    
  private:
    void (*m_handler)(const std::string& topic, byte* payload, unsigned int length) = NULL;
    void (*m_handler_string)(const std::string& topic, const std::string& payload) = NULL;
    std::string m_topic;
};

class MyMQTT
{
  public:
    MyMQTT();
    
    void init(const std::string& server, const std::string& name);
    void add_topic(const std::string& topic, void (*handler)(const std::string& topic, byte* payload, unsigned int length));
    void add_topic(const std::string& topic, void (*handler)(const std::string& topic, const std::string& payload));
    void publish(const std::string& topic, const std::string& payload);
    void publish(const std::string& topic, int payload);
    void publish_retain(const std::string& topic, const std::string& payload);
    void publish_retain(const std::string& topic, int payload);
    void set_max_retry_count_and_handler(int count, void (*max_retry_handler)(void)) {m_max_retry_count = count; m_max_retry_handler = max_retry_handler;};
    void loop();
    void mqtt_callback(const std::string& topic, byte* payload, unsigned int length);
    
  private:
    std::string m_server;
    std::string m_name;
    bool m_configured = false;
    
    unsigned int m_max_retry_count = 5;
    void (*m_max_retry_handler)(void) = NULL;

    std::vector<hand> m_handlers{};

    WiFiClient m_wifi;    
    PubSubClient m_client;

    void reconnect();
};

#endif
