#ifndef __GUI_H
#define __GUI_H

// #undef min
#include <stdlib.h>
#include <stdint.h>
#include <stack>
#include <memory>
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

class GUI
{
	public:
		GUI() {};

		bool begin();
		void loop();

		ScreenPtr	pushScreen(ScreenType, void* data = NULL);
		ScreenPtr	pushScreen(ScreenPtr, void* data = NULL);
		ScreenPtr	pushMessageScreen(const char* title, const char* line1 = nullptr, const char* line2 = nullptr, const char* line3 = nullptr);
		void		popScreen(Screen* = nullptr);

	private:
		ScreenStack		_scrstack;

		// LVGL
		lv_disp_draw_buf_t 	_lv_draw_buf;
	    lv_color_t 			_lv_color_buf[LV_BUF_SIZE];
		lv_disp_drv_t 		_lv_display_drv;        /*Descriptor of a display driver*/
		lv_indev_drv_t 		_lv_touch_drv;           /*Descriptor of a input device driver*/
		lv_indev_drv_t 		_lv_keys_drv;           /*Descriptor of a input device driver*/

#ifdef DEBUG
		void			draw_debug();
		int				_debug_page;
		bool			_debug		= false;
		time_t 			_next_debug = 0;
#endif
};

#endif // __GUI_H