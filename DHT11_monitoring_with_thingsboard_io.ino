#include <WiFi.h>
#include "DHT.h"
#include <ThingsBoard.h>
#include <Arduino_MQTT_Client.h>

// WiFi credentials
#define WIFI_SSID     "AndroidAP3DEC"
#define WIFI_PASSWORD "12345678"

// ThingsBoard server
#define TOKEN              "v36rgg2kib26wqjdqhi3"
#define THINGSBOARD_SERVER "demo.thingsboard.io"  // Replace with IP if using local server

// DHT settings
#define DHTPIN 14         // GPIO pin
#define DHTTYPE DHT11     // or DHT22

DHT dht(DHTPIN, DHTTYPE);

// WiFi and MQTT
WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient);

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}

void connectThingsBoard() {
  while (!tb.connected()) {
    Serial.print("Connecting to ThingsBoard... ");
    if (tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("connected");
    } else {
      Serial.print("failed. Retry in 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!tb.connected()) {
    connectThingsBoard();
  }

  tb.loop();

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Sending Temp: ");
  Serial.print(temp);
  Serial.print(" °C, Hum: ");
  Serial.println(hum);

  // Send data to ThingsBoard
  tb.sendTelemetryData("temperature", temp);
  tb.sendTelemetryData("humidity", hum);

  delay(5000);  // Delay between readings
}
