#ifndef __PIDLOOP_H
#define __PIDLOOP_H

#include <FPID.h>

#include "config.h"
// #include "settings.h"
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
            // MODE_NZ,        // Negative/Zero: cool only
        } output_mode_t;

        typedef enum
        {
            STATUS_OFF,
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
            uint32_t lock_window;
            uint32_t lock_time;
        } settings_t;

        PIDLoop(settings_t& s, const double& input);

        bool begin();
        void loop();
        // double get_input() { return *_input; };
        double get_output() { return _output; };
        double get_output_percent() { return _output*100/_settings.windowtime; };
        status_t get_status() { return _status; };
        // int get_output_state() { return _output_state; };
        void set_active(bool);
        // bool active() { return _settings->active; };
        void reset_output();

    private: 
        void calculate();

        FPID _pid;
        settings_t &_settings;

        // double Input, Output, Setpoint;
        const double &_input_ref;
        double _output;
        status_t _status;
        time_t _windowstarttime;
        gpio_num_t _pin_n, _pin_p;
        time_t _pid_last;
        bool _active_last;
        time_t _unlocked_last;
};

#endif // __PIDLOOP_H