#ifndef __PIDLOOP_H
#define __PIDLOOP_H

#include <FPID.h>

#include "config.h"
// #include "settings.h"
#include "driver/gpio.h"

typedef double (*pid_value_callback_ptr)();

class PIDLoop
{
    public:
        typedef enum
        {
            MODE_NONE,      // Unconfigured
            MODE_ZP,        // Zero/Positive: heater
            MODE_NP,        // Negative/Positive: peltier
        } output_mode_t;

        typedef struct
        {
            gpio_num_t pin_a, pin_b;
            bool active;
            PIDLoop::output_mode_t	mode;
            FPID::fpid_settings_t fpid;
        } pidloop_settings_t;

        PIDLoop(gpio_num_t pin_a, gpio_num_t pin_b, pid_value_callback_ptr func, pidloop_settings_t* s);

        bool begin();
        // void set_tuning(double P, double I, double D);
        // void set_tuning(pidsettings_t&);
        // void set_setpoint(double);
        void loop();
        double get_input() { return _input; };
        double get_output() { return _output; };
        double get_output_percent() { return _output*100/PID_WINDOWSIZE; };
        // int get_output_state() { return _output_state; };
        void set_active(bool);
        // bool active() { return _settings->active; };
        void reset_output();

    private: 
        FPID _pid;
        pidloop_settings_t *_settings;
        pid_value_callback_ptr _cb_value;

        // double Input, Output, Setpoint;
        double _input, _output;
        time_t _windowstarttime;
        gpio_num_t _pin_a, _pin_b;
        time_t _pid_last;
        bool _active_last;
};

#endif // __PIDLOOP_H