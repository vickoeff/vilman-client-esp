#include <LiquidCrystal_I2C.h>
#include "Switch.h"

int lcdColumns = 20;
int lcdRows = 4;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

struct MapEntry {
  int id;
  const char* value;
};

// Decimal Map
const MapEntry doughFlavourMap[] = {
// Doughs
  {1, "Original"},
  {2, "Red Velvet"},
  {3, "Pandan"},
  {4, "Black Forest"},
// Flavour
  {5, "Coklat"},
  {6, "Coklat Crunch"},
  {7, "Silver Queen"},
  {8, "Coklat Crunch Full"},
  {9, "Kacang"},
  {10, "Keju"},
  {11, "Pisang"},
  {12, ""},
  {13, ""},
  {14, "custom"},
  {15, "Spesial"},
};

const int mapSize = sizeof(doughFlavourMap) / sizeof(doughFlavourMap[0]);

#define SUBMIT_BUTTON 16
#define POP_FLAVOUR_BUTTON 0
#define SWITCH_A 2
#define SWITCH_B 12
#define SWITCH_C 13
#define SWITCH_D 14

Switch SW_A(SWITCH_A);
Switch SW_B(SWITCH_B);
Switch SW_C(SWITCH_C);
Switch SW_D(SWITCH_D);
Switch SW_POP(POP_FLAVOUR_BUTTON);
Switch SW_SUBMIT(SUBMIT_BUTTON);

byte decimal = 0;

#define MAX_FLAVOUR 7 
int flavourSize = 0;

#define DEBOUNCE_BUTTON 300
#define IDLE_TIMEOUT 15000// millisecond
int timeoutCounter = 0;

String messageStatic = "+====+_VILMAN_+====+";
String messageToScroll = "Pilih menu untuk memesan ^_^";

int selectedDough = 0;
int selectedFlavour[MAX_FLAVOUR];

const char* getValue(int id) {
  for (int i = 0; i < mapSize; i++) {
    if (doughFlavourMap[i].id == id) {
      return doughFlavourMap[i].value;
    }
  }
  return " ";
};

// Push flavour
bool addFlavour(int value) {
  bool result = false;

  if (flavourSize < MAX_FLAVOUR) {
    selectedFlavour[flavourSize++] = value;
    result = true;
  } else {
    lcd.setCursor(0, 3);
    lcd.print("Isian sudah penuh");
  }

  return result;
};

// Pop flavour
void popFlavour() {
  if (flavourSize > 0) {
    selectedFlavour[--flavourSize];
  }
};

void resetFlavour() {
  for (int i = 0; i < flavourSize; i++) {
    selectedFlavour[--flavourSize];
  }
}

void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  }
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
    if(SW_A.isPressed() || SW_B.isPressed() || SW_C.isPressed() || SW_D.isPressed()) {
      return;
    }
  }
}

void checkButtonState() {
  int bit0 = !SW_A.getState();
  int bit1 = !SW_B.getState();
  int bit2 = !SW_C.getState();
  int bit3 = !SW_D.getState();
  int pop = !SW_POP.getState();
  
  if (pop) {
    popFlavour();
    return;
  }

  decimal = (bit3 << 3) | (bit2 << 2) | (bit1 << 1) | bit0;
}

void updateMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Adonan Terang Bulan:");
  lcd.setCursor(0, 1);
  lcd.print(getValue(selectedDough));
  lcd.setCursor(0, 2);
  lcd.print("Isian:");
  lcd.setCursor(0, 3);
  for (int i = 0; i < flavourSize; i++) {
    lcd.print(getValue(selectedFlavour[i]));
    if (i < flavourSize - 1) {
      lcd.print("|");
    }
  }
}

void SELECT_FLAVOUR_STATE() {
  timeoutCounter = 0;

  while(timeoutCounter <= IDLE_TIMEOUT) {
    checkButtonState();
    Serial.print("Decimal: ");
    Serial.println(decimal);
    Serial.print("flavourSize: ");
    Serial.println(flavourSize);
    Serial.print("flavour: ");
    for (int i = 0; i < flavourSize; i++) {
      Serial.print(getValue(selectedFlavour[i]));
      if (i < flavourSize - 1) {
        Serial.print("|");
      }
    }
    Serial.println("===========");
    
    // Set Dough or Flavour
    if (decimal <= 4 && decimal > 0 && timeoutCounter > DEBOUNCE_BUTTON) {
      selectedDough = decimal;
      resetFlavour();
      timeoutCounter = 0;
      updateMenu();
    } else if (decimal >= 4 && timeoutCounter > 300 && timeoutCounter > DEBOUNCE_BUTTON) {
      addFlavour(decimal);
      timeoutCounter = 0;
      updateMenu();
    } else {
      // Idle Timeout Handler
      if(timeoutCounter == IDLE_TIMEOUT) {
        selectedDough = 0;
        resetFlavour();
      }
      timeoutCounter += 10;
      delay(10);
    }
  }
}

void setup(){
  Serial.begin(115200);
  Serial.println("Serial Begin");
  lcd.init();
  lcd.backlight();
}

void loop(){
    checkButtonState();
    Serial.print("Decimal: ");
    Serial.println(decimal);

  // Idle Condition
  if (selectedDough == 0 && decimal == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(messageStatic);
    scrollText(2, messageToScroll, 250, lcdColumns);
    return;
  } else {
    SELECT_FLAVOUR_STATE();
  }
}