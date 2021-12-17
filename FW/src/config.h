#ifndef __CONFIG_H
#define __CONFIG_H

#define DEBUG                   1
#define TOOLS_LOG_DEBUG
#define GUI_KEYPAD

#define DEBUG_INTERVAL_MS		100
#define BOOTSCREEN_TIMEOUT_MS	1000

#define VERSION				    20211111

#define NVS_NAMESPACE			"PidSettings"
#define NVS_SETTINGS_IDENT		"EPIDSET"
#define SETTINGS_DELAY_SAVE		60E3
#define DEFAULT_SETPOINT		50.0
#define DEFAULT_PID_P			5.0
#define DEFAULT_PID_I			2.0
#define DEFAULT_PID_D			2.0
#define MEASURE_INTERVAL_MS		1000
#define HISTORY_GRAPH_POINTS	120
#define HISTORY_GRAPH_DELTA_MS	60E3

#define PID1_UNIT_TEXT			"C"
#define PID2_UNIT_TEXT			"%RH"

// #define PIN_VALVE           11
// #define ANALOG_SENSOR_RH    35
//#define ANALOG_HUM_ADJUST   2
#define PID_LOOPTIME_MS     	1000            // PID delta-T: 2 seconds as it was
// #define DISPLAY_LOOPTIME_MS 	500             // Display update rate: every half a second
#define PID_WINDOWSIZE        	5000      		// time-windows size in ms

// #define DISPLAY_WIDTH			320
// #define DISPLAY_HEIGHT			240
// #define LV_BUF_SIZE				(DISPLAY_WIDTH*DISPLAY_WIDTH/10)

#define SHT_ADDRESS             0x45

#define PIN_SDA					GPIO_NUM_21
#define PIN_SCL					GPIO_NUM_22
#define PIN_HB1_A				GPIO_NUM_2
#define PIN_HB1_B				GPIO_NUM_12
#define PIN_HB2_A				GPIO_NUM_13
#define PIN_HB2_B				GPIO_NUM_15
#define PIN_SPEAKER				GPIO_NUM_25
#define PIN_BACKLIGHT			GPIO_NUM_32

#define PIN_BTN_A				GPIO_NUM_39
#define PIN_BTN_B				GPIO_NUM_38
#define PIN_BTN_C				GPIO_NUM_37

#endif // __CONFIG_H
