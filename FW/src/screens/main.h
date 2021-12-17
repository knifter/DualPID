#ifndef __SCREENS_MAIN_H
#define __SCREENS_MAIN_H

#include <stdlib.h>
#include <stdint.h>

#include <soogh.h>
#include <treemenu.h>

#include "gui.h"

/*** MAIN ************************************************************************************/
class PidWidget;
class GraphWidget;
class MainScreen : public Screen
{
    public:
		MainScreen(SooghGUI& g);

		virtual ScreenType type() { return ScreenType::MAIN; };

        virtual bool loop();
		virtual bool handle(soogh_event_t);
	
	private:
		PidWidget* pw1 = nullptr;
		PidWidget* pw2 = nullptr;
		GraphWidget* gw = nullptr;
};

#endif // __SCREENS_MAIN_H
