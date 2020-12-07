// Sohow an menu item with current value
#include "gui.h"

#include <M5Stack.h>

#include "globals.h"
#include "pid.h"
#include "config.h"

bool GUI::begin()
{
	return true;
};

void GUI::loop()
{
	switch(_state)
	{
		case CHANGE_P:
			parameter_change();
			break;
		case SELECT_P:
			select_parameter();
			break;
		case MEASURE:
			if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed())
			{
				_state = SELECT_P;
				M5.Lcd.fillScreen(BLACK);
				draw_menu(0);
			}else{
				draw_main(); // FIXME
			};
			break;
	}; //switch
};

// Current parameter to change
void GUI::draw_menuitem(String txt, int place, int selected)
{
	M5.Lcd.setTextSize(3);

	int y = place * 40 + 20;

	M5.Lcd.setCursor (10, y);
	if ( selected == place )
		M5.Lcd.setTextColor(BLUE, WHITE);
	else
		M5.Lcd.setTextColor(WHITE, BLACK);

	M5.Lcd.print(txt);
	if (txt != "X")
	{
		M5.Lcd.setTextSize(2);
		M5.Lcd.print(" = ");
		Para_Cusor_X[place] = M5.Lcd.getCursorX();
		Para_Cusor_Y[place] = M5.Lcd.getCursorY();
		M5.Lcd.print(para[place]);
		M5.Lcd.print("      ");
	};
};

// show every menu item with current values
void GUI::draw_menu(int it )
{ 
	int i;

	// Menu list
  	M5.Lcd.setTextSize(3);
  	Selected_Parameter = it;
  	for (i = 0; i < 5; i++)
    	draw_menuitem(item[i], i, Selected_Parameter);

	// Bottom help
	M5.Lcd.setTextColor(WHITE, BLUE);
	M5.Lcd.setCursor(40, 225);
	M5.Lcd.setTextSize(2);
	M5.Lcd.print("<<<    Select    >>>");
};

// Parameter to change will be red colored
void GUI::highlight_param(int item)
{
	M5.Lcd.setCursor(Para_Cusor_X[item], Para_Cusor_Y[item]);
	M5.Lcd.setTextColor(RED, WHITE);
	M5.Lcd.print(para[item]);
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
    	para[Selected_Parameter] = para[Selected_Parameter] - key_step;
    	highlight_param(Selected_Parameter);
    	delay(key_delay);
  	};
  	if (M5.BtnB.wasPressed())
  	{
    	_state = SELECT_P;
    	M5.Lcd.fillScreen(BLACK);
    	draw_menu(Selected_Parameter);
  	};
  	if (M5.BtnC.isPressed())
  	{
    	para[Selected_Parameter] = para[Selected_Parameter] + key_step;
    	highlight_param(Selected_Parameter);
    	delay(key_delay);
  	};
};

void GUI::select_parameter()
{
  	if (M5.BtnA.wasPressed())
  	{
    	Selected_Parameter = Selected_Parameter - 1;
    	if ( Selected_Parameter < 0) 
			Selected_Parameter = 4;
    	draw_menu(Selected_Parameter);
  	}
  	if (M5.BtnB.wasPressed())
  	{
    	if (item[Selected_Parameter] != "X")
    	{
      		_state = CHANGE_P;
      		highlight_param(Selected_Parameter);
    	}else{
      		M5.Lcd.fillScreen(BLACK);
      		settings.write_flash(para);
			//TODO: settings->flagUpdate()
			pid_set_tuning(para[Kp], para[Ki], para[Kd]);
      		_state = MEASURE;
    	};
  	};
  	if (M5.BtnC.wasPressed())
  	{
    	Selected_Parameter = Selected_Parameter + 1;
    	if ( Selected_Parameter > 4) 
			Selected_Parameter = 0;
    	draw_menu(Selected_Parameter);
  	};
};


void GUI::draw_main()
{

	// See if its time yet
	time_t now = millis();
	static time_t display_next = 0;
	if(now < display_next)
		return;

	float MeasRH  = sht_sensor.getHumidity();
	float MeasT = sht_sensor.getTemperature();
	M5.Lcd.setTextSize(4);
	M5.Lcd.setTextColor(WHITE, BLACK);

	M5.Lcd.setCursor(10, 50);
	M5.Lcd.print("T =  ");
	M5.Lcd.print(MeasT, 1);
	M5.Lcd.print(" C  ");

	M5.Lcd.setCursor(10, 100);
	M5.Lcd.print("RH =  ");
	M5.Lcd.print(MeasRH, 1);
	M5.Lcd.print(" %  ");

	M5.Lcd.setCursor(10, 150);
	M5.Lcd.print("Set = ");
	M5.Lcd.print(para[0], 1);
	M5.Lcd.print(" %  ");
    // lcd.print(Output, 1);

	display_next += DISPLAY_LOOPTIME_MS;
};
