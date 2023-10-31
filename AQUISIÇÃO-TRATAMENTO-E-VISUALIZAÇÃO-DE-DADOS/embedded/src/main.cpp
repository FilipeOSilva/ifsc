#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
#include <PubSubClient.h>

#define DELAY_SEND_MQTT_MSG 1000
#define DELAY_READ_INPUT 50
#define DELAY_PUMP_CONTROL 100
#define MAX_TRY_CONNECT 5
#define SENSOR_PIN D3
#define ACTUATOR D5
#define ANALOG_PIN A0
#define DEVICE_ID 10001
#define DEBOUNCE_RANGE 10

//Wi Fi
const char* ssid = "zeus_backend";
const char* password = "lockzeus";

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org";
const char *topic = "BOBSIEN/teste";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

// control values
bool _input_state = 0;
float _flood_level = 0.000;
float _voltage_level = 0.0000;
bool _mqtt_status = 0;

WiFiClient espClient;
PubSubClient client(espClient);

// internal metod
void control_pump(void); 
void control_mqtt(void);
void control_input(void);
void send_data_to_broker(void);
bool connect_MQTT(void);
void callback(char *topic, byte * payload, unsigned int length);

void setup(void) {
  WiFi.begin(ssid, password);
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("Conectando");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  _mqtt_status =  connect_MQTT();

  pinMode(SENSOR_PIN, INPUT); 
  pinMode(ACTUATOR, OUTPUT);  
}

void loop() {
  control_mqtt();
  control_input();
  control_pump();
}

bool connect_MQTT(void) {
  int tried = 0;
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  do {
    String client_id = "Client-"+String(DEVICE_ID);
    client_id += String(WiFi.macAddress());

    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected:");
      Serial.printf("Client %s connected to the broker\n", client_id.c_str());
    } else {
      Serial.print("Failed to connect: ");
      Serial.print(client.state());
      Serial.println();
      Serial.print("Tried: ");
      Serial.println(tried);
      delay(DELAY_SEND_MQTT_MSG);
    }
    tried++;
  } while (!client.connected() && tried < MAX_TRY_CONNECT);

  if (tried < MAX_TRY_CONNECT) {
    send_data_to_broker();
    client.subscribe(topic);
    
    return true;
  } else {
    Serial.println("Failed. Not connected.");    
    
    return false;
  }
}

void callback(char *topic, byte * payload, unsigned int length) {
  unsigned int i;
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (i=0;i<length;i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void send_data_to_broker(void) {
  StaticJsonDocument<300> analog;
  char data[100];

  analog["data"] = "Voltage";
  analog["tensao"] = _voltage_level;
  analog["nivel"] = _flood_level;
  analog["id"] = DEVICE_ID;
  analog["interruptor"] = _input_state;
  analog["bomba"] = 1;
  serializeJson(analog, data);
 
  client.publish(topic, data); 
}

void control_mqtt(void) {
  static long long pooling  = 0;

  if (_mqtt_status) {
    client.loop();    
    if (millis() > pooling) {
      send_data_to_broker();
      pooling = millis() +  DELAY_SEND_MQTT_MSG;
    }
  }
}

void control_input(void) {
  static long long pooling  = 0;
  static int debounce = 0;
  int adcValue = 0;
  
  if (millis() > pooling) {
    pooling = millis() + DELAY_READ_INPUT;

    adcValue = analogRead(ANALOG_PIN); 
    _voltage_level = ((float)adcValue/1024) * 3.3;
    _flood_level = (_voltage_level*100) / 3.3;
    
    if (digitalRead(SENSOR_PIN)) {
      if (debounce >= DEBOUNCE_RANGE) {
        _input_state = 1;
        return;
      }
      debounce++;
    } else {
      debounce = 0;
      _input_state = 0;
    }
  }
}

void control_pump(void) {
  static long long pooling  = 0;

  if (millis() > pooling) {
    pooling = millis() + DELAY_PUMP_CONTROL;

  }
}