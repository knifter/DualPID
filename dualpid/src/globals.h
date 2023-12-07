#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <SHT3x.h>
#include "gui.h"
#include "pidloop.h"
#include "settings.h"


extern settings_t settings;

extern bool expert_mode;

extern GUI gui;
extern SettingsManager setman;
extern PIDLoop pid1;
extern PIDLoop pid2;

#endif //__GLOBALS_H
