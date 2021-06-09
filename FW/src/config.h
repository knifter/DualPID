#ifndef __CONFIG_H
#define __CONFIG_H

#define DEBUG                   1
#define TOOLS_LOG_DEBUG

#define VERSION					20210609

#define NVS_NAMESPACE			"PidSettings"
#define NVS_SETTINGS_IDENT		"EPIDSET"
#define SETTINGS_DELAY_SAVE		5000
#define DEFAULT_SETPOINT		0.0
#define DEFAULT_PID_P			1.0
#define DEFAULT_PID_I			0.0
#define DEFAULT_PID_D			0.0
#define MEASURE_INTERVAL_MS		1000

// #define PIN_VALVE           11
// #define ANALOG_SENSOR_RH    35
//#define ANALOG_HUM_ADJUST   2
#define PID_LOOPTIME_MS     	1000            // PID delta-T: 2 seconds as it was
#define DISPLAY_LOOPTIME_MS 	500             // Display update rate: every half a second
#define WINDOWSIZE          	5000      		// time-windows size in ms

#define SCREEN_WIDTH			320
#define SCREEN_HEIGHT			240

#define PIN_SDA					GPIO_NUM_21
#define PIN_SCL					GPIO_NUM_22
#define PIN_HB1_A				GPIO_NUM_2
#define PIN_HB1_B				GPIO_NUM_5
#define PIN_HB2_A				GPIO_NUM_13
#define PIN_HB2_B				GPIO_NUM_15
#define PIN_SPEAKER				GPIO_NUM_25

#endif // __CONFIG_H
