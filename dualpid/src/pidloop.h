#ifndef __PIDLOOP_H
#define __PIDLOOP_H

#include <FPID.h>

#include "config.h"
#include "sensors.h"
#include "driver/gpio.h"
#include "tools-nocopy.h"

class PIDLoop: private NonCopyable
{
    public:
        typedef enum
        {
            OUTPUT_MODE_NONE,      // Unconfigured, Display sensor only
            OUTPUT_MODE_ZP,        // Zero/Positive: heater
            // MODE_NZP,            // Negative/Zero/Positive: peltier off when in setpoint window
            OUTPUT_MODE_NP,        // Negative/Positive: peltier
            OUTPUT_MODE_NZ,        // Negative/Zero: cool only
        } output_mode_t;

        typedef enum
        {
            CONTROL_MODE_NONE,      // initial state
            // CONTROL_MODE_OFF,       // Channel is completly off
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
            STATUS_INACTIVE,        // PID off
            // CONTROL_MODE_PID
            STATUS_LOCKED,      
            STATUS_UNLOCKED,
            STATUS_SATURATED,       

            // CONTROL_MODE_FIXED
            STATUS_FIXED,       // Fixed value: set new value
        } status_t;

        typedef struct
        {
            bool active;
            int32_t output_mode;            
            int32_t pin_n;
            int32_t pin_p;
            int32_t looptime;
            int32_t windowtime;
            double min_output;
            double max_output;
            FPID::fpid_settings_t fpid;
            double lock_window;
            int32_t lock_time;
            double input_filter;
            int32_t sensor_type;
            int32_t fixed_output_value;
        } settings_t;

        PIDLoop(uint32_t id, PIDLoop::settings_t& s);

        bool begin();
        void loop();

        // inspection
        double input_value() { return _input_value; };
        double output_value() { return _output_value; };
        bool set_output_value(double value);
        status_t status() { return _status; };
        settings_t& _settings;

    private: 
        // priv functions
        void output_off();  // set output in a safe off-state: no heat, no cool

        // Internal loop steps
        void sync_mode();
        void do_sensor();
        void do_pid();
        void do_output();

        control_mode_t _mode = CONTROL_MODE_NONE;

        // SENSOR
        time_t _next_sensor = 0;
        sensor_begin_fptr _sensor_begin = nullptr;
        sensor_read_fptr _sensor_read = nullptr;

        // PID
        FPID _pid;
        double _input_value;
        double _output_value;
        time_t _next_pid, _last_pid;

        // OUTPUT
        // const double &_input_ref;
        status_t _status;
        time_t _windowstarttime;
        gpio_num_t _pin_n, _pin_p;

        uint32_t _id;
        time_t _unlocked_last;
};

#endif // __PIDLOOP_H