#include "MyMQTT.h"

using namespace std;

static MyMQTT *local_mqtt;

void local_mqtt_callback(char* topic, byte* payload, unsigned int length)
{
//  Serial.print("local_mqtt_callback: ");
//  Serial.println(topic);
  local_mqtt->mqtt_callback(topic, payload, length);
}

MyMQTT::MyMQTT()
{
  local_mqtt = this;
}

void MyMQTT::init(const char* server, const char* name)
{
  strcpy(m_server, server);
  strcpy(m_name, name);

  m_client.setClient(m_wifi);
  m_client.setServer(m_server, MQTT_PORT);
  m_client.setCallback(local_mqtt_callback);
  m_configured = true;
}

void MyMQTT::add_topic(const char* topic, void (*handler)(const char* topic, byte* payload, unsigned int length))
{
  m_handlers.push_back(hand{topic, handler});
}

void MyMQTT::add_topic(const char* topic, void (*handler)(const char* topic, const char* payload))
{
  m_handlers.push_back(hand{topic, handler});
}

void MyMQTT::add_topic(const char* topic, void (*handler)(const char* topic, int payload))
{
  m_handlers.push_back(hand{topic, handler});
}

void MyMQTT::publish(const char* topic, const char* payload)
{
  m_client.publish(topic, payload);
}

void MyMQTT::publish(const char* topic, int payload)
{
  char n[8];
  m_client.publish(topic, itoa(payload, n, 10));
}

void MyMQTT::publish_retain(const char* topic, const char* payload)
{
  m_client.publish(topic, payload, true);
}

void MyMQTT::publish_retain(const char* topic, int payload)
{
  char n[8];
  m_client.publish(topic, itoa(payload, n, 10), true);
}


void MyMQTT::loop()
{
  static long int last_mqtt_reconnect = 0;
  long int current_time = millis();

  if (m_configured && !m_client.connected() && ((current_time - last_mqtt_reconnect) > 5000)) {
    reconnect();
    last_mqtt_reconnect = current_time;
  }
  m_client.loop();
}

void MyMQTT::reconnect()
{
  static int reconnect_count = 0;
  Serial.print("Attempting MQTT connection...");

  if (m_client.connect(m_name)) {
    Serial.println("connected");

    Serial.println("Subscribing to:");
    for (auto it = begin(m_handlers); it != end(m_handlers); ++it)
    {
      Serial.println(it->topic());
      m_client.subscribe(it->topic());
      m_client.loop();
      yield();
    }
  }
  else
  {
    Serial.println("Failed");
    reconnect_count++;
    if ((reconnect_count > m_max_retry_count) && (m_max_retry_handler != NULL))
    {
      reconnect_count = 0;
      m_max_retry_handler();
    }
  }
}

void MyMQTT::mqtt_callback(const char* topic, byte* payload, unsigned int length)
{
  for (auto it = begin(m_handlers); it != end(m_handlers); ++it)
  {
    if(strcmp(it->topic(), topic) == 0)
    {
//      Serial.println(topic);
      it->exe(topic, payload, length);
    }
  }
}
