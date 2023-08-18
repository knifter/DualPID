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

SelectorField::item_t hardware_ports[] = {
	{0, "none", "none/unused"},
	PIDLOOP_PORTS_LIST
	,{0, 0, 0}
	};

SelectorField::item_t sensor_loop_times [] = {
	{100, "100ms", "100 ms"},
	{1000, "1s", "1 sec"},
	{5000, "5s", "5 sec"},
	{0, 0, 0}
	};

SelectorField::item_t pid_loop_times [] = {
	{100, 	"100ms", "100 ms"},
	{200, 	"200ms", "200 ms"},
	{500, 	"500ms", "500 ms"},
	{1000, 	"1s",    "1 sec"},
	{5000, 	"5s",    "5 sec"},
	{10000, "10s",   "10 sec"},
	{60000, "1m",    "1 min"},
	{0, 0, 0}
	};

SelectorField::item_t window_loop_times [] = {
	{1000, 	"1s",   "1 sec"},
	{2000, 	"2s",   "2 sec"},
	{5000, 	"5s",   "5 sec"},
	{10000, "10s",  "10 sec"},
	{60000, "1m",   "1 min"},
	{0, 0, 0}
	};

SelectorField::item_t pid_modes [] = {
    {PIDLoop::MODE_NONE, "-", "Sensor"},
    {PIDLoop::MODE_ZP,   "ZP", "ZeroPos"},
    // {PIDLoop::MODE_NZP,     "NZP", "NegZeroPos"},
    {PIDLoop::MODE_NP,   "NP", "NegPos"},
    // {PIDLoop::MODE_NZ,   }; "NZ", "NegZero"},
    {0, 0, 0}
};

SelectorField::item_t graph_delta_times [] = {
	{100, "100ms", "100 ms"},
	{200, "200ms", "200 ms"},
	{1000, "1s", "1 sec"},
	{5000, "5s", "5 sec"},
	{60000, "1m", "1 min"},
	{600000, "10m", "10 min"},
	{0, 0, 0}
	};

SelectorField::item_t lock_times [] = {
	{1000, "1s", "1 sec"},
	{10000, "10s", "10 sec"},
	{60000, "1m", "1 min"},
	{0, 0, 0}
	};

bool need_reboot = false;

MenuScreen::MenuScreen(SooghGUI& g) : Screen(g)
{
    // Add menu for each active channel
    int ch = 0;
    while(++ch <= NUMBER_OF_CHANNELS)
    {
        PIDLoop::settings_t* set = nullptr;
        const char* name;
        float sp_min, sp_max, sp_prec;
        switch(ch)
        {
            case 1: set = &settings.pid1;
                    name = PID1_NAME;
                    sp_min = PID1_SETPOINT_MIN;
                    sp_max = PID1_SETPOINT_MAX;
                    sp_prec = PID1_SETPOINT_PRECISION;
                    break;
            case 2: set = &settings.pid2;
                    name = PID2_NAME;
                    sp_min = PID2_SETPOINT_MIN;
                    sp_max = PID2_SETPOINT_MAX;
                    sp_prec = PID2_SETPOINT_PRECISION;
                    break;
            default: ERROR("Un-implemented channel number: %d", ch); 
                    return;
        };
        DBG("menu channel %d: %p %d", ch, set, set->mode);
            
        if(set->mode != PIDLoop::MODE_NONE)
        {
            menu.addSeparator(name);
            menu.addSpinbox("Setpoint", &set->fpid.setpoint, sp_min, sp_max, sp_prec);
            menu.addSwitch("Active", &set->active);
            auto sub = menu.addSubMenu("PID Settings");
            sub->addSpinbox("kP", &set->fpid.kP, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
            sub->addSpinbox("kI", &set->fpid.kI, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
            sub->addSpinbox("kD", &set->fpid.kD, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
        };
        if(expert_mode)
        {
            if(set->mode == PIDLoop::MODE_NONE)
                menu.addSeparator(name);
            auto sub = menu.addSubMenu("Setup");
            sub->addSelector("Mode", &set->mode, pid_modes)->onChange( [](MenuItem*, void*){ need_reboot = true; });
            switch(set->mode)
            {
                case PIDLoop::MODE_NONE: break;
                case PIDLoop::MODE_NP:
                    sub->addSelector("Pin N (-)", &set->pin_n, hardware_ports)->onChange( [](MenuItem*, void*){ need_reboot = true; });
                case PIDLoop::MODE_ZP:
                    sub->addSelector("Pin P (+)", &set->pin_p, hardware_ports)->onChange( [](MenuItem*, void*){ need_reboot = true; });
            };
        	// sub->addCheckbox("Take-Back-Half", &set->fpid.takebackhalf);
            sub->addSelector("Looptime", &set->looptime, pid_loop_times);
            sub->addSelector("Windowtime", &set->windowtime, window_loop_times)->onChange( [](MenuItem*, void*){ pid1.begin(); });    
            sub->addSelector("Lock Time", &set->lock_time, lock_times);
            sub->addSpinbox("Lock Window", &set->lock_window, 0, sp_max - sp_min);

            sub->onClose(check_reboot_cb);
        };
    };

    menu.addSeparator("Config");
    menu.addSelector("Graph Delta", &settings.graph_delta, graph_delta_times);

    // General
    if(expert_mode)
    {
        auto sub = menu.addSubMenu("General");
        sub->addSelector("Measure time", &settings.sensor_loop_ms, sensor_loop_times);
        sub = menu.addSubMenu("NVM");
        sub->addAction("Save NVM", [](MenuItem*, void*){ setman.save(); });
        sub->addAction("Erase NVM", [](MenuItem*, void*){ setman.erase(); });    
        // sub->addAction("Begin", [](MenuItem*, void*){ setman.begin(true); });
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
		case KEY_B_LONG:
			menu.sendKey(LV_KEY_ESC);
			break;
		case KEY_C_SHORT:
		case KEY_C_LONG:
		case KEY_C_LONG_REPEAT:
			menu.sendKey(LV_KEY_RIGHT);
			break;
		case KEY_AC_LONG:
			gui.popScreen();
			return true;
		default: break;
	};
	return true;
};
