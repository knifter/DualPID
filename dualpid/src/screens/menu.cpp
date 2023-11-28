#include "screens.h"

#include <soogh.h>
#include <widgets/treemenu.h>

#include "gui.h"

#include "config.h"
#include "sensors.h"
#include "tools-log.h"
#include "globals.h"
#include "soogh-debug.h"

// C-style callbacks
void menu_close_cb(MenuItem* item, void* data);

SelectorField::item_t hardware_ports[] = {
	{0, "none", "none/unused"},
	PIDLOOP_PORTS_LIST
	,{0, 0, 0}
	};

// SelectorField::item_t sensor_loop_times [] = {
// 	{100, "100ms", "100 ms"},
// 	{200, "200ms", "200 ms"},
// 	{500, "500ms", "500 ms"},
// 	{1000, "1s", "1 sec"},
// 	{5000, "5s", "5 sec"},
// 	{0, 0, 0}
// 	};

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
    {PIDLoop::MODE_ZP,   "0+", "ZeroPos"},
    // {PIDLoop::MODE_NZP,  "-0+", "NegZeroPos"},
    {PIDLoop::MODE_NP,   "-+", "NegPos"},
    {PIDLoop::MODE_NZ,   "-0", "NegZero"},
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

SelectorField::item_t sensor_types [] = {
    SENSOR_TYPES_LIST,
    {0, 0, 0},
    };

bool need_reboot = false;
void set_need_reboot(MenuItem*, void*)
{
    need_reboot = true;
};

MenuScreen::MenuScreen(SooghGUI& g) : Screen(g)
{
    menu.addCloseMenuButton();
    

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
                    sp_prec = PID1_PRECISION;
                    break;
            case 2: set = &settings.pid2;
                    name = PID2_NAME;
                    sp_min = PID2_SETPOINT_MIN;
                    sp_max = PID2_SETPOINT_MAX;
                    sp_prec = PID2_PRECISION;
                    break;
            default: ERROR("Un-implemented channel number: %d", ch); 
                    return;
        };

        if(set->mode != PIDLoop::MODE_NONE || expert_mode)
        {
            menu.addSeparator(name);
            menu.addSpinbox("Setpoint", &set->fpid.setpoint, sp_min, sp_max, sp_prec);
            menu.addSwitch("Active", &set->active);
            auto sub = menu.addSubMenu("PID Settings");
            sub->addSpinbox("kP", &set->fpid.kP, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
            sub->addSpinbox("kI", &set->fpid.kI, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
            sub->addSpinbox("kD", &set->fpid.kD, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
            sub->addSpinbox("kF", &set->fpid.kF, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
            sub->addSpinbox("F-Offset", &set->fpid.kF_offset, sp_min, sp_max, sp_prec);
            if(expert_mode)
            {
                sub = menu.addSubMenu("Advanced");
                sub->addSpinbox("D-Filter", &set->input_filter, 0, 1, 2);
                sub->addSelector("Lock Time", &set->lock_time, lock_times);
                sub->addSpinbox("Lock Window", &set->lock_window, 0, sp_max - sp_min);
            };
        };
        if(expert_mode)
        {
            auto sub = menu.addSubMenu("Setup");
            sub->addSelector("Sensor", &set->sensor_type, sensor_types)->onChange(set_need_reboot);
            sub->addSelector("Mode", &set->mode, pid_modes)->onChange(set_need_reboot);
            sub->addSelector("Pin N (-)", &set->pin_n, hardware_ports)->onChange(set_need_reboot);
            sub->addSelector("Pin P (+)", &set->pin_p, hardware_ports)->onChange(set_need_reboot);
            sub->addSpinbox("Input Filter", &set->input_filter, 0, 1, 2);
            sub->addSelector("Windowtime", &set->windowtime, window_loop_times)->onChange(set_need_reboot);    
            sub->addSelector("Looptime", &set->looptime, pid_loop_times);
            // sub->addCheckbox("Take-Back-Half", &set->fpid.takebackhalf);
        };
    };

    menu.addSeparator("Config");
    menu.addSelector("Graph Delta", &settings.graph_delta, graph_delta_times);

    // General
    if(expert_mode)
    {
        auto sub = menu.addSubMenu("General");
        sub->addSelector("Measure time", &settings.sensor_loop_ms, /* sensor_loop_times */ pid_loop_times);
        sub = menu.addSubMenu("NVM");
        sub->addAction("Save NVM", [](MenuItem*, void*){ setman.save(); });
        sub->addAction("Erase NVM", [](MenuItem*, void*){ setman.erase(); });    
        // sub->addAction("Begin", [](MenuItem*, void*){ setman.begin(true); });
    };

	menu.onClose(menu_close_cb, this);
	menu.open();
};

void menu_close_cb(MenuItem* item, void* data)
{
	DBG("Menu closing!");
	setman.saveDelayed();

	if(need_reboot)
	{
		setman.save();
		gui.showMessage("INFO", "Settings changed. Need to reboot!", [](lv_event_t* e){ ESP.restart(); });
	};

    static_cast<MenuScreen*>(data)->close();
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
			return true;
		case KEY_B_SHORT:
			menu.sendKey(LV_KEY_ENTER);
			return true;
		case KEY_B_LONG:
			menu.sendKey(LV_KEY_ESC);
			return true;
		case KEY_C_SHORT:
		case KEY_C_LONG:
		case KEY_C_LONG_REPEAT:
			menu.sendKey(LV_KEY_RIGHT);
			return true;
		case KEY_AC_LONG:
            DBG("before AC_LONG");
			// gui.popScreen();
            close();
            DBG("after AC_LONG");
			return true;
		default: break;
	};

	return true; // Don't propagate event further below mainscreen
};
