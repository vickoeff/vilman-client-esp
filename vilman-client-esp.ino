
#include <LiquidCrystal_I2C.h>

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

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

#define SUBMIT_BUTTON 0
#define SWITCH_A 2
#define SWITCH_B 14
#define SWITCH_C 12
#define SWITCH_D 13

byte decimal = 0;

#define MAX_FLAVOUR 7 
int flavourSize = 0;

#define DEBOUNCE_BUTTON 300
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
const char root_ca[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

X509List cert(root_ca);

WiFiClientSecure espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  char* billText = "vilman/bill";
  bool isBill = String(topic).equals(billText);

  // Membuat buffer untuk menyimpan payload
  char msg[length + 1];

  // Mengkopi payload ke buffer
  memcpy(msg, payload, length);

  // Menambahkan null-terminator
  msg[length] = '\0';

  // Mengubah buffer menjadi string
  String message = String(msg);

  if (isBill) {
    isBillReady = isBill;
    billMessage = message;
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
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

void checkButtonState() {
  int bit0 = !digitalRead(SWITCH_A);
  int bit1 = !digitalRead(SWITCH_B);
  int bit2 = !digitalRead(SWITCH_C);
  int bit3 = !digitalRead(SWITCH_D);
  int submit = !digitalRead(SUBMIT_BUTTON);

  decimal = (bit3 << 3) | (bit2 << 2) | (bit1 << 1) | bit0;
  
  if(submit && selectedDough && flavourSize > 0) {
    CREATE_TRANSACTION();
    return;
  }
  
  // Set Dough or Flavour
  if (decimal <= 4 && decimal > 0 && timeoutCounter > DEBOUNCE_BUTTON) {
    selectedDough = decimal;
    if (flavourSize > 0) {
      resetFlavour();
    }
    timeoutCounter = 0;
  } else if (decimal >= 4 && timeoutCounter > 300 && timeoutCounter > DEBOUNCE_BUTTON) {
    addFlavour(decimal);
    timeoutCounter = 0;
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
    scrollText(3, flavours, 250, lcdColumns);
  }
}

// STATES START =========================================
void CREATE_TRANSACTION() {
    String payload = "";
    payload.concat(String(selectedDough));
    payload.concat("/");

    for (int i = 0; i < flavourSize; i++) {
      if(i > 0 && i < flavourSize - 1) {
        payload.concat(",");
      }
      payload.concat(String(selectedFlavour[i]));
    }
    Serial.println(payload);
    
    client.publish("vilman/transaction", payload.c_str());
    WAITING_BILL();
};

void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  }
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    checkButtonState();
    if(decimal > 0) {
      return;
    }
    delay(delayTime);
  }
}


void SELECT_FLAVOUR_STATE() {
  timeoutCounter = 0;

  while(timeoutCounter <= IDLE_TIMEOUT) {
    checkButtonState();
    if (decimal > 0) {
      updateMenu();
    }
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

    // Idle Timeout Handler
    if(timeoutCounter == IDLE_TIMEOUT) {
      selectedDough = 0;
      resetFlavour();
    }
    timeoutCounter += 10;
    delay(10);
  }
}

void WAITING_BILL() {
  int timeout = 0;
  
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.println("Mohon ditunggu ^_^");

  while(!isBillReady) {
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
    lcd.println(billMessage);
    
    delay(10000);
  }

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
  lcd.println("WiFi connected");
  lcd.print("IP address: ");
  lcd.println(WiFi.localIP());
  delay(300);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    lcd.clear();      
    lcd.println("Attempting MQTT connection...");
    
    String clientId = "ESP8266Client - vilman";

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      lcd.clear();   
      lcd.println("connected to MQTT");
      // set subscribtion topic
      client.subscribe("vilman/bill");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      lcd.clear();      
      lcd.print("failed, rc=");    
      lcd.println(client.state());  
      lcd.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup(){
  Serial.begin(115200);
  Serial.println("Serial Begin");
  lcd.init();
  lcd.backlight();

  pinMode(SWITCH_A, INPUT_PULLUP);
  pinMode(SWITCH_B, INPUT_PULLUP);
  pinMode(SWITCH_C, INPUT_PULLUP);
  pinMode(SWITCH_D, INPUT_PULLUP);
  pinMode(SUBMIT_BUTTON, INPUT_PULLUP);

  setup_wifi();

  // Set the root CA
  espClient.setTrustAnchors(&cert);

  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);

}

void loop(){  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

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
    updateMenu();
    SELECT_FLAVOUR_STATE();
  }
}