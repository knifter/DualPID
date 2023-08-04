#ifndef __CONFIG_H
#define __CONFIG_H

#define _STRINGIFY(x)      #x
#define STRINGIFY(x)      _STRINGIFY(x)

#define DEBUG                   1
#ifdef DEBUG
	// Enable DBG(...) globally:
	#define TOOLS_LOG_DEBUG

	// Or per file:

	// Specific parts:
	#define DEBUG_INTERVAL_MS		250
#endif

#define GUI_KEYPAD

#define VERSION                BUILD_DATETIME
#define VERSIONSTR_LONG        STRINGIFY(GIT_BRANCH-VERSION)
#define VERSIONSTR_SHORT       STRINGIFY(VERSION)

// #define DEBUG_INTERVAL_MS		100
#define BOOTSCREEN_TIMEOUT_MS	1000

#define SETTINGS_DELAY_SAVE		20E3
#define DEFAULT_SENSOR_LOOP_MS  1000
#define DEFAULT_LOOPTIME_MS     1000            // time between each pid-loop iteration
#define DEFAULT_WINDOWTIME      5000      		// soft-pwm time-window size in ms
#define DEFAULT_GRAPH_LEN       0               // Length of graph on display
#define DEFAULT_MAX_OUTPUT      1.0             // Factor max output, between 0..1
#define DEFAULT_PID_SETPOINT1	20.0
#define DEFAULT_PID_SETPOINT2	50.0
#define DEFAULT_PID_F			0.0
#define DEFAULT_PID_P			5.0
#define DEFAULT_PID_I			2.0
#define DEFAULT_PID_D			2.0
#define DEFAULT_PID_OFILTER     0.0             // PID Output filter, 0 = no filter
#define DEFAULT_PID_DFILTER     0.0             // PID D-Term filter
#define DEFAULT_PID_TBH         0

#define MAIN_LOOP_MS			100

#define GRAPH_DELTA_MS	        60E3		    // Time between graph points
#define GRAPH_POINTS	        240             // Amount of points in graph
#define GRAPH_SCALE_ROUND		5			    // scale min/max is rounded to a multiple of this
#define GRAPH_MULTIPLIER		10			    // Graph scale precision for calculation(10 = 1 decimal)
#define GRAPH_XDIVS             9
#define GRAPH_YDIVS             5
#define GRAPH_XTICKS            5

#define PID1_UNIT_TEXT			"C"
#define PID2_UNIT_TEXT			"ppm"

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

#define PIN_CO2_RX				GPIO_NUM_16
#define PIN_CO2_TX				GPIO_NUM_17

#define PIDLOOP_PORTS_LIST	\
	{GPIO_NUM_2, 	"IO2", 	"GPIO2"},	\
	{GPIO_NUM_12, 	"IO12", "GPIO12"},	\
	{GPIO_NUM_13, 	"IO13", "GPIO13"},	\
	{GPIO_NUM_15, 	"IO15", "GPIO15"},	\
	{GPIO_NUM_26, 	"IO26", "GPIO26"}

#endif // __CONFIG_H
