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
	
	// Hold startup, and registere debugging purposes
    expert_mode = 0;
	while(scan_keys() == KEY_AC)
    {
        expert_mode = 1;
    };

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

    input_value1 = sensor1_read();
    input_value2 = sensor2_read();    
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
