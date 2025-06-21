#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <AESLib.h>

// Pin Tanımları
#define DHTPIN 4
#define DHTTYPE DHT11
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2
#define GPS_RX 16
#define GPS_TX 17
#define BUTTON_PIN 12
#define BATTERY_PIN 35

DHT dht(DHTPIN, DHTTYPE);
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
AESLib aesLib;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// AES Anahtarı
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                   0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

String encrypt(String msg) {
  char encrypted[128];
  aesLib.encrypt64(msg.c_str(), msg.length(), encrypted, aes_key, sizeof(aes_key));
  return String(encrypted);
}

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  dht.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  analogReadResolution(12);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(868E6)) {
    display.println("LoRa Fail");
    display.display();
    while (1);
  }
}

void loop() {
  while (gpsSerial.available()) gps.encode(gpsSerial.read());

  if (gps.location.isUpdated()) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    float bat = analogRead(BATTERY_PIN) * 2 * 3.3 / 4095;

    String data = String(gps.location.lat(), 6) + "," +
                  String(gps.location.lng(), 6) + "," +
                  String(temp, 1) + "," +
                  String(hum, 1) + "," +
                  String(bat, 2);

    String encrypted = encrypt(data);

    LoRa.beginPacket();
    LoRa.print(encrypted);
    LoRa.endPacket();

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Gonderildi:");
    display.println(data);
    display.display();

    delay(5000);
  }
}