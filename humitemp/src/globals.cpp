#include "globals.h"

#include "gui.h"
#include <SHT3x.h>
#include "settings.h"
#include "pidloop.h"

GUI gui;
settings_t settings;
SettingsManager setman(settings);

uint32_t developer_mode;
double input_value1;
double input_value2;

PIDLoop pid1((settings.pid1), input_value1);
PIDLoop pid2((settings.pid2), input_value2);
