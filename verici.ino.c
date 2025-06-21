#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(5, 14, 26); // NSS, RESET, DIO0

  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa başlatılamadı!");
    while (1);
  }

  Serial.println("LoRa Verici Hazır");
}

void loop() {
  Serial.println("Paket gönderiliyor...");
  LoRa.beginPacket();
  LoRa.print("Merhaba, dünya!");
  LoRa.endPacket();
  delay(2000); // 2 saniyede bir gönderim
}