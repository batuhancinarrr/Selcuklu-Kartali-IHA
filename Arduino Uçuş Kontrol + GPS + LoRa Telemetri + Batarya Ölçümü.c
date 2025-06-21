#include <SPI.h>
#include <LoRa.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define SS 10
#define RST 9
#define DI0 2

// ESC Pin (�rnek)
#define ESC_PIN 3

// Batarya voltaj �l��m pini
#define BATTERY_PIN A0

// GPS i�in SoftwareSerial
SoftwareSerial ss(4, 5); // RX, TX
TinyGPSPlus gps;

uint16_t packetCounter = 0;

// ESC kontrol de�i�kenleri
int escValue = 1000;  // Minimum ESC sinyal s�resi (1000us)
int escMin = 1000;
int escMax = 2000;

void setup() {
  Serial.begin(115200);
  ss.begin(9600);

  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa ba�lat�lamad�!");
    while (1);
  }
  Serial.println("LoRa ba�lat�ld�.");

  pinMode(ESC_PIN, OUTPUT);

  // ESC'yi ba�lang��ta minimum sinyalle ba�lat
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

  Serial.println("G�nderildi: " + packet);
}

float readBatteryVoltage() {
  int raw = analogRead(BATTERY_PIN);
  // �l��m: �rn. 5V referans, 1:3 voltaj b�l�c� kullan�ld�ysa �arpanla d�zelt
  float voltage = raw * (5.0 / 1023.0) * 3.0;
  return voltage;
}

void readGPS() {
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
}

void controlESC() {
  // Basit �rnek: ESC sinyalini art�r�p azalt
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
