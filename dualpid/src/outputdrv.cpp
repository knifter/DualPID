#include "outputdrv.h"

#include "config.h"
#include "tools-log.h"

#include "freertos/task.h"

bool SlowPWMBase::begin(output_driver_config_t &cfg, int32_t channel_id)
{
	_window_len = cfg.slowpwm.windowtime;

    // start the output task
    _task_running = true;
    _channel_id = channel_id;

    uint32_t core = xPortGetCoreID();
    uint32_t taskcore = (core + 1) & 0x01;
    DBG("ch%d: Starting SlowPWM task on core %d", channel_id, taskcore);

    xTaskCreatePinnedToCore(
        task,               // pcTaskCode
        "SlowPWM",          // const char *const pcName,
        2048,               // ulStackDepth,
        this,               // pvParameters,
        8,                  // uxPriority,
        &_taskh,            // pvCreatedTask
        taskcore            // xCoreID
    );

	return OutputDriver::begin(cfg, channel_id);
};

void SlowPWMBase::off()
{
    // make sure task doesn't turn it on and also set lowtime low so task responds fast
    _window_lowtime = 100;
    _window_hightime = 0;
};

void SlowPWMBase::set(float percent)
{
    float duty = percent / 100.0;

    _window_hightime = duty * _window_len;
    _window_lowtime = (1-duty) * _window_len;
    // DBG("set duty = %.2f, low = %d, high = %d", duty, _window_lowtime, _window_hightime);
};

void SlowPWMBase::task(void* ptr)
{       
    SlowPWMBase* me = static_cast<SlowPWMBase*>(ptr);

    DBG("ch%dtask: Starting up on core %d", me->_channel_id, xPortGetCoreID());

    me->_state = LOW;
    me->_window_lowtime = 100;
    me->_window_hightime = 0;
    
    while(me->_task_running)
    {
        // time_t now = millis();
        switch(me->_state)
        {
            case HIGH:
                // set pin LOW
                me->_off();

                // DBG("ch%d, %lu: set LOW, wait %d", 
                //     me->_channel_id, now, me->_window_lowtime);

                // delay = low_time = (1-percent)*windowtime
                delay(me->_window_lowtime);

                // state LOW only if there is high_time to prevent glitches
                if(me->_window_hightime)
                    me->_state = LOW;

                break;
            case LOW:
                // set pin HIGH
                me->_on();

                // DBG("ch%d, %lu: set HIGH, wait %d",
                //     me->_channel_id, now, me->_window_hightime);

                // delay = high_time = percent*windowtime
                delay(me->_window_hightime);

                // state HIGH only if there is LOW_time
                if(me->_window_lowtime)
                    me->_state = HIGH;

                break;
        };
    };
};

bool SlowPWMDriver::begin(output_driver_config_t &cfg, int32_t channel_id)
{
    _pin_p = static_cast<gpio_num_t>(cfg.slowpwm.pin_p);
    DBG("ch%d SlowPWM (IO) on gpio_num_%u", channel_id, _pin_p);

    if(_pin_p == GPIO_NUM_NC)
    {
        WARNING("SlowPWMDriver not configured (pin_p)");
        return false;
    };

    digitalWrite(_pin_p, LOW);
    pinMode(_pin_p, OUTPUT);

    return SlowPWMBase::begin(cfg, channel_id);
};

void SlowPWMDriver::off()
{
    if(_pin_p != GPIO_NUM_NC)
        digitalWrite(_pin_p, LOW);

    SlowPWMBase::off();
};

void SlowPWMDriver::_on()
{
    digitalWrite(_pin_p, LOW);
};
void SlowPWMDriver::_off()
{
    digitalWrite(_pin_p, HIGH);
};


bool FastPWMDriver::begin(output_driver_config_t &cfg, int32_t channel_id)
{
    _pin_p = static_cast<gpio_num_t>(cfg.fastpwm.pin_p);
    _channel_id = channel_id;

	// We need atleast a pin..
    if(_pin_p == GPIO_NUM_NC)
    {
        WARNING("FastPWMDriver not configured: no pin_p");
        return false;
    };

    // config hardware
    pinMode(_pin_p, OUTPUT);
    digitalWrite(_pin_p, LOW);

    uint32_t freq = ledcSetup(channel_id, cfg.fastpwm.frequency, FASTPWM_BITRES);
    ledcAttachPin(_pin_p, _channel_id);
    ledcWrite(_channel_id, 0);
    DBG("ch%d: FastPWM Configured on gpio_num_%u, actual frequency %u Hz", _channel_id, _pin_p, freq);

	return OutputDriver::begin(cfg, channel_id);
};

void FastPWMDriver::off()
{
    ledcWrite(_channel_id, 0);
    return;
};

void FastPWMDriver::set(float percent)
{
    uint32_t pwm_val = percent * (1<<FASTPWM_BITRES) / 100;
    // DBG("PWM set %u / %u", pwm_val, (1<<FASTPWM_BITRES));
    ledcWrite(_channel_id, pwm_val);
    return;
};

#ifdef OUTPUTDRV_GP8413_ENABLED
#include <GP8413.h>
bool GP8413Driver::begin(output_driver_config_t &cfg, int32_t channel_id)
{
    for(uint8_t addr = 0; addr < 8; addr++)
    {
        if(_dac.begin(GP8413_ADDRESS_DEFAULT + addr))
        {
            _dac_channel = (GP8413::channel_num_t) (channel_id == 2);

            _dac.setOutputRange(GP8413::RANGE_10V);

            return true;
        };

        // DBG("DAC-GP8413(0x%x) Not Found!", GP8413_ADDRESS_DEFAULT + addr);
    };
    ERROR("DAC-GP8413(0x%x - 0x%x) Not Found!", GP8413_ADDRESS_DEFAULT, GP8413_ADDRESS_DEFAULT + 7);
    return false;
};

void GP8413Driver::off()
{
    _dac.setOutput(_dac_channel, 0x0000);
};

void GP8413Driver::set(float percent)
{
    uint16_t value = 0x7FFF * (percent / 100.0);
    // DBG("set value = ch:%d = %f %% =  %d", _dac_channel, percent, value);
    _dac.setOutput(_dac_channel, value);
};
#endif // OUTPUTDRV_GP8413_ENABLED

