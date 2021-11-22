#ifndef __GUI_H
#define __GUI_H

#include <stdlib.h>
#include <stdint.h>
#include <Arduino.h>
#include <lvgl.h>
#include <soogh.h>

#include "config.h"
#include "event.h"

class GUI : public SooghGUI
{
	public:

		ScreenPtr	pushScreen(ScreenPtr scr, void* data = nullptr) { return SooghGUI::pushScreen(scr); };
		ScreenPtr	pushScreen(ScreenType, void* data = nullptr);

		virtual bool handle(soogh_event_t e);

	private:
		uint32_t _last_key = 0;
};

#endif // __GUI_H
