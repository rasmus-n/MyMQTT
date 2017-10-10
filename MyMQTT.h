#ifndef MYMQTT_H
#define MYMQTT_H

#include "Arduino.h"
//#include <string>
#include <vector>

#include <PubSubClient.h>
#include <WiFiManager.h>

#define MQTT_PORT 1883

class hand
{
  public:
    hand(const char* topic, void (*handler)(const char* topic, byte* payload, unsigned int length))
    {
      strcpy(m_topic, topic);
      m_handler = handler;
    };
    hand(const char* topic, void (*handler)(const char* topic, const char* payload))
    {
      strcpy(m_topic, topic);
      m_handler_string = handler;
    };
    
    const char* topic() {return m_topic;};
    
    void exe (const char* topic, byte* payload, unsigned int length)
    {
      char str[length+1];
      if(m_handler != NULL)
      {
        m_handler(topic, payload, length);
      }
      if(m_handler_string != NULL)
      {
        strncpy(str, (char*) payload, length);
        str[length] = 0;
        m_handler_string(topic, str);
      };
    };
    
  private:
    void (*m_handler)(const char* topic, byte* payload, unsigned int length) = NULL;
    void (*m_handler_string)(const char* topic, const char* payload) = NULL;
    char m_topic[32];
};

class MyMQTT
{
  public:
    MyMQTT();
    
    void init(const char* server, const char *name);
    void add_topic(const char* topic, void (*handler)(const char* topic, byte* payload, unsigned int length));
    void add_topic(const char* topic, void (*handler)(const char* topic, const char* payload));
    void publish(const char* topic, const char* payload);
    void publish(const char* topic, int payload);
    void publish_retain(const char* topic, const char* payload);
    void publish_retain(const char* topic, int payload);
    void set_max_retry_count_and_handler(int count, void (*max_retry_handler)(void)) {m_max_retry_count = count; m_max_retry_handler = max_retry_handler;};
    void loop();
    void mqtt_callback(const char *topic, byte* payload, unsigned int length);
    
  private:
    char m_server[16];
    char m_name[16];
    bool m_configured = false;
    
    unsigned int m_max_retry_count = 5;
    void (*m_max_retry_handler)(void) = NULL;

    std::vector<hand> m_handlers{};

    WiFiClient m_wifi;    
    PubSubClient m_client;

    void reconnect();
};

#endif
