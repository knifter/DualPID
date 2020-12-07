#include "settings.h"

#include <Arduino.h>
#include <EEPROM.h>

bool SettingsManager::begin()
{  
	EEPROM.begin(4 * sizeof(double));
	return true;
};

void SettingsManager::write_flash(double *para)
{
	int eeAddress = 0;   //Location we want the data to be put.

	for (int i = 0; i < 5; i++)
	{
		EEPROM.writeDouble(eeAddress, para[i]);
		Serial.println(para[i]);
		eeAddress += sizeof(double);
	};

	EEPROM.commit();
};

void SettingsManager::read_flash(double *para)
{
	double test = 0;

	test = EEPROM.readDouble(3 *sizeof(double));
	Serial.println(test);
	if ( test != 0.0)
	{
		Serial.println("test");
		int eeAddress = 0;   //Location we want the data to be put.

		for (int i = 0; i < 4; i++)
		{
			para[i] = EEPROM.readDouble(eeAddress);
			Serial.println(para[i]);
			eeAddress += sizeof(double);
		}
	}
}
