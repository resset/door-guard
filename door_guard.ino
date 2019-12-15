#include <ESP8266WiFi.h>
//#include <EspMQTTClient.h>

#include "door_guard.h"

//EspMQTTClient mqttclient(
//  WIFI_SSID,
//  WIFI_PASS,
//  MQTT_IP,
//  MQTT_USER,
//  MQTT_PASS,
//  "door-guard"
//);

//void onConnectionEstablished() {
//  Serial.println("MQTT connection!");
//}

#error "TODO: add interrupt queue"

#define DOOR_SENSOR_PIN 13
#define PIR_SENSOR_PIN 12

inline void internal_led_on()
{
  digitalWrite(LED_BUILTIN, LOW);
}

inline void internal_led_off()
{
  digitalWrite(LED_BUILTIN, HIGH);
}

ICACHE_RAM_ATTR void door_change() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 200) {
    if (digitalRead(DOOR_SENSOR_PIN)) {
      Serial.println("door closed");
    } else {
      Serial.println("door opened");
    }
  }

  last_interrupt_time = interrupt_time;
}

void door_sensor_setup() {
  pinMode(DOOR_SENSOR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(DOOR_SENSOR_PIN), door_change, CHANGE);
}

ICACHE_RAM_ATTR void pir_rising() {
  internal_led_on();
  //delayMicroseconds(50000);
  Serial.println("pir detection");
  internal_led_off();
}

void pir_sensor_setup() {
  pinMode(PIR_SENSOR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_PIN), pir_rising, RISING);
}

//void send_report() {
//  Serial.println("sending report!");
//  delay(500);
//  if (mqttclient.isConnected()) {
//    Serial.println("connected, publishing");
//    mqttclient.publish("channel/test", "This is a message from ESP");
//  }
//}

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("boot sequence start");

  pinMode(LED_BUILTIN, OUTPUT);
  internal_led_off();

  door_sensor_setup();
  pir_sensor_setup();

  Serial.println("boot sequence finished");
}

void loop() {
  //mqttclient.loop();

  Serial.println("hb");

  delay(5000);
}
