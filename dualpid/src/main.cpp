#include <Arduino.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "pidloop.h"
#include "screens.h"
#include "inputdrv.h"
#include "rtc.h"
#include "cli.h"

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

	rtc_begin();

	gui.begin();

	Wire.begin(PIN_SDA, PIN_SCL);

	setman.begin();

	// Start initializing PIDs and their hardware
	for(PIDLoop* pid : pids)
	{
		pid->begin();
	};

    ScreenPtr scr = std::make_shared<BootScreen>(gui);
    gui.pushScreen(scr);

	cli_begin();

};

void loop()
{
	time_t t1 = micros();
    soogh_event_t e = static_cast<soogh_event_t>(key2event(scan_keys()));
    // if(e & KEYTOOL_EVENT_MASK)
    //     DBG("event: %s", soogh_event_name(e));

	time_t t2 = micros();
	gui.handle(e);
	time_t t3 = micros();
	gui.loop();
	time_t t4 = micros();

	setman.loop();
	time_t t5 = micros();
	for(PIDLoop* pid : pids)
	{
		pid->loop();
	};
	time_t t6 = micros();

	// cli_loop(); 
	time_t t7 = micros();

	static time_t next = micros() + 3000000;
	static uint32_t time1 = 0;
	static uint32_t time2 = 0;
	static uint32_t time3 = 0;
	static uint32_t time4 = 0;
	static uint32_t time5 = 0;
	static uint32_t time6 = 0;
	static int n = 0;
	
	time1 += t2 - t1;
	time2 += t3 - t2;
	time3 += t4 - t3;
	time4 += t5 - t4;
	time5 += t6 - t5;
	time6 += t7 - t6;
	n++;

	if(t7 > next)
	{
		time1 /= n;
		time2 /= n;
		time3 /= n;
		time4 /= n;
		time5 /= n;
		time6 /= n;

		DBG("time1_event:    %d", time1);
		DBG("time2_guihandle:%d", time2);
		DBG("time3_guiloop:  %d", time3);
		DBG("time4_setman:   %d", time4);
		DBG("time5_pidloops: %d", time5);
		DBG("time6_cli:      %d", time6);
		uint32_t total = time1+time2+time3+time4+time5+time6;
		DBG("%d us total. %d loops per sec. %d us per loop: %d extra", total, n / 3, 3000000/n, 3000000/n - total);

		time1 = 0; time2 = 0; time3 = 0; time4=0;time5=0;time6=0;
		n = 0;
		next += 3000000;
	};
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
