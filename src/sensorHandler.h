#pragma once
#include "configurations.h"
#include <HX711.h>

#define CALIB 21069.f
HX711 loadcell;

class Scale {
    public:
        void setup() {
            loadcell.begin(SG_DOUT, SG_SCK);
            loadcell.set_scale(CALIB);
            loadcell.tare();
        }
        void calibrate() {
            loadcell.set_scale();
            loadcell.tare();
        }
        // void read_data() {
        //     Serial.println("Reading:" + String((int32_t)loadcell.get_units(10)) +" & "+ "Divided:" + String((int32_t)loadcell.get_units(10)/CALIB_WEIGHT));
        // }
        void power_down() {
            loadcell.power_down();
        }
        void power_up() {
            loadcell.power_up();
        }

        float data(int avg = 10) {
            // Serial.print("Reading...");
            if (avg == 0) {
                return loadcell.get_units();
            }
            return loadcell.get_units(avg);
        }
};
Scale scale;