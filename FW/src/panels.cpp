#include "panels.h"

#include "config.h"
#include "globals.h"
#include "pid.h"

// #include <M5Stack.h>

// void Panel::draw()
// {

// };

void Panel::drawBorder(uint32_t color, uint32_t w)
{
	// while(w--)
	// {
	// 	M5.lcd.drawRect(_left + w, _top + w, _width, _height, color);
	// 	// M5.lcd.drawRect(_left + w, _top + w, _width - 2*(w-1), _height - 2*(w - 1), color);
	// };
};

TempStatPanel::TempStatPanel(int t, int l, int h, int w) : Panel(t, l, h, w)
{
    _bar.setSize(bottom() - 11, left(), 10, width() );
};

void TempStatPanel::draw()
{
	drawBorder(0);

	int x = _left+5;
	int y = _top+5;
	// M5.Lcd.setTextSize(2);
	// M5.Lcd.setTextColor(DARKGREY, BLACK);
	// M5.Lcd.setCursor(x, y);
	// M5.Lcd.print("set: ");
	// M5.Lcd.print(settings.pid1.setpoint, 1);
	// M5.Lcd.print(" C ");
	// lcd.print(Output, 1);

	y += 28;
	x += 10;
	// M5.Lcd.setTextSize(3);
	// M5.Lcd.setTextColor(WHITE, BLACK);
	// M5.Lcd.setCursor(x, y);
	// // M5.Lcd.print("");
	// M5.Lcd.print(pid1.get_input(), 1);
	// M5.Lcd.print(" C ");

    _bar.setValue(pid1.get_output()*100/WINDOWSIZE);
    _bar.draw();
};

HumStatPanel::HumStatPanel(int t, int l, int h, int w) : Panel(t, l, h, w)
{
    _bar.setSize(bottom() - 11, left(), 10, width() );
};

void HumStatPanel::draw()
{
	drawBorder(0);

	int x = _left+5;
	int y = _top+5;
	// M5.Lcd.setTextSize(2);
	// M5.Lcd.setTextColor(WHITE, BLACK);
	// M5.Lcd.setCursor(x, y);
	// M5.Lcd.print("set: ");
	// M5.Lcd.print(settings.pid2.setpoint, 1);
	// M5.Lcd.print(" % ");
	// lcd.print(Output, 1);

	y += 28;
	x += 10;
	// M5.Lcd.setTextSize(3);
	// M5.Lcd.setCursor(x, y);
	// // M5.Lcd.print("");
	// M5.Lcd.print(pid2.get_input(), 1);
	// M5.Lcd.print(" % ");

    _bar.setValue(pid2.get_output()*100/WINDOWSIZE);
    _bar.draw();
};
