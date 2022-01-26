#ifndef __PIDLOOP_H
#define __PIDLOOP_H

#include <FPID.h>

#include "config.h"
#include "settings.h"
#include "driver/gpio.h"

typedef double (*pid_value_callback_ptr)();

class PIDLoop // : protected MiniPID
{
    public:
        PIDLoop(gpio_num_t pin_a, gpio_num_t pin_b, pid_value_callback_ptr func, FPID::fpid_settings_t* pidsettings) :
            _pid(pidsettings, &_input, &_output),
            _pidsettings(pidsettings),
            _cb_value(func),
            _pin_a(pin_a), 
            _pin_b(pin_b)
            {};

        bool begin();
        // void set_tuning(double P, double I, double D);
        // void set_tuning(pidsettings_t&);
        // void set_setpoint(double);
        void loop();
        double get_input() { return _input; };
        double get_output() { return _output; };
        double get_output_percent() { return _output*100/PID_WINDOWSIZE; };
        int get_output_state() { return _output_state; };

    private: 
        FPID _pid;
        FPID::fpid_settings_t *_pidsettings = nullptr;
        pid_value_callback_ptr _cb_value;

        // double Input, Output, Setpoint;
        double _input, _output;
        int _output_state;
        time_t _windowstarttime;
        gpio_num_t _pin_a, _pin_b;
        time_t _pid_last;
};

#endif // __PIDLOOP_H