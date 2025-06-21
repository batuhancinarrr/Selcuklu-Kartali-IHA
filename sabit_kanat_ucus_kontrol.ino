
/*
  Sabit Kanatlı Uçuş Kontrol Yazılımı (Arduino)
  Özellikler:
  - GPS tabanlı otonom uçuş
  - LoRa üzerinden görev komutu alma
  - PID kontrollü servo motor çıkışı
  - Acil iniş ve fail-safe
*/

#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <SoftwareSerial.h>

// === Pin Tanımları ===
#define LORA_SS 10
#define LORA_RST 9
#define LORA_DI0 2

#define GPS_RX 4
#define GPS_TX 3

#define VOLTAGE_PIN A0

#define SERVO_ELEVATOR 5
#define SERVO_AILERON 6
#define SERVO_RUDDER 7

// === Servo Tanımları ===
Servo elevator, aileron, rudder;

// === GPS ===
TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);

// === PID Parametreleri ===
float Kp = 1.5, Ki = 0.02, Kd = 0.05;
float pitchSet = 0, rollSet = 0, yawSet = 0;
float pitchMeas = 0, rollMeas = 0, yawMeas = 0;
float pitchErrorLast = 0, rollErrorLast = 0, yawErrorLast = 0;
float pitchI = 0, rollI = 0, yawI = 0;

// === Waypoint Sistemi ===
struct Waypoint { double lat; double lng; };
Waypoint waypoints[10];
int wpIndex = 0, wpTotal = 0;

// === Durum Bayrakları ===
bool emergencyLanding = false;

// === Yardımcı Fonksiyonlar ===
float pid(float setpoint, float measured, float &I, float &lastE) {
  float error = setpoint - measured;
  I += error;
  float D = error - lastE;
  lastE = error;
  return Kp * error + Ki * I + Kd * D;
}

// === Ayarlar ===
void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  elevator.attach(SERVO_ELEVATOR);
  aileron.attach(SERVO_AILERON);
  rudder.attach(SERVO_RUDDER);

  // Servolar ortalanıyor
  elevator.write(90); aileron.write(90); rudder.write(90);

  // LoRa başlat
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa başlatılamadı!");
    while (1);
  }
  Serial.println("LoRa başlatıldı.");
}

// === Ana Döngü ===
void loop() {
  while (gpsSerial.available()) gps.encode(gpsSerial.read());
  if (LoRa.parsePacket()) handleLoRa();

  if (emergencyLanding) {
    performEmergency();
    return;
  }

  if (gps.location.isValid() && wpTotal > 0) {
    double dist = TinyGPSPlus::distanceBetween(
      gps.location.lat(), gps.location.lng(),
      waypoints[wpIndex].lat, waypoints[wpIndex].lng
    );
    double heading = TinyGPSPlus::courseTo(
      gps.location.lat(), gps.location.lng(),
      waypoints[wpIndex].lat, waypoints[wpIndex].lng
    );

    yawSet = heading;
    if (dist < 10 && wpIndex < wpTotal - 1) wpIndex++;

    // PID kontrol uygulaması (ölçülen değerler yerine sıfır varsayımı)
    float pitchOut = pid(pitchSet, pitchMeas, pitchI, pitchErrorLast);
    float rollOut  = pid(rollSet, rollMeas, rollI, rollErrorLast);
    float yawOut   = pid(yawSet, yawMeas, yawI, yawErrorLast);

    elevator.write(constrain(90 + pitchOut, 0, 180));
    aileron.write(constrain(90 + rollOut, 0, 180));
    rudder.write(constrain(90 + yawOut, 0, 180));
  }

  // Batarya kontrolü
  float volt = analogRead(VOLTAGE_PIN) * (5.0 / 1023.0) * 11.0;
  if (volt < 10.5) emergencyLanding = true;
}

// === Acil iniş fonksiyonu ===
void performEmergency() {
  elevator.write(60); // burnu aşağı
  aileron.write(90);
  rudder.write(90);
  Serial.println("Acil iniş modunda.");
}

// === LoRa komutları işleme ===
void handleLoRa() {
  String cmd = "";
  while (LoRa.available()) cmd += (char)LoRa.read();
  cmd.trim();

  if (cmd.startsWith("ADDWP")) {
    int i1 = cmd.indexOf(','), i2 = cmd.lastIndexOf(',');
    if (i1 > 0 && i2 > i1) {
      double lat = cmd.substring(i1 + 1, i2).toDouble();
      double lng = cmd.substring(i2 + 1).toDouble();
      if (wpTotal < 10) {
        waypoints[wpTotal++] = {lat, lng};
        LoRa.print("WP_OK");
      }
    }
  } else if (cmd == "CLEARWP") {
    wpTotal = 0;
    wpIndex = 0;
    LoRa.print("CLEARED");
  } else if (cmd == "EMERGENCY_LAND") {
    emergencyLanding = true;
    LoRa.print("EMG_OK");
  } else if (cmd == "STATUS") {
    String msg = "BAT:" + String(analogRead(VOLTAGE_PIN)) + ";LAT:" + String(gps.location.lat(), 6)
                + ";LNG:" + String(gps.location.lng(), 6);
    LoRa.print(msg);
  }
}
