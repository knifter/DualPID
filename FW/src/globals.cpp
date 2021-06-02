#include "globals.h"

SHT3X sht_sensor(Wire);
GUI gui;
SettingsManager setman;
settings_t& settings = setman.settings;

float g_MeasT;
float g_MeasRH;
