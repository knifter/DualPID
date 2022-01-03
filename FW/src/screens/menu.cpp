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
	menu.addFloat("Setpoint", &settings.pid1.setpoint);
    auto sub1 = menu.addSubMenu("PID");
	sub1->addFloat("kP", &settings.pid1.Kp);
	sub1->addFloat("kI", &settings.pid1.Ki);
	sub1->addFloat("kD", &settings.pid1.Kd);

	menu.addSeparator("Humidity");
	menu.addFloat("Setpoint", &settings.pid1.setpoint);
    auto sub2 = menu.addSubMenu("PID");
	sub2->addFloat("kP", &settings.pid2.Kp);
	sub2->addFloat("kI", &settings.pid2.Ki);
	sub2->addFloat("kD", &settings.pid2.Kd);

	menu.onClose(menu_close_cb);

	menu.open();
};

void menu_close_cb(MenuItem* item, void* data)
{
	DBG("Menu closing!");
	setman.saveDelayed();
	pid1.set_tuning(settings.pid1);
	pid2.set_tuning(settings.pid2);
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
	if(e>KEY_C)
		DBG("e = %s", soogh_event_name(e));
	
	if(!menu.isOpen())
	{
		DBG("Menu is closed, end screen.");
		gui.popScreen();
		return true;
	};

	switch(e)
	{
		case KEY_A_SHORT:
			menu.sendKey(TreeMenu::KEY_LEFT);
			break;
		case KEY_B_SHORT:
			menu.sendKey(TreeMenu::KEY_ENTER);
			break;
		case KEY_C_SHORT:
			menu.sendKey(TreeMenu::KEY_RIGHT);
			break;
		case KEY_B_LONG:
			menu.sendKey(TreeMenu::KEY_ESC);
			break;
		case KEY_AC_LONG:
			gui.popScreen();
			return true;
		default: break;
	};
	return false;
};
