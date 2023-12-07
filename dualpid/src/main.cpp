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

    expert_mode = 0;
	
	setman.begin();

	for(PIDLoop* pid : pids)
	{
		pid->begin();
	};

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
	for(PIDLoop* pid : pids)
	{
		pid->loop();
	};
	gui.loop();
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
