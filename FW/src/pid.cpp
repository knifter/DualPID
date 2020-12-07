
#include "pid.h"

#include <Arduino.h>
#include <PID_v1.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "tools-log.h"

// Define Variables for the PID controler
double Input, Output, Setpoint;
unsigned long windowStartTime;

PID myPID(&Input, &Output, &Setpoint, 
	DEFAULT_PID_P, DEFAULT_PID_I, DEFAULT_PID_D, DIRECT);

bool pid_begin()
{
	digitalWrite(PIN_VALVE, HIGH);

	windowStartTime = millis();
	Output = WINDOWSIZE / 2;

	// Set the range between 0 and the full window size
	myPID.SetOutputLimits(0, WINDOWSIZE);

	// Turn the PID on
	myPID.SetMode(AUTOMATIC);

	return true;
};

void pid_set_tuning(double p, double i, double d)
{
	DBG("Setting tunings: P = %02f, I = %02f, D = %02f", p, i, d);
	myPID.SetTunings(p, i, d);
};

void pid_set_setpoint(double sp)
{
	Setpoint = sp;
};

void pid_set_tuning(settings_t& s)
{
	pid_set_tuning(
		s.Kp, 
		s.Ki,
		s.Kd);
	pid_set_setpoint(s.setpoint);
	return;
};

void pid_loop()
{
  	float RH_actual = sht_sensor.getHumidity();

	// See if its time yet
	time_t now = millis();
	static time_t pid_next = 0;
	if(now > pid_next)
  	{
    	// Setpoint for the PID
    	// Setpoint = RH_setpoint;

    	// Input for the PID
    	Input = RH_actual;
    	myPID.Compute();

		// Queue next iteration
		pid_next += PID_LOOPTIME_MS;
	};

	if (now - windowStartTime > WINDOWSIZE)
  	{ //time to shift the Relay Window
	    windowStartTime += WINDOWSIZE;
  	};
  	if (Output > (now - windowStartTime))
  	{
	    // Increase
    	digitalWrite(PIN_VALVE, HIGH);
  	} else {
    	// Decrease
    	digitalWrite(PIN_VALVE, LOW);
  	};
};
