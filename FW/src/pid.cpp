
#include "pid.h"

#include <Arduino.h>
#include <MiniPID.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "tools-log.h"

// Define Variables for the PID controler
double Input, Output, Setpoint;
unsigned long windowStartTime;

// PID myPID(&Input, &Output, &Setpoint, 
// 	DEFAULT_PID_P, DEFAULT_PID_I, DEFAULT_PID_D, DIRECT);
MiniPID pid(DEFAULT_PID_P, DEFAULT_PID_I, DEFAULT_PID_D);

bool pid_begin()
{
	digitalWrite(PIN_HB1_A, LOW);
	digitalWrite(PIN_HB1_B, LOW);

	windowStartTime = millis();
	Input = 50;
	Setpoint = DEFAULT_SETPOINT;
	Output = WINDOWSIZE / 2;

	// Set the range between 0 and the full window size
	pid.setOutputLimits(0, WINDOWSIZE);
	pid.setOutput(Output);

	return true;
};

void pid_set_tuning(double p, double i, double d)
{
	DBG("Setting tunings: P = %02f, I = %02f, D = %02f", p, i, d);
	pid.setParameters(p, i, d);
};

void pid_set_setpoint(double sp)
{
	pid.setSetpoint(sp);
};

void pid_set_tuning(settings_t& s)
{
	pid.setParameters(
		s.Kp, 
		s.Ki,
		s.Kd);
	pid.setSetpoint(s.setpoint);
	return;
};

double pid_get_output()
{
	return Output;
};

void pid_loop()
{
	// See if its time to do another PID iteration
	time_t now = millis();
	static time_t pid_next = 0;
	if(now > pid_next)
  	{
    	// Setpoint for the PID
    	// Setpoint = RH_setpoint;

    	// Input for the PID
    	Input = sht_sensor.getHumidity();
		Output = pid.getOutput(Input);

		DBG("Output: %.0f", Output);

		// Queue next iteration
		pid_next += PID_LOOPTIME_MS;
	};

	// Process timewindow valve depending on PID Output
	if (now - windowStartTime > WINDOWSIZE)
  	{ //time to shift the Relay Window
	    windowStartTime += WINDOWSIZE;
  	};
  	if (Output > (now - windowStartTime))
  	{
	    // Increase
		digitalWrite(PIN_HB1_A, LOW);
		digitalWrite(PIN_HB1_B, HIGH);
  	} else {
    	// Decrease
		digitalWrite(PIN_HB1_A, HIGH);
		digitalWrite(PIN_HB1_B, LOW);
  	};
};
