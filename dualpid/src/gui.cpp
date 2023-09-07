// Sohow an menu item with current value
#include "gui.h"

#include <lvgl.h>
#include <Arduino.h>

#include "config.h"

#include "screens.h"

ScreenPtr GUI::pushScreenType(ScreenType scrtype, void* data)
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

