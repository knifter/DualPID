#include "globals.h"

#include "pid.h"

LGFX gfx;

SHT3X sht_sensor(Wire, SHT_ADDRESS);
GUI gui;
SettingsManager setman;
settings_t& settings = setman.settings;

double get_value1() { return sht_sensor.getTemperature(); };
double get_value2() { return sht_sensor.getHumidity(); };

PIDLoop pid1(PIN_HB1_A, PIN_HB1_B, &get_value1);
PIDLoop pid2(PIN_HB2_A, PIN_HB2_B, &get_value2);
