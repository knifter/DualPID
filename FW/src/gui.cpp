// Sohow an menu item with current value
#include "gui.h"

#include <M5Stack.h>

#include "globals.h"
#include "pid.h"
#include "config.h"
#include "tools-log.h"

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240

param_t& operator++(param_t& orig)
{
	if(orig < _PARAM_MAX)
		orig = static_cast<param_t>(orig + 1);
	else
		orig = static_cast<param_t>(0);
	return orig;
};
param_t& operator--(param_t& orig)
{
	if(orig == 0)
		orig = static_cast<param_t>(_PARAM_MAX - 1);
	else
	    orig = static_cast<param_t>(orig - 1);
	return orig;
};

// param_t operator++(param_t& orig, int)
// {
// 	param_t rVal = orig;
// 	++orig;
// 	return rVal;
// };
// param_t operator--(param_t& orig, int)
// {
// 	param_t rVal = orig;
// 	--orig;
// 	return rVal;
// };

bool GUI::begin()
{
    M5.Lcd.fillScreen(BLACK);
	_state = state_t::BOOT;
	return true;
};

void GUI::loop()
{
	switch(_state)
	{
		case state_t::BOOT:
			draw_boot();
			_holdoff = millis() + 1500;
			_state = state_t::BOOT_WAIT;
			break;
		case BOOT_WAIT:
			if(millis() > _holdoff)
				_state = state_t::MAIN;
			break;
		case state_t::MAIN:
			if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed())
			{
				_state = state_t::SELECT_PARAM;
				draw_menu();
				return;
			}
			draw_main();
			break;

		case state_t::CHANGE_PARAM:
			parameter_change();
			break;

		case state_t::SELECT_PARAM:
			select_parameter();
			break;
	}; //switch
};

void GUI::draw_boot()
{
	M5.Lcd.fillScreen(BLACK);

	M5.Lcd.setTextSize(3);
	M5.Lcd.setCursor (10, 10);
	M5.Lcd.print("ESP-PID");
	M5.Lcd.setTextSize(2);
	M5.Lcd.setCursor (10, 40);
	M5.Lcd.printf("version %d", VERSION);
};

void GUI::draw_main()
{
	// M5.Lcd.fillScreen(BLACK);

	// See if its time yet
	time_t now = millis();
	static time_t display_next = millis();
	if(now < display_next)
		return;

	float MeasRH  = sht_sensor.getHumidity();
	float MeasT = sht_sensor.getTemperature();
	M5.Lcd.setTextSize(3);
	M5.Lcd.setTextColor(WHITE, BLACK);

	int y = 10;
	M5.Lcd.setCursor(10, y);
	M5.Lcd.print("T =  ");
	M5.Lcd.print(MeasT, 1);
	M5.Lcd.print(" C  ");

	y += 30;
	M5.Lcd.setCursor(10, y);
	M5.Lcd.print("RH =  ");
	M5.Lcd.print(MeasRH, 1);
	M5.Lcd.print(" %  ");

	y += 30;
	M5.Lcd.setCursor(10, y);
	M5.Lcd.print("Set = ");
	M5.Lcd.print(settings.setpoint, 1);
	M5.Lcd.print(" %  ");
    // lcd.print(Output, 1);

	display_next += DISPLAY_LOOPTIME_MS;

	// float outpercent = ( (pid_get_output()-WINDOWSIZE)/WINDOWSIZE);
	float outpercent = -1 + pid_get_output()/WINDOWSIZE*2;
	draw_outputbar(0, 100, SCREEN_WIDTH, 12, outpercent);
};

void GUI::draw_outputbar(const int x, const int y, const int w, const int h, const float percent)
{
	const int y_mid = y+h/2;
	const int x_mid = x+w/2;
	const int bw = abs(percent*w/2);	// Width of bar
	// DBG("Out = %.0f, p = %.2f, bw=%d", pid_get_output(), percent, bw);

	// Clear previous
	M5.Lcd.fillRect(x, y, w, h, BLACK);

	// HZ Centerline with vertical mid-marker
	M5.Lcd.drawLine(x, y_mid, x+w, y_mid, LIGHTGREY);
	M5.Lcd.drawLine(x_mid, y, x_mid, y+h, RED);

	// Filled bar, starting mid going left or right
	if(percent > 0)
		M5.Lcd.fillRect(x_mid+1, y+1, bw, h-2, WHITE);
	else
		M5.Lcd.fillRect(x_mid-bw, y+1, bw, h-2, WHITE);
};

// show every menu item with current values
void GUI::draw_menu()
{ 
	M5.Lcd.fillScreen(BLACK);

	// Menu list
  	M5.Lcd.setTextSize(3);

  	for (int i = 0; i < _PARAM_MAX; i++)
    	draw_menuitem(i);

	// Bottom help
	M5.Lcd.setTextColor(WHITE, BLUE);
	M5.Lcd.setCursor(40, 225);
	M5.Lcd.setTextSize(2);
	M5.Lcd.print("<<<    Select    >>>");
};

// Current parameter to change
void GUI::draw_menuitem(int item)
{
	M5.Lcd.setTextSize(3);

	int y = item * 40 + 20;
	const char* txt = ParamNames[item];

	// Selected item gets a different color
	M5.Lcd.setCursor (10, y);
	if ( _selected_parameter == item )
		M5.Lcd.setTextColor(BLUE, WHITE);
	else
		M5.Lcd.setTextColor(WHITE, BLACK);

	// Print item
	M5.Lcd.print(txt);
	if(item == PARAM_BACK)
		return;

	// draw value
	M5.Lcd.setTextSize(3);
	M5.Lcd.print(" = ");
	Para_Cusor_X[item] = M5.Lcd.getCursorX();
	Para_Cusor_Y[item] = M5.Lcd.getCursorY();
	double value;
	switch(item)
	{
		case PARAM_SETPOINT:value = settings.setpoint; break;
		case PARAM_KP: 		value = settings.Kp; break;
		case PARAM_KI: 		value = settings.Ki; break;
		case PARAM_KD: 		value = settings.Kd; break;
		default: 			value= NAN; break;
	};
	M5.Lcd.print(value);
	M5.Lcd.print("      ");
};

// Parameter to change will be red colored
void GUI::draw_highlight_param()
{
	int item = _selected_parameter;
	M5.Lcd.setTextSize(3);
	M5.Lcd.setCursor(Para_Cusor_X[item], Para_Cusor_Y[item]);
	M5.Lcd.setTextColor(RED, WHITE);
	M5.Lcd.print(*_settingptr);
};

void GUI::parameter_change()
{
	// Acceleration logic
	static float key_step = KEY_STEP_SLOW;
	static int key_delay = KEY_SLOW;
	if (M5.BtnA.pressedFor(5000) or M5.BtnC.pressedFor(5000))
	{
		key_delay = KEY_VERY_FAST;
		key_step = KEY_STEP_FAST;
	}
  	else if (M5.BtnA.pressedFor(2000) or M5.BtnC.pressedFor(2000))
  	{
	    key_delay = KEY_FAST;
  	}
	else
  	{
    	key_delay = KEY_SLOW;
    	key_step = KEY_STEP_SLOW;
  	};

	// Action
  	if (M5.BtnA.isPressed())
  	{
		*_settingptr -= key_step;
		draw_highlight_param();
		delay(key_delay);
  	};
  	if (M5.BtnB.wasPressed())
  	{
    	_state = state_t::SELECT_PARAM;
    	M5.Lcd.fillScreen(BLACK);
    	draw_menu();
  	};
  	if (M5.BtnC.isPressed())
	{	
		*_settingptr += key_step;
		draw_highlight_param();
		delay(key_delay);
  	};
};


void GUI::select_parameter()
{
  	if (M5.BtnA.wasPressed())
  	{
		--_selected_parameter;
		// if(_selected_parameter == PARAM_NONE)
		// 	_selected_parameter = (_PARAM_MAX - 1;
		draw_menu();
		return;
  	};
  	if (M5.BtnB.wasPressed())
  	{
		if(_selected_parameter == PARAM_BACK)
		{
			// TODO: move to state machine
			// settings.write_flash(para);
			setman.saveDelayed();
			pid_set_tuning(setman.settings);

			M5.Lcd.fillScreen(BLACK);
			_state = state_t::MAIN;
			return;
		};
		_state = state_t::CHANGE_PARAM;
		switch(_selected_parameter)
		{
			case PARAM_SETPOINT:	_settingptr = &settings.setpoint; break;
			case PARAM_KP: 			_settingptr = &settings.Kp; break;
			case PARAM_KI: 			_settingptr = &settings.Ki; break;
			case PARAM_KD: 			_settingptr = &settings.Kd; break;
			default: 				_settingptr = nullptr; break;
		};
		draw_highlight_param();
		return;
	};
	if (M5.BtnC.wasPressed())
	{
		++_selected_parameter;
		// if(_selected_parameter == _PARAM_MAX)
		// 	_selected_parameter = 1;
		draw_menu();
		return;
	};
};
