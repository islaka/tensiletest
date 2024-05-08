#include <Arduino.h>
#include "ota.h"
// #include <TMCStepper.h> //TMC stepper library
#include "oledHandler.h"
#include <AiEsp32RotaryEncoder.h>
#include "sensorHandler.h"
#include "stepperHandler.h"

OLED oled;
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ENCODER_A_PIN, ENCODER_B_PIN, ENCODER_BUTTON_PIN, 33, 4);

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

float max_weight = 0;

void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}

void homing() {
  // run till the switch is pressed
  stepper.setDirection(1);
  while (digitalRead(END_STOP_PIN) == HIGH) {
    stepper.step(1);
  }
  stage = 0;
  render = true;
}

void run_test() {
  // scale.data() will increase and go over 1, after that when it drop below 1, the stepper will stop
  stepper.setDirection(0);
  bool armed = false;
  unsigned int count = 0;
  float weight = scale.data(0);
  // step until the loadcell is over 1
  while (true) {
    Serial.println(weight);
    stepper.step(1, 80);
    if (weight > 0.5) {
      count++;
    } else {
      count = 0;
    }
    if (count > 10) { // make sure the loadcell is over 1 consistently
      Serial.println("Armed");
      armed = true;
      break;
    }
    weight = scale.data(0);
    if (weight > max_weight) {
      max_weight = weight;
    }
  }
  // now the loadcell is over 1, step until the loadcell is below 1
  while (armed) {
    Serial.println(weight);
    stepper.step(1, 80);
    if (weight < 0.5) {
      count++;
    } else {
      count = 0;
    }
    if (count > 10) { // make sure the loadcell is below 1 consistently
      Serial.println("Disarmed");
      armed = false;
      break; // disarm
    }
    weight = scale.data(0);
    if (weight > max_weight) {
      max_weight = weight;
    }

    // render the scale data on the bottom
    char buffer[10];
    dtostrf(scale.data(0), 1, 2, buffer);
    oled.write(buffer, 0, 7, 0, 0, 1);
  }
}

void menuSetup() { // print initial menu
  oled.clear();
  oled.write((char *)"Tension Tester", 0, 0, 1, 0, 1);
  for (int i = 0; i < numOptions; i++) {
    oled.write((char *)options0[i], 0, (i+1), 0, 0, 1);
  }
  // max weight on line 4
  char buffer[10];
  dtostrf(max_weight, 1, 2, buffer);
  oled.write(buffer, 0, 4, 0, 0, 1);
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
  // static uint8_t prevStage = stage;
  // if (prevStage != stage) {
  //   render = true;
  //   oled.clear();
  //   prevStage = stage;
  //   menuSetup();
  //   render = false;
  // }
  
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
        render = true;
      }
      // render the scale data on the bottom
      char buffer[10];
      dtostrf(scale.data(0), 1, 2, buffer);
      oled.write(buffer, 0, 5, 0, 0, 1);
      break;
    }
    case 1: { // homing
      // handleMenuSelectionLogic();
      if (render) { // print Homing... \n Press to cancel
        // reset max_weight
        max_weight = 0;
        oled.clear();
        oled.write((char *)"Homing...", 0, 0, 1, 0, 1);
        oled.write((char *)"Press button to cancel", 0, 1, 0, 0, 1);
        render = false;
        homing();
        // after successfully homing, go back to menu
        stage = 0;
        menuSetup();
        render = true;
      }
      if (rotaryEncoder.isEncoderButtonClicked()) {
        stage = 0; // todo: NOT WORKING
      }
      break;
    }
    case 2: { // testing
      // handleMenuSelectionLogic();
      if (render) { // print Testing... \n Press to cancel
        oled.clear();
        oled.write((char *)"Testing...", 0, 0, 1, 0, 1);
        oled.write((char *)"Press button to cancel", 0, 1, 0, 0, 1);
        render = false;
        run_test();
        // after successfully testing, go back to menu
        stage = 0;
        menuSetup();
        render = true;
      }
      if (rotaryEncoder.isEncoderButtonClicked()) {
        stage = 0; // todo: NOT WORKING
      }
      
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
        menuSetup();
        stage = 0;
        render = true;
      }
      break;
    }
    default: {
      break;
    }
  }
}

void setup() {
  Serial.begin(921600);
  oled.setup();
  scale.setup();
  stepper.setup();

  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(0, 2);
  rotaryEncoder.setAcceleration(0); 

  menuSetup();
  render = true;
}

void loop() {
  #ifdef OTA
  ota.handle();
  #endif
  screen();
  Serial.println(scale.data(0));
}

