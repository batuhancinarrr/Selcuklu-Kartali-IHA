#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>

#define DHTPIN 13
#define DHTTYPE DHT22 // veya DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  LoRa.setPins(5, 14, 26);
  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa başlatılamadı!");
    while (1);
  }

  Serial.println("Verici Başladı");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("DHT okumada hata!");
    return;
  }

  String msg = "Sıcaklık: " + String(t) + "°C, Nem: " + String(h) + "%";
  Serial.println("Gönderiliyor: " + msg);

  LoRa.beginPacket();
  LoRa.print(msg);
  LoRa.endPacket();

  delay(5000); // 5 saniyede bir gönderim
}