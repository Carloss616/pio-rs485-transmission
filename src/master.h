#include <Arduino.h>
#include <SoftwareSerial.h>
#include "functions.h"

const int TX_RX_PIN = 2; // HIGH:TX y LOW:RX

long current_time = 0;
long next_1 = 0;
long next_2 = 20000;

SoftwareSerial Serial485(8, 9);
String cadena;
Trama package;

void setup()
{
  Serial485.begin(9600);
  Serial485.listen(); // Enables the selected software serial port to listen
  // Serial485.setTimeout(1000); // necessary ?
  pinMode(TX_RX_PIN, OUTPUT);
  digitalWrite(TX_RX_PIN, HIGH); // RS485 como transmisor

  Serial.begin(115200);
  Serial.print("Iniciando programa master: \n\n");
  package.node = 11;
}

void loop()
{
  current_time = millis(); 
  package.node = 11;
  package.func = "RC"; // Rall Call
  send_trama(Serial485, TX_RX_PIN, package, current_time, next_1, 1500, true);

  current_time = millis();
  package.node = 12;
  package.func = "VAT"; // Voltage - Ampere - Temperature
  send_trama(Serial485, TX_RX_PIN, package, current_time, next_2, 3000, true);

}