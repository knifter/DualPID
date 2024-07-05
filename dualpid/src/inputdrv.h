#ifndef __INPUTDRV_H
#define __INPUTDRV_H

#include "config.h"
#include <soogh-color.h>

typedef enum
{
    INPUT_DRIVER_NONE = 0,

    // 100 = TEMP
    INPUT_DRIVER_SHT31_TEMP = 100,
    INPUT_DRIVER_M5KMETER,
    INPUT_DRIVER_MCP9600,
    INPUT_DRIVER_MAX31865,

    // 200 = HR
    INPUT_DRIVER_SHT31_RH = 200,

    // 300 = CO2
    INPUT_DRIVER_SPRINTIR = 300,
} input_driver_t;

#define INPUT_DRIVER_MENULIST \
    {INPUT_DRIVER_NONE, "none", "No sensor"},                 \
    {INPUT_DRIVER_SHT31_TEMP, "sht-t", "SHT31 Temperature"},  \
    {INPUT_DRIVER_SHT31_RH, "sht-rh", "SHT31 Humidity"},      \
    {INPUT_DRIVER_M5KMETER, "kmeter", "M5-KMeter"},           \
    {INPUT_DRIVER_MCP9600, "mcp9600", "MCP9600"},             \
    {INPUT_DRIVER_MAX31865, "max31865", "MAX31865"},          \
    {INPUT_DRIVER_SPRINTIR, "sprintir", "SprintIR"}          

class InputDriver
{
	public:
		typedef enum {
			TYPE_NONE,
			TYPE_TEMPERATURE,
			TYPE_RH,
			TYPE_CO2,
		} data_type_t;

		InputDriver() {};

		virtual bool begin() { return true; };
		virtual float read() = 0;
		virtual data_type_t data_type() { return TYPE_NONE; };

		// overridable settings
		virtual const char* name();
		// static constexpr const char* name = "<none>";
		virtual const char* unit_text();
		virtual float setpoint_min();
		virtual float setpoint_max();
		virtual int precision();
		virtual lv_color_t color1();
};

// Dummy driver to save testing for nullptr everywhere
class NoneInputDriver : public InputDriver
{
	public:
		NoneInputDriver() : InputDriver() {};

		bool begin() { return false; };
		float read() { return NAN; };
		data_type_t data_type() { return TYPE_NONE; };
};

#ifdef SENSOR_SHT31_ENABLED
#include <SHT3x.h>
class SHT31TDriver : public InputDriver
{
	public:
		SHT31TDriver() : InputDriver() {};

		bool begin();
		float read();
		data_type_t data_type() { return TYPE_TEMPERATURE; };
};
class SHT31RHDriver : public InputDriver
{
	public:
		SHT31RHDriver() : InputDriver() {};

		bool begin();
		float read();
		data_type_t data_type() { return TYPE_RH; };
};
#endif // SENSOR_SHT31_ENABLED

#ifdef SENSOR_M5KMETER_ENABLED
#include <M5_KMeter.h>
class M5KMeterDriver : public InputDriver
{
	public:
		M5KMeterDriver() : InputDriver() {};

		bool begin();
		float read();
		data_type_t data_type() { return TYPE_TEMPERATURE; };
	
	private:
		M5_KMeter _sensor;
};
#endif // SENSOR_M5KMETER_ENABLED

#ifdef SENSOR_MCP9600_ENABLED
#include <MCP9600.h>
class MCP9600Driver : public InputDriver
{
	public:
		MCP9600Driver() : InputDriver(), _sensor(Wire) {};

		bool begin();
		float read();		
		data_type_t data_type() { return TYPE_TEMPERATURE; };

	private:
		MCP9600 _sensor;
};
#endif // SENSOR_MCP9600_ENABLED

#ifdef SENSOR_MAX31865_ENABLED
#include <MAX31865.h>
class MAX31865Driver : public InputDriver
{
	public:
		MAX31865Driver() : InputDriver(), _sensor(SPI, PIN_MAX31865_CS) {};

		bool begin();
		float read();
		data_type_t data_type() { return TYPE_TEMPERATURE; };

	private:
		MAX31865 _sensor;
};
#endif // SENSOR_MAX31865_ENABLED

#ifdef SENSOR_SPRINTIR_ENABLED
#include <SprintIR.h>
class SprintIR20Driver : public InputDriver
{
	public:
		SprintIR20Driver() : InputDriver(), _sensor(Serial) {};

		bool begin();
		float read();
		data_type_t data_type() { 	return TYPE_CO2; };

	private:
		SprintIR _sensor;
};
#endif // SENSOR_SPRINTIR_ENABLED

#endif // __INPUTDRV_H