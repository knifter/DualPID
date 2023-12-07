#include "globals.h"

#include "gui.h"
#include <SHT3x.h>
#include "settings.h"
#include "pidloop.h"

GUI gui;
settings_t settings;
SettingsManager setman(settings);

bool expert_mode;

PIDLoop pids[] = {
	PIDLoop(settings.pid1),
	PIDLoop(settings.pid2)
	};