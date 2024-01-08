#include "globals.h"

#include "gui.h"
#include <SHT3x.h>
#include "settings.h"
#include "pidloop.h"

GUI gui;
settings_t settings;
SettingsManager setman(settings);


PIDLoop pid1(settings.pid1);
PIDLoop pid2(settings.pid2);

PIDLoop* pids[NUMBER_OF_CHANNELS] = {&pid1, &pid2};

