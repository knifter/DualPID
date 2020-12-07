#include <Arduino.h>
#include <M5Stack.h>
#include <SHT3X.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "pid.h"

#include <tools-log.h>

// Modi of display

void setup()
{
	Serial.begin(115200);

  	M5.begin();
  	M5.Lcd.fillScreen(BLACK);

  	pinMode(PIN_VALVE, OUTPUT);
	digitalWrite(PIN_SPEAKER, LOW);
	pinMode(PIN_SPEAKER, OUTPUT);
	

	Wire.begin(PIN_SDA, PIN_SCL);
	if(!sht_sensor.begin())
	{
		DBG("ERROR: SHT31 Sensor init failed.");
		while(1);
	};

	settings.begin();
  	read_flash(para);
  	pid_begin();
};

void loop()
{
	M5.update();
	gui.loop();
	pid_loop();
};
