#include "screens.h"

#include <memory>

#include "gui.h"
#include "widgets.h"

#include "config.h"
#ifdef DEBUG_ACTIVITIES_CPP
	#define TOOLS_LOG_DEBUG
#endif
#include "tools-log.h"
#include "globals.h"

Screen::Screen()
{
	DBG("CONSTRUCT %s(%p)", this->name(), this);
    _screen = lv_obj_create(NULL);
};

Screen::~Screen() 
{ 
	DBG("DESTROY %s(%p)", this->name(), this); 
    lv_obj_del(_screen);
    _screen = nullptr;
};

void Screen::load()
{
    lv_scr_load(_screen);
};

void Screen::close() 
{ 
    gui.popScreen(this);
};

/*** BOOT ************************************************************************************/
BootScreen::BootScreen() : Screen()
{
    _start = millis();

	_label = lv_label_create(_screen);
    lv_obj_set_size(_label, DISPLAY_WIDTH, 50);
	lv_obj_center(_label);
    lv_obj_set_style_text_color(_label, lv_palette_main(LV_PALETTE_RED), LV_PART_ANY);
    lv_obj_set_style_text_align(_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(_label, "M5DualPID v%d", VERSION);
};

bool BootScreen::loop()
{
    uint32_t now = millis();
    if((now - _start) > BOOTSCREEN_TIMEOUT_MS)
    {
        gui.pushScreen(ScreenType::MAIN);
    };
	return false;
};

/*** MAIN ************************************************************************************/
MainScreen::MainScreen()
{
    _box_pid1 = lv_obj_create(_screen);
    lv_obj_set_pos(_box_pid1, 0, 0);
    lv_obj_set_size(_box_pid1, DISPLAY_WIDTH/2, 100);

    _box_pid2 = lv_obj_create(_screen);

	// M5.Lcd.setTextSize(2);
	// M5.Lcd.setTextColor(WHITE, BLACK);


	// t_panel.setSize(0, 0, SCREEN_HEIGHT/2, SCREEN_WIDTH/2);
	// rh_panel.setSize(0, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2);
	// _t_panel.draw();
	// _rh_panel.draw();
};

bool MainScreen::loop()
{
    return false;
};

/*** MESSAGE ************************************************************************************/
MessageScreen::MessageScreen(const char* title, const char* line1, const char* line2, const char* line3, const char* line_btn)
{
	_title[0] = '\0';
	_line1[0] = '\0';
	_line2[0] = '\0';
	_line3[0] = '\0';
	_line_btn[0] = '\0';
	if(title)
		strncpy(_title, title, sizeof(_title)-1);
	if(line1 != nullptr)
		strncpy(_line1, line1, sizeof(_line1)-1); 
	if(line2 != nullptr)
		strncpy(_line2, line2, sizeof(_line2)-1); 
	if(line3 != nullptr)
		strncpy(_line3, line3, sizeof(_line3)-1);
	if(line_btn != nullptr)
		strncpy(_line_btn, line_btn, sizeof(_line_btn)-1);
	_title[sizeof(_title) - 1] = '\0';
	_line1[sizeof(_line1) - 1] = '\0';
	_line2[sizeof(_line2) - 1] = '\0';
	_line3[sizeof(_line3) - 1] = '\0';
	_line_btn[sizeof(_line_btn) - 1] = '\0';
};

// void MessageScreen::draw()
// {
// 	const int x = 10, y = 8, w = DISPLAY_WIDTH-20, h=DISPLAY_HEIGHT-10;

	// Draw bounding box.
	// _disp.setDrawColor(0);
	// _disp.drawBox(x-1, y-1, w+2, h+2);
	// draw_roundedrectangle(_disp, x, y, w, h, 5);
	// M5.Lcd.drawRect(x-1, y-1, w+2, h+2, WHITE);
	// M5.Lcd.drawRoundRect(x, y, w, h, 5, BLACK);
	

	// Text in box
	// _disp.setDrawColor(1);
	// int ty = y + 3;
	// if(_title[0])
	// {
	// 	ty+=FONT_MSGHEAD_H;
		// _disp.setFont(FONT_MSGHEAD);
		// _disp.drawStr(x+5, ty, _title);
	// };
	// _disp.setFont(FONT_NORMAL);
	// ty+=1;
	// if(_line1[0])
	// {
	// 	ty+=FONT_NORMAL_H + 1;
	// 	_disp.drawStr(x+3, ty, _line1);
	// };
	// if(_line2[0])
	// {
	// 	ty+=FONT_NORMAL_H + 1;
	// 	_disp.drawStr(x+3, ty, _line2);
	// };
	// if(_line3[0])
	// {
	// 	ty+=FONT_NORMAL_H + 1;
	// 	_disp.drawStr(x+3, ty, _line3);
	// };
	// if(_line_btn[0])
	// {
	// 	_disp.drawHLine(x, y + h - 10, w);
	// 	_disp.drawStr(
	// 		x+w/2 - _disp.getStrWidth(_line_btn)/2, y+h-2,
	// 		_line_btn);
	// };
// };

bool MessageScreen::loop()
{
	// switch(event)
	// {
	// 	case KEY_A:
	// 	case KEY_B:
	// 	case KEY_C:
	// 		gui.popScreen();
	// 		return true;
	// 	default:
	// 		return false;
	// };
	return false;
};

/*** MENU ************************************************************************************/
param_t& operator++(param_t& orig)
{
    if(orig < _PARAM_MAX)
        orig = static_cast<param_t>(orig + 1);
    else
        orig = static_cast<param_t>(0);
    return orig;
};
param_t& operator--(param_t& orig)
{
    if(orig == 0)
        orig = static_cast<param_t>(_PARAM_MAX - 1);
    else
        orig = static_cast<param_t>(orig - 1);
    return orig;
};

// void MenuScreen::draw()
// {
// 	switch(_state)
// 	{
// 		case state_t::MAIN:
// 			// if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed())
// 			// {
// 			// 	_state = state_t::SELECT_PARAM;
// 			// 	draw_menu();
// 			// 	return;
// 			// };
// 			break;

// 		case state_t::CHANGE_PARAM:
// 			parameter_change();
// 			break;

// 		case state_t::SELECT_PARAM:
// 			select_parameter();
// 			break;
// 	}; //switch
// };

// show every menu item with current values
void MenuScreen::draw_menu()
{ 
	// M5.Lcd.fillScreen(BLACK);

	// Menu list
  	// M5.Lcd.setTextSize(3);

  	for (int i = 0; i < _PARAM_MAX; i++)
    	draw_menuitem(i);

	// Bottom help
	// M5.Lcd.setTextColor(WHITE, BLUE);
	// M5.Lcd.setCursor(40, 225);
	// M5.Lcd.setTextSize(2);
	// M5.Lcd.print("<<<    Select    >>>");
};

bool MenuScreen::handle(event_t event)
{
	parameter_change();
    // switch(event)
    // {
    //     case KEY_A: 
    //     case KEY_B: 
    //     case KEY_C: 
	// 		break;
	// 	case KEY_P:
	// 	default: return false; break;
    // };
	return true;
};

// Current parameter to change
void MenuScreen::draw_menuitem(int item)
{
	// M5.Lcd.setTextSize(3);

	int y = item * 40 + 20;
	const char* txt = ParamNames[item];

	// Selected item gets a different color
	// M5.Lcd.setCursor (10, y);
	// if ( _selected_parameter == item )
	// 	M5.Lcd.setTextColor(BLUE, WHITE);
	// else
	// 	M5.Lcd.setTextColor(WHITE, BLACK);

	// Print item
	// M5.Lcd.print(txt);
	if(item == PARAM_BACK)
		return;

	// draw value
	// M5.Lcd.setTextSize(3);
	// M5.Lcd.print(" = ");
	// Para_Cusor_X[item] = M5.Lcd.getCursorX();
	// Para_Cusor_Y[item] = M5.Lcd.getCursorY();
	double value;
	switch(item)
	{
		case PARAM_SETPOINT:value = settings.pid2.setpoint; break;
		case PARAM_KP: 		value = settings.pid2.Kp; break;
		case PARAM_KI: 		value = settings.pid2.Ki; break;
		case PARAM_KD: 		value = settings.pid2.Kd; break;
		default: 			value= NAN; break;
	};
	// M5.Lcd.print(value);
	// M5.Lcd.print("      ");
};

// Parameter to change will be red colored
void MenuScreen::draw_highlight_param()
{
	int item = _selected_parameter;
	// M5.Lcd.setTextSize(3);
	// M5.Lcd.setCursor(Para_Cusor_X[item], Para_Cusor_Y[item]);
	// M5.Lcd.setTextColor(RED, WHITE);
	// M5.Lcd.print(*_settingptr);
};

void MenuScreen::parameter_change()
{
	// Acceleration logic
	static float key_step = KEY_STEP_SLOW;
	static int key_delay = KEY_SLOW;
	// if (M5.BtnA.pressedFor(5000) or M5.BtnC.pressedFor(5000))
	// {
	// 	key_delay = KEY_VERY_FAST;
	// 	key_step = KEY_STEP_FAST;
	// }
  	// else if (M5.BtnA.pressedFor(2000) or M5.BtnC.pressedFor(2000))
  	// {
	//     key_delay = KEY_FAST;
  	// }
	// else
  	{
    	key_delay = KEY_SLOW;
    	key_step = KEY_STEP_SLOW;
  	};

	// Action
  	// if (M5.BtnA.isPressed())
  	// {
	// 	*_settingptr -= key_step;
	// 	draw_highlight_param();
	// 	delay(key_delay);
  	// };
  	// if (M5.BtnB.wasPressed())
  	// {
    // 	_state = state_t::SELECT_PARAM;
    // 	M5.Lcd.fillScreen(BLACK);
    // 	draw();
  	// };
  	// if (M5.BtnC.isPressed())
	// {	
	// 	*_settingptr += key_step;
	// 	draw_highlight_param();
	// 	delay(key_delay);
  	// };
};

void MenuScreen::select_parameter()
{
	pidsettings_t& ps = setman.settings.pid2;

  	// if (M5.BtnA.wasPressed())
  	// {
	// 	--_selected_parameter;
	// 	// if(_selected_parameter == PARAM_NONE)
	// 	// 	_selected_parameter = (_PARAM_MAX - 1;
	// 	draw_menu();
	// 	return;
  	// };
  	// if (M5.BtnB.wasPressed())
  	// {
	// 	if(_selected_parameter == PARAM_BACK)
	// 	{
	// 		// TODO: move to state machine
	// 		// settings.write_flash(para);
	// 		setman.saveDelayed();
	// 		pid2.set_tuning(ps);

	// 		M5.Lcd.fillScreen(BLACK);
	// 		_state = state_t::MAIN;
	// 		return;
	// 	};
	// 	_state = state_t::CHANGE_PARAM;
	// 	switch(_selected_parameter)
	// 	{
	// 		case PARAM_SETPOINT:	_settingptr = &ps.setpoint; break;
	// 		case PARAM_KP: 			_settingptr = &ps.Kp; break;
	// 		case PARAM_KI: 			_settingptr = &ps.Ki; break;
	// 		case PARAM_KD: 			_settingptr = &ps.Kd; break;
	// 		default: 				_settingptr = nullptr; break;
	// 	};
	// 	draw_highlight_param();
	// 	return;
	// };
	// if (M5.BtnC.wasPressed())
	// {
	// 	++_selected_parameter;
	// 	// if(_selected_parameter == _PARAM_MAX)
	// 	// 	_selected_parameter = 1;
	// 	draw_menu();
	// 	return;
	// };
};
