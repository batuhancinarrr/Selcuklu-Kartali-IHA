#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2
#define BUTTON_PIN 12

#define DHTPIN 4
#define DHTTYPE DHT11

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  dht.begin();

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa başlatılamadı!");
    while (1);
  }

  Serial.println("GPS + DHT11 + LoRa verici hazır!");
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (digitalRead(BUTTON_PIN) == LOW && gps.location.isUpdated()) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    String message = "Konum: ";
    message += gps.location.lat(), message += ",";
    message += gps.location.lng();
    message += " | Sicaklik: ";
    message += String(temp, 1) + "C";
    message += " | Nem: ";
    message += String(hum, 1) + "%";

    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();

    Serial.println("Gönderildi: " + message);
    delay(1000);
  }
}