if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  Serial.println(F("OLED başlatılamadı!"));
  while (1);
}

display.clearDisplay();
display.setTextSize(1);
display.setTextColor(SSD1306_WHITE);
display.setCursor(0, 0);
display.println("LoRa Verici Hazir");
display.display();
delay(2000);