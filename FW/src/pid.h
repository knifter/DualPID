#ifndef __PID_H
#define __PID_H

#include "settings.h"

bool pid_begin();
void pid_set_tuning(double, double, double);
void pid_set_tuning(settings_t&);
void pid_set_setpoint(double);
void pid_loop();
double pid_get_output();

#endif // __PID_H