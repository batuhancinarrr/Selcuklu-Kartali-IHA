#include "esp_camera.h"
#include <LoRa.h>

#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO0 26

void setup() {
  Serial.begin(115200);

  // Kamera yap�land�rmas�
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Kamera ba�lat�lamad�");
    while(1);
  }

  // LoRa ba�latma
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa ba�lat�lamad�");
    while(1);
  }
  Serial.println("LoRa ba�lad�");
}

void loop() {
  // Foto�raf �ek
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Foto�raf al�namad�");
    return;
  }

  // Foto�raf� base64 stringe �evir
  String imgBase64 = "";
  for (size_t i = 0; i < fb->len; i++) {
    imgBase64 += (char)fb->buf[i];
  }
  esp_camera_fb_return(fb);

  // �ok b�y�k ise LoRa paket boyutuna dikkat! Burada snapshot'u k���k tut
  // K�saca sadece k���k snapshot �rne�i
  String packet = "IMG:" + imgBase64;

  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();

  Serial.println("Snapshot g�nderildi");

  delay(10000);
}
