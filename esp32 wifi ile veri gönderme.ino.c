#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "WiFi_ADINIZ";
const char* password = "WiFi_SIFRENIZ";

// Web API URL'si (mobil backend ya da webhook olabilir)
const char* serverName = "http://192.168.1.100:3000/api/lora"; // örnek IP

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi bağlı!");

  LoRa.setPins(5, 14, 26);
  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa başlatılamadı!");
    while (1);
  }
  Serial.println("LoRa Alıcı WiFi ile Hazır");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String msg = "";
    while (LoRa.available()) {
      msg += (char)LoRa.read();
    }

    Serial.println("Alındı: " + msg);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");

      String json = "{\"data\":\"" + msg + "\"}";

      int responseCode = http.POST(json);
      Serial.print("HTTP Yanıt: ");
      Serial.println(responseCode);
      http.end();
    }
  }
}