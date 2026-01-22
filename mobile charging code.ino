#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

// ================= PIN DEFINITIONS =================
const int relayPins[] = {25, 26, 27, 14}; 
const int buttonPins[] = {32, 33, 34, 35}; 

#define COIN_PIN 12
#define RFID_SS 5
#define RFID_RST 4

// ================= CONSTANTS =================
#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define RFID_DURATION 20   // seconds
#define COIN_DURATION 20   // seconds

// ================= OBJECTS =================
LiquidCrystal_I2C lcd(0x27, 20, 4);
MFRC522 rfid(RFID_SS, RFID_RST);

// ================= VARIABLES =================
bool relayActive[4] = {false, false, false, false};
unsigned long relayStart[4] = {0, 0, 0, 0};

volatile bool coinDetected = false;
bool rfidDetected = false;

// Button state tracking - edge detection এর জন্য
bool previousButtonState[4] = {HIGH, HIGH, HIGH, HIGH};
bool currentButtonState[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastButtonChange[4] = {0, 0, 0, 0};
const unsigned long debounceDelay = 50; // 50ms debounce

unsigned long coinReceivedTime = 0;
const unsigned long coinTimeout = 30000; // 30 seconds coin validity

// ================= COIN ISR =================
void IRAM_ATTR coinISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 300) { // 300ms debounce
    coinDetected = true;
  }
  lastInterruptTime = interruptTime;
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("System Starting...");

  // Relays: সব OFF রাখা
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], RELAY_OFF);
    relayActive[i] = false;
  }

  // Buttons: INPUT_PULLUP mode
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    previousButtonState[i] = HIGH;
    currentButtonState[i] = HIGH;
  }

  // Coin acceptor
  pinMode(COIN_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(COIN_PIN), coinISR, FALLING);

  // LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   WELCOME TO   ");
  lcd.setCursor(0, 1);
  lcd.print(" MOBILE CHARGER ");
  lcd.setCursor(0, 2);
  lcd.print("  System Ready!");

  // RFID
  SPI.begin();
  rfid.PCD_Init();
  
  delay(2000);
  
  // Final confirmation - সব relay OFF
  for (int i = 0; i < 4; i++) {
    digitalWrite(relayPins[i], RELAY_OFF);
  }
  
  lcd.clear();
  Serial.println("System Ready - Waiting for payment");
}

// ================= LOOP =================
void loop() {
  handleCoinTimeout();
  handleRFID();
  readButtons();
  handleButtonPress();
  updateRelays();
  updateLCD();
}

// ================= COIN TIMEOUT =================
void handleCoinTimeout() {
  // Coin detection হলে timer start করা
  if (coinDetected && coinReceivedTime == 0) {
    coinReceivedTime = millis();
    Serial.println(">>> COIN RECEIVED! Press button 2, 3, or 4 within 30 seconds");
  }
  
  // যদি 30 সেকেন্ড পার হয়ে যায় এবং কোন relay activate না হয়
  if (coinDetected && (millis() - coinReceivedTime > coinTimeout)) {
    coinDetected = false;
    coinReceivedTime = 0;
    Serial.println(">>> COIN TIMEOUT - Please insert coin again");
  }
}

// ================= HANDLE RFID =================
void handleRFID() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    rfidDetected = true;
    Serial.println(">>> RFID CARD DETECTED! Press button 1");
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

// ================= READ BUTTONS (DEBOUNCED) =================
void readButtons() {
  unsigned long now = millis();
  
  for (int i = 0; i < 4; i++) {
    int reading = digitalRead(buttonPins[i]);
    
    // যদি reading change হয়
    if (reading != currentButtonState[i]) {
      lastButtonChange[i] = now;
      currentButtonState[i] = reading;
    }
    
    // যদি debounce time পার হয়ে যায় এবং state stable থাকে
    if ((now - lastButtonChange[i]) > debounceDelay) {
      // এটাই final state
      previousButtonState[i] = currentButtonState[i];
    }
  }
}

// ================= HANDLE BUTTON PRESS =================
void handleButtonPress() {
  for (int i = 0; i < 4; i++) {
    // Edge detection: HIGH থেকে LOW তে গেলে = button press
    if (previousButtonState[i] == HIGH && currentButtonState[i] == LOW) {
      
      // যদি relay already active থাকে তাহলে ignore করা
      if (relayActive[i]) {
        Serial.print("Button ");
        Serial.print(i + 1);
        Serial.println(" ignored - Relay already active");
        continue;
      }
      
      // Button 1 = শুধু RFID এর জন্য
      if (i == 0) {
        if (rfidDetected) {
          activateRelay(0, RFID_DURATION);
          rfidDetected = false;
          Serial.println(">>> BUTTON 1 PRESSED - RFID Charging Started");
        } else {
          Serial.println(">>> Button 1 pressed but no RFID detected!");
        }
      }
      // Button 2, 3, 4 = শুধু Coin এর জন্য
      else {
        if (coinDetected) {
          activateRelay(i, COIN_DURATION);
          coinDetected = false;
          coinReceivedTime = 0;
          Serial.print(">>> BUTTON ");
          Serial.print(i + 1);
          Serial.println(" PRESSED - COIN Charging Started");
        } else {
          Serial.print(">>> Button ");
          Serial.print(i + 1);
          Serial.println(" pressed but no coin detected!");
        }
      }
      
      // Button state update করা
      previousButtonState[i] = LOW;
    }
    
    // Button release detection (LOW থেকে HIGH)
    if (previousButtonState[i] == LOW && currentButtonState[i] == HIGH) {
      previousButtonState[i] = HIGH;
    }
  }
}

// ================= ACTIVATE RELAY =================
void activateRelay(int index, int durationSec) {
  relayActive[index] = true;
  relayStart[index] = millis();
  digitalWrite(relayPins[index], RELAY_ON);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Charging Start!  ");
  lcd.setCursor(0, 1);
  lcd.print("  Slot: ");
  lcd.print(index + 1);
  lcd.print("          ");
  lcd.setCursor(0, 2);
  lcd.print(" Duration: ");
  lcd.print(durationSec);
  lcd.print(" sec  ");
  
  Serial.print("==> Relay ");
  Serial.print(index + 1);
  Serial.print(" ON for ");
  Serial.print(durationSec);
  Serial.println(" seconds");
  
  delay(2000); // মেসেজ দেখার জন্য
}

// ================= UPDATE RELAYS =================
void updateRelays() {
  for (int i = 0; i < 4; i++) {
    if (relayActive[i]) {
      unsigned long duration = (i == 0 ? RFID_DURATION : COIN_DURATION) * 1000UL;
      if (millis() - relayStart[i] >= duration) {
        relayActive[i] = false;
        digitalWrite(relayPins[i], RELAY_OFF);
        Serial.print("==> Relay ");
        Serial.print(i + 1);
        Serial.println(" OFF - Duration completed");
      }
    }
  }
}

// ================= UPDATE LCD =================
void updateLCD() {
  static unsigned long lastLcdUpdate = 0;
  if (millis() - lastLcdUpdate < 300) return; 
  lastLcdUpdate = millis();

  // যদি কোন relay চালু থাকে
  bool anyRelayActive = false;
  for (int i = 0; i < 4; i++) {
    if (relayActive[i]) {
      anyRelayActive = true;
      unsigned long duration = (i == 0 ? RFID_DURATION : COIN_DURATION) * 1000UL;
      unsigned long elapsed = millis() - relayStart[i];
      unsigned long remaining = (duration > elapsed) ? (duration - elapsed) / 1000 : 0;
      
      lcd.setCursor(0, 0);
      lcd.print("  Slot ");
      lcd.print(i + 1);
      lcd.print(" Charging ");
      lcd.setCursor(0, 1);
      lcd.print("  Time: ");
      lcd.print(remaining);
      lcd.print(" sec      ");
      break;
    }
  }

  // যদি কোন relay active না থাকে
  if (!anyRelayActive) {
    lcd.setCursor(0, 0);
    if (coinDetected) {
      lcd.print(" COIN OK! Select   ");
      lcd.setCursor(0, 1);
      lcd.print(" BTN: 2, 3, or 4   ");
    } else if (rfidDetected) {
      lcd.print(" CARD OK! Press    ");
      lcd.setCursor(0, 1);
      lcd.print(" Button 1 to Start ");
    } else {
      lcd.print("Insert COIN or CARD");
      lcd.setCursor(0, 1);
      lcd.print("Waiting Payment... ");
    }
  }

  // Line 3: Relay Status
  lcd.setCursor(0, 2);
  for (int i = 0; i < 4; i++) {
    lcd.print(i + 1);
    lcd.print(":");
    if (relayActive[i]) {
      lcd.print("ON ");
    } else {
      lcd.print("-- ");
    }
  }

  // Line 4: Payment Status
  lcd.setCursor(0, 3);
  lcd.print("Coin:");
  lcd.print(coinDetected ? "YES" : "NO ");
  lcd.print(" Card:");
  lcd.print(rfidDetected ? "YES" : "NO ");
  lcd.print(" ");
}