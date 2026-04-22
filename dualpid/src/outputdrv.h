#ifndef __OUTPUT_H
#define __OUTPUT_H

#include <stdint.h>
#include <time.h>

#include <Arduino.h>

#include "config.h"

typedef enum
{
	OUTPUT_DRIVER_NONE 					= 0,

	// PWM
	OUTPUT_DRIVER_SLOWPWM_IO 			= 10,
	OUTPUT_DRIVER_SLOWPWM_M5HBRIDGE,
	OUTPUT_DRIVER_SLOWPWM_M5SSR,
	OUTPUT_DRIVER_SLOWPWM_M5ACSSR,

	OUTPUT_DRIVER_FASTPWM_IO 			= 20,

	// DAC, Network, another PID input, etc?
	OUTPUT_DRIVER_GP8413 				= 100,
} output_driver_t;

#define OUTPUT_DRIVER_MENULIST \
	{OUTPUT_DRIVER_NONE, 				"none",   	"no output"}, 		\
	{OUTPUT_DRIVER_SLOWPWM_IO, 			"sPWM",   	"SlowPWM-IO"}, 		\
	{OUTPUT_DRIVER_SLOWPWM_M5HBRIDGE, 	"sHBG", 	"SlowPWM-HBridge"},	\
	{OUTPUT_DRIVER_FASTPWM_IO, 			"PWM",    	"FastPWM-IO"},		\
	{OUTPUT_DRIVER_GP8413,				"DAC",	  	"GP8413"},			\
	{OUTPUT_DRIVER_SLOWPWM_M5SSR,		"UnitSSR", 	"Unit SSR"},		\
	{OUTPUT_DRIVER_SLOWPWM_M5ACSSR,		"ACSSR",   	"Unit ACSSR"}	

typedef struct
{
	bool inverted;
	union {
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
		struct
		{
			int32_t i2c_addr;	// 0 = use default (0x20)
			int32_t _unused;
			int32_t windowtime;
		} m5unit_hbridge;
	};
} output_driver_config_t;


class OutputDriver
{
	public:
		OutputDriver() 
			// : _settings(settings) 
			{};
		virtual ~OutputDriver() {};

		virtual bool begin(const output_driver_config_t &cfg, const int32_t channel_id);		    

		virtual void loop() {};
		virtual void off() = 0;
		virtual void set(float percent) = 0;
		virtual void setInverted(bool inverted) { _inverted = inverted; };
	
	protected:
		bool	 _inverted = false;	// Output is inverted
		uint32_t _channel_id;

	private:
		OutputDriver(const OutputDriver&) = delete;
	    OutputDriver& operator=(OutputDriver const&) = delete;
};

class NoneOutputDriver : public OutputDriver
{
	public:
		NoneOutputDriver() : OutputDriver() {};

		void off() override {};
		void set(float percent) override {};
};

class SlowPWMBase : public OutputDriver
{
	public:
		SlowPWMBase() : OutputDriver() {};

		bool begin(const output_driver_config_t &cfg, const int32_t channel_id);
		void loop() override;
		void off();
		void set(float percent);

	protected:
		virtual void _on() = 0;
		virtual void _off() = 0;

		uint32_t _window_len; 		// windowtime in ms (1000/f)
		uint32_t _state;
		uint32_t _window_lowtime;
		uint32_t _window_hightime;

	private:
		uint32_t _next_transition_ms = 0;

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

#ifdef OUTPUTDRV_M5UNIT_HBRIDGE_ENABLED
#include <M5UnitHbridge.h>
class M5UnitHbridgeDriver : public SlowPWMBase
{
	public:
		bool begin(const output_driver_config_t &cfg, const int32_t channel_id);
		void off();

	private:
		void _on();
		void _off();

		M5UnitHBridge _hbridge;
};
#endif // OUTPUTDRV_M5UNIT_HBRIDGE_ENABLED

#endif // __OUTPUT_H