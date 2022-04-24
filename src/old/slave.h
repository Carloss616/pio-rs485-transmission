#include <Arduino.h>
#include <SoftwareSerial.h>
#include "functions.h"

const int TX_RX_PIN = 2;     // HIGH:TX y LOW:RX
const int my_direction = 10; // identificador del esclavo
float current_time = 0;
SoftwareSerial Serial485(8, 9);
String cadena;
String answer;
// Trama package;
// Trama response;

void setup()
{
  Serial485.begin(9600);
  Serial485.listen(); // Enables the selected software serial port to listen
  Serial485.setTimeout(1000); // necessary ?
  pinMode(TX_RX_PIN, OUTPUT);
  digitalWrite(TX_RX_PIN, LOW); // RS485 como receptor
  
  Serial.begin(115200);
  Serial.print("Iniciando programa esclavo: \n\n");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  current_time = millis();
  cadena = read_line_serial_485(Serial485);

  if (cadena != "")
  {
    // ------ verbose mode ------
    Serial.print("Segundo ");
    Serial.println(current_time / 1000);
    Serial.println("|-- Recibiendo mensaje del master...");

    // ------ leemos el mensaje del master ------
    Serial.print("|  |-- Trama recibida: ");
    Serial.println(cadena);
    
    Trama package = str2trama(cadena);
    cadena = "";
    
    if (package.node == my_direction) {
      // ------ verbose mode ------
      Serial.print("|  |-- Funcion recibida: ");
      Serial.println(package.func);
      
      Trama response = package;
      response.is_answer = true;
      if (package.func == "ON") {
        digitalWrite(LED_BUILTIN, HIGH);
        response.data = "LED encendido";

      } else if (package.func == "OFF") {
        digitalWrite(LED_BUILTIN, LOW);
        response.data = "LED apagado";

      } else
        response.data = "No se ejecutaron acciones";

      // ------ verbose mode ------
      if (package.data != "") {
        Serial.print("|  |-- Data recibido: ");
        Serial.println(package.data);
      }
      Serial.println("|");
      Serial.println("|-- Enviando respuesta al master...");

      // ------ enviamos la respuesta al master ------
      answer = trama2str(response);
      digitalWrite(TX_RX_PIN, HIGH);
      Serial485.print(answer);
      digitalWrite(TX_RX_PIN, LOW);
      
      // ------ verbose mode ------
      Serial.print("|  |-- Trama enviada: ");
      Serial.println(answer);
      
    } else // ------ verbose mode ------
      Serial.println("|  |-- Direccion diferente!");
    
    Serial.print("\n");
  }
}