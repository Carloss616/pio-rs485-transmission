#include <Arduino.h>
#include <SoftwareSerial.h>

struct Trama
{
  bool is_answer = false; // modo respuesta
  const char ans = 'R'; // código de respuesta
  const char pre = 'N'; // inicio de trama
  const char sep = ':';   // separador
  const char end = ';';   // final de trama
  int node;         // dirección del esclavo
  String func;      // funcion-pregunta
  String data;      // data
};

String trama2str(Trama t) {
  String str = "";
  if (t.is_answer)
    str += t.ans;

  str += t.pre + String(t.node) + t.sep + t.func;

  if (t.data != "")
    str += t.sep + t.data;

  str += t.end;

  return str;
}

Trama str2trama(String s) {
  Trama trama; 
  
  // (R)? N{node} : {func} (: {data})? ;
  trama.is_answer = s[0] == trama.ans;
  int count = s.indexOf(trama.pre) + sizeof(trama.pre);
  s.remove(0, count); 
  
  // {node} : {func} (: {data})? ;
  // nodo
  int limit = s.indexOf(trama.sep);
  trama.node = s.substring(0, limit).toInt();
  count = limit + sizeof(trama.sep);
  s.remove(0, count); 
  
  // {func} (: {data})? ;
  // función
  bool is_done = false;
  limit = s.indexOf(trama.sep);
  if (limit < 0) {
    // {func} ;
    limit = s.indexOf(trama.end);
    is_done = true;
  }
  trama.func = s.substring(0, limit);

  if (!is_done) {
    count = limit + sizeof(trama.sep);
    s.remove(0, count);

    // {data} ;
    // data
    limit = s.indexOf(trama.end);
    trama.data = s.substring(0, limit);
  }

  return trama;
}

// String read_serial_monitor()
// {
//   if (Serial.available()){
//     String message = Serial.readStringUntil('\r');
//     message.trim();
//     // message.remove(message.length() - 1);
//     return message;
//   }

//   return "";
// }

String read_line_serial_485(SoftwareSerial &Serial485) {
  String cadena;

  if (Serial485.available())
    cadena = Serial485.readString();

  return cadena;
}

void send_trama(SoftwareSerial &Serial485, const int TX_RX_PIN, Trama package, long current_time, long &next_start, long frequency, bool verbose=false) {
  if (current_time >= next_start) {
    String cadena;

    if (verbose) { // ------ verbose mode ------
      Serial.print("Segundo ");
      Serial.println(current_time / 1000);
      Serial.print("|-- Enviando mensaje al nodo ");
      Serial.println(package.node);
    }

    digitalWrite(TX_RX_PIN, HIGH); // RS485 como transmisor

    // ------ enviamos la trama al esclavo ------
    cadena = trama2str(package);
    Serial485.print(cadena);

    if (verbose) { // ------ verbose mode ------
      Serial.print("|  |-- Trama enviada: ");
      Serial.println(cadena);

      Serial.println("|");
      Serial.println("|-- Buscando respuesta del esclavo...");
    }

    digitalWrite(TX_RX_PIN, LOW); // RS485 como receptor

    // ------ leemos la respuesta del esclavo ------
    if (Serial485.find(package.ans)) { // RN10:OK;
      cadena = Serial485.readString(); //N10:OK;
      Trama response = str2trama(cadena);
      response.is_answer = true;

      if (verbose) { // ------ verbose mode ------
        Serial.print("|  |-- Trama recibida: ");
        Serial.print(package.ans);
        Serial.println(cadena);
      }

      if (response.node == package.node) {
        if (verbose) { // ------ verbose mode ------
          Serial.println("|  |-- Nodo correcto (/)");

          if (response.func == package.func) {
            Serial.println("|  |-- Función correcta (/)");
          } else
            Serial.println("|  |-- Función incorrecta (X)");
        
          // if (response.data != "") {
          //   Serial.print("|  |  |-- Data recibida: ");
          //   Serial.println(response.data);
          // }
        }
      
      } else if (verbose) // ------ verbose mode ------
        Serial.println("|  |-- Nodo equivocado (X)");

    } else if (verbose) // ------ verbose mode ------
      Serial.println("|  |-- No se encontró respuesta");
    
    if (verbose)
      Serial.print("\n");

    next_start += frequency;
  }
}