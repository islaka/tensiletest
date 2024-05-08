#include <Arduino.h>
#include <TMCStepper.h> //TMC stepper library
#include "oledHandler.h"
#include "ota.h"
#include <AiEsp32RotaryEncoder.h>
#include "sensorHandler.h"

// using namespace TMC2209_n;
Scale scale;
// TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);
OLED oled;
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ENCODER_A_PIN, ENCODER_B_PIN, ENCODER_BUTTON_PIN, 33, ENCODER_STEP);

#define IS_HOMING false
#define IS_TESTING false
// #define IS_CALIBRATING false

const char* options0[] = {"Homing", "Start Test", "About"};

uint8_t stage = 0; // 0 = menu, 1 = homing, 2 = testing, 3 = about
uint8_t currentSelection = 0;
uint8_t prevSelection = 0;
uint8_t numOptions = sizeof(options0) / sizeof(options0[0]);

bool render = false;

void homing() {}
void testing() {
  scale.read_data();
}
void about() {}

void menuSetup() { // print initial menu
  oled.write((char *)"Tension Tester", 0, 0, 1, 0, 1);
  for (int i = 0; i < numOptions; i++) {
    oled.write((char *)options0[i], 0, (i+1), 0, 0, 1);
  }
  oled.write((char *)"Sensor - Group 9",0, 6, 0, 0, 1);
  oled.write((char *)"0.w.0",0, 7, 0, 0, 1);
}

void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}

void handleMenuSelectionLogic() {
  // decrease currentSelection when count increase and increase when count reduce, roll over when count > sizeof(option) and vice versa
  uint8_t count = rotaryEncoder.readEncoder();
  if (currentSelection < count) {
    prevSelection = currentSelection;
    currentSelection++;
    render = true;
  } else if (currentSelection > count) {
    prevSelection = currentSelection;
    currentSelection--;
    render = true;
  }
  if (count > numOptions - 1) {
    count = 0;
    rotaryEncoder.reset();
  } else if (count < 0) {
    count = numOptions - 1;
    rotaryEncoder.setEncoderValue(numOptions - 1);
  }
}

void screen() {
  switch(stage) {
    case 0: {
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
        // oled.clear();
        render = true;
      }
      break;
    }
    case 1: {
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
        oled.clear();
        render = true;
        menuSetup();
      }
      break;
    }
    case 2: {
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
        oled.clear();
        render = true;
        menuSetup();
      }
      testing();
      break;
    }
    case 3: {
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
        oled.clear();
        render = true;
        menuSetup();
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
  // while(!Serial); 
  // Serial.println("\nStart...");
  // SERIAL_PORT.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  oled.setup();
  scale.setup();
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  bool circleValues = false;
  rotaryEncoder.setBoundaries(0, 2, circleValues);
  rotaryEncoder.setAcceleration(0); 

  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  pinMode(23,OUTPUT);
  digitalWrite(23, LOW);
  // // // TMC2226 setup
  // pinMode(STEP_PIN, OUTPUT);
  // pinMode(DIR_PIN, OUTPUT);
  // pinMode(EN_PIN, OUTPUT);
  // digitalWrite(EN_PIN, HIGH); // Disable driver
  // pinMode(TX_PIN, OUTPUT);
  // pinMode(RX_PIN, INPUT); 
  // digitalWrite(STEP_PIN, LOW);
  // digitalWrite(DIR_PIN, LOW);
  // digitalWrite(EN_PIN, LOW); // Enable driver

  // driver.begin();
  // driver.toff(3);
  // driver.blank_time(24);
  // driver.rms_current(500); //mA
  // driver.microsteps(16);
  // driver.TCOOLTHRS(0xFFFFF); // 20bit max
  // driver.semin(0);
  // driver.semax(2);
  // driver.shaft(false);
  // driver.sedn(0b01);
  // driver.SGTHRS(STALL_VALUE);

  // HX711 setup
  
  // activate_interrupt();
  menuSetup();
}

void loop() {
  #ifdef OTA
  ota.handle();
  #endif
  // static uint32_t last_time = 0;
  // static uint32_t last_step = 0;
  // uint32_t current = millis();
  // non blocking delay
  // if (current - last_time > delay_scale) {
  //   last_time = current;
  //   Serial.println(scale.get_units(10));
  // }
  // if (current - last_step > delay_step) {
  //   last_step = current;
  //   Serial.print("0 ");
  //   Serial.print(driver.SG_RESULT(), DEC);
  //   Serial.print(" ");
  //   Serial.println(driver.cs2rms(driver.cs_actual()), DEC);
  // rotary_loop();
	// delay(1);
  screen();
  // }
}

