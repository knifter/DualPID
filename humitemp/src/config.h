#ifndef __CONFIG_H
#define __CONFIG_H

#define DEBUG                   1
#define TOOLS_LOG_DEBUG
#define GUI_KEYPAD

#define DEBUG_INTERVAL_MS		100
#define BOOTSCREEN_TIMEOUT_MS	1000

#define SETTINGS_DELAY_SAVE		20E3
// #define DEFAULT_SENSOR_LOOP_MS  1000
#define DEFAULT_SETPOINT		50.0
#define DEFAULT_PID_F			0.0
#define DEFAULT_PID_P			5.0
#define DEFAULT_PID_I			2.0
#define DEFAULT_PID_D			2.0

#define MAIN_LOOP_MS			500
#define PIDLOOP_LOOP_MS     	1000            // PID delta-T: 2 seconds as it was
#define PIDLOOP_WINDOWSIZE      5000      		// time-windows size in ms

#define GRAPH_DELTA_MS	        60E3		    // Time between graph points
#define GRAPH_POINTS	        240             // Amount of points in graph
#define GRAPH_SCALE_ROUND		5			    // scale min/max is rounded to a multiple of this
#define GRAPH_MULTIPLIER		10			    // Graph scale precision for calculation(10 = 1 decimal)
#define GRAPH_XDIVS             9
#define GRAPH_YDIVS             5
#define GRAPH_XTICKS            5

#define PID1_UNIT_TEXT			"C"
#define PID2_UNIT_TEXT			"%RH"

#define TEMPERATURE_MIN			0
#define TEMPERATURE_MAX			60
#define TEMPERATURE_PRECISION	1
#define HUMIDITY_MIN			0
#define HUMIDITY_MAX			100
#define HUMIDITY_PRECISION		0
#define PID_PAR_MIN				0
#define PID_PAR_MAX				100
#define PID_PAR_PRECISION		1

// #define DISPLAY_WIDTH			320
// #define DISPLAY_HEIGHT			240
// #define LV_BUF_SIZE				(DISPLAY_WIDTH*DISPLAY_WIDTH/10)

#define PIN_SDA					GPIO_NUM_21
#define PIN_SCL					GPIO_NUM_22

#define PIN_SPEAKER				GPIO_NUM_25
#define PIN_BACKLIGHT			GPIO_NUM_32

#define PIN_BTN_A				GPIO_NUM_39
#define PIN_BTN_B				GPIO_NUM_38
#define PIN_BTN_C				GPIO_NUM_37

#define PIDLOOP_PORTS_LIST	\
	{GPIO_NUM_2, 	"IO2", 	"GPIO2"},	\
	{GPIO_NUM_12, 	"IO12", "GPIO12"},	\
	{GPIO_NUM_13, 	"IO13", "GPIO13"},	\
	{GPIO_NUM_15, 	"IO15", "GPIO15"},	\
	{GPIO_NUM_26, 	"IO26", "GPIO26"}

#endif // __CONFIG_H
