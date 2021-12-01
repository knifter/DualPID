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
    lv_label_set_text_fmt(label, "AutoPilot v%d", VERSION);
};

void BootScreen::load()
{
	Screen::load();
    _start = millis();
};

bool BootScreen::loop()
{
    uint32_t now = millis();
    if((now - _start) > BOOTSCREEN_TIMEOUT_MS)
    {
		ScreenPtr scr = std::make_shared<MainScreen>(_gui);
        _gui.pushScreen(scr);
    };
	return false;
};

bool BootScreen::handle(uint32_t e)
{
	switch(e)
	{
		case KEY_A_SHORT:
		case KEY_B_SHORT:
		case KEY_C_SHORT:
	        gui.pushScreen(ScreenType::MAIN);
			return true;
		default:
			return false;
	};
};

