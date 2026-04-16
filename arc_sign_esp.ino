constexpr int RED_LED = 0;
constexpr int YELLOW_LED = 1;
constexpr int GREEN_LED = 2;
constexpr int BLUE_LED = 3;
constexpr int EXTRA_LED = 6;
constexpr int SW = 10;
constexpr int CLK = 20;
constexpr int DT = 21;

int currentStateCLK;
int lastStateCLK;
int currentDutyCycle = 255;
int lastDutyCycle = 255;
bool light_on = true;
unsigned long lastButtonPress = 0;

void setup() {
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(EXTRA_LED, OUTPUT);

  lastStateCLK = digitalRead(CLK);
  set_duty_cycles(lastDutyCycle);
  analogWrite(EXTRA_LED, 32);
}

void loop() {
  if (light_on == false) {
    analogWrite(RED_LED, 0);
    analogWrite(YELLOW_LED, 0);
    analogWrite(GREEN_LED, 0);
    analogWrite(BLUE_LED, 0);
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






