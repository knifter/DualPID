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
	{GPIO_NUM_NC, "none", "none/unused"},
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

SelectorField::item_t pid_fixed_values [] = {
    {0, "-", "Off/PID"},
    {1, "1", "1%"},
    {2, "2", "2%"},
    {5, "5", "5%"},
    {10, "10", "10%"},
    {20, "20", "20%"},
    {30, "30", "30%"},
    {50, "50", "50%"},
    {80, "80", "80%"},
    {90, "90", "90%"},
    {100, "max", "100%"},
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
	{300000, "5m", "5 min"},
	{0, 0, 0}
	};

SelectorField::item_t sensor_types [] = {
    SENSOR_TYPES_LIST,
    {0, 0, 0},
    };

SelectorField::item_t output_drivers [] {
	{0, 	"none",   "no output"},
	{1, 	"sPWM",   "SlowPWM"},
	{2, 	"PWM",   "FastPWM"},
	{0, 0, 0}
	};

SelectorField::item_t output_slowpwm_windowtimes [] {
	{1000, 	"1s",   "1 sec"},
	{2000, 	"2s",   "2 sec"},
	{5000, 	"5s",   "5 sec"},
	{10000, "10s",  "10 sec"},
	{60000, "1m",   "1 min"},
	{0, 0, 0}
	};

SelectorField::item_t output_fastpwm_frequencies [] {
	{1, 	"1Hz",   "1 Hz"},
	{100, 	"100Hz",   "100 Hz"},
	{1000, 	"1kHz",   "1 kHz"},
	{1000, 	"5kHz",   "5 kHz"},
	{1000, 	"30kHz",   "30 kHz"},
	{0, 0, 0}
	};

SelectorField::item_t lock_windows [] {
    // 1000 == 1%
	{1000, 	"1%",    "1%"},
	{2000, 	"2%",    "2%"},
	{5000, 	"5%",    "5%"},
	{10000, "10%",   "10%"},
	{0, 0, 0}
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
    int ch = -1;
    while(++ch < NUMBER_OF_CHANNELS)
    {
        PIDLoop* pidloop = pids[ch];
        PIDLoop::settings_t& set = pidloop->_settings;
        const char* name;
        float sp_min, sp_max, sp_prec;
        switch(set.sensor_type)
        {
            case 100 ... 199: // Temperature
                name =   Temperature_Name;
                sp_min = Temperature_Setpoint_Min;
                sp_max = Temperature_Setpoint_Max;
                sp_prec =Temperature_Precision;
                break;
            case 200 ... 299: // Humidity
                name =   Humidity_Name;
                sp_min = Humidity_Setpoint_Min;
                sp_max = Humidity_Setpoint_Max;
                sp_prec =Humidity_Precision;
                break;
            case 300 ... 399: // CO2
                name =   CO2_Name;
                sp_min = CO2_Setpoint_Min;
                sp_max = CO2_Setpoint_Max;
                sp_prec =CO2_Precision;
                break;
            case SENSOR_NONE: //break;
            default:
                name = "none";
                sp_min = 0;
                sp_max = 1;
                sp_prec = 2;
                break;
        };

        if(pidloop->mode() > PIDLoop::CONTROL_MODE_SENSOR || settings.expert_mode)
        {
            menu.addSeparator(name);
            menu.addSpinbox("Setpoint", &set.fpid.setpoint, sp_min, sp_max, sp_prec);
            menu.addSwitch("Active", &set.active);
            if(settings.expert_mode)
            {
                menu.addSelector("Fixed Output", &set.fixed_output_value, pid_fixed_values);
            };
            if(settings.expert_mode)
            {
                auto sub = menu.addSubMenu("PID Settings");
                sub->addSwitch("Active", &set.active);                

                sub->addSeparator("Standard");
                sub->addSpinbox("kP", &set.fpid.kP, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
                sub->addSpinbox("kI", &set.fpid.kI, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
                sub->addSpinbox("kD", &set.fpid.kD, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
                sub->addSpinbox("kF", &set.fpid.kF, PID_PAR_MIN, PID_PAR_MAX, PID_PAR_PRECISION);
                sub->addSpinbox("F-Offset", &set.fpid.kF_offset, sp_min, sp_max, sp_prec);

                sub->addSeparator("Advanced");
                sub->addSpinbox("D-Filter", &set.fpid.D_filter, 0, 1, 2);
#ifdef FPID_TAKEBACKHALF
                sub->addCheckbox("Take-Back-Half", &set.fpid.takebackhalf);
#endif
                sub->addAction("Save Settings now", [](MenuItem*, void*){ setman.save(); });

            };
        };
        if(settings.expert_mode)
        {
            auto sub = menu.addSubMenu("Setup");
            sub->addSeparator("Input");
            sub->addSelector("Sensor", &set.sensor_type, sensor_types)->onChange(set_need_reboot);
            sub->addSpinbox("Input Filter", &set.input_filter, 0, 1, 2);
            sub->addSeparator("PID");
            sub->addSelector("Looptime", &set.looptime, pid_loop_times);
            sub->addSelector("Lock Time", &set.lock_time, lock_times);
            sub->addSelector("Lock Window", &set.lock_window, lock_windows);
            sub->addSeparator("Output");
            sub->addSelector("Driver", &set.output_drv, output_drivers)->onChange(set_need_reboot);
            switch(set.output_drv)
            {
                case PIDLoop::OUTPUT_DRIVER_NONE:
                    break;
                case PIDLoop::OUTPUT_DRIVER_SLOWPWM:
                    // sub->addSelector("Pin N (-)", &set.pin_n, hardware_ports)->onChange(set_need_reboot);
                    sub->addSelector("Pin P (+)", &set.output.slowpwm.pin_p, hardware_ports)->onChange(set_need_reboot);
                    sub->addSelector("Windowtime", &set.output.slowpwm.windowtime, output_slowpwm_windowtimes)->onChange(set_need_reboot);    
                    break;
                case PIDLoop::OUTPUT_DRIVER_FASTPWM:
                    // sub->addSelector("Pin N (-)", &set.pin_n, hardware_ports)->onChange(set_need_reboot);
                    sub->addSelector("Pin P (+)", &set.output.fastpwm.pin_p, hardware_ports)->onChange(set_need_reboot);
                    sub->addSelector("Frequency", &set.output.fastpwm.frequency, output_fastpwm_frequencies)->onChange(set_need_reboot);    
                    break;
            };
            sub->addSpinbox("Min Output", &set.min_output, 0, 100, 0)->onChange(set_need_reboot);
            sub->addSpinbox("Max Output", &set.max_output, 0, 100, 0)->onChange(set_need_reboot);
        };
    };

    menu.addSeparator("Config");
    menu.addSelector("Graph Delta", &settings.graph_delta, graph_delta_times);

    // General
    if(settings.expert_mode)
    {
        auto sub = menu.addSubMenu("General");
        sub->addSelector("Measure time", &settings.sensor_loop_ms, /* sensor_loop_times */ pid_loop_times);
        sub = menu.addSubMenu("NVM");
        sub->addAction("Save NVM", [](MenuItem*, void*){ setman.save(); });
        sub->addAction("Erase NVM", [](MenuItem*, void*){ setman.erase(); });    
        // sub->addAction("Begin", [](MenuItem*, void*){ setman.begin(true); });
        menu.addAction("End Expert-mode", [](MenuItem*, void* me){ settings.expert_mode = false; static_cast<Screen*>(me)->close(); }, this);
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
