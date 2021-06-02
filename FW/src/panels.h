#pragma once

#include <M5Stack.h>

#include "widgets.h"


class Panel : public Widget
{
	public:
		Panel(const int top, const int left, const int height, const int width) : Widget(top, left, height, width) {};
		// ~Panel();

		// void draw();
		void drawBorder(uint32_t color = WHITE, uint32_t width = 1);
};

class TempStatPanel : public Panel
{
	public:
		TempStatPanel(const int t, const int l, const int h, const int w);

		void draw();
};

class HumStatPanel : public Panel
{
	public:
		HumStatPanel(const int t, const int l, const int h, const int w);

		void draw();
};
