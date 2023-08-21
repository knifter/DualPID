#include "sensors.h"
#include "config.h"

/***** CHANNEL 1 ********************************************/
#if defined(PID1_SENSOR_SHT3X_TEMP) || defined(PID2_SENSOR_SHT3X_RH)
    #include <SHT3x.h>
    SHT3X sensor_sht(Wire);\
    SHT3X::measurement_t* sensor_sht_m;
    bool sht_begin()
    {
        if(!sensor_sht.begin(SHT3X_ADDRESS_DEFAULT))
            if(!sensor_sht.begin(SHT3X_ADDRESS_ALT))
                return false;
        return true;
    };
    void sht_measure()
    {
        sensor_sht_m = sensor_sht.newMeasurement();
    };
#endif

#if defined(PID1_SENSOR_SHT3X_TEMP)
    bool sensor1_begin()
    {
        return sht_begin();    
    };

    double sensor1_read()
    {
        sht_measure();

        if(sensor_sht_m->error)
            return NAN;
        return sensor_sht_m->temperature;    
    };
#endif

#if defined(PID1_SENSOR_MAX31855)
double sensors1_read()
{
};
#endif

/***** CHANNEL 2 ********************************************/
#if defined(PID2_SENSOR_SHT3X_RH)
    bool sensor2_begin()
    {
        #ifndef PID1_SENSOR_SHT3X_TEMP
        return sht_begin();
        #endif
        return true;
    };
    double sensor2_read()
    {
        #ifndef PID1_SENSOR_SHT3X_TEMP
        return sht_measure();
        #endif

        if(sensor_sht_m->error)
            return NAN;
        return sensor_sht_m->humidity;    
    };
#endif

#ifdef PID2_SENSOR_SPRINTIR
#include <SprintIR.h>
SprintIR sensor_sprint(Serial2);

bool sensor2_begin()
{
	Serial2.begin(9600, SERIAL_8N1, PIN_SPRINT_RX, PIN_SPRINT_TX);
    return sprint.begin();
};

double sensors_pid2_value()
{
	// CO2 from SprintIR-WX-20
	int ppm = sprint.getPPM();
	if(ppm < 0)
	{
        return NAN;
	};
    return ppm;
};
#endif
