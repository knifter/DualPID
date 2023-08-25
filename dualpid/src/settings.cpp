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
        case 1: // 
        case 2: //
        case 3: //
        case 4: //
            DBG("Init settings v5: defaults");
            memset(_data, 0, _data_size);

            settings->sensor_loop_ms =              DEFAULT_SENSOR_LOOP_MS;
            settings->graph_delta =                 DEFAULT_GRAPH_DELTA_MS;

            settings->pid1.active =                 false;
            settings->pid1.mode =                   PID1_DEFAULT_MODE;
            settings->pid1.pin_n =                  0;
            settings->pid1.pin_p =                  0;
            settings->pid1.looptime =               PID1_DEFAULT_LOOPTIME_MS;
            settings->pid1.max_output =             PID1_DEFAULT_MAX_OUTPUT;
            settings->pid1.windowtime =             PID1_DEFAULT_WINDOWTIME;
            settings->pid1.fpid.kF =                PID1_DEFAULT_F;
            settings->pid1.fpid.kF_offset =         PID1_DEFAULT_F_OFFSET;
            settings->pid1.fpid.kP =                PID1_DEFAULT_P;
            settings->pid1.fpid.kI =                PID1_DEFAULT_I;
            settings->pid1.fpid.kD =                PID1_DEFAULT_D;
            settings->pid1.fpid.setpoint =          PID1_DEFAULT_SETPOINT;
            settings->pid1.fpid.output_filter =     PID1_DEFAULT_OFILTER;
            settings->pid1.fpid.dterm_filter =      PID1_DEFAULT_DFILTER;
            settings->pid1.fpid.takebackhalf =      PID1_DEFAULT_TBH;
            settings->pid1.lock_window =            PID1_DEFAULT_LOCK_WINDOW;
            settings->pid1.lock_time =              PID1_DEFAULT_LOCK_TIME_MS;
            settings->pid1.input_filter =           PID1_DEFAULT_INPUT_FILTER;

            settings->pid2.active =                 false;
            settings->pid2.mode =                   PID2_DEFAULT_MODE;
            settings->pid2.pin_n =                  0;
            settings->pid2.pin_p =                  0;
            settings->pid2.looptime =               PID2_DEFAULT_LOOPTIME_MS;
            settings->pid2.max_output =             PID2_DEFAULT_MAX_OUTPUT;
            settings->pid2.windowtime =             PID2_DEFAULT_WINDOWTIME;
            settings->pid2.fpid.kF =                PID1_DEFAULT_F;
            settings->pid2.fpid.kF_offset =         PID1_DEFAULT_F_OFFSET;
            settings->pid2.fpid.kP =                PID2_DEFAULT_P;
            settings->pid2.fpid.kI =                PID2_DEFAULT_I;
            settings->pid2.fpid.kD =                PID2_DEFAULT_D;
            settings->pid2.fpid.setpoint =          PID2_DEFAULT_SETPOINT;
            settings->pid2.fpid.output_filter =     PID2_DEFAULT_OFILTER;
            settings->pid2.fpid.dterm_filter =      PID2_DEFAULT_DFILTER;
            settings->pid2.fpid.takebackhalf =      PID2_DEFAULT_TBH;
            settings->pid2.lock_window =            PID2_DEFAULT_LOCK_WINDOW;
            settings->pid2.lock_time =              PID2_DEFAULT_LOCK_TIME_MS;
            settings->pid2.input_filter =           PID2_DEFAULT_INPUT_FILTER;

        // End with the current version:
        case 5:
            _data_version = 5;
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

        // old, non convertible settings
        case 1: // 
        case 2: 
        case 3: 
        case 4: 
            set_defaults_since(0);
            _dirty = true;
            gui.showMessage("INFO", "Default settings loaded.");
            return true;

        // latest
        case 5:
            if(blob_size != sizeof(settings_t))
            {
                ERROR("Settings blob size mismatch (version is ok).");
                return false;
            };
            memcpy(_data, blob, blob_size);
            _data_version = 5;
            _dirty = false;
            return true;
    };

	return true;
};
