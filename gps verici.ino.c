#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>

TinyGPSPlus gps;
HardwareSerial GPSSerial(2); // UART2: GPIO16 (RX), GPIO17 (TX)

void setup() {
  Serial.begin(115200);
  GPSSerial.begin(9600, SERIAL_8N1, 16, 17); // GPS bağlantısı

  LoRa.setPins(5, 14, 26);
  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa başlatılamadı!");
    while (1);
  }

  Serial.println("GPS Verici Hazır");
}

void loop() {
  while (GPSSerial.available()) {
    gps.encode(GPSSerial.read());
  }

  if (gps.location.isUpdated()) {
    float lat = gps.location.lat();
    float lng = gps.location.lng();

    String msg = "LAT:" + String(lat, 6) + ", LNG:" + String(lng, 6);
    Serial.println("Gönderiliyor: " + msg);

    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();

    delay(5000); // 5 saniyede bir gönder
  }
}