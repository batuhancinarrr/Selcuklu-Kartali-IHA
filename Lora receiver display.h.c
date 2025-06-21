// ESP32 LoRa Alıcı - AES Çözme + OLED

#include <SPI.h> #include <LoRa.h> #include <AESLib.h> #include <Adafruit_GFX.h> #include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 #define SCREEN_HEIGHT 64 #define OLED_RESET -1 #define LORA_SS 5 #define LORA_RST 14 #define LORA_DIO0 2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); AESLib aesLib;

// Şifreleme anahtarı vericiyle aynı olmalı byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

String decrypt(String input) { char decrypted[128]; aesLib.decrypt64(input.c_str(), decrypted, aes_key, sizeof(aes_key)); return String(decrypted); }

void setup() { Serial.begin(115200); if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { Serial.println(F("OLED baslatilamadi")); while (1); }

display.clearDisplay(); display.setTextSize(1); display.setTextColor(SSD1306_WHITE); display.setCursor(0, 0); display.println("LoRa Alici Hazir"); display.display(); delay(2000);

LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0); if (!LoRa.begin(868E6)) { Serial.println("LoRa baslatilamadi!"); while (1); } }

void loop() { int packetSize = LoRa.parsePacket(); if (packetSize) { String encrypted = ""; while (LoRa.available()) { encrypted += (char)LoRa.read(); }

String decrypted = decrypt(encrypted);

Serial.println("Sifreli: " + encrypted);
Serial.println("Cozulen: " + decrypted);

display.clearDisplay();
display.setCursor(0, 0);
display.println("Veri Alindi:");

// Parselere ayır
int index;
index = decrypted.indexOf(",");
String lat = decrypted.substring(0, index);
decrypted.remove(0, index + 1);

index = decrypted.indexOf(",");
String lng = decrypted.substring(0, index);
decrypted.remove(0, index + 1);

index = decrypted.indexOf(",");
String temp = decrypted.substring(0, index);
decrypted.remove(0, index + 1);

index = decrypted.indexOf(",");
String hum = decrypted.substring(0, index);
decrypted.remove(0, index + 1);

String batt = decrypted;

display.println(lat);
display.println(lng);
display.println(temp);
display.println(hum);
display.println(batt);
display.display();

} }
