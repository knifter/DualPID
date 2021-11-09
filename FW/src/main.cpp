#include <Arduino.h>
#include <SHT3X.h>

#include "config.h"
#include "globals.h"
#include "settings.h"
#include "pid.h"

#include "tools-log.h"

#include <M5Stack.h>


void halt(const char*);
void loop_measure();

void setup()
{
	Serial.begin(115200);

  	M5.begin();

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

	Wire.begin(PIN_SDA, PIN_SCL);
	if(!sht_sensor.begin())
	{
		halt("SHT3X sensor error");
	};

	gui.begin();
	setman.begin();
	pid1.begin();
	pid1.set_tuning(setman.settings.pid1);
	pid2.begin();
	pid2.set_tuning(setman.settings.pid2);
};

void loop()
{
	M5.update();
	gui.loop();
	setman.loop();
	pid1.loop();
	pid2.loop();
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
