/*
  Masterpiece Person Counter + AI Room Control + Critical Alarm + LCD MONITOR
  
  Features:
  1. Bi-directional Counting.
  2. Smart Buzzer (Entry/Exit/Empty).
  3. AI Fan Control.
  4. AI LED Intensity.
  5. LCD MONITOR: Displays Count, Capacity, and Safety Status.
  6. OVERLOAD ALARM SYSTEM (Level 1 & 2).

  Hardware Pinout:
  - Sensors: Pin 8, Pin 9
  - Buzzer: Pin 10
  - Capacity Switches: Pin 11, Pin 12
  - Fan (L293D): Pin 13
  - OVERRIDE SWITCH: Pin 6 (GND to Mute)
  - LEDs 1-4: Pins A0, A1, A2, A3
  - LED 5: PIN 7 (MOVED FROM A4!)
  - LCD (I2C): SDA -> A4, SCL -> A5
*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Initialize LCD (Address 0x27 is common, sometimes 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Pins ---
const int SENSOR_I1_PIN = 8;
const int SENSOR_I2_PIN = 9;
const int BUZZER_PIN = 10;
const int SWITCH_A_PIN = 11;
const int SWITCH_B_PIN = 12;
const int FAN_PIN = 13;
const int OVERRIDE_PIN = 6; 

// Note: LED 5 moved to Pin 7 to free up A4 for LCD
const int LED_PINS[] = {A0, A1, A2, A3, 7};

// --- Variables ---
int personCount = 0;
int i1_state, i2_state;
int machineState = 0;
int maxCapacity = 5; 

// Tracking variables to prevent LCD flickering
int lastCount = -1;
int lastCapacity = -1;
bool lastOverride = false;
unsigned long lastLcdUpdate = 0;

void setup() {
  Serial.begin(9600);
  
  // Init Sensors & Actuators
  pinMode(SENSOR_I1_PIN, INPUT);
  pinMode(SENSOR_I2_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
  
  pinMode(SWITCH_A_PIN, INPUT_PULLUP);
  pinMode(SWITCH_B_PIN, INPUT_PULLUP);
  pinMode(OVERRIDE_PIN, INPUT_PULLUP);

  for (int i = 0; i < 5; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  
  // Init LCD
  lcd.init();
  lcd.backlight();
  
  // Intro Screen
  lcd.setCursor(0, 0);
  lcd.print("AI Room Guard");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
  
  Serial.println("--- Super AI Person Counter Initialized ---");
}

void loop() {
  // 1. READ SENSORS
  i1_state = digitalRead(SENSOR_I1_PIN);
  i2_state = digitalRead(SENSOR_I2_PIN);

  // 2. RUN COUNTING LOGIC
  switch (machineState) {
    case 0: // IDLE
      if (i1_state == LOW) { machineState = 1; delay(50); } 
      else if (i2_state == LOW) { machineState = 2; delay(50); }
      break;

    case 1: // ENTRY
      if (i2_state == LOW) {
        personCount++;
        buzz(500); 
        machineState = 3; delay(50);
      } else if (i1_state == HIGH && i2_state == HIGH) { machineState = 0; }
      break;

    case 2: // EXIT
      if (i1_state == LOW) {
        bool willBeEmpty = (personCount == 1);
        if (personCount > 0) personCount--; else personCount = 0;
        
        if (willBeEmpty) {
          buzz(4000); 
        } else {
          buzz(500); 
        }
        machineState = 3; delay(50);
      } else if (i1_state == HIGH && i2_state == HIGH) { machineState = 0; }
      break;

    case 3: // RESET
      if (i1_state == HIGH && i2_state == HIGH) { machineState = 0; }
      break;
  }

  // 3. RUN AI MODELS
  updateRoomAI();
  
  // 4. UPDATE LCD (Only if changed)
  updateLCD();
}

// --- Helper Function for Buzzer ---
void buzz(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

// --- THE AI MODEL ---
void updateRoomAI() {
  // -- Part A: Fan Control --
  if (personCount > 0) digitalWrite(FAN_PIN, HIGH); 
  else digitalWrite(FAN_PIN, LOW);

  // -- Part B: Determine Capacity --
  int swA = digitalRead(SWITCH_A_PIN);
  int swB = digitalRead(SWITCH_B_PIN);

  if (swA == LOW && swB == LOW)      maxCapacity = 5;
  else if (swA == LOW && swB == HIGH) maxCapacity = 10;
  else if (swA == HIGH && swB == LOW) maxCapacity = 20;
  else                                maxCapacity = 30;

  // -- Part C: LEDs --
  int ledsToLight = map(personCount, 0, maxCapacity, 0, 5);
  ledsToLight = constrain(ledsToLight, 0, 5);
  if (personCount > 0 && ledsToLight == 0) ledsToLight = 1;

  for (int i = 0; i < 5; i++) {
    if (i < ledsToLight) digitalWrite(LED_PINS[i], HIGH);
    else digitalWrite(LED_PINS[i], LOW);
  }

  // -- Part D: ADVANCED ALARM SYSTEM --
  bool overrideActive = (digitalRead(OVERRIDE_PIN) == LOW);
  int excessPeople = personCount - maxCapacity;

  if (excessPeople > 0 && !overrideActive) {
    // Alarm Logic (Crazy Scale vs Critical)
    if (excessPeople >= 5) {
       // Critical: Long + Burst
       digitalWrite(BUZZER_PIN, HIGH); delay(1000); 
       digitalWrite(BUZZER_PIN, LOW); delay(100);
       for(int k=0; k<15; k++) {
         digitalWrite(BUZZER_PIN, HIGH); delay(40); 
         digitalWrite(BUZZER_PIN, LOW); delay(40);
       }
       delay(200);
    } else {
      // Crazy Scale
      int beepSpeed = map(excessPeople, 1, 5, 500, 100);
      digitalWrite(BUZZER_PIN, HIGH); delay(beepSpeed); 
      digitalWrite(BUZZER_PIN, LOW); delay(beepSpeed);
    }
  }
}

void updateLCD() {
  // Check if anything important changed to avoid screen flicker
  bool overrideActive = (digitalRead(OVERRIDE_PIN) == LOW);
  
  if (personCount != lastCount || maxCapacity != lastCapacity || overrideActive != lastOverride) {
    
    // Line 1: Count and Capacity
    lcd.setCursor(0, 0);
    lcd.print("Ppl:");
    lcd.print(personCount);
    lcd.print("  Max:");
    lcd.print(maxCapacity);
    lcd.print("  "); // Clear leftover characters
    
    // Line 2: Status Message
    lcd.setCursor(0, 1);
    
    if (overrideActive) {
       lcd.print("ALARM MUTED!    ");
    } else if (personCount > maxCapacity) {
       lcd.print("OVERLOAD! -");
       lcd.print(personCount - maxCapacity);
       lcd.print("   ");
    } else if (personCount == maxCapacity) {
       lcd.print("ROOM FULL!      ");
    } else if (personCount == 0) {
       lcd.print("ROOM EMPTY      ");
    } else {
       lcd.print("SYSTEM NORMAL   ");
    }

    // Update last known values
    lastCount = personCount;
    lastCapacity = maxCapacity;
    lastOverride = overrideActive;
  }
}