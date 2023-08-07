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

SelectorField::item_t sensor_loop_times [] = {
	{100, "100ms", "100ms"},
	{1000, "1s", "1s"},
	{5000, "5s", "5s"},
	{0, 0, 0}
	};

SelectorField::item_t pid_loop_times [] = {
	{100, 	"100ms","100 ms"},
	{1000, 	"1s",   "1 sec"},
	{5000, 	"5s",   "5 sec"},
	{10000, "10s",  "10 sec"},
	{60000, "1m",   "1 min"},
	{0, 0, 0}
	};

SelectorField::item_t window_loop_times [] = {
	{1000, 	"1s",   "1 sec"},
	{5000, 	"5s",   "5 sec"},
	{10000, "10s",  "10 sec"},
	{60000, "1m",   "1 min"},
	{0, 0, 0}
	};


bool need_reboot = false;

MenuScreen::MenuScreen(SooghGUI& g) : Screen(g)
{
    // Channel 1
	menu.addSeparator(PID1_NAME);
	menu.addSpinbox("Setpoint", &settings.pid1.fpid.setpoint, PID1_MIN, PID1_MAX, PID1_PRECISION);
	menu.addSwitch("Active", &settings.pid1.active);
    auto sub = menu.addSubMenu("PID Settings");
	sub->addSpinbox("kP", &settings.pid1.fpid.kP, PID1_PAR_MIN, PID1_PAR_MAX, PID1_PAR_PRECISION);
	sub->addSpinbox("kI", &settings.pid1.fpid.kI, PID1_PAR_MIN, PID1_PAR_MAX, PID1_PAR_PRECISION);
	sub->addSpinbox("kD", &settings.pid1.fpid.kD, PID1_PAR_MIN, PID1_PAR_MAX, PID1_PAR_PRECISION);
    sub = menu.addSubMenu("Setup");
	sub->addSelector("Pin N (-)", &settings.pid1.pin_n, pidloop_ports)->onChange( [](MenuItem*, void*){ need_reboot = true; });
	sub->addSelector("Pin P (+)", &settings.pid1.pin_p, pidloop_ports)->onChange( [](MenuItem*, void*){ need_reboot = true; });
	menu.addSelector("Measure time", &settings.sensor_loop_ms, sensor_loop_times);    
	sub->addSelector("Looptime", &settings.pid1.looptime, pid_loop_times);
	sub->addSelector("Windowtime", &settings.pid1.windowtime, window_loop_times)->onChange( [](MenuItem*, void*){ pid1.begin(); });    
	sub->onClose(check_reboot_cb);

    // Channel 2
	menu.addSeparator(PID2_NAME);
	menu.addSpinbox("Setpoint", &settings.pid2.fpid.setpoint, PID2_MIN, PID2_MAX, PID2_PRECISION);
	menu.addSwitch("Active", &settings.pid2.active);
    sub = menu.addSubMenu("Settings");
	sub->addSpinbox("kP", &settings.pid2.fpid.kP, PID2_PAR_MIN, PID2_PAR_MAX, PID1_PAR_PRECISION);
	sub->addSpinbox("kI", &settings.pid2.fpid.kI, PID2_PAR_MIN, PID2_PAR_MAX, PID1_PAR_PRECISION);
	sub->addSpinbox("kD", &settings.pid2.fpid.kD, PID2_PAR_MIN, PID2_PAR_MAX, PID1_PAR_PRECISION);	
    sub = menu.addSubMenu("Setup");
	sub->addSelector("Pin N (-)", &settings.pid2.pin_n, pidloop_ports)->onChange( [](MenuItem*, void*){ need_reboot = true; });
	sub->addSelector("Pin P (+)", &settings.pid2.pin_p, pidloop_ports)->onChange( [](MenuItem*, void*){ need_reboot = true; });
	menu.addSelector("Measure time", &settings.sensor_loop_ms, sensor_loop_times);    
	sub->addSelector("Looptime", &settings.pid2.looptime, pid_loop_times);
	sub->addSelector("Windowtime", &settings.pid2.windowtime, window_loop_times)->onChange( [](MenuItem*, void*){ pid2.begin(); });
	sub->onClose(check_reboot_cb);

	// sub->addCheckbox("Take-Back-Half", &settings.pid1.fpid.takebackhalf);
    
    
    // sub->addAction("Begin", [](MenuItem*, void*){ setman.begin(true); });
    if(developer_mode)
    {
        menu.addSeparator("Developer Mode");
        menu.addAction("Save NVM", [](MenuItem*, void*){ setman.save(); });
        menu.addAction("Erase NVM", [](MenuItem*, void*){ setman.erase(); });
    };

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

void MenuScreen::loop()
{
    return;
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
