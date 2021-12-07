#ifndef __SCREENS_MAIN_H
#define __SCREENS_MAIN_H

#include <stdlib.h>
#include <stdint.h>

#include <soogh.h>
#include <treemenu.h>

#include "gui.h"

/*** MAIN ************************************************************************************/
class PidWidget;
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
	    lv_obj_t * chart;
		lv_chart_series_t *ser1, *ser2;
        // lv_obj_t    *_box_pid2;

		TreeMenu menu;

};

#endif // __SCREENS_MAIN_H
