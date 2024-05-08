#pragma once
#include <Arduino.h>

/*****FLAGS*****/
// #define OTA
// #define USE_BUTTON_INTERRUPT
// #define DEBUG 
#define IS_LEGACY_DRIVER true

/*****HARD-CODED CONFIGURATION*****/
const char* ssid = "JINODK";
const char* password = "nevergonnaletyoudown";
const char* hostname = "Mini-Tension-Tester";

/*****HARDWARE PINS DEFINITION*****/
// HX711 circuit wiring
#define SG_DOUT 19   // Serial data output
#define SG_SCK 18

// Motor driver
#define STEP_PIN 32
#define DIR_PIN 33
// #define EN_PIN 34

// Encoder setup
#define ENCODER_A_PIN 26
#define ENCODER_B_PIN 25
#define ENCODER_BUTTON_PIN 27

// OLED setup
#define SDA_PIN 21
#define SCL_PIN 22

// Switch setup
#define END_STOP_PIN 23
#define EMERGENCY_STOP_PIN 4