#ifndef __SCREENS_MAIN_H
#define __SCREENS_MAIN_H

#include <stdlib.h>
#include <stdint.h>

#include <soogh.h>

#include "event.h"
#include "gui.h"

/*** MAIN ************************************************************************************/
class PidWidget
{
	public:
		PidWidget(lv_obj_t* parent, const char* unit);
		lv_obj_t	*box, *lbl_sp, *lbl_value, *bar_output;
		lv_style_t 	style_font26;
		String unit;
		void setSetPoint(float sp);
		void setValue(float v);
		void setBar(float p);   
};

class MainScreen : public Screen
{
    public:
		MainScreen(SooghGUI& g);

		virtual ScreenType type() { return ScreenType::MAIN; };

        virtual bool loop();
		virtual bool handle(event_t);
	
	private:
		PidWidget* pw1 = nullptr;
		PidWidget* pw2 = nullptr;
		
        // lv_obj_t    *_box_pid2;

};

#endif // __SCREENS_MAIN_H
