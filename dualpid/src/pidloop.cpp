
#include "pidloop.h"

#include <Arduino.h>
#include <FPID.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "tools-log.h"
#include "sensors.h"

PIDLoop::PIDLoop(settings_t& s) :
    _pid(&(s.fpid), (&_input_value), (&_output_value)),
    _settings(s)
{
};

bool PIDLoop::begin()
{
    _pin_n = static_cast<gpio_num_t>(_settings.pin_n);
    _pin_p = static_cast<gpio_num_t>(_settings.pin_p);

    // config hardware
  	pinMode(_pin_n, OUTPUT);
  	pinMode(_pin_p, OUTPUT);
	digitalWrite(_pin_n, LOW);
	digitalWrite(_pin_p, LOW);

	_windowstarttime = millis();
    _status = STATUS_DISABLED;
   	_input_value = NAN;
    _output_value = NAN;

	_sensor_begin = find_sensor_begin(_settings.sensor_type);
	_sensor_read = find_sensor_read(_settings.sensor_type);

	if(_sensor_begin != nullptr)
    {
    	if(!_sensor_begin())  
    	{
			gui.showMessage("WARNING:", "Channel x sensor error."); // FIXME x
			_sensor_read = nullptr;
	    };
    };

    switch(_settings.mode)
    {
        case MODE_NONE:
            set_active(false);
        	_pid.setOutputLimits(0, 100);
            break;
        case MODE_NP:
        	_pid.setOutputLimits(-100, 100);
            break;
        case MODE_ZP:
        	_pid.setOutputLimits(0, 100);
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
        	_output_value = _settings.windowtime / 2;
            break;
        case MODE_ZP:
	        _output_value = 0;
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
        _status = STATUS_DISABLED;
    };

    if(active)
    {
        _pid.alignOutput();
        reset_output();

        _status = STATUS_UNLOCKED;
    } else {
        if(_pin_n)
            digitalWrite(_pin_n, LOW);
        if(_pin_p)
            digitalWrite(_pin_p, LOW);

        _status = STATUS_INACTIVE;
    };
    _settings.active = active;
    _active_last = active;
};

void PIDLoop::loop()
{
    do_sensor();
    do_pid();
    do_output();
};

void PIDLoop::do_sensor()
{
    // Read sensor?
	time_t now = millis();
	if(_next_sensor > now)
        return;

    _next_sensor = now + settings.sensor_loop_ms;

    // Read sensors and apply averaging/filter
    if(_sensor_read != nullptr)
    {
        double read = _sensor_read();
        if(isnan(_input_value))
            _input_value = read;

        // apply input filter
        _input_value = read*(1 - _settings.input_filter) + _input_value*_settings.input_filter;
    }else{
        _input_value = NAN;
    };
};

void PIDLoop::do_pid()
{
    // If not configured as PID, nothing further to do
    if(_settings.mode == MODE_NONE)
    {
        _status = STATUS_DISABLED;
        return;
    };

    // Follow _settings when it changes.
    if(_active_last != _settings.active)
    {
        set_active(_settings.active);
    };

    // If not active, don't run
    if(!_settings.active)
    {
        // DBG("PID: Input = %.2f, In-Active, Output = %.2f", _input_ref, _output);
        _status = STATUS_INACTIVE;
        return;
    };

    // See if its time to do another PID iteration
    // in in-active mode just update input value for display purposes
    time_t now = millis();
    if(_next_pid > now)
        return;
    _next_pid = now + _settings.looptime;

    // Run the inner pidloop
    bool res = _pid.calculate();

    // DBG("%u: PID = %s: Input = %.2f, Setpoint = %.2f, Output = %.2f", 
    //     now, res?"ok":"err", _input_ref, _settings.fpid.setpoint, _output);

    // If saturated, we're in error
    if(!res)
    {
        _status = STATUS_ERROR;
        _unlocked_last = now;
        return;
    };

    if(abs(_input_value - _settings.fpid.setpoint) > _settings.lock_window)
    {
        // UNLOCKED
        _status = STATUS_UNLOCKED;
        _unlocked_last = now;
        return;
    };

    // In lock window, long enough?
    if(now - _unlocked_last < _settings.lock_time)
    {
        _status = STATUS_UNLOCKED;
        return;
    };

    // LOCKED
    _status = STATUS_LOCKED;
};

void PIDLoop::do_output()
{
    // turn of output if PID loop had an error
    if(isnan(_output_value))
    {
        // TODO: Set in-active - depending on mode what that is
        if(_pin_n)
            digitalWrite(_pin_n, LOW);
        if(_pin_p)
            digitalWrite(_pin_p, LOW);
        return;
    };

    // Process timewindow valve depending on PID Output
    time_t now = millis();
    if (now - _windowstarttime > _settings.windowtime)
    { //time to shift the Relay Window
        _windowstarttime += _settings.windowtime;
    };

    // Set output
    uint8_t N = LOW;
    uint8_t P = LOW;
    uint32_t output_time = _output_value * _settings.windowtime / 100;
    switch(_settings.mode)
    {
        case MODE_NONE:
            break;
        case MODE_NZ:   // Negative-Zero
            if (output_time <= (now - _windowstarttime))
                N = HIGH;
            break;
        case MODE_NP:   // Negative or Positive
            if (output_time >= (now - _windowstarttime))
                P = HIGH;
            else
                N = HIGH;
            break;
        // case MODE_NZP:
        //     break;
        case MODE_ZP:
            if (output_time >= (now - _windowstarttime))
                P = HIGH;
            break;
    };
    if(_pin_n)
        digitalWrite(_pin_n, N);
    if(_pin_p)
        digitalWrite(_pin_p, P);
};
