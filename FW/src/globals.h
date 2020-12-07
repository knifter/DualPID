#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <SHT3x.h>
#include "gui.h"
#include "settings.h"

extern SHT3X sht_sensor;
extern GUI gui;
extern SettingsManager setman;
extern settings_t& settings;

#endif //__GLOBALS_H