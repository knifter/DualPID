#ifndef __GUI_H
#define __GUI_H

#include <Arduino.h>

#define KSETPOINT  0
#define Kp  1
#define Ki  2
#define Kd  3
#define KEY_SLOW 				500
#define KEY_FAST 				100
#define KEY_VERY_FAST 			50
#define KEY_STEP_SLOW 			0.1
#define KEY_STEP_FAST 			0.5

typedef enum
{
	SELECT_P,
	CHANGE_P,
	MEASURE,
} gui_state_t;

class GUI
{
	public:
		// GUI() {};

		bool begin();
		void loop();

		void draw_help();
		void draw_menu(int it);
		void draw_main();
		void draw_menuitem(String txt, int place, int selected);
		void parameter_change();
		void select_parameter();
		void highlight_param(int item);

	private:
		gui_state_t _state = MEASURE;
		int Selected_Parameter = KSETPOINT;

		// Memorize place of the numbers to changes
		uint16_t Para_Cusor_X[5];
		uint16_t Para_Cusor_Y[5];
		
		// P I D parameters
		double para[5] = {50.0, 2., 5., 1., -1.0};
		// Menuitems
		String item[5] = {"S", "P", "I", "D", "X"};
};

#endif // __GUI_H