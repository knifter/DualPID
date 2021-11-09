// Sohow an menu item with current value
#include "gui.h"

#include "pid.h"
#include "config.h"
#include "tools-log.h"
#include "globals.h"
#include "tools-keys.h"

#include "screens.h"

#include <M5Stack.h>

bool GUI::begin()
{
    M5.Lcd.fillScreen(BLACK);

		// Empty activity stack
	while(!_scrstack.empty())
		_scrstack.pop();

	// And put bootscreen on the bottom
	pushScreen(ScreenType::BOOT);

	return true;
};

void GUI::loop()
{
		// DBG("HANDLE-BEGIN");
	_event = (event_t) keytool_get_event(scan_keys());
	if(_event)
	{
		_last_event = _event;
		DBG("GUI: event = 0x%x", _event);
	};

	if(handle_global_events(_event))
		_event = KEY_NONE;

	// ActStack may not be empty
	if(_scrstack.size() == 0)
	{
		ERROR("actstack empty, push(BOOT)");
		pushScreen(ScreenType::BOOT);
	};

	// Keeping this (smart) ptr here is important! It prevents pop() from deleting the 
	// Activity while in handle(). It will be deleted when act goes out of scope too
	ScreenPtr scr = _scrstack.top();

	// Debug activity
#ifdef DEBUG_GUI
	static ActivityPtr prev_act = nullptr;
	if(act != prev_act)
	{
		if(prev_act == nullptr)
			DBG("GUI: <null> -> %s", act->name());
		else
			DBG("GUI: %s -> %s", prev_act->name(), act->name());
		prev_act = act;
	};
#endif

    // gui statemachine, draw when needed
    switch(_state)
    {
		case DRAW:
			DBG("GUI: draw(%s)", scr->name());
			scr->draw();
			// draw_battery();

			_next_debug = 0;
			_state = SEND;
            // no break: send it right away
            break; // TODO: without this screen is not properly cleared in debug?
		case SEND:
		    // _disp.sendBuffer();

			_state = HANDLE;
			// no break; handle it right away:
            // break;
		case HANDLE:
			scr->handle(_event);
            break;
	};

#ifdef DEBUG
	if(_debug)
	{
		// Throttle our debug printing a bit
		time_t now = millis();
		if(now > _next_debug)
		{
			_next_debug = now + DEBUG_INTERVAL_MS;

			draw_debug();
            // _disp.sendBuffer();
			_state = SEND;
		};
	};
#endif

};

void GUI::draw()
{
	_state = DRAW;
};

void GUI::resend()
{
	_state = SEND;
};

bool GUI::handle_global_events(const event_t event)
{
	// Handle global key-presses
	switch(event)
	{
		case KEY_ABC_LONG:
			_debug = !_debug;
			_state = DRAW;
			return true;
		case KEY_A_LONG:
			if(_debug)
			{
				_debug_page--;
				_state = DRAW;
				return true;
			};
		case KEY_C_LONG: 
			if(_debug)
			{
				_debug_page++;
				_state = DRAW;
				return true;
			};
		// case KEY_P_LONG:
		// 	// Someone is holding the power button, we're probably going down soon. Let's save what we can
		// 	return true;
		default:
			return false;
	};

	return false;
};

ScreenPtr GUI::pushScreen(ScreenType scrtype, void* data)
{
	ScreenPtr scr = NULL;
	switch(scrtype)
	{
		case ScreenType::BOOT:			scr = std::make_shared<BootScreen>(); break;
		case ScreenType::MAIN:			scr = std::make_shared<MainScreen>(); break;
		case ScreenType::MENU:			scr = std::make_shared<MenuScreen>(); break;

		default:
			pushMessageScreen("Error:", __FUNCTION__, "Invalid <ScreenType>", " identifier"); 
			return NULL;
	};
	DBG("GUI: Push(%s)", scr->name());
	_scrstack.push(scr);
	draw();
	return scr;
};

void GUI::pushMessageScreen(const char* title, const char* line1, const char* line2, const char* line3)
{	
	_scrstack.push(
		std::make_shared<MessageScreen>(title, line1, line2, line3)
	);
	draw();
};

void GUI::popScreen(Screen* scr)
{
	// There must an activity on the stack to do this..
	if(_scrstack.empty())
		return;

	// ActivityPtr is a smart ptr. It will delete a in GUI::handle() eventually
	ScreenPtr top = _scrstack.top();
	_scrstack.pop();
	DBG("GUI: pop(%s)", top->name());

    // Just a check for now
    if(scr != nullptr && top.get() != scr)
    {
        ERROR("Screen* given does not match top().");
        pushMessageScreen("ERROR", "Screen* != top()");
        return;
    };

	// DBG("popped, will delete (eventually): %s(%p)", a->name(), a);
	_state = DRAW;
	return;
};

uint32_t GUI::scan_keys()
{
	// Read current states
	uint32_t pressed = KEY_NONE;
	if(digitalRead(PIN_BTN_A) == LOW)
		pressed |= KEY_A;
	if(digitalRead(PIN_BTN_B) == LOW)
		pressed |= KEY_B;
	if(digitalRead(PIN_BTN_C) == LOW)
		pressed |= KEY_C;
	// if(digitalRead(PIN_POWERINT) == LOW)
	// 	pressed |= KEY_P;
	return pressed;
};


#ifdef DEBUG
void GUI::draw_debug()
{	

};
#endif // DEBUG
