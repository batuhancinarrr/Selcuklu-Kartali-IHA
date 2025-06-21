#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <AESLib.h>

// Pin Tanımları
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
AESLib aesLib;

// AES Anahtarı (vericiyle aynı)
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                   0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

String decrypt(String input) {
  char decrypted[128];
  aesLib.decrypt64(input.c_str(), input.length(), decrypted, aes_key, sizeof(aes_key));
  return String(decrypted);
}

void setup() {
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("LoRa Alıcı Hazır");
  display.display();

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(868E6)) {
    display.println("LoRa Başlatma Hatası");
    display.display();
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String encryptedData = "";
    while (LoRa.available()) {
      encryptedData += (char)LoRa.read();
    }

    String decrypted = decrypt(encryptedData);
    Serial.println("Çözülmüş Veri: " + decrypted);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Veri Geldi:");
    display.println(decrypted);
    display.display();
  }
}