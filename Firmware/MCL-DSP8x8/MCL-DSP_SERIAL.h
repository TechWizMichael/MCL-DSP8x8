// Values and functions for using Serial/USB
// Last updated October 26, 2026

#include <Arduino.h>

// USB Communication and handshake
#define SERIAL_HANDSHAKE_QUERY "opensesame"
#define SERIAL_HANDSHAKE_RESPONSE "behold"

void setup_serial(void){
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial begin success");
  return;
}

String read_serial(void){
  if(Serial.available()){
    String input = Serial.readStringUntil('\n');

    if (input == SERIAL_HANDSHAKE_QUERY) {
      Serial.println(SERIAL_HANDSHAKE_RESPONSE);
      return "";
    }
    return input;
  }
  return "";
}

void serial_respond(String response){
  if (!response.equals("NOCOMMAND")) {
    Serial.println("RESPONSE: " + response);
  }
  
  return;
}