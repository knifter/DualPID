#include "inputdrv.h"
#include "BME280.h"

#include <SHT3x.h>

// #include "tools-log.h"

// Base
const char* InputDriver::name()
{
	switch(data_type())
	{
		case TYPE_TEMPERATURE: 	return "Temperature";
		case TYPE_RH: 			return "Humidity";
		case TYPE_CO2: 			return "CO2 Content";
		case TYPE_PRESSURE: 	return "Pressure";
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
		case TYPE_PRESSURE: 	return "hPa";
		default: 				return "<none>";
	};
};
float InputDriver::setpoint_min()
{
	switch(data_type())
	{
		case TYPE_TEMPERATURE: 	return -20;
		case TYPE_PRESSURE: 	return 300;		// BME280 range 300..1100 hPa
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
		case TYPE_PRESSURE: 	return 1100;
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
		case TYPE_PRESSURE: 	return 1;
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
		case TYPE_PRESSURE: 	return COLOR_PURPLE;
		default: 				return COLOR_BLACK;
	};			
};

// SHT31
SHT3X sensor_sht31(Wire);
bool SHT31TDriver::begin()
{
	if(sensor_sht31.begin(SHT3X_ADDRESS_DEFAULT))
	{
		return InputDriver::begin();
	};
	if(sensor_sht31.begin(SHT3X_ADDRESS_ALT))
	{
		return InputDriver::begin();
	};
	return false;
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
	if(sensor_sht31.begin(SHT3X_ADDRESS_DEFAULT))
	{
		return InputDriver::begin();
	};
	if(sensor_sht31.begin(SHT3X_ADDRESS_ALT))
	{
		return InputDriver::begin();
	};
	return false;
};
float SHT31RHDriver::read()
{
	SHT3X::measurement_t* m = sensor_sht31.newMeasurement();

	if(m->error)
		return NAN;
	return m->humidity;
};

#ifdef INPUTDRV_BME280_ENABLED
// BME280
BME280 sensor_bme280(Wire);
bool BME280Driver::begin()
{
	if(!sensor_bme280.begin(BME280_ADDRESS_DEFAULT) && !sensor_bme280.begin(BME280_ADDRESS_ALT))
		return false;

	sensor_bme280.setMode(BME280::Mode_t::NORMAL);
	sensor_bme280.setOversampling(
		BME280::Oversampling_t::X2,
		BME280::Oversampling_t::X1,
		BME280::Oversampling_t::X4);
	sensor_bme280.setStandby(BME280::Standby_t::MS_125);
	sensor_bme280.setFilter(BME280::Filter_t::X2);
	return InputDriver::begin();
};
BME280::measurement_t* BME280Driver::measure()
{
	// NORMAL mode: the sensor measures continuously, this just reads the latest values
	return sensor_bme280.newMeasurement();
};
float BME280TDriver::read()
{
	BME280::measurement_t* m = measure();
	if(m->error)
		return NAN;
	return m->temperature;
};
float BME280RHDriver::read()
{
	BME280::measurement_t* m = measure();
	if(m->error)
		return NAN;
	return m->humidity;
};
float BME280PDriver::read()
{
	BME280::measurement_t* m = measure();
	if(m->error)
		return NAN;
	return m->pressure / 100.0f;		// Pa -> hPa
};
#endif // INPUTDRV_BME280_ENABLED

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

// MLX90614
bool MLX90614Driver::begin()
{
	if(!_sensor.begin())
		return false;
	return InputDriver::begin();
};
float MLX90614Driver::read()
{
	return _sensor.readObjectTemp();
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
