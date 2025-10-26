
/*
  --- Task A: LED Mode Controller ---
  Name: Raja M Ayyaz
  Roll No: CS-1278
  course embeded iot

*/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int buttonNext = 32;
const int buttonReset = 33;
const int led1 = 2;
const int led2 = 5;

int mode = 0;
bool lastButtonStateNext = HIGH;
bool lastButtonStateReset = HIGH;

unsigned long previousMillis = 0;
int fadeValue = 0;
int fadeStep = 5;

void showMode() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  
  if(mode == 0) display.println("OFF");
  if(mode == 1) display.println("ALT Blink");
  if(mode == 2) display.println("Both ON");
  if(mode == 3) display.println("PWM Fade");

  display.display();
}

void setup() {
  pinMode(buttonNext, INPUT_PULLUP);
  pinMode(buttonReset, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showMode();
}

void loop() {
  bool currentNext = digitalRead(buttonNext);
  bool currentReset = digitalRead(buttonReset);

  if(currentNext == LOW && lastButtonStateNext == HIGH) {
    mode++;
    if(mode > 3) mode = 0;
    showMode();
    delay(200);
  }

  if(currentReset == LOW && lastButtonStateReset == HIGH) {
    mode = 0;
    showMode();
    delay(200);
  }

  lastButtonStateNext = currentNext;
  lastButtonStateReset = currentReset;

  if(mode == 0) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }

  if(mode == 1) {
    if(millis() - previousMillis >= 500) {
      previousMillis = millis();
      digitalWrite(led1, !digitalRead(led1));
      digitalWrite(led2, !digitalRead(led2));
    }
  }

  if(mode == 2) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
  }

  if(mode == 3) {
    analogWrite(led1, fadeValue);
    analogWrite(led2, fadeValue);
    fadeValue += fadeStep;
    if(fadeValue <= 0 || fadeValue >= 255) fadeStep = -fadeStep;
    delay(15);
  }
}
