#include <Arduino.h>
#include <M5Stack.h>
#include <SHT3X.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "pid.h"

#include <tools-log.h>

void halt(const char*);

void setup()
{
	Serial.begin(115200);

  	M5.begin();

  	pinMode(PIN_VALVE, OUTPUT);
	digitalWrite(PIN_SPEAKER, LOW);
	pinMode(PIN_SPEAKER, OUTPUT);
	
	Wire.begin(PIN_SDA, PIN_SCL);
	if(!sht_sensor.begin())
	{
		halt("SHT3X error");
	};

	gui.begin();
	setman.begin();
	pid_begin();
	pid_set_tuning(setman.settings);
};

void loop()
{
	M5.update();
	gui.loop();
	setman.loop();
	pid_loop();
};

void halt(const char* error)
{
	DBG("HALT: %s", error);
	M5.Lcd.fillScreen(RED);
	M5.Lcd.setTextSize(3);
	M5.Lcd.setTextColor(WHITE, RED);
	M5.Lcd.setCursor(5, 5);
	M5.Lcd.print(error);
	while(1);
};
