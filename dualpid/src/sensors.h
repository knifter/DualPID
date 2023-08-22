#ifndef __SENSORS_H
#define __SENSORS_H

#include "config.h"

// Temperature
#if defined(PID1_SENSOR_M5KMETER) || defined(PID1_SENSOR_SHT3X_TEMP)
    #define SENSOR1_PRESENT
    #define PID1_NAME                   "Temperature"
    #define PID1_UNIT_TEXT			    "C"
    #define PID1_SETPOINT_MIN		    -20
    #define PID1_SETPOINT_MAX		    60
    #define PID1_SETPOINT_PRECISION     1
#endif

#if defined(PID2_SENSOR_SHT3X_RH)
    #define SENSOR2_PRESENT
    #define PID2_NAME                   "Humidity"
    #define PID2_UNIT_TEXT			    "%RH"
    #define PID2_SETPOINT_MIN	        0
    #define PID2_SETPOINT_MAX	        100
    #define PID2_SETPOINT_PRECISION	    0
#endif

#ifdef PID2_SENSOR_SPRINTIR
    #define SENSOR2_PRESENT
    #define PID2_NAME                   "CO2 Content"
    #define PID2_UNIT_TEXT			    "ppm"
    #define PID2_SETPOINT_MIN	        0
    #define PID2_SETPOINT_MAX	        100
    #define PID2_SETPOINT_PRECISION	    0
#endif

bool sensor1_begin();
double sensor1_read();

bool sensor2_begin();
double sensor2_read();


#ifndef SENSOR1_PRESENT
    #error "Sensor for channel 1 not defined"
#endif

#ifndef SENSOR2_PRESENT
    #error "Sensor for channel 1 not defined"
#endif

#endif //__SENSORS_H