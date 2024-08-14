#include "Switch.h"

Switch::Switch(uint8_t key, byte addressRow, byte addressCol) {
  this->key = key;
  this->addressRow = addressRow;
  this->addressCol = addressCol;
  lastSteadyState = LOW;
  onHold = false;
  init();
}
void Switch::init() {
  pinMode(addressCol, INPUT);
  update();
}
void Switch::update() {
  int newStateRow = digitalRead(addressRow);
  int newStateCol = digitalRead(addressCol);
  int newState;
    
  if (newStateRow && newStateCol) {
    newState = HIGH;
  } else {
    newState = LOW;
  }

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
  return (getState() == HIGH);
}
uint8_t Switch::getKey() {
  return key;
}
