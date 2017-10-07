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

void MyMQTT::init(const string& server, const string& name)
{
  m_server = server;
  m_name = name;

  m_client.setClient(m_wifi);
  m_client.setServer(m_server.c_str(), MQTT_PORT);
  m_client.setCallback(local_mqtt_callback);
  m_configured = true;
}

void MyMQTT::add_topic(const string& topic, void (*handler)(const string& topic, byte* payload, unsigned int length))
{
  m_handlers.push_back(hand{topic, handler});
}

void MyMQTT::add_topic(const string& topic, void (*handler)(const string& topic, const string& payload))
{
  m_handlers.push_back(hand{topic, handler});
}

void MyMQTT::publish(const string& topic, const string& payload)
{
  m_client.publish(topic.c_str(), payload.c_str());
}

void MyMQTT::publish(const string& topic, int payload)
{
  char n[8];
  m_client.publish(topic.c_str(), itoa(payload, n, 10));
}

void MyMQTT::publish_retain(const string& topic, const string& payload)
{
  m_client.publish(topic.c_str(), payload.c_str(), true);
}

void MyMQTT::publish_retain(const string& topic, int payload)
{
  char n[8];
  m_client.publish(topic.c_str(), itoa(payload, n, 10), true);
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

  if (m_client.connect(m_name.c_str())) {
    Serial.println("connected");
    
    Serial.println("Subscribing to:");
    for (auto it = begin(m_handlers); it != end(m_handlers); ++it)
    {
      Serial.println(it->topic().c_str());
      m_client.subscribe(it->topic().c_str());
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

void MyMQTT::mqtt_callback(const string& topic, byte* payload, unsigned int length)
{
  for (auto it = begin(m_handlers); it != end(m_handlers); ++it)
  {
    if(it->topic() == topic)
    {
//      Serial.println(topic.c_str());
      it->exe(topic, payload, length);
    }
  }
}
