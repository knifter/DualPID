#include "settings.h"

#include <Arduino.h>
#include <nvs.h>

#include "config.h"
#include "globals.h"
#include "tools-log.h"

SettingsManager::SettingsManager(settings_t& settings) : NVSettings(&settings, sizeof(settings_t))
{

};

bool SettingsManager::set_defaults_since(const uint32_t data_version)
{
    settings_t *settings = static_cast<settings_t*>(_data);

    switch(data_version)
    {
        default:
            ERROR("Unknown data version: %d", data_version);
            return false;
        case 0: // empty blob
            DBG("Init settings v0: defaults");
            memset(_data, 0, _data_size);
            settings->pid1.active = false;
            settings->pid1.mode = PIDLoop::MODE_ZP;
            settings->pid1.pin_n = 0;
            settings->pid1.pin_p = 0;
            settings->pid1.fpid.kF = DEFAULT_PID_F;
            settings->pid1.fpid.kP = DEFAULT_PID_P;
            settings->pid1.fpid.kI = DEFAULT_PID_I;
            settings->pid1.fpid.kD = DEFAULT_PID_D;
            settings->pid1.fpid.setpoint = DEFAULT_PID_SETPOINT;

            settings->pid2.active = false;
            settings->pid2.mode = PIDLoop::MODE_ZP;
            settings->pid2.pin_n = 0;
            settings->pid2.pin_p = 0;
            settings->pid2.fpid.kF = DEFAULT_PID_F;
            settings->pid2.fpid.kP = DEFAULT_PID_P;
            settings->pid2.fpid.kI = DEFAULT_PID_I;
            settings->pid2.fpid.kD = DEFAULT_PID_D;
            settings->pid2.fpid.setpoint = DEFAULT_SETPOINT;

        case 1:
            DBG("Settings v1->v2: sensor_loop");
            settings->sensor_loop_ms = DEFAULT_SENSOR_LOOP_MS;

        // End with the current version:
        case 2:
            _data_version = 2;
            return true;
    };

	return true;
};

bool SettingsManager::read_blob(void* blob, const size_t blob_size, const uint32_t blob_version)
{
    switch(blob_version)
    {
        default:
            ERROR("Unknown blob version: %d", blob_version);
            return false;

        case 1: // updateable
        case 2: // valid, up-to-date settings
            memcpy(_data, blob, blob_size);
            set_defaults_since(blob_version);
            _dirty = false;
            return true;
    };

	return true;
};
