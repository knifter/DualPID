
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

PIDLoop::PIDLoop(uint32_t id, settings_t& s) :
    _settings(s), 
    _channel_id(id), 
    _pid(&(s.fpid), (&_input_value), (&_output_value))
{
    _output = new SlowPWMDriver();
};

PIDLoop::~PIDLoop()
{
    if(_output != nullptr)
        delete _output;
};

bool PIDLoop::begin()
{
   	_input_value = NAN;
    _output_value = NAN;
    _mode = CONTROL_MODE_NONE;
    _status = STATUS_NONE;
    _last_pid = 0;

    _output->begin(_channel_id);

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
			gui.showMessage("WARNING:", "Channel sensor error."); // FIXME x
			_sensor_read = nullptr;
	    };
    };

    _pid.setOutputLimits(_settings.min_output, _settings.max_output);
    DBG("ch%d: Output min:%.0f%% max:%.0f%%", _channel_id, _settings.min_output, _settings.max_output);
    _output_value = 50; // 50%
    _pid.alignOutput();

    // configure initial mode (depends on input/output available)
    control_mode_t initmode = CONTROL_MODE_NONE;
    if(_sensor_read != nullptr)
    {
        if(_output->begin_ok())
            initmode = CONTROL_MODE_INACTIVE;
        else
            initmode = CONTROL_MODE_SENSOR;
    };
    DBG("ch%d: Init with mode %s, output = %.0f%%", _channel_id, control_mode2str(initmode), _output_value);
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
            _output->off();

            _status = STATUS_INACTIVE;
            _mode = CONTROL_MODE_INACTIVE;
            break;
        case CONTROL_MODE_PID:
            if(!_output->begin_ok())
            {
                WARNING("No pid output mode set: pid remains in-active.");
                _mode = CONTROL_MODE_INACTIVE;
                _status = STATUS_INACTIVE;
                break;
            };

            _last_pid = 0; // Reset dt timer

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
    // See if its time to do another PID iteration
    // in in-active mode just update input value for display purposes
    time_t now = millis();
    if(_next_pid > now)
        return;
    _next_pid += _settings.looptime;

    // calc dt, first loop is just looptime
    if(_last_pid == 0)
        _last_pid = now - _settings.looptime;
    float dt = (now - _last_pid) / 1000.0;
    _last_pid = now;

    // If active: Run the inner pidloop
    bool pidres = false;
    float sp = 0;
    switch(_mode)
    {
        case CONTROL_MODE_FIXED: 
            sp = _settings.fixed_output_value; 
            break;
        case CONTROL_MODE_PID:   
            sp = _settings.fpid.setpoint; 
            pidres = _pid.calculate(dt);
            break;
        default: 
            sp = 0;
            break;
    };

    // always print status
#ifdef PIDLOOP_STATUS
    Serial.printf("ST%u:%.3f, %1d, %.3f, %.3f, %.3f\n", _channel_id, dt, _status, _input_value, sp, _output_value);
#endif
    // DBG("%lu: PID = %s: Input = %.2f, Setpoint = %.2f, Output = %.2f", 
    //     now, res?"ok":"sat", _input_value, _settings.fpid.setpoint, _output_value);

    // And, if not active, stop here
    if(_mode != CONTROL_MODE_PID)
        return;

    // If saturated, we're in error
    if(!pidres)
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

void PIDLoop::do_output()
{
    // turn of output if PID loop had an error
    if(isnan(_output_value))
    {
        _output->off();
        return;
    };

    // Output is only enabled when PID is ruuning or when Fixed Output is set (in expert_mode)
    if(_mode != CONTROL_MODE_PID && _mode != CONTROL_MODE_FIXED)
    {
        _output->off();
        return;
    };

    _output->set(_output_value);
};
