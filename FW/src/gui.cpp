// Sohow an menu item with current value
#include "gui.h"

#include <lvgl.h>
#include <Arduino.h>

#include "config.h"
#include "globals.h"
#include "tools-log.h"
#include "tools-keys.h"

#include "screens.h"

ScreenPtr GUI::pushScreen(ScreenType scrtype, void* data)
{
	ScreenPtr scr = NULL;
	switch(scrtype)
	{
		case ScreenType::BOOT:			scr = std::make_shared<BootScreen>(*this); break;
		case ScreenType::MAIN:			scr = std::make_shared<MainScreen>(*this); break;

		default:
			showMessage("ERROR:",  "Invalid <ScreenType> identifier"); 
			return NULL;
	};
    SooghGUI::pushScreen(scr, data);
	return scr;
};

bool GUI::handle(soogh_event_t e)
{
    // Handle global events
    switch(e)
    {
        case KEY_A_SHORT:
        case KEY_B_SHORT:
        case KEY_C_SHORT:
            if(_msgbox)
            {
                lv_msgbox_close(_msgbox); _msgbox = nullptr;
                return true;
            };
        default: break;
    };

    // See if the Screen handles it
   	ScreenPtr scr = _scrstack.top();
    if(scr->handle(e))
        return true;

    // Give the bare keys to LVGL
    switch(e)
    {
        case KEY_A:     lvgl_enc_pressed = true;  lvgl_enc_last_key = LV_KEY_LEFT ; break;
        case KEY_B:     lvgl_enc_pressed = true;  lvgl_enc_last_key = LV_KEY_ENTER; break;
        case KEY_C:     lvgl_enc_pressed = true;  lvgl_enc_last_key = LV_KEY_RIGHT; break;
        case KEY_AC:    lvgl_enc_pressed = true;  lvgl_enc_last_key = LV_KEY_BACKSPACE; break;
        case KEY_NONE:  lvgl_enc_pressed = false;
    };
    return true;
};
