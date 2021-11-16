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
#include "event.h"
#include "tools-nocopy.h"

#define KEY_SLOW 				500
#define KEY_FAST 				100
#define KEY_VERY_FAST 			50
#define KEY_STEP_SLOW 			0.1
#define KEY_STEP_FAST 			0.5

class Screen;
enum class ScreenType;

typedef std::shared_ptr<Screen> ScreenPtr;
typedef std::stack<ScreenPtr>	ScreenStack;

class GUI : NonCopyable
{
	public:
		// GUI() {};

		bool begin();
		time_t loop();

		ScreenPtr	pushScreen(ScreenType, void* data = nullptr);
		ScreenPtr	pushScreen(ScreenPtr, void* data = nullptr);
		void		popScreen(Screen* = nullptr);

		void showMessage(const char* title, const char* text);

	private:
		ScreenStack		_scrstack;
		time_t			_prv_tick;
		
		// LVGL
		lv_disp_draw_buf_t 	_lv_draw_buf;
	    lv_color_t 			_lv_color_buf[LV_BUF_SIZE];
		lv_disp_drv_t 		_lv_display_drv;        /*Descriptor of a display driver*/
		lv_indev_drv_t 		_lv_touch_drv;           /*Descriptor of a input device driver*/
		lv_indev_drv_t 		_lv_keys_drv;           /*Descriptor of a input device driver*/

	#ifdef GUI_DEBUG
		void			draw_debug();
		int				_debug_page;
		bool			_debug		= false;
		time_t 			_next_debug = 0;
	#endif

};

#endif // __GUI_H