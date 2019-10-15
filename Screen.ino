void drawScreen() {
  if (MenuePos_alt != MenuePos) tft.fillScreen(ILI9341_BLACK);
  
  switch (MenuePos) {
    case 0:   // Hauptbildschirm
  Serial.println("0");
      tft.setTextColor(ILI9341_WHITE);
      if (MenuePos_alt != MenuePos) drawHauptbildschirm();
      break;
    case 1:   // Setup - Selektor: was wollen wir einstellen?
//  Serial.println("1");
      tft.setTextColor(ILI9341_GREEN);
      if (MenuePos_alt != MenuePos) drawMenuSelector();
      break;
    case 11:  // Setup - Restlaufzeit einstellen
//  Serial.println("11");
      tft.setTextColor(ILI9341_GREEN);
      if (MenuePos_alt != MenuePos) drawMenuRestlaufzeit();
      break;
    case 12:  // Setup - Drehzahl einstellen
  Serial.println("12");
      tft.setTextColor(ILI9341_GREEN);
      if (MenuePos_alt != MenuePos) drawMenuDrehzahl();
      break;
    default: 
      Serial.println("Fehler!");
      ;;
  }
  MenuePos_alt = MenuePos;
}

void drawHauptbildschirm() {
  tft.setCursor(0, 20);
  tft.setTextSize(1);
  tft.setFont(&FreeSans12pt7b);
  tft.println("Restlaufzeit [hh:mm]: ");

  tft.setCursor(0, 150);
  if (Restlaufzeit_Stunden > 9) {
    tft.setTextSize(2);
  } else {
    tft.setTextSize(3);
  }
  tft.setFont(&FreeSans24pt7b);
  tft.printf("%01d:%02d", Restlaufzeit_Stunden, Restlaufzeit_Minuten);  

  tft.fillRect(285, 205, 30, 6, ILI9341_GREEN); // Setup-Button
  tft.fillRect(285, 215, 30, 6, ILI9341_GREEN);  
  tft.fillRect(285, 225, 30, 6, ILI9341_GREEN);  

  drawPlayPause();
  // temp - Update der Status-Anzeige mal noch rausgenommen
  //  drawTrommel();
}

void drawMenuSelector() {
  tft.setCursor(0, 20);
  tft.setTextSize(1);
  tft.setFont(&FreeSans12pt7b);
  tft.println("Auswahl Setup: ");
  tft.setCursor(0, 100);
  tft.setTextSize(1);  
  tft.setFont(&FreeSans24pt7b);
  tft.printf("Restlaufzeit");
  tft.setCursor(0, 180);
  tft.setTextSize(1);  
  tft.setFont(&FreeSans24pt7b);
  tft.printf("Drehzahl");
}

void drawMenuRestlaufzeit() {
  tft.setCursor(0, 20);
  tft.setTextSize(1);
  tft.println("Setup Restlaufzeit [hh:mm]: ");
  tft.setCursor(0, 150);
  if (Restlaufzeit_Stunden > 9) {
    tft.setTextSize(2);
  } else {
    tft.setTextSize(3);
  }
  tft.setFont(&FreeSans24pt7b);
  tft.printf("%01d:%02d", Restlaufzeit_Stunden, Restlaufzeit_Minuten);    
  tft.fillCircle(330, 250, 60, ILI9341_WHITE);    
}

void drawMenuDrehzahl() {
  tft.setCursor(0, 20);
  tft.setTextSize(1);
  tft.println("Setup Drehzahl [U/min]: ");
  tft.setCursor(0, 150);
  tft.setTextSize(3);
  tft.setFont(&FreeSans24pt7b);
  tft.printf("%01d", Drehzahl);
  tft.fillCircle(330, 250, 60, ILI9341_WHITE);    
}

void drawPlayPause() {
  tft.fillRect(10, 200, 30, 30, ILI9341_BLACK); // loeschen
  if (laeuft) {
    tft.fillRect(10, 200, 10, 30, ILI9341_GREEN); // Pause-Anzeige
    tft.fillRect(30, 200, 10, 30, ILI9341_GREEN);  
  } else {
    tft.fillTriangle(10, 200, 10, 230, 30, 215, ILI9341_GREEN); // Play-Anzeige    
  }
}

void drawTrommel() { // stilisierte Bewegung der Trommel
//  tft.fillCircle(Offset_Kreis_X, Offset_Kreis_Y, Radius_Kreis, ILI9341_WHITE);
  tft.drawCircle(Offset_Kreis_X, Offset_Kreis_Y, Radius_Kreis, ILI9341_WHITE);
  switch (Status) {
    case 0: tft.fillRect(Offset_Kreis_X-Radius_Kreis, Offset_Kreis_Y-Radius_Kreis, Radius_Kreis+1, Radius_Kreis+1, ILI9341_BLACK);
      break;
    case 1: tft.fillRect(Offset_Kreis_X, Offset_Kreis_Y-Radius_Kreis, Radius_Kreis+1, Radius_Kreis+1, ILI9341_BLACK);
      break;
    case 2: tft.fillRect(Offset_Kreis_X, Offset_Kreis_Y, Radius_Kreis+1, Radius_Kreis+1, ILI9341_BLACK);
      break;
    case 3: tft.drawRect(Offset_Kreis_X-Radius_Kreis, Offset_Kreis_Y, Radius_Kreis+1, Radius_Kreis+1, ILI9341_BLACK);
      break;
    default: 
      Serial.println("Fehler!");
      ;;
  }  
}
