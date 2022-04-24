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


// void set_led_by_next_blink(long current_time, long &next_blink, bool &state_led)
// {
//   if (current_time >= next_blink)
//   {
//     // toggle_led(current_time, state_led);
//     next_blink += 2000;
//   }
// }

String read_serial_monitor()
{
  if (Serial.available()){
    String message = Serial.readStringUntil('\r');
    message.trim();
    // message.remove(message.length() - 1);
    return message;
  }

  return "";
}

String read_line_serial_485(SoftwareSerial &Serial485) {
  String cadena;

  if (Serial485.available())
    cadena = Serial485.readString();

  return cadena;
}