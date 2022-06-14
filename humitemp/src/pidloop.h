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
            MODE_NONE,      // Unconfigured
            MODE_ZP,        // Zero/Positive: heater
            MODE_NP,        // Negative/Positive: peltier
        } output_mode_t;

        typedef struct
        {
            bool active;
            PIDLoop::output_mode_t	mode;
            uint32_t pin_n;
            uint32_t pin_p;
            FPID::fpid_settings_t fpid;
            uint32_t looptime;
            double max_output;
        } pidloop_settings_t;

        PIDLoop(pidloop_settings_t& s, const double& input);

        bool begin();
        void loop();
        // double get_input() { return *_input; };
        double get_output() { return _output; };
        double get_output_percent() { return _output*100/PIDLOOP_WINDOWSIZE; };
        // int get_output_state() { return _output_state; };
        void set_active(bool);
        // bool active() { return _settings->active; };
        void reset_output();

    private: 
        FPID _pid;
        pidloop_settings_t &_settings;

        // double Input, Output, Setpoint;
        const double &_input_ref;
        double _output;
        time_t _windowstarttime;
        gpio_num_t _pin_n, _pin_p;
        time_t _pid_last;
        bool _active_last;
};

#endif // __PIDLOOP_H