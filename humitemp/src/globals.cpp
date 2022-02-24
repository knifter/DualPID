#include "globals.h"

#include "gui.h"
#include <SHT3x.h>
#include "settings.h"
#include "pidloop.h"

SHT3X sht_sensor(Wire);
GUI gui;
settings_t settings;
SettingsManager setman(settings);

double get_value1() { return sht_sensor.newMeasurement()->temperature; };
double get_value2() { return sht_sensor.getHumidity(); };

PIDLoop pid1(&(settings.pid1), &get_value1);
PIDLoop pid2(&(settings.pid2), &get_value2);
