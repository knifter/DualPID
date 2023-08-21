#include "screens.h"

#include <memory>
#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "tools-log.h"
#include "globals.h"

/*** BOOT ************************************************************************************/
BootScreen::BootScreen(SooghGUI& g) : Screen(g)
{
    _start = millis();

	lv_obj_t *label = lv_label_create(_screen);
    lv_obj_set_size(label, LV_PCT(100), 50);
	lv_obj_center(label);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(label, "DualPID " VERSIONSTR_LONG);
    if(expert_mode)
        lv_obj_set_style_text_color(label, COLOR_DEEP_ORANGE, 0);
};

void BootScreen::load()
{
	Screen::load();
    _start = millis();
};

void BootScreen::loop()
{
    uint32_t now = millis();
    if(expert_mode)
        return;
    if((now - _start) > BOOTSCREEN_TIMEOUT_MS)
    {
        gui.pushScreenType(ScreenType::MAIN);
    };
	return;
};

bool BootScreen::handle(soogh_event_t e)
{
	switch(e)
	{
		case KEY_A_SHORT:
		case KEY_B_SHORT:
		case KEY_C_SHORT:
            gui.pushScreenType(ScreenType::MAIN);
			return true;
		default:
			return false;
	};
};

