#ifndef SWITCH_H
#define SWITCH_H

#include <Arduino.h>

class Switch {
  private:
    uint8_t key;
    byte addressRow;
    byte addressCol;
    int lastSteadyState;
    bool onHold;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 12;
    unsigned long holdTime = 400;
    unsigned long lastHoldTime = 0;
  public:
    Switch(uint8_t key, byte addressRow, byte addressCol);
    void init();
    void update();
    byte getState();
    uint8_t getKey();
    bool isPressed();
    bool getHoldState();
};

#endif