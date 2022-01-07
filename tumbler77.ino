// tumbler77
/*
 * Libs: 
 *   StepperDriver by Laurentiu Badea
 *   PubSubClient by Nick O'Leary
 * 
 * Stepper-Pins:
 *   D3: Step
 *   D4: Dir
 *   
 *  Helpful resources:
 *    wiring A4988:
 *      https://www.instructables.com/id/DIY-Arduino-controlled-Egg-Bot/
 *      https://www.pololu.com/file/0J450/A4988.pdf
 *      https://forum.allaboutcircuits.com/attachments/wemos-nema-driver-pdf.128982/ 
 *    source stepper driver:
 *      https://github.com/laurb9/StepperDriver/blob/master/examples/BasicStepperDriver/BasicStepperDriver.ino
 *      https://www.makerguides.com/a4988-stepper-motor-driver-arduino-tutorial/
 */

//#define debug=true

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

#include "settings.h"
#include "BasicStepperDriver.h"

#define MOTOR_STEPS 200 // Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define RPM 120         // rotation speed
#define steps 10        

#define MICROSTEPS 16   // Microstepping mode. If you hardwired it to save pins, set to the same value here.

#define DIR D4  
#define STEP D3 


int remaining_hours=24;
int remaining_minutes=0;
int remaining_seconds=0;

char remaining_hours_char[10];
char remaining_minutes_char[10];
char remaining_seconds_char[10];
char remaining_time[32];

bool tumbling = false;
bool forward = true;
bool done = false;

unsigned long time_change_dir;
unsigned long time_update;
int interval_change=1800; // Intervall, nach dem die Richtung gewechselt wird (in Sekunden)

BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);

void setup() {
  Serial.begin(115200);
  Serial.println();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  stepper.begin(RPM, MICROSTEPS);
  tumbling=true;
  accelerate();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  if ((millis() - time_update > 1000) and tumbling) { // 1 s passed
    update_time();
    time_update=millis();
  }

  // Motor
  if ((remaining_hours > 0 or remaining_minutes > 0 or remaining_seconds > 0) and tumbling) { // time remaining > 0
    if (tumbling and millis() - time_change_dir > interval_change*1000) {
      change_dir();
      time_change_dir=millis();
    }
    if (forward) {
      stepper.move(steps);
    } else {
      stepper.move(-steps);
    }
  } else {
      tumbling = false;
      done = true;
      Serial.println("finished!");
  }

}
