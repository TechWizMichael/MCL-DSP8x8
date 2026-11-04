// Values and functions for the heartbeat
// Last updated October 31, 2026

// Heartbeat Constants and variables
#define HEARTBEAT_HALF_PERIOD_MS 1000
#define HEARTBEAT_LED 1
bool heartbeat_state = HIGH;
int heartbeat_time = 1;
bool heartbeat_mode = LOW;

void setup_heartbeat(void){
  // Initialize Heartbeat LED
  pinMode(HEARTBEAT_LED, OUTPUT);
  heartbeat_time = millis();

  return;
}

void process_heartbeat(void){
  if (millis() > (heartbeat_time + HEARTBEAT_HALF_PERIOD_MS)) {
    heartbeat_time = millis();
    digitalWrite(HEARTBEAT_LED, (heartbeat_mode | heartbeat_state));

    heartbeat_state = !heartbeat_state;
  }

  return;
}

void set_heartbeat_mode(bool mode){
  heartbeat_mode = mode;
  return;
}