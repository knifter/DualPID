
#include "pidloop.h"

#include <Arduino.h>
#include <FPID.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "tools-log.h"
#include "inputdrv.h"

const char* control_mode2str(PIDLoop::control_mode_t mode)
{
    switch(mode)
    {
        case PIDLoop::CONTROL_MODE_NONE:     return "(none)";
        case PIDLoop::CONTROL_MODE_DISABLED: return "DISABLED";
        case PIDLoop::CONTROL_MODE_SENSOR:   return "SENSOR";
        case PIDLoop::CONTROL_MODE_INACTIVE: return "INACTIVE";
        case PIDLoop::CONTROL_MODE_PID:      return "PID";
        case PIDLoop::CONTROL_MODE_FIXED:    return "FIXED";
    };
    return "(unknown)";
};

const char* status2str(PIDLoop::status_t status)
{
    switch(status)
    {
        case PIDLoop::STATUS_NONE:          return "(none)";
        case PIDLoop::STATUS_DISABLED:      return "DISABLED";
        case PIDLoop::STATUS_SENSOR:        return "SENSOR";
        case PIDLoop::STATUS_INACTIVE:      return "OFF";
        case PIDLoop::STATUS_ERROR:         return "ERROR";
        case PIDLoop::STATUS_UNLOCKED:      return "UNLOCKED";
        case PIDLoop::STATUS_LOCKED_WAIT:   return "WAIT";
        case PIDLoop::STATUS_LOCKED:        return "LOCKED";
        case PIDLoop::STATUS_FIXED:         return "FIXED";
    };
    return "(unknown)";
};

PIDLoop::PIDLoop(uint32_t id, settings_t& s) :
    _settings(s), 
    _channel_id(id), 
    _pid(&(s.fpid), (&_input_value), (&_output_value)),
    _outputdrv(nullptr)
{
};

PIDLoop::~PIDLoop()
{
    if(_outputdrv != nullptr)
        delete _outputdrv;
};

bool PIDLoop::begin()
{
   	_input_value = NAN;
    _output_value = NAN;
    _mode = CONTROL_MODE_NONE;
    _status = STATUS_NONE;
    _last_pid = 0;
    _inputdrv_ok = false;
    _outputdrv_ok = false;

    // Configure InputDriver
    //  if configured and works mode can go up to SENSOR
    switch(_settings.input_drv)
    {
        default: _inputdrv = new NoneInputDriver(); break;
        case INPUT_DRIVER_SHT31_TEMP:   _inputdrv = new SHT31TDriver(); break;
        case INPUT_DRIVER_SHT31_RH:     _inputdrv = new SHT31RHDriver(); break;
        case INPUT_DRIVER_M5KMETER:     _inputdrv = new M5KMeterDriver(); break;
        case INPUT_DRIVER_MCP9600:      _inputdrv = new MCP9600Driver(); break;
        case INPUT_DRIVER_MAX31865:     _inputdrv = new MAX31865Driver(); break;
        case INPUT_DRIVER_SPRINTIR:     _inputdrv = new SprintIR20Driver(); break;
    };
    if(_settings.input_drv != INPUT_DRIVER_NONE)
    {
        if(!_inputdrv->begin())
        {
            gui.showMessage("WARNING:", "Channel InputDriver error.");
            ERROR("Channel %d input.begin() failed.", _channel_id);
            _inputdrv_ok = false;
        }else{
            DBG("Channel %d: input(%d).begin() == ok", _channel_id, _settings.input_drv);
            _inputdrv_ok = true;
        };
    };

    // Configure OutputDriver
    switch(_settings.output_drv)
    {
        // TODO: default: _outputdrv = new NoneOutputDriver(); break;
        case OUTPUT_DRIVER_NONE:    _outputdrv = nullptr; break;
        case OUTPUT_DRIVER_SLOWPWM: _outputdrv = new SlowPWMDriver(); break;
        case OUTPUT_DRIVER_FASTPWM: _outputdrv = new FastPWMDriver(); break;
    };

    if(_settings.output_drv != OUTPUT_DRIVER_NONE)
    {
        if(!_outputdrv->begin(_channel_id))
        {
            gui.showMessage("WARNING:", "Channel OutputDriver error.");
            ERROR("Channel %d output.begin() failed.", _channel_id);
            _outputdrv_ok = false;
        }else{
            DBG("Channel %d output.begin() == ok", _channel_id);
            _outputdrv_ok = true;
        };
    };

    if(!::settings.expert_mode)
    {
        // Reset fixed output if not in export_mode anymore
        _settings.fixed_output_value = 0;
    };

    _pid.setOutputLimits(_settings.min_output, _settings.max_output);
    DBG("ch%d: Output min:%.0f%% max:%.0f%%", _channel_id, _settings.min_output, _settings.max_output);
    _output_value = NAN;
    _pid.alignOutput();

    // configure initial mode (depends on input/output available)
    control_mode_t initmode = CONTROL_MODE_DISABLED;
    if(_inputdrv_ok)
    {
        initmode = CONTROL_MODE_SENSOR;
    };
    if(_outputdrv_ok) // even if input failed: set fixed is available
    {
        initmode = CONTROL_MODE_INACTIVE;
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
        case CONTROL_MODE_DISABLED:
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
        case CONTROL_MODE_DISABLED:
            if(_outputdrv)
                _outputdrv->off();

            _status = STATUS_DISABLED;
            _mode = CONTROL_MODE_DISABLED;
            break;

        case CONTROL_MODE_SENSOR:
            if(!_inputdrv_ok)
            {
                WARNING("Can't set channeld to SENSOR: No valid input.");
                _status = STATUS_DISABLED;
                _mode = CONTROL_MODE_DISABLED;
                break;
            };

            _status = STATUS_SENSOR;
            _mode = CONTROL_MODE_SENSOR;
            break;

        case CONTROL_MODE_NONE:
        case CONTROL_MODE_INACTIVE:
            if(!_outputdrv_ok)
            {
                WARNING("Can't set channel to INACTIVE: No working output driver.");
                set_mode(CONTROL_MODE_SENSOR); // try input-only mode, otherwise disable
                return;
            };

            _status = STATUS_INACTIVE;
            _mode = CONTROL_MODE_INACTIVE;
            break;
        case CONTROL_MODE_PID:
            if(!_outputdrv_ok)
            {
                WARNING("No pid output driver available: pid remains in-active.");
                set_mode(CONTROL_MODE_SENSOR); // try input-only mode, otherwise disable
                return;
            };

            _last_pid = 0; // Reset dt timer

            // align FPID internals with current _output_value
            _pid.alignOutput();

            _status = STATUS_UNLOCKED;
            _mode = CONTROL_MODE_PID;
            break;
        case CONTROL_MODE_FIXED:
            if(!_outputdrv_ok)
            {
                WARNING("No pid output driver available: pid remains in-active.");
                set_mode(CONTROL_MODE_SENSOR); // try input-only mode, otherwise disable
                return;
            };

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
	if(_next_input > now)
        return;
    _next_input = now + settings.sensor_loop_ms; // FIXME: rather absolute dT

    // Read sensors and apply averaging/filter
    if(!_inputdrv_ok)
    {
        _input_value = NAN;
        return;
    };

    double read = _inputdrv->read();

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
        _status = STATUS_ERROR;
        _unlocked_last = now;
        return;
    };

    // on sensor/input error, we mark an error
    if(!isfinite(_input_value))
    {
        _status = STATUS_ERROR;
        _unlocked_last = now;
    };

    // If PV is outside lock window, we're unlocked
    // 1000 == 1%
    float window = static_cast<double>(_settings.lock_window)*_settings.fpid.setpoint/1E5;
    float error = abs(_input_value - _settings.fpid.setpoint);
    if(error > window)
    {
        // UNLOCKED
        _status = STATUS_UNLOCKED;
        _unlocked_last = now;
        return;
    };

    // In lock window, long enough?
    if(now - _unlocked_last < _settings.lock_time)
    {
        _status = STATUS_LOCKED_WAIT;
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
        case CONTROL_MODE_DISABLED:
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
    if(!_outputdrv_ok)
        return;
        
    // turn of output if PID loop had an error
    if(isnan(_output_value))
    {
        _outputdrv->off();
        return;
    };

    // Output is only enabled when PID is ruuning or when Fixed Output is set (in expert_mode)
    if(_mode != CONTROL_MODE_PID && _mode != CONTROL_MODE_FIXED)
    {
        _outputdrv->off();
        return;
    };

    _outputdrv->set(_output_value);
};
