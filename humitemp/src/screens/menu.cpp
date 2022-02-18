#include "screens.h"

#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "tools-log.h"
#include "globals.h"
#include "soogh-debug.h"
#include <treemenu.h>

// C-style callbacks
void check_reboot_cb(MenuItem* item, void* data);
void menu_close_cb(MenuItem* item, void* data);

SelectorField::item_t pidloop_ports[] = {
	{0, "none", "none/unused"},
	PIDLOOP_PORTS_LIST
	,{0, 0, 0}
	};

bool need_reboot;

MenuScreen::MenuScreen(SooghGUI& g) : Screen(g)
{
	menu.addSeparator("Temperature");
	menu.addSpinbox("Setpoint", &settings.pid1.fpid.setpoint, TEMPERATURE_MIN, TEMPERATURE_MAX, TEMPERATURE_PRECISION);
	menu.addSwitch("Active", &settings.pid1.active);
    auto sub = menu.addSubMenu("PID Settings");
	sub->addSpinbox("kP", &settings.pid1.fpid.kP, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
	sub->addSpinbox("kI", &settings.pid1.fpid.kI, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
	sub->addSpinbox("kD", &settings.pid1.fpid.kD, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);

	menu.addSeparator("Humidity");
	menu.addSpinbox("Setpoint", &settings.pid2.fpid.setpoint, HUMIDITY_MIN, HUMIDITY_MAX, HUMIDITY_PRECISION);
	menu.addSwitch("Active", &settings.pid2.active);
    sub = menu.addSubMenu("Settings");
	sub->addSpinbox("kP", &settings.pid2.fpid.kP, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
	sub->addSpinbox("kI", &settings.pid2.fpid.kI, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
	sub->addSpinbox("kD", &settings.pid2.fpid.kD, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);	// sub->addCheckbox("Take-Back-Half", &settings.pid1.fpid.takebackhalf);

	menu.addSeparator("Setup");
    sub = menu.addSubMenu("Pins");
	sub->addSelector("Temp -, Pin N", &settings.pid1.pin_n, pidloop_ports)->onChange( [](MenuItem*, void*){ need_reboot = true; });
	sub->addSelector("Temp +, Pin P", &settings.pid1.pin_p, pidloop_ports)->onChange( [](MenuItem*, void*){ need_reboot = true; });
	sub->addSelector("RH% -, Pin N", &settings.pid2.pin_n, pidloop_ports)->onChange( [](MenuItem*, void*){ need_reboot = true; });
	sub->addSelector("RH% +, Pin P", &settings.pid2.pin_p, pidloop_ports)->onChange( [](MenuItem*, void*){ need_reboot = true; });
	sub->onClose(check_reboot_cb);

	menu.onClose(menu_close_cb);
	menu.open();
};

void check_reboot_cb(MenuItem* item, void* data)
{
	if(need_reboot)
	{
		setman.save();
		gui.showMessage("INFO", "Settings changed. Need to reboot!", [](lv_event_t* e){ ESP.restart(); });
	};
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