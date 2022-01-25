
#include "pid.h"

#include <Arduino.h>
#include <FPID.h>

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
	_output = PID_WINDOWSIZE / 2;
    _pid_last = millis();
    _output_state = 0;

	// Set the range between 0 and the full window size
	_pid.setOutputLimits(0, PID_WINDOWSIZE);
    _pid.alignOutput();

    DBG("begin output: %f", _output);
	return true;
};

void PIDLoop::loop()
{
    // See if its time to do another PID iteration
    time_t now = millis();
    if(now > _pid_last + PID_LOOPTIME_MS)
    {
        // Input for the PID
        _input = _cb_value();
        double dt = (now - _pid_last) / PID_LOOPTIME_MS;
        _pid.calculate(NAN);

        DBG("PID: Input = %.2f, Setpoint = %.2f, Output = %.2f (dt = %.9f)", _input, _pidsettings->setpoint, _output, dt);

        // Queue next iteration
        _pid_last = now;
    };

    // Process timewindow valve depending on PID Output
    if (now - _windowstarttime > PID_WINDOWSIZE)
    { //time to shift the Relay Window
        _windowstarttime += PID_WINDOWSIZE;
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
