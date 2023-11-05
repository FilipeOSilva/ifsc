#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
#include <PubSubClient.h>

#define DELAY_SEND_MQTT_MSG 1000
#define DELAY_READ_INPUT    50
#define DELAY_PUMP_CONTROL  100
#define MAX_TRY_CONNECT     5
#define SENSOR_PIN  D3
#define ACTUATOR    D5
#define ANALOG_PIN  A0
#define DEVICE_ID       10001
#define DEBOUNCE_RANGE  5
#define MAX_LEVEL_FLOOD 100
#define MIN_LEVEL_FLOOD 0
#define MAX_VOLTAGE     3.3
#define RESOLUTION_AC   1024

//Wi Fi
const char* ssid = "ssid_2.4G";
const char* password = "senhassid";

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org";
const char *topic = "trabalho-filipe-jorge";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

// control values
bool _input_state = 0;
bool _pump_state = 0;
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
void set_actuator(bool set);

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

  pinMode(SENSOR_PIN, INPUT_PULLUP); 
  pinMode(ACTUATOR, OUTPUT_OPEN_DRAIN);  
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
  char data[300];

  analog["data"] = millis();
  analog["tensao"] = _voltage_level;
  analog["nivel"] = _flood_level;
  analog["id"] = DEVICE_ID;
  analog["interruptor"] = _input_state;
  analog["status_bomba"] = _pump_state;
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
    _voltage_level = ((float)adcValue/RESOLUTION_AC) * MAX_VOLTAGE;

    /*
      The general equation of a straight line
        y = Ax + B
    */
    _flood_level = (_voltage_level* -97.09) + 310.68;

    if (_flood_level >= MAX_LEVEL_FLOOD) {
      _flood_level = MAX_LEVEL_FLOOD;
    } else if (_flood_level <= MIN_LEVEL_FLOOD) {
      _flood_level = MIN_LEVEL_FLOOD;
    }
    
    if (digitalRead(SENSOR_PIN)) {
      if (debounce >= DEBOUNCE_RANGE) {
        _input_state = true;
        return;
      }
      debounce++;
    } else {
      debounce = 0;
      _input_state = false;
    }
  }
}

void control_pump(void) {
  static long long pooling  = 0;

  if (millis() > pooling) {
    pooling = millis() + DELAY_PUMP_CONTROL;

    if (_pump_state && !_input_state) {
      set_actuator(false);
    }

    if (_flood_level <= 85) {
      set_actuator(true);
    } else if (_flood_level >= 95) {
      set_actuator(false);
    }
  }
}

void set_actuator(bool set) {
  if (set && _input_state) {
    digitalWrite(ACTUATOR, LOW);
    _pump_state = true;
  } else {
    digitalWrite(ACTUATOR, HIGH);
    _pump_state = false;
  }
}