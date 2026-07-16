/*
 * ESP32 MQTT - Control de LEDs para dashboard web
 *
 * Características:
 * - Control de 3 LEDs vía MQTT (ON, OFF, TOGGLE)
 * - Sincronización bidireccional de estado
 * - Reconexión automática WiFi y MQTT
 * - Configuración opcional en config_secrets.h
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Forward declarations
void setupWiFi();
bool reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void processControlCommand(String command, int ledNum);
void setLedState(bool state, int ledNum);
void publishLedStates();
void WiFiEvent(WiFiEvent_t event);

#ifdef __has_include
  #if __has_include("config_secrets.h")
    #include "config_secrets.h"
  #endif
#endif

#ifndef WIFI_SSID
  #define WIFI_SSID "Wokwi-GUEST"
  #define WIFI_PASSWORD ""
#endif

#ifndef MQTT_BROKER
  #define MQTT_BROKER "broker.hivemq.com"
  #define MQTT_PORT 1883
#endif

const char* MQTT_TOPIC_LED1 = "LED1/control";
const char* MQTT_TOPIC_LED2 = "LED2/control";
const char* MQTT_TOPIC_LED3 = "LED3/control";

const char* MQTT_TOPIC_LED1_STATUS = "LED1/status";
const char* MQTT_TOPIC_LED2_STATUS = "LED2/status";
const char* MQTT_TOPIC_LED3_STATUS = "LED3/status";

#ifndef DEVICE_ID
  #define MQTT_CLIENT_ID "ESP32-LED-Control-001"
#else
  const char* MQTT_CLIENT_ID = DEVICE_ID;
#endif

#ifndef LED1_PIN
  #define LED1_PIN 3
#endif

#ifndef LED2_PIN
  #define LED2_PIN 5
#endif

#ifndef LED3_PIN
  #define LED3_PIN 7
#endif

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

unsigned long lastReconnectAttempt = 0;
const unsigned long RECONNECT_INTERVAL = 5000;

bool led1State = false;
bool led2State = false;
bool led3State = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\n========================================");
  Serial.println("ESP32 MQTT LED Control - Iniciando...");
  Serial.println("========================================");

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);

  setupWiFi();

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  Serial.println("========================================");
  Serial.println("Sistema listo. Esperando mensajes MQTT...");
  Serial.println("========================================");
}

void loop() {
  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > RECONNECT_INTERVAL) {
      lastReconnectAttempt = now;
      if (reconnectMQTT()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    mqttClient.loop();
  }

  delay(100);
}

void setupWiFi() {
  Serial.print("Conectando a WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi conectado exitosamente");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Señal RSSI: ");
    Serial.println(WiFi.RSSI());
  } else {
    Serial.println("\n❌ Error: No se pudo conectar a WiFi");
    Serial.println("Reiniciando en 5 segundos...");
    delay(5000);
    ESP.restart();
  }
}

bool reconnectMQTT() {
  Serial.print("Intentando conectar al broker MQTT: ");
  Serial.println(MQTT_BROKER);

  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println("✅ Conectado al broker MQTT");

    mqttClient.subscribe(MQTT_TOPIC_LED1);
    Serial.print("Suscrito al tópico: ");
    Serial.println(MQTT_TOPIC_LED1);

    mqttClient.subscribe(MQTT_TOPIC_LED2);
    Serial.print("Suscrito al tópico: ");
    Serial.println(MQTT_TOPIC_LED2);

    mqttClient.subscribe(MQTT_TOPIC_LED3);
    Serial.print("Suscrito al tópico: ");
    Serial.println(MQTT_TOPIC_LED3);

    publishLedStates();
    return true;
  }

  Serial.print("❌ Falló conexión MQTT, rc=");
  Serial.println(mqttClient.state());
  Serial.println("Intentando nuevamente en 5 segundos...");
  return false;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");

  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == MQTT_TOPIC_LED1) {
    processControlCommand(message, 1);
  } else if (String(topic) == MQTT_TOPIC_LED2) {
    processControlCommand(message, 2);
  } else if (String(topic) == MQTT_TOPIC_LED3) {
    processControlCommand(message, 3);
  }
}

void processControlCommand(String command, int ledNum) {
  Serial.print("Procesando comando para LED");
  Serial.print(ledNum);
  Serial.print(": ");
  Serial.println(command);

  bool newState = false;

  if (command == "ON" || command == "on") {
    newState = true;
  } else if (command == "OFF" || command == "off") {
    newState = false;
  } else if (command == "TOGGLE" || command == "toggle") {
    switch (ledNum) {
      case 1: newState = !led1State; break;
      case 2: newState = !led2State; break;
      case 3: newState = !led3State; break;
    }
  } else {
    Serial.println("Comando no reconocido");
    return;
  }

  setLedState(newState, ledNum);
}

void setLedState(bool state, int ledNum) {
  int pin;
  const char* statusTopic;

  switch (ledNum) {
    case 1:
      led1State = state;
      pin = LED1_PIN;
      statusTopic = MQTT_TOPIC_LED1_STATUS;
      break;
    case 2:
      led2State = state;
      pin = LED2_PIN;
      statusTopic = MQTT_TOPIC_LED2_STATUS;
      break;
    case 3:
      led3State = state;
      pin = LED3_PIN;
      statusTopic = MQTT_TOPIC_LED3_STATUS;
      break;
    default:
      return;
  }

  digitalWrite(pin, state ? HIGH : LOW);
  Serial.print("LED");
  Serial.print(ledNum);
  Serial.println(state ? " encendido" : " apagado");

  String stateMessage = state ? "ON" : "OFF";
  mqttClient.publish(statusTopic, stateMessage.c_str());
  Serial.print("Estado LED");
  Serial.print(ledNum);
  Serial.print(" publicado: ");
  Serial.println(stateMessage);

  // Si se está encendiendo un LED, apagar automáticamente los otros dos
  if (state) {
    Serial.println("Verificando otros LEDs para exclusión mutua...");
    
    if (ledNum != 1 && led1State) {
      led1State = false;
      digitalWrite(LED1_PIN, LOW);
      mqttClient.publish(MQTT_TOPIC_LED1_STATUS, "OFF");
      Serial.println("LED1 apagado automáticamente (exclusión mutua)");
    }
    if (ledNum != 2 && led2State) {
      led2State = false;
      digitalWrite(LED2_PIN, LOW);
      mqttClient.publish(MQTT_TOPIC_LED2_STATUS, "OFF");
      Serial.println("LED2 apagado automáticamente (exclusión mutua)");
    }
    if (ledNum != 3 && led3State) {
      led3State = false;
      digitalWrite(LED3_PIN, LOW);
      mqttClient.publish(MQTT_TOPIC_LED3_STATUS, "OFF");
      Serial.println("LED3 apagado automáticamente (exclusión mutua)");
    }
  }
}

void publishLedStates() {
  String state1 = led1State ? "ON" : "OFF";
  String state2 = led2State ? "ON" : "OFF";
  String state3 = led3State ? "ON" : "OFF";

  mqttClient.publish(MQTT_TOPIC_LED1_STATUS, state1.c_str());
  mqttClient.publish(MQTT_TOPIC_LED2_STATUS, state2.c_str());
  mqttClient.publish(MQTT_TOPIC_LED3_STATUS, state3.c_str());

  Serial.println("Estados iniciales publicados:");
  Serial.print("LED1: "); Serial.println(state1);
  Serial.print("LED2: "); Serial.println(state2);
  Serial.print("LED3: "); Serial.println(state3);
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi desconectado, intentando reconectar...");
      WiFi.reconnect();
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("WiFi reconectado");
      break;
    default:
      break;
  }
}

WiFiEventId_t eventID = WiFi.onEvent(WiFiEvent);