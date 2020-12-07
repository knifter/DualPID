#ifndef __PID_H
#define __PID_H

void pid_begin();
void pid_set_tuning(double, double, double);
void pid_loop();

#endif // __PID_H