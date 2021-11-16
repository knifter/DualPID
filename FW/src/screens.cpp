#include "screens.h"

#include <memory>

#include "gui.h"

#include "config.h"
#ifdef DEBUG_ACTIVITIES_CPP
	#define TOOLS_LOG_DEBUG
#endif
#include "tools-log.h"
#include "globals.h"
#include "soogh-color.h"


Screen::Screen()
{
	// DBG("CONSTRUCT %s(%p)", this->name(), this);
    _screen = lv_obj_create(NULL);
};

Screen::~Screen() 
{ 
	// DBG("DESTROY %s(%p)", this->name(), this); 
    lv_obj_del(_screen); _screen = nullptr;
};

void Screen::load()
{
    // lv_scr_load(_screen);
    lv_scr_load_anim(_screen, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, false);
};

void Screen::close() 
{ 
    gui.popScreen(this);
};

/*** BOOT ************************************************************************************/
BootScreen::BootScreen() : Screen()
{
    _start = millis();

	lv_obj_t* l = lv_label_create(_screen);
    lv_obj_set_size(l, DISPLAY_WIDTH, 50);
	lv_obj_center(l);
    lv_obj_set_style_text_color(l, COLOR_RED, LV_PART_ANY);
    lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(l, "M5DualPID v%d", VERSION);
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
PidWidget::PidWidget(lv_obj_t* parent, const char* unit_in)
{
	unit = unit_in;
	box = lv_obj_create(parent);
	lv_obj_set_size(box, DISPLAY_WIDTH/2, 80);
	lv_obj_set_style_border_width(box, 2, 0);
	lv_obj_set_style_pad_all(box, 5, 0);

	lv_style_init(&style_font26);
	lv_style_set_text_font(&style_font26, &lv_font_montserrat_26);

	// Setpoint label
	{
		lbl_sp = lv_label_create(box);
		lv_obj_set_style_border_width(lbl_sp, 1, 0);
		lv_obj_set_size(lbl_sp, LV_PCT(100), 20);
		lv_obj_align(lbl_sp, LV_ALIGN_TOP_MID, 0, 0);
		lv_obj_set_style_text_color(lbl_sp, COLOR_GREY, LV_PART_ANY);
		lv_obj_set_style_text_align(lbl_sp, LV_TEXT_ALIGN_CENTER, 0);
		lv_label_set_text(lbl_sp, "<setpoint>");
	};
	// Value label
	{
		lbl_value = lv_label_create(box);
		lv_obj_set_style_border_width(lbl_value, 1, 0);
		lv_obj_set_size(lbl_value, LV_PCT(100), 30);
		lv_obj_align_to(lbl_value, lbl_sp, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
		lv_obj_add_style(lbl_value, &style_font26, 0);
		lv_obj_set_style_text_color(lbl_value, COLOR_BLACK, LV_PART_ANY);
		lv_obj_set_style_text_align(lbl_value, LV_TEXT_ALIGN_CENTER, 0);
		lv_label_set_text(lbl_value, "<value>");
	};

	// Output bar
	{
		bar_output = lv_bar_create(box);
		lv_obj_set_style_border_width(bar_output, 1, 0);
		lv_obj_set_size(bar_output, LV_PCT(100), 10);
		lv_obj_align(bar_output, LV_ALIGN_BOTTOM_MID, 0, 0);
		lv_bar_set_range(bar_output, 0, 100);
	};
}; // PidWidget()

void PidWidget::setSetPoint(float sp) { lv_label_set_text_fmt(lbl_sp, "sp = %0.01f %s", sp, unit.c_str()); };
void PidWidget::setValue(float v) { 	lv_label_set_text_fmt(lbl_value, "%0.01f %s", v, unit.c_str()); };
void PidWidget::setBar(float p) {     lv_bar_set_value(bar_output, p, LV_ANIM_ON); };

MainScreen::MainScreen()
{
	// pw1 = new PidWidget(_screen, "\xe2\x84\x83");
	pw1 = new PidWidget(_screen, "\xc2\xb0""C");
	pw2 = new PidWidget(_screen, "%RH");
	lv_obj_align_to(pw2->box, pw1->box, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
};

bool MainScreen::loop()
{
	pw1->setSetPoint(settings.pid1.setpoint);
	pw1->setValue(pid1.get_input());
	pw1->setBar(pid1.get_output()*100/PID_WINDOWSIZE);

	pw2->setSetPoint(settings.pid2.setpoint);
	pw2->setValue(pid2.get_input());
	pw2->setBar(pid2.get_output()*100/PID_WINDOWSIZE);
    
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
