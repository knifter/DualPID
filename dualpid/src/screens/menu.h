#ifndef __SCREENS_MENU_H
#define __SCREENS_MENU_H

#include <stdlib.h>
#include <stdint.h>

#include <soogh.h>
#include <widgets/treemenu.h>

#include "gui.h"

class MenuScreen : public Screen
{
    public:
		MenuScreen(SooghGUI &g);

		virtual ScreenType type() { return ScreenType::MENU; };

		virtual void load();
        virtual void loop();
		virtual bool handle(soogh_event_t);
	
	private:
		TreeMenu menu;
};

#endif // __SCREENS_MAIN_H
