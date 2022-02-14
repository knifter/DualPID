
#include "pidloop.h"

#include <Arduino.h>
#include <FPID.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "tools-log.h"

PIDLoop::PIDLoop(gpio_num_t pin_a, gpio_num_t pin_b, pid_value_callback_ptr func, pidloop_settings_t* s) :
    _pid(&(s->fpid), &_input, &_output),
    _settings(s),
    _cb_value(func),
    _pin_a(pin_a), 
    _pin_b(pin_b)
{
};

bool PIDLoop::begin()
{
	digitalWrite(_pin_a, LOW);
	digitalWrite(_pin_b, LOW);

	_windowstarttime = millis();
	_input = NAN;
    _pid_last = millis();

    switch(_settings->mode)
    {
        case MODE_NONE:
            set_active(false);
        	_pid.setOutputLimits(0,0);
            break;
        case MODE_NP:
        	_pid.setOutputLimits(0, PIDLOOP_WINDOWSIZE);
            break;
        case MODE_ZP:
        	_pid.setOutputLimits(0, PIDLOOP_WINDOWSIZE);
            break;
    };

    reset_output();

    set_active(_settings->active);

	return true;
};

void PIDLoop::reset_output()
{
    switch(_settings->mode)
    {
        case MODE_NONE:
            break;
        case MODE_NP:
        	_output = PIDLOOP_WINDOWSIZE / 2;
            break;
        case MODE_ZP:
	        _output = 0;
            break;
    };
};

void PIDLoop::set_active(bool active)
{
    DBG("set_active(%s)", active ? "true":"false");

    if(_settings->mode == MODE_NONE)
    {
        WARNING("No pid output mode set: pid remains in-active.");
        active = false;
    };

    if(active)
    {
        _pid.alignOutput();
        reset_output();
    } else {
        digitalWrite(_pin_a, LOW);
        digitalWrite(_pin_b, LOW);
    };
    _settings->active = active;
    _active_last = active;
};

void PIDLoop::loop()
{
    if(_active_last != _settings->active)
    {
        set_active(_settings->active);
    };

    // See if its time to do another PID iteration
    // in in-active mode just update input value for display purposes
    time_t now = millis();
    if(now > _pid_last + PIDLOOP_LOOP_MS)
    {
        // Input for the PID
        _input = _cb_value();
        if(isnan(_input))
        {
            // Sensor error
            WARNING("Sensor error. Going back to fail-safe.");
            digitalWrite(_pin_a, LOW);
            digitalWrite(_pin_b, LOW);
            return;
        };

        if(_settings->active)
        {
            double dt = (now - _pid_last) / PIDLOOP_LOOP_MS;
            _pid.calculate(dt);

            // DBG("PID: Input = %.2f, Setpoint = %.2f, Output = %.2f (dt = %.9f)", _input, _settings->fpid.setpoint, _output, dt);
        }else{
            // DBG("PID: Input = %.2f, In-Active, Output = %.2f", _input, _output);
        };

        // Queue next iteration
        _pid_last = now;
    };

    // dont update output if inactive
    if(!_settings->active)
        return;

    // Process timewindow valve depending on PID Output
    if (now - _windowstarttime > PIDLOOP_WINDOWSIZE)
    { //time to shift the Relay Window
        _windowstarttime += PIDLOOP_WINDOWSIZE;
    };

    // Set output
    uint A = LOW;
    uint B = LOW;
    switch(_settings->mode)
    {
        case MODE_NONE:
            break;
        case MODE_NP:
            if (_output > (now - _windowstarttime))
                B = HIGH;
            else
                A = HIGH;
            break;
        case MODE_ZP:
            if (_output > (now - _windowstarttime))
                B = HIGH;
            break;
    };
    digitalWrite(_pin_a, A);
    digitalWrite(_pin_b, B);
};
