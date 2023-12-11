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

extern const char*     Temperature_Name;
extern const char*     Temperature_Unit_Text;
extern const double    Temperature_Setpoint_Min;
extern const double    Temperature_Setpoint_Max;
extern const int       Temperature_Precision;
extern const lv_color_t Temperature_Color;
extern const char*     Humidity_Name;
extern const char*     Humidity_Unit_Text;
extern const double    Humidity_Setpoint_Min;
extern const double    Humidity_Setpoint_Max;
extern const int       Humidity_Precision;
extern const lv_color_t Humidity_Color;
extern const char*     CO2_Name;
extern const char*     CO2_Unit_Text;
extern const double    CO2_Setpoint_Min;
extern const double    CO2_Setpoint_Max;
extern const int       CO2_Precision;
extern const lv_color_t CO2_Color;

sensor_begin_fptr find_sensor_begin(uint32_t sensor_type);
sensor_read_fptr find_sensor_read(uint32_t sensor_type);
lv_color_t find_sensor_color(uint32_t sensor_type);

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

#endif //__SENSORS_H