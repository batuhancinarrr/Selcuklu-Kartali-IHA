display.clearDisplay();
display.setCursor(0, 0);

display.print("Lat: ");
display.println(gps.location.lat(), 6);
display.print("Lng: ");
display.println(gps.location.lng(), 6);

display.print("Sicaklik: ");
display.print(temp, 1);
display.println("C");

display.print("Nem: ");
display.print(hum, 1);
display.println("%");

display.print("Batarya: ");
display.print(percent);
display.println("%");

display.display();