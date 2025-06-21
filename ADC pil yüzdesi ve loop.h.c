#define BATTERY_PIN 35

float readBatteryVoltage() {
  int raw = analogRead(BATTERY_PIN);
  float voltage = raw * (3.3 / 4095.0); // ADC dönüşümü
  return voltage * 2.0; // direnç bölücü düzeltme
}

int batteryPercent(float voltage) {
  // Tipik 1S LiPo için yaklaşık değerler
  if (voltage >= 4.2) return 100;
  if (voltage >= 3.7) return map(voltage * 100, 370, 420, 0, 100);
  if (voltage >= 3.2) return map(voltage * 100, 320, 370, 0, 50);
  return 0;
}

loop

float voltage = readBatteryVoltage();
int percent = batteryPercent(voltage);
message += ", Batarya: " + String(percent) + "%";