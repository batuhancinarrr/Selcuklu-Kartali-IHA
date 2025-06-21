#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <LoRa.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2
#define BUTTON_PIN 12

TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // UART1

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // TX=16, RX=17
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa başlatılamadı!");
    while (1);
  }

  Serial.println("GPS + LoRa verici hazır!");
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (digitalRead(BUTTON_PIN) == LOW && gps.location.isUpdated()) {
    String message = "Konum: ";
    message += gps.location.lat(), message += ",";
    message += gps.location.lng();

    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();

    Serial.println("Gönderildi: " + message);
    delay(1000);
  }
}