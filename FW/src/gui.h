#ifndef __GUI_H
#define __GUI_H

// #undef min
#include <stdlib.h>
#include <stdint.h>
#include <stack>
#include <Arduino.h>
#include <lvgl.h>

#include "config.h"
#include "panels.h"
#include "event.h"

#define KEY_SLOW 				500
#define KEY_FAST 				100
#define KEY_VERY_FAST 			50
#define KEY_STEP_SLOW 			0.1
#define KEY_STEP_FAST 			0.5

class Screen;
enum class ScreenType;

typedef std::shared_ptr<Screen> ScreenPtr;
typedef std::stack<ScreenPtr>	ScreenStack;

enum param_t
{
	PARAM_SETPOINT, 
	PARAM_KP,
	PARAM_KI,
	PARAM_KD,
	PARAM_BACK,
	_PARAM_MAX
};
param_t& operator++(param_t& orig);
param_t& operator--(param_t& orig);

class GUI
{
	public:
		GUI() {};

		bool begin();
		void loop();
        void draw();        // Always redraws the whole screen for the current state
		void resend();		// re-send screenbuffer
        typedef enum
        {
            DRAW,
			SEND,
			HANDLE
        } state_t;

		ScreenPtr	pushScreen(ScreenType, void* data = NULL);
		void		pushMessageScreen(const char* title, const char* line1 = nullptr, const char* line2 = nullptr, const char* line3 = nullptr);
		void		popScreen(Screen* = nullptr);

	private:
		uint32_t 	scan_keys();
		bool 		handle_global_events(const event_t);

		event_t 		_event		= KEY_NONE;
        state_t 		_state		= DRAW;
		ScreenStack		_scrstack;

		// LVGL
		lv_disp_draw_buf_t 	_lv_draw_buf;
	    lv_color_t 			_lv_color_buf[LV_BUF_SIZE];
		lv_disp_drv_t 		_lv_display_drv;        /*Descriptor of a display driver*/
		lv_indev_drv_t 		_lv_touch_drv;           /*Descriptor of a input device driver*/
		lv_indev_drv_t 		_lv_keys_drv;           /*Descriptor of a input device driver*/

		int				_debug_page;
#ifdef DEBUG
		void			draw_debug();
		bool			_debug		= false;
		time_t 			_next_debug = 0;
		event_t			_last_event = KEY_NONE;
#endif
};

#endif // __GUI_H