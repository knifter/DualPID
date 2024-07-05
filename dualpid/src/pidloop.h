#ifndef __PIDLOOP_H
#define __PIDLOOP_H

#include <FPID.h>

#include "config.h"
#include "inputdrv.h"
#include "driver/gpio.h"
#include "tools-nocopy.h"
#include "outputdrv.h"

class PIDLoop: private NonCopyable
{
    public:
        typedef enum
        {
            CONTROL_MODE_NONE,      // initial state
            CONTROL_MODE_DISABLED,  // Channel is completly off
            CONTROL_MODE_SENSOR,    // sensor is polled, no output
            CONTROL_MODE_INACTIVE,  // _output_value is set to approriate off-state
            CONTROL_MODE_PID,       // _output_value controlled by PID output
            CONTROL_MODE_FIXED,     // _output_value set to fixed value by user
        } control_mode_t;
        void set_mode(control_mode_t newmode);

        typedef enum
        {
            // CONTROL_MODE_NONE
            STATUS_NONE,

            // CONTROL_MODE_SENSOR
            STATUS_SENSOR,

            // CONTROL_MODE_OFF
            STATUS_DISABLED,        // PID off, display channel off
            STATUS_INACTIVE,        // PID off
            // CONTROL_MODE_PID
            STATUS_ERROR,           // 3
            STATUS_UNLOCKED,        // 
            STATUS_LOCKED_WAIT,     // 
            STATUS_LOCKED,          // 6 

            // CONTROL_MODE_FIXED
            STATUS_FIXED,       // Fixed value: set new value
        } status_t;

        typedef struct
        {
            bool active;
            int32_t looptime;
            int32_t output_drv;
            output_driver_config_t output;

            double min_output;
            double max_output;
            FPID::fpid_settings_t fpid;
            int32_t lock_window;    // 1000 == 1%
            int32_t _reserved; // left from previous double lock_window
            int32_t lock_time;
            double input_filter;
            int32_t input_drv;
            // input_driver_config_t input_cfg;
            int32_t fixed_output_value;
        } settings_t;

        PIDLoop(uint32_t id, PIDLoop::settings_t& s);
        ~PIDLoop();

        bool begin();
        void loop();

        // inspection
        double input_value() { return _input_value; };
        InputDriver* input_drv() { return _inputdrv; };
        
        double output_value() { return _output_value; };
        bool set_output_value(double value);
        status_t status() { return _status; };
        control_mode_t mode() { return _mode; };
        settings_t& _settings;

    private: 
        // Internal loop steps
        void sync_mode();
        void do_sensor();
        void do_pid();
        void do_output();

        uint32_t _channel_id;
        control_mode_t _mode = CONTROL_MODE_NONE;
        status_t _status;

        // SENSOR
        time_t _next_input = 0;
        InputDriver* _inputdrv;
        bool _inputdrv_ok;
        // sensor_begin_fptr _sensor_begin = nullptr;
        // sensor_read_fptr _sensor_read = nullptr;

        // PID
        FPID _pid;
        double _input_value;
        double _output_value;
        time_t _next_pid, _last_pid;
        time_t _unlocked_last;

        // OUTPUT
        OutputDriver* _outputdrv;
        bool _outputdrv_ok;
};

#endif // __PIDLOOP_H