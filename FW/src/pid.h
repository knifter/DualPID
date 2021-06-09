#ifndef __PID_H
#define __PID_H

#include <MiniPID.h>

#include "config.h"
#include "settings.h"

typedef double (*pid_value_callback_ptr)();

class PIDLoop // : protected MiniPID
{
    public:
        PIDLoop(int pin_a, int pin_b, pid_value_callback_ptr func) :
            _pid(DEFAULT_PID_P, DEFAULT_PID_I, DEFAULT_PID_D)
            {};

        bool begin();
        void set_tuning(double P, double I, double D);
        void set_tuning(pidsettings_t&);
        void set_setpoint(double);
        void loop();
        double get_output();

    private: 
        MiniPID _pid;

        // double Input, Output, Setpoint;
        double _input, _output;
        unsigned long _windowstarttime;
        int _pin_a, _pin_b;
};

#endif // __PID_H