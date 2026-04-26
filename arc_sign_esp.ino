#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Fonts/TomThumb.h>
#include <Pushbutton.h>
#include "qr_bitmap.h"

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 32;
constexpr int OLED_SDA = 8;
constexpr int OLED_SCL = 9;
constexpr int RESTART_BUTTON = 0;
constexpr int RED_LED = 1;
constexpr int YELLOW_LED = 2;
constexpr int GREEN_LED = 3;
constexpr int BLUE_LED = 4;
constexpr int SW = 10;
constexpr int CLK = 20;
constexpr int DT = 21;

int currentStateCLK;
int lastStateCLK;
int currentDutyCycle = 255;
int lastDutyCycle = 255;
bool light_on = true;
unsigned long lastButtonPress = 0;

Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);

void setup() {
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(0x3C, true)) {
    while (true) {
      delay(1000);
    }
  }
  display.clearDisplay();
  display.setFont(&TomThumb);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 10);

  const char* init_text[] = {
    "power routing to core systems...",
    "checking subsystem integrity...",
    "establishing uplink to speranza...",
    "synchronizing env sensors...",
    "initializing defensive subroutines...",
    "verifying identity: a7F3k9Qx2Z",
    "decrypting last known location...",
    "scanning for machine signatures...",
    "all systems nominal...",
  };
  
  display.print("initializing");
  display.display();
  delay(500);
  display.print(".");
  display.display();
  delay(1000);
  display.print(".");
  display.display();
  delay(1000);
  display.println(".");
  display.display();
  delay(1000);

  for (const char* x : init_text){
    display.println(x);
    display.display();
    delay(random(500, 750));
  }

  display.clearDisplay();
  display.fillRect(35, 3, QR_WIDTH, QR_HEIGHT, SH110X_WHITE);
  display.drawXBitmap(35, 3, qr_bitmap, QR_WIDTH, QR_HEIGHT, SH110X_BLACK);
  display.display();

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  pinMode(RESTART_BUTTON, INPUT_PULLUP);

  lastStateCLK = digitalRead(CLK);
  set_duty_cycles(lastDutyCycle);
}

void loop() {
  if (digitalRead(RESTART_BUTTON) == LOW){
    set_duty_cycles(0);
    lastDutyCycle = 255;
    esp_restart();
  }

  if (light_on == true) {
    currentStateCLK = digitalRead(CLK);

    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
      if (digitalRead(DT) != currentStateCLK) {
        currentDutyCycle = constrain((lastDutyCycle * 2), 8, 255);
      } else {
        currentDutyCycle = constrain((lastDutyCycle / 2), 8, 255);
      }
    }

    if (currentDutyCycle != lastDutyCycle) {
      if (currentDutyCycle < lastDutyCycle) {
        for (int i = lastDutyCycle; i >= currentDutyCycle; i--) {
          set_duty_cycles(i);
        }
      } else {
        for (int i = lastDutyCycle; i <= currentDutyCycle; i++) {
          set_duty_cycles(i);
        }
      }
    }
  }
  
  lastStateCLK = currentStateCLK;
  lastDutyCycle = currentDutyCycle;

  int btnState = digitalRead(SW);

  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {
      light_on = !light_on;
      if (light_on == true) {
        set_duty_cycles(lastDutyCycle);
      } else {
        set_duty_cycles(0);
      }
    }

    lastButtonPress = millis();
  }

  delay(1);
}

void set_duty_cycles(int duty_cycle) {
  analogWrite(RED_LED, duty_cycle);
  analogWrite(YELLOW_LED, duty_cycle);
  analogWrite(GREEN_LED, duty_cycle);
  analogWrite(BLUE_LED, duty_cycle);
  delay(5);
}