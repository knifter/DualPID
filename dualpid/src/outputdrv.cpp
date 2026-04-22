#include "outputdrv.h"

#include "config.h"
#include "tools-log.h"

bool OutputDriver::begin(const output_driver_config_t &cfg, const int32_t channel_id)
{
    _inverted = cfg.inverted;

    return true;
};

bool SlowPWMBase::begin(const output_driver_config_t &cfg, const int32_t channel_id)
{
	_window_len = cfg.slowpwm.windowtime;
	_state = LOW;

	DBG("ch%d: SlowPWM window %d ms", channel_id, _window_len);
	return OutputDriver::begin(cfg, channel_id);
};

void SlowPWMBase::off()
{
    _state = LOW;
    _window_hightime = 0;
    _window_lowtime = _window_len;
};

void SlowPWMBase::set(float percent)
{
    float duty = percent / 100.0f;
    _window_hightime = duty * _window_len;
    _window_lowtime = (1.0f - duty) * _window_len;
};

void SlowPWMBase::loop()
{
    uint32_t now = millis();
    if(_next_transition_ms > now)
        return;

    switch(_state)
    {
        case LOW:
            _off();
            _next_transition_ms = now + _window_lowtime;
            if(_window_hightime)
                _state = HIGH;
            break;
        case HIGH:
            _on();
            _next_transition_ms = now + _window_hightime;
            if(_window_lowtime)
                _state = LOW;
            break;
    };
};

bool SlowPWMDriver::begin(const output_driver_config_t &cfg, const int32_t channel_id)
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
    _off();

    SlowPWMBase::off();
};

void SlowPWMDriver::_on()
{
    digitalWrite(_pin_p, _inverted ? LOW : HIGH);
};
void SlowPWMDriver::_off()
{
    digitalWrite(_pin_p, _inverted ? HIGH : LOW);
};

bool FastPWMDriver::begin(const output_driver_config_t &cfg, const int32_t channel_id)
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
    ledcWrite(_channel_id, _inverted ? (1<<FASTPWM_BITRES) : 0);
    return;
};

void FastPWMDriver::set(float percent)
{
    if(_inverted)
        percent = 100.0f - percent;

    uint32_t pwm_val = percent * (1<<FASTPWM_BITRES) / 100;
    // DBG("PWM set %u / %u", pwm_val, (1<<FASTPWM_BITRES));
    ledcWrite(_channel_id, pwm_val);
    return;
};

#ifdef OUTPUTDRV_GP8413_ENABLED
#include <GP8413.h>
bool GP8413Driver::begin(const output_driver_config_t &cfg, const int32_t channel_id)
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
    _dac.setOutput(_dac_channel, _inverted ? 0x7FFF : 0x0000);
};

void GP8413Driver::set(float percent)
{
    if(_inverted)
        percent = 100.0f - percent;
    uint16_t value = 0x7FFF * (percent / 100.0);
    // DBG("set value = ch:%d = %f %% =  %d", _dac_channel, percent, value);
    _dac.setOutput(_dac_channel, value);
};
#endif // OUTPUTDRV_GP8413_ENABLED

#ifdef OUTPUTDRV_UNITSSR_ENABLED
bool UnitSSRDriver::begin(const output_driver_config_t &cfg, const int32_t channel_id)
{
    uint8_t addr = cfg.unitssr.i2c_addr ? (uint8_t)cfg.unitssr.i2c_addr : UNITSSR_ADDRESS_DEFAULT;
    if(!_ssr.begin(addr))
    {
        ERROR("UnitSSR not found at 0x%02x", addr);
        return false;
    };
    return SlowPWMBase::begin(cfg, channel_id);
};

void UnitSSRDriver::off()
{
    _ssr.setRelay(false);
    SlowPWMBase::off();
};

void UnitSSRDriver::_on()  { _ssr.setRelay(!_inverted); };
void UnitSSRDriver::_off() { _ssr.setRelay(_inverted); };
#endif // OUTPUTDRV_UNITSSR_ENABLED

#ifdef OUTPUTDRV_UNITACSSR_ENABLED
bool UnitACSSRDriver::begin(const output_driver_config_t &cfg, const int32_t channel_id)
{
    uint8_t addr = cfg.unitacssr.i2c_addr ? (uint8_t)cfg.unitacssr.i2c_addr : UNITACSSR_ADDRESS_DEFAULT;
    if(!_ssr.begin(addr))
    {
        ERROR("UnitACSSR not found at 0x%02x", addr);
        return false;
    };
    return SlowPWMBase::begin(cfg, channel_id);
};

void UnitACSSRDriver::off()
{
    _ssr.setRelay(false);
    SlowPWMBase::off();
};

void UnitACSSRDriver::_on()  { _ssr.setRelay(true); };
void UnitACSSRDriver::_off() { _ssr.setRelay(false); };
#endif // OUTPUTDRV_UNITACSSR_ENABLED

#ifdef OUTPUTDRV_M5UNIT_HBRIDGE_ENABLED
#include <M5UnitHbridge.h>
bool M5UnitHbridgeDriver::begin(const output_driver_config_t &cfg, const int32_t channel_id)
{
    uint8_t addr = cfg.m5unit_hbridge.i2c_addr ? (uint8_t)cfg.m5unit_hbridge.i2c_addr : M5UNIT_HBRIDGE_ADDRESS_DEFAULT;
    if(!_hbridge.begin(addr))
    {
        ERROR("M5UnitHBridge not found at 0x%02x", addr);
        return false;
    };
    _hbridge.setDirection(M5UnitHBridge::FORWARD);
    _hbridge.setSpeed8(0);
    return SlowPWMBase::begin(cfg, channel_id);
};

void M5UnitHbridgeDriver::off()
{
    _hbridge.setSpeed8(0);
    SlowPWMBase::off();
};

void M5UnitHbridgeDriver::_on()  { _hbridge.setSpeed8(_inverted ? 0   : 255); };
void M5UnitHbridgeDriver::_off() { _hbridge.setSpeed8(_inverted ? 255 : 0  ); };
#endif // OUTPUTDRV_M5UNIT_HBRIDGE_ENABLED

