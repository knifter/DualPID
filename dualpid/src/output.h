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

		bool begin(int32_t channel_id);
		virtual void off() = 0;
		virtual void set(float percent) = 0;
		bool begin_ok() { return _begin_ok; };
	
	private:
		uint32_t _channel_id;
		bool _begin_ok = false;

		// PIDLoop::settings_t& _settings;

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
		void loop();

        // typedef enum
        // {
        //     OUTPUT_MODE_NONE,      // Unconfigured, Display sensor only
        //     OUTPUT_MODE_ZP,        // Zero/Positive: heater
        //     // MODE_NZP,            // Negative/Zero/Positive: peltier off when in setpoint window
        //     OUTPUT_MODE_NP,        // Negative/Positive: peltier
        //     OUTPUT_MODE_NZ,        // Negative/Zero: cool only
        // } output_mode_t;

	private:
		uint32_t _window_len; 		// windowtime in ms (1000/f)
        time_t _window_start;
		uint32_t _window_low;

        // gpio_num_t _pin_n;
		gpio_num_t _pin_p;
};

// class FastPWMDriver
// {
// 	public:
// 		FastPWMDriver() : _settings(settings) {};

// 		begin();
// 		off();
// 		setDuty();

// 	private:
// 		PIDLoop::settings_t& _settings;
// };

#endif // __OUTPUT_H