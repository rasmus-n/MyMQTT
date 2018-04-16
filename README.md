# MyMQTT
My MQTT helper library for Arduino.

Nothing fancy. All the actual MQTT stuff is handled by [PubSubClient](https://github.com/knolleary/pubsubclient).

The aim for me was just to make use of MQTT easier and not continue copying big blocks of code from project to project with small adaptions.

## Example

```cpp
#include <MyWiFi.h>
MyWiFi my_wifi;

#include <MyMQTT.h>
MyMQTT my_mqtt;

#include <ArduinoOTA.h>

#define RELAY_OUT 14

void wifi_config(const char* topic, byte* payload, unsigned int length)
{
  my_wifi.config();
}

void wifi_config()
{
  my_wifi.config();
}

void set_switch(const char* topic, int payload)
{
  char reply_topic[32];

  digitalWrite(RELAY_OUT, payload == 1);

  sprintf(reply_topic, "%s/state", my_wifi.hostname());
  my_mqtt.publish_retain(reply_topic, payload);
}

void setup() {
  char topic[32];

  pinMode(RELAY_OUT, OUTPUT);
  digitalWrite(RELAY_OUT, LOW);

  Serial.begin(115200);

  my_wifi.setup();

  ArduinoOTA.setHostname(my_wifi.hostname());
  ArduinoOTA.begin();

  sprintf(topic, "%s/set", my_wifi.hostname());
  my_mqtt.add_topic(topic, set_switch);

  sprintf(topic, "%s/wifi", my_wifi.hostname());
  my_mqtt.add_topic(topic, wifi_config);

  my_mqtt.set_max_retry_count_and_handler(5, wifi_config);

  my_mqtt.init(my_wifi.server(), my_wifi.hostname());

}

void loop() {
  my_mqtt.loop();
  my_wifi.loop();
  ArduinoOTA.handle();
}
```
