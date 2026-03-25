#ifndef __OUTPUT_H
#define __OUTPUT_H

#include <stdint.h>
#include <time.h>

#include <Arduino.h>

#include "config.h"

typedef enum
{
	OUTPUT_DRIVER_NONE = 0,

	// PWM
	OUTPUT_DRIVER_SLOWPWM,
	OUTPUT_DRIVER_FASTPWM,

	// DAC, Network, another PID input, etc?
	OUTPUT_DRIVER_GP8413 = 10,
	OUTPUT_DRIVER_UNITSSR,
	OUTPUT_DRIVER_UNITACSSR,
} output_driver_t;

#define OUTPUT_DRIVER_MENULIST \
	{OUTPUT_DRIVER_NONE, 		"none",   "no output"}, 	\
	{OUTPUT_DRIVER_SLOWPWM, 	"sPWM",   "SlowPWM-IO"}, 		\
	{OUTPUT_DRIVER_FASTPWM, 	"PWM",    "FastPWM-IO"},		\
	{OUTPUT_DRIVER_GP8413,		"DAC",	  "GP8413"},		\
	{OUTPUT_DRIVER_UNITSSR,		"UnitSSR", "Unit SSR"},		\
	{OUTPUT_DRIVER_UNITACSSR,	"ACSSR",   "Unit ACSSR"}

typedef union {
	int32_t param[3];
	struct
	{
		int32_t pin_n;
		int32_t pin_p;
		int32_t windowtime;
	} slowpwm;
	struct
	{
		int32_t pin_n;
		int32_t pin_p;
		int32_t frequency;
	} fastpwm;
	struct
	{
		int32_t i2c_addr;	// 0 = use default (0x25)
		int32_t _unused;
		int32_t windowtime;
	} unitssr;
	struct
	{
		int32_t i2c_addr;	// 0 = use default (0x50)
		int32_t _unused;
		int32_t windowtime;
	} unitacssr;
} output_driver_config_t;

class OutputDriver
{
	public:
		OutputDriver() 
			// : _settings(settings) 
			{};
		virtual ~OutputDriver() {};

		virtual bool begin(const output_driver_config_t &cfg, const int32_t channel_id) { return true; };
		    

		virtual void off() = 0;
		virtual void set(float percent) = 0;
	
	protected:
		uint32_t _channel_id;

		// PIDLoop::settings_t& _settings;

	private:
		OutputDriver(const OutputDriver&) = delete;
	    OutputDriver& operator=(OutputDriver const&) = delete;
};

class SlowPWMBase : public OutputDriver
{
	public:
		SlowPWMBase() : OutputDriver() {};

		bool begin(const output_driver_config_t &cfg, const int32_t channel_id);
		void off();
		void set(float percent);
		// void loop();

	protected:
		virtual void _on() = 0;
		virtual void _off() = 0;

		static void task(void*);
		TaskHandle_t _taskh;
		bool _task_running = false;
		uint32_t _window_len; 		// windowtime in ms (1000/f)

		// task variables
		uint32_t _state;
		uint32_t _window_lowtime;
		uint32_t _window_hightime;

};

class SlowPWMDriver : public SlowPWMBase
{
	public:
		bool begin(const output_driver_config_t &cfg, const int32_t channel_id);
		void off();

	private:
		void _on();
		void _off();

        // gpio_num_t _pin_n;
		gpio_num_t _pin_p;
};

class FastPWMDriver : public OutputDriver
{
	public:
		FastPWMDriver() : OutputDriver() {};

		bool begin(const output_driver_config_t &cfg, const int32_t channel_id);
		void off();
		void set(float percent);
		// void loop();

	private:
		gpio_num_t _pin_p;
};

#ifdef OUTPUTDRV_GP8413_ENABLED
#include <GP8413.h>
class GP8413Driver : public OutputDriver
{
	public:
		GP8413Driver() : OutputDriver() {};

		bool begin(const output_driver_config_t &cfg, const int32_t channel_id);
		void off();
		void set(float percent);
		// void loop();

	private:
		gpio_num_t _pin_p;
		GP8413 _dac;
		GP8413::channel_num_t _dac_channel;
};
#endif // OUTPUT_DAC_GP8413

#ifdef OUTPUTDRV_UNITSSR_ENABLED
#include <UnitSSR.h>
class UnitSSRDriver : public SlowPWMBase
{
	public:
		bool begin(const output_driver_config_t &cfg, const int32_t channel_id);
		void off();

	private:
		void _on();
		void _off();

		UnitSSR _ssr;
};
#endif // OUTPUTDRV_UNITSSR_ENABLED

#ifdef OUTPUTDRV_UNITACSSR_ENABLED
#include <UnitACSSR.h>
class UnitACSSRDriver : public SlowPWMBase
{
	public:
		bool begin(const output_driver_config_t &cfg, const int32_t channel_id);
		void off();

	private:
		void _on();
		void _off();

		UnitACSSR _ssr;
};
#endif // OUTPUTDRV_UNITACSSR_ENABLED

#endif // __OUTPUT_H