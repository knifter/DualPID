#ifndef __PIDLOOP_H
#define __PIDLOOP_H

#include <FPID.h>

#include "config.h"
#include "sensors.h"
#include "driver/gpio.h"

class PIDLoop
{
    public:
        typedef enum
        {
            MODE_NONE,      // Unconfigured, Display sensor only
            MODE_ZP,        // Zero/Positive: heater
            // MODE_NZP,       // Negative/Zero/Positive: peltier off when in setpoint window
            MODE_NP,        // Negative/Positive: peltier
            MODE_NZ,        // Negative/Zero: cool only
        } output_mode_t;

        typedef enum
        {
            STATUS_DISABLED,
            STATUS_INACTIVE,
            STATUS_LOCKED,
            STATUS_UNLOCKED,
            STATUS_ERROR
        } status_t;

        typedef struct
        {
            bool active;
            uint32_t mode;
            uint32_t pin_n;
            uint32_t pin_p;
            uint32_t looptime;
            uint32_t windowtime;
            double max_output;
            FPID::fpid_settings_t fpid;
            double lock_window;
            uint32_t lock_time;
            double input_filter;
            uint32_t sensor_type;
        } settings_t;

        PIDLoop(settings_t& s);

        bool begin();
        void loop();

        // inspection
        double input_value() { return _input_value; };
        double output_value() { return _output_value; };
        status_t status() { return _status; };
        settings_t pid_settings() { return _settings; };
        // int get_output_state() { return _output_state; };

    private: 
        // priv functions
        void set_active(bool);
        // bool active() { return _settings->active; };
        void reset_output();

        void do_sensor();
        void do_pid();
        void do_output();
        void calculate();

        // SENSOR
        time_t _next_sensor = 0;
        sensor_begin_fptr _sensor_begin = nullptr;
        sensor_read_fptr _sensor_read = nullptr;

        // PID
        FPID _pid;
        double _input_value;
        double _output_value;
        time_t _next_pid;
        settings_t &_settings;

        // OUTPUT
        // const double &_input_ref;
        status_t _status;
        time_t _windowstarttime;
        gpio_num_t _pin_n, _pin_p;

        bool _active_last;
        time_t _unlocked_last;
};

#endif // __PIDLOOP_H