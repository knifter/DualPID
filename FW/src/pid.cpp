
#include "pid.h"

#include <Arduino.h>
#include <MiniPID.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "tools-log.h"


// PIDLoop::PIDLoop()
// {
// };

bool PIDLoop::begin()
{
	digitalWrite(_pin_a, LOW);
	digitalWrite(_pin_b, LOW);

	_windowstarttime = millis();
	_input = 50;
	// _setpoint = DEFAULT_SETPOINT;
	_output = WINDOWSIZE / 2;
    _pid_next = 0;
    _output_state = 0;

	// Set the range between 0 and the full window size
	_pid.setOutputLimits(0, WINDOWSIZE);
	_pid.setOutput(_output);

	return true;
};

void PIDLoop::set_tuning(double p, double i, double d)
{
	DBG("Setting tunings: P = %02f, I = %02f, D = %02f", p, i, d);
	_pid.setParameters(p, i, d);
};

void PIDLoop::set_setpoint(double sp)
{
	_pid.setSetpoint(sp);
};

void PIDLoop::set_tuning(pidsettings_t& s)
{
	_pid.setParameters(
		s.Kp, 
		s.Ki,
		s.Kd);
	_pid.setSetpoint(s.setpoint);
	return;
};

void PIDLoop::loop()
{
    // See if its time to do another PID iteration
    time_t now = millis();
    if(now > _pid_next)
    {
        // Setpoint for the PID
        // Setpoint = RH_setpoint;

        // Input for the PID
        _input = _cb_value();
        _output = _pid.getOutput(_input);

        DBG("Output: %.0f", _output);

        // Queue next iteration
        _pid_next += PID_LOOPTIME_MS;
    };

    // Process timewindow valve depending on PID Output
    if (now - _windowstarttime > WINDOWSIZE)
    { //time to shift the Relay Window
        _windowstarttime += WINDOWSIZE;
    };

    // Set output
    if (_output > (now - _windowstarttime))
    {
        // Increase
        _output_state = 1;
        digitalWrite(_pin_a, LOW);
        digitalWrite(_pin_b, HIGH);
    } else {
        // Decrease
        _output_state = 0;
        digitalWrite(_pin_a, LOW);
        digitalWrite(_pin_b, LOW);
    };
};
