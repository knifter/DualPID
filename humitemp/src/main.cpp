#include <Arduino.h>
#include <SHT3X.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "pidloop.h"
#include "screens.h"

#include "tools-log.h"

void halt(const char*);
void loop_measure();
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

    // while(1)
    // {
    //     static bool tmp;
    //     digitalWrite(PIN_HB2_B, tmp=!tmp);
    //     delay(500);
    // };

	gui.begin();

	Wire.begin(PIN_SDA, PIN_SCL);
	if(!sht_sensor.begin(SHT3X_ADDRESS_DEFAULT))
	{
        if(!sht_sensor.begin(SHT3X_ADDRESS_ALT))
        {
			gui.showMessage("WARNING:", "SHT3X(0x44/0x45): not found");
        };
	};

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
	pid1.loop();
	pid2.loop();
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
