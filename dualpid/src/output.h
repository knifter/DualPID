#ifndef __OUTPUT_H
#define __OUTPUT_H

#include <stdint.h>
#include <time.h>

#include <Arduino.h>

class OutputDriver
{
	public:
		OutputDriver() 
			// : _settings(settings) 
			{};
		virtual ~OutputDriver() {};

		virtual bool begin(int32_t channel_id);
		virtual bool begin_ok() { return _begin_ok; };
		virtual void off() = 0;
		virtual void set(float percent) = 0;
	
	protected:
		uint32_t _channel_id;
		bool _begin_ok = false;

		// PIDLoop::settings_t& _settings;

	private:
		OutputDriver(const OutputDriver&) = delete;
	    OutputDriver& operator=(OutputDriver const&) = delete;
};

class SlowPWMDriver : public OutputDriver
{
	public:
		SlowPWMDriver() 
			: OutputDriver() 
			{};

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