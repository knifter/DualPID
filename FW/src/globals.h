#ifndef __GLOBALS_H
#define __GLOBALS_H

#define LGFX_USE_V1
#define LGFX_AUTODETECT
// #define LGFX_M5STACK_CORE2         // M5Stack Core2
#include <LGFX_TFT_eSPI.hpp>

#include <SHT3x.h>
#include "gui.h"
#include "settings.h"
#include "pid.h"

extern LGFX gfx;

extern SHT3X sht_sensor;
extern GUI gui;
extern SettingsManager setman;
extern settings_t& settings;
extern PIDLoop pid1;
extern PIDLoop pid2;

double get_value1();
double get_value2();

#endif //__GLOBALS_H