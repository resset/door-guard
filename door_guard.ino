#include <ESP8266WiFi.h>
//#include <EspMQTTClient.h>

#include "mq.h"
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

#define DOOR_SENSOR_PIN 13
#define PIR_SENSOR_PIN 12

char serial_queue_buffer[300];
mq_t serial_queue;

volatile bool door_open = false;
volatile unsigned long last_change_time = 0;

inline void serial_print(const char *message)
{
  noInterrupts();
  mq_push(&serial_queue, message);
  interrupts();
}

inline void serial_println(const char *message)
{
  noInterrupts();
  mq_push(&serial_queue, message);
  mq_push(&serial_queue, "\r\n");
  interrupts();
}

inline void internal_led_init()
{
  pinMode(LED_BUILTIN, OUTPUT);
  internal_led_off();
}

inline void internal_led_on()
{
  digitalWrite(LED_BUILTIN, LOW);
}

inline void internal_led_off()
{
  digitalWrite(LED_BUILTIN, HIGH);
}

ICACHE_RAM_ATTR void door_change() {
  last_change_time = millis();
}

void door_sensor_init() {
  pinMode(DOOR_SENSOR_PIN, INPUT);

  if (digitalRead(DOOR_SENSOR_PIN)) {
    door_open = false;
  } else {
    door_open = true;
  }

  attachInterrupt(digitalPinToInterrupt(DOOR_SENSOR_PIN), door_change, CHANGE);
}

ICACHE_RAM_ATTR void pir_rising() {
  serial_println("pir");
}

void pir_sensor_init() {
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

  internal_led_init();
  door_sensor_init();
  pir_sensor_init();

  Serial.println("door-guard boot finished");

  mq_init(&serial_queue, serial_queue_buffer, (size_t)20);
}

void handle_door()
{
  if (last_change_time > 0) {
    if (millis() - last_change_time > 500) {

      if (digitalRead(DOOR_SENSOR_PIN)) {
        door_open = false;
        serial_println("door closed");
      } else {
        door_open = true;
        serial_println("door open");
      }

      last_change_time = 0;
    }
  }
}

void handle_serial_queue()
{
  while (MQ_FALSE == mq_is_empty(&serial_queue)) {
    char message[100];
    noInterrupts();
    mq_result_t res = mq_pop(&serial_queue, message);
    interrupts();
    if (res == MQ_SUCCESS) {
        Serial.write(message, strlen(message));
    }
  }
}

void loop() {
  //mqttclient.loop();

  handle_door();
  handle_serial_queue();

  if (door_open) {
    internal_led_on();
  } else {
    internal_led_off();
  }
}
