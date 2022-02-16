#include "globals.h"

#include "gui.h"
#include <SHT3x.h>
#include "settings.h"
#include "pidloop.h"

SHT3X sht_sensor(Wire);
GUI gui;
SettingsManager setman;
settings_t& settings = setman.settings;

double get_value1() { return sht_sensor.newMeasurement()->temperature; };
double get_value2() { return sht_sensor.getHumidity(); };

PIDLoop pid1(PIN_HB1_A, PIN_HB1_B, &get_value1, &(settings.pid1));
PIDLoop pid2(PIN_HB2_A, PIN_HB2_B, &get_value2, &(settings.pid2));
