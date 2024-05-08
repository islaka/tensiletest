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

// // TMC2226 setup

// #define STALL_VALUE 2 // [0..255]
// #define SERIAL_PORT Serial2
// #define DRIVER_ADDRESS 0b00
// #define R_SENSE 0.11f
// #define IN_1 12
// #define IN_2 13

// Encoder setup
#define ENCODER_A_PIN 27
#define ENCODER_B_PIN 26
#define ENCODER_BUTTON_PIN 25
#define ENCODER_STEP 4

// OLED setup
#define SDA_PIN 21
#define SCL_PIN 22

// Switch setup
#define STOP_PIN 33

/*****DEFAULT TIMER CONFIGURATION*****/
uint8_t delay_step = 100;
uint8_t delay_scale = 10;