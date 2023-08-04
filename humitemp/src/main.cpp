#include <Arduino.h>
#include <SHT3X.h>
#include <SprintIR.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "pidloop.h"
#include "screens.h"

#include "tools-log.h"

SHT3X sht_sensor(Wire);
SprintIR sprint(Serial2);

void halt(const char*);
uint32_t scan_keys();
void sensor_loop();

void setup()
{
	Serial.begin(115200);

    // Init IO
	digitalWrite(PIN_SPEAKER, LOW);
	pinMode(PIN_SPEAKER, OUTPUT);

	pinMode(PIN_BTN_A, INPUT);
	pinMode(PIN_BTN_B, INPUT);
	pinMode(PIN_BTN_C, INPUT);

	Serial2.begin(9600, SERIAL_8N1, PIN_CO2_RX, PIN_CO2_TX);
	
	
	// Hold startup, and registere debugging purposes
    developer_mode = 0;
	while(scan_keys() == KEY_AC)
        developer_mode = 1;

	gui.begin();

	Wire.begin(PIN_SDA, PIN_SCL);
	if(!sht_sensor.begin(SHT3X_ADDRESS_DEFAULT))
	{
        if(!sht_sensor.begin(SHT3X_ADDRESS_ALT))
        {
			gui.showMessage("WARNING:", "SHT3X(0x44/0x45): not found");
        };
	};

	if(!sprint.begin())
	{
		gui.showMessage("WARNING:", "CO2 Sensor not found.");
	};

	input_value1 = NAN;
	input_value2 = NAN;

	setman.begin();
	pid1.begin();
	pid2.begin();

    ScreenPtr scr = std::make_shared<BootScreen>(gui);
    gui.pushScreen(scr);
};

void loop()
{
    soogh_event_t e = static_cast<soogh_event_t>(key2event(scan_keys()));
	gui.handle(e);

	setman.loop();
	sensor_loop();
	pid1.loop();
	pid2.loop();
	gui.loop();
};

void sensor_loop()
{
	static time_t next = 0;
	time_t now = millis();
	if(next > now)
		return;
	next = now + settings.sensor_loop_ms;

	// Temperature (from SHT31)
	SHT3X::measurement_t* m = sht_sensor.newMeasurement();
	if(m->error)
	{
		input_value1 = NAN;
		return;
	};

	input_value1 = m->temperature;

	// CO2 from SprintIR-WX-20
	int ppm = sprint.getPPM();
	if(ppm < 0)
	{
		input_value2 = NAN;
		return;
	};
	input_value2 = ppm;
};

void halt(const char* error)
{
	DBG("HALT: %s", error);
	while(true)
		gui.loop();
};

uint32_t scan_keys()
{
	// Read current states
	uint32_t pressed = KEY_NONE;
	if(digitalRead(PIN_BTN_A) == LOW)
		pressed |= KEY_A;
	if(digitalRead(PIN_BTN_B) == LOW)
		pressed |= KEY_B;
	if(digitalRead(PIN_BTN_C) == LOW)
		pressed |= KEY_C;
	// if(digitalRead(PIN_POWERINT) == LOW)
	// 	pressed |= KEY_P;
	return pressed;
};
