#include <ESP8266WiFi.h>
#include <EspMQTTClient.h>

#include "mq.h"
#include "door_guard.h"

EspMQTTClient mqttclient(
  WIFI_SSID,
  WIFI_PASS,
  MQTT_IP,
  MQTT_USER,
  MQTT_PASS,
  "door-guard",
  1883
);

#define DOOR_SENSOR_PIN 13
#define PIR_SENSOR_PIN 12

#define SERIAL_BUFFER_SIZE 300
#define SERIAL_MESSAGE_SIZE 80
char serial_queue_buffer[SERIAL_BUFFER_SIZE];
mq_t serial_queue;

volatile bool door_open = false;
volatile unsigned long last_change_time = 0;

inline void serial_print_i(const char *message)
{
  mq_push(&serial_queue, message);
}

inline void serial_println_i(const char *message)
{
  serial_print_i(message);
  serial_print_i("\r\n");
}

inline void serial_print(const char *message)
{
  noInterrupts();
  serial_print_i(message);
  interrupts();
}

inline void serial_println(const char *message)
{
  noInterrupts();
  serial_println_i(message);
  interrupts();
}

void onConnectionEstablished() {
  serial_println("MQTT connected");
}

void send_report(const char *topic, const char *message)
{
  noInterrupts();
  serial_print_i(topic);
  serial_print_i(": ");
  serial_print_i(message);
  interrupts();

  if (mqttclient.isConnected()) {
    mqttclient.publish(topic, message);
    serial_println(" ...sent!");
  } else {
    serial_println(" ...not sent!");
  }
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
  send_report("home/sensors/pir_door", "event");
}

void pir_sensor_init() {
  pinMode(PIR_SENSOR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_PIN), pir_rising, RISING);
}

void setup() {
  Serial.begin(115200);
  Serial.println("");

  internal_led_init();
  door_sensor_init();
  pir_sensor_init();

  Serial.println("door-guard boot finished");

  mq_init(&serial_queue, serial_queue_buffer, (size_t)SERIAL_BUFFER_SIZE);
}

void handle_door()
{
  if (last_change_time > 0) {
    if (millis() - last_change_time > 500) {

      if (digitalRead(DOOR_SENSOR_PIN)) {
        door_open = false;
        send_report("home/sensors/door", "closed");
      } else {
        door_open = true;
        send_report("home/sensors/door", "open");
      }

      last_change_time = 0;
    }
  }
}

void handle_serial_queue()
{
  while (MQ_FALSE == mq_is_empty(&serial_queue)) {
    char message[SERIAL_MESSAGE_SIZE];
    noInterrupts();
    mq_result_t res = mq_pop(&serial_queue, message);
    interrupts();
    if (res == MQ_SUCCESS) {
        Serial.write(message, strlen(message));
    }
  }
}

void loop() {
  mqttclient.loop();

  handle_door();
  handle_serial_queue();

  if (door_open) {
    internal_led_on();
  } else {
    internal_led_off();
  }
}
