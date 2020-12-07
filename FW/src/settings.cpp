#include "settings.h"

#include <Arduino.h>

#include "config.h"
#include "tools-log.h"

typedef struct
{
	uint32_t version;
	settings_t data;
} settingswrapper_t;

bool SettingsManager::begin()
{  
	_err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &_handle);
	if (_err != ESP_OK) 
	{
		ERROR("Unable to open NVS namespace: %s", esp_err_to_name(_err));
		return false;
	};

	if(!read_flash())
	{
		INFO("Reading settings from flash failed, initializing NVS");
		setDefaults();
		if(!write_flash())
		{
			ERROR("Writing initial settings to flash failed: %s", esp_err_to_name(_err));
			return _init = false;
		};
		return _init = true;
	};

	INFO("Read settings from flash.");
	return _init = true;
};

void SettingsManager::loop()
{
	if(!_init)
		return;
	time_t now = millis();
	if(now > _saveat && _dirty)
	{
		INFO("Saving settings to NVS");
		write_flash();
	};
};

void SettingsManager::saveDelayed(time_t later)
{
	time_t when = millis() + later;
	if(when > _saveat)
		_saveat = when;
	_dirty = true;
	DBG("%lu: Will save settings at %lu", millis(), when);
};

void SettingsManager::setDefaults()
{
	DBG("Initializing defaults.");
	settings.Kp = DEFAULT_PID_P;
	settings.Ki = DEFAULT_PID_I;
	settings.Kd = DEFAULT_PID_D;
	settings.setpoint = DEFAULT_SETPOINT;

	return;
};

bool SettingsManager::write_flash()
{
	settingswrapper_t wrapper;
	wrapper.version = VERSION;
	memcpy(&(wrapper.data), &settings, sizeof(settings));
	_err = nvs_set_blob(_handle, NVS_SETTINGS_IDENT, &wrapper, sizeof(settingswrapper_t));
	if(_err != OK)
	{
		ERROR("Failed to save settings to NVS: %s", esp_err_to_name(_err));
		return false;
	};

	nvs_commit(_handle);
	INFO("Settings saved to NVS.");
	_dirty = false;
	return true;
};

bool SettingsManager::read_flash()
{
	// get settings
	settingswrapper_t wrapper;
	size_t blobsize = sizeof(settingswrapper_t);
	_err = nvs_get_blob(_handle, NVS_SETTINGS_IDENT, &wrapper, &blobsize);
	if(_err == ESP_ERR_NVS_NOT_FOUND) 
	{
		WARNING("Pristine flash. Can't read settings.");
		return false;
	};
	if(_err != ESP_OK) 
	{
		ERROR("Unable to read settings: %s", esp_err_to_name(_err));
		return false;
  	};
	if(blobsize != sizeof(settingswrapper_t))
	{
		ERROR("NVS blobsize(%u) != my wrapper size(%u).", blobsize, sizeof(settingswrapper_t));
		return false;
	};
	if(wrapper.version != VERSION)
	{
		INFO("NVS Settings wrong version.");
		return false;
	};

	// settings were read from flash
	memcpy(&settings, &(wrapper.data), sizeof(settings_t));
	_dirty = false;

	return true;
};

bool SettingsManager::erase()
{
	esp_err_t err = nvs_erase_key(_handle, NVS_SETTINGS_IDENT);
	if(err == ESP_ERR_NVS_NOT_FOUND)
	{
		WARNING("NVS key '%s' not present. Nothing to erase.", NVS_SETTINGS_IDENT);
		return false;
	};
	ESP_ERROR_CHECK(err);
	nvs_commit(_handle);
	INFO("NVM Settings erased.");
	return true;
};
