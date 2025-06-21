// ESP32 LoRa Verici - GPS + DHT11 + AES + Pil + OLED

#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>
#include <AESLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// PIN TANIMLARI
#define DHTPIN 4
#define DHTTYPE DHT11
#define BUTTON_PIN 12
#define BATTERY_PIN 35
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// NESNELER
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
DHT dht(DHTPIN, DHTTYPE);
AESLib aesLib;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// AES ANAHTARI
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                   0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// ŞİFRELEME
String encrypt(String msg) {
  char encrypted[128];
  aesLib.encrypt64(msg.c_str(), encrypted, aes_key, sizeof(aes_key));
  return String(encrypted);
}

// BATARYA ÖLÇÜMÜ
float readBatteryVoltage() {
  int raw = analogRead(BATTERY_PIN);
  float voltage = raw * (3.3 / 4095.0);
  return voltage * 2.0;
}

int batteryPercent(float voltage) {
  if (voltage >= 4.2) return 100;
  if (voltage >= 3.7) return map(voltage * 100, 370, 420, 0, 100);
  if (voltage >= 3.2) return map(voltage * 100, 320, 370, 0, 50);
  return 0;
}

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
  dht.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED baslatilamadi!"));
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("LoRa Verici Hazir");
  display.display();
  delay(2000);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa baslatilamadi!");
    while (1);
  }
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (digitalRead(BUTTON_PIN) == LOW && gps.location.isUpdated()) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    float voltage = readBatteryVoltage();
    int percent = batteryPercent(voltage);

    String message = "Lat:" + String(gps.location.lat(), 6) +
                     ",Lng:" + String(gps.location.lng(), 6) +
                     ",T:" + String(temp, 1) +
                     ",H:" + String(hum, 1) +
                     ",B:" + String(percent) + "%";

    String encrypted = encrypt(message);

    LoRa.beginPacket();
    LoRa.print(encrypted);
    LoRa.endPacket();

    Serial.println("Gonderildi (Sifreli): " + encrypted);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Veri Gonderildi");
    display.println(" ");
    display.print("Batarya: "); display.print(percent); display.println("%");
    display.print("Temp: "); display.print(temp); display.println(" C");
    display.print("Nem: "); display.print(hum); display.println(" %");
    display.display();

    delay(1500);
  }
}
