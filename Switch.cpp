#include "Switch.h"

Switch::Switch(byte pin) {
  this->pin = pin;
  lastSteadyState = HIGH;
  onHold = false;
  init();
}
void Switch::init() {
  pinMode(pin, INPUT_PULLUP);
  update();
}
void Switch::update() {
  int newState = digitalRead(pin);

  if (newState != lastSteadyState) {
    if ((millis() - lastDebounceTime) > debounceDelay || (millis() - lastHoldTime) > holdTime) {
      // save on release state
      lastSteadyState = newState;
      Serial.println("Update State");

      lastHoldTime = millis();
    }

    lastDebounceTime = millis(); 
  }
}
byte Switch::getState() {
  update();
  return lastSteadyState;
}
bool Switch::getHoldState() {
  update();
  return onHold;
}
bool Switch::isPressed() {
  return (getState() == LOW);
}