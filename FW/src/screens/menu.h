#ifndef __SCREENS_MENU_H
#define __SCREENS_MENU_H

#include <stdlib.h>
#include <stdint.h>

#include <soogh.h>

#include "event.h"
#include "gui.h"

class MenuScreen : public Screen
{
    public:
		MenuScreen(SooghGUI &g);

		virtual ScreenType type() { return ScreenType::MENU; };

        virtual bool loop();
		virtual bool handle(event_t);
	
	private:

};

#endif // __SCREENS_MAIN_H
