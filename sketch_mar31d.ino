#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3
#define ENCODER_BUTTON_PIN 4
#define BUTTON_ON_PIN 5
#define BUTTON_OFF_PIN 6
#define ENABLE_PIN 8
#define DIR_PIN 9
#define STEP_PIN 10

#define FORWARD HIGH
#define REVERSE LOW

Encoder myEnc(ENCODER_PIN_A, ENCODER_PIN_B);
long oldPosition  = -999;
int adjustmentMode = 0;
int HA_speed = 2;
int LA_speed = 4;
int numCycles = 10;
int pauseTime = 10;
bool experimentRunning = false;
unsigned long experimentStartTime = 0;
int currentCycle = 0; // Added to track the current cycle globally

const unsigned char epd_bitmap_OLEDnewlogo[] PROGMEM = {/* Bitmap data */};

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_ON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_OFF_PIN, INPUT_PULLUP);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);

  digitalWrite(ENABLE_PIN, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  display.drawBitmap((SCREEN_WIDTH - 76) / 2, (SCREEN_HEIGHT - 64) / 2, epd_bitmap_OLEDnewlogo, 76, 64, WHITE);
  display.display();
  delay(2000);
}

void loop() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    switch (adjustmentMode) {
      case 0:
        HA_speed = constrain(newPosition, 1, 60);
        break;
      case 1:
        LA_speed = constrain(newPosition, 1, 60);
        break;
      case 2:
        numCycles = constrain(newPosition, 1, 100);
        break;
      case 3:
        pauseTime = constrain(newPosition, 1, 60);
        break;
    }
    if (!experimentRunning) {
      updateDisplay();
    }
  }

  if (!experimentRunning && digitalRead(ENCODER_BUTTON_PIN) == LOW) {
    delay(200);
    adjustmentMode = (adjustmentMode + 1) % 4;
    myEnc.write(0);
    if (!experimentRunning) {
      updateDisplay();
    }
  }

  if (digitalRead(BUTTON_ON_PIN) == LOW && !experimentRunning) {
    Serial.println("ON button pressed");
    startExperiment();
  }

  if (digitalRead(BUTTON_OFF_PIN) == LOW && experimentRunning) {
    stopExperiment();
  }

  // Continuously update the display if the experiment is running
  if (experimentRunning) {
    unsigned long elapsedTime = (millis() - experimentStartTime) / 1000;
    updateExperimentDisplay(elapsedTime, currentCycle); // Keep updating the display
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  switch (adjustmentMode) {
    case 0:
      display.print("H-A Speed: ");
      display.print(HA_speed);
      display.println(" sec");
      break;
    case 1:
      display.print("L-A Speed: ");
      display.print(LA_speed);
      display.println(" sec");
      break;
    case 2:
      display.print("# of Cycles: ");
      display.print(numCycles);
      break;
    case 3:
      display.print("Pause Time: ");
      display.print(pauseTime);
      display.println(" sec");
      break;
  }
  display.display();
}

void startExperiment() {
  Serial.println("Experiment started");
  experimentRunning = true;
  experimentStartTime = millis();
  currentCycle = 0; // Reset current cycle count
  for (int cycle = 0; cycle < numCycles; cycle++) {
    if (!experimentRunning) break;
    currentCycle = cycle; // Update current cycle
    rotateMotor(FORWARD, HA_speed);
    delay(pauseTime * 1000);
    if (!experimentRunning) break;
    rotateMotor(REVERSE, LA_speed);
    delay(pauseTime * 1000);
  }
  experimentRunning = false;
}

void stopExperiment() {
  experimentRunning = false;
}

void rotateMotor(bool direction, int durationSec) {
  digitalWrite(DIR_PIN, direction);
  int steps = 250;
  long delayPerStep = (durationSec * 1000L) / steps;
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delay(delayPerStep / 2);
    digitalWrite(STEP_PIN, LOW);
    delay(delayPerStep / 2);
  }
}

void updateExperimentDisplay(unsigned long elapsedTime, int currentCycle) {
  // Calculate hours, minutes, and seconds
  unsigned long hours = elapsedTime / 3600;
  unsigned long minutes = (elapsedTime % 3600) / 60;
  unsigned long seconds = elapsedTime % 60;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.print("Elapsed Time: ");
  if (hours < 10) display.print("0");
  display.print(hours);
  display.print(":");
  if (minutes < 10) display.print("0");
  display.print(minutes);
  display.print(":");
  if (seconds < 10) display.print("0");
  display.print(seconds);
  display.println(" sec");
  display.print("H-A: ");
  display.print(HA_speed);
  display.print(" sec, L-A: ");
  display.print(LA_speed);
  display.println(" sec");
  display.print("Cycles: ");
  display.print(numCycles);
  display.print(", Pause: ");
  display.print(pauseTime);
  display.println(" sec");

  display.display();}