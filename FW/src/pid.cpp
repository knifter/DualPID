
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
	digitalWrite(PIN_HB1_A, LOW);
	digitalWrite(PIN_HB1_B, LOW);

	_windowstarttime = millis();
	_input = 50;
	// _setpoint = DEFAULT_SETPOINT;
	_output = WINDOWSIZE / 2;

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

double PIDLoop::get_output()
{
	return _output;
};

int PIDLoop::get_output_digital()
{
    return _output_digital;
};

void PIDLoop::loop()
{
	// See if its time to do another PID iteration
	time_t now = millis();
	static time_t pid_next = 0;
	if(now > pid_next)
  	{
    	// Setpoint for the PID
    	// Setpoint = RH_setpoint;

    	// Input for the PID
    	_input = sht_sensor.getHumidity();
		_output = _pid.getOutput(_input);

		DBG("Output: %.0f", _output);

		// Queue next iteration
		pid_next += PID_LOOPTIME_MS;
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
		digitalWrite(_pin_a, LOW);
		digitalWrite(_pin_b, HIGH);
  	} else {
    	// Decrease
		digitalWrite(_pin_a, HIGH);
		digitalWrite(_pin_b, LOW);
  	};
};
