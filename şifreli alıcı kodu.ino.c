#include <SPI.h>
#include <LoRa.h>
#include <AESLib.h>

AESLib aesLib;

byte aes_key[] = { 'S','u','p','e','r','S','e','c','r','e','t','1','2','3','4','!' };
char decrypted[128];

void setup() {
  Serial.begin(115200);
  LoRa.setPins(5, 14, 26);
  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa başlatılamadı!");
    while (1);
  }
  Serial.println("Şifreli Alıcı Hazır");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    byte encData[128];
    int i = 0;
    while (LoRa.available()) {
      encData[i++] = (byte)LoRa.read();
    }

    aesLib.decrypt((char*)encData, aes_key, decrypted);
    Serial.print("Çözülen Mesaj: ");
    Serial.println(decrypted);
  }
}