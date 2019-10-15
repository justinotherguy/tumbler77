void anlauf() {
  Serial.println ("anlauf");
  Serial.println (stepper.getDirection());
  for (int i=min; i<max; i++) {
    stepper.begin(i, MICROSTEPS);
    if (vorwaerts) {
      stepper.rotate(schritt); 
    } else {
      stepper.rotate(-schritt); 
    }
  }
}

void bremsen() {
  Serial.println ("bremsen");
  for (int i=max; i>min; i--) {
    stepper.begin(i, MICROSTEPS);
    if (vorwaerts) {
      stepper.rotate(schritt); 
    } else {
      stepper.rotate(-schritt); 
    }
  }
}

void Richtungswechsel() {
  if ((Restlaufzeit_Minuten-1) % 30 < 15) {
    if (vorwaerts) { // laeuft gerade vorwaerts, Richtung aendern!
      Serial.println("vorwaerts -> rueckwaerts");    
      bremsen();
      vorwaerts=false;
      anlauf();
    }
  } else {
    if (!vorwaerts) { // laeuft gerade rueckwaerts, Richtung aendern!
      Serial.println("rueckwaerts -> vorwaerts");    
      bremsen();
      vorwaerts=true;
      anlauf();
    }
  }
}

void TrommelStatusUpdate() {
  if (vorwaerts) { // Status aktualisieren
    if (Status < 3) {
      Status++;
    } else {
      Status = 0;
    }
  } else {
    if (Status > 0) {
      Status--;
    } else {
      Status = 3;
    }      
  }
}

void ZeitAktualisieren() {
  if (Restlaufzeit_Sekunden > 0 ) {
    Restlaufzeit_Sekunden--;
  } else {
    Restlaufzeit_Sekunden=59;
    if (Restlaufzeit_Minuten > 0 ) {
      Restlaufzeit_Minuten--;
      drawScreen();
    } else {
      Restlaufzeit_Minuten=59;
      if (Restlaufzeit_Stunden > 0 ) {
        Restlaufzeit_Stunden--;      
        drawScreen();
      }
    }
  }
}

void PlayPause() {
  starttime=millis();
  if (laeuft) {
    laeuft = false;
    bremsen();
  } else {
    laeuft = true;
    anlauf();
  }
}
