#ifndef __SETTINGS_H
#define __SETTINGS_H

#include <tools-nvs.h>

// #include "config.h"
// #include "globals.h"
#include "pidloop.h"

typedef struct
{
    uint32_t sensor_loop_ms;         // Sensor poll loop-time
	PIDLoop::pidloop_settings_t pid1;
	PIDLoop::pidloop_settings_t pid2;
} settings_t;

class SettingsManager : public NVSettings
{
	public:
        SettingsManager(settings_t&);

    private:
        bool set_defaults_since(const uint32_t data_version);
        bool read_blob(void* blob, const size_t blob_size, const uint32_t blob_version);

};

#endif //__SETTINGS_H

