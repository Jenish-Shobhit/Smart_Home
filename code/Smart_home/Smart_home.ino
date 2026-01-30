#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin Definitions
#define PIR_PIN 2
#define BUZZER_PIN 3
#define PASSWORD_BTN 4
#define ENTER_BTN 8
#define DOOR_SERVO 9
#define WINDOW_SERVO 10
#define WHITE_LED 13
#define YELLOW_LED 12
#define FAN_PIN1 6
#define FAN_PIN2 7
#define GAS_SENSOR A0
#define LIGHT_SENSOR A1
#define SOIL_SENSOR A2
#define STEAM_SENSOR A3
#define RELAY_PIN 5

// Threshold Values
#define LIGHT_THRESHOLD 300
#define GAS_THRESHOLD 500
#define SOIL_THRESHOLD 700
#define STEAM_THRESHOLD 800
#define DOT_TIME 250
#define DASH_TIME 1000
#define PIR_DELAY 5000

// System States
bool systemActive = false;
bool doorOpen = false;
bool windowOpen = true;
bool alarmActive = false;

// Password System
const String correctPassword = "..";  
String currentPassword = "";
unsigned long buttonPressTime = 0;
bool buttonPressed = false;

// Timing Variables
unsigned long lastSensorUpdate = 0;
unsigned long lastMotionDetected = 0;
unsigned long lastDisplayUpdate = 0;

// LCD Display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo Motors
Servo doorServo;
Servo windowServo;

// Sensor Values
struct SensorData {
    int gas;
    int light;
    int soil;
    int steam;
    bool motion;
} sensors;

// Add melody definitions for PIR alert
#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392

// Function Declarations
void updateSensors();
void handleAlarms();
void updateDisplay();
void handlePassword();
void activateSystem();
void deactivateSystem();
void handleMotion();
void playMelody(int melody[], float durations[], int length);
void playBirthday();
void playOdeToJoy();

// Function to play motion alert melody
void playMotionAlert() {
    int notes[] = {C4, E4, G4, E4, C4};
    int durations[] = {100, 100, 100, 100, 100};
    
    for(int i = 0; i < 5; i++) {
        tone(BUZZER_PIN, notes[i], durations[i]);
        delay(durations[i] + 50);  // Small gap between notes
    }
    noTone(BUZZER_PIN);
}

void setup() {
    // Initialize LCD
    lcd.init();
    lcd.backlight();
    
    // Initialize Pins
    pinMode(PIR_PIN, INPUT);
    pinMode(PASSWORD_BTN, INPUT_PULLUP);
    pinMode(ENTER_BTN, INPUT_PULLUP);
    pinMode(WHITE_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(FAN_PIN1, OUTPUT);
    pinMode(FAN_PIN2, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    
    // Initialize Servos
    doorServo.attach(DOOR_SERVO);
    windowServo.attach(WINDOW_SERVO);
    doorServo.write(0);  // Close door
    windowServo.write(0); // Close window
    
    // Initial Display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Password:");
    lcd.setCursor(0, 1);
    lcd.print("BTN1:DOT BTN2:ENT");
}

void loop() {
    handlePassword();
    
    if (systemActive) {
        unsigned long currentMillis = millis();
        
        // Update sensors every 500ms
        if (currentMillis - lastSensorUpdate >= 500) {
            updateSensors();
            handleAlarms();
            lastSensorUpdate = currentMillis;
        }
        
        // Update display every 1000ms
        if (currentMillis - lastDisplayUpdate >= 1000) {
            updateDisplay();
            lastDisplayUpdate = currentMillis;
        }
        
        handleMotion();
    }
}

void handlePassword() {
    static unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 50;  // 50ms debounce time
    
    // Read password button (BTN1)
    if (digitalRead(PASSWORD_BTN) == LOW && !buttonPressed) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
            buttonPressed = true;
            buttonPressTime = millis();
            lastDebounceTime = millis();
        }
    }
    
    // Button released
    if (digitalRead(PASSWORD_BTN) == HIGH && buttonPressed) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
            unsigned long pressDuration = millis() - buttonPressTime;
            buttonPressed = false;
            lastDebounceTime = millis();
            
            // Only add dot if press was short and clean
            if (pressDuration > 50 && pressDuration < DOT_TIME) {
                currentPassword += ".";
                lcd.setCursor(0, 1);
                lcd.print("                "); // Clear line
                for(int i = 0; i < currentPassword.length(); i++) {
                    lcd.setCursor(i, 1);
                    lcd.print("*");
                }
            }
        }
    }
    
    // Check enter button (BTN2)
    if (digitalRead(ENTER_BTN) == LOW) {
        delay(50); // Simple debounce
        
        if (currentPassword == "..") {  // Correct password
            // Success melody
            tone(BUZZER_PIN, 2000, 200);
            delay(250);
            tone(BUZZER_PIN, 2500, 200);
            delay(250);
            tone(BUZZER_PIN, 3000, 400);
            
            lcd.clear();
            lcd.print("Welcome Jenish!");
            lcd.setCursor(0, 1);
            lcd.print("Door Opening...");
            
            // Enhanced door opening with wider range and smoother motion
            if (!doorOpen) {
                // First move to 90 degrees quickly
                for(int angle = 0; angle <= 90; angle += 10) {
                    doorServo.write(angle);
                    delay(10);
                }
                // Then complete the motion more smoothly
                for(int angle = 90; angle <= 180; angle += 5) {
                    doorServo.write(angle);
                    delay(20);
                }
                doorOpen = true;
                delay(500);
            }
            
            systemActive = true;
            lcd.clear();
            lcd.print("System Active");
            lcd.setCursor(0, 1);
            lcd.print("Monitoring...");
            delay(1000);  // Reduced delay for better responsiveness
        }
        else if (currentPassword == "...") {  // Special code for Birthday song
            lcd.clear();
            lcd.print("Playing Birthday");
            playBirthday();
        }
        else if (currentPassword == "---") {  // Special code for Ode to Joy
            lcd.clear();
            lcd.print("Playing Ode2Joy");
            playOdeToJoy();
        }
        else if (currentPassword == "##") {  // Special code to close door
            lcd.clear();
            lcd.print("Door Closing...");
            
            if (doorOpen) {
                // Enhanced door closing with smoother motion
                // Quick initial movement
                for(int angle = 180; angle >= 90; angle -= 10) {
                    doorServo.write(angle);
                    delay(10);
                }
                // Slower final approach
                for(int angle = 90; angle >= 0; angle -= 5) {
                    doorServo.write(angle);
                    delay(20);
                }
                doorOpen = false;
                tone(BUZZER_PIN, 1500, 200);
                delay(250);
                tone(BUZZER_PIN, 1000, 400);
            }
            delay(500);  // Reduced delay
        }
        else {  // Wrong password
            // Error melody
            tone(BUZZER_PIN, 500, 200);
            delay(250);
            tone(BUZZER_PIN, 400, 400);
            
            lcd.clear();
            lcd.print("Wrong Password!");
            lcd.setCursor(0, 1);
            lcd.print("Try Again");
            delay(2000);
        }
        
        // Reset for next input
        currentPassword = "";
        lcd.clear();
        lcd.print("Enter Password:");
        lcd.setCursor(0, 1);
        lcd.print("BTN1:DOT BTN2:ENT");
    }
}

void updateSensors() {
    sensors.gas = analogRead(GAS_SENSOR);
    sensors.light = analogRead(LIGHT_SENSOR);
    sensors.soil = analogRead(SOIL_SENSOR);
    sensors.steam = analogRead(STEAM_SENSOR);
    sensors.motion = digitalRead(PIR_PIN);
}

void handleAlarms() {
    // Gas Detection with improved alerts
    if (sensors.gas > GAS_THRESHOLD) {
        digitalWrite(YELLOW_LED, HIGH);  // Solid ON instead of blinking
        digitalWrite(FAN_PIN1, HIGH);
        digitalWrite(FAN_PIN2, LOW);
        digitalWrite(RELAY_PIN, HIGH);  // Activate relay for additional ventilation
        tone(BUZZER_PIN, 1000, 500);   // Add buzzer alert
        alarmActive = true;
    } else {
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(FAN_PIN1, LOW);
        digitalWrite(FAN_PIN2, LOW);
        digitalWrite(RELAY_PIN, LOW);
        noTone(BUZZER_PIN);
        alarmActive = false;
    }
    
    // Light Control
    if (sensors.light < LIGHT_THRESHOLD) {
        digitalWrite(WHITE_LED, HIGH);
    } else {
        digitalWrite(WHITE_LED, LOW);
    }
    
    // Steam/Rain Detection with improved window control
    static bool isClosing = false;
    static bool isOpening = false;
    
    if (sensors.steam > STEAM_THRESHOLD && !isClosing) {
        // Start closing window
        isClosing = true;
        isOpening = false;
        for(int angle = windowServo.read(); angle >= 0; angle -= 5) {
            windowServo.write(angle);
            delay(15);
        }
        windowOpen = false;
        isClosing = false;
    } 
    else if (sensors.steam < (STEAM_THRESHOLD - 100) && !isOpening && !windowOpen) {
        // Start opening window
        isOpening = true;
        isClosing = false;
        for(int angle = windowServo.read(); angle <= 90; angle += 5) {
            windowServo.write(angle);
            delay(15);
        }
        windowOpen = true;
        isOpening = false;
    }
}

void updateDisplay() {
    if (!alarmActive) {
        lcd.clear();
        lcd.setCursor(0, 0);
        
        static byte displayState = 0;
        switch(displayState) {
            case 0:
                lcd.print("Gas: ");
                lcd.print(sensors.gas);
                break;
            case 1:
                lcd.print("Light: ");
                lcd.print(sensors.light);
                if (sensors.light < LIGHT_THRESHOLD) {
                    lcd.setCursor(0, 1);
                    lcd.print("Low Light!");
                }
                break;
            case 2:
                lcd.print("Motion Status:");
                lcd.setCursor(0, 1);
                lcd.print(sensors.motion ? "Movement!" : "No Movement");
                break;
            case 3:
                lcd.print("Soil: ");
                lcd.print(sensors.soil);
                lcd.setCursor(0, 1);
                if (sensors.soil > SOIL_THRESHOLD) {
                    lcd.print("Enough Water!");
                } else {
                    lcd.print("Need Water!");
                }
                break;
            case 4:
                lcd.print("Door Status:");
                lcd.setCursor(0, 1);
                lcd.print(doorOpen ? "Open" : "Closed");
                break;
            case 5:
                lcd.print("Steam: ");
                lcd.print(sensors.steam);
                if (sensors.steam > STEAM_THRESHOLD) {
                    lcd.setCursor(0, 1);
                    lcd.print("Rain Detected!");
                }
                break;
        }
        displayState = (displayState + 1) % 6;  // Now 6 states to include soil
    } else {
        lcd.clear();
        lcd.print("ALERT: GAS/FIRE!");
        lcd.setCursor(0, 1);
        lcd.print("Fan Active!");
    }
}

void handleMotion() {
    static unsigned long lastMotionCheck = 0;
    static bool lastMotionState = false;
    static unsigned long lastAlertTime = 0;
    const unsigned long ALERT_INTERVAL = 3000;  // Minimum time between alerts
    
    if (systemActive && (millis() - lastMotionCheck > 1000)) {  // Check every second
        lastMotionCheck = millis();
        
        if (sensors.motion != lastMotionState) {  // Only act on state changes
            lastMotionState = sensors.motion;
            
            if (sensors.motion && (millis() - lastAlertTime > ALERT_INTERVAL)) {
                playMotionAlert();  // Play alert melody
                lastAlertTime = millis();
            }
        }
    }
}

void activateSystem() {
    systemActive = true;
    lcd.clear();
    lcd.print("System Active!");
    doorServo.write(90);  // Open door
    doorOpen = true;
    delay(1000);
}

void deactivateSystem() {
    systemActive = false;
    doorServo.write(0);   // Close door
    doorOpen = false;
    windowServo.write(0); // Close window
    windowOpen = false;
    digitalWrite(WHITE_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(FAN_PIN1, LOW);
    digitalWrite(FAN_PIN2, LOW);
    
    lcd.clear();
    lcd.print("Enter Password:");
}
//Programmed by Jenish Shobhit

