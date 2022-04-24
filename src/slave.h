#include <Arduino.h>
#include <SoftwareSerial.h>
#include <stdlib.h>
#include <time.h>
#include "functions.h"

const int TX_RX_PIN = 2;     // HIGH:TX y LOW:RX
const int my_node = 11; // identificador del esclavo
long current_time = 0;
bool verbose = true;

float voltage = 5;
float ampere = 3;
float temperature = 16;

SoftwareSerial Serial485(8, 9);
String cadena;

void setup()
{
  Serial485.begin(9600);
  Serial485.listen(); // Enables the selected software serial port to listen
  // Serial485.setTimeout(1000); // necessary ?
  pinMode(TX_RX_PIN, OUTPUT);
  digitalWrite(TX_RX_PIN, LOW); // RS485 como receptor
  
  Serial.begin(115200);
  Serial.print("Iniciando programa esclavo: \n\n");
  pinMode(LED_BUILTIN, OUTPUT);
}

void set_value(float &value) {
  // srand(time(NULL));
  randomSeed(analogRead(0));
  float variator = (rand() % 100 + 1) / 100.0; // 0.01 to 1.00
  Serial.print("variator: ");
  Serial.println(variator);

  if (rand() % 2 == 0) //0 or 1
    value -= variator;
  else
    value += variator;
}

void loop()
{
  current_time = millis();
  cadena = read_line_serial_485(Serial485);

  if (cadena != "") {
    if (verbose) { // ------ verbose mode ------
      Serial.print("Segundo ");
      Serial.println(current_time / 1000);
      Serial.println("|-- Recibiendo mensaje del master");
      Serial.print("|  |-- Trama recibida: ");
      Serial.println(cadena);
    }
    
    // ------ leemos el mensaje del master ------
    Trama package = str2trama(cadena);
    cadena = "";
    
    if (package.node == my_node) {
      if (verbose) { // ------ verbose mode ------
        Serial.println("|  |-- Nodo correcto (/)");
        Serial.print("|  |-- Funcion recibida: ");
        Serial.println(package.func);
      }

      package.is_answer = true;
      package.data = "";
      if (package.func == "RC") {
        digitalWrite(LED_BUILTIN, LOW);

      } else if (package.func == "VAT") {
        digitalWrite(LED_BUILTIN, HIGH);

        set_value(voltage);
        set_value(ampere);
        set_value(temperature);

        if (verbose) { // ------ verbose mode ------
          Serial.println("|  |  |-- Preparando datos ");
          Serial.print("|  |  |-- Voltaje: ");
          Serial.println(voltage);
          Serial.print("|  |  |-- Corriente: ");
          Serial.println(ampere);
          Serial.print("|  |  |-- Temperatura: ");
          Serial.println(temperature);
        }

        package.data = String(voltage) + ":" + String(ampere) + ":" + String(temperature);
      }

      if (verbose ) { // ------ verbose mode ------
        Serial.println("|");
        Serial.println("|-- Enviando respuesta al master");
      }

      // ------ enviamos la respuesta al master ------
      cadena = trama2str(package);
      digitalWrite(TX_RX_PIN, HIGH);
      Serial485.print(cadena);
      digitalWrite(TX_RX_PIN, LOW);
      
      if (verbose) {// ------ verbose mode ------
        Serial.print("|  |-- Trama enviada: ");
        Serial.println(cadena);
      }
      
    } else if (verbose) // ------ verbose mode ------
      Serial.println("|  |-- Nodo diferente (!)");
    
    if (verbose)
      Serial.print("\n");
    
    cadena = "";
  }
}