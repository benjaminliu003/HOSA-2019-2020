#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <HX711.h>
#include "pitches.h"

//------------------------- Variables used for thermistor--------
int ThermistorPin = A0;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
//------------------------------ Variables used for load cell---------
const int LOADCELL_DOUT_PIN = D0;
//The pin that outputs the square signal of the clock
const int LOADCELL_SCK_PIN = D1;
long val = 0;
// Stores the current tempurature when the button is pressed
int tempnow;
// Stores the current pressure when the button is pressed, used as the benchmark for healthy tissue without swelling
int pressurenow;
//-------------------------------------------------------------------

HX711 scale;

WidgetTerminal terminal(V2);

/*
   The unique authentication ID for the user's project and device ID
*/
char auth[] = "";

/*
   Network SSIDs and Passcodes are stored here.
*/
char ssid[] = "";
char pass[] = "";

//At School:
//char ssid[] = "";
//char pass[] = "";

/*
   This function sends and receives data to anbd from virtual pin 2, the terminal on the Blynk UI.

   SET TO USE FOR TEMP AND DIAMETER SETTINGS

   On the terminal assigned to Virtual Pin 2, the user will see the output written to it.
*/
BLYNK_WRITE(V2) {
  String i = param.asStr();
  Serial.print("V2 Text is: ");
  Serial.println(i);

  delay(2500);

  Serial.println("Initializing loop");
  Serial.println("Detected no input");
  /*
    Serial.println(SUART.peek());
    if (SUART.available()) {
    Serial.println("Detected input");
    SUART.readBytesUntil('`', received, 10);
    Serial.println("Input is not NULL");
    Serial.println(received);

    if (received[0] == 'p') {
      Serial.println("Received Positive");
      terminal.println(" ");
      Blynk.virtualWrite(V2, "Password Changed to: ");
      Blynk.virtualWrite(V2, i);
      delay(600);
      char received[10] = "";
      Blynk.notify("Password has been changed to: " + i);
      Blynk.email("Password Changed", "Your password has been changed to: " + i);
    }
    }*/
}

/*
   Change Virtual Pin 0 to a reset button function
*/

BLYNK_WRITE(V0) {
  int p = param.asInt();
  if (p == 1) {
    terminal.clear();
    Serial.println("locked");
    Blynk.virtualWrite(V2, "Your safe has been remotely locked.");
    Blynk.notify("Your safe has been remotely locked");
    Blynk.email("Remote Lock", "Your safe has been remotely locked");
  } else {
    terminal.clear();
    Serial.println("unlocked");
    Blynk.virtualWrite(V2, "Your safe has been remotely unlocked.");
  }
}

/*
   In the setup, serial communication is started and the software serial communication with the arduno Mega is started as well.

   Blynk then connects to the specified WiFi network and the terminal prints out instructions for the user.
*/

void setup()
{
  // Debug console
  Serial.begin(9600);
  Serial.flush();

  Blynk.begin(auth, ssid, pass);

  terminal.clear();
  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.println(F("-------------"));
  terminal.println("HOSA TEST APPLICATION Ver.1 Initialized");
  terminal.println("Includes Load Cell and Thermosistor functions.");
  terminal.println("Diameter and temperature can be set in application");//--------------------------------MAKE THIS

  pinMode(ThermistorPin, INPUT);
  pinMode(LOADCELL_SCK_PIN, OUTPUT);
  pinMode(LOADCELL_DOUT_PIN, INPUT);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void inflamed(int x) {
  if (x == 1) {
    Blynk.virtualWrite(V2, "Inflammation Detected");
    Blynk.notify("Inflammation detected");

    Vo = analogRead(ThermistorPin);
    R2 = R1 * (1023.0 / (float)Vo - 1.0);
    logR2 = log(R2);
    T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
    Tc = T - 273.15;

    if (Tc > 37.5) {
      Blynk.virtualWrite(V2, "Inflammation Detected");
      Blynk.notify("Inflammation detected");
    } else{
      Blynk.virtualWrite(V2, "Diameter indicates potential inflammation.");
      Blynk.notify("Diameter indicates potential inflammation.");
    }
  }

  if (x == 0){
    Blynk.virtualWrite(V2, "Temperature indicates potential inflammation.");
    Blynk.notify("Temperature indicates potential inflammation.");
  }
}

void loop()
{
  Blynk.run();
  if (scale.is_ready()) {
    long reading = scale.read();
    Serial.print("HX711 reading: ");
    Serial.println(reading);
    if (reading > 2000) {
      inflamed(1);
    }
  }
  else {
    Serial.println("HX711 not found.");
    delay(2500);
    setup();
  }

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 273.15;
  Serial.print("Temperature: ");
  Serial.print(Tc);
  Blynk.virtualWrite(V1, Tc);
  Serial.println(" C");

  if (Tc > 37.5) {
    inflamed(0);
  }
}
