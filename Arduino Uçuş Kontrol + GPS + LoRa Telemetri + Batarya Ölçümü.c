#include <SPI.h>
#include <LoRa.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define SS 10
#define RST 9
#define DI0 2

// ESC Pin (örnek)
#define ESC_PIN 3

// Batarya voltaj ölçüm pini
#define BATTERY_PIN A0

// GPS için SoftwareSerial
SoftwareSerial ss(4, 5); // RX, TX
TinyGPSPlus gps;

uint16_t packetCounter = 0;

// ESC kontrol deðiþkenleri
int escValue = 1000;  // Minimum ESC sinyal süresi (1000us)
int escMin = 1000;
int escMax = 2000;

void setup() {
  Serial.begin(115200);
  ss.begin(9600);

  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa baþlatýlamadý!");
    while (1);
  }
  Serial.println("LoRa baþlatýldý.");

  pinMode(ESC_PIN, OUTPUT);

  // ESC'yi baþlangýçta minimum sinyalle baþlat
  analogWrite(ESC_PIN, map(escValue, 1000, 2000, 0, 255));
}

uint8_t calculateChecksum(const String &data) {
  uint8_t sum = 0;
  for (size_t i = 0; i < data.length(); i++) {
    sum ^= data[i];
  }
  return sum;
}

void sendPacket(String payload) {
  packetCounter++;
  uint8_t checksum = calculateChecksum(payload);

  String packet = String(packetCounter) + "|" + payload + "|" + String(checksum);

  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();

  Serial.println("Gönderildi: " + packet);
}

float readBatteryVoltage() {
  int raw = analogRead(BATTERY_PIN);
  // Ölçüm: örn. 5V referans, 1:3 voltaj bölücü kullanýldýysa çarpanla düzelt
  float voltage = raw * (5.0 / 1023.0) * 3.0;
  return voltage;
}

void readGPS() {
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
}

void controlESC() {
  // Basit örnek: ESC sinyalini artýrýp azalt
  escValue += 10;
  if (escValue > escMax) escValue = escMin;

  analogWrite(ESC_PIN, map(escValue, 1000, 2000, 0, 255));
}

void loop() {
  readGPS();

  float batteryVoltage = readBatteryVoltage();
  String gpsStatus = gps.location.isValid() ? "GPS OK" : "GPS Yok";

  String payload = String("voltaj:") + batteryVoltage +
                   ",lat:" + (gps.location.isValid() ? String(gps.location.lat(), 6) : "0") +
                   ",lng:" + (gps.location.isValid() ? String(gps.location.lng(), 6) : "0") +
                   "," + gpsStatus;

  sendPacket(payload);

  controlESC();

  delay(1000);
}
