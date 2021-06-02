#include "widgets.h"

#include <stdint.h>
#include <M5Stack.h>

#include "config.h"

#ifdef DEBUG_WIDGETS_C
	#define TOOLS_LOG_DEBUG
#endif
#include "tools-log.h"

Widget::Widget(const int t, const int l, const int h, const int w)
{
	_top = t;
	_left = l;
	_height = h;
	_width = w;
};

void Widget::setSize(const int t, const int l, const int h, const int w)
{
	_top = t;
	_left = l;
	_height = h;
	_width = w;	
};

ProgressBar::ProgressBar(int t, int l, int w, int h) : Widget(t, l, w, h)
{
    _percent = 0;
};

void ProgressBar::setValue(int p)
{
    if(p<0)
        p = 0;
    if(p>100)
        p = 100;
    _percent = p;
};

void ProgressBar::draw()
{
    // Width of filled bar
    int bar = (_width * _percent) / 100;

    // Clear pb center
    // _disp.setColorIndex(0); // White
    // _disp.drawBox(_x+1, _y+1, _w-2, _h-2);
	M5.lcd.fillRect(_left+1, _top+1, _width-2, _height-2, WHITE);

    // Fill center
    // _disp.setColorIndex(1); // Black
    // _disp.drawFrame(_x, _y, _w, _h);
	M5.lcd.drawRect(_left, _top, _width, _height, BLACK);
    // _disp.drawBox(_x+1, _y+1, bar, _h-2);
	M5.lcd.fillRect(_left+1, _top+1, bar, _height-2, BLACK);

    // Print percentage
    // _disp.setColorIndex(1);
    // _disp.setFont(FONT_NORMAL);
	M5.lcd.setTextSize(1);
    // _disp.setCursor(_x+_w/2, _y+(_h+FONT_NORMAL_H)/2 - 1);
#define FONT_NORMAL_H	5
	M5.lcd.setCursor(_left+_width/2, _top+(_height+FONT_NORMAL_H)/2 - 1);
    // _disp.printf("%d%%", _percent);
	M5.lcd.printf("%d%%", _percent);
};

void do_not_me(const int x, const int y, const int w, const int h, const float percent)
{
	const int y_mid = y+h/2;
	const int x_mid = x+w/2;
	const int bw = abs(percent*w/2);	// Width of bar
	// DBG("Out = %.0f, p = %.2f, bw=%d", pid_get_output(), percent, bw);

	// Clear previous
	M5.Lcd.fillRect(x, y, w, h, BLACK);

	// HZ Centerline with vertical mid-marker
	M5.Lcd.drawLine(x, y_mid, x+w, y_mid, LIGHTGREY);
	M5.Lcd.drawLine(x_mid, y, x_mid, y+h, RED);

	// Filled bar, starting mid going left or right
	if(percent > 0)
		M5.Lcd.fillRect(x_mid+1, y+1, bw, h-2, WHITE);
	else
		M5.Lcd.fillRect(x_mid-bw, y+1, bw, h-2, WHITE);
};


// void draw_roundedrectangle(int x, int y, int w, int h, int r)
// {
// 	// // Clear box
// 	// d.setDrawColor(0);
// 	// d.drawFrame(x-1, y-1, w+1, h+1);
// 	// d.drawBox(x-1, y-1, r+1, r+1);
// 	// d.drawBox(x+w-r, y-1, r+1, r+1);
// 	// d.drawBox(x+w-r, y+h-r, r+1, r+1);
// 	// d.drawBox(x-1, y+h-r, r+1, r+1);
	
// 	// rounded rectangle
// 	d.setDrawColor(1);
// 	// top-left
// 	d.drawCircle(x+r, y+r , r, U8G2_DRAW_UPPER_LEFT);
// 	// top
// 	d.drawHLine(x+r, y, w-2*r);
// 	// top-right
// 	d.drawCircle(x+w-r, y+r, r, U8G2_DRAW_UPPER_RIGHT);
// 	// right
// 	d.drawVLine(x+w, y+r, h-2*r);
// 	// bottom-right
// 	d.drawCircle(x+w-r, y+h-r, r, U8G2_DRAW_LOWER_RIGHT);
// 	// bottom
// 	d.drawHLine(x+r, y+h, w-2*r);
// 	// bottom-left
// 	d.drawCircle(x+r, y+h-r, r, U8G2_DRAW_LOWER_LEFT);
// 	// left
// 	d.drawVLine(x, y+r, h-2*r);
// };
