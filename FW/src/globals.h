#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <SHT3x.h>

#include "gui.h"
#include "settings.h"
#include "pid.h"

extern SHT3X sht_sensor;
extern GUI gui;
extern SettingsManager setman;
extern settings_t& settings;
extern PIDLoop pid1;
extern PIDLoop pid2;

double get_value1();
double get_value2();

#endif //__GLOBALS_H