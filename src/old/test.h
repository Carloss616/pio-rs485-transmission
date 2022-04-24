#include <Arduino.h>
#include "functions.h"

String message;

void setup() {
  Serial.begin(115200);
  Serial.print("Iniciando programa \n\n");
}

void loop() {
  message = read_serial_monitor();
  if (message != "") {
    // ========= String to Trama =========
    Trama trama;
    trama.is_answer = true;
    trama.node = 13;
    trama.func = message;
    trama.data = "Comentario";

    Serial.print("Trama: ");
    Serial.println(trama2str(trama));


    // ========= String to Trama =========
    // Trama trama = str2trama(message);

    // Serial.print("Mensaje: ");
    // Serial.println(message);
    // message = "";

    // Serial.println("Trama: ");
    // Serial.print("|-- is_answer: ");
    // Serial.println(trama.is_answer);
    // Serial.print("|-- pre: ");
    // Serial.println(trama.pre);
    // Serial.print("|-- sep: ");
    // Serial.println(trama.sep);
    // Serial.print("|-- end: ");
    // Serial.println(trama.end);
    // Serial.print("|-- node: ");
    // Serial.println(trama.node);
    // Serial.print("|-- func: ");
    // Serial.println(trama.func);
    // Serial.print("|-- data: ");
    // Serial.println(trama.data);
    // Serial.print("\n");
  }
}