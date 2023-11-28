#ifndef __SENSORS_H
#define __SENSORS_H

#include "config.h"
#include <soogh-color.h>

typedef bool (*sensor_begin_fptr)();
typedef double (*sensor_read_fptr)();

typedef enum
{
    SENSOR_NONE = 0,

    // 100 = TEMP
    SENSOR_SHT31_TEMP = 100,
    SENSOR_M5KMETER,
    SENSOR_MCP9600,
    SENSOR_MAX31865,

    // 200 = HR
    SENSOR_SHT31_RH = 200,

    // 300 = CO2
    SENSOR_SPRINTIR = 300,
} sensor_type_t;

#define SENSOR_TYPES_LIST \
    {SENSOR_NONE, "none", "No sensor"},                 \
    {SENSOR_SHT31_TEMP, "sht-t", "SHT31 Temperature"},  \
    {SENSOR_M5KMETER, "kmeter", "M5-KMeter"},           \
    {SENSOR_MCP9600, "mcp9600", "MCP9600"},             \
    {SENSOR_MAX31865, "max31865", "MAX31865"},          \
    {SENSOR_SHT31_RH, "sht-rh", "SHT31 Humidity"},      \
    {SENSOR_SPRINTIR, "sprintir", "SprintIR"}          

sensor_begin_fptr find_sensor_begin(uint32_t sensor_type);
sensor_read_fptr find_sensor_read(uint32_t sensor_type);

#ifdef SENSOR_SHT31_ENABLED
    bool sensor_sht31_begin();
    double sensor_sht31temp_read();
    double sensor_sht31rh_read();
#endif

#ifdef SENSOR_M5KMETER_ENABLED
    bool sensor_m5kmeter_begin();
    double sensor_m5kmeter_read();
#endif

#ifdef SENSOR_MCP9600_ENABLED
    bool sensor_mcp9600_begin();
    double sensor_mcp9600_read();
#endif

#ifdef SENSOR_MAX31865_ENABLED
    bool sensor_max31865_begin();
    double sensor_max31865_read();
#endif

#ifdef SENSOR_SPRINTIR_ENABLED
    bool sensor_sprintir_begin();
    double sensor_sprintir_read();
#endif

#define PID1_NAME                   "Temperature"
#define PID1_COLOR                  COLOR_RED
#define PID1_UNIT_TEXT			    "\xc2\xb0""C"
#define PID1_SETPOINT_MIN		    -20
#define PID1_SETPOINT_MAX		    60
#define PID1_PRECISION              2

#define PID2_NAME                   "Humidity"
#define PID2_COLOR                 COLOR_BLUE
#define PID2_UNIT_TEXT			    "%RH"
#define PID2_SETPOINT_MIN	        0
#define PID2_SETPOINT_MAX	        100
#define PID2_PRECISION	            0

// #ifdef PID2_SENSOR_SPRINTIR
//     #define SENSOR2_PRESENT
//     #define PID2_NAME                   "CO2 Content"
//     #define PID2_COLOR                 COLOR_YELLOW
//     #define PID2_UNIT_TEXT			    "ppm"
//     #define PID2_SETPOINT_MIN	        400
//     #define PID2_SETPOINT_MAX	        20000
//     #define PID2_PRECISION	            0
// #endif

#endif //__SENSORS_H