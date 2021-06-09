#include "panels.h"

#include "config.h"
#include "globals.h"
#include "pid.h"


// void Panel::draw()
// {

// };

void Panel::drawBorder(uint32_t color, uint32_t w)
{
	while(w--)
	{
		M5.lcd.drawRect(_left + w, _top + w, _width, _height, color);
		// M5.lcd.drawRect(_left + w, _top + w, _width - 2*(w-1), _height - 2*(w - 1), color);
	};
};

TempStatPanel::TempStatPanel(int t, int l, int h, int w) : Panel(t, l, h, w)
{

};

void TempStatPanel::draw()
{
	drawBorder();

	int x = _left+5;
	int y = _top+5;
	M5.Lcd.setTextSize(2);
	M5.Lcd.setTextColor(WHITE, BLACK);
	M5.Lcd.setCursor(x, y);
	M5.Lcd.print("set: ");
	M5.Lcd.print(settings.pid1.setpoint, 1);
	M5.Lcd.print(" C ");
	// lcd.print(Output, 1);

	y += 30;
	x += 10;
	M5.Lcd.setTextSize(3);
	M5.Lcd.setCursor(x, y);
	// M5.Lcd.print("");
	M5.Lcd.print(g_MeasT, 1);
	M5.Lcd.print(" C ");

	// float outpercent = -1 + pid_get_output()/WINDOWSIZE*2;
	// const int bottom = _top+_height;
	// const int bar_height = 12;
	// draw_outputbar(left, bottom() - bar_height, width(), bar_height, outpercent);

};

HumStatPanel::HumStatPanel(int t, int l, int h, int w) : Panel(t, l, h, w)
{

};

void HumStatPanel::draw()
{
	drawBorder();

	int x = _left+5;
	int y = _top+5;
	M5.Lcd.setTextSize(2);
	M5.Lcd.setTextColor(WHITE, BLACK);
	M5.Lcd.setCursor(x, y);
	M5.Lcd.print("set: ");
	M5.Lcd.print(settings.pid2.setpoint, 1);
	M5.Lcd.print(" % ");
	// lcd.print(Output, 1);

	y += 30;
	x += 10;
	M5.Lcd.setTextSize(3);
	M5.Lcd.setCursor(x, y);
	// M5.Lcd.print("");
	M5.Lcd.print(g_MeasRH, 1);
	M5.Lcd.print(" % ");

	// float outpercent = -1 + pid_get_output()/WINDOWSIZE*2;
	// int bottom = top+height;
	// int bar_height = 12;
	// // draw_outputbar(left, bottom 
};
