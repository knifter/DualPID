#include "globals.h"

#include "gui.h"
#include <SHT3x.h>
#include "settings.h"
#include "pidloop.h"

GUI gui;
settings_t settings;
SettingsManager setman(settings);

bool expert_mode;

PIDLoop pid1((settings.pid1), input_value1);
PIDLoop pid2((settings.pid2), input_value2);
