#include "screens.h"

#include <memory>
#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "tools-log.h"
#include "globals.h"

// C-style callbacks

/*** MAIN ************************************************************************************/
class PidWidget
{
	public:
		PidWidget(lv_obj_t* parent, const char* unit);
		lv_obj_t	*box, *lbl_sp, *lbl_value, *bar_output;
		lv_style_t 	style_font26;
		String unit;
		void setSetPoint(float sp);
		void setValue(float v);
		void setBar(float p);   
};

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
void PidWidget::setBar(float p) {     	lv_bar_set_value(bar_output, p, LV_ANIM_ON); };

MainScreen::MainScreen(SooghGUI& g) : Screen(g)
{
	// pw1 = new PidWidget(_screen, "\xe2\x84\x83");
	pw1 = new PidWidget(_screen, "\xc2\xb0""C");
	pw2 = new PidWidget(_screen, "%RH");
	lv_obj_align_to(pw2->box, pw1->box, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	/*Create a chart*/	
    chart = lv_chart_create(_screen);
    lv_obj_set_size(chart, DISPLAY_WIDTH - 40, DISPLAY_HEIGHT - 80 - 20);	
	lv_obj_align(chart, LV_ALIGN_TOP_MID, 0, 78);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_div_line_count(chart, 5, 7);
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);

	// Temp
	lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -10, 30);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 1, 0, 3, 1, true, 20);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 1, 0, 7, 1, true, 20);

	// RH
	lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, 0, 100);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_SECONDARY_Y, 1, 0, 3, 1, true, 20);
	lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT );

    /*Add two data series*/
#define HISTORY_POINTS		120
#define HISTORY_DELTA_MS	60E3
	lv_chart_set_point_count(chart, HISTORY_POINTS);
    ser1 = lv_chart_add_series(chart, COLOR_RED, LV_CHART_AXIS_PRIMARY_Y);
    ser2 = lv_chart_add_series(chart, COLOR_BLUE, LV_CHART_AXIS_SECONDARY_Y);
	
	// pre-fill the chart
	float v1 = pid1.get_input();
	float v2 = pid2.get_input();
	int p = HISTORY_POINTS;
	while(p--)
	{
	    lv_chart_set_next_value(chart, ser1, v1);
	    lv_chart_set_next_value(chart, ser2, v2);
	};
};

bool MainScreen::loop()
{
	float i1 = pid1.get_input();
	float i2 = pid2.get_input();
	pw1->setSetPoint(settings.pid1.setpoint);
	pw1->setValue(i1);
	pw1->setBar(pid1.get_output()*100/PID_WINDOWSIZE);

	pw2->setSetPoint(settings.pid2.setpoint);
	pw2->setValue(i2);
	pw2->setBar(pid2.get_output()*100/PID_WINDOWSIZE);

	static time_t next_chart = 0;
	time_t now = millis();
	if(now > next_chart)
	{
	    lv_chart_set_next_value(chart, ser1, i1);
	    lv_chart_set_next_value(chart, ser2, i2);

		next_chart = now + HISTORY_DELTA_MS;
	};
    
    return false;
};

bool MainScreen::handle(soogh_event_t key)
{
	switch(key)
	{
		case KEY_A_SHORT:
			
			break;
		case KEY_B_SHORT:
			break;
		case KEY_C_SHORT:
			break;
		case KEY_B_LONG:
		{
			ScreenPtr scr = std::make_shared<MenuScreen>(_gui);
	        _gui.pushScreen(scr);
			return true;
		};
		default: break;
	};
	return false;
};
