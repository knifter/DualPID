#include "screens.h"

#include <memory>
#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "tools-log.h"
#include "globals.h"

#include <treemenu.h>

// C-style callbacks

/*********************************************************************************************************************************/
class PidPanel
{
	public:
		typedef enum{
			PS_DISABLED,
			PS_OK,
		} state_t;

		PidPanel(lv_obj_t* parent, const char* unit);
		void selected(bool);

		lv_obj_t	*box, *lbl_sp, *lbl_value, *bar_output;
		lv_style_t 	style_font26;
		String unit;
		lv_style_t style_indic;

		void setSetPoint(float sp);
		void setValue(float v);
		void setBar(float p);   
		void setState(state_t s);
};

PidPanel::PidPanel(lv_obj_t* parent, const char* unit_in)
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
		
    	lv_style_init(&style_indic);
    	lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    	lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_HOR);

		lv_obj_add_style(bar_output, &style_indic, LV_PART_INDICATOR);

	};
}; // PidPanel()

void PidPanel::setState(PidPanel::state_t state)
{
	switch(state)
	{
		case PS_DISABLED:
			lv_obj_set_style_bg_color(box, COLOR_GREY_LIGHT1, 0);

			lv_obj_set_style_bg_color(bar_output, COLOR_GREY, 0);
			lv_style_set_bg_color(&style_indic, COLOR_GREY);
			lv_style_set_bg_grad_color(&style_indic, COLOR_GREY);
			
			break;
		case PS_OK:
			lv_obj_set_style_bg_color(box, COLOR_GREEN_LIGHT2, 0);

			lv_obj_set_style_bg_color(bar_output, COLOR_LIGHT_BLUE, 0);
			lv_style_set_bg_color(&style_indic, COLOR_BLUE);
			lv_style_set_bg_grad_color(&style_indic, COLOR_RED);
			break;
	};
};
void PidPanel::setSetPoint(float sp) { lv_label_set_text_fmt(lbl_sp, "sp = %0.01f %s", sp, unit.c_str()); };
void PidPanel::setValue(float v) { 	lv_label_set_text_fmt(lbl_value, "%0.01f %s", v, unit.c_str()); };
void PidPanel::setBar(float p) {     	lv_bar_set_value(bar_output, p, LV_ANIM_ON); };
void PidPanel::selected(bool select)
{
	if(select)
	{
		lv_obj_set_style_border_width(box, 4, 0);
		lv_obj_set_style_border_color(box, COLOR_BLACK, 0);

	}else{
		lv_obj_set_style_border_width(box, 2, 0);
		lv_obj_set_style_border_color(box, COLOR_GREY, 0);
	};
};

/*********************************************************************************************************************************/
class GraphPanel
{
	public:
		GraphPanel(lv_obj_t* parent);

		void appendVals(const float val1, const float val2);
		void setScaleY(const lv_chart_axis_t, float min, float max);
		void autoScale(const lv_chart_axis_t axis, const float inc1, const float inc2);

		lv_obj_t *box;
	    lv_obj_t *chart;
		lv_chart_series_t *ser1, *ser2;
};

GraphPanel::GraphPanel(lv_obj_t* parent)
{
	/*Create a chart*/	
	box = lv_obj_create(parent);
	lv_obj_set_size(box, DISPLAY_WIDTH/2, 80);
	lv_obj_set_style_border_width(box, 2, 0);
	lv_obj_set_style_pad_all(box, 0, 0);
	lv_obj_set_size(box, DISPLAY_WIDTH, DISPLAY_HEIGHT - 78);

    chart = lv_chart_create(box);
    lv_obj_set_size(chart, DISPLAY_WIDTH - 50, DISPLAY_HEIGHT - 80 - 20);
	lv_obj_align(chart, LV_ALIGN_TOP_MID, 0, 0);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_div_line_count(chart, 5, 7);
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);

    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 1, 0, 7, 1, true, 20);
	lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT );

	// Primary = Temp, secondary = Rh
	setScaleY(LV_CHART_AXIS_PRIMARY_Y, -10, 30);
	setScaleY(LV_CHART_AXIS_SECONDARY_Y, 0, 100);

    /*Add two data series*/
	lv_chart_set_point_count(chart, HISTORY_GRAPH_POINTS);
    ser1 = lv_chart_add_series(chart, COLOR_RED, LV_CHART_AXIS_PRIMARY_Y);
    ser2 = lv_chart_add_series(chart, COLOR_BLUE, LV_CHART_AXIS_SECONDARY_Y);
	
	// pre-fill the chart
	float v1 = pid1.get_input();
	float v2 = pid2.get_input();
	int p = HISTORY_GRAPH_POINTS;
	while(p--)
	{
	    lv_chart_set_next_value(chart, ser1, v1);
	    lv_chart_set_next_value(chart, ser2, v2);
	};
};

void GraphPanel::appendVals(const float val1, const float val2)
{
	lv_chart_set_next_value(chart, ser1, val1);
	lv_chart_set_next_value(chart, ser2, val2);
};

void GraphPanel::setScaleY(const lv_chart_axis_t axis, const float miny, const float maxy)
{
	// DBG("axis: %d min/max: %f %f", axis, miny, maxy);

	lv_chart_set_range(chart, axis, miny, maxy);
	// void lv_chart_set_axis_tick(obj, axis, major_len, minor_len, major_cnt, minor_cnt, label_en, draw_size)
    lv_chart_set_axis_tick(		chart,  axis, 1,         0,         3,         1,         true,     20);
};

void GraphPanel::autoScale(const lv_chart_axis_t axis, const float inc1, const float inc2)
{
	float miny = min(inc1, inc2);
	float maxy = max(inc1, inc2);

	// find min/max
	int cnt = lv_chart_get_point_count(chart);
	lv_chart_series_t *ser = axis == LV_CHART_AXIS_PRIMARY_Y ? ser1 : ser2;
	while(--cnt)
	{
		miny = min(miny, static_cast<float>(ser->y_points[cnt]));
		maxy = max(maxy, static_cast<float>(ser->y_points[cnt]));
	};

	miny = floor(miny/5)*5;
	maxy = ceil(maxy/5)*5;

	setScaleY(axis, miny - 5, maxy + 5);
};

/*********************************************************************************************************************************/
MainScreen::MainScreen(SooghGUI& g) : Screen(g)
{
	// pw1 = new PidPanel(_screen, "\xe2\x84\x83");
	pw1 = new PidPanel(_screen, "\xc2\xb0""C");
	pw2 = new PidPanel(_screen, "%RH");
	lv_obj_align_to(pw2->box, pw1->box, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	gw = new GraphPanel(_screen);
	lv_obj_align(gw->box, LV_ALIGN_BOTTOM_MID, 0, 0);
};

bool MainScreen::loop()
{
	time_t now = millis();
	if(now < _next_update)
		return true;

	_next_update = now + MAIN_LOOP_MS;

	float i1 = pid1.get_input();
	float i2 = pid2.get_input();
	pw1->setSetPoint(settings.pid1.fpid.setpoint);
	pw1->setValue(i1);
	pw1->setBar(pid1.get_output_percent());
	if(settings.pid1.active)
	{
		pw1->setState(PidPanel::PS_OK);
	}else{
		pw1->setState(PidPanel::PS_DISABLED);
	};

	pw2->setSetPoint(settings.pid2.fpid.setpoint);
	pw2->setValue(i2);
	pw2->setBar(pid2.get_output_percent());
	if(settings.pid2.active)
	{
		pw2->setState(PidPanel::PS_OK);
	}else{
		pw2->setState(PidPanel::PS_DISABLED);
	};

	// autoscale
	gw->autoScale(LV_CHART_AXIS_PRIMARY_Y, pid1.get_input(), settings.pid1.fpid.setpoint);
	gw->autoScale(LV_CHART_AXIS_SECONDARY_Y, pid2.get_input(), settings.pid2.fpid.setpoint);

	if(now < _next_chart)
		return true;

	gw->appendVals(i1, i2);
	_next_chart = now + HISTORY_GRAPH_DELTA_MS;
    
    return false;
};

bool MainScreen::handle(soogh_event_t key)
{
	switch(key)
	{
		case KEY_A_SHORT:
			pw1->selected(true);
			pw2->selected(false);
			break;
		case KEY_B_SHORT:
		    gui.pushScreen(std::make_shared<MenuScreen>(gui));
			return true;
		case KEY_C_SHORT:
			pw1->selected(false);
			pw2->selected(true);
			break;
		case KEY_B_LONG:
		default: break;
	};
	return true;
};
