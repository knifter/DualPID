#ifndef __CONFIG_H
#define __CONFIG_H

#define DEBUG
#define TOOLS_LOG_DEBUG

#define VERSION					20210421

#define NVS_NAMESPACE			"PidSettings"
#define NVS_SETTINGS_IDENT		"EPIDSET"
#define SETTINGS_DELAY_SAVE		5000
#define DEFAULT_SETPOINT		50.0
#define DEFAULT_PID_P			2.0
#define DEFAULT_PID_I			5.0
#define DEFAULT_PID_D			1.0

// #define PIN_VALVE           11
// #define ANALOG_SENSOR_RH    35
//#define ANALOG_HUM_ADJUST   2
#define PID_LOOPTIME_MS     	1000            // PID delta-T: 2 seconds as it was
#define DISPLAY_LOOPTIME_MS 	500             // Display update rate: every half a second
#define WINDOWSIZE          	5000      		// time-windows size in ms

#define PIN_SDA					GPIO_NUM_21
#define PIN_SCL					GPIO_NUM_22
#define PIN_VALVE				GPIO_NUM_26
#define PIN_SPEAKER				GPIO_NUM_25

#endif // __CONFIG_H