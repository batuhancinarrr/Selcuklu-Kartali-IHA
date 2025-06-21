#include <SPI.h>
#include <LoRa.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2
#define BUTTON_PIN 12

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // LoRa donanım pinleri
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa başlatılamadı!");
    while (1);
  }

  Serial.println("LoRa verici hazır!");
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    LoRa.beginPacket();
    LoRa.print("Merhaba, bu bir test paketidir!");
    LoRa.endPacket();
    Serial.println("Veri gönderildi!");
    delay(1000); // spam önlemi
  }
}