#include "screens.h"

#include <memory>
#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "tools-log.h"
#include "globals.h"

#include <esp_mac.h>

/*** BOOT ************************************************************************************/
BootScreen::BootScreen(SooghGUI& g) : Screen(g)
{
    _start = millis();

	lv_obj_t *label = lv_label_create(_screen);
    lv_obj_set_size(label, LV_PCT(100), LV_SIZE_CONTENT);
	lv_obj_center(label);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    lv_label_set_text_fmt(label, "DualPID %s\n%02x-%02x-%02x-%02x-%02x-%02x", VERSION_STR,
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if(settings.expert_mode)
        lv_obj_set_style_text_color(label, COLOR_DEEP_ORANGE, 0);
};

void BootScreen::load()
{
	Screen::load();
    _start = millis();
    _hold = false;  // also when MAIN pops back to us: drop the hold that pushed MAIN
    // if(settings.expert_mode)
    //     _start = 0;
};

void BootScreen::loop()
{
    uint32_t now = millis();
    if(!_hold && (now - _start) > BOOTSCREEN_TIMEOUT_MS)
    {
        gui.pushScreenType(ScreenType::MAIN);
    };
	return;
};

bool BootScreen::handle(soogh_event_t e)
{
	switch(e)
	{
		// All keys up (always the last event of any press): skip the remaining delay
		case KEY_RELEASED:
			if(_hold)
				gui.pushScreenType(ScreenType::MAIN);
			return true;
		// Key down (SHORT arrives ~50ms after press) keeps the boot screen up until KEY_RELEASED
		case KEY_A_SHORT:
		case KEY_B_SHORT:
		case KEY_C_SHORT:
		case KEY_A_PRESSED:
		case KEY_B_PRESSED:
		case KEY_C_PRESSED:
			_hold = true;
			return true;
		default:
			return false;
	};
};

