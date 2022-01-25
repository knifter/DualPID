#include "screens.h"

#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "tools-log.h"
#include "globals.h"
#include "soogh-debug.h"
#include <treemenu.h>

// C-style callbacks
void menu_close_cb(MenuItem* item, void* data);

MenuScreen::MenuScreen(SooghGUI& g) : Screen(g)
{
	menu.addSeparator("Temperature");
	menu.addSpinbox("Setpoint", &settings.pid1.setpoint, TEMPERATURE_MIN, TEMPERATURE_MAX, TEMPERATURE_PRECISION);
	menu.addSwitch("Active", &settings.pid1.active);
    auto sub1 = menu.addSubMenu("Settings");
	sub1->addSpinbox("kP", &settings.pid1.kP, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
	sub1->addSpinbox("kI", &settings.pid1.kI, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
	sub1->addSpinbox("kD", &settings.pid1.kD, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);

	menu.addSeparator("Humidity");
	menu.addSpinbox("Setpoint", &settings.pid2.setpoint, HUMIDITY_MIN, HUMIDITY_MAX, HUMIDITY_PRECISION);
	menu.addSwitch("Active", &settings.pid2.active);
    auto sub2 = menu.addSubMenu("Settings");
	sub2->addSpinbox("kP", &settings.pid2.kP, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
	sub2->addSpinbox("kI", &settings.pid2.kI, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
	sub2->addSpinbox("kD", &settings.pid2.kD, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);

	menu.onClose(menu_close_cb);

	menu.open();
};

void menu_close_cb(MenuItem* item, void* data)
{
	DBG("Menu closing!");
	setman.saveDelayed();

	gui.popScreen();
};

void MenuScreen::load()
{
	// Do not load my screen, menu is overlayed
};

bool MenuScreen::loop()
{
    
    return false;
};

bool MenuScreen::handle(soogh_event_t e)
{
	// if(e>KEY_C)
	// 	DBG("e = %s", soogh_event_name(e));
	
	switch(e)
	{
		case KEY_A_SHORT:
		case KEY_A_LONG:
		case KEY_A_LONG_REPEAT:
			menu.sendKey(LV_KEY_LEFT);
			break;
		case KEY_B_SHORT:
			menu.sendKey(LV_KEY_ENTER);
			break;
		case KEY_C_SHORT:
		case KEY_C_LONG:
		case KEY_C_LONG_REPEAT:
			menu.sendKey(LV_KEY_RIGHT);
			break;
		case KEY_B_LONG:
			menu.sendKey(LV_KEY_ESC);
			break;
		case KEY_AC_LONG:
			gui.popScreen();
			return true;
		default: break;
	};
	return true;
};
