#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // For LCD

// Pin Definitions
#define COIN_PIN 12            // Coin insertion button (GPIO 12)
#define BUTTON1_PIN 4          // Button 1 pin (Port 1 selection - GPIO 4)
#define BUTTON2_PIN 5          // Button 2 pin (Port 2 selection - GPIO 5)
#define RELAY_PIN1 14          // Relay pin for Port 1 (GPIO 14)
#define RELAY_PIN2 15          // Relay pin for Port 2 (GPIO 15)

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD setup (I2C Address: 0x27)

int selectedPort = 0;               // To store the selected port
int remainingTime = 0;              // Timer for charging time
int coinCount = 0;                  // To store the number of coins inserted
bool coinInserted = false;          // Coin insertion status

unsigned long lastDebounceTime = 0;  // Last time the button was pressed
unsigned long debounceDelay = 200;   // Debounce time (milliseconds)

void setup() {
  Serial.begin(9600);  // Start serial communication

  pinMode(COIN_PIN, INPUT_PULLUP);  // Coin button (GPIO 12) with internal pull-up
  pinMode(BUTTON1_PIN, INPUT_PULLUP);  // Button 1 (Port 1 selection - GPIO 4)
  pinMode(BUTTON2_PIN, INPUT_PULLUP);  // Button 2 (Port 2 selection - GPIO 5)
  pinMode(RELAY_PIN1, OUTPUT);        // Relay for Port 1 (GPIO 14)
  pinMode(RELAY_PIN2, OUTPUT);        // Relay for Port 2 (GPIO 15)

  lcd.init();                      // Initialize LCD
  lcd.backlight();              // Turn on the backlight
 
  Serial.println("System Initialized");

  // Testing LCD with simple message
  lcd.clear();
  lcd.print("System Ready");
  delay(1000);  // Wait for 1 second
}

void loop() {
  int coinReading = digitalRead(COIN_PIN); // Read the coin button

  // Coin Pressed (Debouncing)
  if (coinReading == LOW && (millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();   // Update debounce time
    coinInserted = true;            // Mark coin as inserted
    coinCount++;                    // Increase the coin count
    remainingTime += 10;            // Increase charge time by 10 sec for each coin

    // Display the updated coin count and charging time
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Coins: ");
    lcd.print(coinCount);  // Display the coin count

    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(remainingTime); // Display the charge time
    delay(300);            // Wait for a while to show updated values
  }

  // Check if Button 1 is pressed for Port 1 (GPIO 4)
  if (digitalRead(BUTTON1_PIN) == LOW && coinInserted) { // Button 1 pressed
    selectedPort = 1;
    lcd.clear();
    lcd.print("Port 1 Selected");
    Serial.println("Port 1 Selected"); // Debugging in Serial Monitor
    delay(1000);
  }

  // Check if Button 2 is pressed for Port 2 (GPIO 5)
  if (digitalRead(BUTTON2_PIN) == LOW && coinInserted) { // Button 2 pressed
    selectedPort = 2;
    lcd.clear();
    lcd.print("Port 2 Selected");
    Serial.println("Port 2 Selected"); // Debugging in Serial Monitor
    delay(1000);
  }

  // If a port is selected, simulate the charging process
  if (selectedPort != 0 && coinInserted) {
    // Turn off both relays first
    digitalWrite(RELAY_PIN1, LOW);
    digitalWrite(RELAY_PIN2, LOW);

    // Activate the relay for the selected port (This also controls the LED)
    switch (selectedPort) {
      case 1:
        digitalWrite(RELAY_PIN1, HIGH);  // Activate Relay for Port 1
        break;
      case 2:
        digitalWrite(RELAY_PIN2, HIGH);  // Activate Relay for Port 2
        break;
      default:
        break;
    }

    Serial.println("Charging Started");
    lcd.clear();
    lcd.print("Charging...");

    // Charging countdown
    while (remainingTime > 0) {
      lcd.setCursor(0, 1);
      lcd.print("Time Left: ");
      lcd.print(remainingTime);  // Display the remaining time
      lcd.print(" sec");
      delay(1000);  // Wait for 1 second
      remainingTime--;  // Decrease the time by 1 second
    }

    // Turn off the relay after charging is done (LED will turn off automatically)
    digitalWrite(RELAY_PIN1, LOW);
    digitalWrite(RELAY_PIN2, LOW);
    Serial.println("Charging Done");

    selectedPort = 0;               // Reset port selection
    coinInserted = false;           // Reset coin insertion status
    coinCount = 0;                  // Reset coin count
    remainingTime = 0;              // Reset charging time

    lcd.clear();
    lcd.print("Charging Done");
    delay(2000);                    // Wait for 2 seconds before starting again
  }
}
