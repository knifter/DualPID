#include "screens.h"

#include <memory>
#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "tools-log.h"
#include "globals.h"

#include <treemenu.h>

#define GRAPH_TOTAL_SEC         ((int)(GRAPH_POINTS * GRAPH_DELTA_MS / 1000))
#define GRAPH_SEC_PER_DEV       (GRAPH_TOTAL_SEC / (GRAPH_XTICKS-1))

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
	lv_obj_set_style_border_width(box, 3, 0);
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
		lv_obj_set_style_bg_opa(lbl_sp, LV_OPA_COVER, 0);
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
		// lv_obj_set_style_bg_opa(lbl_value, LV_OPA_COVER, 0);
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
		lv_obj_set_style_bg_opa(bar_output, LV_OPA_COVER, 0);
	};
}; // PidPanel()

void PidPanel::setState(PidPanel::state_t state)
{
	switch(state)
	{
		case PS_DISABLED:
			lv_obj_set_style_border_color(box, COLOR_WHITE, 0);
			lv_obj_set_style_bg_color(box, COLOR_WHITE, 0);

			lv_obj_set_style_bg_color(lbl_sp, COLOR_GREY, 0);
			// lv_obj_set_style_bg_color(lbl_value, COLOR_WHITE, 0);

			lv_obj_set_style_bg_color(bar_output, COLOR_GREY, 0);
			lv_style_set_bg_color(&style_indic, COLOR_GREY);
			lv_style_set_bg_grad_color(&style_indic, COLOR_GREY);
			break;
		case PS_OK:
			lv_obj_set_style_border_color(box, COLOR_WHITE, 0);
			lv_obj_set_style_bg_color(box, COLOR_WHITE, 0);

			lv_obj_set_style_bg_color(lbl_sp, COLOR_GREEN_LIGHT(2), 0);
			// lv_obj_set_style_bg_color(lbl_value, COLOR_WHITE, 0);

			lv_obj_set_style_bg_color(bar_output, COLOR_WHITE, 0);
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
        lv_obj_set_style_border_color(box, COLOR_BLACK, 0);
	}else{
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
		void autoScale(const lv_chart_axis_t axis, const float inc_sp);
		static void draw_lbl_cb(lv_event_t* e);

		lv_obj_t *box;
	    lv_obj_t *chart;
		lv_chart_series_t *ser1, *ser2;
        bool redraw = true;
};

GraphPanel::GraphPanel(lv_obj_t* parent)
{
	/*Create a chart*/	
	box = lv_obj_create(parent);
	lv_obj_set_style_border_width(box, 0, 0);
	lv_obj_set_style_pad_all(box, 0, 0);
	lv_obj_set_size(box, DISPLAY_WIDTH, DISPLAY_HEIGHT - 78);

    chart = lv_chart_create(box);
    lv_obj_set_size(chart, DISPLAY_WIDTH - 55, DISPLAY_HEIGHT - 80 - 20);
	lv_obj_align(chart, LV_ALIGN_TOP_MID, 0, 0);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_div_line_count(chart, GRAPH_YDIVS, GRAPH_XDIVS);
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);

    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 1, 0, GRAPH_XTICKS, 1, true, 20);
	lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT );

	// Primary = Temp, secondary = Rh
	// setScaleY(LV_CHART_AXIS_PRIMARY_Y, -10*GRAPH_MULTIPLIER, 30*GRAPH_MULTIPLIER);
	// setScaleY(LV_CHART_AXIS_SECONDARY_Y, 0*GRAPH_MULTIPLIER, 100*GRAPH_MULTIPLIER);
	// void lv_chart_set_axis_tick(obj, axis, 						major_len, minor_len, major_cnt, minor_cnt, label_en, draw_size)
    lv_chart_set_axis_tick(		chart,  LV_CHART_AXIS_PRIMARY_Y, 	1,         0,         3,         1,         true,     40);
    lv_chart_set_axis_tick(		chart,  LV_CHART_AXIS_SECONDARY_Y, 	1,         0,         3,         1,         true,     40);

    /*Add two data series*/
	lv_chart_set_point_count(chart, GRAPH_POINTS);
    ser1 = lv_chart_add_series(chart, COLOR_RED, LV_CHART_AXIS_PRIMARY_Y);
    ser2 = lv_chart_add_series(chart, COLOR_BLUE, LV_CHART_AXIS_SECONDARY_Y);
	
	lv_obj_add_event_cb(chart, draw_lbl_cb, LV_EVENT_DRAW_PART_BEGIN, this);
};

void GraphPanel::draw_lbl_cb(lv_event_t* e)
{
	lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) 
		return;

    // if no target buffer, forget about it
    if(!dsc->text)
    {
        WARNING("No target buffer for tick-label?");
        return;
    };


    // tmp value, don't allocate
    static int this_sec;

	// GraphPanel* me = static_cast<GraphPanel*>(e->user_data);
    switch(dsc->id)
    {
        case LV_CHART_AXIS_PRIMARY_X:
            this_sec = GRAPH_SEC_PER_DEV * (GRAPH_XTICKS-1-dsc->value);
            lv_snprintf(dsc->text, dsc->text_length, "-%d:%02d", this_sec / 3600, (this_sec % 3600) / 60);
    		return;
        case LV_CHART_AXIS_PRIMARY_Y:
		    dsc->label_dsc->color = COLOR_RED;
            lv_snprintf(dsc->text, dsc->text_length, "%d", dsc->value / GRAPH_MULTIPLIER);
            return;
	    case LV_CHART_AXIS_SECONDARY_Y:
		    dsc->label_dsc->color = COLOR_BLUE;
            lv_snprintf(dsc->text, dsc->text_length, "%d", dsc->value / GRAPH_MULTIPLIER);
            return;
        default:
            return;  
    };
};

void GraphPanel::appendVals(const float val1, const float val2)
{
	lv_chart_set_next_value(chart, ser1, isnan(val1) ? LV_CHART_POINT_NONE : val1*GRAPH_MULTIPLIER);
	lv_chart_set_next_value(chart, ser2, isnan(val2) ? LV_CHART_POINT_NONE : val2*GRAPH_MULTIPLIER);

	autoScale(LV_CHART_AXIS_PRIMARY_Y, settings.pid1.fpid.setpoint);
	autoScale(LV_CHART_AXIS_SECONDARY_Y, settings.pid2.fpid.setpoint);
};

void GraphPanel::setScaleY(const lv_chart_axis_t axis, const float miny, const float maxy)
{
	// DBG("axis: %d min/max: %f %f", axis, miny, maxy);

	lv_chart_set_range(chart, axis, miny*GRAPH_MULTIPLIER, maxy*GRAPH_MULTIPLIER);
	// void lv_chart_set_axis_tick(obj, axis, major_len, minor_len, major_cnt, minor_cnt, label_en, draw_size)
    lv_chart_set_axis_tick(		chart,  axis, 1,         0,         3,         1,         true,     20);
    // redraw = true;
};

void GraphPanel::autoScale(const lv_chart_axis_t axis, const float inc_sp)
{
	float minyM = inc_sp * GRAPH_MULTIPLIER;
	float maxyM = inc_sp * GRAPH_MULTIPLIER;

	// note: instead of dividing all the points, we multiply the others with GRAPH_MULTIPLIER and then divide once

	// find min/max
	int cnt = lv_chart_get_point_count(chart);
	lv_chart_series_t *ser = axis == LV_CHART_AXIS_PRIMARY_Y ? ser1 : ser2;
	while(cnt--)
	{
        if(ser->y_points[cnt] == LV_CHART_POINT_NONE)
            continue;

        float point = static_cast<float>(ser->y_points[cnt]);

		minyM = min(minyM, point);
		maxyM = max(maxyM, point);
	};
	minyM = floor(minyM/(GRAPH_SCALE_ROUND*GRAPH_MULTIPLIER)) *(GRAPH_SCALE_ROUND*GRAPH_MULTIPLIER);
	maxyM = ceil(maxyM/(GRAPH_SCALE_ROUND*GRAPH_MULTIPLIER)) *(GRAPH_SCALE_ROUND*GRAPH_MULTIPLIER);

	// setScaleY(axis, (minyM/GRAPH_MULTIPLIER) - GRAPH_SCALE_ROOUND, (maxyM/GRAPH_MULTIPLIER) + GRAPH_SCALE_ROOUND);
	lv_chart_set_range(chart, axis, minyM, maxyM);
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

	pw1->setSetPoint(settings.pid1.fpid.setpoint);
	pw1->setValue(input_value1);
	pw1->setBar(pid1.get_output_percent());
	if(settings.pid1.active)
	{
		pw1->setState(PidPanel::PS_OK);
	}else{
		pw1->setState(PidPanel::PS_DISABLED);
	};

	pw2->setSetPoint(settings.pid2.fpid.setpoint);
	pw2->setValue(input_value2);
	pw2->setBar(pid2.get_output_percent());
	if(settings.pid2.active)
	{
		pw2->setState(PidPanel::PS_OK);
	}else{
		pw2->setState(PidPanel::PS_DISABLED);
	};

	if(now < _next_chart)
		return true;

	gw->appendVals(input_value1, input_value2);

	_next_chart = now + GRAPH_DELTA_MS;
    
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
