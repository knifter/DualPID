#ifndef __OUTPUT_H
#define __OUTPUT_H

#include <stdint.h>
#include <time.h>

#include <Arduino.h>


typedef enum
{
	OUTPUT_DRIVER_NONE = 0,

	// PWM
	OUTPUT_DRIVER_SLOWPWM,
	OUTPUT_DRIVER_FASTPWM,

	// DAC, Network, another PID input, etc?
} output_driver_t;

#define OUTPUT_DRIVER_MENULIST \
	{OUTPUT_DRIVER_NONE, 		"none",   "no output"}, 	\
	{OUTPUT_DRIVER_SLOWPWM, 	"sPWM",   "SlowPWM"}, 		\
	{OUTPUT_DRIVER_FASTPWM, 	"PWM",    "FastPWM"}		

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
} output_driver_config_t;

class OutputDriver
{
	public:
		OutputDriver() 
			// : _settings(settings) 
			{};
		virtual ~OutputDriver() {};

		virtual bool begin(int32_t channel_id) { return true; };
		virtual void off() = 0;
		virtual void set(float percent) = 0;
	
	protected:
		uint32_t _channel_id;

		// PIDLoop::settings_t& _settings;

	private:
		OutputDriver(const OutputDriver&) = delete;
	    OutputDriver& operator=(OutputDriver const&) = delete;
};

class SlowPWMDriver : public OutputDriver
{
	public:
		SlowPWMDriver() : OutputDriver() {};

		bool begin(int32_t channel_id);
		void off();
		void set(float percent);
		// void loop();

	private:
		static void task(void*);
		TaskHandle_t _taskh;
		bool _task_running = false;
		uint32_t _window_len; 		// windowtime in ms (1000/f)

		// task variables
		uint32_t _state;
		uint32_t _window_lowtime;
		uint32_t _window_hightime;

        // gpio_num_t _pin_n;
		gpio_num_t _pin_p;
};

class FastPWMDriver : public OutputDriver
{
	public:
		FastPWMDriver() : OutputDriver() {};

		bool begin(int32_t channel_id);
		void off();
		void set(float percent);
		// void loop();

	private:
		gpio_num_t _pin_p;
};

#endif // __OUTPUT_H