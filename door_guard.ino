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

void serial_println(const char *message)
{
  mq_push(&serial_queue, message);
  mq_push(&serial_queue, "\r\n");
}

ICACHE_RAM_ATTR void door_change() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 200) {
    if (digitalRead(DOOR_SENSOR_PIN)) {
      serial_println("door closed");
    } else {
      serial_println("door opened");
    }
  }

  last_interrupt_time = interrupt_time;
}

void door_sensor_init() {
  pinMode(DOOR_SENSOR_PIN, INPUT);
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
  Serial.println("boot sequence start");

  internal_led_init();

  door_sensor_init();
  pir_sensor_init();

  Serial.println("boot sequence finished");

  mq_init(&serial_queue, serial_queue_buffer, (size_t)20);
}

void handle_serial_queue()
{
  if (MQ_FALSE == mq_is_empty(&serial_queue)) {
    char message[30];
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
  handle_serial_queue();
}
