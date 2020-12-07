#ifndef __GUI_H
#define __GUI_H

#include <Arduino.h>

#define KEY_SLOW 				500
#define KEY_FAST 				100
#define KEY_VERY_FAST 			50
#define KEY_STEP_SLOW 			0.1
#define KEY_STEP_FAST 			0.5

// typedef enum
// {
// 	SELECT_P,
// 	CHANGE_P,
// 	MEASURE,
// } gui_state_t;

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
		// GUI() {};

		bool begin();
		void loop();

		void draw_boot();
		void draw_help();
		void draw_menu();
		void draw_main();
		void draw_menuitem(int place);
		void parameter_change();
		void select_parameter();
		void draw_highlight_param();
		void draw_outputbar(const int x, const int y, const int w, const int h, const float percent);

	private:
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

		typedef enum
		{
			BOOT,
			BOOT_WAIT,
			MAIN,
			CHANGE_PARAM,
			SELECT_PARAM
		} state_t;

		param_t _selected_parameter = PARAM_SETPOINT;
		double* _settingptr = nullptr;
		time_t _holdoff = 0;
		state_t _state = state_t::BOOT;
		// int Selected_Parameter = KSETPOINT;

		// Memorize place of the numbers to changes
		uint16_t Para_Cusor_X[5];
		uint16_t Para_Cusor_Y[5];
		
		// P I D parameters
		// double para[5] = {50.0, 2., 5., 1., -1.0};
		// Menuitems
};

#endif // __GUI_H