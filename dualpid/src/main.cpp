#include <Arduino.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "pidloop.h"
#include "screens.h"
#include "sensors.h"

#include "tools-log.h"

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
	
	// Hold startup if C pressed
	while(scan_keys() == KEY_C);

	gui.begin();

	Wire.begin(PIN_SDA, PIN_SCL);

    if(!sensor1_begin())  
    {
		gui.showMessage("WARNING:", "Channel 1 sensor error.");
    };

    if(!sensor2_begin())
    {
		gui.showMessage("WARNING:", "Channel 2 sensor error.");
    };

    expert_mode = 0;
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
    // if(e & KEYTOOL_EVENT_MASK)
    //     DBG("event: %s", soogh_event_name(e));

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

	// Read sensors and apply averaging/filter
    double read1 = sensor1_read();
    if(isnan(input_value1))
        input_value1 = read1;
	input_value1 = read1*(1 - settings.pid1.input_filter) + input_value1*settings.pid1.input_filter;

    double read2 = sensor2_read();
    if(isnan(input_value2)) 
        input_value2 = read2;
	input_value2 = read2*(1 - settings.pid2.input_filter) + input_value2*settings.pid2.input_filter;
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
