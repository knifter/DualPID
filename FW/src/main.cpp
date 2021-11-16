#include <Arduino.h>
#include <SHT3X.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "pid.h"

#include "tools-log.h"

void halt(const char*);
void loop_measure();

void setup()
{
	Serial.begin(115200);

    // Init IO
    digitalWrite(PIN_HB1_A, LOW);
    digitalWrite(PIN_HB1_B, LOW);
    digitalWrite(PIN_HB2_A, LOW);
    digitalWrite(PIN_HB2_B, LOW);
  	pinMode(PIN_HB1_A, OUTPUT);
  	pinMode(PIN_HB1_B, OUTPUT);
  	pinMode(PIN_HB2_A, OUTPUT);
  	pinMode(PIN_HB2_B, OUTPUT);
	digitalWrite(PIN_SPEAKER, LOW);
	pinMode(PIN_SPEAKER, OUTPUT);

    // while(1)
    // {
    //     static bool tmp;
    //     digitalWrite(PIN_HB2_B, tmp=!tmp);
    //     delay(500);
    // };

	gui.begin();

	Wire.begin(PIN_SDA, PIN_SCL);
	if(!sht_sensor.begin())
	{
		halt("SHT3X sensor error");
	};

	setman.begin();
	pid1.begin();
	pid1.set_tuning(setman.settings.pid1);
	pid2.begin();
	pid2.set_tuning(setman.settings.pid2);

	// gui.showMessage("Test", "a very long text this is not.\nBut long enough?");
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
	return keytool_get_event(pressed);
};

void loop()
{
	setman.loop();
	pid1.loop();
	pid2.loop();
	gui.loop(); 
};

void halt(const char* error)
{
	DBG("HALT: %s", error);
	gfx.fillScreen(TFT_RED);
	gfx.setTextSize(3);
	gfx.setTextColor(TFT_WHITE, TFT_RED);
	gfx.setCursor(5, 5);
	gfx.print(error);
	while(1);
};
