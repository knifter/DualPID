#ifndef __SCREENS_H
#define __SCREENS_H

#include <stdlib.h>
#include <stdint.h>

#include "tools-nocopy.h"

#include "event.h"
#include "gui.h"

enum class ScreenType
{
	BASE,
	BOOT,
	MAIN,
	MENU,
	MESSAGE,

	// POWEROFF,
	// BATTLOW,
	// MESSAGE,
	// OTA,
	_NUM
};

constexpr const char* ScreenNames[static_cast<int>(ScreenType::_NUM)] = 
{
	"BASE",
	"BOOT",
	"MAIN",
	"MENU",
	"MESSAGE"

	// "SCREENSAVE",
	// "POWEROFF",
	// "MESSAGE",
	// "OTA"
};

class Screen : NonCopyable
{
    public:
        Screen();
		virtual ~Screen();
		virtual ScreenType type() { return ScreenType::BASE; };

		void close();
        virtual void draw() = 0;
        virtual bool handle(event_t) = 0;
		virtual const char* name() { return ScreenNames[(int) type()]; };
		// virtual const char* title() { return ScreenTitles[(int) type()]; };

    protected:
		GUI& _gui;
};

/*** BOOT ************************************************************************************/
class BootScreen : public Screen
{
    public:
		BootScreen();
		virtual ScreenType type() { return ScreenType::BOOT; };

        void draw();
        bool handle(event_t);

    private:
        uint32_t _start = 0;
};

/*** MAIN ************************************************************************************/
class MainScreen : public Screen
{
    public:
		MainScreen() :
			_t_panel(0, 0, 70, SCREEN_WIDTH/2), 
			_rh_panel(0, SCREEN_WIDTH/2, 70, SCREEN_WIDTH/2)
			{};

		virtual ScreenType type() { return ScreenType::MAIN; };

        void draw();
        bool handle(event_t);
	
	private:
		TempStatPanel _t_panel;
		HumStatPanel _rh_panel;

};

/*** MESSAGE ************************************************************************************/
class MessageScreen : public Screen
{
    public:
        MessageScreen(const char* title, const char* line1 = nullptr, const char* line2 = nullptr, const char* line3 = nullptr, const char* line_btn = "[ Ok ]");
		virtual ScreenType type() { return ScreenType::MESSAGE; };

        void draw();
        bool handle(event_t);

    private:
		char _title[10];
		char _line1[24];
		char _line2[24];
		char _line3[24];
		char _line_btn[24];
};

/*** Menu ************************************************************************************/
class MenuScreen : public Screen
{
    public:
		virtual ScreenType type() { return ScreenType::MENU; };

        void draw();
        bool handle(event_t);
		void draw_menu();
		void draw_menuitem(int item);
		void draw_highlight_param();
		void parameter_change();
		void select_parameter();

	private:
		typedef enum
		{
			MAIN,
			CHANGE_PARAM,
			SELECT_PARAM
		} state_t;
		state_t _state = state_t::MAIN;

		// typedef enum : int
		// {
		// 	PARAM_NONE,
		// 	PARAM_SETPOINT, 
		// 	PARAM_KP,
		// 	PARAM_KI,
		// 	PARAM_KD,
		// 	PARAM_BACK,
		// 	_PARAM_MAX
		// } param_t;

		const char* ParamNames[_PARAM_MAX+1] = {
			"S",
			"P",
			"I",
			"D",
			"<Back"
		};	

		param_t _selected_parameter = PARAM_SETPOINT;
		double* _settingptr = nullptr;
		time_t _holdoff = 0;

		// Memorize place of the numbers to changes
		uint16_t Para_Cusor_X[5];
		uint16_t Para_Cusor_Y[5];
		
		// P I D parameters
		// double para[5] = {50.0, 2., 5., 1., -1.0};
		// Menuitems


};

#endif // __SCREENS_H