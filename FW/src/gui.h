#ifndef __GUI_H
#define __GUI_H

#include <stdlib.h>
#include <stdint.h>
#include <Arduino.h>
#include <lvgl.h>
#include <soogh.h>

#include "config.h"

class GUI : public SooghGUI
{
	public:
		ScreenPtr	pushScreenType(ScreenType, void* data = nullptr);

	private:
		uint32_t _last_key = 0;
};

#endif // __GUI_H
