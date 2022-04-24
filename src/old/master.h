#include <Arduino.h>
#include <SoftwareSerial.h>
#include "functions.h"

// const long freq_tx_rx = 1500;
// long next_tx_rx = freq_tx_rx;

const int TX_RX_PIN = 2; // HIGH:TX y LOW:RX
float current_time = 0;
SoftwareSerial Serial485(8, 9);
String message;
String cadena;
String answer;
Trama package;
// Trama response;

void setup()
{
  Serial485.begin(9600);
  Serial485.listen(); // Enables the selected software serial port to listen
  Serial485.setTimeout(5000); // necessary ?
  pinMode(TX_RX_PIN, OUTPUT);
  digitalWrite(TX_RX_PIN, HIGH); // RS485 como transmisor

  Serial.begin(115200);
  Serial.print("Iniciando programa master: \n\n");
}

void loop()
{
  current_time = millis();
  message = read_serial_monitor();

  // if (current_time >= next_tx_rx)
  // {
  if (message != "")
  {
    // ------ verbose mode ------
    Serial.print("Segundo ");
    Serial.println(current_time / 1000);
    Serial.println("|-- Enviando mensaje al esclavo...");

    // ------ enviamos el mensaje al esclavo------
    package.node = 10;      // dirección del esclavo
    package.func = message;  // funcion
    // package.data = message; // data

    cadena = trama2str(package);
    Serial485.print(cadena);

    // ------ verbose mode ------
    Serial.print("|  |-- Trama enviada: ");
    Serial.println(cadena);
    message = "";             // limpiamos la variable

    // ------ verbose mode ------
    Serial.println("|");
    Serial.println("|-- Buscando respuesta del esclavo...");

    // ------ leemos la respuesta del esclavo ------
    digitalWrite(TX_RX_PIN, LOW); // RS485 como receptor
    if (Serial485.find(package.ans)) { // RN10:OK;
      answer = Serial485.readString(); //N10:OK;
      Trama response = str2trama(answer);
      response.is_answer = true;
      Serial.println(answer);

      if (response.node == package.node) { // falta validar si el mensaje (func) es correcto
        // ------ verbose mode ------
        Serial.println("|  |-- La respuesta es valida");
        if (response.data != "") {
          Serial.print("|  |  |-- Trama recibida: ");
          Serial.println(answer);
        }
      
      } else // ------ verbose mode ------
        Serial.println("|  |-- La respuesta es invalida (nodo equivocado)");

    } else // ------ verbose mode ------
      Serial.println("|  |-- No se encontró respuesta");
    
    digitalWrite(TX_RX_PIN, HIGH); // RS485 como transmisor
    Serial.print("\n");
  }

    // next_tx_rx += freq_tx_rx;
  // }
}