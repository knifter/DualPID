
#include "pidloop.h"

#include <Arduino.h>
#include <FPID.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "tools-log.h"
#include "sensors.h"

const char* control_mode2str(PIDLoop::control_mode_t mode)
{
    switch(mode)
    {
        case PIDLoop::CONTROL_MODE_NONE:    return "(none)";
        // case PIDLoop::CONTROL_MODE_OFF:     return "CHANNEL_OFF";
        case PIDLoop::CONTROL_MODE_SENSOR:  return "SENSOR";
        case PIDLoop::CONTROL_MODE_INACTIVE:return "INACTIVE";
        case PIDLoop::CONTROL_MODE_PID:     return "PID";
        case PIDLoop::CONTROL_MODE_FIXED:   return "FIXED";
    };
    return "(unknown)";
};

const char* status2str(PIDLoop::status_t status)
{
    switch(status)
    {
        case PIDLoop::STATUS_NONE:          return "(none)";
        case PIDLoop::STATUS_SENSOR:        return "SENSOR";
        case PIDLoop::STATUS_INACTIVE:      return "OFF";
        case PIDLoop::STATUS_LOCKED:        return "LOCKED";
        case PIDLoop::STATUS_UNLOCKED:      return "UNLOCKED";
        case PIDLoop::STATUS_SATURATED:     return "SATURATED";
        case PIDLoop::STATUS_FIXED:         return "FIXED";
    };
    return "(unknown)";
};

PIDLoop::PIDLoop(settings_t& s) :
    _settings(s), _pid(&(s.fpid), (&_input_value), (&_output_value))
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
   	_input_value = NAN;
    _output_value = NAN;
    _mode = CONTROL_MODE_NONE;
    _status = STATUS_NONE;
    if(!::settings.expert_mode)
    {
        // Reset fixed output if not in export_mode anymore
        _settings.fixed_output_value = 0;
    };

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

    switch(_settings.output_mode)
    {
        case OUTPUT_MODE_NONE:
            DBG("No output mode configured.");
        	_pid.setOutputLimits(0, 0);
            _output_value = 0;
            break;
        case OUTPUT_MODE_ZP:
        	_pid.setOutputLimits(_settings.min_output, _settings.max_output);
            DBG("Output mode: ZP, min:%.0f%% max:%.0f%%", _settings.min_output, _settings.max_output);
        	_output_value = 50; // 50%
            _pid.alignOutput();
            break;
        case OUTPUT_MODE_NP:
            DBG("Output mode: NP");
        	_pid.setOutputLimits(-1*_settings.max_output, _settings.max_output);
        	_output_value = 0; // 0%
            _pid.alignOutput();
            break;
        case OUTPUT_MODE_NZ:
            DBG("Output mode: NZ");
        	_pid.setOutputLimits(-1*_settings.max_output, -1*_settings.min_output);
	        _output_value = -50;
            _pid.alignOutput();
            break;
    };

    // configure initial mode (depends on input/output available)
    control_mode_t initmode = CONTROL_MODE_NONE;
    if(_sensor_read != nullptr)
    {
        initmode = CONTROL_MODE_SENSOR;
        if(_settings.output_mode != OUTPUT_MODE_NONE)
            initmode = CONTROL_MODE_INACTIVE;
    };
    DBG("Init with mode %s, output = %.0f%%", control_mode2str(initmode), _output_value);
    set_mode(initmode);
    // sync_mode will set it active when _settings have stored so

	return true;
};

void PIDLoop::sync_mode()
{
    // _settings might change due to menu options, they will be synced with the PIDLoop here

    // Follow _settings when it changes.
    switch(_mode)
    {
        case CONTROL_MODE_NONE:
        case CONTROL_MODE_SENSOR:
            if(_settings.active)
            {
                WARNING("Setting sensor-only channel to inactive. Should not be active.");
                _settings.active = false;
            };
            break;
        case CONTROL_MODE_INACTIVE:
            if(_settings.active)
            {
                if(::settings.expert_mode && _settings.fixed_output_value > 0)
                {
                    DBG("Activating Fixed-Output.");
                    set_mode(CONTROL_MODE_FIXED);
                    set_output_value(_settings.fixed_output_value);
                    break;
                };
                DBG("Activating PID because of setting change.");
                set_mode(CONTROL_MODE_PID);
            };
            break;
        case CONTROL_MODE_PID:
            if(!_settings.active)
            {
                DBG("De-activating PID because of setting change.");
                set_mode(CONTROL_MODE_INACTIVE);
            };
            break;
        case CONTROL_MODE_FIXED:
            if(!_settings.active)
            {
                set_mode(CONTROL_MODE_INACTIVE);
            };
            break;
    };
};

void PIDLoop::set_mode(control_mode_t newmode)
{
    DBG("set_mode(%s -> %s)", control_mode2str(_mode), control_mode2str(newmode));

    if(newmode == _mode)
    {
        DBG("Mode is already %s, skip.", control_mode2str(newmode));
        return;
    };

    switch(newmode)
    {
        case CONTROL_MODE_SENSOR:
            _status = STATUS_SENSOR;
            _mode = CONTROL_MODE_SENSOR;
            break;
        case CONTROL_MODE_NONE:
        case CONTROL_MODE_INACTIVE:
            output_off();

            _status = STATUS_INACTIVE;
            _mode = CONTROL_MODE_INACTIVE;
            break;
        case CONTROL_MODE_PID:
            if(_settings.output_mode == OUTPUT_MODE_NONE)
            {
                WARNING("No pid output mode set: pid remains in-active.");
                _mode = CONTROL_MODE_INACTIVE;
                _status = STATUS_INACTIVE;
                break;
            };

            // align FPID internals with current _output_value
            _pid.alignOutput();

            _status = STATUS_UNLOCKED;
            _mode = CONTROL_MODE_PID;
            break;
        case CONTROL_MODE_FIXED:
        
            _status = STATUS_FIXED;
            _mode = CONTROL_MODE_FIXED;
            break;
    };
    
};

void PIDLoop::loop()
{
    sync_mode();
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
    _next_sensor = now + settings.sensor_loop_ms; // FIXME: rather absolute dT

    // Read sensors and apply averaging/filter
    if(_sensor_read == nullptr)
    {
        _input_value = NAN;
        return;
    };

    double read = _sensor_read();

    // reset filter if previous was NaN
    if(isnan(_input_value))
        _input_value = read;

    // apply input filter
    _input_value = read*(1 - _settings.input_filter) + _input_value*_settings.input_filter;
};

void PIDLoop::do_pid()
{
    // If not active, don't run
    if(_mode != CONTROL_MODE_PID)
    {
        // DBG("PID: Input = %.2f, In-Active, Output = %.2f", _input_ref, _output);
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

    DBG("%lu: PID = %s: Input = %.2f, Setpoint = %.2f, Output = %.2f", 
        now, res?"ok":"sat", _input_value, _settings.fpid.setpoint, _output_value);

    // If saturated, we're in error
    if(!res)
    {
        _status = STATUS_SATURATED;
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

bool PIDLoop::set_output_value(double value)
{
    // Do not touch output if PID is in control (or if we don't have an output..)
    switch(_mode)
    {
        case CONTROL_MODE_INACTIVE:
        case CONTROL_MODE_FIXED:
            break;
        default:
            WARNING("Can't set output when PID Running, ignored.");
            return false;
    };

    if(value > _settings.max_output)
    {
        ERROR("set_output value > max_output.");
        return false;
    };
    if(value < _settings.min_output)
    {
        ERROR("set_output value < min_output.");
        return false;
    };

    _output_value = value;
    _status = STATUS_FIXED;
    return true;
};

void PIDLoop::output_off()
{
    if(_pin_n)
        digitalWrite(_pin_n, LOW);
    if(_pin_p)
        digitalWrite(_pin_p, LOW);
};

void PIDLoop::do_output()
{
    // turn of output if PID loop had an error
    if(isnan(_output_value))
    {
        output_off();
        return;
    };

    // Output is only enabled when PID is ruuning or when Fixed Output is set (in expert_mode)
    if(_mode != CONTROL_MODE_PID && _mode != CONTROL_MODE_FIXED)
    {
        output_off();
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
    switch(_settings.output_mode)
    {
        case OUTPUT_MODE_NONE:
            break;
        case OUTPUT_MODE_NZ:   // Negative-Zero
            if (output_time <= (now - _windowstarttime))
                N = HIGH;
            break;
        case OUTPUT_MODE_NP:   // Negative or Positive
            if (output_time >= (now - _windowstarttime))
                P = HIGH;
            else
                N = HIGH;
            break;
        // case MODE_NZP:
        //     break;
        case OUTPUT_MODE_ZP:
            if (output_time >= (now - _windowstarttime))
                P = HIGH;
            break;
    };
    if(_pin_n)
        digitalWrite(_pin_n, N);
    if(_pin_p)
        digitalWrite(_pin_p, P);
};
