#include "output.h"

#include "globals.h"
#include "pidloop.h"
#include "tools-log.h"

bool OutputDriver::begin(int32_t channel_id)
{
    return _begin_ok = true;
};

bool SlowPWMDriver::begin(int32_t channel_id)
{
    PIDLoop::settings_t pidset;
    switch(channel_id)
    {
        case 1: pidset = settings.pid1; break;
        case 2: pidset = settings.pid2; break;
    };

    // _pin_n = static_cast<gpio_num_t>(_settings.pin_n);
	// _pin_n = GPIO_NUM_NC;
    _pin_p = static_cast<gpio_num_t>(pidset.pin_p);

	// We need atleast a pin..
	if(_pin_p == GPIO_NUM_NC)
	{
		WARNING("OutputDriver SlowPWMDriver not configured (pin_p)");
		return false;
	};

    // config hardware
  	// pinMode(_pin_n, OUTPUT);
  	pinMode(_pin_p, OUTPUT);
	// digitalWrite(_pin_n, LOW);
	digitalWrite(_pin_p, LOW);

	_window_start = millis();
	_window_len = pidset.windowtime;
    _window_low = 0;
	return OutputDriver::begin();
};

void SlowPWMDriver::off()
{
    // digitalWrite(_pin_n, LOW);
	digitalWrite(_pin_p, LOW);
    _window_low = 0;
};

void SlowPWMDriver::set(float percent)
{
    _window_low = percent * _window_len / 100;
};

void SlowPWMDriver::loop()
{
    // Process timewindow valve depending on PID Output
    time_t now = millis();
    if (now - _window_start > _window_len)
    { //time to shift the Relay Window
        _window_start += _window_len;
    };

    // Set output
    // uint8_t N = LOW;
    uint8_t P = LOW;

	// Always MODE_ZP since 01-2024
	if((now - _window_start) > _window_low)
		P = HIGH;
    digitalWrite(_pin_p, P);
};

// bool FastPWMDriver::begin()
// {
//     ledcSetup(_id, 1, 10);
//     ledcAttachPin(_pin_p, _id);
//     ledcWrite(_id, 0);
// };

// void FastPWMDriver::off()
// {
//     ledcWrite(_id, 0);
//     return;
// };

// void FastPWMDriver::set(float percent)
// {
//     ledcWrite(_id, _output_value * 1024 / 100);
//     return;
// };
