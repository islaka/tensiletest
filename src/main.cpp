#include <Arduino.h>
// #include <TMCStepper.h> //TMC stepper library
#include "oledHandler.h"
#include "ota.h"
#include <AiEsp32RotaryEncoder.h>
#include "sensorHandler.h"
#include "stepperHandler.h"

Scale scale;
OLED oled;
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ENCODER_A_PIN, ENCODER_B_PIN, ENCODER_BUTTON_PIN, 33, ENCODER_STEP);
StepperHandler stepper;

#define IS_HOMING false
#define IS_TESTING false

const char* options0[] = {"Homing", "Start Test", "About"};
const uint8_t numOptions = sizeof(options0) / sizeof(options0[0]);

uint8_t stage = 0; // 0 = menu, 1 = homing, 2 = testing, 3 = about
uint8_t currentSelection = 0;
uint8_t prevSelection = 0;
bool render = false;
long prev = 0;
long val = 0;

void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}

void homing() {
  // run till the switch is pressed
  stepper.setDirection(0);
  while (digitalRead(STOP_PIN) == HIGH) {
    stepper.step();
  }
}

void testing() {
  // scale.data() always >=0. when it increases then suddenly drop, stop the motor
  stepper.setDirection(1);
  while (true) {
    val = scale.data();
    if (val > prev) {
      stepper.step();
    }
    prev = val;
  }
}

void menuSetup() { // print initial menu
  oled.write((char *)"Tension Tester", 0, 0, 1, 0, 1);
  for (int i = 0; i < numOptions; i++) {
    oled.write((char *)options0[i], 0, (i+1), 0, 0, 1);
  }
  oled.write((char *)"Sensor - Group 9",0, 6, 0, 0, 1);
  oled.write((char *)"0.w.0",0, 7, 0, 0, 1);
}

void handleMenuSelectionLogic() {
  // decrease currentSelection when count increase and increase when count reduce, roll over when count > sizeof(option) and vice versa
  uint8_t count = rotaryEncoder.readEncoder();
  if (currentSelection != count) {
    prevSelection = currentSelection;
    currentSelection = count;
    render = true;
  }
  if (count >= numOptions) {
    count = 0;
    rotaryEncoder.reset();
  } else if (count < 0) {
    count = numOptions - 1;
    rotaryEncoder.setEncoderValue(numOptions - 1);
  }
}

void screen() {
  static uint8_t prevStage = stage;
  if (prevStage != stage) {
    render = true;
    oled.clear();
    prevStage = stage;
    menuSetup();
    render = false;
  }
  
  switch(stage) {
    case 0: { // menu
      handleMenuSelectionLogic();
      if (render) { // new content
        // update the menu by invert the new selection and the old selection
        oled.write((char *)options0[prevSelection], 0, (prevSelection+1), 0, 0, 1);
        oled.write((char *)options0[currentSelection], 0, (currentSelection+1), 0, 1, 1);
        render = false;
      }
      // if press button, go to the selected menu
      if (rotaryEncoder.isEncoderButtonClicked()) {
        stage = currentSelection + 1;
      }
      break;
    }
    case 1: { // homing
      // handleMenuSelectionLogic();
      if (render) { // print Homing... \n Press to cancel
        oled.clear();
        oled.write((char *)"Homing...", 0, 0, 1, 0, 1);
        oled.write((char *)"Press to cancel", 0, 1, 0, 0, 1);
        render = false;
        // homing();
      }
      if (rotaryEncoder.isEncoderButtonClicked()) {
        stage = 0;
      }
      break;
    }
    case 2: { // testing
      // handleMenuSelectionLogic();
      if (render) { // print Testing... \n Press to cancel
        oled.clear();
        oled.write((char *)"Testing...", 0, 0, 1, 0, 1);
        oled.write((char *)"Press to cancel", 0, 1, 0, 0, 1);
        render = false;
        scale.calibrate();
      }
      if (rotaryEncoder.isEncoderButtonClicked()) {
        stage = 0;
      }
      testing();
      break;
    }
    case 3: { // about
      // handleMenuSelectionLogic();
      if (render) { // print About... \n Press to cancel
        oled.clear();
        oled.write((char *)"About", 0, 0, 1, 0, 1);
        oled.write((char *)"Heck", 0, 1, 0, 0, 1);
        oled.write((char *)"Press to go back", 0, 5, 0, 0, 1);
        // oled.write((char *)"go back", 0, 2, 0, 0, 1);
        render = false;
        // about();
      }
      if (rotaryEncoder.isEncoderButtonClicked()) {
        stage = 0;
      }
      break;
    }
    default: {
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  oled.setup();
  scale.setup();
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  bool circleValues = false;
  rotaryEncoder.setBoundaries(0, 2, circleValues);
  rotaryEncoder.setAcceleration(0); 
  stepper.setup();

  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  pinMode(23,OUTPUT);
  digitalWrite(23, LOW);
  menuSetup();
}

void loop() {
  #ifdef OTA
  ota.handle();
  #endif
  screen();
}

