// tumbler77
/*
 * was fehlt noch:
 * - WLAN
 * - MQTT-Anbindung
 * x Richtungswechsel
 * - Motor disable
 * - aktuelle Werte (Restlaufzeit, Drehzahl) per MQTT melden
 * - Kommandos per MQTT abfragen
 * - Display-Refresh von Motordrehen entkoppeln
 * 
 * Libs: StepperDriver by Laurentiu Badea v1.1.4
 * 
 * Display-Pins:
 *   D0: TS-CS
 *   D1: TFT-RST
 *   D2: TFT-LED
 *   D5: SCK
 *   D6: MISO
 *   D7: MOSI
 *   D8: TFT_DC
 *   
 * Stepper-Pins:
 *   D3: Step
 *   D4: Dir
 *   A0: En
 *   
 *   
 *  Helpful resources:
 *    wiring A4988:
 *      https://www.instructables.com/id/DIY-Arduino-controlled-Egg-Bot/
 *      https://www.pololu.com/file/0J450/A4988.pdf
 *      https://forum.allaboutcircuits.com/attachments/wemos-nema-driver-pdf.128982/ 
 *    source stepper driver:
 *      https://github.com/laurb9/StepperDriver/blob/master/examples/AccelTest/AccelTest.ino
 *      https://github.com/laurb9/StepperDriver/blob/master/examples/BasicStepperDriver/BasicStepperDriver.ino
 *    wiring TFT:
 *      https://wiki.wemos.cc/_media/products:d1_mini_shields:tft_2.4_v1.0.0_2_16x9.jpg
 *      https://wiki.wemos.cc/_media/products:d1_mini_shields:sch_tft2.4_v1.0.0.pdf
 */

// Menue
//
// Hauptbildschirm                                // Menu="0"
// +-+--> Einstellungen   -> rechts unten         // Menu="1"
//   +----> Restlaufzeit      -> oben             // Menu="11"
//   +----> Drehzahl          -> unten            // Menu="12"
//   +----> zurueck           -> rechts unten     

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <XPT2046_Touchscreen.h>

#include "BasicStepperDriver.h"

#define TFT_CS D0  //for D1 mini or TFT I2C Connector Shield (V1.1.0 or later)
#define TFT_DC D8  //for D1 mini or TFT I2C Connector Shield (V1.1.0 or later)
#define TFT_RST -1 //for D1 mini or TFT I2C Connector Shield (V1.1.0 or later)
#define TS_CS D3   //for D1 mini or TFT I2C Connector Shield (V1.1.0 or later)

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200

// Microstepping mode. If you hardwired it to save pins, set to the same value here.
#define MICROSTEPS 16

#define DIR D1 // D1 mini: D2
#define STEP D2 // wemos D1 mini: D3 // Arduino Uno: 9
//#define SLEEP 13 // optional (just delete SLEEP from everywhere if not used)

#define min 20   // Startdrehzahl beim Anlauf
#define max 250 // 30 // 250  // Drehzahl beim Lauf
int Drehzahl=max;
#define schritt 10

#define Offset_Kreis_X 290
#define Offset_Kreis_Y 20
#define Radius_Kreis 15

int Restlaufzeit_Stunden=6;
int Restlaufzeit_Minuten=0;
int Restlaufzeit_Sekunden=0;

bool laeuft = false;
bool vorwaerts = true;
bool fertig = false;

int MenuePos = 0;
int MenuePos_alt = -1;
int Status = 0; // fuer Fortschrittsanzeige

unsigned long starttime;
unsigned long starttime_temp;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

XPT2046_Touchscreen ts(TS_CS);

BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);

void setup() {
  Serial.begin(115200);
  ts.begin();
  ts.setRotation(1);

  tft.begin();
  tft.setRotation(3);
  while (!Serial && (millis() <= 1000))
    ;
  drawScreen();

  // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
  // stepper.setEnableActiveState(LOW);
  stepper.enable();
}

void loop() {
  // Zeit
  if ((millis() - starttime > 1000) and laeuft) { // 1 s vorbei
  //if ((millis() - starttime > 10000) and laeuft) { // 1 s vorbei
Serial.println("1s um");
    TrommelStatusUpdate();
//    drawTrommel(); // temp - Update der Status-Anzeige mal noch rausgenommen
    ZeitAktualisieren();
    starttime=millis();
  }

  // Motor
  if (Restlaufzeit_Stunden > 0 or Restlaufzeit_Minuten > 0 or Restlaufzeit_Sekunden > 0) { // Restlaufzeit > 0
    if (laeuft) {

      Richtungswechsel();
      if (vorwaerts) {
        stepper.rotate(schritt); 
      } else {
        stepper.rotate(-schritt); 
      }
    }
  } else { //Ende
    if (laeuft) {
      tft.setCursor(0, 150);
      tft.setTextSize(3);
      tft.setFont(&FreeSans24pt7b);
      tft.fillScreen(ILI9341_BLACK);
      tft.printf("fertig");  
      laeuft = false;
      fertig = true;
      bremsen();
    }
  }

  // Touch?
  if (ts.touched()) {
    if (fertig) { // -> Reset
      Restlaufzeit_Stunden=6;
      Restlaufzeit_Minuten=0;
      Restlaufzeit_Sekunden=0;
      fertig=false;
    }
    if (MenuePos==0) {
      TS_Point p = ts.getPoint();
      if (p.y<1000 and p.x>3300) {  // Setup
        MenuePos=1;                 // Menue: Setup
        while (ts.touched())        // warten, bis wieder losgelassen
          ;
      } else {
        if (p.y<1000 and p.x<800) { // Play/Pause
          while (ts.touched())      // warten, bis wieder losgelassen
            ;
          PlayPause();
          drawPlayPause();
        }
      }
    }
  } else if (MenuePos!=0) {
    Einstellungen();
  }
}

void Einstellungen() {
  drawScreen();
  if (laeuft) {
    laeuft=false;
    bremsen();
  }
  if ((millis() - starttime_temp > 1000)) { // 1 s vorbei
Serial.println("1s um");
    starttime_temp=millis();
  }
  
  TS_Point p = ts.getPoint();
  if (ts.touched()) {
    switch (MenuePos) {
      case 0:   // Hauptbildschirm
Serial.println("zurueck-2");
        tft.setTextColor(ILI9341_WHITE);
        drawHauptbildschirm();
        break;
      case 1:   // Setup - Selektor: was wollen wir einstellen?
Serial.println("zu 1");
        tft.setTextColor(ILI9341_GREEN);
        drawMenuSelector();
        if (p.y<1000 and p.x>3300) {  // zurueck zum Hauptbildschirm
          MenuePos = 0;
          while (ts.touched())        // warten, bis wieder losgelassen
            ;
        } else if (p.y>2000) {             // Restlaufzeit einstellen
          MenuePos=  11;
    Serial.println("Restlaufzeit einstellen");
        } else {                    // Drehzahl einstellen
          MenuePos = 12;
    Serial.println("Drehzahl einstellen");
        } 
        break;
      case 11:  // Setup - Restlaufzeit einstellen
Serial.println("zu 11");
        tft.setTextColor(ILI9341_GREEN);
        drawMenuRestlaufzeit();
        if (p.y<1000 and p.x>3300) {  // zurueck zum Hauptbildschirm
          MenuePos = 0;
          while (ts.touched())        // warten, bis wieder losgelassen
            ;
        } else 
        
        if (p.y>2000) {             // 
          MenuePos=  11;
Serial.println("Restlaufzeit einstellen");
          if (p.y>2000) {             // hoch
            if (p.x<2000) {           // Stunde  
              if (Restlaufzeit_Stunden < 23) Restlaufzeit_Stunden++;
            } else if (Restlaufzeit_Minuten==59 and Restlaufzeit_Stunden < 23) { // Minute
              Restlaufzeit_Minuten=0;
              Restlaufzeit_Stunden++;
            } else if (Restlaufzeit_Minuten<59) Restlaufzeit_Minuten++;
          } else {                    // runter
            if (p.x<2000) {           // Stunde  
              if (Restlaufzeit_Stunden > 0) Restlaufzeit_Stunden--;
            } else {                  // Minute
              if (Restlaufzeit_Minuten==0 and Restlaufzeit_Stunden>0) {
                Restlaufzeit_Minuten=59;
                Restlaufzeit_Stunden--;
              } else if (Restlaufzeit_Minuten > 0) Restlaufzeit_Minuten--;
            }
          }        
        } else {                    // Drehzahl einstellen
          MenuePos = 12;
    Serial.println("Drehzahl einstellen");
        } 
        
        
        break;
      case 12:  // Setup - Drehzahl einstellen
Serial.println("zu 12");
        tft.setTextColor(ILI9341_GREEN);
        drawMenuDrehzahl();
        break;
      default: 
        Serial.println("Fehler!");
        ;;
    }
    while (ts.touched())      // warten, bis wieder losgelassen
    ;
  
/*        if (DrehzahlSelector){      // Drehzahl einstellen
Serial.println("Drehzahl einstellen");
          if (p.y>2000) {           // hoch
            Drehzahl=Drehzahl+10;
Serial.println("hoch");
          } else {                  // runter
             if (Drehzahl>9) {
               Drehzahl=Drehzahl-10;
Serial.println("runter");
             }
          }
        */
  }
//  while (ts.touched())        // warten, bis wieder losgelassen
//    ;
  delay(1); // wenn das weg ist, hängt sich das Programm im Setup auf
}
