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

	lv_group_t* g = menu.group;
	lv_obj_t *obj = lv_group_get_focused(g);
	bool editable_or_scrollable = lv_obj_is_editable(obj) || lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

	switch(e)
	{
		case KEY_A_SHORT:
			if(lv_group_get_editing(g))
			{
				DBG("group.send(LEFT)");
				lv_group_send_data(g, LV_KEY_LEFT);
			}else{
				DBG("group.prev");
				lv_group_focus_prev(g);
			};
			break;
		case KEY_B_SHORT:
			if(editable_or_scrollable)
			{
				DBG("obj.editable");
				if(lv_group_get_editing(g))
				{
					DBG("send(ENTER)");
					lv_group_send_data(g, LV_KEY_ENTER);
				}else{
					DBG("group.edit -> true");
					lv_group_set_editing(g, true);
				};
			}else{
				DBG("obj.send(SHORT_CLICKED)");
				lv_event_send(obj, LV_EVENT_SHORT_CLICKED, nullptr);
				DBG("obj.send(CLICKED)");
				lv_event_send(obj, LV_EVENT_CLICKED, nullptr);
			};
			break;
		case KEY_C_SHORT:
			if(lv_group_get_editing(g))
			{
				DBG("group.send(RIGHT)");
				lv_group_send_data(g, LV_KEY_RIGHT);
			}else{
				DBG("group.next");
				lv_group_focus_next(g);
			};
			break;
		case KEY_B_LONG:
		{
			if(lv_group_get_editing(g))
			{	
				DBG("group.edit -> false");
				lv_group_set_editing(g, false);
			}else{
				DBG("group.send(ESC)");
				lv_group_send_data(g, LV_KEY_ESC);
			};
			break;
		};
		case KEY_AC_LONG:
			menu.close();
			return true;

		default: break;
	};
	return false;
};
