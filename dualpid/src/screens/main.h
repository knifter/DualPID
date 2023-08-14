#ifndef __SCREENS_MAIN_H
#define __SCREENS_MAIN_H

#include <stdlib.h>
#include <stdint.h>

#include <soogh.h>
#include <treemenu.h>

#include "gui.h"

/*** MAIN ************************************************************************************/
class PidPanel;
class GraphPanel;
class MainScreen : public Screen
{
    public:
		MainScreen(SooghGUI& g);

		virtual ScreenType type() { return ScreenType::MAIN; };

        virtual void loop();
		virtual bool handle(soogh_event_t);
	
	private:
		PidPanel* pw1 = nullptr;
		PidPanel* pw2 = nullptr;
		GraphPanel* gw = nullptr;

		time_t _next_update = 0;
		time_t _next_chart = 0;
};

#endif // __SCREENS_MAIN_H
