#include "inputdrv.h"

#include <SHT3x.h>

// Base
const char* InputDriver::name()
{
	switch(data_type())
	{
		case TYPE_TEMPERATURE: 	return "Temperature";
		case TYPE_RH: 			return "Humidity";
		case TYPE_CO2: 			return "CO2 Content";
		default: 				return "<none>";
	};
};
const char* InputDriver::unit_text()
{
	switch(data_type())
	{
		case TYPE_TEMPERATURE: 	return "\xc2\xb0""C";
		case TYPE_RH: 			return "%RH";
		case TYPE_CO2: 			return "%";
		default: 				return "<none>";
	};
};
float InputDriver::setpoint_min()
{
	switch(data_type())
	{
		case TYPE_TEMPERATURE: 	return -20;
		// case TYPE_RH: 			return 0;
		// case TYPE_CO2: 			return 0;
		default: 				return 0;
	};
	
};
float InputDriver::setpoint_max()
{
	switch(data_type())
	{
		case TYPE_TEMPERATURE: 	return 60;
		// case TYPE_RH: 			return 100;
		// case TYPE_CO2: 			return 100;
		default: 				return 100;
	};
};
int InputDriver::precision()
{
	switch(data_type())
	{
		// case TYPE_TEMPERATURE: 	return 2;
		case TYPE_RH: 			return 0;
		case TYPE_CO2: 			return 3;
		default: 				return 2;
	};			
};
lv_color_t InputDriver::color1()
{
	switch(data_type())
	{
		case TYPE_TEMPERATURE: 	return COLOR_RED;
		case TYPE_RH: 			return COLOR_BLUE;
		case TYPE_CO2: 			return COLOR_YELLOW;
		default: 				return COLOR_BLACK;
	};			
};

// SHT31
SHT3X sensor_sht31(Wire);
bool SHT31TDriver::begin()
{
	if(!sensor_sht31.begin(SHT3X_ADDRESS_DEFAULT))
		if(!sensor_sht31.begin(SHT3X_ADDRESS_ALT))
			return false;
	return InputDriver::begin();
};
float SHT31TDriver::read() 
{
	SHT3X::measurement_t* m = sensor_sht31.newMeasurement();

	if(m->error)
		return NAN;
	return m->temperature;
};
bool SHT31RHDriver::begin()
{
	if(!sensor_sht31.begin(SHT3X_ADDRESS_DEFAULT))
		if(!sensor_sht31.begin(SHT3X_ADDRESS_ALT))
			return false;
	return InputDriver::begin();
};
float SHT31RHDriver::read()
{
	SHT3X::measurement_t* m = sensor_sht31.newMeasurement();

	if(m->error)
		return NAN;
	return m->humidity;
};

// M5_KMeter
bool M5KMeterDriver::begin()
{
	_sensor.begin(&Wire);
	if(!_sensor.setSleepTime(1))
		return false;
	return InputDriver::begin();
};
float M5KMeterDriver::read()
{
	if(!_sensor.update())
		return NAN;
	return _sensor.getTemperature();
};

// MCP9600
bool MCP9600Driver::begin()
{
	if(!_sensor.begin())
		return false;
	return InputDriver::begin();
};

float MCP9600Driver::read()
{
	return _sensor.readThermocouple();
};

// MAX31865
bool MAX31865Driver::begin()
{
	if(!_sensor.begin(100, 430, true, true))
		return false;
	_sensor.setAutoConvert(true);
	return InputDriver::begin();
};
float MAX31865Driver::read()
{
	return _sensor.getTemperature();
};

// SprintIR20
bool SprintIR20Driver::begin()
{
	Serial2.begin(9600, SERIAL_8N1, PIN_RX, PIN_TX);
	if(!_sensor.begin())
		return false;
	return InputDriver::begin();
};
float SprintIR20Driver::read()
{
	// CO2 from SprintIR-WX-20
	return _sensor.getPercent();
	// return sensor_sprintir.getCompensatedPercent(SPRINTIR_PRESSURE_MBAR);
};
