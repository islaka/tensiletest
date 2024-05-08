#pragma once

#include "configurations.h"
#include "ss_oled.h"

// if your system doesn't have enough RAM for a back buffer, comment out
// this line (e.g. ATtiny85)
#define USE_BACKBUFFER

#ifdef USE_BACKBUFFER
static uint8_t ucBackBuffer[1024];
#else
static uint8_t *ucBackBuffer = NULL;
#endif

// Use -1 for the Wire library default pins
// or specify the pin numbers to use with the Wire library or bit banging on any GPIO pins
// These are the pin numbers for the M5Stack Atom default I2C
#define SDA_PIN 21
#define SCL_PIN 22
// Set this to -1 to disable or the GPIO pin number connected to the reset
// line of your display if it requires an external reset
#define RESET_PIN -1
// let ss_oled figure out the display address
#define OLED_ADDR -1
// don't rotate the display
#define FLIP180 1
// don't invert the display
#define INVERT 0
// Bit-Bang the I2C bus
#define USE_HW_I2C 0

// Change these if you're using a different OLED display
#define MY_OLED OLED_128x64
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
//#define MY_OLED OLED_64x32
//#define OLED_WIDTH 64
//#define OLED_HEIGHT 32

SSOLED ssoled;

class OLED {
private:
    int rc;
public:
    void setup() {
        rc = oledInit(&ssoled, MY_OLED, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, SDA_PIN, SCL_PIN, RESET_PIN, 400000L); // use standard I2C bus at 400Khz
        if (rc != OLED_NOT_FOUND)
        {
            // The I2C SDA/SCL pins set to -1 means to use the default Wire library
            // If pins were specified, they would be bit-banged in software
            // This isn't inferior to hw I2C and in fact allows you to go faster on certain CPUs
            // The reset pin is optional and I've only seen it needed on larger OLEDs (2.4")
            //    that can be configured as either SPI or I2C
            //
            // oledInit(SSOLED *, type, oled_addr, rotate180, invert, bWire, SDA_PIN, SCL_PIN, RESET_PIN, speed)
            // char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D",(char *)"SH1106 @ 0x3C",(char *)"SH1106 @ 0x3D"};
            oledFill(&ssoled, 0, 1);
            // oledWriteString(&ssoled, 0, 0, 7, msgs[rc], FONT_NORMAL, 0, 1);
            // oledWriteString(&ssoled, 0, 0, 0, (char *)"Tiny washer", FONT_SMALL, 0, 1);
            oledSetBackBuffer(&ssoled, ucBackBuffer);
            delay(1000);
        }
    }
    void write(char* msg, int x, int y, int fontSize, int invert, int render) {
        switch (fontSize) {
            case 0:
                oledWriteString(&ssoled, 0, x, y, msg, FONT_SMALL, invert, render);
                break;
            case 1:
                oledWriteString(&ssoled, 0, x, y, msg, FONT_NORMAL, invert, render);
                break;
            case 2:
                oledWriteString(&ssoled, 0, x, y, msg, FONT_LARGE, invert, render);
                break;
            default:
                oledWriteString(&ssoled, 0, x, y, msg, FONT_NORMAL, invert, render);
                break;
        }
    }
    void clear() {
        oledFill(&ssoled, 0, 1);
    }
    void setPixel(int x, int y, int color, int render) {
        oledSetPixel(&ssoled, x, y, color, render);
    }
    void drawLine(int x1, int y1, int x2, int y2, int render) {
        oledDrawLine(&ssoled, x1, y1, x2, y2, render);
    }
};