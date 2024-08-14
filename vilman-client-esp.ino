
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "Switch.h"

#include "pitches.h"

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
  {8, "Kacang"},
  {9, "Keju"},
  {10, "Pisang"},
  {11, "Spesial"},
  {12, "Custom"},
  {13, ""},
  {14, ""},
  {15, ""},
};

const int mapSize = sizeof(doughFlavourMap) / sizeof(doughFlavourMap[0]);

// Define keyboard scale
#define X_KEY 4
#define Y_KEY 4

#define SW_ROW_0 16
#define SW_ROW_1 17
#define SW_ROW_2 18
#define SW_ROW_3 19
#define SW_COL_0 23
#define SW_COL_1 25
#define SW_COL_2 26
#define SW_COL_3 27
#define SUBMIT_BUTTON 33

#define BUZZER_PIN 13

// Define Default Switch key
uint8_t sw_1_key = 1;
uint8_t sw_2_key = 2;
uint8_t sw_3_key = 3;
uint8_t sw_4_key = 4;
uint8_t sw_5_key = 5;
uint8_t sw_6_key = 6;
uint8_t sw_7_key = 7;
uint8_t sw_8_key = 8;
uint8_t sw_9_key = 9;
uint8_t sw_10_key = 10;
uint8_t sw_11_key = 11;
uint8_t sw_12_key = 12;
uint8_t sw_13_key = 13;
uint8_t sw_14_key = 14;
uint8_t sw_15_key = 15;
uint8_t sw_16_key = 16;

Switch sw_1(sw_1_key, SW_ROW_0, SW_COL_0);
Switch sw_2(sw_2_key, SW_ROW_0, SW_COL_1);
Switch sw_3(sw_3_key, SW_ROW_0, SW_COL_2);
Switch sw_4(sw_4_key, SW_ROW_0, SW_COL_3);
Switch sw_5(sw_5_key, SW_ROW_1, SW_COL_0);
Switch sw_6(sw_6_key, SW_ROW_1, SW_COL_1);
Switch sw_7(sw_7_key, SW_ROW_1, SW_COL_2);
Switch sw_8(sw_8_key, SW_ROW_1, SW_COL_3);
Switch sw_9(sw_9_key, SW_ROW_2, SW_COL_0);
Switch sw_10(sw_10_key, SW_ROW_2, SW_COL_1);
Switch sw_11(sw_11_key, SW_ROW_2, SW_COL_2);
Switch sw_12(sw_12_key, SW_ROW_2, SW_COL_3);
Switch sw_13(sw_13_key, SW_ROW_3, SW_COL_0);
Switch sw_14(sw_14_key, SW_ROW_3, SW_COL_1);
Switch sw_15(sw_15_key, SW_ROW_3, SW_COL_2);
Switch sw_16(sw_16_key, SW_ROW_3, SW_COL_3);

// Row Active States
byte rowActv[4] = {1, 0, 0, 0};
int iteration = 0;

#define MAX_FLAVOUR 7 
int flavourSize = 0;

#define DEBOUNCE_BUTTON 500
#define WAITING_BUTTON_SCROLL 300
#define IDLE_TIMEOUT 20000// millisecond
int timeoutCounter = 0;

String messageStatic = "+====+_VILMAN_+====+";
String messageToScroll = "Pilih menu untuk memesan ^_^";

int selectedDough = 0;
int selectedFlavour[MAX_FLAVOUR];

bool isBillReady = false;
String billMessage;

const char* ssid = "______";
const char* password = "picilucu";
const char* mqtt_server = "919a078e0d7f4c6a93566fc5f7320b0d.s1.eu.hivemq.cloud";

const char* mqtt_username = "vilman-iot";
const char* mqtt_password = "Bq7g##ibkAj6B_4";
// Root CA certificate
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String message;

  for (int i = 0; i < length; i++) {
    message.concat(((char)payload[i]));
  }

  isBillReady = String(topic).equals("vilman/bill");
  billMessage = message;

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(billMessage);
}

void handleRunningRow() {
  // Update Row active state
  for(int i = 0; i < X_KEY; i++) {
    if (i == iteration) {
      rowActv[i] = 1;
    } else {
      rowActv[i] = 0;
    }
  }

  // Write Digital Row Pin according to rowState
  digitalWrite(SW_ROW_0, LOW);
  digitalWrite(SW_ROW_1, LOW);
  digitalWrite(SW_ROW_2, LOW);
  digitalWrite(SW_ROW_3, LOW);
  if(rowActv[0]) {
    digitalWrite(SW_ROW_0, HIGH);
  } else if(rowActv[1]) {
    digitalWrite(SW_ROW_1, HIGH);
  } else if(rowActv[2]) {
    digitalWrite(SW_ROW_2, HIGH);
  } else if(rowActv[3]) {
    digitalWrite(SW_ROW_3, HIGH);
  }

  if(iteration >= X_KEY - 1) {
    iteration = 0;
  } else {
    iteration++;
  }
}

uint8_t watchKeySwitch() {
  handleRunningRow();

  if(rowActv[0]) {
    if(sw_1.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_1.getKey());
      return sw_1.getKey();
    }
    
    if(sw_2.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_2.getKey());
      return sw_2.getKey();
    } 

    if(sw_3.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_3.getKey());
      return sw_3.getKey();
    }
    
    if(sw_4.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_4.getKey());
      return sw_4.getKey();
    }
  } else if(rowActv[1]) {
    if(sw_5.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_5.getKey());
      return sw_5.getKey();
    }

    if(sw_6.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_6.getKey());
      return sw_6.getKey();
    }

    if(sw_7.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_7.getKey());
      return sw_7.getKey();
    }
    
    if(sw_8.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_8.getKey());
      return sw_8.getKey();
    }
  } else if(rowActv[2]) {
    if(sw_9.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_9.getKey());
      return sw_9.getKey();
    }

    if(sw_10.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_10.getKey());
      return sw_10.getKey();
    }

    if(sw_11.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_11.getKey());
      return sw_11.getKey();
    }

    if(sw_12.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_12.getKey());
      return sw_12.getKey();
    }
  } else if(rowActv[3]) {
    if(sw_13.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_13.getKey());
      return sw_13.getKey();
    }

    if(sw_14.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_14.getKey());
      return sw_14.getKey();
    }

    if(sw_15.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_15.getKey());
      return sw_15.getKey();
    }

    if(sw_16.isPressed()) {
      Serial.println("pressed");
      updateKey(sw_16.getKey());
      return sw_16.getKey();
    }
  }

  int submit = digitalRead(SUBMIT_BUTTON);

  if(submit) {
    updateKey(17);
    return 17;
  }

  return 0;

  delay(3); // 12 debounced delay / 4 column
}

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
  int temp = flavourSize;

  for (int i = 0; i < temp; i++) {
    selectedFlavour[--flavourSize];
  }
};

void updateKey(uint8_t key) {
  if(key > 0) {
    int noteDuration = 100;
    tone(BUZZER_PIN, NOTE_E1, noteDuration);
  }

  if(key == 17 && selectedDough > 0 && flavourSize > 0) {
    CREATE_TRANSACTION();
  }
  
  // Set Dough or Flavour
  if (key <= 4 && key > 0 && (timeoutCounter == 0 || timeoutCounter > DEBOUNCE_BUTTON)) {
    timeoutCounter = 0;
    selectedDough = key;

    if (flavourSize > 0) {
      resetFlavour();
    }
  } else if (key > 4 && key <= 16 && (timeoutCounter == 0 || timeoutCounter > DEBOUNCE_BUTTON)) {
    timeoutCounter = 0;
    addFlavour(key);
  }
}

void updateMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Adonan Terang Bulan:");
  lcd.setCursor(0, 1);
  lcd.print(getValue(selectedDough));
  lcd.setCursor(0, 2);
  lcd.print("Isian:");
  
  String flavours = "";
  
  for (int i = 0; i < flavourSize; i++) {
    if (i > 0) {
      flavours.concat(" | ");
    }
    flavours.concat(getValue(selectedFlavour[i]));
  }
  
  if (flavours.length() <= 20) {
    lcd.setCursor(0, 3);
    lcd.print(flavours);
  } else {
    scrollText(3, flavours, 100, lcdColumns);
  }
}

// STATES START =========================================
void CREATE_TRANSACTION() {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    String payload = "";
    payload.concat(String(selectedDough));
    payload.concat("/");

    for (int i = 0; i < flavourSize; i++) {
      if(i > 0 && i < flavourSize) {
        payload.concat(",");
      }
      payload.concat(String(selectedFlavour[i] - 4));
    }
    
    lcd.clear();
    lcd.setCursor(2, 1);
    lcd.print("Membuat pesanan");
    delay(500);
    Serial.println(payload);
    
    client.publish("vilman/transaction", payload.c_str());
    WAITING_BILL();
};

void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i= 0; i < lcdColumns; i++) {
    message = " " + message;  
  }
  message = message + " "; 

  uint8_t key = 0;

  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);

    int wait = 0;

    while(wait <= WAITING_BUTTON_SCROLL) {
      key = watchKeySwitch();

      if(key > 0) {
        updateMenu();
        return;
      }
  
      wait++;
      delay(1);
    }

    if(key > 0) {
      return;
    }
  }
}

void SELECT_FLAVOUR_STATE() {
  timeoutCounter = 0;

  while(timeoutCounter <= IDLE_TIMEOUT) {
    uint8_t key = watchKeySwitch();
    if (key > 0) {
      updateMenu();
    }
    // Serial.print("key: ");
    // Serial.println(key);
    // Serial.print("flavourSize: ");
    // Serial.println(flavourSize);
    // Serial.print("flavour: ");
    // for (int i = 0; i < flavourSize; i++) {
    //   Serial.print(getValue(selectedFlavour[i]));
    //   if (i < flavourSize - 1) {
    //     Serial.print("|");
    //   }
    // }
    // Serial.println("===========");

    // Idle Timeout Handler
    if(timeoutCounter == IDLE_TIMEOUT) {
      selectedDough = 0;
      resetFlavour();
    }
    if(selectedDough == 0 && flavourSize == 0) {
      return;
    }
    timeoutCounter += 10;
    delay(10);
  }
}

void WAITING_BILL() {
  int timeout = 0;
  
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Mohon ditunggu ^_^");

  while(!isBillReady) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    if (timeout >= IDLE_TIMEOUT) {
      lcd.clear();
      lcd.println("Transaksi Gagal");
      delay(1000);
      break;
    }

    timeout+= 10;
    delay(10);
  }
  

  if (isBillReady) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Total Harga:");
    lcd.setCursor(0, 2);
    lcd.print("Rp ");
    lcd.print(billMessage);
    
    delay(10000);
  }
  
  isBillReady = false;
  billMessage = "";
  selectedDough = 0;
  resetFlavour();
}
// STATES END =========================================

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Connecting to ");
  lcd.print(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected");
  lcd.setCursor(0, 1);
  lcd.print("IP address: ");
  lcd.print(WiFi.localIP());
  delay(300);
}

void reconnect() {
  while (!client.connected()) {
    if(WiFi.status() != WL_CONNECTED) {
      setup_wifi();
    }
    Serial.print("Attempting MQTT connection...");
    lcd.setCursor(19, 3);
    lcd.print("-");
    
    String clientId = "ESP8266Client - vilman";

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      lcd.setCursor(19, 3);
      lcd.print("#");
      // set subscribtion topic
      client.subscribe("vilman/bill");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      lcd.setCursor(19, 3);
      lcd.print("!");
      delay(5000);
    }
  }
}

void setup(){
  Serial.begin(115200);
  Serial.println("Serial Begin");
  lcd.init();
  lcd.backlight();

  // PinIO Configurations
  pinMode(SW_ROW_0, OUTPUT);
  pinMode(SW_ROW_1, OUTPUT);
  pinMode(SW_ROW_2, OUTPUT);
  pinMode(SW_ROW_3, OUTPUT);
  pinMode(SUBMIT_BUTTON, INPUT);

  setup_wifi();

  // Set the root CA
  espClient.setCACert(root_ca);

  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);

}

void loop(){  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  uint8_t key = watchKeySwitch();

  // Idle Condition
  if (selectedDough == 0 && key == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(messageStatic);
    scrollText(2, messageToScroll, 100, lcdColumns);
  } else {
    updateMenu();
    SELECT_FLAVOUR_STATE();
  }
}