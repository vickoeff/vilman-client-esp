#include <LiquidCrystal_I2C.h>

int lcdColumns = 20;
int lcdRows = 4;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

String messageStatic = "+====+_VILMAN_+====+";
String messageToScroll = "Pilih menu untuk memesan :)";

void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}

void setup(){
  lcd.init();                   
  lcd.backlight();
}

void loop(){
  lcd.setCursor(0, 0);
  lcd.print(messageStatic);
  scrollText(2, messageToScroll, 250, lcdColumns);
  
}