#include <ESP8266WiFi.h>
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
#ifndef STASSID
//#define STASSID "OnePlus 7"
//#define STAPSK  "ESP-8266-2"
#define STASSID "Lion"
#define STAPSK  "Shawn249"

#endif

const char* ssid     = STASSID;
const char* password = STAPSK;
const char* host = "djxmmx.net";
const uint16_t port = 17;
HX711 scale;
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  pinMode(ThermistorPin, INPUT);
  pinMode(LOADCELL_SCK_PIN, OUTPUT);
  pinMode(LOADCELL_DOUT_PIN, INPUT);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void loop() {
   WiFiClient client;
   while (client.available() == 0) {
      if (millis() - timeout > 5000) {
       Serial.println(">>> Client Timeout !");
       client.stop();
       delay(60000);
       return;
   }
   delay(200);
   if (scale.is_ready()) {
   long reading = scale.read();
   Serial.print("HX711 reading: ");
   Serial.println(reading);
    
   } 
  else {
    Serial.println("HX711 not found.");
  } 
  
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15; 
  Serial.print("Temperature: "); 
  Serial.print(Tc);
  Serial.println(" C");  
  }
}
