
#include "pidloop.h"

#include <Arduino.h>
#include <FPID.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "tools-log.h"

PIDLoop::PIDLoop(pidloop_settings_t& s, const double& input) :
    _pid(&(s.fpid), (&input), (&_output)),
    _settings(s), _input_ref(input)
{
};

bool PIDLoop::begin()
{
    _pin_n = static_cast<gpio_num_t>(_settings.pin_n);
    _pin_p = static_cast<gpio_num_t>(_settings.pin_p);
    // settings pin to >35 will make the digitalWrite do nothing
    if(!_pin_n)
        _pin_n = GPIO_NUM_MAX;
    if(!_pin_p)
        _pin_p = GPIO_NUM_MAX;

    // config hardware
  	pinMode(_pin_n, OUTPUT);
  	pinMode(_pin_p, OUTPUT);
	digitalWrite(_pin_n, LOW);
	digitalWrite(_pin_p, LOW);

	_windowstarttime = millis();
    _pid_last = millis();

    switch(_settings.mode)
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

    set_active(_settings.active);

	return true;
};

void PIDLoop::reset_output()
{
    switch(_settings.mode)
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

    if(_settings.mode == MODE_NONE)
    {
        WARNING("No pid output mode set: pid remains in-active.");
        active = false;
    };

    if(active)
    {
        _pid.alignOutput();
        reset_output();
    } else {
        digitalWrite(_pin_n, LOW);
        digitalWrite(_pin_p, LOW);
    };
    _settings.active = active;
    _active_last = active;
};

void PIDLoop::loop()
{
    if(_active_last != _settings.active)
    {
        set_active(_settings.active);
    };

    // See if its time to do another PID iteration
    // in in-active mode just update input value for display purposes
    time_t now = millis();
    if(now > _pid_last + PIDLOOP_LOOP_MS)
    {
        // Input for the PID
        if(isnan(*_input))
        {
            // Sensor error
            // WARNING("Sensor error. Going back to fail-safe.");
            digitalWrite(_pin_n, LOW);
            digitalWrite(_pin_p, LOW);
            return;
        };
        if(_settings.active)
        {
            double dt = (now - _pid_last) / PIDLOOP_LOOP_MS;
            _pid.calculate(dt);

            // DBG("PID: Input = %.2f, Setpoint = %.2f, Output = %.2f (dt = %.9f)", _input_ref, _settings.fpid.setpoint, _output, dt);
        }else{
            // DBG("PID: Input = %.2f, In-Active, Output = %.2f", _input, _output);
        };

        // Queue next iteration
        _pid_last = now;
    };

    // turn of output if not active or if PID loop had an error
    if(!_settings.active || isnan(_output))
        return;

    // Process timewindow valve depending on PID Output
    if (now - _windowstarttime > PIDLOOP_WINDOWSIZE)
    { //time to shift the Relay Window
        _windowstarttime += PIDLOOP_WINDOWSIZE;
    };

    // Set output
    uint A = LOW;
    uint B = LOW;
    switch(_settings.mode)
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
    digitalWrite(_pin_n, A);
    digitalWrite(_pin_p, B);
};
