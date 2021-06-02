#pragma once

#include <stdint.h>

// void draw_roundedrectangle(int x, int y, int w, int h, int r = 5);

class Widget
{
    public:
		Widget() : _top(0), _left(0), _width(0), _height(0) {};
 		Widget(const int top, const int left, const int height, const int width);
   
        virtual void draw() = 0;

        int top() { return _top;};
        int left() { return _left; };        
		int bottom() { return _top+_height; };
		int right() { return _left+_width; };
		void setSize(const int top, const int left, const int height, const int width);

    protected:
        int _top, _left;
        int _width, _height;
};

/*********************************************************************************************/
class ProgressBar : Widget
{
    public:
        ProgressBar(int x, int y, int w, int h);

        void setValue(int p);
        void draw();

    private:
        int _percent = 0;
};
